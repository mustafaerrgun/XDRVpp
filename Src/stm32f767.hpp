/* General template functions for register manipulation*/

#include <cstdint>

namespace mcal
{
    namespace rcc
    {
        constexpr uint32_t ahb1enr_addr = UINT32_C(0x40023800 + 0x30);
    }
};

template<typename addr_type,
         typename reg_type,
         const addr_type addr,
         const reg_type val>
struct reg_access
{
    static void reg_set(){ *reinterpret_cast<volatile reg_type*>(addr)   = val;}
    static void reg_or() { *reinterpret_cast<volatile reg_type*>(addr)  |= val;}
    static void reg_and(){ *reinterpret_cast<volatile reg_type*>(addr)  &= val; }
    static void reg_msk(){ *reinterpret_cast<volatile reg_type*>(addr)  &= ~val;}

    static void bit_set(){ *reinterpret_cast<volatile reg_type*>(addr) |=  reg_type(reg_type(1U) << val);}
    static void bit_clr(){ *reinterpret_cast<volatile reg_type*>(addr) &= ~reg_type(reg_type(1U) << val);}
    static void bit_not(){ *reinterpret_cast<volatile reg_type*>(addr) ^=  reg_type(reg_type(1U) << val);}
};

