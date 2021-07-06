#include <stdio.h>
#include <string.h>

int main() {
  printf("strchr(abc, 0)=%d\n", strchr("abc", '\0'));
  printf("strchr(abc, b)=%d\n", strchr("abc", 'b'));
  printf("strchr(,()# \t\r\n, o)=%d", strchr(",()# \t\r\n", 'o'));
}