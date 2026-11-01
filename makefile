# -----------------------
# Toolchain and flags
# -----------------------
CC = arm-none-eabi-gcc
OPENOCD = openocd

CFLAGS = --specs=nano.specs -mcpu=cortex-m3 -mthumb -std=c11 -Wall -g -O0
CPPFLAGS = \
	-Iinclude \
	-IApp \
	-Ilib/CMSIS \
	-Ilib/SPL/inc \
	-IMcal \
	-IBsw/Cfg \
	-IBsw/lcd \
	-IBsw/rc522 \
	-IBsw/servo \
	-IBsw/Systick \
	-IBsw/usbttl \
	-DSTM32F10X_MD \
	-DUSE_STDPERIPH_DRIVER

# -----------------------
# Sources and outputs
# -----------------------
SRC := $(wildcard lib/SPL/src/*.c) \
	$(wildcard Mcal/*.c) \
	$(wildcard Bsw/Cfg/*.c) \
	$(wildcard App/*.c) \
	$(wildcard Bsw/lcd/*.c) \
	$(wildcard Bsw/rc522/*.c) \
	$(wildcard Bsw/servo/*.c) \
	$(wildcard Bsw/Systick/*.c) \
	$(wildcard Bsw/usbttl/*.c)

STARTUP := startup/startup_stm32f103c8tx.s
OBJDIR := Object
OBJ := $(patsubst %.c,$(OBJDIR)/%.o,$(SRC)) \
	$(OBJDIR)/$(STARTUP:.s=.o)
DEPS := $(patsubst %.o,%.d,$(filter %.o,$(OBJ)))
OUT := build/main.elf

LDFLAGS = -T linker.ld -Wl,-Map=build/main.map -nostartfiles

.PHONY: all build run flash clean

all: $(OUT)

build: $(OUT)

$(OUT): $(OBJ) linker.ld
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(LDFLAGS)

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -MF $(@:.o=.d) -c $< -o $@

$(OBJDIR)/%.o: %.s
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

-include $(DEPS)

run: flash

flash: $(OUT)
	$(OPENOCD) -f interface/stlink.cfg -f target/stm32f1x.cfg \
		-c "program $(OUT) verify reset exit"

clean:
	rm -rf $(OBJDIR) build
