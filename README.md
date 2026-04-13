# XDRVpp — eXperimental Drivers in Modern C++

Bare-metal firmware drivers (GPIO, UART) for the **STM32F767ZI** written in modern C++.

## 1. Overview

This project implements two low-level peripheral drivers from scratch:

| Driver | File | Description |
|--------|------|-------------|
| GPIO | [Src/gpio.hpp](Src/gpio.hpp) | Template-based GPIO configuration and control |
| UART | [Src/uart.hpp](Src/uart.hpp) | Template-based UART TX/RX with polling and interrupt modes |

Key design goals:
- **Zero runtime overhead** — all configuration resolved at compile time via templates
- **Type safety** — strongly-typed enums for ports, pins, modes, baud rates, and UART instances
- **No external dependencies** — direct register access, no CMSIS or STM32 HAL
- **Header-only** — easy to integrate, no separate compilation units needed

## 2. Project Structure

```
XDRpp/
├── Src/
│   ├── main.cpp                  # Entry point
│   ├── gpio.hpp                  # GPIO driver
│   ├── uart.hpp                  # UART driver
│   ├── stm32f767.hpp             # MCU peripheral definitions & register abstraction
│   ├── startup_stm32f767.cpp     # Startup code & interrupt vector table (149 vectors)
│   ├── syscalls.c                # Newlib system call stubs
│   ├── sysmem.c                  # Heap management (_sbrk)
│   └── tests/
│       ├── test_gpio.hpp         # GPIO LED blink test
│       ├── test_uart1.hpp        # UART echo test (char array)
│       ├── test_uart2.hpp        # UART integer reception test
│       ├── test_uart3.hpp        # UART array of uint32_t reception
│       └── test_uart4.hpp        # UART interrupt-driven reception
├── Docs/
│   └── Cpp_Concepts_Guide.md     # Guide: linker scripts, startup, IRQ handlers
└── STM32F767ZITX_CUSTOM.ld       # Custom linker script
```


## 3. Hardware Target

| Parameter | Value |
|-----------|-------|
| MCU | STM32F767ZI |
| Core | ARM Cortex-M7 |
| Flash | 2 MB @ `0x08000000` |
| RAM | 512 KB @ `0x20000000` |
| Clock | HSI 16 MHz    |
| Board | NUCLEO-F767ZI |


## 4. Architecture

There is no dynamic dispatch, no virtual functions, and no heap allocation in driver code. Register addresses and bit masks are computed by the compiler as `constexpr` values.

The register abstraction template ([Src/stm32f767.hpp](Src/stm32f767.hpp)) provides atomic read/write/OR/AND/toggle operations:

```cpp
template<const uint32_t addr, const uint32_t val = 0xFFFFFFFF>
struct reg_access {
    static void reg_or();        // Bitwise OR (set bits)
    static void reg_and();       // Bitwise AND (clear bits)
    static void reg_set();       // Write value
    static uint32_t reg_read();  // Read value
    static void bit_set();       // Set single bit (val = bit position)
    static void bit_clr();       // Clear single bit
    static void bit_not();       // Toggle single bit
};
```

## 5. GPIO Driver

**File:** [Src/gpio.hpp](Src/gpio.hpp)

### Template Parameters

```cpp
template<
    const gpio_port  Port,   // GPIO port (port_a ... port_i)
    const uint8_t    Pin,    // Pin number (0–15)
    const gpio_mode  Mode,   // input | output | altfn | analog
    const gpio_pupd  PuPd,   // no_pull | pull_up | pull_down
    const gpio_otype OType,  // push_pull | open_drain
    const uint8_t    Af = 0  // Alternate function (AF0–AF15)
>
struct gpio { ... };
```

### API

| Method | Description |
|--------|-------------|
| `init()` | Configure MODER, OTYPER, PUPDR, AFR, and enable port clock |
| `set_pin_high()` | Set pin HIGH via BSRR (atomic) |
| `set_pin_low()` | Set pin LOW via BSRR (atomic) |
| `toggle()` | Toggle pin state via BSRR |

### Example

```cpp
// Configure PB7 as push-pull output, no pull
using led = gpio<port_b, 7, output, no_pull, push_pull>;
led::init();
led::set_pin_high();
delay(1000);
led::set_pin_low();
```

## 6. UART Driver

**File:** [Src/uart.hpp](Src/uart.hpp)

### Template Parameters

```cpp
template<
    const uart_instance Instance,     // uart_1 ... uart_7
    const uart_baud_rate BaudRate,    // baud_4800 ... baud_115200
    const bool UartInterrupt = false  // Enable RXNE interrupt
>
struct uart { ... };
```

