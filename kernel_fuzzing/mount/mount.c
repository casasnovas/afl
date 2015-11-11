#include <sys/mount.h>
#include <sys/stat.h>

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#include <linux/loop.h>

#include "forkserver.h"

static char mount_point[16];
static void unmount_it(int mount_nr)
{
  snprintf(mount_point, sizeof(mount_point), "/mnt/%d", mount_nr);
  umount(mount_point);
}

static int loop_fd;
static char loop_device[16];
static void loop_setup(int loop_nr)
{
  snprintf(loop_device, sizeof(loop_device), "/dev/loop%d", loop_nr);

  loop_fd = open(loop_device, O_RDWR);
}

static void loop_detach(void)
{
  ioctl(loop_fd, LOOP_CLR_FD, 0);
}

static void loop_attach(const char* file)
{
  int file_fd = open(file, O_RDWR);

  ioctl(loop_fd, LOOP_SET_FD, file_fd);
  close(file_fd);
}

static void loop_setinfo(const char* file)
{
  static struct loop_info64 linfo;
  strncpy(linfo.lo_file_name, file, sizeof(linfo.lo_file_name));
  ioctl(loop_fd, LOOP_SET_STATUS64, &linfo);
}

static const char* fstype;
static void mount_it()
{
  mount(loop_device, mount_point, fstype, 0x0, NULL);
}

static int nr_fuzzer;
void pre_hook(unsigned int argc, char**argv)
{
  nr_fuzzer = atoi(argv[2]);
  fstype = argv[1]
  loop_setup(nr_fuzzer);
}

int run(unsigned int argc, char** argv)
{
  if (argc < 4) {
    fprintf(stderr, "Missing input file to mount.\n");
    return -1;
  }

  unmount_it(nr_fuzzer);

  loop_detach();
  loop_attach(argv[3]);
  loop_setinfo(argv[3]);

  mount_it();

  return 0;
}
