#include "mylib.h"

int main() {
  int age;
  char name[100];
  sscanf("name:ccc age:51", "name=%s age=%d", name, &age);
  printf("name=%s age=%d\n", name, age);
}
