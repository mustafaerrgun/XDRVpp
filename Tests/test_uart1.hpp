// tests/test_uart1.hpp
#pragma once
#include "../Inc/uart.hpp"

using uart_test = uart<uart_inst::uart_3>;

inline void run_test()
{
    uart_test::init();

    char str[20]={};

    while(1)
    {
        uart_test::receive(str);

        uart_test::send(str);

    }
 } 