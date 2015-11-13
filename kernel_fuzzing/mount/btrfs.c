#include <sys/types.h>
#include <sys/uio.h>

#include <assert.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "btrfs-extents.h"

/* Construct a proper filesystem image given a compact input image
 * (typically the testcase afl has mutated). */
static int construct_image(const char *in, const char *out)
{
	int infd = open(in, O_RDONLY);
	if (infd == -1)
		return -1;

	ssize_t read_len = read(infd, buffer, in_size);
	close(infd);
	if (read_len != in_size)
		return -1;

	int outfd = open(out, O_WRONLY | O_CREAT | O_TRUNC, 0700);
	if (outfd == -1)
		return -1;

	if (ftruncate(outfd, out_size) == -1)
		goto error_close_outfd;

	const uint8_t *ptr = buffer;
	for (unsigned int i = 0; i < sizeof(extents) / sizeof(*extents); ++i) {
		if (lseek(outfd, extents[i].offset, SEEK_SET) == -1)
			goto error_close_outfd;

		ssize_t write_len = write(outfd, ptr, extents[i].len);
		if (write_len != extents[i].len)
			goto error_close_outfd;

		ptr += extents[i].len;
	}

	/* Sanity check to see that we read exactly how much we expected */
	assert(ptr == &buffer[in_size]);

	close(outfd);
	return 0;

error_close_outfd:
	close(outfd);
	return -1;
}

#if 0
int main(int argc, char *argv[])
{
	int err = construct_image(argv[1], argv[2]);
	if (err)
		return 1;

	return 0;
}
#endif
