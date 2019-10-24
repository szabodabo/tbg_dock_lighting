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

  randomSeed(analogRead(UNCONNECTED_ANALOG_PIN));

  // Turn on all the outputs.
  buzz(/*enable_effect=*/false, 0);
}

// Before setting fuses to run clock at 8MHz, the serial write was *very* slow,
// resulting in easily visible flicker when an LED is toggled on every run of loop().
// After using an 8MHz clock, the flicker is barely noticable, but still definitely there.
// So unnecessary write() calls should be avoided to avoid flickering of all lights.

// Using an external 16MHz resonator, use fuse setting -U lfuse:w:0xFF:m
// http://eleccelerator.com/fusecalc/fusecalc.php?chip=attiny44a&LOW=FF&HIGH=DF&EXTENDED=FF&LOCKBIT=FF


// Ratio of normal brightness to use as baseline brightness during buzzing
#define BUZZING_BRIGHTNESS_RATIO 0.1
#define BUZZ_PERIOD_RANDOMNESS_FACTOR 2.0

//#define BUZZ_WAIT_MINIMUM_SEC 5
//#define BUZZ_WAIT_MAXIMUM_SEC 10


// For testing only
#define BUZZ_WAIT_MINIMUM_MS 250
#define BUZZ_WAIT_MAXIMUM_MS 1000*30  // 30s
#define BUZZ_LEN_MINIMUM_MS 100
#define BUZZ_LEN_MAXIMUM_MS 7000
#define LED_FULL_BRIGHTNESS 2000

//#define ALL_LEDS_AFFECTED_FOR_TESTING

long randomInRange(long minimum, long maximum) {
  double randFrac = rand() / (float)RAND_MAX;
  float range = maximum - minimum;
  return (randFrac * range) + minimum;
}

uint32_t computeBuzzPeriodLenMs(double blinkiness) {  
  return randomInRange(BUZZ_LEN_MINIMUM_MS, BUZZ_LEN_MAXIMUM_MS);
}

double randZeroToOne() {
  return ((float) rand()) / (float) RAND_MAX;
}

uint32_t BUZZ_PERIOD_MS = 0;
long BUZZ_START_MILLIS = 0;
unsigned LED_FOR_EFFECT = 0;

void buzz(bool enable_effect, unsigned led_for_effect) {
  if (!enable_effect) {
    // Disable the effect by turning on all lights.
    for (int led = 0; led < LEDOUT_BITS; led++) {
      ledout.setPWM(led, LED_FULL_BRIGHTNESS);
    }
    ledout.write();
    return;
  }
  #ifdef ALL_LEDS_AFFECTED_FOR_TESTING
  int ledBeginIdx = 0;
  int ledEndIdx = LEDOUT_BITS;
  #else
  // FIXME WHEN ALL SOLDERED
  int ledBeginIdx = led_for_effect;
  //int ledBeginIdx = randomInRange(8, 15);
  int ledEndIdx = ledBeginIdx;
  #endif

  // Here's where we apply the buzzing effect.
  double newBrightness = randZeroToOne() * (float) LED_FULL_BRIGHTNESS;
  for (int led = ledBeginIdx; led <= ledEndIdx; led++) {
    ledout.setPWM(led, newBrightness);
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
    buzz(/*enable_effect=*/false, 0);

    // Decide when next buzz should start.
    BUZZ_START_MILLIS = millis() + GetMSBeforeNextBuzz();
  }

  if (millis() >= BUZZ_START_MILLIS && BUZZ_PERIOD_MS == 0) {
    // Next buzz starts now.
    BUZZ_PERIOD_MS = computeBuzzPeriodLenMs(blinkiness);
    LED_FOR_EFFECT = randomInRange(0, 23);
  }

  if (BUZZ_PERIOD_MS > 0 && millis() >= BUZZ_START_MILLIS) {
    // Buzz is happening now.
    buzz(/*enable_effect=*/true, LED_FOR_EFFECT);
  }
}
