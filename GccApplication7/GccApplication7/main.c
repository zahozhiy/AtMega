/*
 * GccApplication7.c
 *
 * Created: 29.10.2019 15:00:00
 * Author : zahozhiy
 */ 



 #define F_CPU 16000000UL
 #include <avr/io.h>   
 //#include <avr/iom32.h>     
 #include <util/delay.h>  
 #include <stdio.h>
 #include <stdlib.h>
 #include "uart.h"
 #include <avr/interrupt.h>
 #include <avr/eeprom.h>
 #include <avr/wdt.h>
 #include "types.h"
 #include "main.h"
 
 
 extern void twi_init (void);
 extern int twi_read_bytes(u8, int, u8 *);
 extern int twi_write_bytes(u8, u8, int, u8 *);
 const TWI_SLA = 0x10;;
 
 volatile u8 my_flag;								//11.10.2019
 volatile u8 i2c_done=0;
 volatile uint8_t mb EEMEM;

 u8 mb2=0x10;
  u8 mb3=0x0;
    u8 mb4=0x84;   //6 mA 
	   // u8 mb4=0x04;      //3 mA
	
	u8 mb5=0;
	u8 mb6=0;



u8 addr_board=0;
	 
 u8 mc;
 const uint8_t *p_mb;
  u8 *p_mb2;
    u8 *p_mb3;
	    u8 *p_mb4;
		
		u8 *p_mb5;
		u8 *p_mb6;
		
 u8 *p_mc;
 int i;
 uint16_t time_intgr=0x100;
 u8 ii;
 u8 ink;
 volatile u8 abba[16];
 volatile u8 byte_count=0;
 volatile u8 NUM;
 
 unsigned char bit = 0;
 unsigned char writebuff;
 unsigned char rdDat;
 
 uint32_t sum=0;
// uint16_t checksum=0;
 
 u8 flag_start=0;
 
 
 u8 count_x = 0;
 uint16_t count_y=1;
 
 union my_int
 {
	 uint16_t zx;
	 struct  
	 {
		u8 y22;
		u8 y23;
	 }bb;
 };
 
  
  
typedef union my_int EAX;


EAX bbs;  // nomer paketa
EAX cde;  // crc pervogo i vtorogo paketa

EAX m_offset;

