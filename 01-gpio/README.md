# GPIO

Register-level digital I/O on the RP2350B — driving LEDs, reading pushbuttons, and scanning a matrix keypad without calling into the Pico SDK's GPIO helpers.

## Hardware

| | |
|---|---|
| **MCU** | RP2350B (Proton development board) |
| **LEDs** | GP22, GP23, GP24, GP25 — outputs |
| **Pushbuttons** | GP21, GP26 — inputs |
| **Keypad rows** | GP2, GP3, GP4, GP5 — inputs |
| **Keypad columns** | GP6, GP7, GP8, GP9 — outputs |

## What it does

**LED sequence.** Lights GP22–GP25 one at a time with a 500 ms gap, then clears them in the same order — a Johnson counter walking across the four user LEDs.

**Pushbutton control.** Polls both buttons every 10 ms. GP21 turns all four LEDs on, GP26 turns them all off, with GP21 taking priority when both are held.

**Keypad scan.** Walks the four column pins one at a time, driving each high, waiting 10 ms for the line to settle, then sampling the matching row pin. A closed key on the diagonal lights its corresponding LED — key 1 to GP22, 5 to GP23, 9 to GP24, D to GP25.

## Implementation

Every pin is brought up by writing hardware registers directly, no `gpio_init` or `gpio_put`. Bringing a pin under SIO control takes three distinct writes, which is the part that isn't obvious from the SDK surface:

1. **Pad configuration** — set the input enable bit and clear output disable in `pads_bank0_hw->io[n]`, so the pad will pass a signal in the first place
2. **Function select** — write `GPIO_FUNC_SIO` into the FUNCSEL field of `io_bank0_hw->io[n].ctrl`, routing the pin to the SIO block rather than a peripheral
3. **Isolation latch** — clear the ISO bit in the pad register. The RP2350 holds pads isolated out of reset so they can't glitch while the function is still being chosen; the pin is inert until this is cleared

Direction and state then come from SIO's atomic set/clear registers rather than read-modify-write on a single value:

| Register | Use |
|---|---|
| `sio_hw->gpio_oe_set` / `gpio_oe_clr` | Enable or disable the output driver |
| `sio_hw->gpio_set` / `gpio_clr` | Drive a pin high or low |
| `sio_hw->gpio_in` | Read the current level of any pin, input or output |

Because these are separate set and clear registers, a write only touches the bits in the mask — there's no read of the current state, so no window for an interrupt to land between the read and the write. All four LEDs change on one instruction.

`sleep_ms` is the only SDK function used.
