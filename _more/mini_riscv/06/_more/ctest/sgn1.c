#include <stdio.h>
#include <stdint.h>

#define BITS(x, from, to) (x<<(32-(to+1)))>>(32-(to-from+1))
#define SGN(x, i) (BITS(x,i,i)==0)? x : (-(1<<i)+(int32_t)BITS(x, 0, i-1))+1

int main() {
  uint32_t t = 0x0FD0;
  int32_t s = SGN(0x0FD0, 11);
  // int32_t s = SGN(t, 11);
  printf("s=%d\n", s);
}