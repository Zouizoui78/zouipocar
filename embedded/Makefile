CC = avr-gcc
OBJCOPY = avr-objcopy
SIZE = avr-size
NM = avr-nm
AVRDUDE = avrdude
REMOVE = rm -f

MCU = atmega328p
F_CPU = 16000000

LFUSE = 0x9f
HFUSE = 0xd1

BUILDDIR=build
LIBDIR=$(BUILDDIR)/lib
OBJDIR=$(BUILDDIR)/obj

TARGETNAME = main
TARGET=$(BUILDDIR)/$(TARGETNAME)

SRCDIR=src
SRCEXT=.c
SRC=at.c bit.c gps.c interrupt.c main.c mc60.c pin.c time_utils.c uart.c

OBJDIR=$(BUILDDIR)/obj
OBJ=$(SRC:%$(SRCEXT)=$(OBJDIR)/%.o)
DEPS=$(OBJ:.o=.d)

FORMAT = ihex

CFLAGS = -DF_CPU=$(F_CPU)UL
CFLAGS += -Os
CFLAGS += -mmcu=$(MCU)
CFLAGS += -std=gnu99
CFLAGS += -funsigned-char -funsigned-bitfields -fshort-enums
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -Wall -Wstrict-prototypes -Werror
CFLAGS += -MMD -MP

LDFLAGS = -Wl,--gc-sections

AVRDUDE_MCU = m328p
AVRDUDE_PROGRAMMER = arduino
AVRDUDE_PORT=/dev/ttyACM0

AVRDUDE_FLAGS = -p $(AVRDUDE_MCU)
AVRDUDE_FLAGS += -c $(AVRDUDE_PROGRAMMER)
AVRDUDE_FLAGS += -P $(AVRDUDE_PORT)

MSG_LINKING = Linking:
MSG_COMPILING = Compiling:
MSG_FLASH = Preparing HEX file:

all: $(TARGET).hex

.SECONDARY: $(TARGET).elf
.PRECIOUS: $(OBJ)

$(TARGET).hex: $(TARGET).elf
	@echo $(MSG_FLASH) $@
	@$(OBJCOPY) -O $(FORMAT) -j .text -j .data $< $@

$(TARGET).elf: $(OBJ)
	@echo $(MSG_LINKING) $@
	@$(CC) -mmcu=$(MCU) $(LDFLAGS) $^ --output $@

$(OBJDIR)/%.o: $(SRCDIR)/%$(SRCEXT)
	@echo $(MSG_COMPILING) $<
	@mkdir -p $(OBJDIR)
	@$(CC) $(CFLAGS) -c $< -o $@

-include $(DEPS)

size: $(TARGET).elf
	@echo
	$(SIZE) -C --mcu=$(AVRDUDE_MCU) $(TARGET).elf

analyze: $(TARGET).elf
	$(NM) -S --size-sort -t decimal $(TARGET).elf

flash: $(TARGET).hex
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U flash:w:$(TARGET).hex

fuses:
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m

release: fuses isp

clean:
	@rm -r $(BUILDDIR)
