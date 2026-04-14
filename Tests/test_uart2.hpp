// tests/test_uart2.hpp
#pragma once
#include "../Inc/uart.hpp"

using uart_test = uart<uart_inst::uart_3>;

inline void run_test()
{
    uart_test::init();

    uart_test::send("Test to ASCii to uint32_t");

    uint32_t data = 0U;

    uart_test::receive(data);
 } 