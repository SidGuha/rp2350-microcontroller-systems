#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/xosc.h"
#include "pico/multicore.h"
#include "hardware/pll.h"
#include "hardware/clocks.h"


void autotest();
const char keymap[16] = "DCBA#9630852*741";
char key = '\0';
int col = 0;


// Only uncomment ONE step at a time.
// When testing init_gpio_irq
//#define STEP1
// When testing init_keypad_irq
//#define STEP2
// When testing init_fifo_irq
#define STEP3



void init_outputs() {
    //gpio_init
    u_int32_t mask_ini = (1 << 22) | (1 << 23) | (1 << 24) | (1 << 25);
    //set dir and put
    sio_hw->gpio_oe_set = mask_ini;
    sio_hw->gpio_set = mask_ini;

    pads_bank0_hw->io[22] = (pads_bank0_hw->io[22] | PADS_BANK0_GPIO0_IE_BITS) & ~PADS_BANK0_GPIO0_OD_BITS;
    io_bank0_hw->io[22].ctrl = GPIO_FUNC_SIO << IO_BANK0_GPIO0_CTRL_FUNCSEL_LSB;
    pads_bank0_hw->io[22] &= ~PADS_BANK0_GPIO0_ISO_BITS;

    pads_bank0_hw->io[23] = (pads_bank0_hw->io[23] | PADS_BANK0_GPIO0_IE_BITS) & ~PADS_BANK0_GPIO0_OD_BITS;
    io_bank0_hw->io[23].ctrl = GPIO_FUNC_SIO << IO_BANK0_GPIO0_CTRL_FUNCSEL_LSB;
    pads_bank0_hw->io[23] &= ~PADS_BANK0_GPIO0_ISO_BITS;

    pads_bank0_hw->io[24] = (pads_bank0_hw->io[24] | PADS_BANK0_GPIO0_IE_BITS) & ~PADS_BANK0_GPIO0_OD_BITS;
    io_bank0_hw->io[24].ctrl = GPIO_FUNC_SIO << IO_BANK0_GPIO0_CTRL_FUNCSEL_LSB;
    pads_bank0_hw->io[24] &= ~PADS_BANK0_GPIO0_ISO_BITS;

    pads_bank0_hw->io[25] = (pads_bank0_hw->io[25] | PADS_BANK0_GPIO0_IE_BITS) & ~PADS_BANK0_GPIO0_OD_BITS;
    io_bank0_hw->io[25].ctrl = GPIO_FUNC_SIO << IO_BANK0_GPIO0_CTRL_FUNCSEL_LSB;
    pads_bank0_hw->io[25] &= ~PADS_BANK0_GPIO0_ISO_BITS;
}

void init_inputs() {
    
    uint32_t mask1 = (1 << 21) | (1 << 26);
    sio_hw->gpio_oe_clr = mask1;

    pads_bank0_hw->io[21] = (pads_bank0_hw->io[21] | PADS_BANK0_GPIO0_IE_BITS) & ~PADS_BANK0_GPIO0_OD_BITS;
    io_bank0_hw->io[21].ctrl = GPIO_FUNC_SIO << IO_BANK0_GPIO0_CTRL_FUNCSEL_LSB;
    pads_bank0_hw->io[21] &= ~PADS_BANK0_GPIO0_ISO_BITS;

   

    pads_bank0_hw->io[26] = (pads_bank0_hw->io[26] | PADS_BANK0_GPIO0_IE_BITS) & ~PADS_BANK0_GPIO0_OD_BITS;;
    io_bank0_hw->io[26].ctrl = GPIO_FUNC_SIO << IO_BANK0_GPIO0_CTRL_FUNCSEL_LSB;
    pads_bank0_hw->io[26] &= ~PADS_BANK0_GPIO0_ISO_BITS;
}

