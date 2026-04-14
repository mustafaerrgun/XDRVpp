// tests/test_gpio.hpp
#pragma once
#include "../Inc/gpio.hpp"

using led = gpio<gpio_port::port_b, gpio_pin::pin_7>;

inline void run_test() {
    led::init();
    while (1) {
        led::set_pin_high();
        delay(3000);
        led::set_pin_low();
        delay(3000);
    }
}
