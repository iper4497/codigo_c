CPUFLAGS=-Di386 -Di486 -Di686 -Di586  
# Well, we have old silicon
CPUFLAGS += -DINTEL_BRIDGE_CONFIG 
CPUFLAGS += -DINTEL_PPRO_MTRR  
CPUFLAGS += -DNEWPCI
CPUFLAGS += -DSERIAL_CONSOLE
CPUFLAGS += -DFINAL_MAINBOARD_FIXUP
CPUFLAGS += -DPROTECTED_MODE_STARTUP
# If you enable FIXED_AND_VARIABLE it never makes it to the kernel!
# you have to only enable variable. 
CPUFLAGS += -DENABLE_FIXED_AND_VARIABLE_MTRRS
CPUFLAGS += -DRAMTEST
CPUFLAGS += -DUSE_DOC_MIL
# This makes it fail sooner ...
#CPUFLAGS += -DINBUF_COPY
CPUFLAGS += -DCMD_LINE='"ro root=/dev/hda1 console=ttyS0,115200 single "'

TOP=../..
INCLUDES=-nostdinc -I $(TOP)/src/include  -I $(TOP)/src/arch/i386/include
NOOPT_CFLAGS=$(INCLUDES) $(CPUFLAGS) -Wall
CFLAGS=$(NOOPT_CFLAGS) -O2

OBJECTS=crt0.o hardwaremain.o linuxbiosmain.o 
OBJECTS += mainboard.o mtrr.o subr.o fill_inbuf.o params.o
OBJECTS += southbridge.o northbridge.o
#OBJECTS += pci.o
OBJECTS += printk.o vsprintf.o
OBJECTS += newpci.o linuxpci.o
OBJECTS += cpuid.o 
#OBJECTS += intel_irq_tables.o
OBJECTS += serial_subr.o
OBJECTS += mpspec.o
OBJECTS += microcode.o
OBJECTS += keyboard.o
OBJECTS += ioapic.o
LINUX=$(TOP)/../linux-2.4.0-ali/

LINK = ld -T ldscript.ld -o $@ $(OBJECTS)
CC=cc $(CFLAGS)
CCASM=cc -I$(TOP)/chip/intel $(CFLAGS)

all: romimage
floppy: all 
	mcopy -o romimage a:
# here's the problem: we shouldn't assume we come up with more than 
# 64K of FLASH up. SO we need a working linuxbios at the tail, and it will
# enable all flash and then gunzip the linuxbios. As a result, 
# we need the vmlinux.bin.gz padded out and then cat the linuxbios.rom
# at then end. We always copy it to /tmp so that a waiting root shell 
# can put it on the floppy (see ROOTDOIT)
romimage: linuxbios.rom vmlinux.bin.gz.block docipl
	cat vmlinux.bin.gz.block linuxbios.rom > romimage
	cp romimage /tmp

docipl: ipl.o
	objcopy -O binary -R .note -R .comment -S ipl.o adocipl
	dd if=adocipl of=docipl bs=1 skip=1040384

# I have noe idea why you can not use $(CPUFLAGS) to compile ipl.S
# a bug with -Di686 ??
ipl.o: $(TOP)/src/northbridge/acer/m1631/ipl.S
	gcc -save-temps -c -I$(TOP)/src/include -I$(TOP)/src $<

linuxbios.rom: linuxbios.strip mkrom 
	./mkrom -s 64 -f -o linuxbios.rom linuxbios.strip

linuxbios.strip: linuxbios
	objcopy -O binary -R .note -R .comment -S linuxbios linuxbios.strip

linuxbios: $(OBJECTS) vmlinux.bin.gz
	@rm -f biosobject
	$(LINK) 
	nm -n linuxbios > linuxbios.map

# crt0 actually includes .inc files. 
# For self-documenting purposes, we put the FULL PATH of the 
# .inc files (relative to $TOP/src) in crt0.S. 
# So, for example, earlymtrr.inc is included as cpu/p6/earlymtrr.inc
# To make this work, add the extra -I $(TOP)/src here. 
crt0.s: $(TOP)/src/mainboard/asus/cua/crt0.S 
	$(CCASM) -I $(TOP)/src -E $< > crt0.s

crt0.o : crt0.s
	$(CCASM) -c crt0.s

mkrom: $(TOP)/mkrom/mkrom.c
	cc -o mkrom $<

linuxbiosmain.o: $(TOP)/src/lib/linuxbiosmain.c
	cc $(CFLAGS) -c $<

mainboard.o: $(TOP)/src/mainboard/asus/cua/mainboard.c
	cc $(CFLAGS) -c $<

fill_inbuf.o: $(TOP)/src/lib/fill_inbuf.c
	cc $(CFLAGS) -c $<

params.o: $(TOP)/src/lib/params.c
	cc $(CFLAGS) $(LINUXINCLUDE) -c $<

ioapic.o: $(TOP)/src/cpu/p6/ioapic.c
	cc $(CFLAGS) $(LINUXINCLUDE) -c $<

hardwaremain.o: $(TOP)/src/lib/hardwaremain.c
	cc $(CFLAGS) -c $<

southbridge.o: $(TOP)/src/southbridge/acer/m1535/southbridge.c
	cc $(CFLAGS) -c $<

northbridge.o: $(TOP)/src/northbridge/acer/m1631/northbridge.c
	cc $(CFLAGS) -c $<

pci.o:  $(TOP)/src/lib/pci.c
	cc $(CFLAGS) -c $<

# not on this machine, not yet. 
#intel_irq_tables.o: ../../chip/intel/intel_irq_tables.c
#	cc $(CFLAGS) -o $@ -c $<

mtrr.o: $(TOP)/src/cpu/p6/mtrr.c
	cc $(CFLAGS) -c $<

subr.o: $(TOP)/src/lib/subr.c
	cc $(CFLAGS) -c $<

keyboard.o: $(TOP)/src/pc80/keyboard.c
	cc $(CFLAGS) -c $<

cpuid.o: $(TOP)/src/cpu/p5/cpuid.c
	cc $(CFLAGS) -c $<

mpspec.o: $(TOP)/src/cpu/p6/mpspec.c
	$(CC) $(CFLAGS) -c $<

microcode.o: $(TOP)/src/cpu/p6/microcode.c
	$(CC) $(CFLAGS) -c $<

serial_subr.o: $(TOP)/src/lib/serial_subr.c
	cc $(CFLAGS) -c $<

printk.o: $(TOP)/src/lib/printk.c
	cc $(CFLAGS) -c $<

vsprintf.o: $(TOP)/src/lib/vsprintf.c
	cc $(CFLAGS) -c $<

newpci.o: $(TOP)/src/lib/newpci.c
	cc $(CFLAGS) -c $<

linuxpci.o: $(TOP)/src/lib/linuxpci.c
	cc $(CFLAGS) -c $<

vmlinux.bin.gz.block: vmlinux.bin.gz
	dd conv=sync bs=448k if=vmlinux.bin.gz of=vmlinux.bin.gz.block
vmlinux.bin.gz: vmlinux.bin
	gzip -f -3 vmlinux.bin 

vmlinux.bin: $(LINUX)/vmlinux
	objcopy -O binary -R .note -R .comment -S $< vmlinux.bin

alltags: 
	gctags ../inflate/*.c ../../lib/*.c ../../chip/intel/*.c
	etags ../inflate/*.c ../../lib/*.c ../../chip/intel/*.c


clean:: 
	rm -f linuxbios.* vmlinux.* *.o mkrom xa? *~ linuxbios romimage crt0.s
	rm -f a.out *.s *.l
	rm -f TAGS tags





