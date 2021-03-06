/*
This software and ancillary information (herein called SOFTWARE )
called LinuxBIOS          is made available under the terms described
here.  The SOFTWARE has been approved for release with associated
LA-CC Number 00-34   .  Unless otherwise indicated, this SOFTWARE has
been authored by an employee or employees of the University of
California, operator of the Los Alamos National Laboratory under
Contract No. W-7405-ENG-36 with the U.S. Department of Energy.  The
U.S. Government has rights to use, reproduce, and distribute this
SOFTWARE.  The public may copy, distribute, prepare derivative works
and publicly display this SOFTWARE without charge, provided that this
Notice and any statement of authorship are reproduced on all copies.
Neither the Government nor the University makes any warranty, express
or implied, or assumes any liability or responsibility for the use of
this SOFTWARE.  If SOFTWARE is modified to produce derivative works,
such modified SOFTWARE should be clearly marked, so as not to confuse
it with the version available from LANL.
 */
/* Parts of this code Copyright 2000, Ron Minnich, Advanced Computing Lab, LANL
 * rminnich@lanl.gov
 * Modifications by Ronnie Liu of Acer Labs Inc.
 */
#include <asm.h>
#include <cpu/p5/macros.h>
#define DOC_WIN_SEG		0xfe00
#define DOC_STACK_SEG		0x0400
#define SPL_RAM_SEG		0x8000

#define DOC_SPL_START_BLK	2	/* 0,1 for IPL */
#define DOC_SPL_SIZE_IN_PAGE	126     /* 1 page = 512 bytes, total 63kB */

/* wow, the usual way to to this hurts. So we do it our way:
 * 32-bit test not needed. 
 */
/* the MCR is 32-bits. You set it, it programs SDRAM. 
 * first check: get Column address size (CAS)
 * Start out assuming that it is 8 bits, then grow. 
 * nicely, if we put the 32-bit MCR value in %ecs, we can 
 * ror it 16 bits, and as we grow the CAS, we just inc cs, and that will
 * set the right value. 
 */
#define INIT_MCR $0xf663b83c
/* GNU AS misassembles this. I can't believe what a piece of 
 * trash gas has turned out to be ...
#define LONGDELAY movw	$0x1000, ax; 1: dec %ax; jnz 1b
 */
#define LONGDELAY  1: dec %ax; jnz 1b
#define WDELAY outb %al, $0x80; 1: outb %al, $0x80
#define WRITE_MCR0 movl %ecx, %eax ;  outl %eax, %dx ; WDELAY
/*#include <cpu/p5/start32.inc>*/
.org 0xfe000
.code16
_start:	jmp _realstart
gdt:	 //GDT entry 0 (null segment)
	.byte 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	.byte 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	// GDT 1
	.word	0xffff, 0x0000		/* flat code segment */
	.byte	0x0, 0x9a, 0xcf, 0x0	
	
	.word	0xffff, 0x0000		/* flat data segment */
	.byte	0x0, 0x92, 0xcf, 0x0	

gdtptr:
	.word	4*8-1
	.long	gdt		/* we know the offset */
	.long 0

_realstart:	

	cli
	movw	%cs, %ax
	movw	%ax, %es

	.byte 0x66 /* prefix */
	.byte 0x26
	.byte 0x0f, 0x01, 0x16 /* lgdt [es:offset]*/
	.word  gdtptr -  EXT(_start) /* offset in segment */

	movl	%cr0, %eax
	andl	$0x7FFAFFD1, %eax /* PG,AM,WP,NE,TS,EM,MP = 0 */
	orl	$0x60000001, %eax /* CD, NW, PE = 1 */
	movl	%eax, %cr0

	/* invalidate the cache */
	invd
	data32	ljmp	$0x10, $.Lprotected

.Lprotected:
	.code32
	movw	$0x18, %bx
	movw	%bx, %ds
	movw	%bx, %es
/*
	movw	%bx, %fs
	movw	%bx, %gs
 */
	movw	%bx, %ss
	xorl	%edi, %edi
#ifdef USE_BIG_REAL
	andb	$0xfe, %al
	movl	%eax, %cr0
	ljmp $0xfe00, $.Lunprotected
Lunprotected:
code16
#endif

