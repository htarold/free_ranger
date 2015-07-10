/*
  free_ranger demo code.  Code compiled and ran for nano V3.
  
  Code below outputs to a 8x2 text LCD as well as through the serial monitor.
  The code will still function if you don't connect an LCD.
  
  Pins:
  LCD RS: 12
  LCD En: 11
  LCD D4: 5
  LCD D5: 4
  LCD D6: 3
  LCD D7: 2
  LCD V0: 9 (display contrast voltage via PWM)
  ranger comp- (signal): 7
  ranger comp+ (dither): 6

 */

#include <LiquidCrystal.h>

//LiquidCrystal lcd(12, 11, 5, 4, 3, 2);


//#include <Arduino.h>
#include <util/delay.h>

uint8_t dither_duty;
/*
  To use dithering, there must be a 10k/10n LPF between
  pin 11/PB3/OC2A and pin 7/AIN1.
  Call dither_calibrate() with the ranger quiescent to
  calibrate the duty.
  62.5kHz => 16us period.
 */
void dither_start(void)
{
  if (!dither_duty) dither_duty = 82;
  DDRB |= _BV(DDB3);                  /* = arduino pin d11 */
  OCR2A = dither_duty;
  TCCR2A = _BV(WGM21) | _BV(WGM20)    /* Fast PWM mode 3 */
         | _BV(COM2A1);               /* output on Arduino 11 = PB3 = OC2A */
  TCCR2B = _BV(CS20);                 /* /1 => 62.5kHz */
}
void dither_stop(void)
{ 
  TCCR2B = 0;
  PORTB &= ~_BV(PORTB3);
  DDRB &= ~_BV(DDB3);
}
static int8_t comparator_triggered(void)
{
  int8_t hit;
  _delay_us(128);                     /* delay 2 periods */
  hit = ACSR & _BV(ACI);
  ACSR |= _BV(ACI);
  return(!!hit);
}
int8_t dither_calibrate(void)
{
  uint8_t i;
  ACSR = _BV(ACI)
       | _BV(ACIS1) | _BV(ACIS0);     /* Rising edge */
  /*
    Start comp+ voltage at signal q-point, around 2.0V
   */
  dither_duty = (uint8_t)(255*2.0/5.0);
  dither_start();
  _delay_ms(500);                     /* stabilise */
  for(i = 0; comparator_triggered(); i++){
    if (i > 250) { dither_stop(); return(-1); }
    dither_duty--;
    dither_start();
    _delay_us(10);                    /* stabilise */
  }
  dither_stop();
  dither_duty -= dither_duty/8;       /* fudge a bit */
  return(0);
}

void setup(void)
{
  //pinMode(9, OUTPUT);
  //analogWrite(9, 50);  /* Contrast control.  Lower voltage = higher contrast */
  //lcd.begin(8, 2);
  //lcd.print("range:");
  Serial.begin(9600);
  //dither_start((1.6/5.0)*256);
  _delay_ms(1000); /* Let everything settle */
  if (dither_calibrate()) {
    Serial.print("Calibrate error\r\n");
    for( ; ; );
  }
  Serial.print("Dither duty = ");
  Serial.print(dither_duty);
  Serial.print("\r\n");
  dither_start();
  Serial.print("Starting\r\n");
  delay(100);
}

static inline void pulse(int txpin)
{
  digitalWrite(txpin, HIGH);
  _delay_us(11);                      /* digitalWrite() very slow */
  digitalWrite(txpin, LOW);
  _delay_us(11);
}
#define NR_CELLS 32
uint8_t cells[NR_CELLS];
int16_t range(uint8_t arduino_pin)
{
  volatile uint8_t sreg;
  uint8_t c;

  sreg = SREG;
  cli();

  /*
    Initialise comparator
   */

  ADCSRB &= ~_BV(ACME);               /* disable Neg input mux */
  ACSR = 0;                           /* enable comparator */
  DIDR1 = _BV(AIN0D) | _BV(AIN1D);    /* disable digital input */

  /*
    Send 2 pulses
   */

  pinMode(arduino_pin, OUTPUT);
  pulse(arduino_pin);
  pulse(arduino_pin);

  /*
    Reduce ringing, dies out after about 550us.
   */

  _delay_us(15);
  pulse(arduino_pin);
  pulse(arduino_pin);
  
  /*
    Fill in histogram.
    On regular Arduinos, timer 0 is configured to run at
    4us/tick.  Each of our cells is US_PER_CELL long.  At
    each tick, we sample the comparator.
   */

#define TICKS_PER_CELL 50
#define US_PER_TICK 4
#define US_PER_CELL (TICKS_PER_CELL * US_PER_TICK)

  for(c = 0; c < NR_CELLS; c++){
    cells[c] = 0;
    for(uint8_t ticks = 0; ticks < TICKS_PER_CELL; ticks++){
      for(uint8_t tcnt0 = TCNT0; tcnt0 == TCNT0; );
      if ((ACSR & _BV(ACO)))
        cells[c]++;
    }
  }

  SREG = sreg;

  /*
    Find the echo.
    If 1 cell is fully
    detected, comparator will be triggered on half the
    samples in that cell (TICKS_PER_CELL/2).

    XXX
   */

  for(c = 0; c < NR_CELLS; c++){
    Serial.print(cells[c]);
    Serial.print(" ");
  }
  Serial.print("\r\n");

  int8_t diff = 0;
  uint8_t echo = 
  c = 550 /* Rings for this long (in us) */
      / US_PER_CELL;
  for( ; c < NR_CELLS; c++){
    int8_t d = cells[c] - cells[c-1]
      + (c/8);      /* emphasise later cells */
    if (d > diff) {
      diff = d;
      echo = c;
    }
  }
  
  if (0 == diff) return(-1);
  
  /*
    Transmission offset is 4.5 cycles, or 4.5 * 30us = 135us,
    or 0.15cm/us * 135us = 20cm constant.
   */
  return((20 +                        /* transmission offset */
         echo * (US_PER_CELL * 0.15)) /* time of flight */
         / 2);                        /* because it's reflected */
}

void loop() {
  int16_t distance;
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  distance = range(8);
  //lcd.setCursor(0, 1);
  //lcd.print("      cm");
  //lcd.setCursor(0, 1);
  //lcd.print(distance);
  Serial.print(distance);
  Serial.print("cm\r\n");
  delay(1000);
}

