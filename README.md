# Modular Dual-Display Cyber-Console

## Project Description

The Modular Dual-Display Cyber-Console is a cyberpunk-inspired handheld embedded system designed to combine AI interaction, retro-console aesthetics, hardware utilities, and touch-based input into a single portable device.

The system uses dual OLED displays, an ESP32 controller, and an MPR121 capacitive touch keyboard to create a futuristic terminal capable of communicating with Google Gemini AI through a custom HTML bridge server.

The goal of the project is to explore embedded AI interaction, modular hardware architecture, tactile interfaces, and portable cyberdeck-style computing systems.

---

# Features

- Dual OLED display architecture
- ESP32 WiFi-enabled control system
- MPR121 capacitive touch keyboard
- Gemini AI integration
- OLED terminal-style rendering
- Touch-based prompt construction
- Modular firmware structure
- DIP-switch operational mode system
- Retro cyberpunk-inspired interface
- Expandable hardware architecture

---

# Hardware Used

## Main Components

- ESP32 Development Board
- 2x OLED Displays
- MPR121 Capacitive Touch Controller
- 8-Position DIP Switch
- Breadboard Prototype Setup
- Jumper Wires
- USB Power System

---

# Images

## Prototype Images

Add your hardware photos inside the `images/` folder.

Example images:
- OLED display running
- ESP32 wiring setup
- MPR121 touch keyboard
- breadboard prototype
- dual-screen layout

---

# Wiring Explanation

## OLED Displays

SDA → GPIO 21  
SCL → GPIO 22  
VCC → 3.3V  
GND → GND  

## MPR121

SDA → GPIO 21  
SCL → GPIO 22  
IRQ → GPIO 4  
VCC → 3.3V  
GND → GND  

## DIP Switch Panel

Connected to dedicated ESP32 GPIO pins for hardware-level mode selection.

---

# Setup Instructions

## 1. Install Arduino IDE

Install the Arduino IDE and ESP32 board package.

## 2. Install Required Libraries

Install:
- Adafruit SSD1306
- Adafruit GFX
- Adafruit MPR121

## 3. Upload Firmware

Open the `.ino` firmware file inside the `firmware/` folder and upload it to the ESP32.

## 4. Connect Hardware

Wire the OLED displays and MPR121 according to the wiring documentation.

## 5. Run HTML Gemini Bridge

Open the HTML bridge page and enter:
- ESP32 IP address
- Gemini API key

## 6. Start AI Communication

Use the MPR121 touch keyboard to type prompts and send them to Gemini AI.

Responses will appear on the OLED display.

---

# How the AI Bridge Works

The system uses a browser-based HTML bridge to securely communicate with the Gemini API.

## Workflow

MPR121 Keyboard  
↓  
ESP32 Prompt Builder  
↓  
Local HTML Bridge  
↓  
Google Gemini API  
↓  
HTML Response Parser  
↓  
ESP32 OLED Display  

This architecture prevents exposing API keys directly inside embedded firmware.

The ESP32 only handles:
- input collection
- display rendering
- WiFi communication
- lightweight HTTP requests

The HTML bridge handles:
- Gemini API authentication
- prompt forwarding
- response parsing
- AI communication routing

---

# Future Plans

- Custom PCB design
- Rechargeable battery system
- Cyberpunk enclosure
- Retro mini-games
- Audio system
- SD card support
- Expandable hardware modules
- Advanced terminal UI
- Split-screen multitasking
- Local AI processing
- Haptic feedback system

---

# Project Status

Currently under active development and prototype testing.