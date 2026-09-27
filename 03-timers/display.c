#include "hardware/timer.h"
#include "hardware/irq.h"
#include "hardware/gpio.h"

// 7-segment display message buffer
// Declared as static to limit scope to this file only.
static char msg[8] = {
    0x3F, // seven-segment value of 0
    0x06, // seven-segment value of 1
    0x5B, // seven-segment value of 2
    0x4F, // seven-segment value of 3
    0x66, // seven-segment value of 4
    0x6D, // seven-segment value of 5
    0x7D, // seven-segment value of 6
    0x07, // seven-segment value of 7
};

extern char font[]; // Font mapping for 7-segment display
static int index = 0; // Current index in the message buffer


void display_char_print(const char message[]) {
    for (int i = 0; i < 8; i++) {
        msg[i] = font[message[i] & 0xFF];
    }
}


void display_init_pins() {
    //masking pins 10-20
    uint32_t mask = 0x1FFC00;
    gpio_init_mask(mask);
    gpio_set_dir_out_masked(mask);
}


void display_isr() {
    //Acknowledge the interrupt for ALARM0 on TIMER1.
    timer1_hw->intr = 1 << 0;

    //Set the value of GP20-GP10 to a new 11-bit value 
    //where the provided global variable index is used to select 
    //the seven-segment display to turn on, and the 8-bit value of 
    //msg[index] is used to determine which segments to light up

    //okay so now we are determining the select variable whych basically tells the 
    //decoder which segment to light up
    //it needs an index and a segment then we move the whole ordeal up by 10
    //so that bits 0-10 fit in gpios 10-20
    uint32_t select = ((index << 8) | (msg[index] & 0xFF)) << 10;
    
    //first lets clear everything
    sio_hw->gpio_clr = 0x1FFC00; 

    //now lets set the needed segments
    sio_hw->gpio_set = select;

    //Increment index by 1, and wrap around to 0 after 7.
    index++;
    if (index >= 8)
    {
        index = 0;
    }

    //Make TIMER1 ALARM0 fire the interrupt again in 3 milliseconds.
    uint32_t fire_time = timer1_hw->timerawl + 3000;

    timer1_hw->alarm[0] = fire_time;



}

void display_init_timer() {
    //exclusive handler
    irq_set_exclusive_handler(TIMER1_IRQ_0, display_isr);

    //Enable the interrupt at the timer with a write 
    //to the appropriate alarm bit in INTE (e.g. (1 << 0) for ALARM0)
    hw_set_bits(&timer1_hw->inte, 1 << 0);

    //Enable the appropriate timer interrupt at the processor
    nvic_hw->iser[TIMER1_IRQ_0 / 32] = (1 << (TIMER1_IRQ_0 % 32));

    //Write the time you would like the interrupt to fire to 
    //ALARM0 (i.e. the current value in TIMERAWL plus your desired alarm
    //time in microseconds). Writing the time to the ALARM 
    //register sets the ARMED bit as a side effect

    uint32_t fire_time = timer1_hw->timerawl + 3000;

    timer1_hw->alarm[0] = fire_time;
}

void display_print(const uint16_t message[]) {
    for (int i = 0; i < 8; i++) {
        uint8_t segment = font[message[i] & 0xFF];
        if (message[i] & 0x100) //anding w 1_00000000
        {
            segment |= 0x80;
        }
        

        msg[i] = segment;
    }
    
}