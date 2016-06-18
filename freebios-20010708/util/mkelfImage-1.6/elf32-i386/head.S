#
# exec_kernel/user_space/head.S
#
# Copyright (C) 2000 Eric Biederman
#
# Parts of this code were take from the linux startup
# code of linux-2.4.0-test9
#

.text
	.globl startup_32
startup_32:
	cld
	cli

	# Save the arguments safely out of the way
	movl %eax, %ebp
	movl %ebx, %esi

	movl stack_start, %esp


	#
	## Put the Hardware in a sane state
	#

	# Disable nmi
	mov	$0x80, %al
	outb	%al, $0x70

	# Make sure any coprocessor is properly reset..
	xorl	%eax, %eax
	outb	%al, $0xf0
	call	delay

	outb	%al, $0xf1
	call	delay
	

/*
 * well, that went ok, I hope. Now we have to reprogram the interrupts :-(
 * we put them right after the intel-reserved hardware interrupts, at
 * int 0x20-0x2F. There they won't mess up anything. Sadly IBM really
 * messed this up with the original PC, and they haven't been able to
 * rectify it afterwards. Thus the bios puts interrupts at 0x08-0x0f,
 * which is used for the internal hardware interrupts as well. We just
 * have to reprogram the 8259's, and it isn't fun.
 */

	movb	$0x11, %al		# initialization sequence
	outb	%al, $0x20		# send it to 8259A-1
	call	delay
	outb	%al, $0xA0		# and to 8259A-2
	call	delay
	
	mov	$0x20, %al		# start of hardware ints (0x20)
	outb	%al, $0x21
	call	delay
	
	mov	$0x28, %al		# start of hardware ints 2 (0x28)
	outb	%al, $0xA1
	call	delay
	
	mov	$0x04, %al		# 8259-1 is master
	outb	%al, $0x21
	call	delay
	mov	$0x02, %al		# 8259-2 is slave
	outb	%al, $0xA1
	call	delay
	
	mov	$0x01, %al		# 8086 mode for both
	out	%al, $0x21
	call	delay
	out	%al, $0xA1
	call	delay
	
	mov	$0xFF, %al		# mask off all interrupts for now
	out	%al, $0xA1
	call	delay
	
	mov	$0xFB, %al		# mask all irqs but irq2 which
	out	%al, $0x21		# is cascaded

	# Initialize eflags.  Some BIOSs leave bits like NT set.  This would
	# confuse the debugger if this code is traced.
	# XXX - best to initialize before switching to protected mode.

	pushl $0
	popfl

	# Clear BSS
	xorl %eax,%eax
	movl $ _edata,%edi
	movl $ _end,%ecx
	subl %edi,%ecx
	cld
	rep
	stosb

	pushl %esi	# boot data pointer as second arg
	pushl %ebp	# boot data type as first argument
	call convert_params
	
	movl %eax, %esi # put the real mode pointer in a safe place
	addl $8, %esp	# pop the arguments

	# Setup the registers before jumping to linux

	# Linux makes stupid assumptions about the segments
	# that are already setup, so setup a new gdt & ldt
	# and then reload the segment registers.
	
	lgdt	gdt_48

	lidt	idt_48

	# Load the data segment registers
	movl	$ 0x18, %eax
	movl	%eax, %ds
	movl	%eax, %es
	movl	%eax, %fs
	movl	%eax, %gs
	movl    %eax, %ss

	# clear eflags
	pushl	$0
	popfl	

	# Flag to indicate we are the bootstrap processor
	xorl	%ebx, %ebx
	
	# Clear the unspecified registers for good measure
	xorl	%eax, %eax
	xorl	%ecx, %ecx
	xorl	%edx, %edx
	xorl	%edi, %edi
	xorl	%esp, %esp
	xorl	%ebp, %ebp


	# Jump to the linux kernel
	ljmp	$ 0x10 , $ 0x100000

# Delay is needed after doing I/O
delay:
	jmp	.+2				# jmp $+2
	ret

# Descriptor tables
# These need to be in a seperate section so I can be
# certain later activities dont stomp them
.section .nokill, "awx", @progbits
gdt:
	.word	0, 0, 0, 0			# dummy
	.word	0, 0, 0, 0			# unused

	.word	0xFFFF				# 4Gb - (0x100000*0x1000 = 4Gb)
	.word	0				# base address = 0
	.word	0x9A00				# code read/exec
	.word	0x00CF				# granularity = 4096, 386
						#  (+5th nibble of limit)

	.word	0xFFFF				# 4Gb - (0x100000*0x1000 = 4Gb)
	.word	0				# base address = 0
	.word	0x9200				# data read/write
	.word	0x00CF				# granularity = 4096, 386
						#  (+5th nibble of limit)
idt_48:
	.word	0				# idt limit = 0
	.word	0, 0				# idt base = 0L
gdt_48:
	.word	32				# gdt limit=32,
						# (4 GDT entries)
	.long   gdt				# gdt base

