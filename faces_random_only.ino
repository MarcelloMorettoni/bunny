#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Display setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Reference eye geometry
static const int ref_eye_height = 40;
static const int ref_eye_width = 40;
static const int ref_space_between_eye = 10;
static const int ref_corner_radius = 10;

// Current eye state
int left_eye_height = ref_eye_height;
int left_eye_width  = ref_eye_width;
int left_eye_x      = 32;
int left_eye_y      = 32;
int right_eye_x     = 32 + ref_eye_width + ref_space_between_eye;
int right_eye_y     = 32;
int right_eye_height = ref_eye_height;
int right_eye_width  = ref_eye_width;

// Animation control
bool randomMode = true;           // corresponds to previous "demo_mode = 2"
unsigned long lastRandomChange = 0;
const unsigned long RANDOM_INTERVAL_MS = 4000;

void drawEyes()
{
  display.clearDisplay();

  int x = left_eye_x - left_eye_width / 2;
  int y = left_eye_y - left_eye_height / 2;
  display.fillRoundRect(x, y, left_eye_width, left_eye_height, ref_corner_radius, SSD1306_WHITE);

  x = right_eye_x - right_eye_width / 2;
  y = right_eye_y - right_eye_height / 2;
  display.fillRoundRect(x, y, right_eye_width, right_eye_height, ref_corner_radius, SSD1306_WHITE);

  display.display();
}

void centerEyes()
{
  left_eye_height  = ref_eye_height;
  left_eye_width   = ref_eye_width;
  right_eye_height = ref_eye_height;
  right_eye_width  = ref_eye_width;

  left_eye_x  = SCREEN_WIDTH / 2 - ref_eye_width / 2 - ref_space_between_eye / 2;
  left_eye_y  = SCREEN_HEIGHT / 2;
  right_eye_x = SCREEN_WIDTH / 2 + ref_eye_width / 2 + ref_space_between_eye / 2;
  right_eye_y = SCREEN_HEIGHT / 2;

  drawEyes();
}

void blink(int speed)
{
  centerEyes();
  for (int i = 0; i < 3; ++i) {
    left_eye_height  -= speed;
    right_eye_height -= speed;
    drawEyes();
    delay(15);
  }
  for (int i = 0; i < 3; ++i) {
    left_eye_height  += speed;
    right_eye_height += speed;
    drawEyes();
    delay(15);
  }
}

void happyEye()
{
  centerEyes();
  int offset = ref_eye_height / 2;
  for (int i = 0; i < 10; ++i) {
    display.fillTriangle(left_eye_x - left_eye_width / 2 - 1, left_eye_y + offset,
                         left_eye_x + left_eye_width / 2 + 1, left_eye_y + 5 + offset,
                         left_eye_x - left_eye_width / 2 - 1, left_eye_y + left_eye_height + offset,
                         SSD1306_BLACK);

    display.fillTriangle(right_eye_x + right_eye_width / 2 + 1, right_eye_y + offset,
                         right_eye_x - right_eye_width / 2 - 1, right_eye_y + 5 + offset,
                         right_eye_x + right_eye_width / 2 + 1, right_eye_y + right_eye_height + offset,
                         SSD1306_BLACK);
    offset -= 2;
    display.display();
    delay(15);
  }
  delay(1000);
}

void sadEye()
{
  centerEyes();
  int offset = ref_eye_height / 2;
  for (int i = 0; i < 10; ++i) {
    display.fillTriangle(left_eye_x - left_eye_width / 2 - 1, left_eye_y - offset,
                         left_eye_x + left_eye_width / 2 + 1, left_eye_y - 5 - offset,
                         left_eye_x - left_eye_width / 2 - 1, left_eye_y - 10 - offset,
                         SSD1306_BLACK);

    display.fillTriangle(right_eye_x + right_eye_width / 2 + 1, right_eye_y - offset,
                         right_eye_x - right_eye_width / 2 - 1, right_eye_y - 5 - offset,
                         right_eye_x + right_eye_width / 2 + 1, right_eye_y - 10 - offset,
                         SSD1306_BLACK);

    display.drawLine(left_eye_x - left_eye_width / 2 + 2, left_eye_y + left_eye_height / 2,
                     left_eye_x + left_eye_width / 2 - 2, left_eye_y + left_eye_height / 2,
                     SSD1306_BLACK);

    display.drawLine(right_eye_x - right_eye_width / 2 + 2, right_eye_y + right_eye_height / 2,
                     right_eye_x + right_eye_width / 2 - 2, right_eye_y + right_eye_height / 2,
                     SSD1306_BLACK);
    offset -= 1;
    display.display();
    delay(15);
  }
  delay(1000);
}

