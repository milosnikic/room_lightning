#include <Arduino_FreeRTOS.h>
#include <FastLED.h>
#define PSH_BTN_1 4
#define PSH_BTN_2 5
#define PSH_BTN_3 6
#define DATA_PIN     3
#define NUM_LEDS    106
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

unsigned char colors[50][3] = {
  {220, 20, 60},
  {255, 0, 0},
  {255, 99, 71},
  {255, 127, 80},
  {255, 140, 0},
  {255, 165, 0},
  {255, 215, 0},
  {184, 134, 11},
  {240, 230, 140},
  {154, 205, 50},
  {107, 142, 35},
  {124, 252, 0},
  {0, 255, 0},
  {50, 205, 50},
  {152, 251, 152},
  {46, 139, 87},
  {60, 179, 113},
  {47, 79, 79},
  {0, 139, 139},
  {0, 255, 255},
  {224, 255, 255},
  {176, 224, 230},
  {70, 130, 180},
  {30, 144, 255},
  {135, 206, 235},
  {25, 25, 112},
  {0, 0, 139},
  {65, 105, 225},
  {75, 0, 130},
  {106, 90, 205},
  {147, 112, 219},
  {148, 0, 211},
  {128, 0, 128},
  {238, 130, 238},
  {199, 21, 133},
  {255, 105, 180},
  {245, 245, 220},
  {255, 235, 205}
};



TaskHandle_t Handle_pushButtons;
TaskHandle_t Handle_ledTapeShining;

void checkIfButtonIsPressedWithDebounce( void *pvParameters );
void shineLedTape(void * pvParameters);

void setup() {
  pinMode(PSH_BTN_1, INPUT);
  pinMode(PSH_BTN_2, INPUT);
  pinMode(PSH_BTN_3, INPUT);
  digitalWrite(PSH_BTN_1, HIGH);
  digitalWrite(PSH_BTN_2, HIGH);
  digitalWrite(PSH_BTN_3, HIGH);
  Serial.begin(9600);
  FastLED.addLeds<WS2812B, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  xTaskCreate(shineLedTape, "shineLedTape", 128, NULL, 1, &Handle_ledTapeShining);
  xTaskCreate(checkIfButtonIsPressedWithDebounce, "task1", 64, (void *) PSH_BTN_1, 1, &Handle_pushButtons);
  xTaskCreate(checkIfButtonIsPressedWithDebounce, "task2", 64, (void *) PSH_BTN_2, 1, &Handle_pushButtons);
  xTaskCreate(checkIfButtonIsPressedWithDebounce, "task3", 64, (void *) PSH_BTN_3, 1, &Handle_pushButtons);
  vTaskStartScheduler();
}

void loop() {}

