#include "i2c.h"
 #include <util/delay.h>

u8 twst;

//extern _delay_us(int);

void twi_init (void)
{
   /* Инициализация тактов TWI: частота тактов 100 кГц,
   TWPS = 0 => прескалер = 1 */
#if defined(TWPS0)
   /* Есть прескалер (ATmega128 и более новые микроконтроллеры) */
   TWSR = 0;
#endif

#if F_CPU < 3600000UL
   TWBR = 0x0c;   /* Самое малое значение TWBR, см. примечание 5 */
  //TWBR=0x50;
//#else
   // Настройка на 100 кГц для SCL:
   //TWBR = (F_CPU / 100000UL - 16) / 2;
   // Настройка на 400 кГц для SCL:
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
    * Первый цикл: master transmitter mode
    */
restart:
   if (n++ >= MAX_ITER)
      return -1;
begin:

   TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN); /* передача start condition */
   while ((TWCR & _BV(TWINT)) == 0); /* ожидание передачи */
   switch ((twst = TW_STATUS))
   {
   case TW_REP_START:   /* OK, но ничего не должно произойти */
   case TW_START:
      break;

   case TW_MT_ARB_LOST: /* Примечание 9 */
      goto begin;

   default:
      return -1;  /* Ошибка: не в состоянии start condition */
                   /* NB: не нужно отправлять stop condition */
    }

   /* Отправка SLA+W */
   TWDR = sla | TW_WRITE;

   TWCR = _BV(TWINT) | _BV(TWEN); /* очистка прерывания для запуска передачи */
   while ((TWCR & _BV(TWINT)) == 0); /* ожидание передачи */
   switch ((twst = TW_STATUS))
   {
   case TW_MT_SLA_ACK:
      break;

   case TW_MT_SLA_NACK: /* NACK при выборе: устройство занято записью */
                        /* Примечание 11 */
      goto restart;

   case TW_MT_ARB_LOST: /* повторный арбитраж */
      goto begin;

   default:
      goto error;       /* нужно отправить stop condition */
    }

   TWDR = addr;  /* младшие 8 бит адреса */
   TWCR = _BV(TWINT) | _BV(TWEN); /* очистка прерывания для запуска передачи */
   while ((TWCR & _BV(TWINT)) == 0) ; /* ожидание передачи */
   switch ((twst = TW_STATUS))
   {
   case TW_MT_DATA_ACK:
      break;

   case TW_MT_DATA_NACK:
      goto quit;

   case TW_MT_ARB_LOST:
      goto begin;

   default:
      goto error; /* нужно отправить stop condition */
    }

   /*
    * Следующий цикл (или циклы): master receiver mode
    */
   TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN); /* send (rep.) start condition */
   while ((TWCR & _BV(TWINT)) == 0) ; /* ожидание передачи */
   switch ((twst = TW_STATUS))
   {
   case TW_START: /* OK, но ничего не должно произойти */
   case TW_REP_START:
      break;

   case TW_MT_ARB_LOST:
      goto begin;

   default:
      goto error;
   }
   

   /* Отправка SLA+R */
   TWDR = sla | TW_READ;
       _delay_us(100);
   TWCR = _BV(TWINT) | _BV(TWEN); /* очистка прерывания для запуска передачи */
 
   while ((TWCR & _BV(TWINT)) == 0); /* ожидание передачи */
  
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

   for (twcr = _BV(TWINT) | _BV(TWEN) | _BV(TWEA) /* Примечание 13 */;
        len > 0;
        len--)
   {
      if (len == 1)
         twcr = _BV(TWINT) | _BV(TWEN); /* на этот раз отправка NACK */
      TWCR = twcr;   /* очистка прерывания для запуска передачи */
      while ((TWCR & _BV(TWINT)) == 0) ; /* ожидание передачи */
      switch ((twst = TW_STATUS))
      {
      case TW_MR_DATA_NACK:
         len = 0;    /* принудительное завершение цикла */
         /* ИДЕМ ДАЛЬШЕ */
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
   /* Примечание 14 */
   TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN); /* отправка stop condition */

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

   TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN); /* Передача start */
   while ((TWCR & _BV(TWINT)) == 0); /* ожидание передачи */
   switch ((twst = TW_STATUS))
   {
   case TW_REP_START:   /* OK, но ничего не должно произойти */
   case TW_START:
      break;

   case TW_MT_ARB_LOST:
      goto begin;

   default:
      return -1;  /* ошибка: не в состоянии start condition */
                  /* NB: не нужно посылать stop condition */
   }

   /* отправка SLA+W */
   TWDR = sla | TW_WRITE;
   TWCR = _BV(TWINT) | _BV(TWEN); /* очистка прерывания для запуска передачи */
   while ((TWCR & _BV(TWINT)) == 0) ; /* ожидание передачи */
   switch ((twst = TW_STATUS))
   {
   case TW_MT_SLA_ACK:
      break;

   case TW_MT_SLA_NACK: /* NACK во время выбора: устройство занято записью */
      goto restart;

   case TW_MT_ARB_LOST: /* повторный арбитраж */
      goto begin;

   default:
      goto error;    /* нужно отправить stop condition */
   }

   TWDR = addr; /* младшие 8 бит адреса */
   TWCR = _BV(TWINT) | _BV(TWEN); /* очистка прерывания для запуска передачи */
   while ((TWCR & _BV(TWINT)) == 0); /* ожидание передачи */
   switch ((twst = TW_STATUS))
   {
   case TW_MT_DATA_ACK:
      break;

   case TW_MT_DATA_NACK:
      goto quit;

   case TW_MT_ARB_LOST:
      goto begin;

   default:
      goto error; /* нужно отправить stop condition */
   }

   for (; len > 0; len--)
   {
      TWDR = *buf++;
      TWCR = _BV(TWINT) | _BV(TWEN); /* запуск передачи */
      while ((TWCR & _BV(TWINT)) == 0) ; /* ожидание передачи */
      switch ((twst = TW_STATUS))
      {
      case TW_MT_DATA_NACK:
         goto error;    /* устройство защищено от записи - Примечание 16 */

      case TW_MT_DATA_ACK:
         rv++;
         break;

      default:
         goto error;
      }
   }
