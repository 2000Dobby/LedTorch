#include <Arduino.h>
#include <FastLED.h>

//  Config
#define LED_PIN 2
#define BTN_PIN 3
#define COLOR_ORDER GRB
#define CHIPSET WS2812B
#define NUM_LEDS 30
#define BRIGHTNESS 200
#define FRAME_DELAY 20
#define REVERSE false

#define COOLING 65
#define SPARKING 130
#define NUM_COLORS 3

const CRGBPalette16 palettes[] = { normal, green, blue };
const CRGBPalette16 normal = CRGBPalette16(CRGB::Black, CRGB::Red, CRGB::Orange, CRGB::Yellow);
const CRGBPalette16 green = CRGBPalette16(CRGB::Black, CRGB::Green, CRGB::Aqua,  CRGB::White);
const CRGBPalette16 blue = CRGBPalette16(CRGB::Black, CRGB::Blue, CRGB::Aqua,  CRGB::White);

//  Don't touch
CRGB leds[NUM_LEDS];
bool btnPressed = false;
int currentColor = 0;


void setup() {
  delay(500);
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  pinMode(BTN_PIN, INPUT);
}

void loop() {
  random16_add_entropy(random());

  btnRead();
  fire();

  FastLED.show();
  FastLED.delay(FRAME_DELAY);
}

void btnRead() {
  int readValue = digitalRead(BTN_PIN);

  if (readValue == HIGH && !btnPressed) {
    btnPressed = true;
    onBtnPress();
  } else if (readValue == LOW && btnPressed) {
    btnPressed = false;
    delay(180);
  }
}

void onBtnPress() {
  currentColor = (currentColor + 1) % NUM_COLORS;
}

void fire() {
  static byte heat[NUM_LEDS];

  for (int i = 0; i < NUM_LEDS; i++) {
    heat[i] = qsub8(heat[i], random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
  }

  for (int i = NUM_LEDS - 1; i >= 2; i--) {
    heat[i] = (heat[i - 1] + heat[i - 2] + heat[i - 2] ) / 3;
  }

  if (random8() < SPARKING) {
    int pos = random8(7);
    heat[pos] = qadd8(heat[pos], random8(160, 255));
  }

  for (int i = 0; i < NUM_LEDS; i++) {
    byte colorIndex = scale8(heat[i], 240);
    CRGB color = ColorFromPalette(palettes[currentColor], colorIndex);

    int pixel = REVERSE ? NUM_LEDS - i - 1 : i; 
    leds[pixel] = color;
  }
}