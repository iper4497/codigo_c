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
/* Copyright 2000, Ron Minnich, Advanced Computing Lab, LANL
 * rminnich@lanl.gov
 */


/** Start code to put an i386 or later processor into 32-bit 
 * protected mode. 
 */
/*
 * $Id: start32.inc,v 1.3 2001/03/23 21:03:12 ebiederm Exp $
 *
 */

/* if you are entering crt0.S in protected mode, then define this option */
#ifndef PROTECTED_MODE_STARTUP
/* this jump is stupid, but mkrom wants to only jump to 0xffff:0 */
.code16
.globl	EXT(_start)
.type _start, @function
/// We have a relative jump here to around the GDT. 
_start: jmp _realstart
/** This gdt has a 4 Gb code segment at 0x10, and a 4 GB data segment
 * at 0x18; these are Linux-compatible. 
 */

/** GDT. we have modified this from the original freebios to make it 
 * compatible with linux. This puts text at seg 0x10 and data at 0x18
 */
gdtptr:
	.word	4*8-1
	.long	gdt		/* we know the offset */
gdt:
	.word	0x0000, 0x0000		/* dummy */
	.byte	0x0, 0x0, 0x0, 0x0
	
	.word	0x0000, 0x0000		/* dummy */
	.byte	0x0, 0x0, 0x0, 0x0
	
	.word	0xffff, 0x0000		/* flat code segment */
	.byte	0x0, 0x9a, 0xcf, 0x0	
	
	.word	0xffff, 0x0000		/* flat data segment */
	.byte	0x0, 0x92, 0xcf, 0x0	

	.word	0x0000, 0x0000		/* kernel tss */
	.byte	0x0, 0x0, 0x0, 0x0


_realstart:
	cli
	movw	%cs, %ax
	movw	%ax, %es

/* thanks to kmliu@sis.tw.com for this TBL fix ... */
/**/
/* IMMEDIATELY invalidate the translation lookaside buffer before executing*/
/* any further code.  Even though paging is we, disabled could still get*/
/*false address translations due to the TLB if we didn't invalidate it.*/
/**/
	xor	%eax, %eax
	mov	%eax, %cr3    /* Invalidate TLB*/

	/* Note: gas handles memory addresses in 16 bit code very poorly.
	 * In particular it doesn't appear to have a directive allowing you
	 * associate a section or even an absolute offset with a segment register.
	 *
	 * This means that anything except cs:ip relative offsets are
	 * a real pain in 16 bit mode.  And explains why it is almost
	 * imposible to get gas to do lgdt correctly.
	 */
	/** load gdt */
	/* need a prefix on this for some stupid as reason. */
	/* I'm really sick of this !$#@ assembly language, and trying
	 * to get @!#$ as to not screw it up. 
	 * hand assembled ...
	lgdt	gdtptr
	 */
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

#if 0
	/* sigh ... more as woes ... */
	/* this USED to work find on most 'as' versions, but is 
	 * fouled up on some. 
	 */
	/* on some versions of ld we get idiot error messages. 
	 * as far as I'm concerned this code is write once, read forever, 
	 * so my concern on this score is quite limited. 
	 * hand assembly until gas/gld get fixed. 
	 */
	.byte 0x66
	.byte 0xea, 0x07, 0x00 /* ljmp */
	.word 0x58
	.word 0x10 
#else
#ifdef BROKEN_GAS
	data32 
	ljmp	$0x10, $.Lprotected
#else
	data32	ljmp	$0x10, $.Lprotected
#endif
#endif

/*
 *	When we come here we are in protected mode. We expand 
 *	the stack and copies the data segment from ROM to the
 *	memory.
 *
 *	After that, we call the chipset bootstrap routine that
 *	does what is left of the chipset initialization. 
 *
 *	NOTE aligned to 4 so that we are sure that the prefetch
 *	cache will be reloaded.
 */
	.align	4
.Lprotected:
	.code32
	intel_chip_post_macro(0x10)	/* post 10 */

	movw	$0x18, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %ss
#ifdef i586
	movw	%ax, %fs
	movw	%ax, %gs
#endif

#else /* PROTECTED_MODE_STARTUP */
	.align	4
	.code32
	intel_chip_post_macro(0x10)	/* post 10 */
#endif /* ifndef PROTECTED_MODE_START */
