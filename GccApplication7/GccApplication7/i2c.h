#ifndef __TWI__
#define __TWI__

#include <inttypes.h>
#include <avr/io.h>
#include <util/twi.h>
#include "types.h"

// ����� ������������ ���������� �� ���� I2C ��� �����������
// Si57x ����� ������ �� ��� ���������� � ������� �����
// Lookup and Customize site:silabs.com (��. ������
// http://microsin.net/adminstuff/hardware/si570-si571-i2c-programmable-xo-vcxo.html)
#define TWI_SLA   0x10

/*
 * ������������ ���������� �������� ��� �������� ����������, �����
 * ��� ������ �������� �� ��� �����. ������ ���� ���������� �������,
 * ����� ��������� ���������� ��������� ������, �� ���������� �����,
 * ����� �������� ����������� ����, ���� slave-���������� �� ��������,
 * ���� ������ ����������� �� ����. �� ������� ������ 100 ��� TWI
 * �������� ������ (start condition) � ������ SLA+R/W �������� �����
 * 10 ���. ���������, ��� ����� ������ ������ ������ �� ���������
 * �������� 10 ��. ����� �������, ���������� ����������������
 * �� ������� ������ 100 ��������, ����� �������� �� ����������
 * ����� �� ��� �����.
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
