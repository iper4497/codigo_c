/*
 * mkrom.c - utility to make ROM images for i386 processors 
 * Copyright (c) 1999 by Net Insight AB. All Rights Reserved.
 *
 * $Id: mkrom.c,v 1.5 2000/11/13 22:55:59 rminnich Exp $
 *
 */

#ifndef lint
static char rcsid[] = "$Id: mkrom.c,v 1.5 2000/11/13 22:55:59 rminnich Exp $"; 
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
/*
#define __KERNEL__
#include <asm/page.h>
 */

/** flags: */
#define	FL_FORCE	0x01

#define	SZ1K		(1024)
#define	SZ64K		(64 * SZ1K)
#define	SZ128K		(128 * SZ1K)
#define	SZ256K		(256 * SZ1K)
#define	SZ512K		(512 * SZ1K)

#define	SZDEFAULT	(SZ256K)

/** global data: */
char *progname = 0;
int flags = 0, sz = (SZDEFAULT);

/* FIX ME SOON */
/* we need a struct that will accurately lay out the last 16 bytes 
 * of memory. messier than it seems it ought to be ... 
 */
struct tailer {
	unsigned short	offset;
	unsigned short	segment;
};
/* ought to be something like: 
struct tailer {
	unsigned char jumpinstruction;
	unsigned short	offset;
	unsigned short	segment;
	unsignec char unused[7];
	unsigned long empty_zero_page;
};
 */

int doit( char *source, char *output, unsigned long empty_zero_page)
{
	struct tailer *t;
	FILE *fin, *fout;
	struct stat st;
	int c = 0, p = 0, n = 0;
	char buf[128], *pc;
	unsigned long *ezp;

	if(!(flags & FL_FORCE) && access(output, R_OK) == 0) {
		printf("%s: file %s already exist\n", progname, output);
		exit(1);
	}
	
	if(access(source, R_OK) != 0) {
		printf("%s: can't access %s\n", progname, source);
		exit(1);
	}
	
	if(stat(source, &st) != 0) {
		printf("%s: can't stat %s\n", progname, source);
		exit(1);
	}
	
	if(st.st_size > (sz - 0x10)) {
		printf("%s: input file to large\n", progname);
		exit(1);
	}

	/* 
	 * Try to open the files.
	 */ 
	fin = fopen(source, "r");
	if(fin == NULL) {
		printf("%s: can't open %s\n", progname, source);
		exit(1);
	}
	
	fout = fopen(output, "w");
	if(fout == NULL) {
		printf("%s: can't create %s\n", progname, output);
		fclose(fin);
		exit(1);
	}

	/*
	 * Start processing image.
	 */
	while((c = fread(buf, 1, sizeof buf, fin)) > 0) {
		(void) fwrite(buf, 1, c, fout);
		p += c;
	}
	
	/* clear the buffer */
	(void) memset(buf, 0, sizeof buf);
	
	while(p < (sz - 0x10)) {
		n = ((sz - 0x10) - p) >= sizeof buf ? sizeof buf : ((sz - 0x10) - p);
		(void) fwrite(buf, 1, n, fout);
		p += n;
	}
	
	/*
	 * XXX must make this look better. Can't use
	 * a structure since single chars are aligned to
	 * 32bit.
	 */
	pc = (char *) buf;
	*pc		= 0xEA;
	t = (struct tailer *) (((char *) buf) + 1);

	t->offset	= 0;
	t->segment	= (0x100000 - sz) >> 4;
	ezp = &((unsigned long *)buf)[3];
	*ezp = empty_zero_page;

	(void) fwrite(buf, 1, 0x10, fout);
	
	fclose(fin);
	fclose(fout);
	
	exit(0);
}

void usage()
{
	printf("usage: %s [-s size] [-f] [-o output] file\n"
	       "valid rom sizes are: 64, 128, 256, 512. default: %d\n", 
		progname, sz >> 10);
	exit(0);
}

int main(argc, argv)
	int argc;
	char **argv;
{
	/* for linuxbios, we make it possible to set the 
	 * last DWORD in memory to the location of the empty_zero_page
 	 * in the kernel. In other words, 0xf000:0xfffc has a pointer
	 * to the empty_zero_page. This in turn allows linuxbios 
	 * bootstrap to pass parameters to the linux kernel. 
	 */
	unsigned long empty_zero_page = 0;
	int ch, size;
	char *out = 0;

	/** set our program name */
	progname = argv[0];

	/** check arguments */
	while((ch = getopt(argc, argv, "s:o:e:f")) != -1)
		switch(ch) {
			case 's':
				size = strtol(optarg, NULL, 0);
				switch(size) {
					case 64:
					case 128:
					case 256:
					case 512:
						sz = size << 10;
						break;
					default:
						printf("%s: invalid rom size\n",
							progname);
						exit(1);
				}
				break;
			
			case 'o':
				out = optarg;
				break;

#if 0
/* history. Eric Biederman has made this unnecessary. 
			case 'e': 
				/* this strtol allows 0x usage */
				/*empty_zero_page = strtol(optarg, 0, 16);*/
				/* oops, forgot, GNU strtol is STILL 
				 * broken ... c0104000 has high-order
				 * bit set, so of course you get 
				 * 0x7fffffff back ...
				 */
				sscanf(optarg, "%x", &empty_zero_page);
				printf("empty_zero_page is 0x%x\n", 
					empty_zero_page);
				/* convert to physical */
				empty_zero_page = __pa(empty_zero_page);
				printf("empty_zero_page physical is 0x%x\n", 
					empty_zero_page);
				break;
#endif
			case 'f':
				flags |= FL_FORCE;
				break;

			case '?':
			default:
				usage();
				break;		
		}

	argc -= optind;
	argv += optind;

	if(argc < 1) 
		usage();
		
	if(out == NULL) {
		char buf[128];
		
		sprintf(buf, "%s.rom", argv[0]);
		out = strdup(buf);
	}

	doit(argv[0], out, empty_zero_page);
	return 0;
}
