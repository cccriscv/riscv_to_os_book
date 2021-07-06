#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char op[100], p1[100], p2[100], p3[100];

void scan1(char *pline) {
  char line[512];
  strcpy(line, pline);
  printf("line1=%s\n", line);
  char *s = line;
  while (*s != '\0') { 
    if (strchr(",\n\t", *s)!=NULL) *s=' ';
    s++;
  }
  printf("line2=%s\n", line);
  int tokens = 0;
  tokens = sscanf(line, "%s %s %s %s", op, p1, p2, p3);
  printf("tokens=%d op=%s p1=%s p2=%s p3=%s\n", tokens, op, p1, p2, p3);
}

int main() {
  scan1("add x1 x2 x3");
  //scan1("gcd:");
  scan1("addi	sp,sp,-48");
  scan1("sw	ra,44(sp)");
  scan1("sw	a0,-36(s0)");
}
/*
	.file	"gcd.c"
	.option nopic
	.text
	.align	2
	.globl	gcd
	.type	gcd, @function
gcd:
	addi	sp,sp,-48
	sw	ra,44(sp)
	sw	s0,40(sp)
	addi	s0,sp,48
	sw	a0,-36(s0)
	sw	a1,-40(s0)
	lw	a4,-36(s0)
*/