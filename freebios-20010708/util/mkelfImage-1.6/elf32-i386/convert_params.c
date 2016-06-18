#include <sys/io.h>
#include "uniform_boot.h"
#define STACK_SIZE (4096)

long user_stack [STACK_SIZE];

unsigned long * stack_start = & user_stack[STACK_SIZE];

struct drive_info_struct { char dummy[32]; };
struct sys_desc_table_struct {
	unsigned short length;
	unsigned char table[0];
};

/*
 * This is set up by the setup-routine at boot-time
 */
#define PARAM   (real_mode)
#define SCREEN_INFO (*(struct screen_info *)(PARAM+0))
#define EXT_MEM_K   (*(unsigned short *)(PARAM + 0x2))
#define ALT_MEM_K   (*(unsigned long *)(PARAM + 0x1e0))
#define E820_MAP_NR (*(char*) (PARAM+E820NR))
#define E820_MAP    ((struct e820entry *) (PARAM+E820MAP))
#define APM_BIOS_INFO (*(struct apm_bios_info *) (PARAM+0x40))
#define DRIVE_INFO (*(struct drive_info_struct *) (PARAM+0x80))
#define SYS_DESC_TABLE (*(struct sys_desc_table_struct*)(PARAM+0xa0))
#define MOUNT_ROOT_RDONLY (*(unsigned short *) (PARAM+0x1F2))
#define RAMDISK_FLAGS (*(unsigned short *) (PARAM+0x1F8))
#define ORIG_ROOT_DEV (*(unsigned short *) (PARAM+0x1FC))
#define AUX_DEVICE_INFO (*(unsigned char *) (PARAM+0x1FF))
#define PARAM_BLOCK_SIGNATURE ((unsigned char *) (PARAM + 0x202))
#define PARAM_BLOCK_VERSION (*(unsigned short *) (PARAM + 0x206))
#define LOADER_TYPE (*(unsigned char *) (PARAM+0x210))
#define LOADER_FLAGS  (*(unsigned char *) (PARAM+0x211))
#define KERNEL_START (*(unsigned long *) (PARAM+0x214))
#define INITRD_START (*(unsigned long *) (PARAM+0x218))
#define INITRD_SIZE (*(unsigned long *) (PARAM+0x21c))
#define COMMAND_LINE ((char *) (PARAM+2048))
#define COMMAND_LINE_SIZE 256
#define COMMAND_LINE_END (((char *) (PARAM+2048)) + COMMAND_LINE_SIZE)

#define CL_MAGIC (*(unsigned short *)(PARAM+0x20))
#define CL_MAGIC_VALUE 0xA33F
#define CL_OFFSET (*(unsigned short *)(PARAM+0x22))

#define RAMDISK_IMAGE_START_MASK  	0x07FF
#define RAMDISK_PROMPT_FLAG		0x8000
#define RAMDISK_LOAD_FLAG		0x4000	

#define LOADER_TYPE_LOADLIN         1
#define LOADER_TYPE_BOOTSECT_LOADER 2
#define LOADER_TYPE_SYSLINUX        3
#define LOADER_TYPE_ETHERBOOT       4
#define LOADER_TYPE_KERNEL          5


#define PAGE_SIZE 4096


#define E820MAP	0x2d0		/* our map */
#define E820MAX	32		/* number of entries in E820MAP */
#define E820NR	0x1e8		/* # entries in E820MAP */

#define E820_RAM	1
#define E820_RESERVED	2
#define E820_ACPI	3 /* usable as RAM once ACPI tables have been read */
#define E820_NVS	4


struct e820map {
    int nr_map;
    struct e820entry {
	unsigned long long addr;	/* start of memory segment */
	unsigned long long size;	/* size of memory segment */
	unsigned long type;		/* type of memory segment */
    } map[E820MAX];
};

/*
 * These are set up by the setup-routine at boot-time:
 */

