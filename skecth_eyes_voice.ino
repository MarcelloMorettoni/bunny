#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DFRobot_DF2301Q.h"
//#include "pitches.h"
 

#define buzzer 11


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
DFRobot_DF2301Q_I2C asr;
//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI);
  //uint8_t w, uint8_t h, SPIClass *spi_ptr,
    //                               int8_t dc_pin, int8_t rst_pin, int8_t cs_pin,
      //                             uint32_t bitrate)

// mic

// states for demo 
int demo_mode = 2;
int sleeping_mode = 0;
static const int max_animation_index = 12;
int current_animation_index = 0;
int blink_sound = 1;
//reference state
int ref_eye_height = 40;
int ref_eye_width = 40;
int ref_space_between_eye = 10;
int ref_corner_radius = 10;
//current state of the eyes
int left_eye_height = ref_eye_height;
int left_eye_width = ref_eye_width;
int left_eye_x = 32;
int left_eye_y = 32;
int right_eye_x = 32+ref_eye_width+ref_space_between_eye;
int right_eye_y = 32;
int right_eye_height = ref_eye_height;
int right_eye_width = ref_eye_width;

void siren_alert(){
  int i;
  int k;
  for(k=0;k<4;k++){
    for (i=50; i<800; i++){
    tone(buzzer, i, 50);
    delay(1);
   }
  }
}


void draw_eyes(bool update=true)
{
    display.clearDisplay();        
    //draw from center
    int x = int(left_eye_x-left_eye_width/2);
    int y = int(left_eye_y-left_eye_height/2);
    display.fillRoundRect(x,y,left_eye_width,left_eye_height,ref_corner_radius,SSD1306_WHITE);
    x = int(right_eye_x-right_eye_width/2);
    y = int(right_eye_y-right_eye_height/2);
    display.fillRoundRect(x,y,right_eye_width,right_eye_height,ref_corner_radius,SSD1306_WHITE);    
    if(update)
    {
      display.display();
    }
    
}


void center_eyes(bool update=true)
{
  //move eyes to the center of the display, defined by SCREEN_WIDTH, SCREEN_HEIGHT
  left_eye_height = ref_eye_height;
  left_eye_width = ref_eye_width;
  right_eye_height = ref_eye_height;
  right_eye_width = ref_eye_width;
  
  left_eye_x = SCREEN_WIDTH/2-ref_eye_width/2-ref_space_between_eye/2;
  left_eye_y = SCREEN_HEIGHT/2;
  right_eye_x = SCREEN_WIDTH/2+ref_eye_width/2+ref_space_between_eye/2;
  right_eye_y = SCREEN_HEIGHT/2;
  
  draw_eyes(update);
}

void blink(int speed=12)
{
  draw_eyes();
  
  
  for(int i=0;i<3;i++)
  {
    left_eye_height = left_eye_height-speed;
    right_eye_height = right_eye_height-speed;    
    draw_eyes();
    delay(1);
  }
  for(int i=0;i<3;i++)
  {
    left_eye_height = left_eye_height+speed;
    right_eye_height = right_eye_height+speed;
    
    draw_eyes();
    delay(1);
  }
  if (blink_sound == 1) {
    tone(buzzer, 400, 30); // Play 400 Hz sound for 30 ms
    delay(100);            // Wait for 100 ms
    tone(buzzer, 600, 30); // Play 600 Hz sound for 30 ms
    delay(100);            // Wait for 100 ms
  } else {
    noTone(buzzer); // Turn off the buzzer
  }
}


void sleep()
{
  left_eye_height = 2;
  right_eye_height = 2;
  draw_eyes(true);  
}
void wakeup()
{
  
  sleep();
  
  for(int h=0; h <= ref_eye_height; h+=2)
  {
    left_eye_height = h;
    right_eye_height = h;
    draw_eyes(true);
  }
  

}


