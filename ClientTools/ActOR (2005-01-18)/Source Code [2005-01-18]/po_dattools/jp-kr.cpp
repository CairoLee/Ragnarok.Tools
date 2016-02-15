#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iconv.h>

int EUCKRtoUTF16(char *out, size_t outlen, char *in, size_t inlen);
void convert8(char *to, char *from);
void convertuni(char *to, char *from);

int main(int argc, char **argv)
{
  FILE *fp;
  char buf[512];
  char strmsg[512], str[512];
  int ret;
  char euckr[512], uni[512], uni_[512], sjis[512];
  char *u, *newu;
  
  fp = fopen("pakviewer.l.po.sjis", "r");

  printf("<html><head>\n");
  printf("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=shift_jis\">\n");
  printf("</head><body>\n");
  printf("<table border=\"1\">\n");

  while (feof(fp) == 0) {
    fgets(buf, sizeof(buf), fp);
    if (buf[0] != 'm' || strstr(buf, "msgid") == 0)  continue;
    sscanf(buf, "%s %s", strmsg, str);
    if (strlen(str) < 3)  {
      fgets(buf, sizeof(buf), fp);
      continue;
    }
    strcpy(euckr, &str[1]);
    euckr[strlen(euckr)-1] = '\0';
    if (euckr[0] == '\\' && '0' <= euckr[1] && euckr[1] <= '9') {
      strcpy(buf, euckr);
      convert8(euckr, buf);
    }
    newu = euckr;
    uni[0] = '\0';
    for (u = euckr; u < euckr+strlen(euckr);) {
      if ((u = strchr(newu, '_')) != NULL) {
        strncpy(buf, newu, u-newu);
        buf[u-newu] = '\0';
        ret = EUCKRtoUTF16(uni_, sizeof(uni), buf, strlen(buf));
        if (ret != 0) {
          strcpy(uni_, buf);
        } else {
          strcpy(buf, uni_);
          convertuni(uni_, buf);
        }
        strcat(uni, uni_);
        strcat(uni, "_");
        newu = u+1;
      } else {
        strcpy(buf, newu);        
        ret = EUCKRtoUTF16(uni_, sizeof(uni), buf, strlen(buf));
        if (ret != 0) {
          strcpy(uni_, buf);
        } else {
          strcpy(buf, uni_);
          convertuni(uni_, buf);
        }
        strcat(uni, uni_);
        break;
      }
    }
    
    fgets(buf, sizeof(buf), fp);
    sscanf(buf, "%s %s", strmsg, str);
    strcpy(sjis, &str[1]);
    sjis[strlen(sjis)-1] = '\0';
    if (strstr(euckr, "item")) continue;
    printf("<tr><td>%s  </td><td>%s  </td><td>%s  </td></tr>\n", sjis, euckr, uni);
    //printf("<p>%s %s %s\n", sjis, euckr, uni);
  }

  printf("</table>\n");
  printf("</body></html>\n");
}

void convertuni(char *to, char *from)
{
  char *c;
  unsigned short w;
  to[0] = '\0';
  char buf[10];

  for (c = from; c < from+strlen(from); c+=2) {
    if (*c == '_') {
      strcat(to, "_");
      c--;
      continue;
    }
    w = *(unsigned short *)c;
    sprintf(buf, "&#x%x;", w);
    strcat(to, buf);
  }
}

void convert8(char *to, char *from)
{
  char *c;
  char one;
  int tc;
  
  for (c = from, tc = 0; c < from+strlen(from); c++, tc++) {
    c++;  // for "\\"
    one = *c - '0';
    while (*(c+1) != '\\' && c != from+strlen(from)-1) {
      c++;
      one = one << 3 | (*c - '0');
    }
    to[tc] = one;
  }
  to[tc] = '\0';
}

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

