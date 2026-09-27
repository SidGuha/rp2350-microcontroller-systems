#include <stdio.h>
#include "pico/stdlib.h"


void init_outputs() {
    

    //gpio_init
    u_int32_t mask_ini = (1 << 22) | (1 << 23) | (1 << 24) | (1 << 25);
    //set dir and put
    sio_hw->gpio_oe_set = mask_ini;
    sio_hw->gpio_set = mask_ini;

    //set function bs
    //basic flow is:
    //1. input enable, output disable
    //2. route gpio pin to sio using the ctrl register
    //3. removed the isolated things using ISO wtv macros/parameters so pin comes to life?
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

int main() {
    
    stdio_init_all();
    
    init_outputs();
    for(;;){
        sio_hw->gpio_set = 1 << 22;
        sleep_ms(500);

        sio_hw->gpio_set = 1 << 23;
        sleep_ms(500);

        sio_hw->gpio_set = 1 << 24;
        sleep_ms(500);

        sio_hw->gpio_set = 1 << 25;
        sleep_ms(500);

        sio_hw->gpio_clr = 1 << 22;
        sleep_ms(500);
        
        sio_hw->gpio_clr = 1 << 23;
        sleep_ms(500);

        sio_hw->gpio_clr = 1 << 24;
        sleep_ms(500);

        sio_hw->gpio_clr = 1 << 25;
        sleep_ms(500);
    }

    init_inputs();

    u_int32_t leds = (1 << 22) | (1 << 23) | (1 << 24) | (1 << 25);
    u_int32_t press21 = (1 << 21);
    u_int32_t press26 = (1 << 26);

    for(;;){    
        u_int32_t input = sio_hw->gpio_in;


        if (input & press21)
        {
            sio_hw->gpio_set = leds;
        }
        else if (input & press26)
        {
            sio_hw->gpio_clr = leds;
        }
        sleep_ms(10);
        
    }

    
    
    init_keypad();
    int COLS[] = {6, 7, 8, 9};  // COL4=GP6, COL3=GP7, COL2=GP8, COL1=GP9
    int ROWS[] = {2, 3, 4, 5};  // ROW4=GP2, ROW3=GP3, ROW2=GP4, ROW1=GP5

    
    while (true)
    {
        for (int i = 0; i < 4; i++)
        {
            sio_hw->gpio_set = (1 << COLS[i]);
            sleep_ms(10);
            
            u_int32_t input2 = sio_hw->gpio_in;

            bool is_high = input2 & (1 << ROWS[i]);

            if (is_high)
            {
                sio_hw->gpio_set = (1 << (25 - i));
            }
            else
            {
                sio_hw->gpio_clr = (1 << (25 - i));
            }
            
            sio_hw->gpio_clr = (1 << COLS[i]);
        } 
    }
    gpio_init
    

    // An infinite loop is necessary to 
    // ensure control flow remains with user.
    for(;;) {
       
    }

    // Never reached.
    return 0;
}