#include "mylib.h"

int main() {
  int age = 51;
  char name[100] = "ccc";
  printf("name:%s age:%d\n", name, age);
  // printf("====\n12");
  // printf("====\n34");
  // printf("====\n\n\n"); // 這個會失敗，看來是連結到系統的 printf 了 ....
  printf("====\n\n");
}
