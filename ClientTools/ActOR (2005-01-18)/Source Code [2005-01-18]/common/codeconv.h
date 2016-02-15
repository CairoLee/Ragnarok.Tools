#ifndef __MY_CODECONV__
#define __MY_CODECONV__

#include <libintl.h>
#include <locale.h>
#include <iconv.h>

#include "global.h"

int EUCKRtoUTF16(char *out, size_t outlen, char *in, size_t inlen);
int UTF16toEUCKR(char *out, size_t outlen, char *in, size_t inlen);
int EUCKRtoUTF8(char *out, size_t outlen, char *in, size_t inlen);
int UTF8toEUCKR(char *out, size_t outlen, char *in, size_t inlen);
int CP932toUTF8(char *out, size_t outlen, char *in, size_t inlen);

#ifndef DISTRIBUTE
int NativetoUTF8(char *out, size_t outlen, char *in, size_t inlen);
int UTF8toNative(char *out, size_t outlen, char *in, size_t inlen);
#endif

#endif  // __MY_CODECONV__

