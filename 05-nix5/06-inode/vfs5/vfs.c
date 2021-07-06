#include <vfs.h>
#include <string.h>
#include <ramfile.h>
#include <console.h>

struct ftable ftable;
struct devsw devsw[NDEV];

void vfs_init() {
  stdin = &ftable.files[STDIN];
  stdout = &ftable.files[STDOUT];
  stderr = &ftable.files[STDERR];
  ramfile = &ftable.files[STDRAMF];
	console_init();
  ramfile_init();
}

/*
int mount(const char *source, const char *target,
          const char *filesystemtype, unsigned long mountflags,
          const void *data);
*/

int vfs_putc(int ch) {
	char c = (char) ch;
	// return console_write(&c, 1);
	// return write(STDOUT, &c, 1);
	return vfs_write(STDOUT, &c, 1);
}

int vfs_getc() {
	char c;
	// console_read(&c, 1);
	// read(STDIN, &c, 1);
	vfs_read(STDIN, &c, 1);
	return (int) c;
}

int vfs_open(const char *pathname, int flags) {
  if (strcmp(pathname, PATH_RAMF)==0) return ramfile_open(flags);
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
