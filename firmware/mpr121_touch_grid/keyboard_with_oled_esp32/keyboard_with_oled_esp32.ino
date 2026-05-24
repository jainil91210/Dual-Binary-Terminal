#include <Wire.h>
#include "Adafruit_MPR121.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
const int OLED_SDA = 21;
const int OLED_SCL = 22;
TwoWire I2C_OLED = TwoWire(0); 

const int SENSOR_SDA = 18;
const int SENSOR_SCL = 19;
TwoWire I2C_SENSOR = TwoWire(1); 
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &I2C_OLED, OLED_RESET);

Adafruit_MPR121 cap = Adafruit_MPR121();
const int TOUCH_THRESHOLD = 20;        
const unsigned long TAP_WINDOW = 1000;   
const unsigned long DEBOUNCE_TIME = 40; 
const unsigned long REPEATED_TAP_DELAY = 220; 
const int NUM_LETTER_PADS = 7;
const String padLetters[NUM_LETTER_PADS] = {
  "ABCD", // Pin 0
  "EFGH", // Pin 1
  "IJKL", // Pin 2
  "MNOP", // Pin 3
  "QRST", // Pin 4
  "UVW",  // Pin 5
  "XYZ"   // Pin 6
};

const int PAD_SPACE     = 7;  
const int PAD_BACKSPACE = 8;  
unsigned long lastTapTime = 0;
unsigned long fingerLiftTime = 0;             
unsigned long lastStateChangeTime[12] = {0}; 
int tapCount = 0;
int activePad = -1;                           
bool isCurrentlyTouched[12] = {false};        
bool waitingForTimeout = false;        
String currentLine = "";
void updateOLED() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2); 
  display.setCursor(0, 4); 
  
  if (waitingForTimeout && activePad != -1) {
    char previewChar = padLetters[activePad][((tapCount - 1) % padLetters[activePad].length())];
    display.print(currentLine + previewChar + "_");
  } else {
    display.print(currentLine + "_");
  }
  display.display();
}

void setup() {
  Serial.begin(115200);
  while (!Serial); 

  I2C_OLED.begin(OLED_SDA, OLED_SCL, 400000); 
  I2C_SENSOR.begin(SENSOR_SDA, SENSOR_SCL, 100000); 

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED not found.");
    while(1);
  }
  
  display.clearDisplay();
  display.display();

  if (!cap.begin(0x5A, &I2C_SENSOR)) {
    Serial.println("MPR121 not found.");
    while (1);
  }
  
  updateOLED(); 
}

void loop() {
  unsigned long currentTime = millis();
  bool stateChanged = false;
  for (int i = 0; i < 12; i++) {
    int rawValue = cap.filteredData(i);
    if (rawValue <= TOUCH_THRESHOLD) {
      if (!isCurrentlyTouched[i] && (currentTime - lastStateChangeTime[i] > DEBOUNCE_TIME)) {
        isCurrentlyTouched[i] = true;
        lastStateChangeTime[i] = currentTime;
        if (i < NUM_LETTER_PADS) {
          if (activePad == i) {
            if (currentTime - fingerLiftTime > REPEATED_TAP_DELAY) {
              char chosen = padLetters[activePad][((tapCount - 1) % padLetters[activePad].length())];
              currentLine += chosen;
              tapCount = 1; 
            } else {
            }
          } else {
            if (waitingForTimeout) {
              char chosen = padLetters[activePad][((tapCount - 1) % padLetters[activePad].length())];
              currentLine += chosen;
            }
            activePad = i;
            tapCount = 1;
          }
          lastTapTime = currentTime;
          waitingForTimeout = true;
          stateChanged = true;
        }
        
        else if (i == PAD_SPACE) {
          if (waitingForTimeout) { 
            char chosen = padLetters[activePad][((tapCount - 1) % padLetters[activePad].length())];
            currentLine += chosen;
            waitingForTimeout = false;
          }
          currentLine += " ";
          tapCount = 0;
          activePad = -1;
          stateChanged = true;
        }
        else if (i == PAD_BACKSPACE) {
          waitingForTimeout = false; 
          tapCount = 0;
          activePad = -1;
          
          if (currentLine.length() > 0) {
            currentLine.remove(currentLine.length() - 1);
          }
          stateChanged = true;
        }
      }
    } else {
      if (isCurrentlyTouched[i] && (currentTime - lastStateChangeTime[i] > DEBOUNCE_TIME)) {
        isCurrentlyTouched[i] = false;
        lastStateChangeTime[i] = currentTime;
        
        if (i < NUM_LETTER_PADS) {
          fingerLiftTime = currentTime;
        }
      }
    }
  }
  if (waitingForTimeout && (currentTime - lastTapTime > TAP_WINDOW)) {
    if (tapCount > 0 && activePad != -1) {
      int maxLetters = padLetters[activePad].length();
      char chosen = padLetters[activePad][((tapCount - 1) % maxLetters)];
      currentLine += chosen; 
    }
    tapCount = 0;
    activePad = -1;
    waitingForTimeout = false;
    stateChanged = true;
  }

  if (stateChanged) {
    updateOLED();
  }

  delay(5); 
}