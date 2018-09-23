/* -*- C -*- */

#ifndef __restart_h__
#define __restart_h__

#include "config.h"
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#define BLKSIZE 1024

#ifdef __cplusplus
extern "C" {
#endif

  /* function copies a file from fromfd to tofd */
  int copyfile(int fromfd, int tofd);

  /*
    r_read is similar to read except that it restarts itself if
    interrupted by a signal and writes the full amount requested
  */
  ssize_t r_read(int fd, void *buf, size_t size);

  /*
    r_write is similar to write except that it restarts itself if
    interrupted by a signal and writes the full amount requested
  */
  ssize_t r_write(int fd, void *buf, size_t size);

  /*
   * A function that reads a specific number of bytes
   *
   */
  size_t readblock(int fd, void *buf, size_t size);

  /*
   * Function is similar to close except it restarts itself if
   * interrupted by a signal
   */
  int r_close(int fd);

  /*
   * Keep waiting for children after a signal
   *
   */
  pid_t r_wait(int *stat_loc);
  
#ifdef __cplusplus
}
#endif

#endif /* __restart_h__ */
