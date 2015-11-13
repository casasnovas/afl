#include <asm/types.h>
#include <sys/socket.h>
#include <linux/rtnetlink.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>

char recv_buf[65536];

void load_hook(unsigned int argc, char**argv)
{
}

int run(int argc, char **argv) {
	if (argc < 3) {
		fprintf(stderr, "Missing input file\n");
		return -1;
	}

	const char *filename = argv[2];

	int input_fd = open(filename, O_RDONLY);
	if (input_fd < 0)
		return -1;

	int protocol;
	if (read(input_fd, &protocol, sizeof(protocol)) != sizeof(protocol))
		goto out;

	int sock_fd = socket(AF_NETLINK, SOCK_RAW, protocol);

	struct nlmsghdr *nh = (void *)&recv_buf[0];
	int input_bytes = read(input_fd, &recv_buf[0], sizeof(recv_buf));

	if (input_bytes < 1)
		goto out;

	send(sock_fd, &recv_buf[0], input_bytes, 0);

#if 0
	// I'm not sure if this really increases coverage
	recv(sock_fd, recv_buf, sizeof(recv_buf), MSG_DONTWAIT);
#endif
	close(sock_fd);

out:
	close(input_fd);
	return 0;
}
