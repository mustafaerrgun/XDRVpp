# Optimization Configuration Guide

## Overview

This document describes compiler optimization levels for the STM32F767ZI bare-metal C++ project,
what each level does, and what configuration changes are required in the Makefile and source code
for each level.

---

## Optimization Levels

### `-O0` — No Optimization (Default Debug)

The compiler makes no attempt to optimize. Every function is compiled exactly as written,
nothing is inlined, and all variables are stored in memory rather than registers.

**Characteristics:**
- Every function exists as a separate, callable unit in the binary
- Template functions (e.g. `reg_access<addr, val>::bit_set()`) are not inlined
- Largest binary size
- Slowest execution speed
- Easiest to debug — every source line maps directly to instructions

**When to use:** During development, testing, and debugging on hardware.

---

### `-O1` — Basic Optimization

The compiler performs simple, low-risk optimizations. Dead code within a function is removed,
simple expressions are simplified, and some small functions may be inlined.

**Characteristics:**
- Minor code size reduction over `-O0`
- No aggressive inlining
- Debug information still reasonably accurate
- Safe for most embedded projects

**When to use:** When `-O0` is too slow for time-sensitive testing but full optimization is not yet needed.

---

### `-O2` — Full Optimization (Recommended Release)

The compiler performs all optimizations that do not involve a space-speed trade-off.
Includes function inlining, loop optimizations, instruction scheduling, and dead code elimination.

**Characteristics:**
- Significantly smaller and faster binary than `-O0`
- Aggressive inlining — small template functions will be inlined into call sites
- Debug stepping becomes difficult — source lines and instructions may not match
- Requires `-fno-inline-small-functions` for this project to prevent `--gc-sections` from
  stripping inlined template functions (see Configuration Changes below)

**When to use:** Production/release firmware.

---

### `-Os` — Optimize for Size

Performs all `-O2` optimizations but favors code size over execution speed. Avoids optimizations
that increase code size even if they would improve speed.

**Characteristics:**
- Smallest binary size
- Still aggressively inlines small functions
- Has the same stripping risk as `-O2` with `--gc-sections` on this project
- Requires the same protections as `-O2`

**When to use:** When flash space is critically limited. Not recommended until the project is
fully working at `-O2`.

---

### `-Og` — Optimize for Debugging

Performs only optimizations that do not interfere with debugging. Intended as a middle ground
between `-O0` and `-O1`.

**Characteristics:**
- Better code quality than `-O0` while keeping debug info accurate
- Variables are not optimized away
- Functions generally not inlined
- GCC-specific flag (not available in all compilers)

**When to use:** When `-O0` produces code too different from the optimized build to be useful
for testing, but full debug capability is still needed.

---

## Configuration Changes Per Optimization Level

### Makefile — `BUILD_TYPE` Variable

The Makefile supports two build modes controlled by `BUILD_TYPE`:

```makefile
BUILD_TYPE ?= debug

ifeq ($(BUILD_TYPE), release)
    OPT = -O2 -DNDEBUG -fno-inline-small-functions
else
    OPT = -O0 -g3
endif
```

| `BUILD_TYPE` | Flags applied |
|---|---|
| `debug` (default) | `-O0 -g3` |
| `release` | `-O2 -DNDEBUG -fno-inline-small-functions` |

**Build commands:**
```bash
make                     # debug build
make BUILD_TYPE=release  # release build
make clean               # clean build directory
```

---

### Flag Reference

| Flag | Used in | Purpose |
|---|---|---|
| `-O0` | Debug | No optimization, all functions separate |
| `-O2` | Release | Full optimization, smaller and faster binary |
| `-g3` | Debug | Full debug symbols including macro definitions |
| `-DNDEBUG` | Release | Disables `assert()` and other debug-only code |
| `-fno-inline-small-functions` | Release | Prevents inlining of small template functions, required to avoid `--gc-sections` stripping |
| `-ffunction-sections` | Both | Places each function in its own section, enables `--gc-sections` |
| `-fdata-sections` | Both | Places each variable in its own section, enables `--gc-sections` |

---

### Linker Flags — Both Modes

Regardless of optimization level, these linker flags must always be present:

```makefile
LDFLAGS = $(ARCH_FLAGS) -T STM32F767ZITX_CUSTOM.ld -Wl,--gc-sections \
          -u Reset_Handler -u main \
          --specs=nano.specs --specs=nosys.specs \
          -Wl,--start-group -lc -lm -lstdc++ -lsupc++ -Wl,--end-group
```

| Flag | Purpose |
|---|---|
| `-u Reset_Handler` | Forces linker to keep `Reset_Handler`, never strip it |
| `-u main` | Forces linker to keep `main`, never strip it |
| `--specs=nano.specs` | Links against minimal newlib (smaller than full libc) |
| `--specs=nosys.specs` | Provides stub syscall implementations for bare-metal |
| `-Wl,--gc-sections` | Strips unused sections — requires `-ffunction-sections` and `-fdata-sections` |

---

### Source Code — Function Protection Attributes

At higher optimization levels, the compiler and linker may strip functions that appear
unreferenced. Mark critical functions explicitly to prevent this.

**In `startup_stm32f767.cpp` — Reset Handler:**
```cpp
[[noreturn]] __attribute__((used)) void Reset_Handler(void)
```

**In `stm32f767.hpp` — Delay function:**
```cpp
__attribute__((noinline, used)) inline void delay(uint32_t ms)
```

| Attribute | Purpose |
|---|---|
| `__attribute__((used))` | Tells compiler/linker: never strip this symbol even if it appears unreferenced |
| `__attribute__((noinline))` | Prevents the optimizer from inlining this function into its call sites |
| `[[noreturn]]` | Tells compiler this function never returns, allows better code generation after the call |

---

## Why `-Os` Broke This Project

This project uses a heavily templated register access pattern:

```cpp
reg_access<addr, val>::bit_set();
reg_access<addr, val>::reg_or();
```

Each instantiation is a one-line function. At `-Os`, the compiler inlines all of them into
their callers, which inline into `gpio::init()`, which inlines into `run_test()`, which inlines
into `main()`. The entire program collapses into one function body.

`--gc-sections` then cannot identify individual reachable functions and strips the code
aggressively, reducing the binary from ~1900 bytes to ~268 bytes — just the vector table,
with no GPIO or delay logic.

**Root causes identified:**
1. `-Os` with heavy template inlining collapsed all code into `main()`
2. `-nostdlib` excluded the C++ runtime startup (fixed by switching to `--specs=nano.specs`)
3. Architecture flags (`-mfpu`, `-mfloat-abi`) were missing from `LDFLAGS`, causing
   a VFP register ABI mismatch at link time

---

## Size Comparison

| Build | Optimization | `text` size | Works on hardware |
|---|---|---|---|
| STM32CubeIDE | `-O0` | 1900 bytes | Yes |
| Makefile (broken) | `-Os` + `-nostdlib` | 268 bytes | No |
| Makefile (fixed) | `-O0` + `nano.specs` | 2032 bytes | Yes |
| Makefile (release target) | `-O2` + `-fno-inline-small-functions` | TBD | Expected yes |
