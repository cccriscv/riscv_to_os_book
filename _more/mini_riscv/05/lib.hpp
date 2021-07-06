#ifndef __LIB_H__
#define __LIB_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <ctype.h>
#include <assert.h>
#include <iostream> 
#include <map>

using namespace std;

#define SMAX 100
#define LMAX 256
#define TMAX 10000

enum BaseType { i8, i16, i32, i64, u8, u16, u32, u64 };
enum Endian { LittleEndian, BigEndian };

#define ERROR(...) { fprintf(stderr, __VA_ARGS__); exit(1); }
#define ASSERT(cond, ...) { if (!cond) { fprintf(stderr, __VA_ARGS__); assert(cond); } }
#define BITS(x, from, to) ((uint32_t)(x)<<(32-(to+1)))>>(32-(to-from+1)) // 包含 to, 用 BITS(x, 1, 30) 去思考
#define SGN(x, i) (BITS(x,i,i)==0)? x : (-(1<<i)+(int32_t)BITS(x, 0, i-1))+1 // x[i] 為 sign bit，轉為整數。
extern int mapFind(map<string, int> *m, char *name);
extern void mapDump(map<string, int> *m);
extern void mapReverse(map<string, int> *m, map<int, string> *rm);
extern void word2bytes(uint32_t word, uint8_t bytes[], enum Endian endian);
extern uint32_t bytes2word(uint8_t bytes[], enum Endian endian);
extern void bytes2hex(uint8_t *bytes, int size, char *hex);

#endif
