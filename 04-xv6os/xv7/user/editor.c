#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
// #include <assert.h>

#define K 1024
#define TMAX (512*K)
#define SMAX K

char text[TMAX], newText[TMAX], block[TMAX];

int readFile(char *fname, char *text, int n) {
  int fd = open(fname, O_RDWR); // O_RDONLY
  if (fd < 0) return -1;
  int len = read(fd, text, n);
  text[len] = '\0';
  close(fd);
  return len;
}

void writeFile(char *fname, char *text) {
  int fd = open(fname, O_CREAT|O_RDWR|O_TRUNC, 644); // fd = open(fname, O_CREAT|O_WRONLY|O_TRUNC); 
  // assert(fd>0);
  write(fd, text, strlen(text));
  close(fd);
}

int inputText(char *text) {
  printf("input lines until <end>\n");
  char *p = text;
  while (1) {
    gets(p);
    // printf("p=%s len=%d\n", p, strlen(p));
    if (memcmp(p, "<end>", 5)==0) break;
    p += strlen(p);
    *p++ = '\n';
  }
  *p = '\0';
  return p-text;
}

void replace0(char *text, int lmin, int lmax, char *block, char *newText) {
  char *p = text, *np = newText;
  int line = 0;
  while (1) {
    char *start = p;
    while (*p != '\n') {
      if (*p == '\0') break; // { strcpy(np, start); printf("newText=%s\n", newText); return; }
      p++;
    }
    line++;
    if (line>=lmin && line<=lmax) {
      printf("%d: %.*s\n", line, (int) (p-start), start);
    } else { // if (line<lmin || line > lmax)
      memcpy(np, start, p-start);
      np += p-start;
      if (*p != '\0') *np ++ = '\n';
      *np = '\0';
      // printf("%d: %.*s\n", line, p-start, start);
    }
    if (line == lmin) { strcpy(np, block); np += strlen(np); }
    if (*p == '\0') break;
    p++;
  }
}

void list(int lmin, int lmax) {
  replace0(text, lmin, lmax, "", newText);
}

void replace(int lmin, int lmax) {
  inputText(block);
  replace0(text, lmin, lmax, block, newText);
  strcpy(text, newText);
}

void insert(int lmin) {
  inputText(block);
  replace0(text, lmin, lmin-1, block, newText);
  strcpy(text, newText);
}

void delete(int lmin, int lmax) {
  replace0(text, lmin, lmax, "", newText);
  strcpy(text, newText);
}

void editor(char *fname) {
  char cmd[SMAX], op[SMAX], ans[SMAX];
  int len = readFile(fname, text, TMAX);
  if (len < 0) *text = '\0';
  while (1) {
    int lmin=0, lmax=TMAX;
    printf("? ");
    gets(cmd);
    sscanf(cmd, "%s %d %d", op, &lmin, &lmax);
    // printf("op=%s lmin=%d lmax=%d\n", op, lmin, lmax);
    switch (*op) {
      case 'h': printf("h:help l:list i:insert r:replace d:delete q:quit\n"); break; // help
      case 'l': list(lmin, lmax); break;      // list file from min to max
      case 'r': replace(lmin, lmax); break;   // replace
      case 'i': insert(lmin); break;          // insert
      case 'd': delete(lmin, lmax); break;    // delete
      case 's': writeFile(fname, text); break; 
      case 'q': // quit
        printf("save file (Y/N) ? ");
        gets(ans);
        if (*ans == 'Y' || *ans == 'y') writeFile(fname, text);
        exit(0);
        break;
      default: printf("error: op=%s not found!", op);
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("editor <filename>\n");
    exit(1);
  }
  editor(argv[1]);
  return 0;
}
