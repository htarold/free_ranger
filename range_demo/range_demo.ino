/*
  free_ranger demo code.  Code compiled and ran for nano V3.
 
  Arduino v1.6.4
 Pins:
 ranger comp- (signal): 7
 ranger comp+ (dither): 6
 Ranger TX pin: 8
 Timer 2 dither output pin: 11
   (connect a 6k8 from 11 to 6; and 10n from 11 to ground)
 
 */

#include <LiquidCrystal.h>

//LiquidCrystal lcd(12, 11, 5, 4, 3, 2);


//#include <Arduino.h>
#include <util/delay.h>

uint8_t dither_duty;
/*
  To use dithering, there must be a 10k/10n LPF between
 pin 11/PB3/OC2A and pin 6/AIN1.
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
    if (i > 250) { 
      dither_stop(); 
      return(-1); 
    }
    dither_duty--;
    dither_start();
    _delay_us(10);                    /* stabilise */
  }
  dither_stop();
  //dither_duty -= dither_duty/8;       /* fudge a bit */
  return(0);
}

void setup(void)
{
  Serial.begin(9600);
  _delay_ms(1000); /* Let everything settle */
  if (dither_calibrate()) {
    Serial.print("Calibrate error\r\n");
    for( ; ; );
  }
  Serial.print("\r\nDither duty = ");
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

void range_sounding(uint8_t arduino_pin, uint8_t cells[], int8_t nr_cells)
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

#define TICKS_PER_CELL 128
#define US_PER_TICK 0.75
#define US_PER_CELL (TICKS_PER_CELL * US_PER_TICK)

  for(c = 0; c < nr_cells; c++){
    cells[c] = 0;
    for(uint8_t ticks = 0; ticks < TICKS_PER_CELL; ticks++){
      uint8_t src, dst;
      /*
        This bit compiles differently depending on the compiler, so
        it is spelled out explicitly so we can depend on the timing.
        The code is basically:
        cells[c] += ((ACSR & _BV(ACO)>>ACO;
        The following assembles to:
 2a4:   90 b7           in      r25, 0x30       ; 48
 2a6:   95 fb           bst     r25, 5
 2a8:   20 f9           bld     r18, 0
 2aa:   90 81           ld      r25, Z
 2ac:   92 0f           add     r25, r18
 2ae:   90 83           st      Z, r25
 2b0:   8f 5f           subi    r24, 0xFF       ; 255
 2b2:   80 38           cpi     r24, 0x80       ; 128
 2b4:   b9 f7           brne    .-18            ; 0x2a4 <_Z5rangeh+0x58>
       12 clocks = 0.75us
       */
      asm volatile("in %0, %2" "\n\t"
                   "bst %0, %3" "\n\t"
  	           "bld %1, %4" "\n\t" :
  	           "=r"(src), "=r"(dst)  :
  	           "I"(_SFR_IO_ADDR(ACSR)), "I"(5), "L"(0));
      cells[c] += dst;
    }
  }

  SREG = sreg;
}

#define NR_CELLS 32
uint8_t cells[2][NR_CELLS];

void loop()
{
  static uint8_t current_row = 0;
  uint8_t i;
  
  range_sounding(8, cells[current_row], NR_CELLS);

  /*
    De-noise: take the square-root of the product of two soundings.  This should
    remove much of the surface reflections.  Then find the cell with the largest
    positive increase in echo strength.
   */

  int8_t max_diff = 0, cellno = -1;
  uint8_t prev_val = 0;
  
  for(i = 0; i < NR_CELLS; i++){
    uint16_t product = cells[0][i] * cells[1][i];
    uint8_t val = (uint8_t)sqrt(product);
    int8_t d = val - prev_val;
    if (d > max_diff) {
      max_diff = d;
      cellno = i;
    }
    Serial.print(val);
    Serial.print(' ');
  }
  Serial.print("\r\n");

  if (cellno > -1)  {
    Serial.print("Cell ");
    Serial.print(cellno);
    Serial.print(": ");
    Serial.print((cellno * (US_PER_CELL*0.15))/2 + 20);
  } else
    Serial.print(-1);
  Serial.print(" cm\r\n");
  
  current_row = (current_row?0:1);
  delay(1000);
}


