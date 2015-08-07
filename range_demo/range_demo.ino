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

#define NR_CELLS 40
#define TICKS_PER_CELL 128
#define US_PER_TICK 0.75
#define US_PER_CELL (TICKS_PER_CELL * US_PER_TICK)

void setup(void)
{
  Serial.begin(9600);
  _delay_ms(1000); /* Let everything settle */
  if (dither_calibrate()) {
    Serial.print("Calibrate error\r\n");
    for( ; ; );
  }
  Serial.print("\r\ndither_duty ");
  Serial.print(dither_duty);
  dither_start();
  Serial.print("\r\nnr_cells ");
  Serial.println(NR_CELLS);
  Serial.print("us_per_cell ");
  Serial.println(US_PER_CELL);
  delay(100);
}

static inline void pulse(int txpin)
{
  digitalWrite(txpin, HIGH);
  _delay_us(11);                      /* digitalWrite() very slow */
  digitalWrite(txpin, LOW);
  _delay_us(11);
}

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

uint8_t cells[2][NR_CELLS];

void loop()
{
  static uint8_t current_row = 0;
  uint8_t i;
  
  current_row = (current_row?0:1);  
  range_sounding(8, cells[current_row], NR_CELLS);

  /*
    De-noise: take the square-root of the product of two soundings.  This should
    remove much of the surface reflections.  Then find the cell with the largest
    positive increase in echo strength.
   */

  int16_t max_diff = 0;
  uint8_t cellno = NR_CELLS-1;
  int16_t w[3] = {127,127,127};

  for(i = 0; i < NR_CELLS; i++) {
    Serial.print(cells[current_row][i]);
    Serial.print(' ');
    w[0] = w[1]; w[1] = w[2];
    if (i < NR_CELLS-1)
      w[2] = sqrt(cells[0][i+1] * cells[1][i+1]);
    else
      w[2] = 0;
    //Serial.print(w[0]); Serial.print(' ');
    //Serial.print(w[1]); Serial.print(' ');
    //Serial.println(w[2]);
    int16_t d = (w[2] + w[1])/2 - w[0];
    if (d+(i/8) > max_diff) {
      max_diff = d+(i/8);
      cellno = i;
    }
  }

  //Serial.print(w[0]); Serial.print(' ');
  //Serial.print(w[1]); Serial.print(' ');
  //Serial.println(w[2]);
  
  Serial.print("\r\nCell ");
  Serial.print(cellno);
  Serial.print("\r\ndistance ");
  Serial.print((cellno * (US_PER_CELL*0.15))/2 + 20);
  Serial.print(" cm\r\n");
  //Serial.println(max_diff);

  delay(1000);
}
