/* UART Driver in C++ */

#pragma once

#include "stm32f767.hpp"
#include "gpio.hpp"

enum class uart_inst : uint32_t
{
    uart_1 = UINT32_C(0x40011000),
    uart_2 = UINT32_C(0x40004400),
    uart_3 = UINT32_C(0x40004800),
    uart_4 = UINT32_C(0x40004C00),
    uart_5 = UINT32_C(0x40005000),
    uart_6 = UINT32_C(0x40011400),
    uart_7 = UINT32_C(0x40007800),
};

enum class uart_baud : uint32_t
{
    baud_48k   = 4800U,
    baud_96k   = 9600U,
    baud_384k  = 38400U,
    baud_576k  = 57600U,
    baud_1152k = 115200U,
};

struct uart_pins
{
    gpio_port tx_port;
    gpio_pin  tx_pin;
    gpio_port rx_port;
    gpio_pin  rx_pin;
    gpio_af   af;
};

struct uart_clk
{
    uint32_t clk_src_bit;
    uint32_t clk_en_addr;
    uint32_t clk_en_bit;
};

constexpr uart_clk uart_clk_config(uart_inst inst)
{
    switch (inst)
    {
        case uart_inst::uart_1: return { 0U,  mcal::rcc::apb2enr_addr,  4U};
        case uart_inst::uart_2: return { 2U,  mcal::rcc::apb1enr_addr, 17U};
        case uart_inst::uart_3: return { 4U,  mcal::rcc::apb1enr_addr, 18U};
        case uart_inst::uart_4: return { 6U,  mcal::rcc::apb1enr_addr, 19U};
        case uart_inst::uart_5: return { 8U,  mcal::rcc::apb1enr_addr, 20U};
        case uart_inst::uart_6: return { 10U, mcal::rcc::apb2enr_addr,  5U};
        case uart_inst::uart_7: return { 12U, mcal::rcc::apb1enr_addr, 30U};
        default:                return { 0U,  mcal::rcc::apb2enr_addr,  4U};
    }
}


constexpr uart_pins uart_gpio_config(uart_inst inst)
{
    switch (inst)
    {
        case uart_inst::uart_1: return { gpio_port::port_a, gpio_pin::pin_9,  gpio_port::port_a, gpio_pin::pin_10, gpio_af::af7 };
        case uart_inst::uart_2: return { gpio_port::port_d, gpio_pin::pin_5,  gpio_port::port_d, gpio_pin::pin_6,  gpio_af::af7 };
        case uart_inst::uart_3: return { gpio_port::port_d, gpio_pin::pin_8,  gpio_port::port_d, gpio_pin::pin_9,  gpio_af::af7 };
        case uart_inst::uart_4: return { gpio_port::port_d, gpio_pin::pin_0,  gpio_port::port_d, gpio_pin::pin_1,  gpio_af::af8 };
        case uart_inst::uart_5: return { gpio_port::port_b, gpio_pin::pin_8,  gpio_port::port_b, gpio_pin::pin_9,  gpio_af::af7 };
        case uart_inst::uart_6: return { gpio_port::port_c, gpio_pin::pin_6,  gpio_port::port_c, gpio_pin::pin_7,  gpio_af::af8 };
        case uart_inst::uart_7: return { gpio_port::port_e, gpio_pin::pin_7,  gpio_port::port_e, gpio_pin::pin_8,  gpio_af::af8 };
        default:                return { gpio_port::port_a, gpio_pin::pin_9,  gpio_port::port_a, gpio_pin::pin_10, gpio_af::af7 };
    }
}

template<uart_inst  Instance      = uart_inst::uart_3,
         uart_baud  BaudRate      = uart_baud::baud_96k,
         bool       UartInterrupt = false,
         bool       UartDMA       = false>
class uart
{
    private:
        static constexpr uint32_t base    = static_cast<uint32_t>(Instance);

        // Register offsets
        static constexpr uint32_t cr1_addr = base + UINT32_C(0x00);
        static constexpr uint32_t brr_addr = base + UINT32_C(0x0C);
        static constexpr uint32_t isr_addr = base + UINT32_C(0x1C);
        static constexpr uint32_t rdr_addr = base + UINT32_C(0x24);
        static constexpr uint32_t tdr_addr = base + UINT32_C(0x28);

        // HSI clock source value in RCC->DCKCFGR2
        static constexpr uint32_t hsi_val = UINT32_C(0x2);

