# Modular Dual-Display Cyber-Console Architecture

## Overview

The Modular Dual-Display Cyber-Console is a cyberpunk-inspired handheld terminal designed to combine AI interaction, retro-console aesthetics, embedded hardware control, and modular utilities into a single portable system.

The project is divided into two major display subsystems that work simultaneously while sharing a common ESP32 controller and touch-input framework.

---

# Core System Layout

## Display 1 — AI & Innovation Hub

The first OLED display acts as the primary AI interaction screen.

This subsystem is responsible for:
- AI response rendering
- terminal-style visual output
- system diagnostics
- future smart-agent frameworks
- technology information display
- compact conversational interaction

The display receives processed Gemini AI responses from a local HTML bridge server and renders them in a minimal cyber-terminal layout optimized for a 128x64 OLED screen.

---

## Display 2 — Console & Utility Engine

The second OLED display handles lightweight console operations and utilities.

This subsystem is responsible for:
- retro-game rendering
- terminal prompt construction
- utility menus
- WiFi management
- touch keyboard visualization
- low-level system interaction

This screen acts as the tactical hardware console of the device.

---

# Input System

The device uses an MPR121 capacitive touch controller as the primary keyboard and interaction matrix.

The MPR121 allows:
- character input
- menu navigation
- command selection
- prompt generation
- game controls
- utility switching

Each capacitive pad maps to software-defined functions depending on the active operating mode.

---

# DIP Switch System

An 8-position DIP switch panel acts as a physical mode selector.

The DIP switch system enables:
- subsystem switching
- game mode selection
- AI routing control
- utility activation
- future expansion modes

This creates a physical hardware-based operating environment rather than relying entirely on software menus.

---

# AI Communication Pipeline

The AI communication system is divided into two layers:

## ESP32 Layer
Responsible for:
- touch input collection
- OLED rendering
- local device control
- HTTP communication

## HTML Gemini Bridge
Responsible for:
- securely storing Gemini API keys
- forwarding prompts to Gemini
- processing AI responses
- returning optimized text back to ESP32

This architecture prevents exposing API keys directly inside embedded firmware.

---

# Communication Flow

MPR121 Keyboard
↓
ESP32 Prompt Builder
↓
Local HTML Gemini Bridge
↓
Google Gemini API
↓
HTML Bridge Response Parser
↓
ESP32 OLED Renderer

---

# Design Philosophy

The project focuses on:
- modularity
- repairability
- tactile interaction
- retro-futuristic aesthetics
- embedded AI interaction
- hardware-first computing

The goal is to create a handheld cyber-terminal that feels like a standalone futuristic computing node rather than a standard microcontroller project.