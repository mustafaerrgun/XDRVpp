

#include <cstdint>

namespace mcal
{
    namespace rcc
    {
        constexpr uint32_t rcc_ahb1enr = UINT32_C(0x40023800 + 0x30);
    }

    namespace gpio_b
    {
        constexpr uint32_t gpio_b_moder  = UINT32_C(0x40020400);
        constexpr uint32_t gpio_b_otyper = UINT32_C(0x40020400 + 0x04);
        constexpr uint32_t gpio_b_pupdr  = UINT32_C(0x40020400 + 0x0C);
        constexpr uint32_t gpio_b_bsrr   = UINT32_C(0x40020400 + 0x18);
    }
};


template<typename addr_type,
         typename reg_type,
         const addr_type addr,
         const reg_type val>
struct reg_access
{
    static void reg_set(){ *reinterpret_cast<volatile reg_type*>(addr)  = val;}
    static void reg_or(){ *reinterpret_cast<volatile reg_type*>(addr)  |= val;}
    static void reg_and(){ *reinterpret_cast<volatile reg_type*>(addr) &= val; }
    static void reg_msk(){ *reinterpret_cast<volatile reg_type*>(addr) &= ~val;}

    static void bit_set(){ *reinterpret_cast<volatile reg_type*>(addr) |=  reg_type(reg_type(1U) << val);}
    static void bit_clr(){ *reinterpret_cast<volatile reg_type*>(addr) &= ~reg_type(reg_type(1U) << val);}
    static void bit_not(){ *reinterpret_cast<volatile reg_type*>(addr) ^=  reg_type(reg_type(1U) << val);}
};

