
## process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS = -I m4
AUTOMAKE_OPTIONS = subdir-objects

AM_CFLAGS = -I./libc/include
AM_CCASFLAGS = $(AM_CFLAGS) $(CFLAGS)
AM_LDFLAGS = --oformat=binary

noinst_LTLIBRARIES = libc/libc.la

libc_libc_la_SOURCES = \
    libc/errno/errno.c \
    libc/stdio/printf.c \
    libc/stdio/putchar.c \
    libc/stdio/puts.c \
    libc/stdio/vprintf.c \
    libc/stdlib/malloc.c \
    libc/string/memcmp.c \
    libc/string/memcpy.c \
    libc/string/memset.c \
    libc/unistd/sbrk.c \
    libc/include/errno.h \
    libc/include/stddef.h \
    libc/include/stdio.h \
    libhc/include/stdlib.h \
    libc/include/string.h \
    libc/include/sys/types.h \
    libc/include/unistd.h

noinst_PROGRAMS = ninjastorms

ninjastorms_SOURCES = \
    kernel/boot/start.S \
    kernel/main.c kernel/main.h \
    kernel/scheduler.c kernel/scheduler.h \
    kernel/interrupt.c kernel/interrupt.h \
    kernel/syscall_handler.c kernel/syscall_handler.h \
    kernel/syscall.c kernel/syscall.h \
    kernel/utilities.c kernel/utilities.h \
    kernel/interrupt_handler.S kernel/interrupt_handler.h \
    kernel/memory.h \
    kernel/mmio.c kernel/mmio.h \
    kernel/pci/pci_mmio.c kernel/pci/pci_mmio.h \
    kernel/drivers/adc.c kernel/drivers/adc.h \
    kernel/drivers/button.c kernel/drivers/button.h \
    kernel/drivers/gpio.c kernel/drivers/gpio.h \
    kernel/drivers/led.c kernel/drivers/led.h \
    kernel/drivers/motor.c kernel/drivers/motor.h \
    kernel/drivers/pininfo.c kernel/drivers/pininfo.h \
    kernel/drivers/sensor.c kernel/drivers/sensor.h \
    kernel/drivers/spi.c kernel/drivers/spi.h \
    kernel/drivers/timer.c kernel/drivers/timer.h \
    kernel/pci/pci.c kernel/pci/pci.h \
    kernel/network/e1000.c kernel/network/e1000.h \
    kernel/network/network_task.c kernel/network/network_task.h \
    kernel/logger/logger.c kernel/logger/logger.h \
    kernel/network/network_io.c kernel/network/network_io.h \
    kernel/network/arp.c kernel/network/arp.h \
    kernel/network/ethernet_frame.c kernel/network/ethernet_frame.h \
    kernel/network/pdu_handler.c kernel/network/pdu_handler.h

ninjastorms_LDADD = libc/libc.la -lgcc
ninjastorms_LDFLAGS = -T kernel/link-arm-eabi.ld -Ttext $(LOADADDR)

all-local: uImage

# workaround for generating plain binaries for arm targets
$(noinst_PROGRAMS).bin: $(noinst_PROGRAMS)
	if file $< | grep executable >/dev/null; then $(STRIP) -O binary -o $<.bin $<; fi

# generate a boot.scr for booting via ymodem download
boot.scr: boot.cmd
	@echo "  MKIMAGE  boot.scr"
	$(Q)mkimage -C none -A arm -T script -d boot.cmd boot.scr

# generate uImage for downloading or booting directly from SD
uImage: $(noinst_PROGRAMS).bin
	@echo "  MKIMAGE  ninjastorms"
	$(Q)mkimage -C none -A arm -T kernel -O linux -a $(LOADADDR) -e $(LOADADDR)  -d $< uImage
