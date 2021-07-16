#define F_CPU 16000000UL
#define USART_BAUDRATE 9600 
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void Usart_init(void);
unsigned char Usart_receive(void);
void Usart_send(unsigned char value);
void send_string(char *s);
void command(unsigned char val);

void Usart_init(void);
unsigned char Usart_receive(void);
void Usart_send(unsigned char value);
void send_string(char *s);
float black[] = {175,102,193};
float white[] = {492,495,608};
float color[] = {0, 0, 0};
int clname[3] = {1, 2, 3};
float Final_ADC[] = {0, 0, 0};
char send[];
int ADC_value;
static volatile int pulse = 0;
static volatile int i = 0;
int16_t count_a = 0; 
char show_a[16];
int main(void){
	Usart_init();
	init();
	_delay_ms(10000);
	while(1){
		set_PWM(0,0,163,150);//forward
		for(int i=0;i<17;i++){
			detectClr();
			obstacle();
		}
		set_PWM(0,0,0,0);
		_delay_ms(250);
		
		set_PWM(150,0,0,150);//left turn
		_delay_ms(815);
		set_PWM(0,0,0,0);
		_delay_ms(250);
		set_PWM(0,0,163,150);//forward
		for(int i=0;i<12;i++){
			detectClr();
			obstacle();
		}
		set_PWM(0,0,0,0);
		_delay_ms(250);
		
		set_PWM(0,150,150,0);//right turn
		_delay_ms(820);
		set_PWM(0,0,0,0);
		_delay_ms(250);
		set_PWM(0,0,163,150);//forward
		for(int i=0;i<14;i++){
			detectClr();
			obstacle();
		}
		set_PWM(0,0,0,0);
		_delay_ms(250);
		
		set_PWM(0,157,150,0);//right turn
		_delay_ms(830);
		set_PWM(0,0,0,0);
		_delay_ms(250);
		set_PWM(0,0,163,150);//forward
		for(int i=0;i<16;i++){
			detectClr();
			obstacle();
		}
		set_PWM(0,150,150,0);
		_delay_ms(30);
		set_PWM(0,0,163,150);//forward
		for(int i=0;i<16;i++){
			detectClr();
			obstacle();
		}
		set_PWM(0,0,0,0);
		_delay_ms(250);
		
		set_PWM(0,150,150,0);//right turn
		_delay_ms(830);
		set_PWM(0,0,0,0);
		_delay_ms(250);
		set_PWM(0,0,163,150);//forward
		for(int i=0;i<14;i++){
			detectClr();
			obstacle();
		}
		set_PWM(0,0,0,0);
		_delay_ms(250);
		
		set_PWM(0,150,150,0);//right turn
		_delay_ms(830);
		set_PWM(0,0,0,0);
		_delay_ms(250);
		set_PWM(0,0,165,150);//forward
		for(int i=0;i<11;i++){
			detectClr();
			obstacle();
		}
		set_PWM(0,0,0,0);
		_delay_ms(250);
		
		set_PWM(150,0,0,150);//left turn
		_delay_ms(850);
		set_PWM(0,0,0,0);
		_delay_ms(250);
		set_PWM(0,0,161,150);//forward
		for(int i=0;i<10;i++){
			detectClr();
			obstacle();
		}
		set_PWM(0,0,0,0);
		break;
	}
}
void detectClr(void){
	PORTC=1<<PC3;//red
	_delay_ms(40);
	color[0] = get_Average();
	PORTC=0<<PC3;
	_delay_ms(10);

	PORTC=1<<PC4;//green
	_delay_ms(40);
	color[1] =get_Average();
	PORTC=0<<PC4;
	_delay_ms(10);
	
	PORTC=1<<PC5;//blue
	_delay_ms(40);
	color[2] = get_Average();
	PORTC=0<<PC5;
	_delay_ms(10);
	
	Set_Range();
}
int Set_Range(void){
	float cb = 0;
	float wb = 0;
	float Set_Range = 0;
	for(int i=0;i<3;i++){
		cb= (color[i] - black[i]);
		wb = (white[i] - black[i]);
		Final_ADC[i] = (cb/wb)*1000;
		//sprintf(send,"%d = %d\n", clname[i], (int)Final_ADC[i]);
		//send_string(send);//Send msg
	}
	float blackSum = Final_ADC[0] + Final_ADC[1] + Final_ADC[2];
	float whiteSum = Final_ADC[0] + Final_ADC[1] + Final_ADC[2];
	float greenBlueGreen = Final_ADC[2] - Final_ADC[1];
	if((Final_ADC[0]<Final_ADC[1]) && (Final_ADC[2]<Final_ADC[1]) && (blackSum>=200) && (blackSum<=2700)){
		//send_string("-------------red-------------------\n");
		PORTB = 1<<PB3|0<<PB4|0<<PB5;
	}
	if((Final_ADC[1]<Final_ADC[0]) && (Final_ADC[1]<Final_ADC[2]) && (blackSum>=200) && (blackSum<=2700) && (Final_ADC[1]<600)){
		//send_string("-------------green-------------------\n");
		PORTB = 0<<PB3|1<<PB4|0<<PB5;
	}
	if((Final_ADC[1]<Final_ADC[0]) && (Final_ADC[1]<Final_ADC[2]) && (blackSum>=200) && (whiteSum<=2700) && (Final_ADC[1]>600)){
		//send_string("-------------blue-------------------\n");
		PORTB = 0<<PB3|0<<PB4|1<<PB5;
	}
}
	