void checkIfButtonIsPressedWithDebounce(void * pvParameters) {
  unsigned char selectedColor = 0;
  unsigned char selectedMode = 4;
  Serial.println("button entered");
  unsigned long lastDebounceTime = 0;
  unsigned long debounceDelay = 100;
  int buttonState;
  int lastButtonState = LOW;
  while (1) {
    int reading = digitalRead(( unsigned char ) pvParameters);
    if (reading != lastButtonState) {
      lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {
      if (reading != buttonState) {
        buttonState = reading;
        if (reading == LOW) {
          switch (( uint32_t ) pvParameters) {
            case 4:
              changeSelectedMode();
              Serial.println("Current mode: " + String(selectedMode));
              break;
            case 5:
              if ( isColorChangable() ) {
                selectPreviousColor();
                Serial.println("Selected color: " + String(selectedColor));
              }
              break;
            case 6:
              if ( isColorChangable() ) {
                selectNextColor();
                Serial.println("Selected color: " + String(selectedColor));
              }
              break;
          }
        }
      }
    }
    lastButtonState = reading;
  }
}

void shineLedTape(void * pvParameters) {
  unsigned char selectedColor = 15;
  unsigned char selectedMode = 4;
  Serial.println("shine led entered");
  while (1) {
    switch (selectedMode) {
      case 0:
        turnLedTapeOff();
        break;
      case 1:
        shineOneByOne(colors[selectedColor][0], colors[selectedColor][1], colors[selectedColor][2]);
        vTaskDelay( 200 / portTICK_PERIOD_MS );
        turnLedTapeOff();
        break;
      case 2:
        shineOneByOneReverse(colors[selectedColor][0], colors[selectedColor][1], colors[selectedColor][2]);
        vTaskDelay( 200 / portTICK_PERIOD_MS );
        turnLedTapeOff();
        break;
      case 3:
        shineOneColor(colors[selectedColor][0], colors[selectedColor][1], colors[selectedColor][2]);
        break;
      case 4:
        cycleThroughColors();
        break;
      case 5:
        fadeColor(colors[selectedColor][0], colors[selectedColor][1], colors[selectedColor][2]);
        break;
      case 6:
        runningLine(5, 10, colors[selectedColor][0], colors[selectedColor][1], colors[selectedColor][2]);
        break;
    }
  }
}

bool isColorChangable() {
  unsigned char selectedColor = 0;
  unsigned char selectedMode = 4;
  return selectedMode == 1 || selectedMode == 2 || selectedMode == 3 || selectedMode == 5 || selectedMode == 6;
}

void changeSelectedMode() {
  unsigned char selectedColor = 0;
  unsigned char selectedMode = 6;
  ++selectedMode %= 7;
}

void selectNextColor() {
  unsigned char selectedColor = 0;
  unsigned char selectedMode = 6;
  selectedColor++;
  if (selectedColor == 39) {
    selectedColor = 0;
  }
}

void selectPreviousColor() {
  unsigned char selectedColor = 0;
  unsigned char selectedMode = 6;
  if (selectedColor == 0) {
    selectedColor = 38;
  } else {
    selectedColor--;
  }
}

void shineOneByOne(unsigned char red, unsigned char green, unsigned char blue) {
  for (unsigned char i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB( red, green, blue);
    vTaskDelay( 20 / portTICK_PERIOD_MS );
    FastLED.show();
  }
}

void shineOneByOneReverse(unsigned char red, unsigned char green, unsigned char blue) {
  for (signed char i = NUM_LEDS; i >= 0; i--) {
    leds[i] = CRGB( red, green, blue);
    vTaskDelay( 20 / portTICK_PERIOD_MS );
    FastLED.show();
  }
}

void turnLedTapeOff() {
  shineOneColor(0, 0, 0);
}

void shineOneColor(unsigned char red, unsigned char green, unsigned char blue) {
  for (unsigned char i = 0; i < NUM_LEDS; i++) {
    leds[i] =  CRGB(red, green, blue);
  }
  FastLED.show();
}

void cycleThroughColors() {
  for (unsigned char color = 0; color < 39; color++) {
    fadeColor(colors[color][0], colors[color][1], colors[color][2]);
  }
}

void fadeColor(unsigned char red, unsigned char green, unsigned char blue) {
  for (unsigned char brightness = 0; brightness < 220; brightness += 5) {
    FastLED.setBrightness(brightness);
    shineOneColor(red, green, blue);
    vTaskDelay( 50 / portTICK_PERIOD_MS );
  }
  vTaskDelay( 1000 / portTICK_PERIOD_MS );
  for (unsigned char brightness = 220; brightness >= 0; brightness -= 5) {
    FastLED.setBrightness(brightness);
    shineOneColor(red, green, blue);
    vTaskDelay( 50 / portTICK_PERIOD_MS );
  }
  vTaskDelay( 10200 / portTICK_PERIOD_MS );
}

void runningLine(unsigned char lineLength, short delayDuration, unsigned char red, unsigned char green, unsigned char blue) {
  for (unsigned char i = 0; i < NUM_LEDS + lineLength; i++) {
    for (unsigned char j = 0; j < lineLength; j++) {
      if (i >= j &&  i - j < NUM_LEDS) {
        leds[i - j] = CRGB(red, green, blue);
      }
    }
    FastLED.show();
    vTaskDelay( delayDuration / portTICK_PERIOD_MS );
    turnLedTapeOff();
  }
}
