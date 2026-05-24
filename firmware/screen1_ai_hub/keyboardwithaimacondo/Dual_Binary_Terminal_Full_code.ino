#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include "Adafruit_MPR121.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1

TwoWire I2C_OLED = TwoWire(0); 
TwoWire I2C_SENSOR = TwoWire(1); 

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &I2C_OLED, OLED_RESET);
Adafruit_MPR121 cap = Adafruit_MPR121();

WebServer server(80);

const char* ssid     = "dp_23-4G";
const char* password = "jainil517";

const int NUM_LETTER_PADS = 7;
const String padLetters[NUM_LETTER_PADS] = {"ABCD", "EFGH", "IJKL", "MNOP", "QRST", "UVW", "XYZ"};

const int PAD_SPACE     = 7;  
const int PAD_BACKSPACE = 8;  
const int PAD_KEY_ENTER = 11; 

const int TOUCH_THRESHOLD = 20;        
const unsigned long TAP_WINDOW = 1000;   
const unsigned long DEBOUNCE_TIME = 40; 
const unsigned long REPEATED_TAP_DELAY = 220; 

unsigned long lastTapTime = 0;
unsigned long fingerLiftTime = 0;             
unsigned long lastStateChangeTime[12] = {0}; 
int tapCount = 0;
int activePad = -1;                           
bool isCurrentlyTouched[12] = {false};        
bool waitingForTimeout = false;        
String currentPrompt = "";
bool promptReadyToSend = false;
String currentText = "";
bool newTextAvailable = false;
const int SCROLL_SPEED_DELAY = 35; 
const int LINE_HEIGHT = 10;        
String lines[120]; 
int lineCount = 0;
enum SystemState { TYPING_PROMPT, WAITING_FOR_AI, SCROLLING_RESPONSE };
SystemState currentState = TYPING_PROMPT;
void handleGetPrompt() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  if (promptReadyToSend) {
    server.send(200, "text/plain", currentPrompt);
    currentPrompt = ""; 
    promptReadyToSend = false;
  } else {
    server.send(204, "text/plain", ""); 
  }
}

void handlePostData() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");

  if (server.method() == HTTP_OPTIONS) {
    server.send(200, "text/plain", "");
    return;
  }

  if (server.hasArg("plain")) {
    currentText = server.arg("plain");
    currentText.trim();
    
    currentText.replace("\n", " ");
    currentText.replace("\r", " ");
    while(currentText.indexOf("  ") >= 0) {
      currentText.replace("  ", " ");
    }
    
    newTextAvailable = true; 
    server.send(200, "text/plain", "Data Received");
  }
}

void wrapTextToArray(String text, int maxCharsPerLine) {
  lineCount = 0;
  int textLength = text.length();
  int currentPos = 0;

  while (currentPos < textLength && lineCount < 120) {
    if (currentPos + maxCharsPerLine >= textLength) {
      lines[lineCount] = text.substring(currentPos);
      lineCount++;
      break;
    }
    int spaceIdx = text.lastIndexOf(' ', currentPos + maxCharsPerLine);
    if (spaceIdx > currentPos) {
      lines[lineCount] = text.substring(currentPos, spaceIdx);
      currentPos = spaceIdx + 1; 
    } else {
      lines[lineCount] = text.substring(currentPos, currentPos + maxCharsPerLine);
      currentPos += maxCharsPerLine;
    }
    lineCount++;
  }
}

void smoothScrollText() {
  int totalScrollHeight = lineCount * LINE_HEIGHT;
  for (int scrollPos = SCREEN_HEIGHT; scrollPos > -totalScrollHeight; scrollPos--) {
    server.handleClient(); 
    
    display.clearDisplay();
    display.setTextWrap(false);
    for (int i = 0; i < lineCount; i++) {
      int lineYPos = scrollPos + (i * LINE_HEIGHT);
      if (lineYPos > -LINE_HEIGHT && lineYPos < SCREEN_HEIGHT) {
        display.setCursor(0, lineYPos);
        display.print(lines[i]);
      }
    }
    display.display();
    delay(SCROLL_SPEED_DELAY);
  }
}

