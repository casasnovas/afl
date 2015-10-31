#include "config.h"

#include <stdio.h>
#include <stropts.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>

#define die(Args, ...) do {						\
		fprintf(stderr, "died at %s:%d:" Args, __FILE__, __LINE__, ## __VA_ARGS__); \
		abort();						\
	} while (0)

static int afl_notify_parent(void)
{
	uint32_t syn = 0x4;

	if (write(FORKSRV_FD + 1, &syn, sizeof(syn)) != 4)
		return -1;
	return 0;
}

static void afl_wait_parent(void)
{
	uint32_t ack;
	
	if (read(FORKSRV_FD, &ack, sizeof(ack)) != 4)
		die("Didn't receive ack from parent.");
}

static pid_t child;
static void afl_tell_child_status(void)
{
	int status;
	if (waitpid(child, &status, 0x0) < 0)
		die("Could not check child status.");
	if (write(FORKSRV_FD + 1, &status, 4) != 4)
		die ("Could not tell child status.");
}

static void afl_tell_pid(void)
{
	if (write(FORKSRV_FD + 1, &child, 4) != 4)
		die("Couldn't tell PID to parent.");
}

static void afl_assoc_area(void)
{
	ioctl(DEVAFL_FD, 42, 0);
}

static int afl_fork(void)
{
	switch ((child = fork())) {
	case -1:
		die("Couldn't fork.");
	case 0:  /* child */
		afl_assoc_area();
		break;
	default: /* us */
		afl_tell_pid();
	}
	return child;
}
		

static void _afl_fork_server(void)
{
	while (1) {
		afl_wait_parent();
		if (afl_fork() == 0)
			return; /* child runs */
		afl_tell_child_status();
	}
}

void afl_fork_server(void)
{
	if (afl_notify_parent() < 0)
		return;

	_afl_fork_server();
}
