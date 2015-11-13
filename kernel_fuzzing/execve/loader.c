#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include "config.h"
#include "forkserver.h"

static int null_fd;
static pid_t child = -1;

int load_hook(unsigned int argc, char** argv)
{
  null_fd = open("/dev/null", O_RDWR);
}

static sigset_t old_set;
void pre_hook(unsigned argc, char** argv)
{
  sigset_t set;

  child = -1;

  sigemptyset(&set);
  sigaddset(&set, SIGCHLD);
  sigprocmask(SIG_BLOCK, &set, &old_set);
}

void post_hook(unsigned int argc, char* argv)
{
  sigprocmask(SIG_SETMASK, &old_set, NULL);
  if (child != -1) {
    kill(child, SIGKILL);
    waitpid(child, NULL, 0x0);
  }
}

static int wait_child(void)
{
  sigset_t sigchild_set;
  struct timespec timeout = {1, 0};

  sigemptyset(&sigchild_set);
  sigaddset(&sigchild_set, SIGCHLD);
  sigtimedwait(&sigchild_set, NULL, &timeout);
  return 0;
}

int run(int argc, char** argv)
{

  if (argc < 2)
    return -1;

  chmod(argv[1], S_IRWXU);
  if ((child = fork())) {
    if (child < 0)
      return 1;
    return wait_child();
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