void angryEye()
{
  centerEyes();
  int offset = ref_eye_height / 2;
  for (int i = 0; i < 10; ++i) {
    display.fillTriangle(left_eye_x - left_eye_width / 2 - 1, left_eye_y - 10 - offset,
                         left_eye_x + left_eye_width / 2 + 1, left_eye_y - offset,
                         left_eye_x + left_eye_width / 2 + 1, left_eye_y - 10 - offset,
                         SSD1306_BLACK);

    display.fillTriangle(right_eye_x + right_eye_width / 2 + 1, right_eye_y - 10 - offset,
                         right_eye_x - right_eye_width / 2 - 1, right_eye_y - offset,
                         right_eye_x - right_eye_width / 2 - 1, right_eye_y - 10 - offset,
                         SSD1306_BLACK);
    offset -= 1;
    display.display();
    delay(15);
  }
  delay(1000);
}

void tiredEye()
{
  centerEyes();
  int offset = ref_eye_height / 2;
  for (int i = 0; i < 10; ++i) {
    display.fillRect(left_eye_x - left_eye_width / 2 - 1, left_eye_y - offset,
                     left_eye_width + 2, left_eye_height / 3, SSD1306_BLACK);

    display.fillRect(right_eye_x - right_eye_width / 2 - 1, right_eye_y - offset,
                     right_eye_width + 2, right_eye_height / 3, SSD1306_BLACK);

    display.drawLine(left_eye_x - left_eye_width / 2 + 2, left_eye_y + left_eye_height / 2 + 2,
                     left_eye_x - left_eye_width / 2 + 8, left_eye_y + left_eye_height / 2 + 2,
                     SSD1306_BLACK);

    display.drawLine(right_eye_x - right_eye_width / 2 + 2, right_eye_y + right_eye_height / 2 + 2,
                     right_eye_x - right_eye_width / 2 + 8, right_eye_y + right_eye_height / 2 + 2,
                     SSD1306_BLACK);
    offset -= 1;
    display.display();
    delay(15);
  }
  delay(1000);
}


void heart_eye() {
  centerEyes();
  int size = 6; // heart size
  int steps = 5; // animation smoothness

  // optional: clear old shapes to avoid mix
  display.clearDisplay();
  display.display();

  for (int i = 0; i < steps; i++) {
    // fade-in or build hearts gradually
    int grow = i; // or use sin/cos for pulse later

    int lx = left_eye_x;
    int ly = left_eye_y;
    display.fillCircle(lx - size/2 - grow, ly - size/2 - grow, size/2 + grow, SSD1306_WHITE);
    display.fillCircle(lx + size/2 + grow, ly - size/2 - grow, size/2 + grow, SSD1306_WHITE);
    display.fillTriangle(lx - size - 1 - grow, ly - size/4 - grow,
                         lx + size + 1 + grow, ly - size/4 - grow,
                         lx, ly + size + 2 + grow, SSD1306_WHITE);

    int rx = right_eye_x;
    int ry = right_eye_y;
    display.fillCircle(rx - size/2 - grow, ry - size/2 - grow, size/2 + grow, SSD1306_WHITE);
    display.fillCircle(rx + size/2 + grow, ry - size/2 - grow, size/2 + grow, SSD1306_WHITE);
    display.fillTriangle(rx - size - 1 - grow, ry - size/4 - grow,
                         rx + size + 1 + grow, ry - size/4 - grow,
                         rx, ry + size + 2 + grow, SSD1306_WHITE);

    display.display();
    delay(60);
  }

  delay(1200);  // hold hearts visible
}


void centerPose()
{
  centerEyes();
  delay(1000);
}

void blinkSlow()
{
  blink(8);
  delay(800);
}

void blinkFast()
{
  blink(14);
  delay(800);
}

typedef void (*ExpressionFn)();

ExpressionFn expressions[] = {
  centerPose,
  blinkSlow,
  blinkFast,
  happyEye,
  sadEye,
  angryEye,
  tiredEye,
  heart_eye
};

const uint8_t EXPRESSION_COUNT = sizeof(expressions) / sizeof(expressions[0]);
int currentExpression = 2;  // start aligned with the previous "random (2)" state

void playCurrentExpression()
{
  expressions[currentExpression]();
}

void playRandomExpression()
{
  currentExpression = random(EXPRESSION_COUNT);
  playCurrentExpression();
}

void handleSerial()
{
  while (Serial.available()) {
    char c = Serial.read();
    if (c == 'o' || c == 'O') {
      randomMode = false;
      currentExpression = (currentExpression + 1) % EXPRESSION_COUNT;
      playCurrentExpression();
    } else if (c == 'r' || c == 'R') {
      randomMode = true;
      playRandomExpression();
      lastRandomChange = millis();
    }
  }
}

void setup()
{
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    for (;;)
      ; // halt if the display is not found
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("Bunny ready"));
  display.display();
  delay(1000);

  randomSeed(analogRead(A0));

  centerEyes();
  playCurrentExpression();
  lastRandomChange = millis();
}

void loop()
{
  handleSerial();

  if (randomMode) {
    unsigned long now = millis();
    if (now - lastRandomChange >= RANDOM_INTERVAL_MS) {
      playRandomExpression();
      lastRandomChange = now;
    }
  }
}
