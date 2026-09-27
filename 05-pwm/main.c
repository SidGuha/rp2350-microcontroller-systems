#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "queue.h"
#include "support.h"


static int duty_cycle = 0;
static int dir = 0;
static int color = 0;
int current_period;

void display_init_pins();
void display_init_timer();
void display_char_print(const char message[]);
void keypad_init_pins();
void keypad_init_timer();
void init_wavetable(void);
void set_freq(int chan, float f);
extern KeyEvents kev;
void drum_machine();


// When testing static duty-cycle PWM
#define STEP2
// When testing variable duty-cycle PWM
// #define STEP3
// When testing 8-bit audio synthesis
// #define STEP4
// When trying out drum machine
// #define DRUM_MACHINE


//pwm_get_counter();
void init_pwm_static(uint32_t period, uint32_t duty_cycle) {
    //step 1: Configure pins 37, 38, 39 as PWM outputs.
    gpio_set_function(37, GPIO_FUNC_PWM);
    gpio_set_function(38, GPIO_FUNC_PWM);
    gpio_set_function(39, GPIO_FUNC_PWM);

    //step 2: Set the corresponding PWM slice's 
    //clock divider value to be 150.
    uint slice_num1 = pwm_gpio_to_slice_num(37);
    uint slice_num2 = pwm_gpio_to_slice_num(38);

    //turns out its redundant cuz gpio38 n 39 both correspond to pwm channel 11
    uint slice_num3 = pwm_gpio_to_slice_num(39); 

    pwm_set_clkdiv(slice_num1, 150);
    pwm_set_clkdiv(slice_num2, 150);

    //redundant
    pwm_set_clkdiv(slice_num3, 150);

    //channel_config_set_ring()
    //pwm_set_wrap

    //step 3: Set the wrapping counter value of the pin's 
    //corresponding PWM slice to the value passed in period minus 1
    pwm_set_wrap(slice_num1, period - 1);
    pwm_set_wrap(slice_num2, period - 1);

    //redundant again
    pwm_set_wrap(slice_num3, period - 1);

    //step 4: For each of the pins, set the corresponding 
    //PWM channel's duty cycle to the value passed in duty_cycle 
    pwm_set_chan_level(slice_num1, pwm_gpio_to_channel(37), duty_cycle);
    pwm_set_chan_level(slice_num2, pwm_gpio_to_channel(38), duty_cycle);

    //redundant
    pwm_set_chan_level(slice_num3, pwm_gpio_to_channel(39), duty_cycle);

    //step 5: Enable the PWM signals for all pins.
    pwm_set_enabled(slice_num1, true);
    pwm_set_enabled(slice_num2, true);

    //redundant
    pwm_set_enabled(slice_num3, true); 
}

void pwm_breathing() {
    //step1: acknowledge the interrupt
    uint slice_num = pwm_gpio_to_slice_num(37);
    pwm_hw->intr = (1 << slice_num);

    //step2: if dir is 0 and duty_cycle is 100, increment color modulo 3
    if (dir == 0 && duty_cycle == 100)
    {
        color = (color + 1) % 3;
    }
    
    //step3: if duty_cycle is 100 and dir is 0, set dir to 1
    //else if duty_cycle is 0 and dir is 1, set dir to 0
    if (duty_cycle == 100 && dir == 0)
    {
        dir = 1;
    }
    else if (duty_cycle == 0 && dir == 1)
    {
        dir = 0;
    }

    //step4: if dir is 0, increment duty_cycle by 1
    //else, decrement duty_cycle by 1
    if (dir == 0)
    {
        duty_cycle++;
    }
    else
    {
        duty_cycle--;
    }

    //step5: set the chosen color's duty cycle to the ratio of the 
    //current duty_cycle to the current period of the PWM signal 
    //(multiply by period value, divide by 100)
    pwm_set_gpio_level(37 + color, duty_cycle * current_period / 100);        
}

