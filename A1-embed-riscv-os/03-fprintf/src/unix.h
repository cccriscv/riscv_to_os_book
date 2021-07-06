#ifndef __UNIX_H__
#define __UNIX_H__

#include <stddef.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define FDT_MAX 128

typedef size_t (*readfp_t)(int, void*, int);
typedef size_t (*writefp_t)(int, void*, int);

size_t read(int fd, void *buf, size_t count);
size_t write(int fd, void *buf, size_t count);

struct fd_t {
  readfp_t readfp;
  writefp_t writefp;
};

struct fd_t fdt[];

#endif