EAX agp;  // crc tretjego paketa
EAX fff;  // crc udp
EAX checksum;
//EAX udp_info1[726];
EAX udp_info2[730];
EAX udp_info3[80];
 
 //--------------------------------------------------------------------------//11.07.2019
 	  
 	 // uint16_t t_dec=1000;
	   volatile uint16_t t_dec;
	  uint16_t time_dec EEMEM;
	  const uint16_t * _ptime_dec=&time_dec;
 	  uint16_t t_hex=0;
 	  u8 num_byte=0;
	   
 	  u8 info[64]={0x5B, 0x20, 0x30, 0x2C, 0x43, 0x4B, 0x48, 0x35, 0x30, 0x20, 0x47, 0x31, 0x38, 0x43, 0x30, 0x33,		// command RI
	 	  0x31, 0x30, 0x33, 0x35, 0x37, 0x2C, 0x31, 0x2C, 0x58, 0x2D, 0x44, 0x41, 0x51, 0x2C, 0x31, 0x2C,
	 	  0x36, 0x30, 0x30, 0x2C, 0x39, 0x36, 0x2C, 0x32, 0x2C, 0x31, 0x35, 0x2C, 0x31, 0x46, 0x34, 0x2C,
 	  0x31, 0x46, 0x34, 0x30, 0x30, 0x2C, 0x41, 0x2C, 0x46, 0x46, 0x46, 0x46, 0x2C, 0x34, 0x30, 0x5D};
 	  
 	  u8 sum_size=0;
 	  u8 i_port=0;
 	  //u8 max_pixel=0;
 	  u8 energy_mode=0;
 	  u8 channel=0;
 	  u8 card_number[4];
 	  u8 avg_filter_size=0;
 	  u8 t=0;
	  
	  static u8 gain=0;
	  u8 test=0; 
	  u8 ch_addr=0;
	  static u8 g_even=0;
	  static u8 g_odd=0;
	  u8 g_addr=0;
	  
	  u8 gAin EEMEM;
	  const u8 *_pgAin=&gAin;
	   
	   
	 
	u8 Converter(u8 az )
	{
		u8 bz;
		switch(az)
		{
			case'0': bz=0; break;
			case'1': bz=1; break;
			case'2': bz=2; break;
			case'3': bz=3; break;
			case'4': bz=4; break;
			case'5': bz=5; break;
			case'6': bz=6; break;
			case'7': bz=7; break;
			case'8': bz=8; break;
			case'9': bz=9; break;
			case('A'): bz=0xa; break;
			case('B'): bz=0xb; break;
			case('C'): bz=0xc; break;
			case('D'): bz=0xd; break;
			case('E'): bz=0xe; break;
			case('F'): bz=0xf; break;
			case('a'): bz=0xa; break;
			case('b'): bz=0xb; break;
			case('c'): bz=0xc; break;
			case('d'): bz=0xd; break;
			case('e'): bz=0xe; break;
			case('f'): bz=0xf; break;
		};		
		return bz;
	}
						  
	
 
 



	void SetTim(uint16_t abba)
	{
		wdt_reset();
		for(int i=0; i<16; i++)
		{
			uint16_t b=((abba<<i)&(0x8000));
			if(b==0x8000)
			{
			 PORTA|=(1<<PORTA1);
			}
			else if(b==0)
			{
			 PORTA&=~(1<<PORTA1);
			}
			_delay_us(5);
			PORTA |= (1<<PORTA0);
			PORTA &= ~(1<<PORTA0);			
		}
		_delay_us(5);
		PORTA |= (1<<PORTA2);//дл€ перезаписи в параллельный регистр
		PORTA &= ~(1<<PORTA2);
	}




 void SerilalIni(uint32_t Bps){

 //uint32_t UBRR_Const;
 uint8_t HiSpeed=0;
 //UBRR_Const=(F_CPU/(Bps<<2));
 UCSRB  = 0x00;
 // UBRRH  = UBRR_Const>>8;                    // Speed setup H
 UBRRH=0;
 // UBRRL  = UBRR_Const&0xFF;                  // Speed setup L
 //UBRRL = 0x67;	//9600
 UBRRL = 0x08;	//115200	
 UCSRA  = (HiSpeed<<U2X);                   // UCSRA setup
 UCSRB = (1<<RXCIE)|(1<<RXEN)|(1<<TXEN);   // UCSRB setup (Tx and Rx enable) interrupt RX enable
// UCSRB = (1<<RXEN)|(1<<TXEN);
 UCSRC  = (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);
 //UCSRC |= (1<<USBS);		   
 }
 
 
 
 ISR(USART_RXC_vect)
 {
	 wdt_reset();
	 u8 tmp;
	 // my_flag = 1;
	 tmp=byte_count;
	 abba[tmp]=UDR;
	 if(abba[0]=='[')
	 {
		 byte_count++;
		 if(abba[tmp]==']')
		 {
			 my_flag = 1;
			 num_byte=byte_count;
			 byte_count=0;
		 }
	 }
	 else
	 {
		 byte_count=0;
	 }
 }
 
 
 void Init_Main(void)
 {
	 //////////////Ustanovka PORT A
	 
	 //PORTA - Ustanavlivaet zaderjku
	 DDRA = 0xFF;
	 PORTA |= (1<<PORTA3); //<=High PA3
	 DDRD = (1<<PORTD3);
	 //PORTA = 0x01;// esli PORTA==0, to sistema rabotat' ne budet
	 PORTD &= ~(1<<PORTD3); // STOP TOTAL
	 
	 ////////////////////////////////////Ustanovka PORT B (Upravlenie PLIC)
	 //PORTB0 - PUSK
	 //PORTB1 - Mladshii bit koefficienta peredachi ADC
	 //PORTB2 - bit koefficienta peredachi ADC
	 //PORTB3 - bit koefficienta peredachi ADC
	 //PORTB4 - RESET PLIC
	 //PORTB5 - sinhronizacia (in or out)
	 //PORTB6 - TEST MODE or WORK MODE of CU
	 //PORTB7 - TEST MODE or WORK MODE of BD
	 DDRB = 0xFF;
	 PORTB |= (1<<PORTB0)|(1<<PORTB1)|(1<<PORTB4)|(1<<PORTB5); // USTANOVKA
	 //PORTB &= ~(1<<PORTB0);
	 PORTB &= ~(1<<PORTB2);
	 PORTB &= ~(1<<PORTB3);
	 PORTB &= ~(1<<PORTB6);
	 PORTB &= ~(1<<PORTB7);
	 _delay_us(100);
	 ////////////////////////////////////////////////////
	 
	 /////////////////////// Impuls RESET PLIC
	 PORTB &= ~(1<<PORTB4);
	 _delay_us(10);
	 PORTB |= (1<<PORTB4);
	 /////////////////////////////////////////////////////
	 
	 
	 //////////////////////////////////// Ustanovka PORT C. Peredacha dannih mejdu DM9000 i MK.
	 ///Read&Write registrov DM9000
	 DDRC = 0xFF;
	 
	 
	 //////////////////////////////////// Ustanovka PORT D (Upravlenie DM9000)
	 
	 //GICR = (1<<INT1);
	 //MCUCR = (1<<ISC10)|(1<<ISC11);
	 
	 //DDRD |= (1<<PORTD7)|(1<<PORTD6)|(1<<PORTD5)|(1<<PORTD4)|(1<<PORTD2)|(1<<PORTD3);
	 DDRD |= (1<<PORTD7)|(1<<PORTD6)|(1<<PORTD5)|(1<<PORTD4)|(1<<PORTD2);

	 PORTD |= (1<<PORTD7); // AVR_Din (0 - BD ; 1 - AVR)
	 PORTD |= (1<<PORTD6); // A_Din (0 - addr ; 1- data)
	 PORTD |= (1<<PORTD2); // Write_Read (0 - write ; 1 - read)
	 PORTD &= ~(1<<PORTD3); // STOP TOTAL
	 PORTD |= (1<<PORTD4); // IORin
	 PORTD |= (1<<PORTD5); // IOWin
	 _delay_us(100);
 }
 
 
 
 /////////////////////////////////////////////////////////Read Registers DM9000
 ///////////////// reg - Adress registra
 unsigned char ReadDM9000(unsigned char reg)
 {

	 DDRC = 0xFF;     // port C out
	 _delay_us(100);
	 PORTD &= ~(1<<PORTD2);   // Write_Read
	 _delay_us(100);
	 PORTD &= ~(1<<PORTD6);   // enable address port  A_Din
	 _delay_us(100);
	 PORTC = reg;     // address
	 _delay_us(100);
	 PORTD &= ~(1<<PORTD5);    //IOWin
	 _delay_us(100);
	 PORTD |= (1<<PORTD5);    //IOWin
	 _delay_us(100);

	 DDRC = 0x00;     // port C in
	 _delay_us(100);
	 PORTD |= (1<<PORTD2); // Write_Read
	 _delay_us(100);
	 PORTD |= (1<<PORTD6);   // enable data port    A_Din
	 _delay_us(100);
	 PORTD &= ~(1<<PORTD4);    //IORin
	 _delay_us(100);
	 rdDat= PINC;
	 _delay_us(100);
	 PORTD |= (1<<PORTD4);    //IORin
	 _delay_us(100);
	 
	 return  rdDat;
 }
 
 
 /////////////////////////////////////////////////////////Write Registers DM9000
 ///////////////// reg - Adress registra
 ///////////////// datab - zanosimoe znachenie
 void WriteDM9000(unsigned char reg, unsigned char datab)
 {

	 DDRC = 0xFF;     // port C out
	 _delay_us(100);
	 PORTD &= ~(1<<PORTD2);   // Write_Read
	 _delay_us(100);
	 PORTD &= ~(1<<PORTD6);   // enable address port  A_Din
	 PORTC = reg;     // address
	 _delay_us(100);
	 PORTD &= ~(1<<PORTD5);    //IOWin
	 _delay_us(100);
	 PORTD |= (1<<PORTD5);    //IOWin
	 _delay_us(100);

	 PORTD |= (1<<PORTD6);     // enable data port    A_Din
	 _delay_us(100);
	 PORTC = datab;     // write data
	 _delay_us(100);
	 PORTD &= ~(1<<PORTD5);    //IOWin
	 _delay_us(200);
	 PORTD |= (1<<PORTD5);    //IOWin
	 
 }
 
 
 // Inicializaciya DM9000
 void InitDM9000(void) {

	 PORTD |= (1<<PORTD7); // AVR_Din (0 - DM ; 1 - AVR)
	 
	 _delay_us(100);
	 // software reset DM9000
	 WriteDM9000(0x00, 0x01);
	 _delay_us(100);
	 WriteDM9000(0x00, 0x00);
	 _delay_us(100);
	 // WriteDM9000(0x00, 0x80);
	 // _delay_us(100);
	 
	 //active the internal PHY
	 WriteDM9000(0x1E, 0x01);
	 WriteDM9000(0x1F, 0x00);
	 
	 //Clear TX status
	 writebuff=ReadDM9000(0x01);
	 
	 // «агрузка MAC-адреса
	 
	/* 
	 WriteDM9000(0x10, 0x01);
	 WriteDM9000(0x11, 0x02);
	 WriteDM9000(0x12, 0x03);
	 WriteDM9000(0x13, 0x04);
	 WriteDM9000(0x14, 0x05);
	 WriteDM9000(0x15, 0x06);
	 */
	 
	  WriteDM9000(0x10, 0x30);
	  WriteDM9000(0x11, 0x85);
	  WriteDM9000(0x12, 0xa9);
	  WriteDM9000(0x13, 0x1b);
	  WriteDM9000(0x14, 0xc5);
	  WriteDM9000(0x15, 0x02);
	 
	 // «агрузка HASH-адреса
	 WriteDM9000(0x16, 0xFF);
	 WriteDM9000(0x17, 0xFF);
	 WriteDM9000(0x18, 0xFF);
	 WriteDM9000(0x19, 0xFF);
	 WriteDM9000(0x1A, 0xFF);
	 WriteDM9000(0x1B, 0xFF);
	 WriteDM9000(0x1C, 0xFF);
	 WriteDM9000(0x1D, 0xFF);
	 
	 //  enable the SRAM read/write pointer
	 WriteDM9000(0xFF, 0x80);   // interrupt mask register
	 
	  WriteDM9000(0xFE, 0x80);  // 8-bit mode
	 
	 
	 WriteDM9000(0x02, 0x40);    // Transmit Jabber Disable
	 // WriteDM9000(0x05, 0x40);    // Transmit Jabber Disable
	 
	  // ”становить длину передаваемых пакетов   1067
	  
	   WriteDM9000(0xFC, 0x2b);
	    WriteDM9000(0xFD, 0x04);
	 
	 

	// WriteDM9000(0x02, 0x40);    // Transmit Jabber Disable
	 
	 PORTD &= ~(1<<PORTD7);   // AVR_Din
	 
 }

