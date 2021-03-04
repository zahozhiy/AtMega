//===========================================================================
#ifndef USART_H                      
#define USART_H                                
//===========================================================================
#define USART_DEFAULT_BPS     9600UL
//#define USART_DEFAULT_BPS      57600UL
//---------------------------------------------------------------------------
#define USART_CountParameters 10
//---------------------------------------------------------------------------
//#ifdef Externals
//extern
//#endif
//uint8_t USART_RxData,USART_RxOk,USART_StringComplete;
//---------------------------------------------------------------------------
#ifdef Externals
extern
#endif
struct{   //   uint8_t  TxBuf [256];
             uint8_t  RxBuf [256];
             uint8_t  BufCmd[256];
    volatile uint8_t  TxPosWr;
    volatile uint8_t  TxPosRd;
    volatile uint8_t  RxPosWr;
    volatile uint8_t  RxPosRd;
             uint64_t Parameters[USART_CountParameters];
             uint8_t  ParametersCnt;
             uint32_t FrameNumber;
//             uint8_t  Echo     :1;
             uint8_t  CheckCRC :1;
}USART;
//---------------------------------------------------------------------------
#ifdef Externals
extern
#endif
void USART_BufRxClr(void);
//---------------------------------------------------------------------------
#ifdef Externals
extern
#endif
uint32_t USART_Init(uint32_t Bps,uint8_t Test);
//---------------------------------------------------------------------------
#ifdef Externals
extern
#endif
uint8_t HexToDec(uint8_t Data);
//---------------------------------------------------------------------------
#ifdef Externals
extern
#endif
uint8_t ReadParameters(void);
//---------------------------------------------------------------------------
#ifdef Externals
extern
#endif
void USART_TxByte(uint8_t Data);
//---------------------------------------------------------------------------
#ifdef Externals
extern
#endif
void USART_TxByte_(uint8_t Data);
//---------------------------------------------------------------------------
#ifdef Externals
extern
#endif
size_t __write(int handle, const unsigned char *buf, size_t bufSize);
//---------------------------------------------------------------------------
#define putchar(x) USART_TxByte((uint8_t) x)
//---------------------------------------------------------------------------
//#define PSTR(s) ((uint8_t const __flash  *)(s))// Flash 2
//#define PSTR(s) ((char __flash  *)(s))// Flash 1
#define PSTR(s) (s)                   // None
//#define PSTR (s) ((const PROGMEM char *)(s)) // Default
//---------------------------------------------------------------------------
#ifdef Externals
extern
#endif
//uint8_t FindString(uint8_t*Data1, uint8_t *Data2)
uint8_t FindString(uint8_t*Data1, char __flash *Data2);
//uint8_t FindString(uint8_t*Data1, uint8_t __flash const *Data2)
//---------------------------------------------------------------------------
#ifdef Externals
extern
#endif
//void USART_TxString(uint8_t *Data);
void USART_TxString(char __flash *Data);
//void USART_TxString(uint8_t __flash const *Data);
//---------------------------------------------------------------------------
#ifdef Externals
extern
#endif
void USART_TxHex4(uint8_t Data);
//---------------------------------------------------------------------------
#ifdef Externals
extern
#endif
void USART_TxHex8 (uint8_t  Data);
//---------------------------------------------------------------------------
#ifdef Externals
extern
#endif
void USART_TxHex12(uint16_t Data);
//---------------------------------------------------------------------------
#ifdef Externals
extern
#endif
void USART_TxHex16(uint16_t Data);
//---------------------------------------------------------------------------
#ifdef Externals
extern
#endif
void USART_TxHex20(uint32_t Data);
//---------------------------------------------------------------------------
#ifdef Externals
extern
#endif
void USART_TxHex24(uint32_t Data);
//---------------------------------------------------------------------------
#ifdef Externals
extern
#endif
void USART_TxHex28(uint32_t Data);
//---------------------------------------------------------------------------
#ifdef Externals
extern
#endif
void USART_TxHex32(uint32_t Data);
//---------------------------------------------------------------------------
#ifdef Externals
extern
#endif
void USART_TxData_Base(uint64_t Data,uint8_t Base);
//---------------------------------------------------------------------------
#ifdef Externals
extern
#endif
void TxCRC(void);
//---------------------------------------------------------------------------
#ifdef Externals
extern
#endif
uint8_t RxByteUSART_(void);
//---------------------------------------------------------------------------
#ifdef Externals
extern
#endif
uint8_t RxByteUSART(void);
//===========================================================================
#endif
//===========================================================================