/* make the flash writeable */
/* select the south bridge, register 44 (base) */
/* south bridge is device 7. 7 << 3 is 0x38 */
	movl	$0x80003844, %eax
	mov	$0x0cf8,%dx
	outl	%eax,%dx
/* but 0x40 makes flash writeable. You need this for DoC */
	movb	$0x40, %al
/* 0xff selects register 0x47 */
	movb	$0xff, %dl
	outb	%al,%dx

/*	Must set regs on North Bridge (device 0) */

	movw	$0x00f8,%ax	/* f9h */
	movb	$0xf8,%dl
	outl	%eax,%dx
	movb	$0xfd,%dl
	movb	$0x08,%al
	outb	%al,%dx

	movb	$0x8c,%al	/* 8eh */
	movb	$0xf8,%dl
	outl	%eax,%dx
	movb	$0xfe,%dl
	inb	%dx,%al
	orb	$0x08,%al
	outb	%al,%dx

	movb	$0x80,%al	/* 82h */
	movb	$0xf8,%dl
	outl	%eax,%dx
	movb	$0xfe,%dl
	movb	$0x10,%al
	outb	%al,%dx

	// move 68 to al, we add 4 below, this is for looping
#if 0
	movb	$0x68, %al
	movl	%eax, %esp
	/*	CALLSP(pci_read_dword)*/
sizeloop:
	movl	%esp, %eax
	addb	$4, %al
	// cmpb	$0x78, %al
	// for now, just do the first bank. 
	// We don't have enough bytes to figure out how to 
	// handle empty banks
	cmpb	$0x70, %al
	jge	sizedone
#endif
	movb	$0x6c, %al
	movb	$0xf8,%dl
	outl	%eax,%dx
	movl	%eax, %esp
	movb	$0xfc, %dl
	movl	INIT_MCR, %ecx
	WRITE_MCR0
// Not only does this not work, it screws up the programming. 
#if 0
	/* check for registered. All this means is write to 0,
	 * read back, if it's not the same, set 'registered',
	 * read back, it it's the same you're ok.
	 */
	movb	$0xa5, (%edi)
	cmpb	$0xa5, (%edi)
	je	nonregistered
	orb	$0x40, %cl
	WRITE_MCR0
	/*
	movb	$0x6c, %al
	CALLSP(pci_write_dword)
	*/
nonregistered:	 
#endif
	rorl	$16, %ecx
	/* clear out the CAS, since we assume start at zero
	 * and increment 
	 */
	andb	$0xf0, %cl
	/* test 8 bit CAS */
Secondtry: 
	movl	$0x800, %esi
	movl	%edi, (%edi)
	movb	$3, %bl
1:
	movb	$1, (%esi) // esi is at 0x800
	cmpl	%edi,(%edi)
	jnz	sizeram
	inc	%cl
	roll	%esi
	dec	%bl
	jnz	1b
#if 0
	/* change the value we store each time. It makes debugging easier */
	movb	$2, (%esi) // esi is at 0x1000
	cmpl	%edi, (%edi)
	jnz	sizeram
	inc	%cl
	roll	%esi
	movb	$3, (%esi) // esi is at 0x2000
	cmpl	%edi, (%edi)
	jnz	sizeram
	inc	%cl
	roll	%esi
	movb	$4, (%esi) // esi is at 0x4000
	cmpl	%edi, (%edi)
	jnz	sizeram
#endif
#if 0
	// This code does not work ...
	// OK, nothing matched. 
	// This could be 64MB 2 internal banks. Dammit.
	andb	$0xf0, %cl
	orb	$0x0a, %cl
	WRITE_MCR0
	jmp	secondtry
#endif

	
sizeram:		
	rorl	$16, %ecx
	/* clear the 'no multi page' bit. */
	/* NOTE: 3/22/01 -- I think this is wrong! -- rgm
	 * I think we should OR IN 0x1000, which we already did!
	andw	$0xefff, %cx
	 */
	WRITE_MCR0 
	/*
	mov	$0x6c, %al
	CALLSP(pci_write_dword)
	*/
/* size is now in cx[19:16] */
/* now size the dram */
/* you had best have at least 4M; that's as small as we go */
/* rorr 20 the ecx value, to get row size into lsb */
	movl	%edi, (%edi)
	ror	$20, %ecx
	/* clear the size out to 4 MB */
	andb	$0xf8, %cl
	/* 4 MB */
	movl	$0x400000, %esi
