#include "i2c.h"
 #include <util/delay.h>

u8 twst;

//extern _delay_us(int);

void twi_init (void)
{
   /* ������������� ������ TWI: ������� ������ 100 ���,
   TWPS = 0 => ��������� = 1 */
#if defined(TWPS0)
   /* ���� ��������� (ATmega128 � ����� ����� ����������������) */
   TWSR = 0;
#endif

#if F_CPU < 3600000UL
   TWBR = 0x0c;   /* ����� ����� �������� TWBR, ��. ���������� 5 */
  //TWBR=0x50;
//#else
   // ��������� �� 100 ��� ��� SCL:
   //TWBR = (F_CPU / 100000UL - 16) / 2;
   // ��������� �� 400 ��� ��� SCL:
   //TWBR = (F_CPU / 400000UL - 16) / 2;
#endif
}








int twi_read_bytes(u8 addr, int len, u8 *buf)
{
   uint8_t sla, twcr, n = 0;
   int rv = 0;

   sla = TWI_SLA << 1;
 // sla=0xa0;

   /*
    * ������ ����: master transmitter mode
    */
restart:
   if (n++ >= MAX_ITER)
      return -1;
begin:

   TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN); /* �������� start condition */
   while ((TWCR & _BV(TWINT)) == 0); /* �������� �������� */
   switch ((twst = TW_STATUS))
   {
   case TW_REP_START:   /* OK, �� ������ �� ������ ��������� */
   case TW_START:
      break;

   case TW_MT_ARB_LOST: /* ���������� 9 */
      goto begin;

   default:
      return -1;  /* ������: �� � ��������� start condition */
                   /* NB: �� ����� ���������� stop condition */
    }

   /* �������� SLA+W */
   TWDR = sla | TW_WRITE;

   TWCR = _BV(TWINT) | _BV(TWEN); /* ������� ���������� ��� ������� �������� */
   while ((TWCR & _BV(TWINT)) == 0); /* �������� �������� */
   switch ((twst = TW_STATUS))
   {
   case TW_MT_SLA_ACK:
      break;

   case TW_MT_SLA_NACK: /* NACK ��� ������: ���������� ������ ������� */
                        /* ���������� 11 */
      goto restart;

   case TW_MT_ARB_LOST: /* ��������� �������� */
      goto begin;

   default:
      goto error;       /* ����� ��������� stop condition */
    }

   TWDR = addr;  /* ������� 8 ��� ������ */
   TWCR = _BV(TWINT) | _BV(TWEN); /* ������� ���������� ��� ������� �������� */
   while ((TWCR & _BV(TWINT)) == 0) ; /* �������� �������� */
   switch ((twst = TW_STATUS))
   {
   case TW_MT_DATA_ACK:
      break;

   case TW_MT_DATA_NACK:
      goto quit;

   case TW_MT_ARB_LOST:
      goto begin;

   default:
      goto error; /* ����� ��������� stop condition */
    }

   /*
    * ��������� ���� (��� �����): master receiver mode
    */
   TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN); /* send (rep.) start condition */
   while ((TWCR & _BV(TWINT)) == 0) ; /* �������� �������� */
   switch ((twst = TW_STATUS))
   {
   case TW_START: /* OK, �� ������ �� ������ ��������� */
   case TW_REP_START:
      break;

   case TW_MT_ARB_LOST:
      goto begin;

   default:
      goto error;
   }
   

   /* �������� SLA+R */
   TWDR = sla | TW_READ;
       _delay_us(100);
   TWCR = _BV(TWINT) | _BV(TWEN); /* ������� ���������� ��� ������� �������� */
 
   while ((TWCR & _BV(TWINT)) == 0); /* �������� �������� */
  
   switch ((twst = TW_STATUS))
   {
   case TW_MR_SLA_ACK:
      break;

   case TW_MR_SLA_NACK:
      goto quit;

   case TW_MR_ARB_LOST:
      goto begin;

   default:
      goto error;
   }
   
   _delay_us(100);

   for (twcr = _BV(TWINT) | _BV(TWEN) | _BV(TWEA) /* ���������� 13 */;
        len > 0;
        len--)
   {
      if (len == 1)
         twcr = _BV(TWINT) | _BV(TWEN); /* �� ���� ��� �������� NACK */
      TWCR = twcr;   /* ������� ���������� ��� ������� �������� */
      while ((TWCR & _BV(TWINT)) == 0) ; /* �������� �������� */
      switch ((twst = TW_STATUS))
      {
      case TW_MR_DATA_NACK:
         len = 0;    /* �������������� ���������� ����� */
         /* ���� ������ */
      case TW_MR_DATA_ACK:
         *buf++ = TWDR;
         rv++;
         if(twst == TW_MR_DATA_NACK) goto quit;
         break;

      default:
         goto error;
      }
   }
