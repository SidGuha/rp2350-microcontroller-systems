# Interrupts

Event-driven input on the RP2350B — replacing polling loops with GPIO edge interrupts, using an interrupt to wake the chip from its lowest power state, and passing keypress events between the two CPU cores over the hardware mailbox.

## Hardware

| | |
|---|---|
| **MCU** | RP2350B (Proton development board) |
| **LEDs** | GP22, GP23, GP24, GP25 — outputs |
| **Pushbuttons** | GP21, GP26 — rising-edge interrupt sources |
| **Keypad rows** | GP2, GP3, GP4, GP5 — rising-edge interrupt sources |
| **Keypad columns** | GP6, GP7, GP8, GP9 — outputs, scanned one at a time |

## What it does

**Sleep and wake on button press.** A rising edge on GP21 clears the LEDs and drops the chip into DORMANT state; a rising edge on GP26 wakes it and lights them again. Between the two, the main loop prints once a second, so the serial output visibly stops and resumes.

**Interrupt-driven keypad.** Instead of continuously reading the row pins, each row raises an interrupt when its line goes high. The scan loop still walks the columns, but the CPU no longer watches for the press — it's told about it.

**Cross-core handoff.** Core 1 owns the keypad entirely: it configures the row interrupts and drives the columns. When a key is pressed, its ISR pushes the character into the inter-processor FIFO. Core 0 blocks on a pop and prints whatever arrives, leaving it free for other work.

## Implementation

### Enabling a GPIO interrupt

Each GPIO's interrupt configuration is packed four bits per pin across an array of registers, so pin *n* lives in index `n / 8` at bit offset `4 * (n % 8)` — GP21 is bits 20–23 of index 2, GP26 is bits 8–11 of index 3. Enabling a rising-edge interrupt means setting the matching bit in `inte`, and acknowledging one means writing that bit back to `intr`.

The enable registers are per-core. `io_bank0_hw->proc0_irq_ctrl` and `proc1_irq_ctrl` are separate banks, and a pin's interrupt is routed only to the core whose bank has the bit set. Since core 1 configures the keypad here, both the enable and the status reads branch on `get_core_num()` — writing to core 0's bank from core 1 would silently produce an interrupt that never arrives.

Beyond the peripheral, the IRQ line itself has to be unmasked at the interrupt controller by setting the `IO_IRQ_BANK0` bit in `nvic_hw->iser[0]`.

Acknowledging is the first thing each ISR does. An edge event latches in `intr` and holds the line asserted, so an unacknowledged interrupt re-fires immediately on return and the CPU makes no forward progress.

### Entering and leaving DORMANT

DORMANT stops the crystal oscillator, which means every clock on the chip stops with it. Getting in and out safely is an ordering problem:

1. **Reparent the system clock** from the PLL to the crystal oscillator with `clock_configure`. The PLL is derived from XOSC, so stopping XOSC while the PLL is still the clock source leaves the PLL unable to relock on wake.
2. **Enter DORMANT.** Execution stops mid-function.
3. **On wake**, a rising edge on GP26 restarts the oscillator and execution resumes at the next line. The sleep-enable registers are restored, `runtime_init_clocks` brings the PLL back to 150 MHz, and `stdio_uart_init` recalculates the UART divisor against the restored clock — without it the baud rate is wrong and `printf` output comes back garbled.

Wake is a separate enable from the ordinary interrupt: GP26's bit is set in `dormant_wake_irq_ctrl` as well as in the core's own bank, so the same edge both wakes the chip and runs the handler.

### Identifying the key

The scan and the interrupt are decoupled, so the ISR reconstructs the keypress from two independent facts: the global `col` tells it which column was being driven when the edge arrived, and the pending-event bits in `ints` tell it which row fired. Together they index the keymap.

`drive_column` holds each column high for 25 ms before advancing. The delay lets current settle through the matrix so a closed key actually pulls its row high, and it bounds the worst-case latency between a press and its report.

### Mailbox FIFO

`multicore_fifo_push_blocking` in the ISR and `multicore_fifo_pop_blocking` in core 0's main loop form the handoff. The pop blocks, so core 0 idles until core 1 reports something rather than spinning on a shared variable — the FIFO provides the synchronization, so no lock is needed around the transfer.