1:
#ifdef USE_BIG_REAL
	addr32 	movb	$4, %es:(%esi)
#else
	/* move a 4 to the next power-of-two address. 
	 * if there is no memory there, it will wrap to zero 
	 */
 	movb	$4, (%esi)
#endif
	cmpl	%edi, (%edi)
	/* if there is not a 0 at 0, the last write wrapped. Hop out */
	jne	1f
	inc	%cl
	roll	%esi
	jmp	1b
/* size is now in esi */
/* %ecx has setting for register */
1:
	rol	$20, %ecx
/* set 4 banks. */
	orb	$1, %cl
/* COMMENT 3-22-01 RGM -- I THINK WE WANT THIS HERE TO TURN
 * MULTIBANK ON
 */
	andb	$0xef, %ch
	WRITE_MCR0
	/*
	mov	$0x6c, %al
	CALLSP(pci_write_dword)
	*/
/* bank detect */
/* you don't need to even care how large CAS is. 
 * Just set 4-bank mode, and set non-zero numbers into a few places. 
 * if the byte at 0 changes, you have two banks. Trivial.
	LONGDELAY
 */
	movl	$0x1000, %esi
	movl	%edi, (%edi)
	movb	$5, (%esi)
	roll	%esi
	movb	$6, (%esi)
	roll	%esi
	movb	$7, (%esi)
	cmpl	%edi, (%edi)
	// If @0 is unchanged, then we have four banks.
	jz	1f /* 4 banks */
	// If @0 got overwritten, then we only have two banks.
	/* clear 4 banks */
	andb	$0xfe, %cl
1:
	WRITE_MCR0
/* Detect single/double side */
	movl	%ecx, %ebx
	andl	$0xe07fffff, %ecx // disable sdram
	WRITE_MCR0
	movl	%ecx, %ebp
	orl	$0x11800000, %ecx 
	WRITE_MCR0
	movl	%edi, (%edi)
	movb	$0x5a, 8(%edi)
	cmpl	%edi, (%edi)
	jnz	1f
	orl	%ecx, %ebx // double side
1:
	movl	%ebp, %ecx
	WRITE_MCR0
	movl	%ebx, %ecx 
	WRITE_MCR0

/*	jmp	sizeloop
*/
	
sizedone:
/* Next line ASSUMES that eax contains 8000000xx */
	movb	$0x7c, %al
	movb	$0xf8, %dl
	outl	%eax, %dx

	movl	$0x2428c411, %eax
	movb	$0xfc, %dl
	outl	%eax, %dx
	/* at this point, dram slot 0 is up.  we hope. */

	/* now for flash ... */
	/* NOTE: MUST NOT DESTROY %ecx! */
	/* INPUT: %al, the register. %ecx, the write data */
	/* Following code courtesy Ollie Lho:	*/
	
	
	
/*
 * ipl.S:	 Initial Program Loader (IPL) for SiS 630 and M-System DoC Millennium
 *
 *
 * Copyright 2000 Silicon Integrated Systems Corporation
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 * Reference:
 *	1. SiS 630 Specification
 *	2. System Management Bus Specification Rev 1.1
 *	3. PC SDRAM Serial Presence Detect (SPD) Specification Rev 1.2B
 *	4. Booting From the DiskOnChip Millennium, M-Systems Application Note Ap-DOC-044
 *	   APR-2000, 93-SR-001-44-7L REV. 1.0
 *
 * $Id: ipl.S,v 1.24 2001/04/18 18:43:19 rminnich Exp $
 */
#define DOC_WIN_SEG		0xfe00
#define DOC_STACK		0x04000
#define SPL_RAM		0x80000

#define DOC_SPL_START_BLK	2	/* 0,1 for IPL */
#define DOC_SPL_SIZE_IN_PAGE	126     /* 1 page = 512 bytes, total 63kB */

ipl_start:
#if 0
	/* stack and es already set. */
	/* O.K. we have DRAM now, so set up STACK for CALL/RET */
	movw	$DOC_STACK_SEG, %ax
	movw	%ax, %ss
	movw	$SPL_RAM_SEG, %ax
	movw	%ax, %es
	movl	$DOC_STACK_SEG, %esp			# set %sp
