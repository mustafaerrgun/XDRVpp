
#include "gpio.hpp"
#include "uart.hpp"

using led = gpio<gpio_port::port_b, gpio_pin::pin_7>;
using uart_test = uart<uart_inst::uart_3>;

//void test_gpio();
//void test1_uart();
//void test2_uart();
//void test3_uart();

int main(void)
{

    //test_gpio();

    //test1_uart();

    //test2_uart();

    //test3_uart();

    while (1)
    {
    }
}


/* void test_gpio()
{
    led::init();

    while(1)
    {
        led::set_pin_high();

        for(int i = 0; i < 2000000; i++) {}

        led::set_pin_low();

        for(int i = 0; i < 2000000; i++) {}

    }
} */

/* void test1_uart()
{
    uart_test::init();

    char str[20]={};

    while(1)
    {
        uart_test::receive(str,20);

        uart_test::send(str);

    }
 } */

/*  void test2_uart()
 {
    uart_test::init();

    uart_test::send("Test to ASCii to uint32_t");

    uint32_t data = 0U;

    uart_test::receive(data);

 } */

/*  void test3_uart()
 {
    uart_test::init();

    uart_test::send("Test to receive uint32_t array");

    uint32_t data[10] = {};

    for(uint32_t i=0U; i < 10U; i++)
    {
        uart_test::receive(data[i]);
    }
 } */
