
#include "stm32f767.h"

int main(void)
{
    // Enable GPIOB Clock
    reg_access<uint32_t, uint32_t, mcal::rcc::rcc_ahb1enr, UINT32_C(1)>::bit_set();

    // Enable GPIOB.P0 to output
    reg_access<uint32_t, uint32_t, mcal::gpio_b::gpio_b_moder, UINT32_C(0)>::bit_set();

    // Push-Pull
    reg_access<uint32_t, uint32_t, mcal::gpio_b::gpio_b_otyper, UINT32_C(0)>::bit_clr();

    // No Pull (bits [1:0] = 00 for PB0)
    reg_access<uint32_t, uint32_t, mcal::gpio_b::gpio_b_pupdr, UINT32_C(0)>::bit_clr();
    reg_access<uint32_t, uint32_t, mcal::gpio_b::gpio_b_pupdr, UINT32_C(1)>::bit_clr();


    while (1)
    {
        // LED ON
        reg_access<uint32_t, uint32_t, mcal::gpio_b::gpio_b_bsrr, UINT32_C(0)>::bit_set();

        for(int i=0; i<200000;i++){}

        // LED OFF
        reg_access<uint32_t, uint32_t, mcal::gpio_b::gpio_b_bsrr, UINT32_C(16)>::bit_set();

        for(int i=0; i<200000;i++){}
    }
}