void init_keypad() {
    
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
    sio_hw->gpio_set = mask_ini2;
    
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


void gpio_isr() {
    
    //gpio_get_irq_event_mask()
    //gpio_acknowledge_irq()

    uint32_t mask_isr1 = (io_bank0_hw->intr[2] >> (20)) & 0xf;
    uint32_t mask_isr2 = (io_bank0_hw->intr[3] >> (8)) & 0xf;
    if (mask_isr1 & GPIO_IRQ_EDGE_RISE)
    {
        //gpio_acknowledge_irq();
        io_bank0_hw->intr[2] = GPIO_IRQ_EDGE_RISE << (20);

        //22-25 clr
        sio_hw->gpio_clr = (1 << 22) | (1 << 23) | (1 << 24) | (1 << 25);
        //make dormant
        //xosc_dormant();
        clock_configure(clk_sys,
            CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLK_REF,  // Clock source is the crystal oscillator, no PLLs
            0,                                      // Using glitchless mux (see 8.1.3.2 Multiplexers)
            XOSC_HZ,                                // What is the frequency of new clock source?   
            XOSC_HZ                                 // What frequency do we want clk_sys to be?
        );

        xosc_dormant();

        clocks_hw->sleep_en0 |= ~(0u);  // Set to all ones
        clocks_hw->sleep_en1 |= ~(0u);  // Set to all ones

        runtime_init_clocks();

        stdio_uart_init();
    }
    else if(mask_isr2 & GPIO_IRQ_EDGE_RISE)
    {
        //gpio_acknowledge_irq();
        io_bank0_hw->intr[3] = GPIO_IRQ_EDGE_RISE << (8);

        //22-25 set
        sio_hw->gpio_set = (1 << 22) | (1 << 23) | (1 << 24) | (1 << 25);

    }

}


void init_gpio_irq() {
    
    //22-25 config
    u_int32_t mask_f2_1 = (1 << 22) | (1 << 23) | (1 << 24) | (1 << 25);
    sio_hw->gpio_set = mask_f2_1;

    //21 and 26 config
    u_int32_t mask_f2_2 = (1 << 21) | (1 << 26);

    //gpio_add_raw_irq_handler_masked();
    gpio_add_raw_irq_handler_masked(mask_f2_2, gpio_isr);
    
    
    //21 is in inte[2] 26 in inte[3]
    //20 n 8 are corresponding bit ranges
    hw_set_bits(&io_bank0_hw->proc0_irq_ctrl.inte[2], GPIO_IRQ_EDGE_RISE << 20);

    hw_set_bits(&io_bank0_hw->proc0_irq_ctrl.inte[3], GPIO_IRQ_EDGE_RISE << 8);

    //dormant
    hw_set_bits(&io_bank0_hw->dormant_wake_irq_ctrl.inte[3], GPIO_IRQ_EDGE_RISE << 8);

    hw_set_bits(&nvic_hw->iser[0], 1 << IO_IRQ_BANK0);

    nvic_hw->iser[0] = (1 << IO_IRQ_BANK0);

}


void drive_column() {
    for (int i = 6; i < 10; i++)
    {
        if (i == col + 6)
        {
            sio_hw->gpio_set = (1 << i);
        }
        else
        {
            sio_hw->gpio_clr = (1 << i);
        }
    }
    


    sleep_ms(25);
    col++;
    if (col >= 4)
    {
        col = 0;
    }
    
}

void keypad_isr() {
    
    for (int i = 2; i <= 5; i++)
    {   
        u_int32_t mask_ki;
        
        
        if (get_core_num() == 0)
        {
            mask_ki = (io_bank0_hw->proc0_irq_ctrl.ints[i / 8] >> (4 * (i % 8))) & 0xfu;
        }
        else
        {
            mask_ki = (io_bank0_hw->proc1_irq_ctrl.ints[i / 8] >> (4 * (i % 8))) & 0xfu;
        }

        if (mask_ki & GPIO_IRQ_EDGE_RISE)
        {
            io_bank0_hw->intr[i / 8] = GPIO_IRQ_EDGE_RISE << (4 * (i % 8));
            int index = col * 4 + (i - 2);
#ifdef STEP3
        multicore_fifo_push_blocking(keymap[index]);
#else
        printf("%c pressed\n", keymap[index]);
#endif
        }
        
        
        
    }
    
}

void init_keypad_irq() {
    
    u_int32_t mask_iki = (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5);
    gpio_add_raw_irq_handler_masked(mask_iki, keypad_isr);

    for(int i = 2; i < 6; i++){
        
        u_int32_t mask_iki2 = GPIO_IRQ_EDGE_RISE << (4 * (i % 8));
        if (get_core_num() == 0) {
            hw_set_bits(&io_bank0_hw->proc0_irq_ctrl.inte[i / 8], mask_iki2);
        } else {
            hw_set_bits(&io_bank0_hw->proc1_irq_ctrl.inte[i / 8], mask_iki2);
        }
    }

    nvic_hw->iser[0] = (1 << IO_IRQ_BANK0);
}

void init_fifo_irq() {

    //printf("Core 1 started\n");
    //printf("Core 1 started, core_num=%d\n", get_core_num());
    init_keypad_irq();
    for (;;)
    {
        drive_column();
    }
}

// Main

int main() {
    // Configures our microcontroller to 
    // communicate over UART through the TX/RX pins
    stdio_init_all();

    // Uncomment when you need to run autotest.
    // Keep this commented out until you need it
    // since it adds a lot of time to the upload process.
    //autotest();


    init_inputs();
    init_outputs();
    init_keypad();


    
    // #ifdef STEP1
    //     init_gpio_irq();
    //     for(;;) {
    //         printf("Hello world\n");
    //         sleep_ms(1000);
    //     }
    // #endif
    // #ifdef STEP2
    //     init_keypad_irq();
    //     for (;;) {
    //         drive_column();
    //     }
    // #endif
    #ifdef STEP3
        // launch the function init_fifo_irq on core 1.
        multicore_reset_core1();
        multicore_launch_core1(init_fifo_irq);
        printf("Core 0 ready, waiting for FIFO...\n");
        for(;;) {
            // pop a value from the FIFO and store it in global variable `key`.
            key = multicore_fifo_pop_blocking();
            // print the value of `key` to the console.
            printf("%c is pressed\n", key);
        }
    #endif
    
    // // Never reached.
    for(;;);
    return 0;
}
