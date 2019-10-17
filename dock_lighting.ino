// Find pinout here: https://www.instructables.com/id/Program-an-ATtiny44458485-with-Arduino/

#include <Adafruit_TLC5947.h>

#define LEDOUT_BITS 24

#define LEDOUT_MOSI_PIN 6 // PA6
#define LEDOUT_MISO_PIN 5 // PA5
#define LEDOUT_XLAT_PIN 7 // PA7
#define LEDOUT_BLANK_OD_PIN 3 // PA3
#define BRIGHTNESS_POT_PIN A1 // PA1
#define BLINKINESS_POT_PIN A2 // PA2
#define SCK_PIN 4 // PA4
#define UNCONNECTED_ANALOG_PIN A2

Adafruit_TLC5947 ledout(1, SCK_PIN, LEDOUT_MOSI_PIN, LEDOUT_XLAT_PIN);

void setup() {
  pinMode(LEDOUT_MOSI_PIN, OUTPUT);
  pinMode(LEDOUT_MISO_PIN, INPUT);
  pinMode(LEDOUT_XLAT_PIN, OUTPUT);
  pinMode(LEDOUT_BLANK_OD_PIN, OUTPUT);
  pinMode(BRIGHTNESS_POT_PIN, INPUT);
  pinMode(BLINKINESS_POT_PIN, INPUT);
  pinMode(SCK_PIN, OUTPUT);
  pinMode(UNCONNECTED_ANALOG_PIN, INPUT);
  
  ledout.begin();
  digitalWrite(LEDOUT_BLANK_OD_PIN, 0);

  //randomSeed(analogRead(UNCONNECTED_ANALOG_PIN));
}

int ticker = 0;

// Before setting fuses to run clock at 8MHz, the serial write was *very* slow,
// resulting in easily visible flicker when an LED is toggled on every run of loop().
// After using an 8MHz clock, the flicker is barely noticable, but still definitely there.
// So unnecessary write() calls should be avoided to avoid flickering of all lights.

// Using an external 16MHz resonator, use fuse setting -U lfuse:w:0xFF:m
// http://eleccelerator.com/fusecalc/fusecalc.php?chip=attiny44a&LOW=FF&HIGH=DF&EXTENDED=FF&LOCKBIT=FF


// Ratio of normal brightness to use as baseline brightness during buzzing
#define BUZZING_BRIGHTNESS_RATIO 0.3
#define BUZZ_PERIOD_RANDOMNESS_FACTOR 2.0

//#define BUZZ_WAIT_MINIMUM_SEC 5
//#define BUZZ_WAIT_MAXIMUM_SEC 10


// For testing only
#define BUZZ_WAIT_MINIMUM_MS 1000
#define BUZZ_WAIT_MAXIMUM_MS 1000
#define BUZZ_LEN_MINIMUM_MS 100
#define BUZZ_LEN_MAXIMUM_MS 7000

long randomInRange(long maximum, long minimum) {
  double randFrac = rand() / (float)RAND_MAX;
  float range = maximum - minimum;
  return (randFrac * range) + minimum;
}

uint32_t computeBuzzPeriodLenMs(double blinkiness) {  
  return randomInRange(BUZZ_LEN_MINIMUM_MS, BUZZ_LEN_MAXIMUM_MS);
}

uint32_t BUZZ_PERIOD_MS = 0;
long BUZZ_START_MILLIS = 0;

void rewriteLEDs(bool dim) {
 for (int led = 0; led < 24; led++) {
    ledout.setPWM(led, dim ? 100 : 4000);
  }
  ledout.write();
}

uint32_t GetMSBeforeNextBuzz() {
  return random(BUZZ_WAIT_MINIMUM_MS, BUZZ_WAIT_MAXIMUM_MS);
}

void loop() {
  // double brightness = (double)analogRead(BIGHTNESS_POT_PIN) / 1024.0;
  double brightness = 1.0;
  //double blinkiness = (double)analogRead(BLINKINESS_POT_PIN) / 1024.0;
  double blinkiness = 1.0;

  if (BUZZ_PERIOD_MS > 0 && millis() - BUZZ_START_MILLIS > BUZZ_PERIOD_MS) {
    // Buzz has just ended
    BUZZ_PERIOD_MS = 0;
    rewriteLEDs(/*dim=*/false);

    // Decide when next buzz should start.
    BUZZ_START_MILLIS = millis() + GetMSBeforeNextBuzz();
  }

  if (millis() >= BUZZ_START_MILLIS && BUZZ_PERIOD_MS == 0) {
    // Next buzz starts now.
    BUZZ_PERIOD_MS = computeBuzzPeriodLenMs(blinkiness);
    rewriteLEDs(/*dim=*/true);
  }
}