#endif
	/* NOTE: in ollie's original code, %ds was at 0xfe00, i.e. 
	 * 0xfe000, i.e. base of DoC. We have to adjust for that when 
	 * we set %edi and %esi
	 */
	movl	$DOC_STACK, %esp		# set %sp
	movl	$SPL_RAM, %edx			# start of RAM
	xorl	%ecx, %ecx			# clear %ecx
	movl	$0xfe800, %esi			# point %si to CDSN Data area
	movl	$0xff000, %edi			# point %di to CDSN Control area
	movl	$DOC_SPL_START_BLK, %ebp	# start page of LinuxBIOS

	movb	$0x84, %al			# Reset DOC Millennium
	call	doc_reset

	movb	$0x85, %al			# Turn into Normal Mode
	call	doc_reset
	
read_next_page:
	movl	$0xff000, %edi			# point %di to CDSN Control area

flash_command:
	movb	$0x03, 0x04(%edi)		# start command cycle
	movb	$0x00, (%esi)			# issue flash command Read00
	call	doc_cycle_end
	
	movw	%bp, %bx			# %bp is current page number

flash_address:
	shll	$0x08, %ebx
        movb	$0x03, %cl			# this one is DANGEROUS but I am
						# sure the upper 3 bytes == 0x00
	
	movb	$0x05, 0x04(%edi)		# start address cycle
0:
	movb	%bl, (%esi)			# write address to CDSNIO
	shrw	$0x08, %bx			# shift next address byte
	loop	0b

	call	doc_cycle_end

wait_for_flash_ready:
	/* delay by reding NOP register before polling the FLASH READY bit,
	   this is inlined to save a call/ret pair */
doc_delay:
	movb	$0x04, %cl			# this one is DANGEROUS but I am
						# sure the upper 3 bytes == 0x00
0:
	movb	0x20(%edi), %al			# read DOC NOP retisger
	loop	0b				# four times
	
	testb	$0x80, 0x04(%edi)		# is flash ready ?
	jz	wait_for_flash_ready

	movb	0x1d(%edi), %al			# init read pipeline
	movw	$0x100, %cx			# 1 page = 512 bytes
	movl	$0xfe800, %esi			# point %si to CDSN Data area
	movl	%edx, %edi			# restore saved current destination
	rep
	movsw

	movl	%edi, %edx			# save current destination
	incw	%bp				# increse current page number
	cmpw	$128, %bp			# moved 63 KB ??
	jl	read_next_page			# no, read next page

sis630ipl_end:
	# jmp to 8000:0000, where SPL
	# (LinuxBIOS) starts in RAM
#	ljmp $0x10,$0x8000
	jmp	spl_vector

doc_reset:
        /* Input: AL = value write to DOC_CONTROL register
           Clobberd: CX */
        movb    %al, 0x02(%edi)                  # write DoC Control retister
        movb    %al, 0x02(%edi)                  # twice
        ret                                     # End of doc_reset

doc_cycle_end:
        movb    $0x00, 0x1e(%edi)                # flush write pepeline
        movb    $0x01, 0x04(%edi)                # end command cycle
        ret


/* we don't need these. We only do I/O to MCR 0 */
#if 0
pci_write_dword:
	mov	$0,%ah
	mov	$0x80000000,%edx
	or	%ax,%dx
	mov	%edx,%eax
	mov	$0x0cf8,%dx
	outl	%eax,%dx
	mov	$0x0fc,%dl
	mov	%ecx, %eax
	outl	%eax,%dx
	RETSP
pci_read_dword:
	mov	$0,%ah
	mov	$0x80000000,%edx
	or	%ax,%dx
	mov	%edx,%eax
	mov	$0x0cf8,%dx
	outl	%eax,%dx
	mov	$0x0fc,%dl
	inl	%dx, %eax
	mov	%eax, %ecx
	RETSP
#endif

	.org 0xfe1f0
reset_vector:
	.byte	0xea				# jmp to fe00:0000, where IPL
	.word	0x0000, DOC_WIN_SEG		# starts in DoC
.code32
spl_vector:
	invd
	jmpl	$0x10, $0x80000
# jmpl	$0x80000
# put this here to buy some room!
pad: 	.byte 0,0

