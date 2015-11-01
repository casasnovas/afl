#ifndef _FORKSERVER_H_
#  define _FORKSERVER_H_

#  include <stdbool.h>

struct fork_server_config {
  bool associate_after_fork;
};

#endif
