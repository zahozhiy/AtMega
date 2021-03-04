#ifndef __TWI__
#define __TWI__

#include <inttypes.h>
#include <avr/io.h>
#include <util/twi.h>
#include "types.h"

// Адрес подчиненного устройства на шине I2C для синтезатора
// Si57x можно узнать по его маркировке в разделе сайта
// Lookup and Customize site:silabs.com (см. статью
// http://microsin.net/adminstuff/hardware/si570-si571-i2c-programmable-xo-vcxo.html)
#define TWI_SLA   0x10

/*
 * Максимальное количество итераций для ожидания устройства, когда
 * оно должно ответить на его выбор. Должно быть достаточно большим,
 * чтобы позволить завершение ожидаемой записи, но достаточно малым,
 * чтобы оборвать бесконечный цикл, если slave-устройство не исправно,
 * либо просто отсутствует на шине. На частоте обмена 100 кГц TWI
 * передача старта (start condition) и пакета SLA+R/W занимает около
 * 10 мкс. Ожидается, что самый долгий период записи не превышает
 * примерно 10 мс. Таким образом, нормальное функционирование
 * не требует больше 100 итераций, чтобы получить от устройства
 * ответ на его выбор.
 */
#define MAX_ITER  512

#define START() TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN)
#define WAITTWI() while ((TWCR & _BV(TWINT)) == 0)
extern u8 twst;
   
void twi_init (void);
int twi_read_bytes(u8 addr, int len, u8 *buf);
int twi_write_bytes(u8 slaa, u8 addr, int len, u8 *buf);
int twi_write_one_byte(u8 slaa, u8 *buf);

#endif
