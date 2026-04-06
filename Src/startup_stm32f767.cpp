/* Startup declarations and function for the STM32F767*/

#include <stdint.h>
#include <algorithm>

#define RAM_START 0x20000000U
#define RAM_SIZE (512U * 1024U) // 512KB
#define RAM_END ((RAM_START) + (RAM_SIZE))
#define STACK_START RAM_END

extern uint32_t _etext;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _la_data;

extern uint32_t _sbss;
extern uint32_t _ebss;

int main(void);

extern "C"
{
  [[noreturn]] void Reset_Handler(void);
  void Default_Handler(void);
  void *memcpy(void *dest, const void *src, size_t n);
}

// Weak function prototypes for the vector table so that they can easily be 
extern "C"
{
    void NMI_Handler(void)               __attribute__((weak, alias("Default_Handler")));
    void HardFault_Handler(void)         __attribute__((weak, alias("Default_Handler")));
    void MemManage_Handler(void)         __attribute__((weak, alias("Default_Handler")));
    void BusFault_Handler(void)          __attribute__((weak, alias("Default_Handler")));
    void UsageFault_Handler(void)        __attribute__((weak, alias("Default_Handler")));
    void SVC_Handler(void)               __attribute__((weak, alias("Default_Handler")));
    void DebugMon_Handler(void)          __attribute__((weak, alias("Default_Handler")));
    void PendSV_Handler(void)            __attribute__((weak, alias("Default_Handler")));
    void SysTick_Handler(void)           __attribute__((weak, alias("Default_Handler")));
    void WWDG_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
    void PVD_IRQHandler(void)            __attribute__((weak, alias("Default_Handler")));
    void TAMP_STAMP_IRQHandler(void)     __attribute__((weak, alias("Default_Handler")));
    void RTC_WKUP_IRQHandler(void)       __attribute__((weak, alias("Default_Handler")));
    void RCC_IRQHandler(void)            __attribute__((weak, alias("Default_Handler")));
    void EXTI0_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
    void EXTI1_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
    void EXTI2_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
    void EXTI3_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
    void EXTI4_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
    void DMA1_Channel1_IRQHandler(void)  __attribute__((weak, alias("Default_Handler")));
    void DMA1_Channel2_IRQHandler(void)  __attribute__((weak, alias("Default_Handler")));
    void DMA1_Channel3_IRQHandler(void)  __attribute__((weak, alias("Default_Handler")));
    void DMA1_Channel4_IRQHandler(void)  __attribute__((weak, alias("Default_Handler")));
    void DMA1_Channel5_IRQHandler(void)  __attribute__((weak, alias("Default_Handler")));
    void DMA1_Channel6_IRQHandler(void)  __attribute__((weak, alias("Default_Handler")));
    void DMA1_Channel7_IRQHandler(void)  __attribute__((weak, alias("Default_Handler")));
    void ADC_IRQHandler(void)            __attribute__((weak, alias("Default_Handler")));
    void CAN1_TX_IRQHandler(void)        __attribute__((weak, alias("Default_Handler")));
    void CAN1_RX0_IRQHandler(void)       __attribute__((weak, alias("Default_Handler")));
    void CAN1_RX1_IRQHandler(void)       __attribute__((weak, alias("Default_Handler")));
    void CAN1_SCE_IRQHandler(void)       __attribute__((weak, alias("Default_Handler")));
    void EXTI9_5_IRQHandler(void)        __attribute__((weak, alias("Default_Handler")));
    void TIM1_BRK_IRQHandler(void)       __attribute__((weak, alias("Default_Handler")));
    void TIM1_UP_IRQHandler(void)        __attribute__((weak, alias("Default_Handler")));
    void TIM1_TRG_COM_IRQHandler(void)   __attribute__((weak, alias("Default_Handler")));
    void TIM1_CC_IRQHandler(void)        __attribute__((weak, alias("Default_Handler")));
    void TIM2_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
    void TIM3_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
    void TIM4_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
    void I2C1_EV_IRQHandler(void)        __attribute__((weak, alias("Default_Handler")));
    void I2C1_ER_IRQHandler(void)        __attribute__((weak, alias("Default_Handler")));
    void I2C2_EV_IRQHandler(void)        __attribute__((weak, alias("Default_Handler")));
    void I2C2_ER_IRQHandler(void)        __attribute__((weak, alias("Default_Handler")));
    void SPI1_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
    void SPI2_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
    void USART1_IRQHandler(void)         __attribute__((weak, alias("Default_Handler")));
    void USART2_IRQHandler(void)         __attribute__((weak, alias("Default_Handler")));
    void USART3_IRQHandler(void)         __attribute__((weak, alias("Default_Handler")));
    void EXTI15_10_IRQHandler(void)      __attribute__((weak, alias("Default_Handler")));
    void RTC_Alarm_IRQHandler(void)      __attribute__((weak, alias("Default_Handler")));
    void USB_FS_WKUP_IRQHandler(void)    __attribute__((weak, alias("Default_Handler")));
    void TIM8_BRK_IRQHandler(void)       __attribute__((weak, alias("Default_Handler")));
    void TIM8_UP_IRQHandler(void)        __attribute__((weak, alias("Default_Handler")));
    void TIM8_TRG_COM_IRQHandler(void)   __attribute__((weak, alias("Default_Handler")));
    void TIM8_CC_IRQHandler(void)        __attribute__((weak, alias("Default_Handler")));
    void DMA1_Channel8_IRQHandler(void)  __attribute__((weak, alias("Default_Handler")));
    void FMC_IRQHandler(void)            __attribute__((weak, alias("Default_Handler")));
    void SDMMC1_IRQHandler(void)         __attribute__((weak, alias("Default_Handler")));
    void TIM5_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
    void SPI3_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
    void UART4_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
    void UART5_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
    void TIM6_DAC_IRQHandler(void)       __attribute__((weak, alias("Default_Handler")));
    void TIM7_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
    void DMA2_Channel1_IRQHandler(void)  __attribute__((weak, alias("Default_Handler")));
    void DMA2_Channel2_IRQHandler(void)  __attribute__((weak, alias("Default_Handler")));
    void DMA2_Channel3_IRQHandler(void)  __attribute__((weak, alias("Default_Handler")));
    void DMA2_Channel4_IRQHandler(void)  __attribute__((weak, alias("Default_Handler")));
    void DMA2_Channel5_IRQHandler(void)  __attribute__((weak, alias("Default_Handler")));
    void ETH_IRQHandler(void)            __attribute__((weak, alias("Default_Handler")));
    void ETH_WKUP_IRQHandler(void)       __attribute__((weak, alias("Default_Handler")));
    void CAN2_TX_IRQHandler(void)        __attribute__((weak, alias("Default_Handler")));
    void CAN2_RX0_IRQHandler(void)       __attribute__((weak, alias("Default_Handler")));
    void CAN2_RX1_IRQHandler(void)       __attribute__((weak, alias("Default_Handler")));
    void CAN2_SCE_IRQHandler(void)       __attribute__((weak, alias("Default_Handler")));
    void USB_FS_IRQHandler(void)         __attribute__((weak, alias("Default_Handler")));
    void DMA2_Channel6_IRQHandler(void)  __attribute__((weak, alias("Default_Handler")));
    void DMA2_Channel7_IRQHandler(void)  __attribute__((weak, alias("Default_Handler")));
    void DMA2_Channel8_IRQHandler(void)  __attribute__((weak, alias("Default_Handler")));
    void USART6_IRQHandler(void)         __attribute__((weak, alias("Default_Handler")));
    void I2C3_EV_IRQHandler(void)        __attribute__((weak, alias("Default_Handler")));
    void I2C3_ER_IRQHandler(void)        __attribute__((weak, alias("Default_Handler")));
    void USB_HS_EP1_OUT_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
    void USB_HS_EP1_IN_IRQHandler(void)  __attribute__((weak, alias("Default_Handler")));
    void USB_HS_WKUP_IRQHandler(void)    __attribute__((weak, alias("Default_Handler")));
    void USB_HS_IRQHandler(void)         __attribute__((weak, alias("Default_Handler")));
    void DCMI_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
    void CRYP_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
    void HASH_RNG_IRQHandler(void)       __attribute__((weak, alias("Default_Handler")));
    void FPU_IRQHandler(void)            __attribute__((weak, alias("Default_Handler")));
    void UART7_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
    void UART8_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
    void SPI4_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
    void SPI5_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
    void SPI6_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
    void SAI1_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
    void LCD_TFT_IRQHandler(void)        __attribute__((weak, alias("Default_Handler")));
    void LCD_TFT_ERR_IRQHandler(void)    __attribute__((weak, alias("Default_Handler")));
    void DMA2D_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
    void SAI2_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
    void QUADSPI_IRQHandler(void)        __attribute__((weak, alias("Default_Handler")));
    void LPI_TIM1_IRQHandler(void)       __attribute__((weak, alias("Default_Handler")));
    void HDMI_CEC_IRQHandler(void)       __attribute__((weak, alias("Default_Handler")));
    void I2C4_EV_IRQHandler(void)        __attribute__((weak, alias("Default_Handler")));
    void I2C4_ER_IRQHandler(void)        __attribute__((weak, alias("Default_Handler")));
    void SPDIFRX_IRQHandler(void)        __attribute__((weak, alias("Default_Handler")));
    void DSIHOST_IRQHandler(void)        __attribute__((weak, alias("Default_Handler")));
    void DFSDM1_FLT0_IRQHandler(void)    __attribute__((weak, alias("Default_Handler")));
    void DFSDM1_FLT1_IRQHandler(void)    __attribute__((weak, alias("Default_Handler")));
    void DFSDM1_FLT2_IRQHandler(void)    __attribute__((weak, alias("Default_Handler")));
    void DFSDM1_FLT3_IRQHandler(void)    __attribute__((weak, alias("Default_Handler")));
    void SDMMC2_IRQHandler(void)         __attribute__((weak, alias("Default_Handler")));
    void CAN3_TX_IRQHandler(void)        __attribute__((weak, alias("Default_Handler")));
    void CAN3_RX0_IRQHandler(void)       __attribute__((weak, alias("Default_Handler")));
    void CAN3_RX1_IRQHandler(void)       __attribute__((weak, alias("Default_Handler")));
    void CAN3_SCE_IRQHandler(void)       __attribute__((weak, alias("Default_Handler")));
    void JPEG_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
    void MDIOS_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
}

