#include <reg51.h>

sbit rs = P2^0;
sbit en = P2^1;
sbit led = P3^6;

#define dat P1

unsigned char rx_data[10];
unsigned int i = 0;
unsigned flag = 0;

void delay(unsigned int ms);
void lcd_cmd(unsigned char s);
void lcd_data(unsigned char s);
void lcd_str(unsigned char *s);
void lcd_init();
void serial_init();
void sendser_char(unsigned char c);
void clear();

void serial_int(void) interrupt 4
{
	while(RI == 0);
	RI = 0;
	rx_data[i++] = SBUF;
	flag = 1;
}

void main()
{
	lcd_init();
	serial_init();

	while(1)
	{
		lcd_cmd(0x80);
		lcd_str("Waiting data...");
		while(flag == 0); // wait for UART data

		delay(100);
		lcd_cmd(0x01);
		lcd_cmd(0x80);
		lcd_str("Data Recvd:");
		lcd_cmd(0xC0);
		lcd_str(rx_data);

		if((rx_data[0] == 'O') && (rx_data[1] == 'N' || rx_data[1] == 'F'))
		{
			sendser_char('Y'); // ACK
			
			if (rx_data[1] == 'N')
				led = 1;
			else
				led = 0;
		}
		
		

		delay(500);
		clear();
		lcd_cmd(0x01);
		flag = 0;
	}
}

void delay(unsigned int ms)
{
	unsigned int i, j;
	for(i = 0; i < ms; i++)
		for(j = 0; j < 1275; j++);
}

void lcd_cmd(unsigned char s)
{
	rs = 0;
	dat = s;
	en = 1;
	delay(5);
	en = 0;
	delay(5);
}

void lcd_data(unsigned char s)
{
	rs = 1;
	dat = s;
	en = 1;
	delay(5);
	en = 0;
	delay(5);
}

void lcd_str(unsigned char *s)
{
	while(*s)
		lcd_data(*s++);
}

void lcd_init()
{
	lcd_cmd(0x38);
	lcd_cmd(0x0C);
	lcd_cmd(0x01);
	lcd_cmd(0x06);
	lcd_cmd(0x80);
	delay(2);
}

void serial_init()
{
	TMOD = 0x20;  // Timer1 mode2
	TH1 = 0xFD;   // Baud rate 9600
	SCON = 0x50;  // 8-bit, REN enabled
	IE = 0x90;    // Enable Serial interrupt
	TR1 = 1;
}

void sendser_char(unsigned char c)
{
	SBUF = c;
	while(TI == 0);
	TI = 0;
}

void clear()
{
	unsigned int j;
	i = 0;
	for(j = 0; j < 10; j++)
		rx_data[j] = '\0';
}