void happy_eye()
{
  center_eyes(false);
  //draw inverted triangle over eye lower part
  int offset = ref_eye_height/2;
  for(int i=0;i<10;i++)
  {
    display.fillTriangle(left_eye_x-left_eye_width/2-1, left_eye_y+offset, left_eye_x+left_eye_width/2+1, left_eye_y+5+offset, left_eye_x-left_eye_width/2-1,left_eye_y+left_eye_height+offset,SSD1306_BLACK);
    //display.fillRect(left_eye_x-left_eye_width/2-1, left_eye_y+5, left_eye_width+1, 20,SSD1306_BLACK);

    display.fillTriangle(right_eye_x+right_eye_width/2+1, right_eye_y+offset, right_eye_x-left_eye_width/2-1, right_eye_y+5+offset, right_eye_x+right_eye_width/2+1,right_eye_y+right_eye_height+offset,SSD1306_BLACK);
    //display.fillRect(right_eye_x-right_eye_width/2-1, right_eye_y+5, right_eye_width+1, 20,SSD1306_BLACK);  
    offset -= 2;
    display.display();
    delay(1);
  }
  
  
  display.display();

  delay(1000);
}

void saccade(int direction_x, int direction_y)
{
  //quick movement of the eye, no size change. stay at position after movement, will not move back,  call again with opposite direction
  //direction == -1 :  move left
  //direction == 1 :  move right
  
  int direction_x_movement_amplitude = 8;
  int direction_y_movement_amplitude = 6;
  int blink_amplitude = 8;

  for(int i=0;i<1;i++)
  {
    left_eye_x+=direction_x_movement_amplitude*direction_x;
    right_eye_x+=direction_x_movement_amplitude*direction_x;    
    left_eye_y+=direction_y_movement_amplitude*direction_y;
    right_eye_y+=direction_y_movement_amplitude*direction_y;    
    
    right_eye_height-=blink_amplitude;
    left_eye_height-=blink_amplitude;
    draw_eyes();
    delay(1);
  }
  
  for(int i=0;i<1;i++)
  {
    left_eye_x+=direction_x_movement_amplitude*direction_x;
    right_eye_x+=direction_x_movement_amplitude*direction_x;    
    left_eye_y+=direction_y_movement_amplitude*direction_y;
    right_eye_y+=direction_y_movement_amplitude*direction_y;

    right_eye_height+=blink_amplitude;
    left_eye_height+=blink_amplitude;

    draw_eyes();
    delay(1);
  }

}

void move_right_big_eye()
{
  move_big_eye(1);
}
void move_left_big_eye()
{
  move_big_eye(-1);
}
void move_big_eye(int direction)
{
  //direction == -1 :  move left
  //direction == 1 :  move right

  int direction_oversize = 1;
  int direction_movement_amplitude = 2;
  int blink_amplitude = 5;

  for(int i=0;i<3;i++)
  {
    left_eye_x+=direction_movement_amplitude*direction;
    right_eye_x+=direction_movement_amplitude*direction;    
    right_eye_height-=blink_amplitude;
    left_eye_height-=blink_amplitude;
    if(direction>0)
    {
      right_eye_height+=direction_oversize;
      right_eye_width+=direction_oversize;
    }else
    {
      left_eye_height+=direction_oversize;
      left_eye_width+=direction_oversize;
    }

    draw_eyes();
    delay(1);
  }
  for(int i=0;i<3;i++)
  {
    left_eye_x+=direction_movement_amplitude*direction;
    right_eye_x+=direction_movement_amplitude*direction;
    right_eye_height+=blink_amplitude;
    left_eye_height+=blink_amplitude;
    if(direction>0)
    {
      right_eye_height+=direction_oversize;
      right_eye_width+=direction_oversize;
    }else
    {
      left_eye_height+=direction_oversize;
      left_eye_width+=direction_oversize;
    }
    draw_eyes();
    delay(1);
  }

  delay(1000);

  for(int i=0;i<3;i++)
  {
    left_eye_x-=direction_movement_amplitude*direction;
    right_eye_x-=direction_movement_amplitude*direction;    
    right_eye_height-=blink_amplitude;
    left_eye_height-=blink_amplitude;
    if(direction>0)
    {
      right_eye_height-=direction_oversize;
      right_eye_width-=direction_oversize;
    }else
    {
      left_eye_height-=direction_oversize;
      left_eye_width-=direction_oversize;
    }
    draw_eyes();
    delay(1);
  }
  for(int i=0;i<3;i++)
  {
    left_eye_x-=direction_movement_amplitude*direction;
    right_eye_x-=direction_movement_amplitude*direction;    
    right_eye_height+=blink_amplitude;
    left_eye_height+=blink_amplitude;
    if(direction>0)
    {
      right_eye_height-=direction_oversize;
      right_eye_width-=direction_oversize;
    }else
    {
      left_eye_height-=direction_oversize;
      left_eye_width-=direction_oversize;
    }
    draw_eyes();
    delay(1);
  }


  center_eyes();


}