### UART Instances

| Instance | Base Address | IRQ | TX Pin | RX Pin | AF |
|----------|-------------|-----|--------|--------|----|
| `uart_1` | `0x40011000` | 37 | PA9 | PA10 | AF7 |
| `uart_2` | `0x40004400` | 38 | PD5 | PD6 | AF7 |
| `uart_3` | `0x40004800` | 39 | PD8 | PD9 | AF7 |
| `uart_4` | `0x40004C00` | 52 | PD0 | PD1 | AF8 |
| `uart_5` | `0x40005000` | 53 | PB8 | PB9 | AF7 |
| `uart_6` | `0x40011400` | 71 | PC6 | PC7 | AF8 |
| `uart_7` | `0x40007800` | 82 | PE7 | PE8 | AF8 |

### API

#### Initialization

```cpp
uart<uart_3, baud_115200>::init();         // Polling mode
uart<uart_3, baud_115200, true>::init();   // Interrupt-driven RX
```

Initialization sequence:
1. Set HSI as UART clock source in `DCKCFGR2`
2. Enable peripheral clock (`APB1ENR` or `APB2ENR`)
3. Configure TX/RX GPIO pins with the correct alternate function
4. Calculate and set BRR (`HSI_CLK / BaudRate`)
5. Enable TX and RX bits in `CR1`
6. If interrupt mode: enable `RXNEIE` in `CR1` and configure NVIC

#### Transmit Overloads

| Method | Description |
|--------|-------------|
| `send(const char* str)` | Send null-terminated string + CRLF |
| `send(char c)` | Send single character + CRLF |
| `send(uint32_t val)` | Convert integer to ASCII and send |

#### Receive Overloads

| Method | Description |
|--------|-------------|
| `receive(char (&buf)[N])` | Receive into a char array (up to N−1 chars) |
| `receive(uint32_t& val)` | Receive ASCII digits and convert to `uint32_t` |

#### Interrupt-Driven Reception

```cpp
// Define handler with extern "C" to override the weak alias in startup
extern "C" void USART3_IRQHandler() {
    my_uart::irq_handler(received_value);
}
```

### Example — Polling Echo

```cpp
using serial = uart<uart_3, baud_115200>;
serial::init();

char buf[32];
while (true) {
    serial::receive(buf);
    serial::send(buf);
}
```

### Example — Interrupt-Driven Reception

```cpp
using serial = uart<uart_3, baud_115200, true>;
uint32_t value = 0;
serial::init();

extern "C" void USART3_IRQHandler() {
    serial::irq_handler(value);
}
```

## 7. Startup & Linker Script

### Startup ([Src/startup_stm32f767.cpp](Src/startup_stm32f767.cpp))

- Defines the **interrupt vector table** with 149 entries (Cortex-M7 exceptions + STM32F767 peripherals)
- All unused handlers are **weak aliases** to `Default_Handler` — override by defining the same name with `extern "C"`
- `Reset_Handler` copies `.data` from Flash to RAM and zeroes `.bss` before calling `main()`

### Linker Script ([STM32F767ZITX_CUSTOM.ld](STM32F767ZITX_CUSTOM.ld))

```
FLASH: 0x08000000  2 MB  (rx)
RAM:   0x20000000  512 KB (rwx)
```

Sections: `.isr_vector` → `.text` → `.data` (loaded from Flash, copied to RAM) → `.bss` (zeroed at startup)

## 8. Test Files

Located in [Src/tests/](Src/tests/), each file defines a `run_test()` function included by [Src/main.cpp](Src/main.cpp):

| File | What it tests |
|------|--------------|
| [test_gpio.hpp](Src/tests/test_gpio.hpp) | PB7 LED blink with 3-second on/off cycle |
| [test_uart1.hpp](Src/tests/test_uart1.hpp) | Receive a char array and echo it back |
| [test_uart2.hpp](Src/tests/test_uart2.hpp) | Receive an ASCII integer, convert to `uint32_t`, send back |
| [test_uart3.hpp](Src/tests/test_uart3.hpp) | Receive 10 `uint32_t` values sequentially |
| [test_uart4.hpp](Src/tests/test_uart4.hpp) | Interrupt-driven reception, verify received value equals 10 |

## 9. Building

The project targets **STM32CubeIDE** (Eclipse-based). Open the project folder in STM32CubeIDE, select the build configuration, and compile. The custom linker script `STM32F767ZITX_CUSTOM.ld` and startup file `startup_stm32f767.cpp` replace the auto-generated ones.

**Compiler flags required:**
- `-std=c++20` 
- `-mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard`
- `-ffreestanding -fno-exceptions -fno-rtti`