uint32_t vectors[] __attribute__((section(".isr_vector"))) =
{
    STACK_START,
    (uint32_t)Reset_Handler,
    (uint32_t)NMI_Handler,
    (uint32_t)HardFault_Handler,
    (uint32_t)MemManage_Handler,
    (uint32_t)BusFault_Handler,
    (uint32_t)UsageFault_Handler,
    0, //reserved
    0, //reserved
    0, //reserved
    0, //reserved
    (uint32_t)SVC_Handler,
    (uint32_t)DebugMon_Handler, 
    0, //reserved
    (uint32_t)PendSV_Handler,
    (uint32_t)SysTick_Handler,
    (uint32_t)WWDG_IRQHandler,
    (uint32_t)PVD_IRQHandler,
    (uint32_t)TAMP_STAMP_IRQHandler,
    (uint32_t)RTC_WKUP_IRQHandler,
    0, // Flash global interrupt
    (uint32_t)RCC_IRQHandler,
    (uint32_t)EXTI0_IRQHandler,
    (uint32_t)EXTI1_IRQHandler,
    (uint32_t)EXTI2_IRQHandler,
    (uint32_t)EXTI3_IRQHandler,
    (uint32_t)EXTI4_IRQHandler,
    (uint32_t)DMA1_Channel1_IRQHandler,
    (uint32_t)DMA1_Channel2_IRQHandler,
    (uint32_t)DMA1_Channel3_IRQHandler,
    (uint32_t)DMA1_Channel4_IRQHandler,
    (uint32_t)DMA1_Channel5_IRQHandler,
    (uint32_t)DMA1_Channel6_IRQHandler,
    (uint32_t)DMA1_Channel7_IRQHandler,
    (uint32_t)ADC_IRQHandler,
    (uint32_t)CAN1_TX_IRQHandler,
    (uint32_t)CAN1_RX0_IRQHandler,
    (uint32_t)CAN1_RX1_IRQHandler,
    (uint32_t)CAN1_SCE_IRQHandler,
    (uint32_t)EXTI9_5_IRQHandler,
    (uint32_t)TIM1_BRK_IRQHandler,
    (uint32_t)TIM1_UP_IRQHandler,
    (uint32_t)TIM1_TRG_COM_IRQHandler,
    (uint32_t)TIM1_CC_IRQHandler,
    (uint32_t)TIM2_IRQHandler,
    (uint32_t)TIM3_IRQHandler,
    (uint32_t)TIM4_IRQHandler,
    (uint32_t)I2C1_EV_IRQHandler,
    (uint32_t)I2C1_ER_IRQHandler,
    (uint32_t)I2C2_EV_IRQHandler,
    (uint32_t)I2C2_ER_IRQHandler,
    (uint32_t)SPI1_IRQHandler,
    (uint32_t)SPI2_IRQHandler,
    (uint32_t)USART1_IRQHandler,
    (uint32_t)USART2_IRQHandler,
    (uint32_t)USART3_IRQHandler,
    (uint32_t)EXTI15_10_IRQHandler,
    (uint32_t)RTC_Alarm_IRQHandler,
    (uint32_t)USB_FS_WKUP_IRQHandler,
    (uint32_t)TIM8_BRK_IRQHandler,
    (uint32_t)TIM8_UP_IRQHandler,
    (uint32_t)TIM8_TRG_COM_IRQHandler,
    (uint32_t)TIM8_CC_IRQHandler,
    (uint32_t)DMA1_Channel8_IRQHandler,
    (uint32_t)FMC_IRQHandler,
    (uint32_t)SDMMC1_IRQHandler,
    (uint32_t)TIM5_IRQHandler,
    (uint32_t)SPI3_IRQHandler,
    (uint32_t)UART4_IRQHandler,
    (uint32_t)UART5_IRQHandler,
    (uint32_t)TIM6_DAC_IRQHandler,
    (uint32_t)TIM7_IRQHandler,
    (uint32_t)DMA2_Channel1_IRQHandler,
    (uint32_t)DMA2_Channel2_IRQHandler,
    (uint32_t)DMA2_Channel3_IRQHandler,
    (uint32_t)DMA2_Channel4_IRQHandler,
    (uint32_t)DMA2_Channel5_IRQHandler,
    (uint32_t)ETH_IRQHandler,
    (uint32_t)ETH_WKUP_IRQHandler,
    (uint32_t)CAN2_TX_IRQHandler,
    (uint32_t)CAN2_RX0_IRQHandler,
    (uint32_t)CAN2_RX1_IRQHandler,
    (uint32_t)CAN2_SCE_IRQHandler,
    (uint32_t)USB_FS_IRQHandler,
    (uint32_t)DMA2_Channel6_IRQHandler,
    (uint32_t)DMA2_Channel7_IRQHandler,
    (uint32_t)DMA2_Channel8_IRQHandler,
    (uint32_t)USART6_IRQHandler,
    (uint32_t)I2C3_EV_IRQHandler,
    (uint32_t)I2C3_ER_IRQHandler,
    (uint32_t)USB_HS_EP1_OUT_IRQHandler,
    (uint32_t)USB_HS_EP1_IN_IRQHandler,
    (uint32_t)USB_HS_WKUP_IRQHandler,
    (uint32_t)USB_HS_IRQHandler,
    (uint32_t)DCMI_IRQHandler,
    (uint32_t)CRYP_IRQHandler,
    (uint32_t)HASH_RNG_IRQHandler,
    (uint32_t)FPU_IRQHandler,
    (uint32_t)UART7_IRQHandler,
    (uint32_t)UART8_IRQHandler,
    (uint32_t)SPI4_IRQHandler,
    (uint32_t)SPI5_IRQHandler,
    (uint32_t)SPI6_IRQHandler,
    (uint32_t)SAI1_IRQHandler,
    (uint32_t)LCD_TFT_IRQHandler,
    (uint32_t)LCD_TFT_ERR_IRQHandler,
    (uint32_t)DMA2D_IRQHandler,
    (uint32_t)SAI2_IRQHandler,
    (uint32_t)QUADSPI_IRQHandler,
    (uint32_t)LPI_TIM1_IRQHandler,
    (uint32_t)HDMI_CEC_IRQHandler,
    (uint32_t)I2C4_EV_IRQHandler,
    (uint32_t)I2C4_ER_IRQHandler,
    (uint32_t)SPDIFRX_IRQHandler,
    (uint32_t)DSIHOST_IRQHandler,
    (uint32_t)DFSDM1_FLT0_IRQHandler,
    (uint32_t)DFSDM1_FLT1_IRQHandler,
    (uint32_t)DFSDM1_FLT2_IRQHandler,
    (uint32_t)DFSDM1_FLT3_IRQHandler,
    (uint32_t)SDMMC2_IRQHandler,
    (uint32_t)CAN3_TX_IRQHandler,
    (uint32_t)CAN3_RX0_IRQHandler,
    (uint32_t)CAN3_RX1_IRQHandler,
    (uint32_t)CAN3_SCE_IRQHandler,
    (uint32_t)JPEG_IRQHandler,
    (uint32_t)MDIOS_IRQHandler,    
};

// Command: a default "do nothing" handler
void Default_Handler(void)
{
  while (1)
    ;
}

// Command: reset memory and restart user program
[[noreturn]] void Reset_Handler(void)
{
  // copy .data section to RAM
  std::copy(&_la_data, &_la_data + (&_edata - &_sdata), &_sdata);

  // init. the .bss section to zero in RAM
  std::fill(&_sbss, &_ebss, 0);
  // now invoke main
  main();
  while (1)
  {
    __asm("nop");
  }
  
}

void *memcpy(void *dest, const void *src, size_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        ((char*)dest)[i] = ((char*)src)[i];
    }
    return dest;
}