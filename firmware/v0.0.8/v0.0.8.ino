#include <FastLED.h>

FASTLED_USING_NAMESPACE

#define DATA_PIN    7
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
#define NUM_LEDS    30
CRGB leds[NUM_LEDS];

#define BRIGHTNESS  200

void setup() {
  delay(1000);
  Serial.begin(9600);
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);


}


void loop()
{
	
  for (int i = 1; i < NUM_LEDS; i++) {
    leds[i] = CRGB(245, 206, 255);
  }

  FastLED.show();

}