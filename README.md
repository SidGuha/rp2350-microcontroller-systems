# stm32-microcontroller-systems

Bare-metal STM32 firmware in C and ARM assembly. Register-level GPIO, interrupts, timers, ADC/DMA, and PWM — written directly against the register map, no HAL abstraction layer.

---

## About

This repository collects my embedded systems work on the STM32 family. Each peripheral is a self-contained project in its own directory, ordered roughly by the sequence I built them in.

Everything here is written against the reference manual rather than a vendor abstraction layer — peripherals are configured by setting bits in memory-mapped registers directly. The goal was to understand what the hardware is actually doing, not to take the fastest path to a blinking LED.

**Target:** `<board / MCU part number>`
**Toolchain:** `<arm-none-eabi-gcc, OpenOCD, etc.>`
**Debugger:** `<ST-LINK / GDB>`

## Contents

| Directory | Focus | What it covers |
|---|---|---|
| [`01-gpio/`](01-gpio) | Digital I/O | Port clock enable, mode and pull-up registers, reading inputs and driving outputs at the register level |
| [`02-interrupts/`](02-interrupts) | NVIC & EXTI | External interrupt lines, priority configuration, ISR handlers, debouncing |
| [`03-timers/`](03-timers) | Hardware timers | Prescaler and auto-reload configuration, periodic interrupts, timing without blocking delays |
| [`04-adc-dma/`](04-adc-dma) | Analog input | ADC sampling and conversion, DMA transfer to memory without CPU involvement |
| [`05-pwm/`](05-pwm) | Output control | Timer compare channels, duty cycle control, driving analog-like output from digital pins |

Each directory has its own README describing the objective, the peripherals configured, and how to build and flash it.

## Repository layout

```
01-gpio/
  src/          firmware source
  inc/          headers
  <startup>     startup + linker script
  Makefile      build configuration
  README.md     notes for this project
02-interrupts/
...
```

## Building

Each project builds independently from its own directory:

```
cd 01-gpio
<build command>
<flash command>
```
