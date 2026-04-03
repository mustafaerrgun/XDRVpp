/* GPIO Driver in C++ */

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

template<gpio_port  Port,
         gpio_pin   Pin,
         gpio_mode  Mode  = gpio_mode::output,
         gpio_pupd  PuPd  = gpio_pupd::none,
         gpio_otype OType = gpio_otype::push_pull>
class gpio
{
    private:
        using reg_t  = uint32_t;
        using addr_t = uint32_t;

        static constexpr addr_t base  = static_cast<addr_t>(Port);
        static constexpr reg_t  pin   = static_cast<reg_t>(Pin);

        // Register offsets
        static constexpr addr_t moder_addr  = base + UINT32_C(0x00);
        static constexpr addr_t otyper_addr = base + UINT32_C(0x04);
        static constexpr addr_t pupdr_addr  = base + UINT32_C(0x0C);
        static constexpr addr_t bsrr_addr   = base + UINT32_C(0x18);

        // AHB1ENR bit position: derived from port base address offset
        static constexpr reg_t  clk_bit = (base - UINT32_C(0x40020000)) / UINT32_C(0x400);

    public:
        static void init()
        {
            // Enable GPIO port clock in RCC AHB1ENR
            reg_access<addr_t, reg_t, mcal::rcc::ahb1enr_addr, clk_bit>::bit_set();

            // MODER: 2 bits per pin
            reg_access<addr_t, reg_t, moder_addr, ~(reg_t(0x3U) << (pin * 2U))>::reg_and();
            reg_access<addr_t, reg_t, moder_addr, reg_t(static_cast<reg_t>(Mode) << (pin * 2U))>::reg_or();

            // OTYPER: 1 bit per pin
            reg_access<addr_t, reg_t, otyper_addr, ~(reg_t(0x1U) << pin)>::reg_and();
            reg_access<addr_t, reg_t, otyper_addr, reg_t(static_cast<reg_t>(OType) << pin)>::reg_or();

            // PUPDR: 2 bits per pin
            reg_access<addr_t, reg_t, pupdr_addr, ~(reg_t(0x3U) << (pin * 2U))>::reg_and();
            reg_access<addr_t, reg_t, pupdr_addr, reg_t(static_cast<reg_t>(PuPd) << (pin * 2U))>::reg_or();
        }

        static void set_pin_high()
        {
            // BSRR lower 16 bits set the pin
            reg_access<addr_t, reg_t, bsrr_addr, pin>::bit_set();
        }

        static void set_pin_low()
        {
            // BSRR upper 16 bits reset the pin
            reg_access<addr_t, reg_t, bsrr_addr, static_cast<reg_t>(pin + 16U)>::bit_set();
        }

        static void toggle()
        {
            reg_access<addr_t, reg_t, bsrr_addr, pin>::bit_not();
        }
};