void setup() {
  Serial.begin(115200);
  
  
  I2C_OLED.begin(21, 22, 400000);     
  I2C_SENSOR.begin(18, 19, 100000);   

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) for(;;);
  if(!cap.begin(0x5A, &I2C_SENSOR)) for(;;);
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 20);
  display.println("Connecting WiFi...");
  display.display();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(100); }

  server.on("/get_prompt", HTTP_GET, handleGetPrompt);
  server.on("/send", HTTP_POST, handlePostData);
  server.on("/send", HTTP_OPTIONS, handlePostData); 
  server.begin();

  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());
  
  currentState = TYPING_PROMPT;
}

void loop() {
  server.handleClient();
  unsigned long currentTime = millis();

  if (currentState == TYPING_PROMPT) {
    
    for (int i = 0; i < 12; i++) {
      int rawValue = cap.filteredData(i);
      if (rawValue <= TOUCH_THRESHOLD) {
        if (!isCurrentlyTouched[i] && (currentTime - lastStateChangeTime[i] > DEBOUNCE_TIME)) {
          isCurrentlyTouched[i] = true;
          lastStateChangeTime[i] = currentTime;

          if (i < NUM_LETTER_PADS) {
            if (activePad == i) {
              if (currentTime - fingerLiftTime > REPEATED_TAP_DELAY) {
                currentPrompt += padLetters[activePad][((tapCount - 1) % padLetters[activePad].length())];
                tapCount = 1; 
              } else {
                tapCount++; 
              }
            } else {
              if (waitingForTimeout) {
                currentPrompt += padLetters[activePad][((tapCount - 1) % padLetters[activePad].length())];
              }
              activePad = i;
              tapCount = 1;
            }
            lastTapTime = currentTime;
            waitingForTimeout = true;
          }
          else if (i == PAD_SPACE) {
            if (waitingForTimeout) { 
              currentPrompt += padLetters[activePad][((tapCount - 1) % padLetters[activePad].length())];
              waitingForTimeout = false;
            }
            currentPrompt += " ";
            tapCount = 0; activePad = -1;
          }
          else if (i == PAD_BACKSPACE) {
            waitingForTimeout = false; tapCount = 0; activePad = -1;
            if (currentPrompt.length() > 0) currentPrompt.remove(currentPrompt.length() - 1);
          }
          else if (i == PAD_KEY_ENTER) {
            if (waitingForTimeout) { 
              currentPrompt += padLetters[activePad][((tapCount - 1) % padLetters[activePad].length())];
              waitingForTimeout = false;
            }
            if (currentPrompt.length() > 0) {
              promptReadyToSend = true;
              currentState = WAITING_FOR_AI; 
            }
            tapCount = 0; activePad = -1;
          }
        }
      } else {
        if (isCurrentlyTouched[i] && (currentTime - lastStateChangeTime[i] > DEBOUNCE_TIME)) {
          isCurrentlyTouched[i] = false;
          lastStateChangeTime[i] = currentTime;
          if (i < NUM_LETTER_PADS) fingerLiftTime = currentTime;
        }
      }
    }

    if (waitingForTimeout && (currentTime - lastTapTime > TAP_WINDOW)) {
      if (tapCount > 0 && activePad != -1) {
        currentPrompt += padLetters[activePad][((tapCount - 1) % padLetters[activePad].length())]; 
      }
      tapCount = 0; activePad = -1; waitingForTimeout = false;
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setTextWrap(true); 
    display.setCursor(0, 0);
    display.print("> ");
    
    String previewPrompt = currentPrompt;
    if (waitingForTimeout && activePad != -1) {
      previewPrompt += padLetters[activePad][((tapCount - 1) % padLetters[activePad].length())];
    }
    display.print(previewPrompt);
    
    if (currentTime % 1000 < 500) {
      display.print("_");
    }
    
    display.drawFastHLine(0, 52, SCREEN_WIDTH, SSD1306_WHITE);
    display.display();
  }

  else if (currentState == WAITING_FOR_AI) {
    display.clearDisplay();
    display.setTextWrap(true);
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 25);
    display.println("Sending Prompt...");
    display.display();

    if (newTextAvailable) {
      currentState = SCROLLING_RESPONSE;
    }
    delay(50);
  }

  else if (currentState == SCROLLING_RESPONSE) {
    newTextAvailable = false;
    wrapTextToArray(currentText, 21); 
    smoothScrollText();
    delay(1500); 
    currentText = "";
    currentState = TYPING_PROMPT; 
    
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("> ");
    display.display();
  }

  delay(5);
}