//Here new eye functions start, created by my own

void tired_eye()
{
  center_eyes(false);
  
  // Draw a "tired" eyelid that partially covers the eyes
  int offset = ref_eye_height / 2;

  // Loop to create the drooping effect by filling a rectangle on the upper part of each eye
  for (int i = 0; i < 10; i++) {
    // Left eye: draw a rectangle across the upper part to simulate a drooping eyelid
    display.fillRect(left_eye_x - left_eye_width / 2 - 1, left_eye_y - offset, left_eye_width + 2, left_eye_height / 3, SSD1306_BLACK);

    // Right eye: same as left, creating the eyelid effect
    display.fillRect(right_eye_x - right_eye_width / 2 - 1, right_eye_y - offset, right_eye_width + 2, right_eye_height / 3, SSD1306_BLACK);

    // Optional: draw small lines under each eye to simulate "tiredness" lines
    display.drawLine(left_eye_x - left_eye_width / 2 + 2, left_eye_y + left_eye_height / 2 + 2, left_eye_x - left_eye_width / 2 + 8, left_eye_y + left_eye_height / 2 + 2, SSD1306_BLACK);
    display.drawLine(right_eye_x - right_eye_width / 2 + 2, right_eye_y + right_eye_height / 2 + 2, right_eye_x - right_eye_width / 2 + 8, right_eye_y + right_eye_height / 2 + 2, SSD1306_BLACK);

    // Reduce offset to move the rectangle down slightly with each iteration
    offset -= 1;
    display.display();
    delay(1);
  }

  display.display();
  delay(1000);
}