void init_pwm_irq() {
    //step1: Within the PWM peripheral registers, enable interrupts for 
    //the PWM slice associated with GP37, associated with the first PWM wrap interrupt
    uint slice_num = pwm_gpio_to_slice_num(37);
    pwm_set_irq0_enabled(slice_num, true);

    //step2: Set pwm_breathing as the exclusive handler when the PWM slice associated 
    //with GP37's counter wraps around to 0
    irq_set_exclusive_handler(PWM_IRQ_WRAP_0 , pwm_breathing);

    //step3: Enable interrupts for the aforementioned PWM the PWM slice associated with GP37 interrupt.
    irq_set_enabled(PWM_IRQ_WRAP_0, true);

    //step4: Obtain the current period for PWM the PWM slice associated with 
    //GP37 via the appropriate register, and store it in a new variable called current_period.
    current_period = pwm_hw->slice[slice_num].top + 1;

    //step5: Set the global variable duty_cycle to 100 and dir to 1
    duty_cycle = 100;
    dir = 1;

    //pwm_set_gpio_level();
    //step6: Set the duty cycles of the three RGB LED PWM outputs to be equal to current_period
    pwm_set_gpio_level(37, current_period);
    pwm_set_gpio_level(38, current_period);
    pwm_set_gpio_level(39, current_period);
}

void pwm_audio_handler() {
    //step1: Acknowledge the corresponding interrupt.
    uint slice_num = pwm_gpio_to_slice_num(36);
    pwm_hw->intr = (1 << slice_num);

    //step2: increment offset0 by step0 and offset1 by step1
    offset0 = offset0 + step0;
    offset1 = offset1 + step1;

    //step3: offset0 and offset1 condition
    if (offset0 >= (N << 16))
    {
        offset0 = offset0 - (N << 16);
    }

    if (offset1 >= (N << 16))
    {
        offset1 = offset1 - (N << 16);
    }

    //step4: Create a new variable called samp, initialized to the sum 
    //of wavetable[offset0 >> 16] + wavetable[offset1 >> 16]
    int samp = wavetable[offset0 >> 16] + wavetable[offset1 >> 16];

    //step5: Divide samp by 2
    samp =  samp / 2;

    //step6: scale samp to the range of the PWM duty cycle by multiplying 
    //it by the period value of the PWM slice, and then dividing it by 1 << 16
    //example on page 55
    samp = samp * (pwm_hw->slice[slice_num].top + 1) / (1 << 16);

    //step7: Write the value of samp to the PWM slice's duty cycle register
    pwm_hw->slice[slice_num].cc = samp;

}

void init_pwm_audio() {
    // fill in

    //step1: Configure pin 36 as a PWM output.
    gpio_set_function(36, GPIO_FUNC_PWM);

    //step2: Set the corresponding PWM slice's clock divider value to be 150
    uint slice_num = pwm_gpio_to_slice_num(36);
    pwm_set_clkdiv(slice_num, 150);

    //step3: Set the period of the PWM signal to be the frequency of the clock 
    //after division above in Hz divided by the sampling rate
    pwm_set_wrap(slice_num, (1000000 / RATE) - 1);

    //step4: Initialize the duty cycle to 0.
    pwm_set_chan_level(slice_num, pwm_gpio_to_channel(36), 0);

    //step5: Call init_wavetable with no arguments
    init_wavetable();

    //step6: enable IRQ in the PWM peripheral.
    pwm_set_irq_enabled(slice_num, true);

    //step7: Set pwm_audio_handler as the exclusive handler
    irq_set_exclusive_handler(PWM_IRQ_WRAP_0 , pwm_audio_handler);

    //step8: Enable the interrupt associated with your chosen PWM interrupt number.
    irq_set_enabled(PWM_IRQ_WRAP_0, true);

    //step9: Enable the PWM slice.
    pwm_set_enabled(slice_num, true);

}


