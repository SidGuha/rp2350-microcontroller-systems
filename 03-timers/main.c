#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/irq.h"


void keypad_init_pins();
void keypad_init_timer();
void display_init_pins();
void display_init_timer();
void display_print(const uint16_t message[]);
uint32_t key_pop();
void autotest();
void game();

extern char font[]; // Font mapping for 7-segment display



// Only uncomment ONE step at a time.

// When testing keypad
//#define STEP2
// When testing seg7
#define STEP3
// When playing the game
// #define STEP4


int main()
{
    // Configures our microcontroller to 
    // communicate over UART through the TX/RX pins
    stdio_init_all();


    //autotest();

    // #ifdef STEP2
    // keypad_init_pins();
    // keypad_init_timer();

    // for (;;) {
    //     uint32_t keyevent = key_pop();
    //     if (keyevent & (1 << 8))
    //         printf("Pressed: %c\n", (char) (keyevent & 0xFF));
    //     else if (keyevent != 0)
    //         printf("Released: %c\n", (char) (keyevent & 0xFF));
    // }
    // #endif

    #ifdef STEP3
        uint16_t message[8]; // 7-segment display message buffer
        memset(message, 0, sizeof(message)); // Initialize message buffer to zero

        keypad_init_pins();
        keypad_init_timer();
        
        display_init_pins();
        display_init_timer();

        for (;;) {
            uint32_t keyevent = key_pop();
            if (keyevent != 0) {
                // push the event into message
                for (int i = 0; i < 7; i++)
                    message[i] = message[i + 1];
                message[7] = keyevent;
    
                display_print(message);
            }
        }
    #endif

    // #ifdef STEP4
    //     game();
    // #endif

    for(;;);
    return 0;
}
