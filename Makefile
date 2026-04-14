# Makefile for build and generate .elf file

# Compilers
CC  = arm-none-eabi-gcc
CXX = arm-none-eabi-g++
OBJCOPY = arm-none-eabi-objcopy
SIZE    = arm-none-eabi-size

# Compiler flags
ARCH_FLAGS = -mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard -I Inc
CFLAGS     = $(ARCH_FLAGS) -O0 -ffunction-sections -fdata-sections -Wall --specs=nano.specs
CXXFLAGS   = $(ARCH_FLAGS) -O0 -ffunction-sections -fdata-sections -Wall -Wno-volatile -fno-exceptions -fno-rtti -std=c++20 --specs=nano.specs

# Linker flags
LDFLAGS = $(ARCH_FLAGS) -T STM32F767ZITX_CUSTOM.ld -Wl,--gc-sections -u Reset_Handler \
          --specs=nano.specs --specs=nosys.specs \
          -Wl,--start-group -lc -lm -lstdc++ -lsupc++ -Wl,--end-group

# Sources
C_SRCS   = $(shell find Src/ -type f -name '*.c')
CPP_SRCS = $(shell find Src/ -type f -name '*.cpp')

# Objects
C_OBJS   = $(patsubst Src/%.c,   $(BUILD_DIR)/%.c.o,   $(C_SRCS))
CPP_OBJS = $(patsubst Src/%.cpp, $(BUILD_DIR)/%.cpp.o, $(CPP_SRCS))

# Output
BUILD_DIR = Build
TARGET    = $(BUILD_DIR)/firmware

# Default target
.PHONY: all clean
all: $(TARGET).elf $(TARGET).bin
	$(SIZE) $(TARGET).elf

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile C sources
$(BUILD_DIR)/%.c.o: Src/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile C++ sources
$(BUILD_DIR)/%.cpp.o: Src/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link
$(TARGET).elf: $(C_OBJS) $(CPP_OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@

# Generate binary
$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

# Clean
clean:
	rm -rf $(BUILD_DIR)