struct screen_info {
	unsigned char  orig_x;			/* 0x00 */
	unsigned char  orig_y;			/* 0x01 */
	unsigned short dontuse1;		/* 0x02 -- EXT_MEM_K sits here */
	unsigned short orig_video_page;		/* 0x04 */
	unsigned char  orig_video_mode;		/* 0x06 */
	unsigned char  orig_video_cols;		/* 0x07 */
	unsigned short unused2;			/* 0x08 */
	unsigned short orig_video_ega_bx;	/* 0x0a */
	unsigned short unused3;			/* 0x0c */
	unsigned char  orig_video_lines;	/* 0x0e */
	unsigned char  orig_video_isVGA;	/* 0x0f */
	unsigned short orig_video_points;	/* 0x10 */

	/* VESA graphic mode -- linear frame buffer */
	unsigned short lfb_width;		/* 0x12 */
	unsigned short lfb_height;		/* 0x14 */
	unsigned short lfb_depth;		/* 0x16 */
	unsigned long  lfb_base;		/* 0x18 */
	unsigned long  lfb_size;		/* 0x1c */
	unsigned short dontuse2, dontuse3;	/* 0x20 -- CL_MAGIC and CL_OFFSET here */
	unsigned short lfb_linelength;		/* 0x24 */
	unsigned char  red_size;		/* 0x26 */
	unsigned char  red_pos;			/* 0x27 */
	unsigned char  green_size;		/* 0x28 */
	unsigned char  green_pos;		/* 0x29 */
	unsigned char  blue_size;		/* 0x2a */
	unsigned char  blue_pos;		/* 0x2b */
	unsigned char  rsvd_size;		/* 0x2c */
	unsigned char  rsvd_pos;		/* 0x2d */
	unsigned short vesapm_seg;		/* 0x2e */
	unsigned short vesapm_off;		/* 0x30 */
	unsigned short pages;			/* 0x32 */
						/* 0x34 -- 0x3f reserved for future expansion */
};


#undef memset
#undef memcpy
#define memzero(s, n)     memset ((s), 0, (n))

typedef unsigned size_t;


static const char default_command_line[] = DEFAULT_COMMAND_LINE ;

/* FIXME handle systems with large EBDA's */
static unsigned char *faked_real_mode = (void *)0x90000;
extern unsigned int ramdisk_data, ramdisk_len;

/* Base Address */
#define TTYS0 0x3f8
#define TTYS0_LSR (TTYS0+0x05)
#define TTYS0_TBR (TTYS0+0x00)

static void ttys0_tx_byte(unsigned byte)
{
	while((inb(TTYS0_LSR) & 0x20) == 0)
		;
	outb(byte, TTYS0_TBR);
}
static void put_char_serial(int c)
{
	if (c == '\n') {
		ttys0_tx_byte('\r');
	}
	ttys0_tx_byte(c);
}

static void put_char(int c)
{
#if 0
	put_char_video(c);
#endif
	put_char_serial(c);
}

static void puts(const char *str)
{
	int c;
	while(c = *str++) {
		put_char(c);
	}
}

static void put_hex(unsigned x)
{
	static const char digit[] = "0123456789ABCDEF";
	int i;
	puts("0x");
	for(i = 28; i >= 0; i -= 4) {
		put_char(digit[(x >> i) & 0xf]);
	}
}

static void put_lhex(unsigned long long x)
{
	static const char digit[] = "0123456789ABCDEF";
	int i;
	puts("0x");
	for(i = 28 + 32; i >= 0; i -= 4) {
		put_char(digit[(x >> i) & 0xf]);
	}
}

size_t strnlen(const char *s, size_t max)
{
	size_t len = 0;
	while(len < max && *s) {
		len++;
		s++;
	}
	return len;
}

void* memset(void* s, int c, size_t n)
{
	int i;
	char *ss = (char*)s;

	for (i=0;i<n;i++) ss[i] = c;
	return s;
}

