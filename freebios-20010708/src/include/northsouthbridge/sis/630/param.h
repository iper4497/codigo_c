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
/* These defines were suggested by SiS.  */

#ifndef SIS630_H
#define SIS630_H
#define BIT0 $0x1
#define BIT1 $0x2
#define BIT2 $0x4
#define BIT3 $0x8
#define BIT4 $0x10
#define BIT5 $0x20
#define BIT6 $0x40
#define BIT7 $0x80
#define BIT8 $0x100
#define BIT9 $0x200
#define BIT10 $0x400
#define BIT11 $0x800
#define BIT12 $0x1000
#define BIT13 $0x2000
#define BIT14 $0x4000
#define BIT15 $0x8000


/* SIS630 PCI register*/
#define SIS_PCMC_REG04 $0x4
#define SIS_PCMC_REG05 $0x5
#define SIS_PCMC_REG06 $0x6
#define SIS_PCMC_REG07 $0x7
#define SIS_PCMC_REG0C $0xC
#define SIS_PCMC_REG0D $0xD
#define SIS_PCMC_REG0E $0xE
#define SIS_PCMC_REG0F $0xF

#define SIS_PCMC_REG50 $0x50 /* Host Bus Interface Control I*/
#define SIS_PCMC_REG51 $0x51 /* Host Bus Interface Control II*/
#define SIS_PCMC_REG52 $0x52 /* DRAM MISC Control 1*/
#define REFRESH_ENABLE BIT0

#define SIS_PCMC_REG53 $0x53 /* DRAM MISC Control 2*/
#define SIS_PCMC_REG54 $0x54 /* DRAM Timing Control 1*/
#define SIS_PCMC_REG55 $0x55 /* DRAM Timing Control 2*/
#define SIS_PCMC_REG56 $0x56 /* DRAM MISC Control 3*/
#define SDRAM_ONE_PAGE BIT2
#define SIS_PCMC_REG57 $0x57  /* SDRAM/VCM Initialization Control*/
#define CAS_LATENCY_3T BIT0
#define PRECHARGE      BIT7
#define MRS            BIT6
#define REFRESH        BIT5
#define NOP            BIT2

#define SIS_PCMC_REG58 $0x58 /* Memory Buffer Pre-driver Slew Rating*/
#define SIS_PCMC_REG59 $0x59 /* Memory Buffer Strength and Current Rating*/
#define SIS_PCMC_REG5A $0x5A /* Memory Buffer Strength and Current Rating*/
#define SIS_PCMC_REG5B $0x5B /* PCI Buffer Strength and Current Rating*/
#define SIS_PCMC_REG5C $0x5C
#define SIS_PCMC_REG5D $0x5D
#define SIS_PCMC_REG5E $0x5E
#define SIS_PCMC_REG5F $0x5F

#define SIS_PCMC_REG60 $0x60 /* DRAM 1 Type Register*/
#define DRAM_TYPE_MASK BIT7
#define DRAM_TYPE_VCM  BIT7
#define DRAM_DOUBLESIDE BIT5
#define DRAM_MAXSIZE $0x2f

#define SIS_PCMC_REG61 $0x61 /* DRAM 2 Type Register*/
#define SIS_PCMC_REG62 $0x62 /* DRAM 3 Type Register*/
#define SIS_PCMC_REG63 $0x63 /* DRAM Status Register*/
#define DIMM0_ENABLE BIT0
#define DIMM1_ENABLE BIT1
#define DIMM2_ENABLE BIT2
#define SIS_PCMC_REG64 $0x64 /* Frame Buffer Cache (FBC) Control Register*/
#define FORCE_TWO_BANKS BIT6
#define GUI_128_MODE BIT5
	
#define SIS_PCMC_REG65 $0x65
#define SIS_PCMC_REG66 $0x66
#define SIS_PCMC_REG67 $0x67
					/*; Power Management*/
#define SIS_PCMC_REG68 $0x68 /* ACPI I/O Space Base Address Register*/
#define SIS_PCMC_REG69 $0x69 /* ACPI I/O Space Base Address Register*/
#define SIS_PCMC_REG6A $0x6A /* SMRAM Access Control Register*/
#define SMRAM_ACCESS_CONTROL BIT4 /* Access SMRAM in non-SMM*/

#define SIS_PCMC_REG6B $0x6B /* Self Refresh Command Output Timing Control*/
#define SIS_PCMC_REG6C $0x6C /* DRAM Self-Refresh Control for Power Management*/
#define SIS_PCMC_REG6D $0x6D
#define SIS_PCMC_REG6E $0x6E
#define SIS_PCMC_REG6F $0x6F

					/*; Shadow RAM Area*/
