# PWM

Pulse-width modulation on the RP2350B — driving an RGB LED at controllable brightness, animating it from a wrap interrupt, and using the same peripheral plus an external filter to synthesize audio on a chip with no DAC.

## Hardware

| | |
|---|---|
| **MCU** | RP2350B (Proton development board) |
| **RGB LED** | GP37, GP38, GP39 through 47 Ω resistors |
| **Audio out** | GP36 → RC low-pass → LM324 op-amp (powered at 5 V) → TRRS jack |
| **Input** | 4×4 keypad and 8-digit 7-segment display, carried over from earlier work |

## What it does

**Static duty cycle.** Type a percentage on the keypad and press `#`; the RGB LED's brightness changes to match. The display echoes the digits as they're entered.

**Breathing light.** The LED fades from full brightness to off and back over two seconds, rotating red → green → blue on each cycle. The main loop does nothing at all — every duty cycle update happens inside a wrap interrupt.

**Audio synthesis.** A 440 Hz sine wave out of the headphone jack, with the frequency settable from the keypad on either of two independently mixed channels.

## Implementation

### Slices and channels

PWM isn't one output per pin. The peripheral has twelve slices, each with an A and a B channel, and GPIO pins map onto them in fixed pairs — so adjacent pins often share a slice, and a slice's divider and wrap value are shared by both its channels. Only the compare level is per-channel.

That's visible in the RGB wiring here: two of the three LED pins land on the same slice, so they share a period but hold independent brightness levels.

### Setting a frequency

Three numbers determine the output:

| Setting | Effect |
|---|---|
| Clock divider = 150 | 150 MHz system clock → 1 MHz counter |
| Wrap value (`period - 1`) | counter resets after `period` ticks |
| Compare level | counter below it drives one level, above it the other |

Period 100 gives a 10 kHz carrier; period 10000 gives 100 Hz. The wrap value is one less than the period because the counter starts at zero.

Duty cycle is the compare level as a fraction of the period — it changes the output's average voltage, not its frequency.

### Changing duty cycle safely

Writing a new compare level while the counter is mid-sweep can produce a malformed pulse: the counter may already have passed the new value, so that cycle comes out at the wrong width. For an LED it's an invisible flicker; for audio it's an audible click.

The fix is to only write at the moment the counter wraps to zero, which the peripheral will raise an interrupt for. The breathing animation lives entirely in that handler — acknowledge, adjust the duty cycle by one percent, write it, return. At a 100 Hz wrap rate, a hundred one-percent steps take exactly one second per ramp and two seconds for a full breath.

The colors are stepped by adding the color index to the base pin number, which works because the three LED pins are consecutive.

### Audio from a peripheral with no DAC

The RP2350 has no digital-to-analog converter, so an analog waveform has to be approximated: run PWM far above the audible range and let a low-pass filter average the pulses into a voltage. The RC filter on GP36 does the averaging and the LM324 buffers and amplifies the result for headphones.

The carrier is fixed at 20 kHz — chosen to sit above hearing while leaving 50 counts of amplitude resolution per sample. What varies is the *sample* being written, once per wrap:

- A sine wavetable is pre-computed in memory, DC-shifted positive because a duty cycle can't be negative
- Two independent read positions walk that table, each with its own increment. The increments are 16.16 fixed-point, so the step can be a fraction of a table entry — that's what allows an arbitrary output frequency from a fixed 20 kHz update rate, rather than only frequencies that divide evenly into it
- Both samples are summed and halved, so two mixed tones can't overflow past full scale
- The result is scaled from the wavetable's range into the slice's compare range and written

Every audible frequency therefore comes out of one fixed-rate interrupt, with the pitch living entirely in how fast the read positions advance.
