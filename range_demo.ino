/*
  LiquidCrystal Library - Hello World
 
 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the 
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.
 
 This sketch prints "Hello World!" to the LCD
 and shows the time.
 
  The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 
 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe
 
 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/LiquidCrystal
 */

// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  pinMode(9, OUTPUT);
  analogWrite(9, 50);  /* Contrast control.  Lower voltage = higher contrast */
  // set up the LCD's number of columns and rows: 
  lcd.begin(8, 2);
  // Print a message to the LCD.
  lcd.print("range:");
  Serial.begin(38400);
  Serial.print("ok\r\n");
  delay(100);
}

#include <Arduino.h>
#include <util/delay.h>

uint8_t range(uint8_t arduino_pin)
{
  uint8_t port, bm;
  volatile uint8_t * ddr, * out;
  uint8_t buckets[32];
  uint16_t u;

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

  /*
  Initialise comparator
   */
  ADCSRB &= ~_BV(ACME);               /* disable Neg input mux */
  ACSR = _BV(ACI)                     /* clear ACI */
       | _BV(ACIS0) | _BV(ACIS1);     /* Triggers on rising edge */

  for(u = 0; u < sizeof(buckets); u++)buckets[u] = 0;
  
  cli();
  
  OUTPUT_HIGH; _delay_us(12);
  OUTPUT_LOW;  _delay_us(12);
  OUTPUT_HIGH; _delay_us(12);
  OUTPUT_LOW;  _delay_us(12);
  *ddr &= ~bm;
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
    uint8_t b;
    b = (ACSR & _BV(ACI));
    b >>= ACI;
    ACSR |= _BV(ACI);
    buckets[u/LOOPS_PER_BUCKET] += b;
  }
  
  sei();
  
  for(u = 0; u < sizeof(buckets); u++){
    Serial.print(buckets[u]);
    Serial.print(" ");
  }
  Serial.print("\r\n");
  
  /*
  Look for first run of 8 or more bits, past the 600us mark.
   */
  for(u = 6; u < sizeof(buckets)-2; u++){
    uint8_t bits;
    bits = buckets[u] + buckets[u+1] + buckets[u+2];
    if( bits >= 8 )return((int8_t)u);
  }

  return(-1);
}

void loop() {
  uint8_t distance;
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  lcd.print("     ");
  lcd.setCursor(0,1);
  distance = range(8);
  lcd.print(distance);
  delay(1000);
}