void* memcpy(void* __dest, const void* __src,
			    size_t __n)
{
	int i;
	char *d = (char *)__dest, *s = (char *)__src;

	for (i=0;i<__n;i++) d[i] = s[i];
	return __dest;
}


/* Helper functions */
void append_command_line(unsigned char *real_mode, char *arg, int arg_bytes)
{
	int len, max;
	char *dest;
	/* skip over what has already been set */
	len = strnlen(COMMAND_LINE, COMMAND_LINE_END - COMMAND_LINE);
	dest = COMMAND_LINE + len;
	max = (COMMAND_LINE_END - dest) - 1;
	if (max < 1) {
		/* No room to append anything :( */
		return;
	}
	/* Add a space in between strings */
	*dest++ = ' ';
	/* Append the added command line */
	max = (COMMAND_LINE_END - dest) - 1;
	if (max > arg_bytes) {
		max = arg_bytes;
	}
	len = strnlen(arg, max);
	memcpy(dest, arg, len);
	/* Null terminate the string */
	*dest++ = '\0';
}

static void set_memsize_k(unsigned char *real_mode, unsigned long mem_k) 
{
	/* ALT_MEM_K maxes out at 4GB */
	if (mem_k > 0x3fffff) {
		mem_k = 0x3fffff;
	}
	if (mem_k > (ALT_MEM_K + (1 << 10))) {
		/* Use our memory size less 1M */
		ALT_MEM_K = mem_k - (1 << 10);
		EXT_MEM_K = mem_k - (1 << 10);
		if ((mem_k - (1 << 10)) > 0xFFFF) {
			EXT_MEM_K = 0xFC00; /* 64 M */
		}
	}
}

static void add_e820_map(unsigned char *real_mode,
	unsigned long long addr, unsigned long long size, 
	unsigned long type)
{
	unsigned long long high;
	unsigned long mem_k;
	int i;
	i = E820_MAP_NR;
	if (i < E820MAX) {
		E820_MAP[i].addr = addr;
		E820_MAP[i].size = size;
		E820_MAP[i].type = type;
		E820_MAP_NR++;
	}
	/* policy I assume that for the legacy memory
	 * variables memory is contiguous.
	 */
	if (type == E820_RAM) {
		high = addr + size;
		if (high >= 0x40000000000ULL) {
			mem_k = 0xFFFFFFFF;
		} else {
			mem_k = high >> 10;
		}
		set_memsize_k(real_mode, mem_k);
	}
}

/* Multiboot Specification */
struct multiboot_mods {
	unsigned mod_start;
	unsigned mod_end;
	unsigned char *string;
	unsigned reserved;
};

struct memory_segment {
	unsigned long long addr;
	unsigned long long size;
	unsigned type;
};

struct multiboot_info {
        unsigned flags;
#define MULTIBOOT_MEM_VALID       0x01
#define MULTIBOOT_BOOT_DEV_VALID  0x02
#define MULTIBOOT_CMDLINE_VALID   0x04
#define MULTIBOOT_MODS_VALID      0x08
#define MULTIBOOT_AOUT_SYMS_VALID 0x10
#define MULTIBOOT_ELF_SYMS_VALID  0x20
#define MULTIBOOT_MMAP_VALID      0x40
	unsigned mem_lower;
	unsigned mem_upper;
	unsigned char boot_device[4];
	void *command_line;
	unsigned mods_count;
	struct multiboot_mods *mods_addr;
	unsigned syms_num;
	unsigned syms_size;
	unsigned syms_addr;
	unsigned syms_shndx;
	unsigned mmap_length;
	struct memory_segment *mmap_addr;
};

#define MULTIBOOT_MAX_COMMAND_LINE 0xFFFFFFFF