quit:
   TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN); /* отправка stop condition */

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

	TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN); /* Передача start */
	while ((TWCR & _BV(TWINT)) == 0); /* ожидание передачи */
	switch ((twst = TW_STATUS))
	{
		case TW_REP_START:   /* OK, но ничего не должно произойти */
		case TW_START:
		break;

		case TW_MT_ARB_LOST:
		goto begin;

		default:
		return -1;  /* ошибка: не в состоянии start condition */
		/* NB: не нужно посылать stop condition */
	}

	/* отправка SLA+W */
	TWDR = sla | TW_WRITE;
	TWCR = _BV(TWINT) | _BV(TWEN); /* очистка прерывания для запуска передачи */
	while ((TWCR & _BV(TWINT)) == 0) ; /* ожидание передачи */
	switch ((twst = TW_STATUS))
	{
		case TW_MT_SLA_ACK:
		break;

		case TW_MT_SLA_NACK: /* NACK во время выбора: устройство занято записью */
		goto restart;

		case TW_MT_ARB_LOST: /* повторный арбитраж */
		goto begin;

		default:
		goto error;    /* нужно отправить stop condition */
	}

	
	
	{
		TWDR = *buf++;
		TWCR = _BV(TWINT) | _BV(TWEN); /* запуск передачи */
		while ((TWCR & _BV(TWINT)) == 0) ; /* ожидание передачи */
		switch ((twst = TW_STATUS))
		{
			case TW_MT_DATA_NACK:
			goto error;    /* устройство защищено от записи - Примечание 16 */

			case TW_MT_DATA_ACK:
			rv++;
			break;

			default:
			goto error;
		}
	}
	quit:
	TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN); /* отправка stop condition */

	return rv;

	error:
	rv = -1;
	goto quit;
}
