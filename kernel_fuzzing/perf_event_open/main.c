#define _GNU_SOURCE

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <linux/perf_event.h>
#include <linux/hw_breakpoint.h>

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

#ifdef VERBOSE
#define die(fmt, ...) \
	do { \
		fprintf(stderr, "error: " fmt "\n", ##__VA_ARGS__); \
		exit(1); \
	} while (0)
#else
#define die(fmt, ...) _exit(1)
#endif

void die_gracefully(int signum)
{
	/* _exit() is safe to call in signal handlers */
	_exit(1);
}

static inline void setup_signal_handlers()
{
	/* We want these signals to kill the process without telling afl-fuzz
	 * that we crashed (as we only really care if the _kernel_ crashed). */
	signal(SIGSEGV, die_gracefully);
	signal(SIGBUS, die_gracefully);
}

struct params {
	/* For perf_event_open() */
	struct perf_event_attr attr;
	unsigned long flags;

	/* For mmap() */
	unsigned long addr;
	unsigned int pages_shift;
	int prot;
	int mmap_flags;
	off_t offset;

	/* For ioctl() */
	int request;
	unsigned long ioctl_data;
} __attribute__((packed));

int perf_event_open(struct perf_event_attr *attr,
	pid_t pid, int cpu, int group_fd,
	unsigned long flags)
{
	return syscall(__NR_perf_event_open, attr, pid, cpu, group_fd, flags);
}

static void create(const char *filename)
{
	FILE *fp = fopen(filename, "w");
	if (!fp)
		die("could not open %s for writing", filename);

	struct params p;
	memset(&p, 0, sizeof(p));
	p.mmap_flags = MAP_PRIVATE | MAP_ANONYMOUS;

	if (fwrite(&p, 1, sizeof(p), fp) != sizeof(p))
		die("could not write initial parameters");

	fclose(fp);
}

static void run(const char *filename)
{
#ifndef VERBOSE
	setup_signal_handlers();
#endif

	FILE *fp = fopen(filename, "r");
	if (!fp)
		die("could not open %s", filename);

	struct params p;
	if (fread(&p, 1, sizeof(p), fp) != sizeof(p))
		die("could not read parameters");

	/* perf_event_open() */
	int fd = perf_event_open(&p.attr, 0, -1, -1, p.flags);
	if (fd == -1)
		die("perf_event_open() failed");

	/* mmap() */
	unsigned int nr_pages = 1 + (1 << p.pages_shift);
	void *pages = mmap((void *) p.addr, nr_pages * PAGE_SIZE, p.prot, p.mmap_flags, fd, p.offset);
	if (pages == MAP_FAILED)
		die("mmap() failed");

	/* ioctl */
	ioctl(fd, p.request, p.ioctl_data);

	char buffer[1024];
	size_t len = read(fd, buffer, sizeof(buffer));
	/* don't care about return value */
}

int main(int argc, char *argv[])
{
	if (argc < 2)
		die("usage: %s image", argv[0]);

#if MODE == MODE_INIT
	create(argv[1]);
#endif

#if MODE == MODE_RUN
	run(argv[1]);
#endif

	return 0;
}
