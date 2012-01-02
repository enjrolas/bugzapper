#include <avr/io.h>
#include <avr/eeprom.h>

char reading;
char lastReading=0;
unsigned char ones, tens, hundreds, thousands;
unsigned int count;
unsigned long debounce=0;

#define DEBOUNCE_THRESHHOLD 1000

#define DEBOUNCE_MAX 2000

#define ONES 0
#define TENS 1
#define HUNDREDS 2
#define THOUSANDS 3
#define OFF 4

unsigned char digit[10]={
0b10000000,  //digit '0'
0b10011011,  //digit '1'
0b01001000,  //digit '2'
0b00001010,  //digit '3'
0b00010011,  //digit '4'
0b00100010,  //digit '5'
0b00100000,  //digit '6'
0b10001011,  //digit '7'
0b00000000,  //digit '8'
0b00000010  //digit '9'
};

unsigned char digitSelector[5]={
0b00001110,
0b00001101,
0b00001011,
0b00000111,
0b00001111};


//upside-down digits
/*
unsigned char digit[10]={
0b10000000,  //digit '0'
0b11110001,  //digit '1'
0b01001000,  //digit '2'
0b01100000,  //digit '3'
0b00110001,  //digit '4'
0b00100010,  //digit '5'
0b00000010,  //digit '6'
0b11110000,  //digit '7'
0b00000000,  //digit '8'
0b00110000  //digit '9'
};

unsigned char digitSelector[5]={
0b00000111,
0b00001011,
0b00001101,
0b00001110,
0b00001111};

*/	

void calculateDigits()
{
	long temp=count;
	thousands=temp/1000;
	temp-=thousands*1000;
	hundreds=temp/100;
	temp-=hundreds*100;
	tens=temp/10;
	temp-=tens*10;
	ones=temp;
}


void delay(long ms)
{
	long i;
	char a;
	ms*=10;
	for(i=0;i<ms;i++){a++;}
}


void display()
{

	PORTD=digit[ones];
	PORTC=digitSelector[ONES];

	delay(5);

	PORTD=digit[tens];
	PORTC=digitSelector[TENS];
	delay(5);

	PORTD=digit[hundreds];
	PORTC=digitSelector[HUNDREDS];
	delay(5);

	PORTD=digit[thousands];
	PORTC=digitSelector[THOUSANDS];
	delay(5);


}

int main(void)
{
//  eeprom_write_word((uint16_t *) 0,0);
  count=eeprom_read_word((uint16_t *)0);
// 	count=9999;
  ones=0;
  tens=0;
  hundreds=0;
  thousands=0;

  DDRD = ~0x04;
  DDRB=0x00;

  DDRC=0xFF;

  while(1)
  {
  	reading=PIND&0x04;

  	calculateDigits();
  	display();
	if((lastReading==0x04)&&(reading==0))//we want to see a falling voltage on the grid
		if((PINB&0x01)==0)  //make sure the button's pressed
			if(debounce>DEBOUNCE_THRESHHOLD)
			{
				count++;
				debounce=0;
				eeprom_write_word(0,count);
			}
    lastReading=reading;
	if(debounce<DEBOUNCE_MAX)
		debounce++;
	else
		debounce=DEBOUNCE_MAX;
  }

  return 1;
}


