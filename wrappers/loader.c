#include <unistd.h>
#include <sys/stat.h>

#include "../config.h"
#include "../forkserver.h"

struct fork_server_config conf = {
  .associate_after_fork = false,
};

int main(int argc, char** argv)
{
  pid_t child;

  if (argc < 2)
    return -1;

  afl_fork_server(&conf);

  chmod(argv[1], S_IRWXU);
  if ((child = fork()))
    return waitpid(child, NULL, 0x0) == child;
  else {
    ioctl(DEVAFL_FD, 42, 0);
    return execl(argv[1], argv[1], NULL);
  }
}
