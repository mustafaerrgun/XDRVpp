# STM32F767 Bare-Metal C++ Firmware — Concepts Guide

A simple reference for everything covered during the development of this project.

---

## Table of Contents
1. [What is a Linker Script?](#1-what-is-a-linker-script)
2. [The Two Generated Linker Scripts](#2-the-two-generated-linker-scripts)
3. [Custom Linker Script Explained](#3-custom-linker-script-explained)
4. [What is the Startup File?](#4-what-is-the-startup-file)
5. [Reset_Handler](#5-reset_handler)
6. [Default_Handler](#6-default_handler)
7. [IRQ Handlers and Weak Aliases](#7-irq-handlers-and-weak-aliases)
8. [extern "C" in C++ Embedded Code](#8-extern-c-in-c-embedded-code)
9. [Implementing IRQ Handlers in Other Files](#9-implementing-irq-handlers-in-other-files)

---

## 1. What is a Linker Script?

After the compiler turns your `.cpp` files into object files (`.o`), the **linker** combines them into a single `.elf` binary. But the linker needs to know:

- What memory exists on the chip (Flash, RAM, their addresses and sizes)
- Where to place each piece of your program (code, constants, variables)

That is exactly what the linker script defines. Without it, the linker has no idea about your hardware.

**Simple analogy:** The compiler builds the furniture, the linker script is the floor plan that says where each piece goes in the house.

---

## 2. The Two Generated Linker Scripts

STM32CubeIDE generates two linker scripts automatically:

| File | Purpose |
|---|---|
| `STM32F767ZITX_FLASH.ld` | Normal build — code runs from Flash |
| `STM32F767ZITX_RAM.ld` | Debug build — code loaded entirely into RAM |

### Why two?

Flash memory has a limited number of erase/write cycles (~10,000–100,000).  
During development when you flash many times a day, **running from RAM avoids wearing out Flash** and is also faster to load.

You select which one to use in:
> Project Properties → C/C++ Build → Settings → MCU G++ Linker → General → Linker Script

---

## 3. Custom Linker Script Explained

File: `STM32F767ZITX_CUSTOM.ld`

### Step 1 — Entry point
```ld
ENTRY(Reset_Handler)
```
Tells the linker which function is the program entry. Debuggers use this to know where execution starts.

---

### Step 2 — Memory map
```ld
MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 2048K
    RAM   (rw)  : ORIGIN = 0x20000000, LENGTH = 512K
}
```
Describes the physical memory of the STM32F767:

| Region | Address | Size | Permissions |
|---|---|---|---|
| FLASH | `0x08000000` | 2 MB | read + execute |
| RAM | `0x20000000` | 512 KB | read + write |

These addresses come directly from the STM32F767 datasheet.

---

### Step 3 — Sections

#### `.isr_vector` — Vector table
```ld
.isr_vector :
{
    . = ALIGN(4);
    KEEP(*(.isr_vector))
    . = ALIGN(4);
} >FLASH
```
Places the interrupt vector table **first in Flash** at `0x08000000`.  
The Cortex-M7 hardware reads this address on reset — the position is mandatory.  
`KEEP` prevents the linker from removing it even with `--gc-sections`.

---

#### `.text` — Code and constants
```ld
.text :
{
    *(.text)
    *(.text.*)
    *(.rodata)
    *(.rodata.*)
    . = ALIGN(4);
    _etext = .;
} >FLASH
```
All compiled code and read-only data (string literals, `const` variables) go into Flash.  
`_etext` marks the end of code — the startup uses this symbol.

---

#### `.data` — Initialized global variables
```ld
_la_data = LOADADDR(.data);
.data :
{
    _sdata = .;
    *(.data)
    *(.data.*)
    . = ALIGN(4);
    _edata = .;
} >RAM AT> FLASH
```
For a variable like `int x = 42;`:
- The value `42` is **stored in Flash** (survives power-off)
- At runtime `x` must **live in RAM** (so it can be modified)

`>RAM AT> FLASH` means:
- **Runtime address (VMA)** → RAM
- **Storage address (LMA)** → Flash

The startup copies the initial values from Flash to RAM before `main()` using the symbols `_la_data`, `_sdata`, `_edata`.

---

#### `.bss` — Uninitialized global variables
```ld
.bss :
{
    _sbss = .;
    *(.bss)
    *(.bss.*)
    . = ALIGN(4);
    _ebss = .;
    end = .;
} >RAM
```
Variables like `int y;` (no initializer) are guaranteed to be zero by the C/C++ standard.  
They take no space in Flash — the startup simply zeroes this RAM region before `main()`.

---

### Big Picture

```
FLASH (0x08000000)                RAM (0x20000000)
┌──────────────────┐              ┌──────────────────┐
│  .isr_vector     │              │  .data           │ ← copied from Flash at startup
│  .text (code)    │              │  .bss            │ ← zeroed at startup
│  .rodata         │              │  heap / stack    │ ← grows at runtime
│  .data values    │ ──copy──>    │                  │
└──────────────────┘              └──────────────────┘
```

---

## 4. What is the Startup File?

File: `Src/startup_stm32f767.cpp`

The startup file is the **first code that runs** when the MCU powers on or resets — before `main()`.

The Cortex-M7 hardware on reset:
1. Loads the stack pointer from address `0x08000000` (first word of Flash)
2. Jumps to the address at `0x08000000 + 4` (second word — `Reset_Handler`)

Everything else — copying `.data`, zeroing `.bss`, calling `main()` — is the startup file's job.

### The startup file does 3 things:

| Step | What | Why |
|---|---|---|
| 1 | Defines the vector table | Hardware reads handler addresses from it |
| 2 | Copies `.data` from Flash to RAM | Initialized globals need their values in RAM |
| 3 | Zeroes `.bss` | C/C++ standard guarantees uninitialized globals are zero |

Only after steps 2 and 3 does the C/C++ memory model hold, so `main()` is called last.

---

## 5. Reset_Handler

```cpp
[[noreturn]] void Reset_Handler(void)
{
    // Step 1: copy .data from Flash to RAM
    uint32_t *src = &_la_data;
    uint32_t *dst = &_sdata;
    while (dst < &_edata)
        *dst++ = *src++;

    // Step 2: zero .bss
    std::fill(&_sbss, &_ebss, 0);

    // Step 3: call main
    main();

    // should never reach here
    while (1) { __asm("nop"); }
}
```

- It is the **second entry in the vector table** (position 1, after the stack pointer)
- The hardware jumps here immediately after reset
- It prepares memory so the C/C++ runtime assumptions hold before `main()` is called
- It must be declared `extern "C"` so the linker can find it by exact name (see section 8)

---

## 6. Default_Handler

```cpp
void Default_Handler(void)
{
    while (1);
}
```

A simple infinite loop. It is the **fallback handler** for every interrupt that has no real implementation.

If an unexpected interrupt fires (one you never enabled but somehow triggers due to a hardware bug or misconfiguration), the MCU lands here and halts — which is safe and easy to catch with a debugger.

---

## 7. IRQ Handlers and Weak Aliases

The STM32F767 has ~110 interrupt sources. All of their handler addresses must exist in the vector table — the hardware requires a valid address at every position.

But you don't want to implement all 110 handlers. The solution is **weak aliases**:

```cpp
void TIM2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
```

- **`weak`** — if you define `TIM2_IRQHandler` anywhere in your project, the linker uses your version and discards this one
- **`alias`** — if you never define it, it silently points to `Default_Handler`

### Workflow
- All handlers declared weak → vector table compiles with no undefined symbols
- When you need an interrupt, define the real handler in any `.cpp` file → linker picks it up automatically
- Unused ones stay as `Default_Handler` — safe fallback

---

## 8. `extern "C"` in C++ Embedded Code

C++ **mangles** function names to support overloading. For example:

```
void Reset_Handler(void)
→ becomes "_Z13Reset_Handlerv" in the object file
```

The hardware and linker script reference functions by their **exact raw name**. If the name is mangled, the linker cannot find it.

`extern "C"` disables name mangling for a function:

```cpp
extern "C"
{
    void Reset_Handler(void);   // stays "Reset_Handler" — not mangled
    void Default_Handler(void); // stays "Default_Handler" — alias target must match exactly
}
```

### Simple rule
> Any function that the hardware, linker script, or assembler needs to find by name must be `extern "C"`.

This applies to:
- `Reset_Handler` — hardware jumps to it on reset
- `Default_Handler` — used as the `alias` string target
- All IRQ handlers — stored by address in the vector table, overridden by name matching

---

## 9. Implementing IRQ Handlers in Other Files

When you implement a real IRQ handler in another `.cpp` file, you **must** use `extern "C"`:

```cpp
// gpio.cpp
extern "C" void EXTI0_IRQHandler(void)
{
    // your interrupt code
}
```

### Without `extern "C"`
- Compiler mangles the name → `_Z16EXTI0_IRQHandlerv`
- Linker cannot match it to the weak alias `"EXTI0_IRQHandler"`
- Weak alias stays → `Default_Handler` is called instead of your code
- **No error, no warning — completely silent bug**

### With `extern "C"`
- Symbol stays as `EXTI0_IRQHandler`
- Linker overrides the weak alias with your implementation
- Your handler runs correctly

### Recommended pattern — declare in a header

```cpp
// irq_handlers.h
#pragma once

extern "C"
{
    void EXTI0_IRQHandler(void);
    void TIM2_IRQHandler(void);
    // only the ones you actually implement
}
```

Include this header in every `.cpp` file that implements handlers to enforce consistent linkage.

---

*Generated for project: cpp_firmware_gpio — STM32F767ZITx*
