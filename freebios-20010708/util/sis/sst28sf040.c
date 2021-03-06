/*
 * sst28sf040.c: driver for SST28SF040C flash models.
 *
 *
 * Copyright 2000 Silicon Integrated System Corporation
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
 *	4 MEgabit (512K x 8) SuperFlash EEPROM, SST28SF040 data sheet
 *
 * $Id: sst28sf040.c,v 1.2 2001/02/07 16:10:54 rminnich Exp $
 */

#include "flash.h"
#include "jedec.h"

#define AUTO_PG_ERASE1		0x20
#define AUTO_PG_ERASE2		0xD0
#define AUTO_PGRM			0x10
#define CHIP_ERASE		0x30
#define RESET			0xFF
#define READ_ID			0x90

static __inline__ void protect_28sf040 (char * bios)
{
	/* ask compiler not to optimize this */
	volatile unsigned char tmp;

	tmp = *(unsigned char *) (bios + 0x1823);
	tmp = *(unsigned char *) (bios + 0x1820);
	tmp = *(unsigned char *) (bios + 0x1822);
	tmp = *(unsigned char *) (bios + 0x0418);
	tmp = *(unsigned char *) (bios + 0x041B);
	tmp = *(unsigned char *) (bios + 0x0419);
	tmp = *(unsigned char *) (bios + 0x040A);
}

static __inline__ void unprotect_28sf040 (char * bios)
{
	/* ask compiler not to optimize this */
	volatile unsigned char tmp;

	tmp = *(unsigned char *) (bios + 0x1823);
	tmp = *(unsigned char *) (bios + 0x1820);
	tmp = *(unsigned char *) (bios + 0x1822);
	tmp = *(unsigned char *) (bios + 0x0418);
	tmp = *(unsigned char *) (bios + 0x041B);
	tmp = *(unsigned char *) (bios + 0x0419);
	tmp = *(unsigned char *) (bios + 0x041A);
}

static __inline__ erase_sector_28sf040 (char * bios, unsigned long address)
{
	*bios = AUTO_PG_ERASE1;
	*(bios + address) = AUTO_PG_ERASE2;

	/* wait for Toggle bit ready         */
	toggle_ready_jedec(bios);
}

static __inline__ write_sector_28sf040(char * bios, unsigned char * src,
				       unsigned char * dst, unsigned int page_size)
{
	int i;

	for (i = 0; i < page_size; i++) {
		/* transfer data from source to destination */
		if (*src == 0xFF) {
			dst++, src++;
			/* If the data is 0xFF, don't program it */
			continue;
		}
		/*issue AUTO PROGRAM command */
		*dst =  AUTO_PGRM;
		*dst++ = *src++;

		/* wait for Toggle bit ready */
		toggle_ready_jedec(bios);
	}
}

int probe_28sf040 (struct flashchip * flash)
{
	char * bios = flash->virt_addr;
	unsigned char  id1, id2, tmp;

	/* save the value at the beginning of the Flash */
	tmp = *bios;

	*bios = RESET;
	usleep(10);

	*bios = READ_ID;
	usleep(10);
	id1 = *(unsigned char *) bios;
	usleep(10);
	id2 = *(unsigned char *) (bios + 0x01);

	*bios = RESET;
	usleep(10);

	if (id1 == flash->manufacture_id && id2 == flash->model_id)
		return 1;

	/* if there is no SST28SF040, restore the original value */
	*bios = tmp;
	return 0;
}

int erase_28sf040 (struct flashchip * flash)
{
	char * bios = flash->virt_addr;

	unprotect_28sf040 (bios);
	*bios = CHIP_ERASE;
	*bios = CHIP_ERASE;
	protect_28sf040 (bios);

	usleep(10);
	toggle_ready_jedec(bios);
}

int write_28sf040 (struct flashchip * flash, char * buf)
{
	int i;
	int total_size = flash->total_size * 1024, page_size = flash->page_size;
	char * bios = flash->virt_addr;

	unprotect_28sf040 (bios);

	printf ("Programming Page: ");
	for (i = 0; i < total_size/page_size; i++) {
		/* erase the page before programming */
		erase_sector_28sf040(bios, i * page_size);

		/* write to the sector */
		printf ("%04d at address: 0x%08x", i, i * page_size);
		write_sector_28sf040(bios, buf + i * page_size, bios + i * page_size,
				     page_size);
		printf ("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
	}
	printf("\n");

	protect_28sf040 (bios);
}
