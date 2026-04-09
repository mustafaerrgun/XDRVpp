/* General template functions for register manipulation*/

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
};

template<const uint32_t addr,
         const uint32_t val = UINT32_C(0xFFFFFFFF)>
struct reg_access
{
    static void reg_set(){ *reinterpret_cast<volatile uint32_t*>(addr)  =  val;             }
    static void reg_or() { *reinterpret_cast<volatile uint32_t*>(addr) |=  val;             }
    static void reg_and(){ *reinterpret_cast<volatile uint32_t*>(addr) &=  val;             }
    static void reg_msk(){ *reinterpret_cast<volatile uint32_t*>(addr) &= ~val;             }
    
    static void reg_write(uint32_t runtime_val){ *reinterpret_cast<volatile uint32_t*>(addr) = runtime_val; }
    static uint32_t reg_read()  { return *reinterpret_cast<volatile uint32_t*>(addr);        }


    static void bit_set(){ *reinterpret_cast<volatile uint32_t*>(addr) |=  uint32_t(1U << val); }
    static void bit_clr(){ *reinterpret_cast<volatile uint32_t*>(addr) &= ~uint32_t(1U << val); }
    static void bit_not(){ *reinterpret_cast<volatile uint32_t*>(addr) ^=  uint32_t(1U << val); }
};

