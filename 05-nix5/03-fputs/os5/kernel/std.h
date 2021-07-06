#ifndef __STD_H__
#define __STD_H__

#include <stdarg.h>
#include <stddef.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2
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

#define FILE struct file

FILE *stdin, *stdout, *stderr;

void std_init();
ssize_t std_read(FILE *f, void *buf, size_t n);
ssize_t std_write(FILE *f, const void *buf, size_t n);
int std_putc(int ch);
int std_getc();

#endif
