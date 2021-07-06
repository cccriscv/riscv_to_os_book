#include "mylib.h"

int main() {
  char *str = "137abc", *end;
  printf("%s base 10 = %d\n", str, strtol(str, &end, 10));
  printf("*end-str=%d\n", (end-str));
  printf("%s base 8  = %d\n", str, strtol(str, &end, 8));
  printf("*end-str=%d\n", (end-str));
  printf("%s base 16 = %d\n", str, strtol(str, &end, 16));
  printf("*end-str=%d\n", (end-str));

  printf("311 base 16 = %x\n", 311);
  printf("137 base 10 = %d\n", 137);
}