void InitCDI64500(void)
{
	wdt_reset();
	
	//u8 addr=0;
	
	  // slow
	  
	  /*
	u8 timtable[128]={0xd2, 0x0b, 0x01, 0x00, 0x92, 0x0b, 0x8f, 0x01, 0x9a, 0x0b, 0x1f, 0x03, 0x92, 0x0b, 0x00, 0x00,
					  0x93, 0x0b, 0x27, 0x00, 0x92, 0x0b, 0x01, 0x00, 0x96, 0x0b, 0x3f, 0x01, 0x92, 0x0b, 0x01, 0x00,
					  0x90, 0x0b, 0x10, 0x00, 0x80, 0x0b, 0x20, 0x00, 0x00, 0x0b, 0x10, 0x00, 0x00, 0x0f, 0xf8, 0x3f,
					  0x20, 0x0b, 0x0f, 0x00, 0x10, 0x0b, 0x00, 0x00, 0x10, 0x4b, 0x00, 0x00, 0x12, 0x8b, 0x01, 0x00,
					  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	*/				  
		//	normal		  
/*	u8 timtable[128]={0xd2, 0x0b, 0x01, 0x00, 0x92, 0x0b, 0xc7, 0x00, 0x9a, 0x0b, 0x8f, 0x01, 0x92, 0x0b, 0x00, 0x00,
		0x93, 0x0b, 0x13, 0x00, 0x92, 0x0b, 0x01, 0x00, 0x96, 0x0b, 0x9f, 0x00, 0x92, 0x0b, 0x08, 0x00,
		0x90, 0x0a, 0x10, 0x00, 0x80, 0x0a, 0x20, 0x00, 0x00, 0x0a, 0x10, 0x00, 0x00, 0x0e, 0xf8, 0x3f,
		0x20, 0x0a, 0x07, 0x00, 0x10, 0x0a, 0x00, 0x00, 0x10, 0x42, 0x00, 0x00, 0x12, 0x80, 0x01, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
*/	
	
	u8 timtable[128]={0xd2, 0x0b, 0x01, 0x00, 0x92, 0x0b, 0xc0, 0x00, 0x9a, 0x0b, 0x90, 0x01, 0x92, 0x0b, 0x00, 0x00,
		0x93, 0x0b, 0x12, 0x00, 0x92, 0x0b, 0x01, 0x00, 0x96, 0x0b, 0x96, 0x00, 0x92, 0x0b, 0x08, 0x00,
		0x90, 0x0a, 0x10, 0x00, 0x80, 0x0a, 0x20, 0x00, 0x00, 0x0a, 0x10, 0x00, 0x00, 0x0e, 0xf8, 0x3f,
		0x20, 0x0a, 0x03, 0x00, 0x10, 0x0a, 0x00, 0x00, 0x10, 0x42, 0x00, 0x00, 0x12, 0x80, 0x01, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0x20, 0x07, 0x22};
					  
	u8 i=0;
	
	for(i=0; i<64; i++)
	{
	wdt_reset();
	mb4=0x10;
	twi_write_bytes(TWI_SLA, 0x81, 1, p_mb4);//abort any current frame, if any	
		mb4=*(timtable+i);
		//mb4=0xd2;
	twi_write_bytes(TWI_SLA, i, 1, p_mb4);
	_delay_us(10000);
	
	}
	mb4=0x3f;
	twi_write_bytes(TWI_SLA, 0x80, 1, p_mb4);
	mb4=0x84;
	twi_write_bytes(TWI_SLA, 0x81, 1, p_mb4);	
}
 
 
 

 