int main()
{
    // Configures our microcontroller to 
    // communicate over UART through the TX/RX pins
    stdio_init_all();

    
    // autotest();

    // Make sure to copy in the latest display.c and keypad.c from your previous labs.
    keypad_init_pins();
    keypad_init_timer();
    display_init_pins();
    display_init_timer();

    #ifdef STEP2
    init_pwm_static(100, 50); // Start out with 500/1000, 50%
    display_char_print("      50");
    uint16_t percent = 50; // Set initial percentage for duty cycle, displayed 
    uint16_t disp_buffer = 0;
    char buf[9];

    // Display initial duty cycle
    snprintf(buf, sizeof(buf), "      50");
    display_char_print(buf);

    bool new_entry = true;  // Flag to track if we're starting a new entry
    
    for (;;) {
        uint16_t keyevent = key_pop(); // Pop a key event from the queue
        if (keyevent & 0x100) {
            char key = keyevent & 0xFF;
            if (key >= '0' && key <= '9') {
                // If the key is a digit, check if we need to clear the buffer first
                if (new_entry) {
                    disp_buffer = 0;  // Clear the buffer for new entry
                    new_entry = false;  // No longer a new entry
                }
                // Shift into buffer
                disp_buffer = (disp_buffer * 10) + (key - '0');
                snprintf(buf, sizeof(buf), "%8d", disp_buffer);
                display_char_print(buf); // Display the new value
            } else if (key == '#') {
                // If the key is '#', set the duty cycle
                percent = disp_buffer;
                if (percent > 100) {
                    percent = 100; // Cap at 100%
                }
                init_pwm_static(100, percent); // Update PWM with new duty cycle
                snprintf(buf, sizeof(buf), "%8d", percent);
                display_char_print(buf); // Display the new duty cycle
                new_entry = true;  // Ready for new entry
            }
            else if (key == '*') {
                // If the key is '*', reset the buffer
                disp_buffer = 50;
                percent = 50;
                init_pwm_static(100, percent); // Reset PWM to 50% duty cycle
                snprintf(buf, sizeof(buf), "      50");
                display_char_print(buf); // Display reset
                new_entry = true;  // Ready for new entry
            }
            else {
                // Any other key also starts a new entry
                new_entry = true;
            }
        }
    }
    #endif

    #ifdef STEP3
    init_pwm_static(10000, 5000); // Start out with 500/1000, 50%
    init_pwm_irq(); // Initialize PWM IRQ for variable duty cycle

    for(;;) {
        // The handler manages everything from now on.
        // Use the CPU to do something else!
        tight_loop_contents();
    }
    #endif
    
    #ifdef STEP4
    char freq_buf[9] = {0};
    int pos = 0;
    bool decimal_entered = false;
    int decimal_pos = 0;
    int current_channel = 0;

    keypad_init_pins();
    keypad_init_timer();
    display_init_pins();
    display_init_timer();

    init_pwm_audio(); 

    // set_freq(0, 440.0f); // Set initial frequency to 440 Hz (A4 note)
    // set_freq(1, 0.0f); // Turn off channel 1 initially
    // set_freq(0, 261.626f);
    // set_freq(1, 329.628f);

    set_freq(0, 440.0f); // Set initial frequency for channel 0
    display_char_print(" 440.000 ");

    for(;;) {
        uint16_t keyevent = key_pop();

        if (keyevent & 0x100) {
            char key = keyevent & 0xFF;
            if (key == 'A') {
                current_channel = 0;
                pos = 0;
                freq_buf[0] = '\0';
                decimal_entered = false;
                decimal_pos = 0;
                display_char_print("         ");
            } else if (key == 'B') {
                current_channel = 1;
                pos = 0;
                freq_buf[0] = '\0';
                decimal_entered = false;
                decimal_pos = 0;
                display_char_print("         ");
            } else if (key >= '0' && key <= '9') {
                if (pos == 0) {
                    snprintf(freq_buf, sizeof(freq_buf), "        "); // Clear buffer on first digit
                    display_char_print(freq_buf);
                }
                if (pos < 8) {
                    freq_buf[pos++] = key;
                    freq_buf[pos] = '\0';
                    display_char_print(freq_buf);
                    if (decimal_entered) decimal_pos++;
                }
                } else if (key == '*') {
                if (!decimal_entered && pos < 7) {
                    freq_buf[pos++] = '.';
                    freq_buf[pos] = '\0';
                    display_char_print(freq_buf);
                    decimal_entered = true;
                    decimal_pos = 0;
                }
                } else if (key == '#') {
                float freq = 0.0f;
                if (decimal_entered) {
                    freq = strtof(freq_buf, NULL);
                } else {
                    freq = (float)atoi(freq_buf);
                }
                set_freq(current_channel, freq);
                snprintf(freq_buf, sizeof(freq_buf), "%8.3f", freq);
                display_char_print(freq_buf);
                pos = 0;
                freq_buf[0] = '\0';
                decimal_entered = false;
                decimal_pos = 0;
            } else {
                // Reset on any other key
                pos = 0;
                freq_buf[0] = '\0';
                decimal_entered = false;
                decimal_pos = 0;
                display_char_print("        ");
            }
        }
    }
    #endif

    #ifdef DRUM_MACHINE
        drum_machine();
    #endif

    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }

    for(;;);
    return 0;
}
