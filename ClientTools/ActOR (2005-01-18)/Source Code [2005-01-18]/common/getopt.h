#ifndef __GETOPT_H__
#define __GETOPT_H__

#ifdef __cplusplus
extern "C" {
#endif

extern char *optarg;
extern int optind;
extern int my_getopt(int, char **, char *);

#ifdef __cplusplus
}
#endif

#endif
