/* This setup assumes you have at least a 64M machine 
 * The problem is that with 2GB of RAM you use nearly 23M
 * of memory on the kernel, and since 2.2 doesn't check 
 * where the initrd is placed before allocating memory this is a
 * problem.   With a 8M Ramdisk + 23M kernel that is 31M leaving
 * little room for things to grow.
 * With the limit at 64M (i.e. 0x4000000) we should be o.k.)
 * 
 * If you need to change the amount of assumed memory.
 * The uppper.LENGTH needs to change so that 
 * upper.LENGTH + upper.ORIGIN = MEMORY_SIZE
 * and the computation just before ramdisk placing also should
 * be corrected, to be:
 * . = MEMORY_SIZE - ((ramdisk_data_end - ramdisk_data) + 4095)
 * .ramdisk(ALIGN(4096))
 */

OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
MEMORY
{
	/* 0x10000 - 0x90000 A good safe area in low memory I can use */
	low (rwx)    : ORIGIN = 0x010000, LENGTH = 0x0080000
	middle (rwx) : ORIGIN = 0x091000, LENGTH = 0x0001000
	upper (rwx)  : ORIGIN = 0x100000, LENGTH = (0x4000000 - 0x100000)
}
ENTRY(startup_32)
SECTIONS
{
  . = 0x10000 ;
  _text = .;			/* Text and read-only data */
  .text (.): {
	*(.text)
	*(.fixup)
	*(.gnu.warning)
	} > low  = 0x9090
  .rodata (.): { *(.rodata) } > low 
  .kstrtab (.): { *(.kstrtab) } > low 

  . = ALIGN(16);		/* Exception table */
  _etext = .;			/* End of text section */

  .data (.): {			/* Data */
	*(.data)
	CONSTRUCTORS
	ramdisk_len = . ;
	LONG(ramdisk_data_end - ramdisk_data)
	} > low 

  _edata = .;			/* End of data section */
  __bss_start = .;		/* BSS */
  .bss (.): {
	*(.bss)
	} > low 
  _end = . ;


  . = 0x91000 ;
  .nokill (.): {
    *(.nokill)
  } > middle 

  . = 0x100000 ;
  .kernel (.): { 
    *(.kernel)
  } > upper 
  . = 0x4000000 - ((ramdisk_data_end - ramdisk_data) + 4095);
  .ramdisk (ALIGN(4096)) : {
    *(.ramdisk)
  }  > upper 
  /* Stabs debugging sections.  */
  .stab 0 : { *(.stab) }
  .stabstr 0 : { *(.stabstr) }
  .stab.excl 0 : { *(.stab.excl) }
  .stab.exclstr 0 : { *(.stab.exclstr) }
  .stab.index 0 : { *(.stab.index) }
  .stab.indexstr 0 : { *(.stab.indexstr) }
  .comment 0 : { *(.comment) }
}
