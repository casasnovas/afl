#include <unistd.h>
#include <sys/stat.h>

extern void afl_fork_server(void);

int main(int argc, char** argv)
{
  pid_t child;

  if (argc < 2)
    return -1;

  afl_fork_server();

  chmod(argv[1], S_IRWXU);
  if ((child = fork()))
    return waitpid(child, NULL, 0x0) == child;
  else {
    ioctl(42, 43, 0);
    ioctl(42, 42, 0);
    return execl(argv[1], argv[1], NULL);
  }
}
