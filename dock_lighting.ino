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

Adafruit_TLC5947 ledout(1, SCK_PIN, LEDOUT_MOSI_PIN, LEDOUT_XLAT_PIN);

//bool out_bit_cache[LEDOUT_BITS];

void setup() {
  pinMode(LEDOUT_MOSI_PIN, OUTPUT);
  pinMode(LEDOUT_MISO_PIN, INPUT);
  pinMode(LEDOUT_XLAT_PIN, OUTPUT);
  pinMode(LEDOUT_BLANK_OD_PIN, OUTPUT);
  pinMode(BRIGHTNESS_POT_PIN, INPUT);
  pinMode(BLINKINESS_POT_PIN, INPUT);
  pinMode(SCK_PIN, OUTPUT);
  
  ledout.begin();
  digitalWrite(LEDOUT_BLANK_OD_PIN, 0);
}

int ticker = 0;

void loop() {
  delay(/*ms=*/1000);

  bool on = ticker++ % 2 == 0;
  
  /*for (int led = 0; led < LEDOUT_BITS; led++) {
    if (out_bit_cache[led] != cmri.get_bit(led)) {
      out_bit_cache[led] = cmri.get_bit(led);
      rewrite_leds = true;
    }
  }*/

  //if (rewrite_leds) {
    for (int led = 0; led < LEDOUT_BITS; led++) {
      ledout.setPWM(led, on ? 4000 : 0);
    }
    ledout.write();
  //}
}