int main(void)
{
 PORTD = 0xff;
 DDRD = 0xfe;	
 
 
 
 
 ////////Inicializaciya portov IO
 Init_Main();
 _delay_us(100);
        
 SerilalIni(9600);
 twi_init ();



// t_dec=3000;
 
 my_flag=0;
 p_mb=&mb;
  p_mb2=&mb2;
    p_mb3=&mb3;
	    p_mb4=&mb4;
		
		p_mb5=&mb5;
		p_mb6=&mb6;
		
		
		  //InitCDI64500();
		
 gain=eeprom_read_byte(_pgAin);
 
 t_dec=eeprom_read_word(_ptime_dec);
 
 	//gain=0;		
	 				PORTD &= ~(1<<PORTD3);    //0<<PORTD3;   stop
 							_delay_us(10);
 
 							PORTB &= ~(1<<PORTB7);    //             Work mode BD
 							_delay_us(104);
							PORTB |= 1<<PORTB5;       // Pb5 = 1;    internal synchronisation  
 
 

 
	_delay_ms(500);
	InitDM9000(); 
	
	
	
	
	 sei();                //  cli(); - disable interrupts
	 
	// _delay_ms(1000);
	
	
	 wdt_enable(7);
	
	PORTD |= (1<<PORTD3); // razreshenie raboti;
	
	PORTB |= 1<<PORTB4; //       reset a
	_delay_us(3);
	PORTB &= ~(1<<PORTB4);
	_delay_us(3);
	PORTB |= 1<<PORTB4;
	_delay_us(3);
	
	PORTA &= ~(1<<PORTA3);//      resetb
	_delay_us(4);		//	resetb
	PORTA |= (1<<PORTA3); //resetb
	
	mb6=0x0f;                           // all channels of i2c are on
	twi_write_one_byte(0x70, p_mb6);    //
	_delay_us(100);
	
	InitCDI64500();
	//----acquiring address-------
	i2c_done=1;
	twi_read_bytes(0xff,1,p_mb);
	i2c_done=0;
	_delay_us(100);
	mc=0x80;
	i2c_done=1;
	twi_read_bytes(0xff,1,p_mb);
	i2c_done=0;
	_delay_us(100);
	//----------------------------
	

	SetTim(t_dec);
	
	PORTB &= ~bmBIT0;     //PUSK_W       40
	flag_start=1;
	_delay_us(10);
	PORTB |= bmBIT0;
	_delay_us(100);
	
	PORTA &= ~(1<<PORTA4);
	
	
	i2c_done=1;
	twi_write_bytes(TWI_SLA, 0x81, 1, p_mb4);
	i2c_done=0;
	_delay_us(400);
	mc=0x80;
	i2c_done=1;
	twi_write_bytes(TWI_SLA, mc, 1, &gain);
	i2c_done=0;
	_delay_us(100);
	
	
	/*
	mb4=0x10;
	i2c_done=1;
	twi_write_bytes(0x2a, 1, p_mb4);
	i2c_done=0;
	_delay_us(100);
	mb4=0x00;
	i2c_done=1;
	twi_write_bytes(0x2b, 1, p_mb4);
	i2c_done=0;
	_delay_us(100);
	*/
	
	PORTA |= 1<<PORTA4;
	
	
	
	

 while (1)
      { 
	  wdt_reset();   
//============================================================================================================//11.07.2019
		    if(my_flag == 1) 
			 { 
 			  my_flag=0;
			   wdt_reset();
	
	if((*(abba+1)=='T')&&(*(abba+2)=='R'))
	{
		wdt_reset();
		UDR = 0x5b;
		_delay_us(100);
		UDR = 0x20;
		_delay_us(100);
		UDR = 0x30;
		_delay_us(100);
		UDR = 0x2c;
		_delay_us(100);
		UDR = 0x31;
		_delay_us(100);
		UDR = 0x43;
		_delay_us(100);
		UDR = 0x5d;
		_delay_us(100);
				
	}
	
	if((*(abba+1)=='F')&&(*(abba+2)=='I'))
	{
		wdt_reset();
		UDR = 0x5b;
		_delay_us(100);
		UDR = 0x20;
	    _delay_us(100);
		UDR = 0x30;
		_delay_us(100);
		UDR = 0x2c;
		_delay_us(100);
		UDR = 0x32;
		_delay_us(100);
		UDR = 0x2c;
		_delay_us(100);
		UDR = 'E';
		_delay_us(100);
		UDR = 0x5d;
		_delay_us(100);

	}
//	_delay_ms(50);



if((*(abba+1)=='R')&&(*(abba+2)=='C'))   //Reset DM9000 03.02.2021
{
	wdt_reset();
		UDR = 0x5b;
		_delay_us(100);
		UDR = 0x20;
		_delay_us(100);
		UDR = 0x30;
		_delay_us(100);
		UDR = 0x5d;
		_delay_us(100);
	
	_delay_ms(5000);

}

	
	
	
	
	if((*(abba+1)=='S')&&(*(abba+2)=='F'))
	{
		wdt_reset();
		
	
		if((*(abba+4)=='1')&&(flag_start==0))
		{
		wdt_reset();
		
		
					
		UDR = 0x5b;
		_delay_us(100);
		UDR = 0x20;
		_delay_us(100);
		UDR = 0x30;
		_delay_us(100);
		UDR = 0x5d;
		_delay_us(100);
		
		
		
		//_delay_ms(2);
		
		//UDR = 0x5b;
		//_delay_us(100);
		//UDR = 0x20;
		//_delay_us(100);
		//UDR = 0x30;
		//_delay_us(100);
		//UDR = 0x5d;
		//_delay_us(100);
		
		/*
	
			PORTD |= (1<<PORTD3); // razreshenie raboti;
		
	
		 
			
			PORTB |= 1<<PORTB4; //       reset a
			_delay_us(3);
			PORTB &= ~(1<<PORTB4);
			_delay_us(3);
			PORTB |= 1<<PORTB4;
			_delay_us(3);
						
			
			PORTA &= ~(1<<PORTA3);//      resetb
			_delay_us(4);		//	resetb
			PORTA |= (1<<PORTA3); //resetb	

			

			SetTim(t_dec);	
			
			
				
				
			
			PORTB &= ~bmBIT0;     //PUSK_W       40
			flag_start=1;
			_delay_us(10);
			PORTB |= bmBIT0;
			_delay_us(100);			
		
			PORTA &= ~(1<<PORTA4);	
			
		 
			
				i2c_done=1;
				twi_write_bytes(0x81, 1, p_mb4);
				i2c_done=0;
				_delay_us(100);
				mc=0x80;
				i2c_done=1;
				twi_write_bytes(mc, 1, &gain);
				i2c_done=0;
				_delay_us(100);
				
				
				PORTA |= 1<<PORTA4;
		*/	
				continue;
											
		}
		if((*(abba+4)=='1')&&(flag_start!=0))
		{
			wdt_reset();
		
		   
		
			UDR = 0x5b;
			_delay_us(100);
			UDR = 0x20;
			_delay_us(100);
			UDR = 0x30;
			_delay_us(100);
			UDR = 0x5d;
			_delay_us(100);
			
			
			
			//_delay_ms(2);
			
			//UDR = 0x5b;
			//_delay_us(100);
			//UDR = 0x20;
			//_delay_us(100);
			//UDR = 0x30;
			//_delay_us(100);
			//UDR = 0x5d;
			//_delay_us(100);
		}
		
		if((*(abba+4)=='0')&&(flag_start==1))
		{
			wdt_reset();
			while(i2c_done==1);
		//	PORTD &= ~bmBIT3;
			
			flag_start=0;
			
		/*	
			_delay_us(1000);
			PORTB |= 1<<PORTB4; //       reset a
			_delay_us(3);
			PORTB &= ~(1<<PORTB4);
			_delay_us(3);
			PORTB |= 1<<PORTB4;
			_delay_us(3);
	*/		
			
			UDR = 0x5b;
			_delay_us(100);
			UDR = 0x20;
			_delay_us(100);
			UDR = 0x30;
			_delay_us(100);
			UDR = 0x5d;
			_delay_us(100);
			
			
			//_delay_ms(2);
			
			//UDR = 0x5b;
			//_delay_us(100);
			//UDR = 0x20;
			//_delay_us(100);
			//UDR = 0x30;
			//_delay_us(100);
			//UDR = 0x5d;
			//_delay_us(100);
			
			continue;
			
		} else
		if((*(abba+4)=='0')&&(flag_start!=1))
		{
			wdt_reset();
			
			
			UDR = 0x5b;
			_delay_us(100);
			UDR = 0x20;
			_delay_us(100);
			UDR = 0x30;
			_delay_us(100);
			UDR = 0x5d;
			_delay_us(100);
			
			
			//_delay_ms(2);
			
			//UDR = 0x5b;
			//_delay_us(100);
			//UDR = 0x20;
			//_delay_us(100);
			//UDR = 0x30;
			//_delay_us(100);
			//UDR = 0x5d;
			//_delay_us(100);
						
			
			continue;
			
		}
	}
	
	
	if((*(abba+1)=='S')&&(*(abba+2)=='T'))
	{
		wdt_reset();		
		if(*(abba+4)=='W')
		{
			wdt_reset();
			UDR = 0x5b;
			_delay_us(100);
			UDR = 0x20;
			_delay_us(100);
			UDR = 0x30;
			_delay_us(100);
			UDR = 0x5d;
			
			if(num_byte==0x0b)
			{
				wdt_reset();
				t_dec=(abba[6]-0x30)*1000+(abba[7]-0x30)*100+(abba[8]-0x30)*10+(abba[9]-0x30);
			}
			if(num_byte==0x0a)
			{
				wdt_reset();
				t_dec=(abba[6]-0x30)*100+(abba[7]-0x30)*10+(abba[8]-0x30);
			}
			if(num_byte==0x09)
			{
				wdt_reset();
				t_dec=(abba[6]-0x30)*10+(abba[7]-0x30);
			}
			if(num_byte==0x08)
			{
				wdt_reset();
				t_dec=(abba[6]-0x30);
			}
			SetTim(t_dec);
		}
		
		if(*(abba+4)=='S')
		{
			wdt_reset();
			eeprom_write_word(&time_dec, t_dec);
			UDR = 0x5b;
			_delay_us(100);
			UDR = 0x20;
			_delay_us(100);
			UDR = 0x30;
			_delay_us(100);
			UDR = 0x5d;
			
		}
		
		
		if(*(abba+4)=='R')
		{
			wdt_reset();
		//	while(i2c_done==1);
			UDR = 0x5b;
			_delay_us(100);
			UDR = 0x20;
			_delay_us(100);
			UDR = 0x30;
			_delay_us(100);
			UDR = 0x2c;
						_delay_us(100);
			t=t_dec/1000;
			if(t!=0)
			{
				wdt_reset();
				UDR = t+0x30;
				_delay_us(1200);
			}
			t=(t_dec%1000)/100;

			if((t!=0)||(t_dec/1000!=0))
			{
				wdt_reset();
				UDR = t+0x30;
				_delay_us(1200);
			}

			t=((t_dec%1000)%100)/10;

			if((t!=0)||((t_dec%1000)/100!=0)||(t_dec/1000!=0))
			{
				wdt_reset();
				UDR = t+0x30;
				_delay_us(1200);
			}

			t=((t_dec%1000)%100)%10;

			if((t!=0)||((t_dec%1000)/100!=0)||(t_dec/1000!=0)||(((t_dec%1000)%100)/10))
			{
				wdt_reset();
				UDR = t+0x30;
				_delay_us(1200);
			}

			//preobrasovat t_dec w stroku!
			UDR = 0x5d;
			_delay_us(100);			
		}
		
	}
	
	if((*(abba+1)=='R')&&(*(abba+2)=='I'))
	{
		wdt_reset();
		//while(i2c_done==1);
		u8 i;
		for(i=0;i<64;i++)
		{
			wdt_reset();
			UDR = info[i];
			_delay_us(100);			
		}
		
	}
	
	if((*(abba+1)=='S')&&(*(abba+2)=='U'))
	{
		wdt_reset();
		//while(i2c_done==1);
		if((*(abba+4)=='W')||(*(abba+4)=='S')||(*(abba+4)=='L'))
		{
			wdt_reset();
			UDR = 0x5b;
			_delay_us(100);
			UDR = 0x20;
			_delay_us(100);
			UDR = 0x30;
			_delay_us(100);
			UDR = 0x5d;
						_delay_us(100);
			sum_size=(*(abba+6)-0x30);			
		}
		if(*(abba+4)=='R')
		{
			wdt_reset();
		//	while(i2c_done==1);
			UDR = 0x5b;
			_delay_us(100);
			UDR = 0x20;
			_delay_us(100);
			UDR = 0x30;
			_delay_us(100);
			UDR = 0x2c;
			_delay_us(100);
			UDR=(sum_size+0x30);
			_delay_us(100);
			UDR = 0x5d;
			_delay_us(100);						
		}
	}
	
	
	if((*(abba+1)=='S')&&(*(abba+2)=='I'))
	{
		wdt_reset();
		if(*(abba+4)=='W')
		{
			wdt_reset();
		//	while(i2c_done==1);
			UDR = 0x5b;
			_delay_us(100);
			UDR = 0x20;
			_delay_us(100);
			UDR = 0x30;
			_delay_us(100);
			UDR = 0x5d;
						_delay_us(100);
			i_port=(*(abba+6)-0x30);			
		}
		if(*(abba+4)=='R')
		{
			wdt_reset();
		//	while(i2c_done==1);
			UDR = 0x5b;
			_delay_us(100);
			UDR = 0x20;
			_delay_us(100);
			UDR = 0x30;
			_delay_us(100);
			UDR = 0x2c;
			_delay_us(100);
			UDR=(i_port+0x30);
			_delay_us(100);
			UDR = 0x5d;
			_delay_us(100);						
		}
	}
	


	if((*(abba+1)=='S')&&(*(abba+2)=='S'))
	{
		u8 g=0;
		wdt_reset();
		if(*(abba+4)=='W')
		{
			wdt_reset();
		//	while(i2c_done==1);
			UDR = 0x5b;
			_delay_us(100);
			UDR = 0x20;
			_delay_us(100);
			UDR = 0x30;
			_delay_us(100);
			UDR = 0x5d;
			_delay_us(100);
			
			
		/*	
			switch(*(abba+7))
			{
				case'0': g_odd=0; break;
				case'1': g_odd=1; break;
				case'2': g_odd=2; break;
				case'3': g_odd=3; break;
				case'4': g_odd=4; break;
				case'5': g_odd=5; break;
				case'6': g_odd=6; break;
				case'7': g_odd=7; break;
				case'8': g_odd=8; break;
				case'9': g_odd=9; break;
				case('A'): g_odd=0xa; break;
				case('B'): g_odd=0xb; break;
				case('C'): g_odd=0xc; break;
				case('D'): g_odd=0xd; break;
				case('E'): g_odd=0xe; break;
				case('F'): g_odd=0xf; break;
				case('a'): g_odd=0xa; break;
				case('b'): g_odd=0xb; break;
				case('c'): g_odd=0xc; break;
				case('d'): g_odd=0xd; break;
				case('e'): g_odd=0xe; break;
				case('f'): g_odd=0xf; break;			
			};
			
			switch(*(abba+8))
			{
				case'0': g_even=0; break;
				case'1': g_even=1; break;
				case'2': g_even=2; break;
				case'3': g_even=3; break;
				case'4': g_even=4; break;
				case'5': g_even=5; break;
				case'6': g_even=6; break;
				case'7': g_even=7; break;
				case'8': g_even=8; break;
				case'9': g_even=9; break;
				case('A'): g_even=0xa; break;
				case('B'): g_even=0xb; break;
				case('C'): g_even=0xc; break;
				case('D'): g_even=0xd; break;
				case('E'): g_even=0xe; break;
				case('F'): g_even=0xf; break;
				case('a'): g_even=0xa; break;
				case('b'): g_even=0xb; break;
				case('c'): g_even=0xc; break;
				case('d'): g_even=0xd; break;
				case('e'): g_even=0xe; break;
				case('f'): g_even=0xf; break;
			};
			
			*/
		
		switch(*(abba+7))
		{
			case'7': g=0; break;
			case'6': g=1; break;
			case'5': g=2; break;
			case'4': g=3; break;
			case'3': g=4; break;
			case'2': g=5; break;
			case'1': g=6; break;
			case'0': g=7; break;
			case'8': g=7; break;
			case'9': g=7; break;
			case('A'): g=7; break;
			case('B'): g=7; break;
			case('C'): g=7; break;
			case('D'): g=7; break;
			case('E'): g=7; break;
			case('F'): g=7; break;
			case('a'): g=7; break;
			case('b'): g=7; break;
			case('c'): g=7; break;
			case('d'): g=7; break;
			case('e'): g=7; break;
			case('f'): g=7; break;
		};
		
		if(*(abba+6)=='0')
		{
		g_even=g;	
		}
		if(*(abba+6)=='1')
		{
		g_odd=g;
		}
		
			
			gain = g_even+((g_odd)<<3);
			//gain=g_even;
			
			mc=0x80;
			i2c_done=1;
			twi_write_bytes(TWI_SLA, mc, 1, &gain);
			i2c_done=0;
			_delay_us(5);
			
		}
			
			if(*(abba+4)=='S')
			{
				wdt_reset();
				//	while(i2c_done==1);
				eeprom_write_byte(_pgAin, gain);
				UDR = 0x5b;
				_delay_us(100);
				UDR = 0x20;
				_delay_us(100);
				UDR = 0x30;
				_delay_us(100);
				UDR = 0x5d;
				_delay_us(100);										
			
			}
		
		if(*(abba+4)=='R')
		{
			wdt_reset();
		//	while(i2c_done==1);
			UDR = 0x5b;
			_delay_us(100);
			UDR = 0x20;
			_delay_us(100);
			UDR = 0x30;
			_delay_us(100);
			UDR = 0x2c;
			_delay_us(100);
			//UDR=itoa(gain);
			//_pgAin=&gAin;
			//gain=eeprom_read_byte(_pgAin);
			UDR=gain;
			_delay_us(100);
			UDR = 0x5d;
			_delay_us(100);
												
		}
	}
	
	
	
	if((*(abba+1)=='S')&&(*(abba+2)=='Z'))
	{
		u8 g=0;
		wdt_reset();
		if(*(abba+4)=='W')
		{
			wdt_reset();
		//	while(i2c_done==1);
			UDR = 0x5b;
			_delay_us(100);
			UDR = 0x20;
			_delay_us(100);
			UDR = 0x30;
			_delay_us(100);
			UDR = 0x5d;
			_delay_us(100);
			
			
			
			switch(*(abba+6))
			{
				case'0': g_odd=0; break;
				case'1': g_odd=1; break;
				case'2': g_odd=2; break;
				case'3': g_odd=3; break;
				case'4': g_odd=4; break;
				case'5': g_odd=5; break;
				case'6': g_odd=6; break;
				case'7': g_odd=7; break;
				case'8': g_odd=8; break;
				case'9': g_odd=9; break;
				case('A'): g_odd=0xa; break;
				case('B'): g_odd=0xb; break;
				case('C'): g_odd=0xc; break;
				case('D'): g_odd=0xd; break;
				case('E'): g_odd=0xe; break;
				case('F'): g_odd=0xf; break;
				case('a'): g_odd=0xa; break;
				case('b'): g_odd=0xb; break;
				case('c'): g_odd=0xc; break;
				case('d'): g_odd=0xd; break;
				case('e'): g_odd=0xe; break;
				case('f'): g_odd=0xf; break;			
			};
			
			switch(*(abba+7))
			{
				case'0': g_even=0; break;
				case'1': g_even=1; break;
				case'2': g_even=2; break;
				case'3': g_even=3; break;
				case'4': g_even=4; break;
				case'5': g_even=5; break;
				case'6': g_even=6; break;
				case'7': g_even=7; break;
				case'8': g_even=8; break;
				case'9': g_even=9; break;
				case('A'): g_even=0xa; break;
				case('B'): g_even=0xb; break;
				case('C'): g_even=0xc; break;
				case('D'): g_even=0xd; break;
				case('E'): g_even=0xe; break;
				case('F'): g_even=0xf; break;
				case('a'): g_even=0xa; break;
				case('b'): g_even=0xb; break;
				case('c'): g_even=0xc; break;
				case('d'): g_even=0xd; break;
				case('e'): g_even=0xe; break;
				case('f'): g_even=0xf; break;
			};
			
		
		
			
			gain = g_even+((g_odd)<<4);
			
			mc=0x80;
			i2c_done=1;
			twi_write_bytes(TWI_SLA, mc, 1, &gain);
			i2c_done=0;
			_delay_us(5);
			
		}
			
			if(*(abba+4)=='S')
			{
				wdt_reset();
				//	while(i2c_done==1);
				eeprom_write_byte(_pgAin, gain);
				UDR = 0x5b;
				_delay_us(100);
				UDR = 0x20;
				_delay_us(100);
				UDR = 0x30;
				_delay_us(100);
				UDR = 0x5d;
				_delay_us(100);										
			
			}
		
		if(*(abba+4)=='R')
		{
			wdt_reset();
		//	while(i2c_done==1);
			UDR = 0x5b;
			_delay_us(100);
			UDR = 0x20;
			_delay_us(100);
			UDR = 0x30;
			_delay_us(100);
			UDR = 0x2c;
			_delay_us(100);
			UDR=gain;
			_delay_us(100);
			UDR = 0x5d;
			_delay_us(100);
												
		}
	}
	



	if((*(abba+1)=='E')&&(*(abba+2)=='M'))
	{
		wdt_reset();
		if((*(abba+4)=='W')||(*(abba+4)=='S')||(*(abba+4)=='L'))
		{
			wdt_reset();
		//	while(i2c_done==1);
			UDR = 0x5b;
			_delay_us(100);
			UDR = 0x20;
			_delay_us(100);
			UDR = 0x30;
			_delay_us(100);
			UDR = 0x5d;
						_delay_us(100);
			energy_mode=(*(abba+6)-0x30);			
		}
		
		if(*(abba+4)=='R')
		{
			wdt_reset();
		//	while(i2c_done==1);
			UDR = 0x5b;
			_delay_us(100);
			UDR = 0x20;
			_delay_us(100);
			UDR = 0x30;
			_delay_us(100);
			UDR = 0x2c;
			_delay_us(100);
			UDR=(energy_mode+0x30);
			_delay_us(100);
			UDR = 0x5d;
			_delay_us(100);						
		}
	}



	if((*(abba+1)=='C')&&(*(abba+2)=='N'))
	{
		wdt_reset();
		u8 i;
		if((*(abba+4)=='W')||(*(abba+4)=='S')||(*(abba+4)=='L'))
		{
			wdt_reset();
			//while(i2c_done==1);
			UDR = 0x5b;
			_delay_us(100);
			UDR = 0x20;
			_delay_us(100);
			UDR = 0x30;
			_delay_us(100);
			UDR = 0x5d;
			_delay_us(100);
			channel=(*(abba+6)-0x30);
			card_number[channel]=(*(abba+8)-0x30);
						
		}
		//_delay_ms(100);
		if(*(abba+4)=='R')
		{
			wdt_reset();
			//while(i2c_done==1);
			UDR = 0x5b;
			_delay_us(100);
			UDR = 0x20;
			_delay_us(100);
			UDR = 0x30;
			_delay_us(100);
			UDR = 0x2c;
			_delay_ms(5);
			for(i=0;i<4;i++)
			{
				UDR=(card_number[i]+0x30);
				_delay_us(1200);
			}
			UDR = 0x5d;
			_delay_us(100);			
		}
	}

	if((*(abba+1)=='A')&&(*(abba+2)=='F'))
	{
		wdt_reset();
		if((*(abba+4)=='W')||(*(abba+4)=='S')||(*(abba+4)=='L'))
		{
			wdt_reset();
			//while(i2c_done==1);
			UDR = 0x5b;
			_delay_us(100);
			UDR = 0x20;
			_delay_us(100);
			UDR = 0x30;
			_delay_us(100);
			UDR = 0x5d;
			_delay_us(100);
			avg_filter_size=(*(abba+6)-0x30);			
		}
		if(*(abba+4)=='R')
		{
			wdt_reset();
		//	while(i2c_done==1);
			UDR = 0x5b;
			_delay_us(100);
			UDR = 0x20;
			_delay_us(100);
			UDR = 0x30;
			_delay_us(100);
			UDR = 0x2c;
			_delay_us(100);
			UDR=(avg_filter_size+0x30);
			_delay_us(100);
			UDR = 0x5d;
			_delay_us(100);
												
		}
	}
	if((*(abba+1)=='T')&&(*(abba+2)=='T'))
	{
		if((*(abba+4)=='W')||(*(abba+4)=='S')||(*(abba+4)=='L'))
		{
			wdt_reset();
			UDR = 0x5b;
			_delay_us(100);
			UDR = 0x20;
			_delay_us(100);
			UDR = 0x30;
			_delay_us(100);
			UDR = 0x5d;
			_delay_us(100);
			
			if(*(abba+6)=='0')
			{
				PORTA &= ~(1<<PORTA5);
				PORTA &= ~(1<<PORTA7);
			}
			if(*(abba+6)=='1')
			{
				PORTA |= 1<<PORTA5;
				PORTA |= 1<<PORTA7;	
			}
		}
	
	}
	
	
	//---------Set Offset----------------------------------

			if((*(abba+1)=='S')&&(*(abba+2)=='O'))
			{
				wdt_reset();
				if(*(abba+4)=='W')
				{
					//mbb=(*(abba+6)-0x30);
					UDR = 0x5b;
					_delay_us(700);
					UDR = 0x20;
					_delay_us(700);
					UDR = 0x30;
					_delay_us(700);
					UDR = 0x5d;
					
					addr_board=Converter(*(abba+6));
					m_offset.bb.y23=Converter(*(abba+11))+((Converter(*(abba+10)))<<4);
					m_offset.bb.y22=Converter(*(abba+9))+((Converter(*(abba+8)))<<4);
					//addr_board=0;
					//m_offset.bb.y23=0xee;
					//m_offset.bb.y22=2;
					
					mc=0x2a;       //младш
					i2c_done=1;
					twi_write_bytes(addr_board, mc, 1, &m_offset.bb.y23);
					i2c_done=0;
					_delay_us(5);
					mc=0x2b;       //старш
					
					i2c_done=1;
					twi_write_bytes(addr_board, mc, 1, &m_offset.bb.y22);
					i2c_done=0;
					_delay_us(5);
					
				}
			}
	//------------------------------------------------------



if((*(abba+1)=='Q')&&(*(abba+2)=='A'))
{
	//wdt_reset();
	if(*(abba+4)=='W')
	{
		//wdt_reset();
		//while(i2c_done==1);
		UDR = 0x5b;
		_delay_us(100);
		UDR = 0x20;
		_delay_us(100);
		UDR = 0x30;
		_delay_us(100);
		UDR = 0x5d;
		_delay_us(100);
		i2c_done=1;
		twi_read_bytes(0xff, 1, p_mb);
		i2c_done=0;
		_delay_us(100);
	}
}
	   	
// с 01 -  0		21 -  8		41 - 16		61 - 24
// к 05 -  1		25 -  9		45 - 17		65 - 25
// с 09 -  2		29 - 10		49 - 18		69 - 26
// к 0d -  3		2d - 11		4d - 19		6d - 27
// с 11 -  4		31 - 12		51 - 20		71 - 28
// к 15 -  5		35 - 13		55 - 21		75 - 29
// с 19 -  6		39 - 14		59 - 22		79 - 30
// к 1d -  7		3d - 15		5d - 23		7d - 31
// к 81 - 32		a1 - 40		c1 - 48		e1 - 56
// с 85 - 33		a5 - 41		c5 - 49		e5 - 57
// к 89 - 34		a9 - 42		c9 - 50		e9 - 58
// с 8d - 35		ad - 43		cd - 51		ed - 59
// к 91 - 36		b1 - 44		d1 - 52		f1 - 60
// с 95 - 37		b5 - 45		d5 - 53		f5 - 61
// к 99 - 38		b9 - 46		d9 - 54		f9 - 62
// с 9d - 39		bd - 47		dd - 55		fd - 63

if((*(abba+1)=='T')&&(*(abba+2)=='X')) //тест  [TX,W,xx]
{
	wdt_reset();
	if(*(abba+4)=='W')
	{
		wdt_reset();
		//	while(i2c_done==1);
		UDR = 0x5b;
		_delay_us(100);
		UDR = 0x20;
			_delay_us(100);
		UDR = 0x30;
			_delay_us(100);
		UDR = 0x5d;
			_delay_us(100);
		
		switch(*(abba+6))
		{
			case'0': g_odd=0; break;
			case'1': g_odd=1; break;
			case'2': g_odd=2; break;
			case'3': g_odd=3; break;
			case'4': g_odd=4; break;
			case'5': g_odd=5; break;
			case'6': g_odd=6; break;
			case'7': g_odd=7; break;
			case'8': g_odd=8; break;
			case'9': g_odd=9; break;
			case('A'): g_odd=0xa; break;
			case('B'): g_odd=0xb; break;
			case('C'): g_odd=0xc; break;
			case('D'): g_odd=0xd; break;
			case('E'): g_odd=0xe; break;
			case('F'): g_odd=0xf; break;
			case('a'): g_odd=0xa; break;
			case('b'): g_odd=0xb; break;
			case('c'): g_odd=0xc; break;
			case('d'): g_odd=0xd; break;
			case('e'): g_odd=0xe; break;
			case('f'): g_odd=0xf; break;
		};
		
		switch(*(abba+7))
		{
			case'0': g_even=0; break;
			case'1': g_even=1; break;
			case'2': g_even=2; break;
			case'3': g_even=3; break;
			case'4': g_even=4; break;
			case'5': g_even=5; break;
			case'6': g_even=6; break;
			case'7': g_even=7; break;
			case'8': g_even=8; break;
			case'9': g_even=9; break;
			case('A'): g_even=0xa; break;
			case('B'): g_even=0xb; break;
			case('C'): g_even=0xc; break;
			case('D'): g_even=0xd; break;
			case('E'): g_even=0xe; break;
			case('F'): g_even=0xf; break;
			case('a'): g_even=0xa; break;
			case('b'): g_even=0xb; break;
			case('c'): g_even=0xc; break;
			case('d'): g_even=0xd; break;
			case('e'): g_even=0xe; break;
			case('f'): g_even=0xf; break;
		};
		
		test = g_even + ((g_odd)<<4);
		
		mc=0x83;
		i2c_done=1;
		twi_write_bytes(TWI_SLA, mc, 1, &test);
		i2c_done=0;
		_delay_us(5);
		
	}

}



if((*(abba+1)=='T')&&(*(abba+2)=='Y')) //тест  [TY,W,y,xx]
{
	wdt_reset();
	if(*(abba+4)=='W')
	{
		wdt_reset();
		//	while(i2c_done==1);
		UDR = 0x5b;
		_delay_us(100);
		UDR = 0x20;
		_delay_us(100);
		UDR = 0x30;
		_delay_us(100);
		UDR = 0x5d;
		_delay_us(100);
		
		switch(*(abba+6))
		{
			case'0': g_addr=0; break;
			case'1': g_addr=1; break;
			case'2': g_addr=2; break;
			case'3': g_addr=3; break;
			case'4': g_addr=4; break;
			case'5': g_addr=5; break;
			case'6': g_addr=6; break;
			case'7': g_addr=7; break;
			case'8': g_addr=8; break;
			case'9': g_addr=9; break;
			case('A'): g_addr=0xa; break;
			case('B'): g_addr=0xb; break;
			case('C'): g_addr=0xc; break;
			case('D'): g_addr=0xd; break;
			case('E'): g_addr=0xe; break;
			case('F'): g_addr=0xf; break;
			case('a'): g_addr=0xa; break;
			case('b'): g_addr=0xb; break;
			case('c'): g_addr=0xc; break;
			case('d'): g_addr=0xd; break;
			case('e'): g_addr=0xe; break;
			case('f'): g_addr=0xf; break;
		};
		
		switch(*(abba+8))
		{
			case'0': g_odd=0; break;
			case'1': g_odd=1; break;
			case'2': g_odd=2; break;
			case'3': g_odd=3; break;
			case'4': g_odd=4; break;
			case'5': g_odd=5; break;
			case'6': g_odd=6; break;
			case'7': g_odd=7; break;
			case'8': g_odd=8; break;
			case'9': g_odd=9; break;
			case('A'): g_odd=0xa; break;
			case('B'): g_odd=0xb; break;
			case('C'): g_odd=0xc; break;
			case('D'): g_odd=0xd; break;
			case('E'): g_odd=0xe; break;
			case('F'): g_odd=0xf; break;
			case('a'): g_odd=0xa; break;
			case('b'): g_odd=0xb; break;
			case('c'): g_odd=0xc; break;
			case('d'): g_odd=0xd; break;
			case('e'): g_odd=0xe; break;
			case('f'): g_odd=0xf; break;
		};
		
		switch(*(abba+9))
		{
			case'0': g_even=0; break;
			case'1': g_even=1; break;
			case'2': g_even=2; break;
			case'3': g_even=3; break;
			case'4': g_even=4; break;
			case'5': g_even=5; break;
			case'6': g_even=6; break;
			case'7': g_even=7; break;
			case'8': g_even=8; break;
			case'9': g_even=9; break;
			case('A'): g_even=0xa; break;
			case('B'): g_even=0xb; break;
			case('C'): g_even=0xc; break;
			case('D'): g_even=0xd; break;
			case('E'): g_even=0xe; break;
			case('F'): g_even=0xf; break;
			case('a'): g_even=0xa; break;
			case('b'): g_even=0xb; break;
			case('c'): g_even=0xc; break;
			case('d'): g_even=0xd; break;
			case('e'): g_even=0xe; break;
			case('f'): g_even=0xf; break;
		};
		
		test = g_even + ((g_odd)<<4);
		
		mc=0x83;
		i2c_done=1;
		twi_write_bytes(g_addr, mc, 1, &test);
		//twi_write_bytes(05, mc, 1, &test);
		i2c_done=0;
		_delay_us(5);
		
	}

}


if((*(abba+1)=='T')&&(*(abba+2)=='Z')) //тест  [TZ,W,z,y,xx]
{
	wdt_reset();
	if(*(abba+4)=='W')
	{
		wdt_reset();
		//	while(i2c_done==1);
		UDR = 0x5b;
		_delay_us(100);
		UDR = 0x20;
		_delay_us(100);
		UDR = 0x30;
		_delay_us(100);
		UDR = 0x5d;
		_delay_us(100);
		
		ch_addr=Converter(*(abba+6));
		mb6=ch_addr;                           // 
		twi_write_one_byte(0x70, p_mb6);    //
		_delay_us(100);
		
		g_addr=Converter(*(abba+8));
		g_odd=Converter(*(abba+10));
		g_even=Converter(*(abba+11));                     //
		
		test = g_even + ((g_odd)<<4);
		
		mc=0x83;
		i2c_done=1;
		twi_write_bytes(g_addr, mc, 1, &test);
		//twi_write_bytes(05, mc, 1, &test);
		i2c_done=0;
		_delay_us(5);
		
	}

}







if((*(abba+1)=='V')&&(*(abba+2)=='V'))

{
	wdt_reset();
	UDR = 0x5b;			//[
	_delay_us(100);
	UDR = 0x30;			//0
	_delay_us(100);
	UDR = 0x2c;			//,
	_delay_us(100);					
	UDR = 0x37;			//7
	_delay_us(100);
	UDR = 0x2e;			//.
	_delay_us(100);
	UDR = 0x30;			//0
	_delay_us(100);
	UDR = 0x32;			//2
	_delay_us(100);
	UDR = 0x2f;			// /
	_delay_us(100);
	UDR = 0x32;			//2
	_delay_us(100);
	UDR = 0x35;			//5
	_delay_us(100);
	UDR = 0x30;			//0
	_delay_us(100);
	UDR = 0x32;			//2
	_delay_us(100);
	UDR = 0x32;			//2
	_delay_us(100);
	UDR = 0x31;			//1
	_delay_us(100);
	UDR = 0x5d;			//]
	_delay_us(100);

 }
 
/*
{
	wdt_reset();
	UDR = 0x5b;			//[
	_delay_us(100);
	UDR = 0x20;			//
	_delay_us(100);
	UDR = 0x37;			//7
	_delay_us(100);
	UDR = 0x2e;			//.
	_delay_us(100);
	UDR = 0x30;			//0
	_delay_us(100);
	UDR = 0x31;			//1
	_delay_us(100);
	UDR = 0x44;			//D
	_delay_us(100);
	UDR = 0x56;			//V
	_delay_us(100);
	UDR = 0x2f;			// /
	_delay_us(100);
	UDR = 0x30;			//0
	_delay_us(100);
	UDR = 0x35;			//1
	_delay_us(100);
	UDR = 0x30;			//0
	_delay_us(100);
	UDR = 0x35;			//1
	_delay_us(100);
	UDR = 0x32;			//2
	_delay_us(100);
	UDR = 0x30;			//1
	_delay_us(100);
	UDR = 0x5d;			//]
	_delay_us(100);

}
*/
   for(ink=0;ink<16;ink++)
	{
	 *(abba+ink)=0;
	}
	my_flag = 0;
	byte_count=0;							   
 }
					
// 			

				/*
				if(*(abba+4)=='S')
				{
					eeprom_write_word(p_mb, mbb);
					UDR = 0x5b;
					_delay_us(700);
					UDR = 0x20;
					_delay_us(700);
					UDR = 0x30;
					_delay_us(700);
					UDR = 0x5d;
				}
				*/
			
			
   
  
 
}
}


			
	 
	  





