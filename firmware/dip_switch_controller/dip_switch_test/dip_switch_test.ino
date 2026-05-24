const int DIP_PIN_CASE     = 13; // Switch 1
const int DIP_PIN_NUMBER   = 15; // Switch 2
const int DIP_PIN_SYMBOL   = 25; // Switch 3 
const int DIP_PIN_LEDS     = 4;  // Switch 4
const int DIP_PIN_TERMINAL = 5;  // Switch 5
const int DIP_PIN_AI       = 23; // Switch 6

void setup() {
  Serial.begin(115200);
  while(!Serial); 
  Serial.println("=========================================");
  Serial.println("=========================================");
  pinMode(DIP_PIN_CASE, INPUT_PULLUP);
  pinMode(DIP_PIN_NUMBER, INPUT_PULLUP);
  pinMode(DIP_PIN_SYMBOL, INPUT_PULLUP);
  pinMode(DIP_PIN_LEDS, INPUT_PULLUP);
  pinMode(DIP_PIN_TERMINAL, INPUT_PULLUP);
  pinMode(DIP_PIN_AI, INPUT_PULLUP);
}

void loop() {
  int sw1_raw = digitalRead(DIP_PIN_CASE);
  int sw2_raw = digitalRead(DIP_PIN_NUMBER);
  int sw3_raw = digitalRead(DIP_PIN_SYMBOL);
  int sw4_raw = digitalRead(DIP_PIN_LEDS);
  int sw5_raw = digitalRead(DIP_PIN_TERMINAL);
  int sw6_raw = digitalRead(DIP_PIN_AI);

  Serial.println("--- Current Switch Status ---"); 
  Serial.print("SW1 (GPIO 13) Case Shift : Raw="); Serial.print(sw1_raw);
  Serial.println(sw1_raw == LOW ? " -> [ON] (Grounded)" : " -> [OFF] (Pulled High)");
  Serial.print("SW2 (GPIO 15) Numbers    : Raw="); Serial.print(sw2_raw);
  Serial.println(sw2_raw == LOW ? " -> [ON] (Grounded)" : " -> [OFF] (Pulled High)");
  Serial.print("SW3 (GPIO 25) Symbols    : Raw="); Serial.print(sw3_raw);
  Serial.println(sw3_raw == LOW ? " -> [ON] (Grounded)" : " -> [OFF] (Pulled High)");
  Serial.print("SW4 (GPIO 4)  LED Toggle : Raw="); Serial.print(sw4_raw);
  Serial.println(sw4_raw == LOW ? " -> [ON] (Grounded)" : " -> [OFF] (Pulled High)");
  Serial.print("SW5 (GPIO 5)  Terminal   : Raw="); Serial.print(sw5_raw);
  Serial.println(sw5_raw == LOW ? " -> [ON] (Grounded)" : " -> [OFF] (Pulled High)");
  Serial.print("SW6 (GPIO 23) AI Hub     : Raw="); Serial.print(sw6_raw);
  Serial.println(sw6_raw == LOW ? " -> [ON] (Grounded)" : " -> [OFF] (Pulled High)");
  Serial.println(); 
  delay(1000);      
}