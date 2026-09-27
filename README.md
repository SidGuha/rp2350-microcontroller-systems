# stm32-microcontroller-systems

Bare-metal STM32 firmware in C and ARM assembly. One branch per peripheral — register-level GPIO, interrupts, timers, ADC/DMA, and PWM. Written directly against the register map, no HAL abstraction layer.

---

## About

This repository collects my embedded systems work on the STM32 family. Each peripheral is developed on its own branch, so the implementation and history for a given subsystem stays self-contained and readable.

Everything here is written against the reference manual rather than a vendor abstraction layer — peripherals are configured by setting bits in memory-mapped registers directly. The goal was to understand what the hardware is actually doing, not to take the fastest path to a blinking LED.

**Target:** `<board / MCU part number>`
**Toolchain:** `<arm-none-eabi-gcc, OpenOCD, etc.>`
**Debugger:** `<ST-LINK / GDB>`

## Branches

| Branch | Focus | What it covers |
|---|---|---|
| [`GPIO`](../../tree/GPIO) | Digital I/O | Port clock enable, mode and pull-up registers, reading inputs and driving outputs at the register level |
| [`Interrupts`](../../tree/Interrupts) | NVIC & EXTI | External interrupt lines, priority configuration, ISR handlers, debouncing |
| [`Timers`](../../tree/Timers) | Hardware timers | Prescaler and auto-reload configuration, periodic interrupts, timing without blocking delays |
| [`ADC-DMA`](../../tree/ADC-DMA) | Analog input | ADC sampling and conversion, DMA transfer to memory without CPU involvement |
| [`PWM`](../../tree/PWM) | Output control | Timer compare channels, duty cycle control, driving analog-like output from digital pins |

Each branch has its own README describing the objective, the peripherals configured, and how to build and flash it.

## Building

```
<build command>
<flash command>
```

## Repository layout

Each branch follows roughly the same shape:

```
src/          firmware source
inc/          headers
<startup>     startup + linker script
Makefile      build configuration
README.md     notes for that branch
```
