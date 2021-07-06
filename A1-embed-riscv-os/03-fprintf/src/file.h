struct file {
  enum { FD_NONE, FD_PIPE, FD_INODE, FD_DEVICE } type;
  int ref; // reference count
  char readable;
  char writable;
  // struct pipe *pipe; // FD_PIPE
  // struct inode *ip;  // FD_INODE and FD_DEVICE
  // uint off;          // FD_INODE
  short major;       // FD_DEVICE
};

// map major device number to device functions.
struct devsw {
  int (*read)(int, uint64, int);
  int (*write)(int, uint64, int);
};

extern struct devsw devsw[];

#define CONSOLE 1
