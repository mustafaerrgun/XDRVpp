// tests/test_uart3.hpp
#pragma once
#include "../Inc/uart.hpp"

using uart_test = uart<uart_inst::uart_3>;

inline void run_test()
{
    uart_test::init();

    uart_test::send("Test to receive uint32_t array");

    uint32_t data[10] = {};

    for(uint32_t i=0U; i < 10U; i++)
    {
        uart_test::receive(data[i]);
    }
 } 