/*
  free_ranger demo code.
  
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
  ranger comp-: 7
  ranger comp+: 6

 */

#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup(void)
{
  pinMode(9, OUTPUT);
  analogWrite(9, 50);  /* Contrast control.  Lower voltage = higher contrast */
  lcd.begin(8, 2);
  lcd.print("range:");
  Serial.begin(9600);
  Serial.print("Starting\r\n");
  delay(100);
}

#include <Arduino.h>
#include <util/delay.h>

int16_t range(uint8_t arduino_pin)
{
  uint8_t port, bm, sreg;
  volatile uint8_t * ddr, * out;
#define NR_CELLS 40
  uint8_t cells[NR_CELLS], tcnt, c;

  /*
  Pos input to comparator MUST be AIN0 = OC0A/PD6/nano6
  Let Neg input be the default, AIN1 = PD7/nano7
   */
#define OUTPUT_HIGH *out |= bm
#define OUTPUT_LOW *out &= ~bm

  port = digitalPinToPort(arduino_pin);
  bm = digitalPinToBitMask(arduino_pin);
  out = portOutputRegister(port);
  OUTPUT_LOW;
  ddr = portModeRegister(port);
  *ddr |= bm;

  sreg = SREG;
  cli();
  
  /*
  Initialise comparator
   */
  ADCSRB &= ~_BV(ACME);               /* disable Neg input mux */
  ACSR = _BV(ACI)                     /* clear ACI */
       | _BV(ACIS0) | _BV(ACIS1);     /* Triggers on rising edge */
       
#define ON_TIME 12
#define OFF_TIME (24-ON_TIME)
  OUTPUT_HIGH; _delay_us(ON_TIME);
  OUTPUT_LOW;  _delay_us(OFF_TIME);
  OUTPUT_HIGH; _delay_us(ON_TIME);
  OUTPUT_LOW;  _delay_us(OFF_TIME);
  *ddr &= ~bm;
  ACSR |= _BV(ACI);

  /*
  On ATmega328 Arduinos, timer 0 is configured to run at 4us per tick.
  We use this to time our loops.
  Each cell is 160us long (40 ticks), which is 24cm one way or
  12cm reflected.
   */
  tcnt = TCNT0;
  for(c = 0; c < NR_CELLS; c++){
    tcnt += 40;
    for(cells[c] = 0; TCNT0 != tcnt; )
      if (ACSR & _BV(ACI) ){
        cells[c]++;
        ACSR |= _BV(ACI);
      }
  }
  
  SREG = sreg;
  
  for(c = 0; c < NR_CELLS; c++){
    Serial.print(cells[c]);
    Serial.print(" ");
  }
  Serial.print("\r\n");
  
  /*
  Coarse resolution (get the cell number):
  Look for first run of THRESHOLD or more bits, past the 600us mark (bypass the ringing).
   */
#define THRESHOLD 7
  for(c = 5; c < sizeof(cells)-1; c++){
    uint8_t bits;
    bits = cells[c] + cells[c+1];
    if( bits >= THRESHOLD )break;
  }

  /*
  Fine resolution: 6.4 cycles per cell of 12cm.
  (15/8 == 12/6.4)
   */
  return(7                                   /* 2 cycles transmitted */
         + (c*12)                            /* Coarse resolution */
         + 12 - (cells[c] * 15)/8);          /* Fine res adjustment */
}

void loop() {
  int16_t distance;
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  distance = range(8);
  lcd.setCursor(0, 1);
  lcd.print("      cm");
  lcd.setCursor(0, 1);
  lcd.print(distance);
  Serial.print(distance);
  Serial.print("cm\r\n");
  delay(1000);
}

