# rp2350-microcontroller-systems

Bare-metal firmware for the RP2350B microcontroller, written in C against the hardware registers directly rather than through the Pico SDK's peripheral helpers.

---

## About

This repository collects my embedded systems work on the RP2350B. Each peripheral is a self-contained project in its own directory, ordered roughly by the sequence I built them in.

The constraint throughout is that peripherals are configured by writing their memory-mapped registers directly — the SDK's convenience functions are read and dissected to find out which registers they touch, then set aside. The goal was to understand what the silicon is actually doing, not to take the shortest path to a blinking LED.

| | |
|---|---|
| **MCU** | RP2350B |
| **Board** | Proton development board |
| **Toolchain** | PlatformIO (VS Code), Pico SDK |
| **Debugger** | Raspberry Pi Debug Probe over SWD |

## Contents

| Directory | Focus | What it covers |
|---|---|---|
| [`01-gpio/`](01-gpio) | Digital I/O | Pad configuration, SIO function select, isolation latches, atomic set/clear registers, matrix keypad scanning |
| [`02-interrupts/`](02-interrupts) | Event handling | Interrupt sources and enables, handler registration, responding to input without polling |
| [`03-timers/`](03-timers) | Hardware timing | Timer configuration and periodic events, timing without blocking the CPU |
| [`04-adc-dma/`](04-adc-dma) | Analog input | ADC sampling and conversion, DMA transfer into memory without CPU involvement |
| [`05-pwm/`](05-pwm) | Output control | PWM slice configuration, duty cycle control, driving analog-like output from digital pins |

Each directory has its own README describing the hardware used, what the program does, and which registers it configures.

## Building

Each project is a single `main.c` built with PlatformIO against the Pico SDK. Open the directory in VS Code and use **Upload and Monitor**, or from the command line:

```
pio run --target upload
```
