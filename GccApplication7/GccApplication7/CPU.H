//===========================================================================
#ifndef CPU_H                      
#define CPU_H                                
//==========================================================================
#include <intrinsics.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <pgmspace.h>
//#pragma inline=forced
//---------------------------------------------------------------------------
#define F_CPU     16000000UL
#define _delay_ms(a) __delay_cycles(a*(F_CPU/1000UL))
#define _delay_us(a) __delay_cycles(a*(F_CPU/1000000UL))
#define _delay_ns(a) __delay_cycles(a*(F_CPU/1000000000UL))
//===========================================================================
#endif
//===========================================================================
