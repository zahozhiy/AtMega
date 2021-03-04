#include <stdio.h>
#include <avr\io.h>

#define BAUD 9600 
#define F_CPU 16000000L 

extern int uart_putchar(char c, FILE *stream); 
extern void SerilalIni(void); 