void heart_eye() {
  center_eyes(false);
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


void sad_eye()
{
  center_eyes(false);

  int offset = ref_eye_height / 2;

  // Loop to create an "sad" expression with slanted lines above each eye
  for (int i = 0; i < 10; i++) {
    // Left eye: draw a slanted triangle above the eye to simulate a furrowed brow
    display.fillTriangle(
      left_eye_x - left_eye_width / 2 - 1, left_eye_y - offset,               // Left corner
      left_eye_x + left_eye_width / 2 + 1, left_eye_y - 5 - offset,           // Right corner
      left_eye_x - left_eye_width / 2 - 1, left_eye_y - 10 - offset,          // Top corner
      SSD1306_BLACK
    );

    // Right eye: mirror the effect for a symmetrical angry look
    display.fillTriangle(
      right_eye_x + right_eye_width / 2 + 1, right_eye_y - offset,            // Right corner
      right_eye_x - right_eye_width / 2 - 1, right_eye_y - 5 - offset,        // Left corner
      right_eye_x + right_eye_width / 2 + 1, right_eye_y - 10 - offset,       // Top corner
      SSD1306_BLACK
    );

    // Optional: Draw a line in the lower part of each eye to make them appear more intense
    display.drawLine(
      left_eye_x - left_eye_width / 2 + 2, left_eye_y + left_eye_height / 2,  // Start point
      left_eye_x + left_eye_width / 2 - 2, left_eye_y + left_eye_height / 2,  // End point
      SSD1306_BLACK
    );

    display.drawLine(
      right_eye_x - right_eye_width / 2 + 2, right_eye_y + right_eye_height / 2, // Start point
      right_eye_x + right_eye_width / 2 - 2, right_eye_y + right_eye_height / 2, // End point
      SSD1306_BLACK
    );

    // Increment offset for a downward movement of the "brows"
    offset -= 1;
    display.display();
    delay(1);
  }

  display.display();
  delay(1000);
}

void angry_eye()
{
  center_eyes(false);

  int offset = ref_eye_height / 2;

  // Loop to create an "angry" expression with upward-slanting lines above each eye
  for (int i = 0; i < 10; i++) {
    // Left eye: draw a slanted triangle above the eye, pointing upward to create an "angry brow"
    display.fillTriangle(
      left_eye_x - left_eye_width / 2 - 1, left_eye_y - 10 - offset,        // Left corner (higher)
      left_eye_x + left_eye_width / 2 + 1, left_eye_y - 5 - offset,         // Right corner (lower)
      left_eye_x + left_eye_width / 2 + 1, left_eye_y - offset,             // Right bottom corner
      SSD1306_BLACK
    );

    // Right eye: mirror the effect, with the inner corner lower and outer corner higher
    display.fillTriangle(
      right_eye_x + right_eye_width / 2 + 1, right_eye_y - 10 - offset,     // Right corner (higher)
      right_eye_x - right_eye_width / 2 - 1, right_eye_y - 5 - offset,      // Left corner (lower)
      right_eye_x - right_eye_width / 2 - 1, right_eye_y - offset,          // Left bottom corner
      SSD1306_BLACK
    );

    // Optional: Draw a line in the lower part of each eye to add intensity to the look
    display.drawLine(
      left_eye_x - left_eye_width / 2 + 2, left_eye_y + left_eye_height / 2,  // Start point
      left_eye_x + left_eye_width / 2 - 2, left_eye_y + left_eye_height / 2,  // End point
      SSD1306_BLACK
    );

    display.drawLine(
      right_eye_x - right_eye_width / 2 + 2, right_eye_y + right_eye_height / 2, // Start point
      right_eye_x + right_eye_width / 2 - 2, right_eye_y + right_eye_height / 2, // End point
      SSD1306_BLACK
    );

    // Increment offset for a slight animation effect
    offset -= 1;
    display.display();
    delay(1);
  }
  siren_alert();
  display.display();
  delay(1000);
}

void sleeping_eye()
{
  center_eyes(false);

  // Draw closed eyes by filling a rectangle across each eye
  int offset = ref_eye_height / 2;

  for (int i = 0; i < 10; i++) {
    // Left eye: draw a horizontal rectangle across the eye to simulate a closed eyelid
    display.fillRect(
      left_eye_x - left_eye_width / 2 - 1, 
      left_eye_y - offset, 
      left_eye_width + 2, 
      left_eye_height / 3, 
      SSD1306_BLACK
    );

    // Right eye: same as left eye, creating the appearance of a closed eyelid
    display.fillRect(
      right_eye_x - right_eye_width / 2 - 1, 
      right_eye_y - offset, 
      right_eye_width + 2, 
      right_eye_height / 3, 
      SSD1306_BLACK
    );

    // Optional: Add a slight animation effect by moving the eyelids down slightly with each iteration
    offset -= 1;
    display.display();
    delay(1);
  }

  // Final delay to keep the "eyes" closed for a bit longer, simulating sleep
  display.display();
  delay(2000);
}

void flirtive_eye()
{
  center_eyes(false);

  int offset = ref_eye_height / 2;

  // Draw half-closed eyes to simulate a flirtive, playful expression
  for (int i = 0; i < 10; i++) {
    // Left eye: draw a half-closed eyelid
    display.fillRect(
      left_eye_x - left_eye_width / 2 - 1, 
      left_eye_y + offset, 
      left_eye_width + 2, 
      left_eye_height / 3, 
      SSD1306_BLACK
    );

    // Right eye: draw a winking or smaller eye
    display.fillTriangle(
      right_eye_x - right_eye_width / 2 - 1, right_eye_y + offset,               // Left corner
      right_eye_x + right_eye_width / 2 + 1, right_eye_y + 5 + offset,           // Right corner
      right_eye_x - right_eye_width / 2 - 1, right_eye_y + left_eye_height / 2 + offset, // Bottom corner
      SSD1306_BLACK
    );

    // Optional: Add lashes on the left eye for a flirtive effect
    display.drawLine(
      left_eye_x - left_eye_width / 2 + 2, left_eye_y - 2, 
      left_eye_x - left_eye_width / 2 + 6, left_eye_y - 6, 
      SSD1306_BLACK
    );
    display.drawLine(
      left_eye_x + left_eye_width / 2 - 2, left_eye_y - 2, 
      left_eye_x + left_eye_width / 2 - 6, left_eye_y - 6, 
      SSD1306_BLACK
    );

    // Slight offset movement for playful animation
    offset -= 1;
    display.display();
    delay(1);
  }

  // Final delay to hold the flirtive expression
  display.display();
  delay(1000);
}

void draw_heart()
{
  display.clearDisplay();
  display.setCursor(24,24);
  display.setTextSize(4);
  display.write(3);
  display.display();
  delay(2000);
}

void countdown(int seconds) {
  for (int i = seconds; i > 0; i--) {
    display.clearDisplay();
    display.setTextSize(4);          // Large digits
    display.setTextColor(SSD1306_WHITE);

    // Estimate text width: 6 pixels per char * size (roughly 24 per digit at size 4)
    int textWidth = (i < 10) ? 24 : 48;
    int x = (SCREEN_WIDTH - textWidth) / 2;   // center horizontally
    int y = (SCREEN_HEIGHT - 32) / 2;         // roughly vertical center

    display.setCursor(x, y);
    display.print(i);
    display.display();

    // Optional beep each second
    tone(buzzer, 700, 100);
    delay(900);
  }

  // Show "GO!" at the end
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);

  int textWidth = 6 * 2 * 3; // "GO!" ~3 chars, 6px each, size 2
  int x = (SCREEN_WIDTH - textWidth) / 2;
  int y = (SCREEN_HEIGHT - 16) / 2;

  display.setCursor(x, y);
  display.print("GO!");
  display.display();

  tone(buzzer, 1000, 300);
  delay(1000);
}








