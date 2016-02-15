/*
        getopt.c
 **/
#include <stdio.h>
#include <string.h>

#define _next_char(string) (char)(*(string+1))

#ifdef __cplusplus
extern "C" {
#endif
char *optarg = NULL;
int optind = 1;
int opterr = 1;

int my_getopt(int argc, char *argv[], char *opstring)
{
  static char *pIndexPosition = NULL;
  char *pArgString = NULL;
  char *pOptString;

  if ( pIndexPosition != NULL) {
    if (*(++pIndexPosition)) {
      pArgString = pIndexPosition;
    }
  }

  if (pArgString == NULL) {
    if (optind >= argc) {
      pIndexPosition = NULL;
      return EOF;
    }
    pArgString = argv[optind++];
/*
    if (('/' != *pArgString) &&
        ('-' != *pArgString)) {
*/
    if ('-' != *pArgString) {
      --optind;
      optarg = NULL;
      pIndexPosition = NULL;
      return EOF;
    }

    if ((strcmp(pArgString, "-") == 0) ||
        (strcmp(pArgString, "--") == 0)) {
      optarg = NULL;
      pIndexPosition = NULL;
      return EOF;
    }
    pArgString++;
  }

  if (':' == *pArgString) {
    return (opterr ? (int)'?' : (int)':');
  }
  else if ((pOptString = strchr(opstring, *pArgString)) == 0) {
    optarg = NULL;
    pIndexPosition = NULL;
    return (opterr ? (int)'?' : (int)*pArgString);
  }
  else {
    if (':' == _next_char(pOptString)) {
      if ('\0' != _next_char(pArgString)) {
        optarg = &pArgString[1];
      }
      else {
        if (optind < argc)
          optarg = argv[optind++];
        else {
          optarg = NULL;
          return (opterr ? (int)'?' : (int)*pArgString);
        }
      }
      pIndexPosition = NULL;
    }
    else {
      optarg = NULL;
      pIndexPosition = pArgString;
    }
    return (int)*pArgString;
  }
}

#ifdef __cplusplus
}
#endif

