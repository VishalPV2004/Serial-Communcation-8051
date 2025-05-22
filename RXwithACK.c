#include <reg51.h>

void delay(unsigned int ms);
void lcd_cmd(unsigned char s);
void lcd_data(unsigned char s);
void lcd_str(unsigned char *s);
void lcd_init();
void sendser_str(unsigned char *s);
void sendser_char(unsigned char c);
void clear();

sbit rs = P2^1;
sbit en = P2^0;
sbit l1 = P3^6;
#define dat P1

unsigned char a[10];
unsigned int i = 0;
unsigned rx_flag = 0;

void ser_int(void) interrupt 4
{
	if (RI)
	{
		RI = 0;
		a[i++] = SBUF;
		rx_flag = 1;
	}
	if (TI)
	{
		TI = 0;
	}
}

void main() 
{
    lcd_init();
    TMOD = 0x20;      // Timer1 in Mode 2
    TH1 = 0xFD;       // 9600 baud
    SCON = 0x50;      // 8-bit UART, REN enabled
    IE = 0x90;        // Enable serial interrupt
    TR1 = 1;          // Start Timer1
   
	  lcd_cmd(0x01);
    lcd_cmd(0x80);
    lcd_str("VeeConn Systems");
    delay(300);
    lcd_cmd(0x01);

    while(1) 
    {
        if (rx_flag)
        {
            delay(500);  // Let buffer fill up before processing

            // Simple check for 'on' or 'off'
            if (a[0] == 'o')
            {
							  delay(4000);
                sendser_char('A');
                delay(100);

                if (a[0] == 'o' && a[1] == 'n')
                {
                    lcd_cmd(0x01);
                    lcd_cmd(0x80);
                    lcd_str("Status: ON");
                    l1 = 1;
                }
                else if (a[0] == 'o' && a[1] == 'f' && a[2] == 'f')
                {
                    lcd_cmd(0x01);
                    lcd_cmd(0x80);
                    lcd_str("Status: OFF");
                    l1 = 0;
                }
            }

            clear();         // Reset buffer
            rx_flag = 0;
        }
    }
}

void delay(unsigned int ms)
{
    unsigned int i, j;
    for(i = 0; i < ms; i++)
        for(j = 0; j < 1275; j++);
}

void clear()
{
    unsigned int j;
    for(j = 0; j < 10; j++)
    {
        a[j] = 0;
    }
    i = 0;
}

void sendser_char(unsigned char c)
{
    SBUF = c;
    while (!TI);
    TI = 0;
}

void sendser_str(unsigned char *c)
{
    while(*c)
    {
        sendser_char(*c++);
        delay(2);  // short gap between characters
    }
}

void lcd_init()
{
    lcd_cmd(0x38);  // 2-line, 5x7 matrix
    lcd_cmd(0x0C);  // Display ON, cursor OFF
    lcd_cmd(0x01);  // Clear display
    lcd_cmd(0x06);  // Entry mode
    lcd_cmd(0x80);  // Set cursor to beginning
    delay(2);
}

void lcd_cmd(unsigned char s)
{
    rs = 0;
    dat = s;
    en = 1;
    delay(2);
    en = 0;
    delay(2);
}

void lcd_data(unsigned char s)
{
    rs = 1;
    dat = s;
    en = 1;
    delay(2);
    en = 0;
    delay(2);
}

void lcd_str(unsigned char *s)
{
    while(*s)
    {
        lcd_data(*s++);
    }
}
