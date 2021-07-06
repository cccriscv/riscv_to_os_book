#include "os.h"

int main() {
  printf("=============== stdio test ==================\n");
  char line[100] = "name:ccc age:51";
  char name[100] = "xxx";
  int age = 0;
  sscanf(line, "name:%s age:%d", name, &age);
  printf("hello %s, you are %d years old!\n", name, age);
  printf("type some thing: ");
  gets(line);
  printf("you type: %s\n", line);
  printf("=============== ramdisk test ==================\n");
  int fd = open("/dev/ramfile", O_CREAT|O_TRUNC);
  char msg[] = "Hello RAM File!";
  printf("write <%s> to ram file\n", msg);
  write(fd, msg, strlen(msg));
  close(fd);
  int fd2 = open("/dev/ramfile", O_RDONLY);
  char msg2[100];
  read(fd2, msg2, 100);
  printf("read <%s> from ram file\n", msg);
  close(fd2);
}