#define SIS_PCMC_REG70 $0x70 /* Shadow RAM Read Attribute Control*/
#define SIS_PCMC_REG71 $0x71 /* Shadow RAM Read Attribute Control*/
#define SIS_PCMC_REG72 $0x72 /* Shadow RAM Write Attribute Control*/
#define SIS_PCMC_REG73 $0x73 /* Shadow RAM Write Attribute Control*/
#define SIS_PCMC_REG74 $0x74
#define SIS_PCMC_REG75 $0x75
#define SIS_PCMC_REG76 $0x76
					/*; PCI Hole Area*/
#define SIS_PCMC_REG77 $0x77 /* Characteristics of PCI-Hole Area*/
#define SIS_PCMC_REG78 $0x78 /* Allocation of PCI-Hole Area #1*/
#define SIS_PCMC_REG79 $0x79 /* Allocation of PCI-Hole Area #1*/
#define SIS_PCMC_REG7A $0x7A /* Allocation of PCI-Hole Area #2*/
#define SIS_PCMC_REG7B $0x7B /* Allocation of PCI-Hole Area #2*/
#define SIS_PCMC_REG7C $0x7C /* VGA-Bridge Hardware-Trap Control*/
#define SIS_PCMC_REG7D $0x7D /* South-Bridge Hardware-Trap Control*/
#define SIS_PCMC_REG7E $0x7E /* North-Bridge Hardware-Trap Control*/
#define SIS_PCMC_REG7F $0x7F /* North-Bridge Hardware-Trap Control II*/

#define SIS_PCMC_REG80 $0x80 /* Target Bridge to DRAM Characteristics*/
#define SIS_PCMC_REG81 $0x81 /* PCI Discard Timer for Delay Transaction*/
#define SIS_PCMC_REG82 $0x82 /* Target Bridge Characteristics*/
#define SIS_PCMC_REG83 $0x83 /* CPU to PCI Characteristics and Arbitration Option*/
#define SIS_PCMC_REG84 $0x84 /* PCI Grant Timer*/
#define SIS_PCMC_REG85 $0x85 /* PCI Grant Timer*/
#define SIS_PCMC_REG86 $0x86 /* CPU Idle Timer*/
#define SIS_PCMC_REG87 $0x87 /* Host Bridge & PCI Master Priority Timer*/
#define SIS_PCMC_REG88 $0x88 /* PCI Discard Timer for PCI Hold*/
#define SIS_PCMC_REG89 $0x89 /* PCI Discard Timer for PCI Hold*/
#define SIS_PCMC_REG8A $0x8A
#define SIS_PCMC_REG8B $0x8B
#define SIS_PCMC_REG8C $0x8C /* SDRCLK/SDWCLK Control Register*/
#define SIS_PCMC_REG8D $0x8D /* SDWCLK Control Register*/
#define SIS_PCMC_REG8E $0x8E /* CPU Clock & SDRAM Clock Relationship*/
#define SIS_PCMC_REG8F $0x8F /* FBCRCLK/FBCWCLK Control Register*/

					/*; AGP GART and Page Table Control */
#define SIS_PCMC_REG90 $0x90 /* GART Base Address for Re-mapping*/
#define SIS_PCMC_REG91 $0x91 /**/
#define SIS_PCMC_REG92 $0x92
#define SIS_PCMC_REG93 $0x93
#define SIS_PCMC_REG94 $0x94 /* Graphic Window Control*/
#define SIS_PCMC_REG95 $0x95
#define SIS_PCMC_REG96 $0x96
#define SIS_PCMC_REG97 $0x97 /* Page Table Cache Control*/
#define SIS_PCMC_REG98 $0x98 /* Page Table Cache invaildation Control*/
#define SIS_PCMC_REG99 $0x99
#define SIS_PCMC_REG9A $0x9A
#define SIS_PCMC_REG9B $0x9B
#define SIS_PCMC_REG9C $0x9C /* Integrated VGA Control*/
#define SIS_PCMC_REG9D $0x9D
#define SIS_PCMC_REG9E $0x9E
#define SIS_PCMC_REG9F $0x9F
					
					/*; DRAM Priority Timer Control Register*/
#define SIS_PCMC_REGA0 $0xA0 /* CPU/PCI-GUI Privilege*/
#define SIS_PCMC_REGA1 $0xA1 /**/
#define SIS_PCMC_REGA2 $0xA2 /* CPU/PCI-GUI Privilege Timer Control*/
#define SIS_PCMC_REGA3 $0xA3 /* Gui Grant Timer*/
#define SIS_PCMC_REGA4 $0xA4
#define SIS_PCMC_REGA5 $0xA5
#define SIS_PCMC_REGA6 $0xA6
#define SIS_PCMC_REGA7 $0xA7
#define SIS_PCMC_REGA8 $0xA8
#define SIS_PCMC_REGA9 $0xA9
#define SIS_PCMC_REGAA $0xAA
#define SIS_PCMC_REGAB $0xAB
#define SIS_PCMC_REGAC $0xAC
#define SIS_PCMC_REGAD $0xAD
#define SIS_PCMC_REGAE $0xAE
#define SIS_PCMC_REGAF $0xAF

