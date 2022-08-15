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
#define NUM_COLORS 4

const CRGBPalette16 normal = CRGBPalette16(CRGB::Black, CRGB::Red, CRGB::Orange, CRGB::Yellow);
const CRGBPalette16 green = CRGBPalette16(CRGB::Black, CRGB::LimeGreen, CRGB::Green,  CRGB::ForestGreen);
const CRGBPalette16 blue = CRGBPalette16(CRGB::Black, CRGB::Blue, CRGB::Aqua,  CRGB::White);
const CRGBPalette16 red = CRGBPalette16(CRGB::Black, CRGB::DarkRed, CRGB::Red, CRGB::DarkRed);
const CRGBPalette16 palettes[] = { normal, green, blue, red };

//  Don't touch
CRGB leds[NUM_LEDS];
bool btnPressed = false;
int currentColor = 0;
int btnTimeout = 25;
int currentTimeout = 0;

void onBtnPress();
void btnRead();
void fire();

void setup() {
  delay(500);
  Serial.begin(9600);
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  pinMode(BTN_PIN, INPUT);
}

void loop() {
  random16_add_entropy(random());

  if (currentTimeout == 0) {
    btnRead();
  } else {
    currentTimeout--;
  }
  
  fire();

  FastLED.show();
  FastLED.delay(FRAME_DELAY);
}

void btnRead() {
  int readValue = digitalRead(BTN_PIN);
  Serial.println(readValue);

  if (readValue == HIGH && !btnPressed) {
    btnPressed = true;
    onBtnPress();
  } else if (readValue == LOW && btnPressed) {
    btnPressed = false;
    currentTimeout = btnTimeout;
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