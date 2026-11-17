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
DEPDIR := $(OBJDIR)/deps
OBJ := $(patsubst %.c,$(OBJDIR)/%.o,$(SRC)) \
	$(OBJDIR)/$(STARTUP:.s=.o)
DEPS := $(patsubst %.c,$(DEPDIR)/%.d,$(SRC))
OUT := build/main.elf

LDFLAGS = -T linker.ld -Wl,-Map=build/main.map -nostartfiles

.PHONY: all build run flash clean changes

all: build

build: $(OUT)
	@$(MAKE) --no-print-directory changes

$(OUT): $(OBJ) linker.ld
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(LDFLAGS)
	@echo Successfully compiled: $@

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@mkdir -p $(dir $(DEPDIR)/$*.d)
	@echo Compiling source: $<
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -MF $(DEPDIR)/$*.d -c $< -o $@

$(OBJDIR)/%.o: %.s
	@mkdir -p $(dir $@)
	@echo Compiling source: $<
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

-include $(DEPS)

run: flash

flash: $(OUT)
	$(OPENOCD) -f interface/stlink.cfg -f target/stm32f1x.cfg \
		-c "program $(OUT) verify reset exit"

clean:
	rm -rf $(OBJDIR) build

changes:
	@git -c safe.directory="$(CURDIR)" ls-files --others --exclude-standard | while IFS= read -r file; do \
		if [ -f "$$file" ]; then \
			line_count=$$(wc -l < "$$file" | tr -d ' '); \
			printf "%s\n  + lines 1-%s (new file)\n" "$$file" "$$line_count"; \
		fi; \
	done
