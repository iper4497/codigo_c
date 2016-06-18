#include <boot/uniform_boot.h>

unsigned long uniform_boot_compute_header_checksum(
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

