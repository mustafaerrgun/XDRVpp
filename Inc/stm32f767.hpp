/* Required peripherals and template functions for register manipulations*/

#pragma once

#include <cstdint>

namespace mcal
{
    constexpr uint32_t hsi_clk_value = 16000000U;

    namespace rcc
    {
        constexpr uint32_t ahb1enr_addr  = UINT32_C(0x40023800 + 0x30);
        constexpr uint32_t apb1enr_addr  = UINT32_C(0x40023800 + 0x40);
        constexpr uint32_t apb2enr_addr  = UINT32_C(0x40023800 + 0x44);
        constexpr uint32_t dckcfgr2_addr = UINT32_C(0x40023800 + 0x90);
    }

    namespace nvic
    {
        constexpr uint32_t iser_base = UINT32_C(0xE000E100);

        inline void enable_irq(uint32_t irq_num)
        {
            const uint32_t reg_addr = iser_base + ((irq_num / 32U) * 4U);
            const uint32_t bit_mask = UINT32_C(1) << (irq_num % 32U);
            *reinterpret_cast<volatile uint32_t*>(reg_addr) = bit_mask;
        }
    }

    namespace systick
    {
        constexpr uint32_t ctrl_addr = UINT32_C(0xE000E010);
        constexpr uint32_t load_addr = UINT32_C(0xE000E014);
        constexpr uint32_t val_addr  = UINT32_C(0xE000E018);

        constexpr uint32_t ctrl_enable    = UINT32_C(0);
        constexpr uint32_t ctrl_clksrc    = UINT32_C(2);
        constexpr uint32_t ctrl_countflag = UINT32_C(16);

        constexpr uint32_t cycles_per_ms  = hsi_clk_value / 1000U;
    }
};

template<const uint32_t addr,
         const uint32_t val = UINT32_C(0xFFFFFFFF)>
struct reg_access
{
    static void reg_set(){ *reinterpret_cast<volatile uint32_t*>(addr)  =  val;  }
    static void reg_or() { *reinterpret_cast<volatile uint32_t*>(addr) |=  val;  }
    static void reg_and(){ *reinterpret_cast<volatile uint32_t*>(addr) &=  val;  }
    static void reg_msk(){ *reinterpret_cast<volatile uint32_t*>(addr) &= ~val;  }
    
    static void reg_write(uint32_t runtime_val){ *reinterpret_cast<volatile uint32_t*>(addr) = runtime_val; }
    static uint32_t reg_read()  { return *reinterpret_cast<volatile uint32_t*>(addr);        }


    static void bit_set(){ *reinterpret_cast<volatile uint32_t*>(addr) |=  uint32_t(1U << val); }
    static void bit_clr(){ *reinterpret_cast<volatile uint32_t*>(addr) &= ~uint32_t(1U << val); }
    static void bit_not(){ *reinterpret_cast<volatile uint32_t*>(addr) ^=  uint32_t(1U << val); }
};

inline void delay(uint32_t ms)
{
    // Load reload value: counts from (cycles_per_ms - 1) down to 0
    reg_access<mcal::systick::load_addr>::reg_write(mcal::systick::cycles_per_ms - 1U);

    // Clear current value register
    reg_access<mcal::systick::val_addr>::reg_write(0U);

    // Select processor clock (HCLK) and enable SysTick
    reg_access<mcal::systick::ctrl_addr, mcal::systick::ctrl_clksrc>::bit_set();
    reg_access<mcal::systick::ctrl_addr, mcal::systick::ctrl_enable>::bit_set();

    for (uint32_t i = 0U; i < ms; i++)
    {
        // Wait for COUNTFLAG (bit 16) — set when counter wraps to 0
        while ((reg_access<mcal::systick::ctrl_addr>::reg_read() & (1U << mcal::systick::ctrl_countflag)) == 0U) {}
    }

    // Disable SysTick
    reg_access<mcal::systick::ctrl_addr>::reg_write(0U);
}