quit:
   /* ���������� 14 */
   TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN); /* �������� stop condition */

   return rv;

error:
   rv = -1;
   goto quit;
}












int twi_write_bytes(u8 slaa, u8 addr, int len, u8 *buf)
{
   uint8_t sla, n = 0;
   int rv = 0;

  // sla = TWI_SLA_SI570 << 1;
//sla=0xa0;
sla=slaa<<1;

restart:
   if (n++ >= MAX_ITER)
      return -1;
begin:

   TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN); /* �������� start */
   while ((TWCR & _BV(TWINT)) == 0); /* �������� �������� */
   switch ((twst = TW_STATUS))
   {
   case TW_REP_START:   /* OK, �� ������ �� ������ ��������� */
   case TW_START:
      break;

   case TW_MT_ARB_LOST:
      goto begin;

   default:
      return -1;  /* ������: �� � ��������� start condition */
                  /* NB: �� ����� �������� stop condition */
   }

   /* �������� SLA+W */
   TWDR = sla | TW_WRITE;
   TWCR = _BV(TWINT) | _BV(TWEN); /* ������� ���������� ��� ������� �������� */
   while ((TWCR & _BV(TWINT)) == 0) ; /* �������� �������� */
   switch ((twst = TW_STATUS))
   {
   case TW_MT_SLA_ACK:
      break;

   case TW_MT_SLA_NACK: /* NACK �� ����� ������: ���������� ������ ������� */
      goto restart;

   case TW_MT_ARB_LOST: /* ��������� �������� */
      goto begin;

   default:
      goto error;    /* ����� ��������� stop condition */
   }

   TWDR = addr; /* ������� 8 ��� ������ */
   TWCR = _BV(TWINT) | _BV(TWEN); /* ������� ���������� ��� ������� �������� */
   while ((TWCR & _BV(TWINT)) == 0); /* �������� �������� */
   switch ((twst = TW_STATUS))
   {
   case TW_MT_DATA_ACK:
      break;

   case TW_MT_DATA_NACK:
      goto quit;

   case TW_MT_ARB_LOST:
      goto begin;

   default:
      goto error; /* ����� ��������� stop condition */
   }

   for (; len > 0; len--)
   {
      TWDR = *buf++;
      TWCR = _BV(TWINT) | _BV(TWEN); /* ������ �������� */
      while ((TWCR & _BV(TWINT)) == 0) ; /* �������� �������� */
      switch ((twst = TW_STATUS))
      {
      case TW_MT_DATA_NACK:
         goto error;    /* ���������� �������� �� ������ - ���������� 16 */

      case TW_MT_DATA_ACK:
         rv++;
         break;

      default:
         goto error;
      }
   }
quit:
   TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN); /* �������� stop condition */

   return rv;

error:
   rv = -1;
   goto quit;
}



int twi_write_one_byte(u8 slaa, u8 *buf)
{
	uint8_t sla, n = 0;
	int rv = 0;

	// sla = TWI_SLA_SI570 << 1;
	//sla=0xa0;
	sla=slaa<<1;

	restart:
	if (n++ >= MAX_ITER)
	return -1;
	begin:

	TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN); /* �������� start */
	while ((TWCR & _BV(TWINT)) == 0); /* �������� �������� */
	switch ((twst = TW_STATUS))
	{
		case TW_REP_START:   /* OK, �� ������ �� ������ ��������� */
		case TW_START:
		break;

		case TW_MT_ARB_LOST:
		goto begin;

		default:
		return -1;  /* ������: �� � ��������� start condition */
		/* NB: �� ����� �������� stop condition */
	}

	/* �������� SLA+W */
	TWDR = sla | TW_WRITE;
	TWCR = _BV(TWINT) | _BV(TWEN); /* ������� ���������� ��� ������� �������� */
	while ((TWCR & _BV(TWINT)) == 0) ; /* �������� �������� */
	switch ((twst = TW_STATUS))
	{
		case TW_MT_SLA_ACK:
		break;

		case TW_MT_SLA_NACK: /* NACK �� ����� ������: ���������� ������ ������� */
		goto restart;

		case TW_MT_ARB_LOST: /* ��������� �������� */
		goto begin;

		default:
		goto error;    /* ����� ��������� stop condition */
	}

	
	
	{
		TWDR = *buf++;
		TWCR = _BV(TWINT) | _BV(TWEN); /* ������ �������� */
		while ((TWCR & _BV(TWINT)) == 0) ; /* �������� �������� */
		switch ((twst = TW_STATUS))
		{
			case TW_MT_DATA_NACK:
			goto error;    /* ���������� �������� �� ������ - ���������� 16 */

			case TW_MT_DATA_ACK:
			rv++;
			break;

			default:
			goto error;
		}
	}
	quit:
	TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN); /* �������� stop condition */

	return rv;

	error:
	rv = -1;
	goto quit;
}
