# Timers

Timer-driven keypad scanning and 7-segment multiplexing on the RP2350B — two peripherals kept alive by periodic alarms instead of blocking delays, leaving the main loop free to consume events.

## Hardware

| | |
|---|---|
| **MCU** | RP2350B (Proton development board) |
| **Keypad rows** | GP2–GP5 — inputs |
| **Keypad columns** | GP6–GP9 — outputs, one driven at a time |
| **Display select** | GP18–GP20 — address lines of a 74HC138 3-to-8 decoder |
| **Display segments** | GP10–GP17 — data lines of a TLC59211 sink driver |

Eight seven-segment digits share one set of segment lines. The decoder selects which digit's common anode is energized and the sink driver pulls the lit segments to ground, so only one digit is physically on at any instant — 11 pins instead of the 64 a direct drive would need.

## What it does

The keypad and the display each run off their own timer, with no `sleep` anywhere:

- **TIMER0 ALARM0**, every 25 ms — advances to the next column and drives it high
- **TIMER0 ALARM1**, every 25 ms but offset 10 ms behind ALARM0 — samples the rows and emits press/release events
- **TIMER1 ALARM0**, every 3 ms — lights the next digit of the display

The main loop does nothing but pop key events and shift them into the display buffer. Holding a key shows it with its decimal point lit; releasing it re-emits the same character with the point off, and the message scrolls left as more keys arrive.

## Implementation

### Alarms

Each alarm is one-shot. Arming it means writing an absolute future timestamp — the current value of `timerawl` plus the desired delay in microseconds — into `alarm[n]`, which sets the ARMED bit as a side effect. A repeating alarm is therefore an ISR that re-arms itself on the way out, which is what the last line of each handler here does.

Acknowledging is write-one-to-clear: `timer0_hw->intr = 1 << n`. Read-modify-write helpers don't work on this register, since reading it back and OR-ing would clear bits that happen to be set for other alarms.

Setup is three steps per alarm: register the handler (exclusively, so nothing else shares the vector), set the alarm's bit in the timer's `inte`, and unmask the timer's IRQ line at the interrupt controller in `nvic_hw->iser`.

### Why two alarms for one keypad

Earlier versions of this scan drove a column, slept 25 ms for the line to settle, then read the rows — burning the settling time in a blocking delay. Splitting the work across two alarms 10 ms apart keeps the settling delay but spends it running other code. The 10 ms offset *is* the delay; it just no longer belongs to the CPU.

`keypad_drive_column` clears all four column pins and sets one in two atomic SIO writes, then re-arms 25 ms out. `keypad_isr` fires 10 ms later, when the driven line has settled, and reads all four rows in a single masked read.

### Press and release events

A `state[16]` array holds the last known state of every key, so the ISR reports edges rather than levels: a row that reads high with `state` low is a new press, a row that reads low with `state` high is a release, and anything else is the key simply continuing to be held. Only transitions are pushed.

Each event is a 9-bit value — bit 8 is the press/release flag, bits 7–0 are the key's ASCII code — pushed into a FIFO the main loop drains with `key_pop`. Decoupling through a queue means the ISR never blocks on a `printf` or a display update, and events keep their ordering even when several arrive within one scan.

Keys on different rows can be held simultaneously; two on the same row alias to one line and can't be distinguished, which is the standard cost of a multiplexed matrix.

### Multiplexing the display

`display_isr` composes the full 11-bit bus in one expression — digit index in the top three bits, segment pattern in the low eight — shifts it up to line up with GP10, then clears the field and writes it. At 3 ms per digit the whole eight-digit sweep takes 24 ms, fast enough that persistence of vision shows all eight lit at once.

`display_print` converts key events into segment patterns through a font table indexed by ASCII, setting bit 7 for the decimal point when the event's press flag is set. The display buffer holds patterns, not characters, so the ISR never does a lookup — it just pushes bytes.
