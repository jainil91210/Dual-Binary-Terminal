#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
WebServer server(80);

// --- ENTER YOUR WI-FI CREDENTIALS HERE ---
const char* ssid     = "dp_23-4G";
const char* password = "jainil517";

// Digital Touch Sensor Pins
const int PIN_LEFT   = 26;
const int PIN_RIGHT  = 27;
const int PIN_SELECT = 14;
const int PIN_SEND   = 12;

// Character Strip Layout
const char keyboard[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ .,?"; 
const int TOTAL_CHARS = 30;

int cursorIndex = 0;
String currentPrompt = "";
bool promptReadyToSend = false;

// Selection tracking timing variables
unsigned long selectTouchTime = 0;
bool isSelectPressed = false;
bool holdActionTriggered = false;

// Flags to ensure single clicks per touch
bool leftLatched = false;
bool rightLatched = false;
bool sendLatched = false;

// Scrolling Engine System Variables
String currentText = "";
bool newTextAvailable = false;
const int SCROLL_SPEED_DELAY = 35; 
const int LINE_HEIGHT = 10;        
String lines[120]; 
int lineCount = 0;

// Device Operating States
enum SystemState { TYPING_PROMPT, WAITING_FOR_AI, SCROLLING_RESPONSE };
SystemState currentState = TYPING_PROMPT;

// --- SERVER HTTP HANDLERS ---
void handleGetPrompt() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  if (promptReadyToSend) {
    server.send(200, "text/plain", currentPrompt);
    currentPrompt = ""; // Clear screen immediately after data is requested by laptop
    promptReadyToSend = false;
  } else {
    server.send(200, "text/plain", ""); 
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
    
    // Clean string components for raw character arrays
    currentText.replace("\n", " ");
    currentText.replace("\r", " ");
    while(currentText.indexOf("  ") >= 0) {
      currentText.replace("  ", " ");
    }
    
    newTextAvailable = true; 
    server.send(200, "text/plain", "Data Received");
  }
}

// --- TEXT PARSING CHUNKER ---
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
    // Re-enable text wrapping restriction just for clean scrolling layouts
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
  
  pinMode(PIN_LEFT, INPUT);
  pinMode(PIN_RIGHT, INPUT);
  pinMode(PIN_SELECT, INPUT);
  pinMode(PIN_SEND, INPUT);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) for(;;);
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
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

  // --- STATE 1: CHARACTER TYPING INTERFACE ---
  if (currentState == TYPING_PROMPT) {
    bool touchLeft   = (digitalRead(PIN_LEFT) == HIGH);
    bool touchRight  = (digitalRead(PIN_RIGHT) == HIGH);
    bool touchSelect = (digitalRead(PIN_SELECT) == HIGH);
    bool touchSend   = (digitalRead(PIN_SEND) == HIGH);

    if (touchLeft && !leftLatched) {
      cursorIndex = (cursorIndex > 0) ? cursorIndex - 1 : TOTAL_CHARS - 1;
      leftLatched = true;
      delay(50);
    } else if (!touchLeft) {
      leftLatched = false;
    }

    if (touchRight && !rightLatched) {
      cursorIndex = (cursorIndex < TOTAL_CHARS - 1) ? cursorIndex + 1 : 0;
      rightLatched = true;
      delay(50);
    } else if (!touchRight) {
      rightLatched = false;
    }

    if (touchSelect) {
      if (!isSelectPressed) {
        isSelectPressed = true;
        holdActionTriggered = false;
        selectTouchTime = millis();
      } else if (!holdActionTriggered && (millis() - selectTouchTime >= 2000)) {
        if (currentPrompt.length() > 0) {
          currentPrompt.remove(currentPrompt.length() - 1);
        }
        holdActionTriggered = true; 
      }
    } else {
      if (isSelectPressed) {
        if (!holdActionTriggered) {
          if (currentPrompt.length() < 40) { // Fits comfortably on two lines
            currentPrompt += keyboard[cursorIndex];
          }
        }
        isSelectPressed = false;
      }
    }

    if (touchSend && !sendLatched) {
      if (currentPrompt.length() > 0) {
        promptReadyToSend = true;
        currentState = WAITING_FOR_AI; 
      }
      sendLatched = true;
    } else if (!touchSend) {
      sendLatched = false;
    }

    // Render Keyboard UI
    display.clearDisplay();
    display.setTextSize(1);
    
    // WORD WRAPPING ENABLED FOR THE INPUT WINDOW
    display.setTextWrap(true); 
    display.setCursor(0, 0);
    display.print("> ");
    display.print(currentPrompt);
    
    // Push line down to Y=32 to allow up to 40 characters wrapped on 2 lines
    display.drawFastHLine(0, 32, SCREEN_WIDTH, WHITE);

    // Disable wrap briefly for the linear character carousel selector format
    display.setTextWrap(false);
    int displayIndexStart = cursorIndex - 3;
    for (int i = 0; i < 7; i++) {
      int targetIdx = displayIndexStart + i;
      if (targetIdx < 0) targetIdx += TOTAL_CHARS;
      if (targetIdx >= TOTAL_CHARS) targetIdx -= TOTAL_CHARS;
      
      int drawX = i * 18 + 4;
      int drawY = 46; // Pushed down to leave room for wrapped lines

      if (i == 3) { 
        display.fillRect(drawX - 2, drawY - 2, 14, 13, WHITE);
        display.setTextColor(BLACK);
      } else {
        display.setTextColor(WHITE);
      }
      display.setCursor(drawX, drawY);
      display.print(keyboard[targetIdx]);
    }
    display.display();
  }

  // --- STATE 2: AWAITING COMPUTER TRANSMISSION ROUTER ---
  else if (currentState == WAITING_FOR_AI) {
    display.clearDisplay();
    display.setTextWrap(true);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(10, 20);
    display.println("Prompt Dispatched...");
    display.setCursor(10, 35);
    display.println("Awaiting AI response");
    display.display();

    if (newTextAvailable) {
      currentState = SCROLLING_RESPONSE;
    }
    delay(50);
  }

  // --- STATE 3: PARSE RESPONSE DATA CHUNKS ---
  else if (currentState == SCROLLING_RESPONSE) {
    newTextAvailable = false;
    wrapTextToArray(currentText, 21); 
    smoothScrollText();
    delay(1500); 
    currentText = "";
    currentState = TYPING_PROMPT; 
  }

  delay(10);
}