#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "config.h"
#include "forkserver.h"

int null_fd;

int pre_hook(unsigned int argc, char** argv)
{
  null_fd = open("/dev/null", O_RDWR);
}

int run(int argc, char** argv)
{
  pid_t child;

  if (argc < 2)
    return -1;

  chmod(argv[1], S_IRWXU);
  if ((child = fork()))
    return waitpid(child, NULL, 0x0) == child;
  else {
    dup2(null_fd, 0);
    dup2(null_fd, 1);
    dup2(null_fd, 3);
    ioctl(DEVAFL_FD, 42, 0); /* assoc */
    close(DEVAFL_FD);
    exit(execl(argv[1], argv[1], NULL));
  }
}
