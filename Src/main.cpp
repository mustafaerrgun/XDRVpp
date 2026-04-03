
#include "gpio.hpp"

using led = gpio<gpio_port::port_b, gpio_pin::pin_7>;

int main(void)
{
    led::init();

    while (1)
    {
        led::set_pin_high();

        for(int i = 0; i < 200000; i++) {}

        led::set_pin_low();

        for(int i = 0; i < 200000; i++) {}
    }
}
