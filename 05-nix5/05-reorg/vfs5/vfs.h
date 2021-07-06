#ifndef __VFS_H__
#define __VFS_H__

#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include "console.h"
#include "ramfile.h"

#define CONSOLE 1
#define RAMFILE 2

#define O_RDONLY  0x000
#define O_WRONLY  0x001
#define O_RDWR    0x002
#define O_CREAT   0x200
#define O_TRUNC   0x400

#define STDIN   0
#define STDOUT  1
#define STDERR  2
#define STDRAMF 3

#define EOF -1

struct file {
  enum { FD_NONE, FD_PIPE, FD_INODE, FD_DEVICE } type;
  int ref; // reference count
  char readable;
  char writable;
  // struct pipe *pipe; // FD_PIPE
  // struct inode *ip;  // FD_INODE and FD_DEVICE
  size_t offset;     // FD_INODE
  short major;       // FD_DEVICE
};

#define NFILE 128

struct ftable {
  // struct spinlock lock;
  struct file files[NFILE];
};

#define NDEV  32
// map major device number to device functions.
struct devsw {
  ssize_t (*read)(void *, size_t);
  ssize_t (*write)(const void *, size_t);
};

extern struct devsw devsw[NDEV];
extern struct ftable ftable;

#define FILE struct file

FILE *stdin, *stdout, *stderr, *ramfile;

void vfs_init();
int vfs_open(const char *pathname, int flags);
ssize_t vfs_read(int fd, void *buf, size_t count);
ssize_t vfs_write(int fd, const void *buf, size_t count);
int vfs_close(int fd);

size_t vfs_fread(void *buf, size_t size, size_t nmemb, FILE* stream);
size_t vfs_fwrite(const void * buf, size_t size, size_t nmemb, FILE* stream);
#define fread vfs_fread
#define fwrite vfs_fwrite
#define PATH_RAMF "/dev/ramfile"

#endif