void setup() {
  /**
   * @brief Set voice volume
   * @param voc - Volume value(1~7)
   */
  asr.setVolume(7);

  /**
     @brief Set mute mode
     @param mode - Mute mode; set value 1: mute, 0: unmute
  */
  asr.setMuteMode(0);

  /**
     @brief Set wake-up duration
     @param wakeTime - Wake-up duration (0-255)
  */
  asr.setWakeTime(20);

  /**
     @brief Get wake-up duration
     @return The currently-set wake-up period
  */
  uint8_t wakeTime = 0;
  wakeTime = asr.getWakeTime();

  /**
     @brief Play the corresponding reply audio according to the ID
     @param CMDID - command word ID
  */
  //asr.playByCMDID(23);  // Command word ID

  Serial.print("wakeTime = ");
  Serial.println(wakeTime);
  // put your setup code here, to run once:
  pinMode(buzzer, OUTPUT);

  tone(buzzer, 400,50);
  delay(100);
  tone(buzzer, 400,50);
  delay(1000);
  noTone(buzzer);
  delay(1000);
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  
  
  //for usb communication  
  Serial.begin(115200);
  
  

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  
  // Clear the buffer
  display.clearDisplay();
  
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("BUNNY IS STARTING UP"));  
  display.display();
  delay(2000);  
  display.startscrolldiagright(0x00, 0x07);
  delay(2000);
  display.startscrolldiagleft(0x00, 0x07);
  delay(2000);
  display.stopscroll();
  delay(2000);
  center_eyes(true);
  sleep();
  delay(2000);
  wakeup();
  // Draw a single pixel in white
  //display.drawPixel(10, 10, SSD1306_WHITE);
  
  
}



void launch_animation_with_index(int animation_index)
{

  

  if(animation_index>max_animation_index)
  {
    animation_index=12;
  }

  switch(animation_index)
      {
        case 0:
          wakeup();
          break;
        case 1:
          center_eyes(true);
          delay(1500);
          break;
        case 2:
          move_right_big_eye();
          break;
        case 3:
          move_left_big_eye();
          break;
        case 4:      
          blink(10);
          break;
        case 5:
          blink(20);
          break;
        case 6:
          happy_eye();      
          break;
        case 7:
          sleep();
          delay(1500);
          break;
        case 8:
          center_eyes(true);
          tone(buzzer, 600,50);
          delay(100);
          tone(buzzer, 400,50);
          delay(100);
          tone(buzzer, 800,50);
          delay(100);
          for(int i=0;i<20;i++)
          { 
            int dir_x = random(-1, 2);
            int dir_y = random(-1, 2);
            saccade(dir_x,dir_y);
            delay(1);
            saccade(-dir_x,-dir_y);
            delay(1);     
          }
          break;
        case 9:
          sad_eye();
          break; 
        case 10:
          tired_eye();
          break;
        case 11:
          angry_eye();
          break;
        case 12:
          flirtive_eye();
          break;        
      }
}

void randomizeFunction() {
    // Initialize random seed (using an analog pin for randomness)
    randomSeed(analogRead(0));

    // Generate a random number between 1 and 8
    int randomChoice = random(0, 5);

    // Call a function based on the random number
    switch (randomChoice) {
        case 0:
          center_eyes(true);
          break;
        case 1:
          move_right_big_eye();
          break;
        case 2:
          move_left_big_eye();
          break;
        case 3:      
          blink(10);
          blink(10);
          break;
        case 4:
          blink(20);
          break;
//        case 5:
//          tired_eye();
//          delay(1000);
    }
}



