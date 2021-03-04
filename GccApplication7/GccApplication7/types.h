#ifndef __TYPES__
#define __TYPES__

//#include <inttypes.h>
#include <stdbool.h>

#define u8 unsigned char
#define u16 unsigned int
#define u32 uint32_t
#define false 0
#define true  1

typedef struct _eeparam
{
   u8 seedAA;
   u8 ledstate;   //состояние светодиода
   // .. тут можно добавить свои параметры
   u16 crc;
}Teeparam;

#endif  //__TYPES__
