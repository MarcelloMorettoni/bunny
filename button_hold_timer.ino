#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <stdlib.h>

// Display configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Button configuration
const uint8_t BUTTON_PIN = 2;           // Use a momentary button wired to ground
const unsigned long DISPLAY_UPDATE_MS = 100;  // Update cadence while holding

unsigned long holdStart = 0;
unsigned long lastUpdate = 0;
bool holding = false;

void showHoldTime(float secondsHeld);

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    for (;;) {
      // If the display fails to initialize, halt here.
    }
  }

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Hold Btn");
  display.setTextSize(1);
  display.println("Seconds:");
  display.display();
}

void loop() {
  bool pressed = digitalRead(BUTTON_PIN) == LOW;
  unsigned long now = millis();

  if (pressed && !holding) {
    holding = true;
    holdStart = now;
    lastUpdate = 0;  // Force immediate refresh
  } else if (!pressed && holding) {
    holding = false;
    showHoldTime(0.0f);
  }

  if (holding && (now - lastUpdate >= DISPLAY_UPDATE_MS)) {
    float secondsHeld = (now - holdStart) / 1000.0f;
    showHoldTime(secondsHeld);
    lastUpdate = now;
  }
}

void showHoldTime(float secondsHeld) {
  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Holding");

  display.setTextSize(2);
  display.setCursor(0, 32);
  char buffer[10];
  dtostrf(secondsHeld, 4, 1, buffer);
  display.print(buffer);
  display.println("s");

  display.display();
}
