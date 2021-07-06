#include "parser.hpp"

char *next(char *p, char *token) {
  assert(p != NULL);
  token[0] = '\0';
  if (*p=='#' || *p=='\0') return p;
  while (*p != '\0' && strchr(" \t\r\n,()", *p)) p++;
  if (*p == '#' || *p == '\0') return p;
  char *tp = token;
  if (*p == '"') {
    p++;
    while (*p != '"') {
      if (*p == '\\') {
        p++;
        switch (*p) {
          case 't': *tp = '\t'; break;
          case 'n': *tp = '\n'; break;
          case 'r': *tp = '\r'; break;
          case '"': *tp = '"'; break;
          case '0': *tp = '\0'; break;
          default: ERROR("string format error");
        }
      } else {
        *tp = *p;
      }
      p++; tp++;
    }
    p++;
    *tp = '\0';
    // printf("token=%s\n", token);
  } else {
    while (*p != '\0' && strchr(",()# \t\r\n", *p)==NULL) {
      *tp++ = *p++;
    }
    *tp = '\0';
  }
  return p;
}

char label[100], p0[100], p1[100], p2[100], p3[100], cmdType;

int parse(char *line) {
  printf("// %s", line);
  label[0] = '\0';
  char *p = line;
  p = next(p, p0);
  if (strchr(p0, ':')) {
    strcpy(label, p0);
    strtok(label, ":");
    p = next(p, p0);
  }
  p = next(p, p1);
  p = next(p, p2);
  p = next(p, p3);
  // printf("label=%s p0=%s p1=%s p2=%s p3=%s\n", label, p0, p1, p2, p3);
  if (strcmp(p0, ".ascii")==0) { // ex: .ascii "Hello World!\n"
    cmdType = 'S'; // string
    return strlen(p1)+1;
  } else if (isalpha(p0[0])) { // ex: add x1, x0, 1
    cmdType = 'I'; // instruction
    return 4;
  } else if (p0[0]=='\0') {
    cmdType = ' ';
    return 0;
  } else {
    printf("parse: command unknown error !\n");
    assert(0);
  }
}