int get_Average(void) {
	int sum = 0;
	ADC_value=0;
    for(int j=1;j<=20;j++){	
		ADC_value = Read_ADC();
		sum +=ADC_value;
	}
    return (sum/20);
}
int Read_ADC(void){											//single conversion mode//ADMUX = (ADMUX & 0xF0) | (ADCchannel & 0x0F);
	ADCSRA = ADCSRA|(1<<ADSC); //start convertion
	while((ADCSRA & (1<<ADSC))==0);	// wait until ADC conversion is complete
	return ADC;
}

void init(void){
	DDRC=1<<PC3|1<<PC4|1<<PC5;//For Tri_color Led	
	ADCSRA =1<<ADEN|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);//ADC enable and set prescaler to 128
	ADMUX = 1<<REFS0|1<<MUX1;//referance voltage selection and set ADC2 pin for anlog read
	
	DDRD=1<<PD5|1<<PD6|0<<PD2|1<<PD3;//For motor and ultrsonic sensor
	DDRB=1<<PB1|1<<PB2|1<<PB3|1<<PB4|1<<PB5;//For motor and Display LED
	_delay_ms(50);

	EIMSK |= 1<<INT0;//External Interrupt Request 0 Enable
	EICRA |= 1<<ISC00;//Any logical change on INT0 generates an interrupt request.

	sei();
}
void set_PWM(unsigned char A0,unsigned char A1,unsigned char B0,unsigned char B1){
	TCCR0A=1<<COM0A1|1<<COM0B1|1<<WGM01|1<<WGM00;//OC0A,OC0B on compare match, set OC0A,OC0B at BOTTOM And Fast PWM
	TCCR0B=1<<CS00;//no prescaler
	
	TCCR1A=1<<COM1A1|1<<COM1B1|1<<WGM12|1<<WGM10;//OC1A,OC1B on compare match, set OC1A,OC1B at BOTTOM And Fast PWM, 8-bit
	TCCR1B=1<<CS10;//no prescaler
	
	OCR0A=A0;
	OCR1A=A1;
	OCR0B=B0;
	OCR1B=B1;

}
void Usart_init(void){
	UCSR0B |= (1<<TXEN0)|(1<<RXEN0);//Enable Transmit, Receive
	UCSR0C |= (1<<UCSZ01)|(1<<UCSZ00);//Select UCSRC, Select transmit and receive bit-size - 8bit
	UBRR0H = (BAUD_PRESCALE >> 8);
	UBRR0L = BAUD_PRESCALE;//Set baudrate 9600
}

unsigned char Usart_receive(void){
	while(!(UCSR0A&(1<<RXC0)));//Wait for byte receive
	return(UDR0);//UDR store received msg
}

void Usart_send(unsigned char value){
	while(!(UCSR0A&(1<<UDRE0)));//Wait for UDR empty
	UDR0 = value;//Load message UDR to sent
	while(!(UCSR0A&(1<<TXC0)));//Wait for msg sent
}

void send_string(char *s){//send string
	for(unsigned char i=0; s[i]!=0; i++){
		Usart_send(s[i]);
	}
}
void obstacle(void){
	PORTD = 1<<PD3;
    _delay_us(15);
    PORTD = 0<<PD3;
    count_a = pulse;
	//sprintf(show_a,"Distance = %d\n", count_a);
	//send_string(show_a);//Send msg
	if(count_a<15){
		send_string("1\n");
	}else{
		send_string("0\n");
	}
}
ISR(INT0_vect){
  if(i == 0){
    TCCR2B |= 1<<CS20|1<<CS21|1<<CS22;//set prescaler to 1024
    i = 1;
  }else{
    TCCR2B = 0;
    pulse = TCNT2;//Timer counter register
    TCNT2 = 0;
    i = 0;
  }
}