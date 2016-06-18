#ifndef UNIFORM_BOOT_H
#define UNIFORM_BOOT_H


/* The uniform boot environment information is restricted to 
 * hardware information.  In particular for a simple enough machine
 * all of the environment information should be able to reside in
 * a rom and not need to be moved.  This information is the
 * information a trivial boot room can pass to linux to let it
 * run the hardware.  
 *
 * Also all of the information should be Position Independent Data.  
 * That is it should be safe to relocated any of the information
 * without it's meaning/correctnes changing. The exception is the
 * uniform_boot_header with it's two pointers arg & env.
 * 
 * The addresses in the arg & env pointers must be physical
 * addresses. A physical address is an address you put in the page
 * table. 
 *
 * The Command line is for user policy.  Things like the default
 * root device.
 *
 * The env is for unprobeable hardware details.  Modern hardware
 * is very probeable, especially when tables from the motherboard
 * ROM's are used as a source of information, so env is not expected
 * to contain much information.  Making calls to a BIOS, or console
 * firmware should never be necessary.  The goal is for this to be
 * an interface that places no backward compatibility constraints
 * upon implementors.  Leading to the expectation that on newer
 * hardware that supports probing for something previously unprobeable
 * that information will not be present in the environment.
 *
 */

#include <stdint.h>
#include <arch/boot/boot.h>

#ifdef UBE32
typedef uint32_t UBE_MWord; /* A machine word */
typedef uint32_t UBE_MAddr; /* A machine address */
#endif
#ifdef UBE64
typedef uint64_t UBE_MWord; /* A machine word */
typedef uint64_t UBE_MAddr; /* A machine address */
#endif
typedef uint64_t UBE_Addr;   /* An arbitrary physical address */
typedef uint64_t UBE_Length; /* An arbitrary length */
typedef uint8_t UBE_char;    /* An arbitrary character */

struct uniform_boot_header
{
	UBE_MWord header_bytes;
	UBE_MWord header_checksum;
	UBE_MAddr arg;
	UBE_MWord arg_bytes;
	UBE_MAddr env;
	UBE_MWord env_bytes;
};

/* Every entry in the boot enviroment list will correspond to a boot
 * info record.  Encoding both type and size.  The type is obviously
 * so you can tell what it is.  The size allows you to skip that
 * boot enviroment record if you don't know what it easy.  This allows
 * forward compatibility with records not yet defined.
 */
struct ube_record {
	UBE_MWord tag;		/* tag ID */
	UBE_MWord size;		/* size of record (in bytes) */
};


#define UBE_TAG_MEMORY	0x0001

struct ube_memory_range {
	UBE_Addr start;
	UBE_Length size;
	UBE_MWord type;
#define UBE_MEM_RAM      1
#define UBE_MEM_RESERVED 2
#define UBE_MEM_ACPI     3
#define UBE_MEM_NVS      4
	
};

struct ube_memory {
	UBE_MWord tag;
	UBE_MWord size;
	struct ube_memory_range map[0];
};

#define UBE_TAG_HWRPB	0x0002
struct ube_hwrpb {
	UBE_MWord tag;
	UBE_MWord size;
	UBE_Addr hwrpb;
};

extern void *get_ube_pointer(unsigned long totalram);
extern unsigned long uniform_boot_compute_header_checksum(
	struct uniform_boot_header *header);

/* Maximum bytes needed to read the elf header and the program header */
#define ELF_HEAD_SIZE 512

#endif /* UNIFORM_BOOT_H */