void happy_sound(){
  tone(buzzer,600);
  delay(100);
  tone(buzzer,900);
  delay(100);
  noTone(buzzer);
}


// ─────────────── ACTIVE VOICE COMMANDS ───────────────
//  ID    Command Phrase              →   Function
//  ----------------------------------------------------
//   1    (custom internal)           →   happy_eye(), demo_mode = 3
//   2    (custom internal)           →   happy_eye(), demo_mode = 2
//   5    (custom internal)           →   sleeping_eye(), sleep()
//  57    Display number five         →   countdown(5)
//  62    Display smiley face         →   happy_eye()
//  63    Display crying face         →   sad_eye()
//  64    Display heart               →   heart_eye()
// 103    Turn on the light           →   happy_eye()
// 104    Turn off the light          →   demo_mode = 2
// 116    Set to red                  →   angry_eye()
// ─────────────────────────────────────────────────────


void loop() {

  // put your main code here, to run repeatedly:
  uint8_t CMDID = asr.getCMDID();
  switch (CMDID) {
    case 1:
      happy_eye();                                                          
      Serial.println("LISTENING',command flag'1'");  
      delay(500);
      demo_mode = 3;
      break;
    case 2:
      happy_eye();                                                          
      Serial.println("LISTENING',command flag'1'");  
      delay(500);
      demo_mode = 2;
      break;
    case 116:
      angry_eye();
      delay(500);
      demo_mode = 2;                                                               
      break;

    case 62:
      happy_eye();
      delay(500);
      demo_mode = 2;                                                                         
      break;

    case 63:
      sad_eye();
      delay(500);
      demo_mode = 2;                                                                         
      break;
    case 5:
      blink_sound = 0;
      demo_mode = 3;
      sleeping_mode = 1;                                             
      sleeping_eye();
      delay(4000);
      sleep();                               
      Serial.println("Sleeping Time',command flag'5");
      delay(1500);
      break;

    case 64:
      demo_mode = 0;
      flirtive_eye();
      delay(200);
      heart_eye();
      Serial.println("H',command flag'64");
      delay(1500);                                                                
      //demo_mode = 2;   
      break;

    case 103:                                                  
      happy_eye();                               
      Serial.println("received'Turn on the light',command flag'103'");  
      break;

    case 104:
      demo_mode = 2;                                                
      Serial.println("received'Turn off the light',command flag'104'");  
      break;

    case 57:
      countdown(5);
      break;

    default:
      if (CMDID != 0) {
        Serial.print("CMDID = ");  //Printing command ID
        Serial.println(CMDID);
      }
  }
  delay(300);
//  if(demo_mode == 1)
//  {
    // cycle animations
//    launch_animation_with_index(current_animation_index++);
//    if(current_animation_index > max_animation_index)
//    {
//      current_animation_index = 0;
//    }
//  }

    // Call the random function at intervals or based on some condition
  //tired_eye();
  //delay(1000);
  //sad_eye();
  //delay(1000);
  //angry_eye();
  //flirtive_eye();
  //delay(1000);
  //draw_heart(); 
  //play_melody();
  if(demo_mode == 2)
  {
    randomizeFunction();
    delay(1000); // Delay to prevent rapid function calls
  }
  //send A0 - A5  for animation 0 to 5
//  if(Serial.available()) {
//    String data = Serial.readString();
//    data.trim();
//    char cmd = data[0];
    
//    if(cmd == 'H')
//    {
//      tone(buzzer, 400,50);
//      delay(100);
//      tone(buzzer, 200,50);
//      delay(100);
//      tone(buzzer, 400,50);
//      delay(100); 
//      happy_eye();
//    }    

//    if(cmd == 'D')
 //   {
  //    demo_mode = 2;
//    }  

    
//    if(cmd == 'A')
//    {
//      demo_mode = 0;

//      String arg = data.substring(1,data.length());
//      int anim = arg.toInt();
//      launch_animation_with_index(anim);
//      Serial.print(cmd);
//      Serial.print(arg);   
//    }

//    if(cmd == 'R')
//    {
//      angry_eye();
//    }
    


//  }


}
