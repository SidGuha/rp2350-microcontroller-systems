#include "pico/stdlib.h"
#include <hardware/gpio.h>
#include <stdio.h>
#include "queue.h"

// Global column variable
int col = -1;

// Global key state
static bool state[16]; // Are keys pressed/released

// Keymap for the keypad
const char keymap[17] = "DCBA#9630852*741";

// Defined here to avoid circular dependency issues with autotest

KeyEvents kev = { 
    .head = 0, 
    .tail = 0 
};


uint8_t keypad_read_rows() {
    return ((gpio_get_all() >> 2) & 0xF);
    
}

void keypad_drive_column() {
    

    //acknowledging alarm0 on timer0
    timer0_hw->intr = 1 << 0;

    col++;
    if (col >= 4)
    {
        col = 0;
    }
    
    sio_hw->gpio_clr = 0x3C0; //this pins 6-9
    sio_hw->gpio_set = (1 << (col + 6));

    timer0_hw->alarm[0] = timer0_hw->timerawl + 25000;
    
}

void keypad_isr() {
    timer0_hw->intr = 1 << 1;

    uint8_t rows = keypad_read_rows();
    int i;
    for ( i = 0; i < 4; i++)
    {
        int idx = col * 4 + i;
        bool is_pressed = (rows >> i) & 1;

        if (is_pressed && state[idx] == 0)
        {   
            state[idx] = true;
            key_push((1<<8) | keymap[idx]);
            
        }
        else if (!is_pressed && state[idx] == 1)
        {
            state[idx] = false;
            key_push((0<<8) | keymap[idx]);
        }
        
        
    }

    timer0_hw->alarm[1] = timer0_hw->timerawl + 25000;
       
}


void keypad_init_pins() {
    
    //inputs
    uint32_t mask2 = (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5);
    sio_hw->gpio_oe_clr = mask2;

    pads_bank0_hw->io[2] = (pads_bank0_hw->io[2] | PADS_BANK0_GPIO0_IE_BITS) & ~PADS_BANK0_GPIO0_OD_BITS;
    io_bank0_hw->io[2].ctrl = GPIO_FUNC_SIO << IO_BANK0_GPIO0_CTRL_FUNCSEL_LSB;
    pads_bank0_hw->io[2] &= ~PADS_BANK0_GPIO0_ISO_BITS;

    pads_bank0_hw->io[3] = (pads_bank0_hw->io[3] | PADS_BANK0_GPIO0_IE_BITS) & ~PADS_BANK0_GPIO0_OD_BITS;
    io_bank0_hw->io[3].ctrl = GPIO_FUNC_SIO << IO_BANK0_GPIO0_CTRL_FUNCSEL_LSB;
    pads_bank0_hw->io[3] &= ~PADS_BANK0_GPIO0_ISO_BITS;

    pads_bank0_hw->io[4] = (pads_bank0_hw->io[4] | PADS_BANK0_GPIO0_IE_BITS) & ~PADS_BANK0_GPIO0_OD_BITS;
    io_bank0_hw->io[4].ctrl = GPIO_FUNC_SIO << IO_BANK0_GPIO0_CTRL_FUNCSEL_LSB;
    pads_bank0_hw->io[4] &= ~PADS_BANK0_GPIO0_ISO_BITS;

    pads_bank0_hw->io[5] = (pads_bank0_hw->io[5] | PADS_BANK0_GPIO0_IE_BITS) & ~PADS_BANK0_GPIO0_OD_BITS;
    io_bank0_hw->io[5].ctrl = GPIO_FUNC_SIO << IO_BANK0_GPIO0_CTRL_FUNCSEL_LSB;
    pads_bank0_hw->io[5] &= ~PADS_BANK0_GPIO0_ISO_BITS;

    //outputs
    u_int32_t mask_ini2 = (1 << 6) | (1 << 7) | (1 << 8) | (1 << 9);
    sio_hw->gpio_oe_set = mask_ini2;
    sio_hw->gpio_clr = mask_ini2;
    
    pads_bank0_hw->io[6] = (pads_bank0_hw->io[6] | PADS_BANK0_GPIO0_IE_BITS) & ~PADS_BANK0_GPIO0_OD_BITS;
    io_bank0_hw->io[6].ctrl = GPIO_FUNC_SIO << IO_BANK0_GPIO0_CTRL_FUNCSEL_LSB;
    pads_bank0_hw->io[6] &= ~PADS_BANK0_GPIO0_ISO_BITS;

    pads_bank0_hw->io[7] = (pads_bank0_hw->io[7] | PADS_BANK0_GPIO0_IE_BITS) & ~PADS_BANK0_GPIO0_OD_BITS;
    io_bank0_hw->io[7].ctrl = GPIO_FUNC_SIO << IO_BANK0_GPIO0_CTRL_FUNCSEL_LSB;
    pads_bank0_hw->io[7] &= ~PADS_BANK0_GPIO0_ISO_BITS;

    pads_bank0_hw->io[8] = (pads_bank0_hw->io[8] | PADS_BANK0_GPIO0_IE_BITS) & ~PADS_BANK0_GPIO0_OD_BITS;
    io_bank0_hw->io[8].ctrl = GPIO_FUNC_SIO << IO_BANK0_GPIO0_CTRL_FUNCSEL_LSB;
    pads_bank0_hw->io[8] &= ~PADS_BANK0_GPIO0_ISO_BITS;

    pads_bank0_hw->io[9] = (pads_bank0_hw->io[9] | PADS_BANK0_GPIO0_IE_BITS) & ~PADS_BANK0_GPIO0_OD_BITS;
    io_bank0_hw->io[9].ctrl = GPIO_FUNC_SIO << IO_BANK0_GPIO0_CTRL_FUNCSEL_LSB;
    pads_bank0_hw->io[9] &= ~PADS_BANK0_GPIO0_ISO_BITS;
}

void keypad_init_timer() {
    irq_set_exclusive_handler(TIMER0_IRQ_0, keypad_drive_column);
    irq_set_exclusive_handler(TIMER0_IRQ_1, keypad_isr);

    //Enable the interrupt at the timer with a write 
    //to the appropriate alarm bit in INTE (e.g. (1 << 0) for ALARM0)
    u_int32_t mask1 = 1u << 0;
    u_int32_t mask2 = 1u << 1;

    hw_set_bits(&timer0_hw->inte, mask1);
    hw_set_bits(&timer0_hw->inte, mask2);

    //Enable the appropriate timer interrupt at the processor
    nvic_hw->iser[TIMER0_IRQ_0 / 32] = (1 << (TIMER0_IRQ_0 % 32));
    nvic_hw->iser[TIMER0_IRQ_1 / 32] = (1 << (TIMER0_IRQ_1 % 32));

    //Write the time you would like the interrupt to fire to 
    //ALARM0 (i.e. the current value in TIMERAWL plus your desired alarm
    //time in microseconds). Writing the time to the ALARM 
    //register sets the ARMED bit as a side effect
    u_int32_t fire_time = timer0_hw->timerawl + 1000000;

    timer0_hw->alarm[0] = fire_time;
    
    timer0_hw->alarm[1] = fire_time + 10000;

}
