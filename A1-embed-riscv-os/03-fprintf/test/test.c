#include <stdio.h>

int main() {
  char line[100] = "name:ccc age:51";
  char name[100] = "xxx";
  int age = 0;
  sscanf(line, "name:%s age:%d", name, &age);
  printf("hello %s, you are %d years old!\n", name, age);
  printf("type some thing: ");
  gets(line);
  printf("you type: %s\n", line);
}
