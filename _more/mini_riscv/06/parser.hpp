#ifndef __LEX_H__
#define __LEX_H__

#include "lib.hpp"

extern char *next(char *p, char *token);
extern int parse(char *line);

extern char label[], p0[], p1[], p2[], p3[], cmdType;

#endif
