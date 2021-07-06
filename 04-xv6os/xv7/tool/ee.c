#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>

#define K 1024
#define TMAX (512*K)
#define SMAX K

char text[TMAX], newText[TMAX], block[TMAX];

int readFile(char *fname, char *text, int n) {
  int fd = open(fname, O_RDWR); // O_RDONLY
  assert(fd>0);
  int len = read(fd, text, n);
  text[len] = '\0';
  close(fd);
  return len;
}

void writeFile(char *fname, char *text) {
  int fd = open(fname, O_CREAT|O_RDWR|O_TRUNC, 644); // fd = open(fname, O_CREAT|O_WRONLY|O_TRUNC); 
  assert(fd>0);
  write(fd, text, strlen(text));
  close(fd);
}

int inputText(char *text) {
  char *p = text;
  while (1) {
    gets(p);
    // printf("p=%s len=%d\n", p, strlen(p));
    if (strncmp(p, "<end>", 5)==0) break;
    p += strlen(p);
    *p++ = '\n';
  }
  *p = '\0';
  return p-text;
}

void replaceText(char *text, int lmin, int lmax, char *block, char *newText) {
  char *p = text, *np = newText;
  int line = 0;
  while (1) {
    char *start = p;
    while (*p != '\n') {
      if (*p == '\0') break; // { strcpy(np, start); printf("newText=%s\n", newText); return; }
      p++;
    }
    line++;
    if (line == lmin) { strcpy(np, block); np += strlen(np); }
    if (line<lmin || line > lmax) {
      strncpy(np, start, p-start);
      np += p-start;
      if (*p != '\0') *np ++ = '\n';
      printf("%d: %.*s\n", line, p-start, start);
    }
    if (*p == '\0') break;
    p++;
  }
}

int create(char *fname) {
  inputText(text);
  writeFile(fname, text);
}

int list(char *text, int lmin, int lmax) {
  char *p = text;
  int line = 0;
  while (1) {
    char *start = p;
    while (*p != '\n') {
      if (*p == '\0') { return p-text; };
      p++;
    }
    line++;
    if (line >= lmin)
      printf("%d: %.*s\n", line, p-start, start);
    if (line >= lmax) break;
    p++;
  }
}

void replace(char *fname, int lmin, int lmax) {
  inputText(block);
  replaceText(text, lmin, lmax, block, newText);
  writeFile(fname, newText);
}

void insert(char *fname, int lmin) {
  inputText(block);
  replaceText(text, lmin, lmin-1, block, newText);
  writeFile(fname, newText);
}

void delete(char *fname, int lmin, int lmax) {
  replaceText(text, lmin, lmax, "", newText);
  writeFile(fname, newText);
}

int main(int argc, char *argv[]) {
  char *fname = argv[1];
  char *op = argv[2];
  int lmin = (argc > 3)?atoi(argv[3]):0;
  int lmax = (argc > 4)?atoi(argv[4]):9999999;
  if (*op == 'c') writeFile(fname, ""); else readFile(fname, text, TMAX);
  switch (*op) {
    case 'c': create(fname); break; // create file
    case 'l': list(text, lmin, lmax); break; // list file from min to max
    case 'r': replace(fname, lmin, lmax); break; // replace
    case 'i': insert(fname, lmin); break;
    case 'd': delete(fname, lmin, lmax); break; 
    default: printf("error: op=%s not found!", op);
  }
}