        static void clk_init()
        {
            constexpr uart_clk clk_conf = uart_clk_config(Instance);
            // Clear and set HSI (0x2) as Clock Source
            reg_access< mcal::rcc::dckcfgr2_addr, ~(uint32_t(0x3U) << clk_conf.clk_src_bit)>::reg_and();
            reg_access< mcal::rcc::dckcfgr2_addr,  uint32_t(hsi_val << clk_conf.clk_src_bit)>::reg_or();
            // Enable Clock
            reg_access< clk_conf.clk_en_addr, clk_conf.clk_en_bit>::bit_set();
        }

        static void gpio_init()
        {
            constexpr uart_pins pins = uart_gpio_config(Instance);

            // Configure GPIO Pins as hardcoded
            gpio<pins.tx_port, pins.tx_pin, gpio_mode::altfn, gpio_pupd::none, gpio_otype::push_pull, pins.af>::init();
            gpio<pins.rx_port, pins.rx_pin, gpio_mode::altfn, gpio_pupd::none, gpio_otype::push_pull, pins.af>::init();
        }

        static void brr_calc()
        {
            constexpr uint32_t uart_div = (mcal::hsi_clk_value + 
                                          (static_cast<uint32_t>(BaudRate)/2U)) 
                                          / static_cast<uint32_t>(BaudRate);
            
            constexpr uint32_t mantissa = uart_div / 16U;
            constexpr uint32_t fraction = uart_div % 16U;

            reg_access<brr_addr, uint32_t((mantissa << 4U) 
                        | (fraction & 0x0FU))>::reg_set();

        }

        // uint32_t to ASCII
        static void conversion(uint32_t data)
        {
            char buf[11] = {};
            uint32_t i = 10U;

            if (data == 0U)
            {
                buf[--i] = '0';
            }
            else
            {
                while (data > 0U)
                {
                    buf[--i] = static_cast<char>('0' + (data % 10U));
                    data /= 10U;
                }
            }

            send(&buf[i]);
        }

        // ASCII to uint32_t
        static uint32_t conversion(const uint8_t* data, uint32_t len)
        {
            uint32_t rx_data = 0U;

            for(uint32_t i=0U; i < len; i++)
            {
                if(data[i] == '\0') break;
                if(data[i] < '0' || data[i] > '9') break;
                rx_data = rx_data * 10U + static_cast<uint32_t>(data[i] - '0');
            }
            return rx_data;
        }

        static void send_byte(uint8_t data)
        {
            // Wait for TXE (bit 7)
            while(!(reg_access<isr_addr>::reg_read() & (1U << 7U))) {}

            // Write byte to TDR
            reg_access<tdr_addr>::reg_write(static_cast<uint32_t>(data));
        }

        static uint8_t receive_byte()
        {
            // Wait for RXE (bit 5)
            while(!(reg_access<isr_addr>::reg_read() & (1U << 5U))) {}

            return static_cast<uint8_t>(reg_access<rdr_addr>::reg_read());
        }


    public:
        static void init()
        {
            clk_init();

            gpio_init();

            // Clear CR1 register
            reg_access<cr1_addr>::reg_msk();

            // Use TX-RX Mode
            reg_access<cr1_addr, 2U>::bit_set();
            reg_access<cr1_addr, 3U>::bit_set();

            // Calculate BRR register value for specified baudrate
            brr_calc();

            // Enable UART Module
            reg_access<cr1_addr, 0U>::bit_set();
        }

        // Send char array
        static void send(const char* data)
        {
            for(uint32_t i=0U; data[i] != '\0'; i++)
                send_byte(static_cast<uint8_t>(data[i]));

            send_byte('\r');
            send_byte('\n');
        }

        // Send single char value
        static void send(const char data)
        {
            send_byte(static_cast<uint8_t>(data));
            send_byte('\r');
            send_byte('\n');
        }

        // Send uint32_t value
        static void send(const uint32_t data)
        {
            conversion(data);
        }

        // Receive uint32_t value
        static void receive(uint32_t& data)
        {
            uint8_t byte_buf[11] = {};

            for (uint32_t i = 0U; i < 10U; i++)
            {
                byte_buf[i] = receive_byte();

                if(byte_buf[i] == '\n' || byte_buf[i] == '\r')
                {   
                    byte_buf[i] = '\0'; 
                    break;
                }
            }
            data = conversion(byte_buf, 11U);
        }

        // Receive char array
        template<uint32_t N>
        static void receive(char (&data)[N])
        {
            for (uint32_t i = 0U; i < N - 1U; i++)
            {
                data[i] = static_cast<char>(receive_byte());

                if(data[i] == '\n' || data[i] == '\r')
                {
                    data[i] = '\0';
                    return;
                }
            }
            data[N - 1U] = '\0';
        }

};
