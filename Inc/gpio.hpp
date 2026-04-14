/* GPIO Driver in C++ */

#pragma once

#include "stm32f767.hpp"

enum class gpio_port : uint32_t
{
    port_a = UINT32_C(0x40020000),
    port_b = UINT32_C(0x40020400),
    port_c = UINT32_C(0x40020800),
    port_d = UINT32_C(0x40020C00),
    port_e = UINT32_C(0x40021000),
    port_f = UINT32_C(0x40021400),
    port_g = UINT32_C(0x40021800),
    port_h = UINT32_C(0x40021C00),
    port_i = UINT32_C(0x40022000),
};

enum class gpio_pin : uint32_t
{
    pin_0  =  0U,
    pin_1  =  1U,
    pin_2  =  2U,
    pin_3  =  3U,
    pin_4  =  4U,
    pin_5  =  5U,
    pin_6  =  6U,
    pin_7  =  7U,
    pin_8  =  8U,
    pin_9  =  9U,
    pin_10 = 10U,
    pin_11 = 11U,
    pin_12 = 12U,
    pin_13 = 13U,
    pin_14 = 14U,
    pin_15 = 15U,
};

enum class gpio_mode : uint32_t
{
    input  = 0x00U,
    output = 0x01U,
    altfn  = 0x02U,
    analog = 0x03U,
};

enum class gpio_otype : uint32_t
{
    push_pull  = 0x00U,
    open_drain = 0x01U,
};

enum class gpio_pupd : uint32_t
{
    none      = 0x00U,
    pull_up   = 0x01U,
    pull_down = 0x02U,
};

enum class gpio_af : uint32_t
{
    af0  = 0x00U,  // System (SWD, MCO, etc.)
    af1  = 0x01U,  // TIM1, TIM2
    af2  = 0x02U,  // TIM3, TIM4, TIM5
    af3  = 0x03U,  // TIM8, TIM9, TIM10, TIM11
    af4  = 0x04U,  // I2C1, I2C2, I2C3, I2C4
    af5  = 0x05U,  // SPI1, SPI2, SPI3, SPI4, SPI5, SPI6
    af6  = 0x06U,  // SPI3, SAI1
    af7  = 0x07U,  // USART1, USART2, USART3, UART5, SPDIF-RX
    af8  = 0x08U,  // USART6, UART4, UART5, UART7, UART8, SPDIF-RX
    af9  = 0x09U,  // CAN1, CAN2, CAN3, TIM12, TIM13, TIM14, QUADSPI, LCD
    af10 = 0x0AU,  // OTG_FS, OTG_HS, QUADSPI, SAI2
    af11 = 0x0BU,  // ETH, SDMMC2
    af12 = 0x0CU,  // FMC, SDMMC1, OTG_FS
    af13 = 0x0DU,  // DCMI, DSI, LCD
    af14 = 0x0EU,  // LCD
    af15 = 0x0FU,  // EVENTOUT
};

template<gpio_port  Port,
         gpio_pin   Pin,
         gpio_mode  Mode  = gpio_mode::output,
         gpio_pupd  PuPd  = gpio_pupd::none,
         gpio_otype OType = gpio_otype::push_pull,
         gpio_af    Af    = gpio_af::af0>
class gpio
{
    private:
        static constexpr uint32_t base  = static_cast<uint32_t>(Port);
        static constexpr uint32_t pin   = static_cast<uint32_t>(Pin);

        // Register offsets
        static constexpr uint32_t moder_addr  = base + UINT32_C(0x00);
        static constexpr uint32_t otyper_addr = base + UINT32_C(0x04);
        static constexpr uint32_t pupdr_addr  = base + UINT32_C(0x0C);
        static constexpr uint32_t bsrr_addr   = base + UINT32_C(0x18);
        static constexpr uint32_t afrl_addr   = base + UINT32_C(0x20); // pins 0-7
        static constexpr uint32_t afrh_addr   = base + UINT32_C(0x24); // pins 8-15

        // AHB1ENR bit position: derived from port base address offset
        static constexpr uint32_t clk_bit = (base - UINT32_C(0x40020000)) / UINT32_C(0x400);

    public:
        static void init()
        {
            // Enable GPIO port clock in RCC AHB1ENR
            reg_access< mcal::rcc::ahb1enr_addr, clk_bit>::bit_set();

            // MODER: 2 bits per pin
            reg_access< moder_addr, ~(uint32_t(0x3U) << (pin * 2U))>::reg_and();
            reg_access< moder_addr, uint32_t(static_cast<uint32_t>(Mode) << (pin * 2U))>::reg_or();

            // OTYPER: 1 bit per pin
            reg_access< otyper_addr, ~(uint32_t(0x1U) << pin)>::reg_and();
            reg_access< otyper_addr, uint32_t(static_cast<uint32_t>(OType) << pin)>::reg_or();

            // PUPDR: 2 bits per pin
            reg_access< pupdr_addr, ~(uint32_t(0x3U) << (pin * 2U))>::reg_and();
            reg_access< pupdr_addr, uint32_t(static_cast<uint32_t>(PuPd) << (pin * 2U))>::reg_or();

            // AFRL (pins 0-7) and AFRH (pins 8-15)
            if constexpr (Mode == gpio_mode::altfn)
            {
                if constexpr (pin < 8U)
                {
                    reg_access< afrl_addr, ~(uint32_t(0xFU) << (pin * 4U))>::reg_and();
                    reg_access< afrl_addr, uint32_t(static_cast<uint32_t>(Af) << (pin * 4U))>::reg_or();
                }
                else
                {
                    constexpr uint32_t pin_h = pin - 8U;
                    reg_access< afrh_addr, ~(uint32_t(0xFU) << (pin_h * 4U))>::reg_and();
                    reg_access< afrh_addr, uint32_t(static_cast<uint32_t>(Af) << (pin_h * 4U))>::reg_or();
                }
            }
        }

        static void set_pin_high()
        {
            // BSRR lower 16 bits set the pin
            reg_access< bsrr_addr, pin>::bit_set();
        }

        static void set_pin_low()
        {
            // BSRR upper 16 bits reset the pin
            reg_access< bsrr_addr, uint32_t(pin + 16U)>::bit_set();
        }

        static void toggle()
        {
            reg_access< bsrr_addr, pin>::bit_not();
        }
};
