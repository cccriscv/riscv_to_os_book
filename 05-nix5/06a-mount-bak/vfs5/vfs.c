#include <vfs.h>
#include <string.h>
#include <ramfile.h>
#include <console.h>

struct ftable ftable;
struct devsw devsw[NDEV];

void vfs_init() {
	mtable.top = 0;
  stdin = &ftable.files[STDIN];
  stdout = &ftable.files[STDOUT];
  stderr = &ftable.files[STDERR];
  ramfile = &ftable.files[STDRAMF];
	console_init();
  ramfile_init();
	vfs_mount("", "/dev/echo", "file", O_WRONLY, &devsw[CONSOLE]);
}

int vfs_mount(const char *source, const char *target,
          const char *filesystemtype, unsigned long mountflags,
          const void *data) {
	struct mount *m =  mtable.mounts[mtable.top];
	m->source = source;
	m->target = target;
	m->filesystemtype = filesystemtype;
	m->mountflags = mountflags;
	m->data = data;
	mtable.top++;
}

int vfs_open(const char *pathname, int flags) {
  if (strcmp(pathname, PATH_RAMF)==0) return ramfile_open(flags);
	struct mount *m = vfs_match_mount(pathname);
	if (m && strcmp(m->filesystemtype, "file")==0) {
		int dev = (int) (m->data);
		int fd = vfs_unused_fd();
		ftable.files[fd].major = dev;
		return fd;
	}
	// vfs_dev_open(di, &pathname[strlen(head)]);
  return -1;
}

size_t vfs_fread(void *buf, size_t size, size_t nmemb, FILE* stream) {
  int n;
	if (stream->type == FD_DEVICE) {
		short  dev = stream->major;
		n = devsw[dev].read(buf, size*nmemb);
	}
	return n/size;
}

ssize_t vfs_read(int fd, void *buf, size_t n) {
	FILE *stream = &ftable.files[fd];
	return vfs_fread(buf, 1, n, stream);
}

size_t vfs_fwrite(const void * buf, size_t size, size_t nmemb, FILE* stream) {
  int n;
	if (stream->type == FD_DEVICE) {
		short  dev = stream->major;
		n = devsw[dev].write(buf, size*nmemb);
	}
	return n/size;
}

ssize_t vfs_write(int fd, const void *buf, size_t n) {
	struct file *stream = &ftable.files[fd];
	return vfs_fwrite(buf, 1, n, stream);
}

int vfs_close(int fd) {
  if (fd == STDRAMF) return ramfile_close();
  return -1;
}

int vfs_putc(int ch) {
	char c = (char) ch;
	return vfs_write(STDOUT, &c, 1);
}

int vfs_getc() {
	char c;
	vfs_read(STDIN, &c, 1);
	return (int) c;
}

