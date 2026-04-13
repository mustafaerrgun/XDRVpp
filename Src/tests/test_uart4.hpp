// tests/test_uart4.hpp
#pragma once
#include "../uart.hpp"

using uart_test = uart<uart_inst::uart_3, uart_baud::baud_96k, true>;

uint32_t rx_int = 0U;

inline void run_test()
{
    uart_test::init();

    uart_test::send("Interrupt Test");

    while(1)
    {

    }
 } 

extern "C" void USART3_IRQHandler()
{
    uart_test::irq_handler(rx_int);

    if(rx_int == 10)
    {
        uart_test::send("Received 10");
    }
    else
    {
        uart_test::send("Not Received 10");
    }
}
