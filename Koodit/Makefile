MCU     = atmega328p
F_CPU   = 16000000UL

CC      = avr-gcc
AR      = ar
OBJCOPY = avr-objcopy
SIZE    = avr-size
AVRDUDE = avrdude

# Project
TARGET  = st7735_test
OBJDIR  = bin

# Programmer
PORT    = COM12
BAUD    = 9600
PROGRAMMER = arduino_as_isp

# Source files
SRC = main.c \
      spi.c \
      st7735.c \
      st7735_gfx.c \
      st7735_font.c \
      adc.c  \
      uart.c   

# Object files
OBJ = $(SRC:%.c=$(OBJDIR)/%.o)
DEPS = $(OBJ:.o=.d)

# Includes
CFLAGS  = -mmcu=$(MCU) -DF_CPU=$(F_CPU)
CFLAGS += -Os -Wall -Wextra -std=c99
CFLAGS += -Iinclude -Iimages -Ifonts
CFLAGS += -MMD -MP
CFLAGS += -ffunction-sections -fdata-sections

# Linker
LDFLAGS = -mmcu=$(MCU) -Wl,--gc-sections

# Default target
all: $(TARGET).hex

# Link
$(TARGET).elf: $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

# Compile
$(OBJDIR)/%.o: %.c
	@if not exist $(OBJDIR) mkdir $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# HEX
$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

# Size
size: $(TARGET).elf
	$(SIZE) --mcu=$(MCU) -C $<

# Flash
flash: $(TARGET).hex
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCU) -P $(PORT) -b $(BAUD) \
	           -U flash:w:$<:i

# Clean
clean:
	rm -rf $(OBJDIR) *.elf *.hex

-include $(DEPS)

.PHONY: all flash clean size
