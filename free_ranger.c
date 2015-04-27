#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

/*
  Tx: PD2 tied to NPN.  (For nano and promini, PD2 = 2)
  Rx:
  Pos input to comparator MUST be AIN0 = OC0A/PD6/nano6
  Let Neg input be the default, AIN1 = PD7/nano7
  Runs with interrupts off, so does not tie up a timer.

  Triggers when input waveform is at least around 250mV.
 */

#define LOW PORTD &= ~_BV(PD2)
#define HIGH PORTD |= _BV(PD2)
#define DELAY _delay_us(12)
#define OUTPUT DDRD |= _BV(PD2)
#define INPUT DDRD &= ~_BV(PD2)

void putch(char ch)
{
  while( ! (UCSR0A & _BV(UDRE0)) );
  UDR0 = ch;
}
void putstr(char * s)
{
  while( *s )putch(*s++);
}
void putd(uint16_t d)
{
  char a[10];
  uint8_t i;
  i = sizeof(a) - 1;
  a[i--] ='\0';
  do { a[i--] = '0' + (d%10); } while( (d /= 10) );
  putstr(a+i+1);
}

int8_t range(void)
{
  uint8_t buckets[32];
  uint16_t u;

  DDRD |= _BV(PD3);
  PORTD |= _BV(PD3);

  for(u = 0; u < sizeof(buckets); u++)buckets[u] = 0;

  ADCSRB &= ~_BV(ACME);               /* disable Neg input mux */
  ACSR = _BV(ACI)                     /* clear ACI */
       | _BV(ACIS0) | _BV(ACIS1);     /* Triggers on rising edge */
  LOW;
  OUTPUT;
  HIGH; DELAY; LOW; DELAY;
  HIGH; DELAY; LOW; INPUT;

  ACSR |= _BV(ACI);

  /*
    100us buckets should be ok, corresponds to round trip
    distance of 15cm, range of 7.5cm.

    Max range not expected to be more than about 3m, or 4ms
    total flight time, or about 40 buckets.  Rounded off to 32.

    At 100us, each bucket cannot possibly contain more than
    100/25 = 4 hits.

    Estimate about 2us around this loop, therefore 50loops per
    bucket round off to 64 loops/bucket, 
   */
#define LOOPS_PER_BUCKET 64
  for(u = 0; u < LOOPS_PER_BUCKET*sizeof(buckets); u++){
    uint8_t bit;
    PIND |= _BV(PD3);
    bit = (ACSR & _BV(ACI));
    bit >>= ACI;
    ACSR |= _BV(ACI);
    buckets[u/LOOPS_PER_BUCKET] += bit;
  }
  PORTD &= ~_BV(PD3);
  /*
    Look for first run of 8 or more bits, past the 600us mark.
   */
  for(u = 0; u < sizeof(buckets)-2; u++){
    putd(buckets[u]);
    putch(' ');
  }
  putstr("\r\n");

  for(u = 6; u < sizeof(buckets)-2; u++){
    uint8_t bits;
    bits = buckets[u] + buckets[u+1] + buckets[u+2];
    if( bits >= 8 )return((int8_t)u);
  }

  return(-1);
}

int
main(void)
{
  UBRR0 = 103;                        /* 9600 bps */
  UCSR0B |= _BV(TXEN0);
  putstr("Starting\r\n");
  for( ; ; ){
    uint16_t i;
    i = range();
    putd(i);
    putstr("\r\n");
    _delay_ms(1200);
  }
}
