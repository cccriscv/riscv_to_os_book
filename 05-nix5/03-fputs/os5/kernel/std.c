#include "nix.h"

void std_init() {
  stdin = &ftable.files[STDIN];
  stdout = &ftable.files[STDOUT];
  stderr = &ftable.files[STDERR];
}

ssize_t std_read(FILE *f, void *buf, size_t n) {
	if (f->type == FD_DEVICE) {
		short  dev = f->major;
		return devsw[dev].read(buf, n);
	}
	return -1;
}

ssize_t std_write(FILE *f, const void *buf, size_t n) {
	if (f->type == FD_DEVICE) {
		short  dev = f->major;
		return devsw[dev].write(buf, n);
	}
	return -1;
}

int std_putc(int ch) {
	char c = (char) ch;
	// return console_write(&c, 1);
	// return write(STDOUT, &c, 1);
	return std_write(stdout, &c, 1);
}

int std_getc() {
	char c;
	// console_read(&c, 1);
	// read(STDIN, &c, 1);
	std_read(stdin, &c, 1);
	return (int) c;
}