#define SIS_PCMC_REGB0 $0xB0
#define SIS_PCMC_REGB1 $0xB1
#define SIS_PCMC_REGB2 $0xB2
#define SIS_PCMC_REGB3 $0xB3
#define SIS_PCMC_REGB4 $0xB4
#define SIS_PCMC_REGB5 $0xB5
#define SIS_PCMC_REGB6 $0xB6
#define SIS_PCMC_REGB7 $0xB7
#define SIS_PCMC_REGB8 $0xB8
#define SIS_PCMC_REGB9 $0xB9
#define SIS_PCMC_REGBA $0xBA
#define SIS_PCMC_REGBB $0xBB
#define SIS_PCMC_REGBC $0xBC
#define SIS_PCMC_REGBD $0xBD
#define SIS_PCMC_REGBE $0xBE
#define SIS_PCMC_REGBF $0xBF
	
					/*; AGP Control Register	*/
#define SIS_PCMC_REGC0 $0xC0 /* AGP Capability Identify Register (RO)*/
#define SIS_PCMC_REGC1 $0xC1 /**/
#define SIS_PCMC_REGC2 $0xC2 /**/
#define SIS_PCMC_REGC3 $0xC3 /**/
#define SIS_PCMC_REGC4 $0xC4 /* AGP Status Register (RO)*/
#define SIS_PCMC_REGC5 $0xC5 /**/
#define SIS_PCMC_REGC6 $0xC6 /**/
#define SIS_PCMC_REGC7 $0xC7 /**/
#define SIS_PCMC_REGC8 $0xC8 /* AGP Command Register*/
#define SIS_PCMC_REGC9 $0xC9 /**/
#define SIS_PCMC_REGCA $0xCA /**/
#define SIS_PCMC_REGCB $0xCB /**/
#define SIS_PCMC_REGCC $0xCC
#define SIS_PCMC_REGCD $0xCD
#define SIS_PCMC_REGCE $0xCE
#define SIS_PCMC_REGCF $0xCF

/**/
/* Vendor ID and Device ID*/
/* */
#define VENDOR_SIS_ID $0x1039
#define DEVICE_5511_ID $0x5591
#define DEVICE_5512_ID $0x0
#define DEVICE_5513_ID $0x5513

/**/
#define SiS501_DEV_NUM $0x /* SiS501 device number*/
#define SiS503_DEV_NUM $0x1 /* SiS502 device number*/
#define PCI_CONFIG_ADDR $0xCF8
#define PCI_CONFIG_DATA $0xCFC
#define PCI_I2C_DATA $0xCFD

#define INT_CNTRL1 $0x4D0
#define INT_CNTRL2 $0x4D1

#define MPATTERN0 $0x55AAAA55
#define MPATTERN1 $~0x55AAAA55

#define MPATTERN24 $0x24242424
#define MPATTERN23 $0x23232323
#define MPATTERN22 $0x22222222
#define MPATTERN21 $0x21212121
#define MPATTERN12 $0x12121212

#define MPATTERN28 $0x28282828
#define MPATTERN27 $0x27272727
#define MPATTERN26 $0x26262626
#define MPATTERN13 $0x13131313

#define MPATTERN10 $0x10101010
#define MPATTERN9 $0x9090909
#define MPATTERN14 $0x14141414

#define AD28 BIT3 /*(00001000b)*/
#define AD27 BIT6 /*(01000000b)*/
#define AD26 BIT2 /*(00000100b)*/
#define AD24 BIT1 /*(00000010b)*/
#define AD23 BIT4 /*(00010000b)*/
#define AD13 BIT5 /*(00100000b)*/
#define AD12 BIT0 /*(00000001b)*/

#define SDPATTERN0 $0xAAAA5555
#define SDPATTERN1 $0x5555aaaa

#define	Fail_Value	$0xff	/* -1h	     */
#define	SMA_1_11_08	$0x00	/* 00000000b */
#define	SMA_1_13_08	$0x01	/* 00000001b */
#define	SMA_1_11_09	$0x04	/* 00000100b */
#define	SMA_1_13_09	$0x05	/* 00000101b */
#define	SMA_1_11_10	$0x08	/* 00001000b */
#define	SMA_1_13_10	$0x09	/* 00001001b */
#define	SMA_1_13_11	$0x0d	/* 00001101b */
#define	SMA_2_11_08	$0x0c	/* 00001100b */
#define	SMA_2_12_08	$0x02	/* 00000010b */
#define	SMA_2_13_08	$0x03	/* 00000011b */
#define	SMA_2_12_09	$0x06	/* 00000110b */
#define	SMA_2_13_09	$0x07	/* 00000111b */
#define	SMA_2_12_10	$0x0a	/* 00000010b */
#define	SMA_2_13_10	$0x0b	/* 00001011b */
#define	SMA_2_12_11	$0x0e	/* 00001110b */
#define	SMA_2_13_11	$0x0f	/* 00001111b */

#endif
