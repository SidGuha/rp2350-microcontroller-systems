#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/irq.h"

#include "hardware/adc.h"
#include "hardware/dma.h"



uint32_t adc_fifo_out = 0;
void display_init_pins();
void display_init_timer();
void display_char_print(const char* buffer);
void autotest();



// When testing manual ADC single-shot conversion
//#define STEP2
// When testing manual ADC free-running conversion
//#define STEP3
// When testing automated ADC sampling with DMA
#define STEP4



void init_adc() {
    // 1. enable adc
    adc_init(); //found in Pico SDK it Initialise the ADC HW.

    // 2.making gpio 45 analog
    adc_gpio_init(45); //again found in Pico SDK example

    // 3. select channel 5 using cs.ainsel
    adc_select_input(5);
}

uint16_t read_adc() {
    return adc_read();
}

void init_adc_freerun() {
    adc_init();
    adc_gpio_init(45);
    adc_select_input(5);

    //Enable or disable free-running sampling mode.
    adc_run(true);
}

void init_dma() {
    //dma_channel_set_read_addr();
    dma_channel_hw_addr(0)->read_addr = (uintptr_t) &adc_hw->fifo;

    //dma_channel_set_write_addr();
    dma_channel_hw_addr(0)->write_addr = (uintptr_t) &adc_fifo_out;

    //dma_encode_transfer_count();
    // making TRANS_COUNT mode[31:28] and count[27:0] 
    dma_channel_hw_addr(0)->transfer_count = (1 << 28) | 1;
    dma_channel_hw_addr(0)->ctrl_trig = 0;

    uint32_t temp = 0;
    //where i found needed macros: channel_config_set_transfer_data_size
    temp |= (1 << DMA_CH0_CTRL_TRIG_DATA_SIZE_LSB);

    //where I found macros, channel_config_set_dreq();
    //shifting w "correct val" aka DREQ_ADC = 48(q7)
    temp |= (48 << DMA_CH0_CTRL_TRIG_TREQ_SEL_LSB);

    //setting enable(bit 0)
    temp |= (1 << 0);


    dma_hw->ch[0].ctrl_trig = temp;
}

void init_adc_dma() {
    //step1: First call init_dma. We'll fill in this function later.
    init_dma();

    //step2: Call init_adc_freerun to configure the ADC for free-running 
    //mode, as well as the GPIO pin we used previously as the ADC input.
    init_adc_freerun();

    //step3: Enable the ADC FIFO to store the generated samples.
    
    //found in Pico SDK
    //static void adc_fifo_setup (bool en, bool dreq_en, uint16_t dreq_thresh, bool err_in_fifo, bool byte_shift)
    adc_fifo_setup(true, true, 1, false, false);

}


int main()
{
    // Configures our microcontroller to 
    // communicate over UART through the TX/RX pins
    stdio_init_all();

    //autotest();

    // Step 2 - singleshot
    // #ifdef STEP2
    // init_adc();
    // for(;;) {
    //     printf("ADC Result: %d     \r", read_adc());
    //     // We've found that when we do NOT send a newline character,
    //     // the output is not flushed immediately, which can cause
    //     // the output to be delayed or not appear at all in some cases.
    //     // The fflush function forces a flush.
    //     fflush(stdout);
    //     sleep_ms(250);
    // }
    // #endif

    // // Step 3 - freerun
    // #ifdef STEP3
    // init_adc_freerun();

    // int i = 0;
    // for(;;) {
    //     printf("ADC Result: %d     \r", adc_hw->result);
    //     fflush(stdout);
    //     sleep_ms(250);
    // }
    // #endif

    // // Step 4 - adc_dma
    #ifdef STEP4

    // Don't forget to copy in display.c from lab 3.
    display_init_pins();
    display_init_timer();

    init_adc_dma();
    char buffer[10];
    for(;;) {
        float f = (adc_fifo_out * 3.3) / 4095.0;
        snprintf(buffer, sizeof(buffer), "%1.7f", f);
        display_char_print(buffer);

        // If you need to debug without the display, 
        // you can uncomment the following lines.

        // printf("ADC Result: %s     \r", buffer);
        // fflush(stdout);
        
        sleep_ms(250);
    }
    #endif
    

    for(;;);
    return 0;
}
