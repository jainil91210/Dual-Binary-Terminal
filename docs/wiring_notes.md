# Wiring Notes

## Main Controller

ESP32 Development Board

The ESP32 acts as the central processing unit for:
- OLED rendering
- WiFi communication
- AI bridge communication
- touch processing
- utility control

---

# OLED Display Connections

## Display 1

VCC → 3.3V
GND → GND
SDA → GPIO 21
SCL → GPIO 22

## Display 2

VCC → 3.3V
GND → GND
SDA → GPIO 21
SCL → GPIO 22

Both displays share the same I2C bus.

---

# MPR121 Connections

VCC → 3.3V
GND → GND
SDA → GPIO 21
SCL → GPIO 22
IRQ → GPIO 4

The MPR121 handles capacitive touch keyboard input.

---

# DIP Switch Connections

Each DIP switch connects to a dedicated GPIO pin.

Example mapping:

Switch 1 → GPIO 13
Switch 2 → GPIO 12
Switch 3 → GPIO 14
Switch 4 → GPIO 27
Switch 5 → GPIO 26
Switch 6 → GPIO 25
Switch 7 → GPIO 33
Switch 8 → GPIO 32

These switches are used for hardware-level mode selection.

---

# Power System

The system is designed to run from:
- USB power
or
- portable battery pack

Future revisions may include:
- lithium battery charging
- battery management system
- power regulation circuitry

---

# Future Expansion Ports

Future hardware expansion may include:
- speaker output
- vibration motor
- SD card support
- wireless communication modules
- sensor integration