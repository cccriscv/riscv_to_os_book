#ifndef __OS_H__
#define __OS_H__

#include <string.h>
#include <stdio.h>
#include <board.h>
#include <vfs.h>

void os_init(void);
int os_start(void);
int main();

#define open vfs_open
#define read vfs_read
#define write vfs_write
#define close vfs_close

#endif
