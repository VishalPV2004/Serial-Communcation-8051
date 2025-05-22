#include <reg51.h>


void delay(unsigned int ms);
void lcd_cmd(unsigned char s);
void lcd_data(unsigned char s);
void lcd_str(unsigned char *s);
void lcd_init();
void sendser_str(unsigned char *s);
void sendser_char(unsigned char c);
void clear();

bit prev_state = 2;

sbit rs = P2^0;
sbit en = P2^1;


sbit in = P3^7;
#define dat P1

unsigned char a[10];
unsigned int i;
unsigned rx_flag;

void ser_int(void) interrupt 4
{
	if(RI == 1)
	{
	  RI = 0;
	  a[i] = SBUF;
	  rx_flag = 1;
	  i++;
	}
	
	
	if (TI == 1)
	{
		TI = 0;
	}
}

void main() 
{
	  in = 0;
    lcd_init();
    TMOD = 0x20;
    TH1 = 0xFD;
    SCON = 0x50;
    IE = 0x90;
    TR1 = 1;

    lcd_cmd(0x80);
    lcd_str("VeeConn systems");
    delay(100);
	  lcd_cmd(0x01);
	  lcd_cmd(0x80);
	  lcd_str("UART Comm!");
	  lcd_cmd(0xC0);
	  lcd_str("Data sent:");
    
    while(1) {
	       lcd_cmd(0x80);
	       lcd_str("UART Comm!");
	       lcd_cmd(0xC0);
	       lcd_str("Data sent:");
			delay(100);
			
			if (in != prev_state)
		  {
				prev_state = in;
			 if (in == 0)
			 {

				 lcd_cmd(0xCA);
				 lcd_str("off");
				 sendser_str("off");
				 sendser_char(0x0D);
				 delay(400);

			 }
			 else if (in == 1)
			 {
				 lcd_cmd(0xCA);
				 lcd_str("on");
				 sendser_str("on");
				 sendser_char(0x0D);
         delay(400);
			 }
			 lcd_cmd(0XCA);
			 lcd_str("      ");
			 
			 lcd_cmd(0x01);
			 lcd_str("Waiting ACK..");
			 while(rx_flag == 0);
			 delay(700);
			 
			 if (a[0] == 'A')
			 {
				 lcd_cmd(0x01);
				 lcd_cmd(0x80);
				 lcd_str("message sent");
				 delay(30);
				 lcd_cmd(0x01);
			 }
			 else
			 {
				 lcd_cmd(0x01);
				 lcd_cmd(0x80);
				 lcd_str("!message sent");
				 delay(30);
				 lcd_cmd(0x01);
			 }
			 
			 clear();
			 rx_flag = 0;
			 delay(50);
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
	unsigned int j = 0;
	for(j = 0 ; j < 10 ; j++)
	{
		a[j] = 0;
	}
	i = 0;
}

void sendser_char(unsigned char c)
{
	SBUF = c;
	while(TI == 0);
	TI = 0;
}

void sendser_str(unsigned char *c)
{
	while(*c)
	{
		sendser_char(*c++);
		delay(2);
	}
}

void lcd_init()
{
    lcd_cmd(0x38);  // 2 lines, 5x7 matrix
    lcd_cmd(0x0C);  // Display ON, cursor OFF
    lcd_cmd(0x01);  // Clear display
    lcd_cmd(0x06);  // Entry mode
    lcd_cmd(0x80);  // Cursor to beginning
    delay(2);
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
    {
        lcd_data(*s++);
    }
}
