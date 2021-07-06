#include "mylib.h"

int main() {
/*
  int age;
  char name[100];
  int count = sscanf("name:ccc age:51", "name:%s age:%d", name, &age);
  printf("count=%d\n", count);
  printf("name=%s age=%d\n", name, age);
*/
  char op[100];
  int lmin, lmax;
  int count = sscanf("list 3 5", "%s %d %d", op, &lmin, &lmax);
  printf("count=%d\n", count);
  printf("op=%s lmin=%d lmax=%d\n", op, lmin, lmax);
}
