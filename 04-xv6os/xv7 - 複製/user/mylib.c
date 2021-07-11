#include "mylib.h"

// ========================string========================
char* strcpy(char *s, const char *t) {
  char *os;
  os = s;
  while((*s++ = *t++) != 0)
    ;
  return os;
}

int strcmp(const char *p, const char *q) {
  while(*p && *p == *q)
    p++, q++;
  // return (uchar)*p - (uchar)*q;
  return *p - *q;
}

size_t strlen(const char *s) {
  int n;
  for(n = 0; s[n]; n++)
    ;
  return n;
}

char* strchr(const char *s, int c) {
  for(; *s; s++)
    if(*s == c)
      return (char*)s;
  return 0;
}

// ================== memory =============================
void* memset(void *dst, int c, size_t n) {
  char *cdst = (char *) dst;
  int i;
  for(i = 0; i < n; i++){
    cdst[i] = c;
  }
  return dst;
}

void* memmove(void *vdst, const void *vsrc, size_t n) {
  char *dst;
  const char *src;

  dst = vdst;
  src = vsrc;
  if (src > dst) {
    while(n-- > 0)
      *dst++ = *src++;
  } else {
    dst += n;
    src += n;
    while(n-- > 0)
      *--dst = *--src;
  }
  return vdst;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  const char *p1 = s1, *p2 = s2;
  while (n-- > 0) {
    if (*p1 != *p2) {
      return *p1 - *p2;
    }
    p1++;
    p2++;
  }
  return 0;
}

void *memcpy(void *dst, const void *src, size_t n) {
  return memmove(dst, src, n);
}

// ============================io===============================
int puts(const char *s) {
  const char *p = s;
	while (*p) putchar(*p++);
  return p-s;
}

char *gets(char *s) {
	char *p = s;
	while (1) {
		char ch = getchar();
		// putchar(ch);
		if (ch == '\n' || ch == '\r') break;
		*p++ = ch;
	}
	*p = '\0';
	return s;
}

// ========================= integer/string convert ===============
static char digits[] = "0123456789ABCDEF";

long strtol(const char *s, char **end, int base) {
  const char *p = s, *dp;
  long n=0;
  while((dp = strchr(digits, *p))!=0) {
    n = n*base + (dp-digits);
    p++;
  }
  *end = p;
  return n;
}

/*
int atoi(const char *s) {
  int n;
  n = 0;
  while('0' <= *s && *s <= '9')
    n = n*10 + *s++ - '0';
  return n;
}
*/
// ========================= printf/sscanf ========================
static char* printint(char *text, int xx, int base, int sgn) {
  char buf[16];
  int i, neg;
  unsigned int x;

  neg = 0;
  if(sgn && xx < 0){
    neg = 1;
    x = -xx;
  } else {
    x = xx;
  }

  i = 0;
  do {
    buf[i++] = digits[x % base];
  }while((x /= base) != 0);
  if(neg)
    buf[i++] = '-';

  char *p = text;
  while(--i >= 0)
    *p++ = buf[i];
	return p;
}

// Print to text. Only understands %d, %x, %s.
int vsprintf(char *text, const char *fmt, va_list vl) {
  char *s, *p=text;
  int c, i, state;

  // puts(fmt);
  state = 0;
  for(i = 0; fmt[i]; i++){
    // c = fmt[i] & 0xff;
    c = fmt[i];
    // putchar('.');
    // putchar(c);
    if(state == 0){
      if(c == '%'){
        state = '%';
      } else {
				*p++ = c;
      }
    } else if(state == '%'){
      if(c == 'd'){
        p=printint(p, va_arg(vl, int), 10, 1);
      } else if(c == 'l') {
        p=printint(p, va_arg(vl, long), 10, 0);
      } else if(c == 'x') {
        p=printint(p, va_arg(vl, int), 16, 0);
      } else if(c == 's'){
        s = va_arg(vl, char*);
        if(s == 0)
          s = "(null)";
        while(*s != 0){
          *p++ = *s++;
        }
      } else if(c == 'c'){
				*p++ = va_arg(vl, unsigned int);
      } else if(c == '%'){
				*p++ = c;
      } else { // Unknown % sequence.  Print it to draw attention.
				*p++ = '%';
				*p++ = c;
      }
      state = 0;
    }
  }
	*p = '\0';
	return p-text;
}

int sprintf(char *text, const char *fmt, ...) {
  va_list vl;
  va_start(vl, fmt);
  int len = vsprintf(text, fmt, vl);
	return len;
}

int printf1(const char *fmt, ...) {
  char text[TMAX];
  va_list vl;
  va_start(vl, fmt);
  int len = vsprintf(text, fmt, vl);
	puts(text);
  // printf("text=%s|\n", text);
	return len;
}

int vsscanf1(const char *text, const char *fmt, va_list vl) {
 	const char *fp=fmt, *tp=text;
  char *s, *out_loc;;
  int ret=0;
 	while (fmt && *fp && *tp)	{
 	    if (*fp == '%') {
 	       fp++;
 	       switch (*fp) {
 	           case 's': 
               s = va_arg(vl, char*);
               while (*tp != ' ' && *tp != '\0') *s++=*tp++;
	 	           ret ++;
	 	           break;
 	           case 'c': 
	 	           *(char *)va_arg( vl, char* ) = *tp++;
	 	           ret ++;
	 	           break;
 	           case 'd': 
	 	           *(int *)va_arg( vl, int* ) =strtol(tp, &out_loc, 10);
	 	           tp = out_loc;
	 	           ret++;
	 	           break;
 	           case 'x': 
	 	           *(int *)va_arg( vl, int* ) =strtol(tp, &out_loc, 16);
	 	           tp =out_loc;
	 	           ret++;
	 	           break;
 	        }
 	    } 
 	    else {
        if (*tp != *fp) {
          printf("*tp=%c *fp=%c : not match!\n");
          return ret;
        }
        tp++;
      }
      fp++;
    }
    return ret;
}

int sscanf(const char *text, const char *fmt, ...) {
  va_list vl;
  va_start(vl, fmt);
  return vsscanf1(text, fmt, vl);
}
