#include <reg51.h>

// Function Declarations
void delay(unsigned int ms);
void lcd_cmd(unsigned char s);
void lcd_data(unsigned char s);
void lcd_str(unsigned char *s);
void lcd_init();
void sendser_str(unsigned char *c);
void sendser_char(unsigned char c);
void serial_init();
void clear();

// Pin Configuration
sbit input = P0^1;
sbit rs = P2^0;
sbit en = P2^1;
#define dat P1
unsigned int j;

// Global Variables
unsigned char arr[10];
unsigned int i = 0;
unsigned char flag = 0;

// UART Interrupt Service Routine
void serial_int(void) interrupt 4
{
	  while(RI == 0);
    RI = 0;                    // Clear receive interrupt flag
    arr[i++] = SBUF;          // Store received data
    flag = 1;                 // Set flag to indicate data received
}

// Main Program
void main()
{
    lcd_init();
    serial_init();
	
  loop:
    while(1)
    {
        lcd_cmd(0x01);
        lcd_str("UART COMM!");
        lcd_cmd(0xC0);

        // Read and send input status
        if (input == 0)
        {
            lcd_str("Data sent: OFF");
            sendser_str("OFF");
            delay(100);
        }
        else
        {
            lcd_str("Data sent: ON");
            sendser_str("ON");
            delay(100);
        }

        // Wait for acknowledgment
        lcd_cmd(0x01);
        lcd_cmd(0x80);
        lcd_str("Waiting for");
        lcd_cmd(0xC0);
        lcd_str("Acknowledgement");

        while(flag == 0);  // Wait for interrupt

        delay(500);

        // Check ACK
        if (i >= 1 && arr[0] == 'Y')
        {
            lcd_cmd(0x01);
            lcd_cmd(0x80);
            lcd_str("Delivered");
            lcd_cmd(0xC0);
            lcd_str("Successfully");
        }
        else
        {
            lcd_cmd(0x01);
            lcd_cmd(0x80);
            lcd_str("Not Delivered");
            lcd_cmd(0xC0);
            lcd_str("Failed");
        }

        delay(300);
        clear();      // Reset buffer
        flag = 0;     // Reset flag
        delay(100);
				lcd_cmd(0x01);
    }
		goto loop;
}

// Delay function
void delay(unsigned int ms)
{
    unsigned int i, j;
    for(i = 0; i < ms; i++)
        for(j = 0; j < 1275; j++);
}

// LCD Initialization
void lcd_init()
{
    lcd_cmd(0x38);  // 2 lines, 5x7 matrix
    lcd_cmd(0x0C);  // Display ON, cursor OFF
    lcd_cmd(0x01);  // Clear display
    lcd_cmd(0x06);  // Auto increment cursor
    lcd_cmd(0x80);  // Cursor at beginning
    delay(2);
}

// Send command to LCD
void lcd_cmd(unsigned char s)
{
    rs = 0;
    dat = s;
    en = 1;
    delay(5);
    en = 0;
    delay(5);
}

// Send data to LCD
void lcd_data(unsigned char s)
{
    rs = 1;
    dat = s;
    en = 1;
    delay(5);
    en = 0;
    delay(5);
}

// Send string to LCD
void lcd_str(unsigned char *s)
{
    while(*s)
    {
        lcd_data(*s++);
    }
}

// UART Initialization
void serial_init()
{
    TMOD = 0x20;   // Timer1, Mode2, 8-bit auto-reload
    TH1 = 0xFD;    // 9600 baud @ 11.0592 MHz
    SCON = 0x50;   // Mode1, 8-bit UART, Enable receiver
    IE = 0x90;     // Enable serial interrupt
    TR1 = 1;       // Start Timer1
}

// Send single char via UART
void sendser_char(unsigned char c)
{
    SBUF = c;
    while(TI == 0);
    TI = 0;
}

// Send string via UART
void sendser_str(unsigned char *c)
{
    while(*c)
    {
        sendser_char(*c++);
        delay(2);
    }
}

// Clear buffer and index
void clear()
{
    i = 0;
    for(j = 0; j < 10; j++)
        arr[j] = '\0';
}
