#include "codeconv.h"

#include <string.h>

extern char NativeLANG[20];

int EUCKRtoUTF16(char *out, size_t outlen, char *in, size_t inlen)
{
  iconv_t icd;
  size_t inbytesleft = inlen + 1;
  size_t outbytesleft = outlen;
  int ret;
                                                                                
  icd = iconv_open("UTF-16LE", "EUC-KR");
  ret = (int)iconv(icd, &in, &inbytesleft, &out, &outbytesleft);
  iconv_close(icd);
                                                                                
  return ret;
}

int UTF16toEUCKR(char *out, size_t outlen, char *in, size_t inlen)
{
  iconv_t icd;
  size_t inbytesleft = inlen + 1;
  size_t outbytesleft = outlen;
  int ret;
                                                                                
  icd = iconv_open("EUC-KR", "UTF-16LE");
  ret = (int)iconv(icd, &in, &inbytesleft, &out, &outbytesleft);
  iconv_close(icd);
                                                                                
  return ret;
}

int EUCKRtoUTF8(char *out, size_t outlen, char *in, size_t inlen)
{
  iconv_t icd;
  size_t inbytesleft = inlen + 1;
  size_t outbytesleft = outlen;
  int ret;
                                                                                
  icd = iconv_open("UTF-8", "EUC-KR");
  ret = (int)iconv(icd, &in, &inbytesleft, &out, &outbytesleft);
  iconv_close(icd);
                                                                                
  return ret;
}

int UTF8toEUCKR(char *out, size_t outlen, char *in, size_t inlen)
{
  iconv_t icd;
  size_t inbytesleft = inlen + 1;
  size_t outbytesleft = outlen;
  int ret;
                                                                                
  icd = iconv_open("EUC-KR", "UTF-8");
  ret = (int)iconv(icd, &in, &inbytesleft, &out, &outbytesleft);
  iconv_close(icd);
                                                                                
  return ret;
}

int CP932toUTF8(char *out, size_t outlen, char *in, size_t inlen)
{
  iconv_t icd;
  size_t inbytesleft = inlen + 1;
  size_t outbytesleft = outlen;
  int ret;
                                                                                
  icd = iconv_open("UTF-8", "CP932");
  ret = (int)iconv(icd, &in, &inbytesleft, &out, &outbytesleft);
  iconv_close(icd);
                                                                                
  return ret;
}

#ifndef DISTRIBUTE
int NativetoUTF8(char *out, size_t outlen, char *in, size_t inlen)
{
  iconv_t icd;
  size_t inbytesleft = inlen + 1;
  size_t outbytesleft = outlen;
  int ret;
                                                                                
  if (NativeLANG[0] == '\0') {
    strcpy(out, in);
    return 0;   }
  icd = iconv_open("UTF-8", NativeLANG);
  ret = (int)iconv(icd, &in, &inbytesleft, &out, &outbytesleft);
  iconv_close(icd);
                                                                                
  return ret;
}

int UTF8toNative(char *out, size_t outlen, char *in, size_t inlen)
{
  iconv_t icd;
  size_t inbytesleft = inlen + 1;
  size_t outbytesleft = outlen;
  int ret;
                                                                                
  if (NativeLANG[0] == '\0') {
    strcpy(out, in);
    return 0;
  }
  icd = iconv_open(NativeLANG, "UTF-8");
  ret = (int)iconv(icd, &in, &inbytesleft, &out, &outbytesleft);
  iconv_close(icd);
 
  return ret;
}
#endif
