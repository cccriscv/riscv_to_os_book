#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define K 1024
#define TMAX (512*K)
#define SMAX K

char text[TMAX];

int readFile(char *fname, char *text, int n) {
  int fd = open(fname, O_RDONLY);
  int len = read(fd, text, n);
  close(fd);
  text[len] = '\0';
  return len;
}

void createFile(char *fname, char *text) {
  int fd = open(fname, O_CREAT|O_WRONLY|O_TRUNC); // fd = open(fname, O_CREAT|O_WRONLY|O_TRUNC); 
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

int create(char *fname) {
  inputText(text);
  createFile(fname, text);
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

int main(int argc, char *argv[]) {
  char *fname = argv[1];
  char *op = argv[2];
  int lmin, lmax;
  if (*op == 'c') createFile(fname, ""); else readFile(fname, text, TMAX);
  switch (*op) {
    case 'c': create(fname); break; // create file
    case 'l': // list file from min to max
      lmin = (argc > 3)?atoi(argv[3]):0;
      lmax = (argc > 4)?atoi(argv[4]):9999999;
      // printf("file=%s lmin=%d lmax=%d\n", text, lmin, lmax);
      list(text, lmin, lmax);
      break; 
    default: printf("error: op=%s not found!", op);
  }
}
