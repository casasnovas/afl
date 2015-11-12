#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>

#include "config.h"
#include "forkserver.h"

static int null_fd;
static pid_t child;

int pre_hook(unsigned int argc, char** argv)
{
  null_fd = open("/dev/null", O_RDWR);
}

void post_hook(unsigned int argc, char* argv)
{
  kill(child, SIGKILL);
}

int run(int argc, char** argv)
{

  if (argc < 2)
    return -1;

  chmod(argv[1], S_IRWXU);
  if ((child = fork())) {
    if (child < 0)
      return 1;
    return waitpid(child, NULL, 0x0) == child;
  }
  else {
    dup2(null_fd, 0);
    dup2(null_fd, 1);
    dup2(null_fd, 3);
    ioctl(DEVAFL_FD, 42, 0); /* assoc */
    close(DEVAFL_FD);
    exit(execl(argv[1], argv[1], NULL));
  }
}