static void convert_multiboot_memmap(
	unsigned char *real_mode,
	struct multiboot_info *info)
{
	unsigned size;
	unsigned *size_addr;
	int i;
#define next_seg(seg, size) ((struct memory_segment *)((char *)(seg) + (size)))
	struct memory_segment *seg, *end;
	seg = info->mmap_addr;
	end  = (void *)(((char *)seg) + info->mmap_length);
	size_addr = (unsigned *)(((char *)seg) - 4);
	size = *size_addr;
	for(seg = info->mmap_addr; (seg < end); seg = next_seg(seg,size)) {
		add_e820_map(real_mode, seg->addr, seg->size, seg->type);
	}
#undef next_seg
	
}

static void convert_multiboot(
	unsigned char *real_mode, struct multiboot_info *info)
{
	if (info->flags & MULTIBOOT_MEM_VALID) {
		/* info->memory is short 1M */
		set_memsize_k(real_mode, info->mem_upper + (1 << 10));
	}
	if (info->flags & MULTIBOOT_CMDLINE_VALID) {
		append_command_line(real_mode, info->command_line, 
			MULTIBOOT_MAX_COMMAND_LINE);
	}
	if (info->flags & MULTIBOOT_MMAP_VALID) {
		convert_multiboot_memmap(real_mode, info);
	}
}
/* Uniform Boot Environment */

#define UBE_MEM_DEBUG 1
#if UBE_MEM_DEBUG
#define ube_puts(x) puts(x)
#define ube_put_hex(x) put_lhex(x)
#define ube_put_lhex(x) put_lhex(x)
#else
#define ube_puts(x)
#define ube_put_hex(x)
#define ube_put_lhex(x)
#endif /* UBE_MEM_DEBUG */
static void convert_uniform_boot_memory(
	unsigned char *real_mode, struct ube_memory *mem)
{
	int i;
	int entries;
	unsigned long mem_k;
	mem_k = 0;
	entries = (mem->size - sizeof(*mem))/sizeof(mem->map[0]);
	for(i = 0; (i < entries) && (i < E820MAX); i++) {
		unsigned long type;
		ube_puts("ube-mem: "); 
		ube_put_lhex(mem->map[i].size); 
		ube_puts(" @ ");
		ube_put_lhex(mem->map[i].start);
		ube_puts(" (");
		switch(mem->map[i].type) {
		case UBE_MEM_RAM:
			type = E820_RAM;
			ube_puts("ram");
			break;
		case UBE_MEM_ACPI:
			type = E820_ACPI;
			ube_puts("ACPI data");
			break;
		case UBE_MEM_NVS:
			type = E820_NVS;
			ube_puts("ACPI NVS");
			break;
		case UBE_MEM_RESERVED:
		default:
			type = E820_RESERVED;
			ube_puts("reserved");
			break;
		}
		ube_puts(")\n");
		add_e820_map(real_mode, 
			mem->map[i].start, mem->map[i].size, type);
	}
}

static unsigned long uniform_boot_compute_header_checksum(
	struct uniform_boot_header *header)
{
	unsigned short *ptr;
	unsigned long sum;
	unsigned long len;
	/* compute an ip style checksum on the header */
	sum = 0;
	len = header->header_bytes >> 1;
	ptr = (void *)header;
	while (len--) {
		sum += *(ptr++);
		if (sum > 0xFFFF)
			sum -= 0xFFFF;
	}
	return (~sum) & 0xFFFF;
}

static void convert_uniform_boot(unsigned char *real_mode, 
	struct uniform_boot_header *header)
{
	/* Uniform boot environment */
	unsigned long env_bytes;
	char *env;
	unsigned long checksum;
	checksum = uniform_boot_compute_header_checksum(header);
	if (checksum != 0) {
		puts("Bad uniform boot header checksum!\n");
	}
	if (header->arg_bytes) {
		append_command_line(real_mode, (void *)(header->arg), header->arg_bytes);
	}
	env = (void *)(header->env);
	env_bytes = header->env_bytes;
	while(env_bytes) {
		struct ube_record *record;
		unsigned long mem_k;
		record = (void *)env;
		if (record->tag == UBE_TAG_MEMORY) {
			convert_uniform_boot_memory(real_mode, (void *)record);
		}
		env += record->size;
		env_bytes -= record->size;
	}
}

