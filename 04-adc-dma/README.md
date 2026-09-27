# ADC and DMA

Analog sampling on the RP2350B, taken through three stages of removing the CPU from the loop — from polling a single conversion, to free-running conversions, to a DMA channel that moves samples into memory with no CPU involvement at all. The end result is a potentiometer voltmeter reading out on the multiplexed 7-segment display.

## Hardware

| | |
|---|---|
| **MCU** | RP2350B (Proton development board) |
| **Potentiometer** | wiper on GP45 (ADC channel 5), ends tied to 3V3 and ground |
| **Display** | 74HC138 decoder on GP18–GP20, TLC59211 sink driver on GP10–GP17 |

The ADC is 12-bit, so a full-scale 3.3 V reads as 4095 and one count is a little under a millivolt.

## What it does

**Single-shot.** Selects channel 5, starts one conversion, waits for it to complete, returns the result. The CPU does everything and waits while the hardware works.

**Free-running.** The ADC starts a new conversion the instant the previous one completes and leaves the result in its register. The CPU no longer asks for samples — it just reads whatever is currently there.

**DMA-fed.** The ADC pushes completed samples into its FIFO and raises a data request; DMA channel 0 sees the request, moves the sample into a variable, and re-arms itself. No interrupt fires and no instruction executes for any of it. The main loop scales whatever is in that variable to volts and prints it to the display.

## Implementation

### The three modes

Single-shot is a start bit and a poll: set the conversion-start bit, spin on the ready bit, read the result register. Everything between those two points is CPU time spent doing nothing.

Free-running flips one enable bit and the ADC self-retriggers. At 48 MHz and 96 clocks per conversion that's roughly 500k samples a second — far faster than any loop is going to read them, which is precisely the problem DMA solves.

### Why the FIFO rather than the result register

Reading the ADC's FIFO register is not a passive read: the act of reading pops the entry and tells the ADC the sample has been consumed. That handshake is what lets the ADC pace the transfer, and it's why the DMA channel is pointed at the FIFO and not at the plain result register.

The pacing signal itself is a DREQ — a peripheral's hardware request line to the DMA, the direct analogue of an IRQ to the CPU. The ADC is configured to raise its DREQ whenever the FIFO has a sample waiting, and the DMA channel is told to wait for that specific request rather than transferring as fast as the bus allows.

### Configuring the channel

A DMA channel is four registers: source address, destination address, transfer count, and a control register.

`trans_count` carries two fields — the number of transfers, and a mode field set here so the channel endlessly re-triggers instead of finishing after one transfer. One sample per trigger, forever.

The control register is built up in a local variable and written last, on purpose. Its `_trig` suffix means writing it starts the channel, so setting the fields directly one at a time would arm the channel partway through configuration. Zeroing it first — a null trigger — guarantees nothing is live while the other registers are being set up. The assembled value carries the transfer size (a half-word, since samples are 12 bits), the ADC's DREQ number in the request-select field, and the enable bit.

### Display

The voltmeter reuses the timer-driven multiplexing from the previous project unchanged: the value is scaled to volts, formatted into a string, and handed to the display, which keeps sweeping its eight digits off its own 3 ms alarm. The two subsystems never coordinate — one writes a buffer, the other reads it.