void initialize_linux_params(unsigned char *real_mode)
{
	int len;
	/* First the defaults */
	memset(real_mode, 0, PAGE_SIZE);
	
	/* Default screen size */
	SCREEN_INFO.orig_video_cols = 80;
	SCREEN_INFO.orig_video_lines = 25;
	
	/* Fill this in later */
	EXT_MEM_K = 0;
		
	/* Fill in later... */
	E820_MAP_NR = 0;

	/* Where I'm putting the command line */
	CL_MAGIC = CL_MAGIC_VALUE;
	CL_OFFSET = 2048;
	
	/* Now set the command line */
	len = strnlen(default_command_line, COMMAND_LINE_SIZE -1);
	memcpy(COMMAND_LINE, default_command_line, len);
	COMMAND_LINE[len] = '\0';

#if 0		
	/* from the bios initially */
	memset(&APM_BIOS_INFO, 0, sizeof(APM_BIOS_INFO));
	
	memset(&DRIVE_INFO, 0, sizeof(DRIVE_INFO));
#endif

	/* forget it for now... */
	SYS_DESC_TABLE.length = 0; 
	
	/* Fill this in later */
	ALT_MEM_K = 0;
	EXT_MEM_K = 0;
		
	/* default yes: this can be overridden on the command line */
	MOUNT_ROOT_RDONLY = 0xFFFF;
	
	/* old ramdisk options, These really should be command line
	 * things...
	 */
	RAMDISK_FLAGS = 0; 
#if defined(DEFAULT_RAMDISK_IMAGE_START)
	RAMDISK_FLAGS |= (DEFAULT_RAMDISK_IMAGE_START & RAMDISK_IMAGE_START_MASK)
#endif
#if defined(DEFAULT_RAMDISK_PROMPT_FLAG)
	RAMDISK_FLAGS |= RAMDISK_PROMPT_FLAG
#endif
#if defined(DEFAULT_RAMDISK_LOAD_FLAG)
	RAMDISK_FLAGS |= RAMDISK_LOAD_FLAG
#endif

	/* default to /dev/hda.
	 * Override this on the command line if necessary 
	 */
	ORIG_ROOT_DEV = DEFAULT_ROOT_DEV;
	
	/* Originally from the bios? */
	AUX_DEVICE_INFO = 0;
	
	/* Boot block magic */
	memcpy(PARAM_BLOCK_SIGNATURE, "HdrS", 4);
	PARAM_BLOCK_VERSION = 0x0201;
	
	/* Say I'm a kernel boot loader */
	LOADER_TYPE = (LOADER_TYPE_KERNEL << 4) + 0 /* version */;
	
	/* No loader flags */
	LOADER_FLAGS = 0;
	
	/* Ramdisk address and size ... */
	INITRD_START = 0;
	INITRD_SIZE = 0;
	if (ramdisk_len) {
		INITRD_START = (unsigned)&ramdisk_data;
		INITRD_SIZE = ramdisk_len;
	}	
}

void *convert_params(unsigned type, void *data)
{
	unsigned char *real_mode; /* Pointer to real-mode data */
	size_t len;

	if (type == 0) {
		/* Native linux mode... */
		return data;
	}
	real_mode = faked_real_mode;

	initialize_linux_params(real_mode);

	if (type == 0x2BADB002) {
		/* Original multiboot specification */
		convert_multiboot(real_mode, data);
	}
	else if (type == 0x0A11B007) {
		/* new Uniform boot proposal */
		convert_uniform_boot(real_mode, data);
	}
	puts("EXT_MEM_K="); put_hex(EXT_MEM_K); puts("\n");
	puts("ALT_MEM_K="); put_hex(ALT_MEM_K); puts("\n");
	return real_mode;
}

