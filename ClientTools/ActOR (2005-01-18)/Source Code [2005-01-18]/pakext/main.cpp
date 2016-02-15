#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <locale.h>
#include <libintl.h>
#include <iconv.h>
#include <zlib.h>
#include "../common/getopt.h"

#include "../common/relocate/progname.h"
#include "../common/relocate/relocatable.h"

#ifdef WIN32
#define _UNICODE
#include <TCHAR.H>
#endif

#include "main.h"
#include "../EULA_comp.z"

extern "C" {
  extern
#ifdef WIN32
  __declspec(dllimport)
#endif
  const char *locale_charset(void);
}
char NativeLANG[20];

CPAK *pak1;
char *vsFilename = NULL;
char *listFilename = NULL;
int version_patch98;
MY_FLAGS flag;
bool agreement;

int main(int argc, char **argv)
{
  pak1 = new CPAK();

  SetTextDomain(argv[0]);

  ReadPreference();
  if (agreement == false) {
    agreement = GetAgreement();
    if (agreement == false) {
      return 0;
    }
  }
  WritePreference();

  ParseOptionAddFile(argc, argv, &flag);

  int reent = 0;

  do {
    if (flag.subtract > 0) {
#ifdef VIOLATION_CODE
      SubtractEntry();
#endif // VIOLATION_CODE
    } else if (flag.easymode) {
      reent = EasyExt(argc, argv);
      if (reent == 2) {
        flag.easymode = false;
        flag.chrviewer = true;
      }
    } else if (flag.chrviewer) {
      reent = CharacterViewer(argc, argv);
      if (reent == 1) {
        flag.chrviewer = false;
        flag.easymode = true;
      }
#ifdef VIOLATION_CODE
    } else if (flag.patch98) {
      MakePatch98();
    } else if (flag.appendPatch) {
      pak1->AppendtoGRF(flag.fnPatch);
    } else if (flag.makePatch) {
      MakePatch(&flag);
#endif // VIOLATION_CODE
    } else if (flag.listOnly) {
      WriteFileList(&flag);
      if (flag.useUnicode) {
        WriteFileListMsgFmt(&flag);
        WriteHangleHTML(&flag);
      }
    } else {
      ExtractFiles(&flag);
    }
  } while (reent);

  if (listFilename)  free(listFilename);
  if (vsFilename)  free(vsFilename);

  delete pak1;

  return 0;
}

#ifdef VIOLATION_CODE
void SubtractEntry(void)
{
  CPAK *pako;
  pako = new CPAK();
  pako->AddPakFile(flag.subFilename);
  if (pako->NumEntry > 0) {
    pako->SubtractGrf(flag.subFilename, pak1, flag.subtract);
    printf("Please repack %s.\n", flag.subFilename);
  }
}

void MakePatch(MY_FLAGS *flag)
{
  if (flag->forceWrite == false) {
    FILE *fp;
    fp = fopen("update.gpf", "rb");
    if (fp != NULL) {
      fclose(fp);
      printf("update.gpf is exist. not overwritten.\n");
      exit(0);
    }
  }

  if (flag->makePatch == 1) {
    pak1->MakePatchOpen("update.gpf", 0x103);
  } else {
    pak1->MakePatchOpen("update.gpf", 0x200);
  }
#ifdef WIN32
  if (flag->useUnicode) {
    WAddPatchDir(_TEXT("data"));
  } else {
    AddPatchDir("data");
  }
#else
  AddPatchDir("data", "data");
#endif
  pak1->MakePatchClose();
}
#endif // VIOLATION_CODE

void ExtractFiles(MY_FLAGS *flag)
{
  if (pak1->NumEntry < 1) return;

  CheckOutputDirectoryFailExit(flag);

  if (flag->FN_ID) {
    CHDIR(flag->outPath);
    MKDIR("data");
    CHDIR("data");
  }

  for (int i=0; i<pak1->NumEntry; i++) {
    if (IsStrMatch(i, flag) == false) continue;
    if (pak1->Entries[i].type == 2){
      printf("No.%d is directory\n", i+1);
      continue;
    } else {
      if (flag->useMatch) {
        printf("Now extract %s\n", pak1->Entries[i].fn);
      } else {
        printf("Now extract %d/%d\n", i+1, pak1->NumEntry);
      }
    }
    if (flag->FN_ID) {
      char buf[128];
      char *suffix;
      suffix = pak1->Entries[i].fn;
      suffix += strlen(suffix) - 4;
      sprintf(buf, "%d%s", i+1, suffix);
      DecodeWriteFile(buf, pak1->Entries[i].declen, pak1->Entries[i].fn, flag);
    } else {
      if (flag->useUnicode) {
        WCHDIR(flag->outPath);
      } else {
        CHDIR(flag->outPath);
      }
      CheckDir(pak1->Entries[i].fn, pak1->Entries[i].declen, pak1->Entries[i].fn, flag);
    }
  }
}

void CheckOutputDirectoryFailExit(MY_FLAGS *flag)
{
  if (flag->outPath[0] == '\0') {
    if (flag->useUnicode) {
      WGETCWD(flag->outPath, sizeof(flag->outPath));
    } else {
      GETCWD(flag->outPath, sizeof(flag->outPath));
    }
  } else {
    if (flag->useUnicode) {
      WCHDIR(flag->outPath);
      if (errno == ENOENT) {
        printf("Can not found directory %s\n", flag->outPath);
        exit(-1);
      }
      WGETCWD(flag->outPath, sizeof(flag->outPath));
    } else {
      CHDIR(flag->outPath);
      if (errno == ENOENT) {
        printf("Can not found directory %s\n", flag->outPath);
        exit(-1);
      }
      GETCWD(flag->outPath, sizeof(flag->outPath));
    }
  }
}

void CheckDir(char *filename, int flen, char *origname, MY_FLAGS *flag)
{
  char *st;
  char buf[FILENAME_MAX];

  if (flag->useData) {
#ifdef WIN32
    st = (char *)_mbschr((unsigned char *)filename, '\\');
    if (st == NULL)  st = (char *)_mbschr((unsigned char *)filename, '/');
#endif
  } else {
    st = strchr(filename, '\\');
    if (st == NULL)  st = strchr(filename, '/');
  }
  if (st) {  // include subdir
    strncpy(buf, filename, st-filename);
    buf[st-filename] = '\0';
    CreateSubDir(buf, st+1, flen, origname, flag);
  } else {
    DecodeWriteFile(filename, flen, origname, flag);
  }
}

void CreateSubDir(char *dirname, char *filename, int flen, char *origname, MY_FLAGS *flag)
{
  if (flag->useUnicode) {
    char uni[FILENAME_MAX];
    int ret;
#ifdef WIN32
    ret = EUCKRtoUTF16(uni, sizeof(uni), dirname, strlen(dirname));
#else
    ret = EUCKRtoUTF8(uni, sizeof(uni), dirname, strlen(dirname));
#endif
    if (ret != 0) {
#ifdef WIN32
      printf("can not convert %s to UTF16\n", dirname);
#else
      printf("can not convert %s to UTF8\n", dirname);
#endif
      MKDIR(dirname);
      CHDIR(dirname);
    } else {
      WMKDIR(uni);
      WCHDIR(uni);
    }
  } else {
    MKDIR(dirname);
    CHDIR(dirname);
  }
  CheckDir(filename, flen, origname, flag);
}

void DecodeWriteFile(char *filename, int flen, char *origname, MY_FLAGS *flag)
{
  if (CheckFileExist(origname, filename, flag)) return;

  pak1->Uncomp2tmp(origname);

  FILE *fp;
  if (flag->useUnicode) {
    char uni[FILENAME_MAX];
    int ret;
#ifdef WIN32
    ret = EUCKRtoUTF16(uni, sizeof(uni), filename, strlen(filename));
#else
    ret = EUCKRtoUTF8(uni, sizeof(uni), filename, strlen(filename));
#endif
    if (ret != 0) {
#ifdef WIN32
      printf("can not convert %s to UTF16\n", filename);
#else
      printf("can not convert %s to UTF8\n", filename);
#endif
      fp = fopen(filename, "wb");
    } else {
      fp = WFOPEN(uni, "wb");
    }
  } else {
    fp = fopen(filename, "wb");
  }
  if (fp == NULL) {
    printf("can not create %s.\n", filename);
  } else {
    fwrite(pak1->tdata, 1, pak1->tlen, fp);
    fclose(fp);
  }
}

bool INLINE CheckFileExist(char *origname, char *filename, MY_FLAGS *flag)
{
  if (flag->forceWrite == false) {
    FILE *fp;

    if (flag->useUnicode) {
      char uni[FILENAME_MAX];
      int ret;
#ifdef WIN32
      ret = EUCKRtoUTF16(uni, sizeof(uni), filename, strlen(filename));
#else
      ret = EUCKRtoUTF8(uni, sizeof(uni), filename, strlen(filename));
#endif
      if (ret != 0) {
#ifdef WIN32
        printf("can not convert %s to UTF16\n", filename);
#else
        printf("can not convert %s to UTF8\n", filename);
#endif
        fp = fopen(filename, "rb");
      } else {
        fp = WFOPEN(uni, "rb");
      }
    } else {
      fp = fopen(filename, "rb");
    }
    if (fp != NULL) { //if exist file
      fclose(fp);
      printf("%s is exist.\n", origname);
      return true;
    }
  }
  return false;
}

void ParseOptionAddFile(int argc, char **argv, MY_FLAGS *flag)
{
  char fnbuf[FILENAME_MAX];

  int c;
  extern char *optarg;
  extern int optind;
  int errflg = 0;

  if (argc == 1) PrintUsageExit(argv[0]);

  InitFlags(flag);
#ifdef VIOLATION_CODE
  while ((c = my_getopt(argc, argv, "9Efkhiutps:S:CPa:l:m:o:T:")) != -1) {
#else
  while ((c = my_getopt(argc, argv, "EfkhiutCl:m:o:T:")) != -1) {
#endif // VIOLATION_CODE
    switch (c) {
    case '9':
#ifdef WIN32
      flag->patch98 = true;
#endif
      break;
    case 'E':
      flag->easymode = true;
      break;
    case 'a':
      flag->appendPatch = true;
      strncpy(flag->fnPatch, optarg, sizeof(flag->fnPatch));
      break;
    case 'f':
      flag->forceWrite = true;
      break;
    case 'k':
      flag->useUnicode = true;
      break;
    case 'h':
      PrintUsageExit(argv[0]);
      break;
    case 'i':
      flag->FN_ID = true;
      break;
    case 'o':
      strncpy(flag->outPath, optarg, sizeof(flag->outPath));
#ifdef WIN32
      GetFullPathName(flag->outPath, sizeof(fnbuf), fnbuf, NULL);
#else
      if (flag->outPath[0] != '/') {
        SNPRINTF(fnbuf, sizeof(fnbuf), "./%s", flag->outPath);
      } else {
        strncpy(fnbuf, flag->outPath, sizeof(fnbuf));
      }
#endif
      strncpy(flag->outPath, fnbuf, sizeof(flag->outPath));
      break;
    case 'l':
      listFilename = (char *)malloc(strlen(optarg)+1);
      strcpy(listFilename, optarg);
      break;
    case 'm':
      flag->useMatch = true;
      strncpy(flag->strMatch, optarg, sizeof(flag->strMatch));
      break;
    case 's':
      flag->subtract = 2;
      strcpy(flag->subFilename, optarg);
#ifdef WIN32
      GetFullPathName(flag->subFilename, sizeof(fnbuf), fnbuf, NULL);
#else
      if (flag->subFilename[0] != '/') {
        SNPRINTF(fnbuf, sizeof(fnbuf), "./%s", flag->subFilename);
      } else {
        strncpy(fnbuf, flag->subFilename, sizeof(fnbuf));
      }
#endif
      strncpy(flag->subFilename, fnbuf, sizeof(flag->subFilename));
      break;
    case 'S':
      flag->subtract = 1;
      strcpy(flag->subFilename, optarg);
#ifdef WIN32
      GetFullPathName(flag->subFilename, sizeof(fnbuf), fnbuf, NULL);
#else
      if (flag->subFilename[0] != '/') {
        SNPRINTF(fnbuf, sizeof(fnbuf), "./%s", flag->subFilename);
      } else {
        strncpy(fnbuf, flag->subFilename, sizeof(fnbuf));
      }
#endif
      strncpy(flag->subFilename, fnbuf, sizeof(flag->subFilename));
      break;
    case 't':
      flag->listOnly = true;
      break;
    case 'T':
      vsFilename = (char *)malloc(strlen(optarg)+1);
      strcpy(vsFilename, optarg);
      break;
    case 'u':
      flag->useData = true;
      break;
    case 'p':
      flag->makePatch = 2;
      break;
    case 'P':
      flag->makePatch = 1;
      break;
    case 'C':
      flag->chrviewer = true;
      break;
    default:
      errflg++;
      break;
    }
  }

#ifdef linux
  flag->useData = false;
#endif

  if (flag->useData) {
    flag->useUnicode = false;
  }

  if (flag->makePatch)  return;

  for (int i=optind; i<argc; i++) {
#ifdef WIN32
    GetFullPathName(argv[i], sizeof(fnbuf), fnbuf, NULL);
#else
    if (argv[i][0] != '/') {
      char fnbuf2[FILENAME_MAX];
      GETCWD(fnbuf2, sizeof(fnbuf2));
      SNPRINTF(fnbuf, sizeof(fnbuf), "%s/%s", fnbuf2, argv[i]);
    } else {
      strcpy(fnbuf, argv[i]);
    }
#endif
    pak1->AddPakFile(fnbuf);
  }

  if (flag->patch98) {
    version_patch98 = flag->makePatch;
    if (version_patch98 == 0) {
      version_patch98 = 1;
    }
  }
}

void InitFlags(MY_FLAGS *flag)
{
  flag->subtract = 0;
  flag->easymode = false;
  flag->chrviewer = false;
  flag->patch98 = false;
  flag->FN_ID = false;
  flag->forceWrite = false;
  flag->outPath[0] = '\0';
  flag->useData = false;
  flag->useMatch = false;
  flag->listOnly = false;
  flag->useUnicode = false;
  flag->makePatch = 0;
  flag->appendPatch = false;
}

void PrintUsageExit(char *pname)
{
  printf("This program extract PAK/GRF/GPF file of **. Build %s %s.\n\n", __DATE__, __TIME__);
  printf("Usage:\n\t%s [-fituk] [-m STR] [-o DIR] PAK/GRF/GPF_file1 ...\n", pname);
  printf("\t  If you execute with no options,\n");
  printf("\t  extract all files in current directory.\n");
#ifdef VIOLATION_CODE
  printf("\tor, %s -p -k or -P -k\n", pname);
#ifdef WIN32
  printf("\tor, %s -9 [[-p or -P] -l listfilename -T taiofilename]\n", pname);
#endif // WIN32
  printf("\tor, %s -a GPF_file GRF_file\n", pname);
#endif // VIOLATION_CODE
  printf("\tor, %s -C GRF/GPF_file\n", pname);
  printf("\tor, %s -E PAK/GRF/GPF_file\n", pname);
#ifdef VIOLATION_CODE
  printf("\tor, %s -s|-S GRF_orig GRF/GPF_file\n\n", pname);
#endif // VIOLATION_CODE
  printf("\tor, %s -h\n\n", pname);
  printf("Options:\n");
#ifdef VIOLATION_CODE
#ifdef WIN32
  printf("  -9        Can make patch on Win98/ME.\n");
#endif
  printf("  -a GPF    Patch GPF file to GRF file.\n");
#endif // VIOLATION_CODE
  printf("  -f        Force extract. If exist file of same filename,\n");
  printf("            old file is overwritten.\n");
  printf("  -i        If this option is setted in extract, filename chages to ID.\n");
  printf("            If you use Win98/ME and extract GRF/GPF, you must set this opt.\n");
  printf("  -k        Extract/Make in Hangle(use Unicode.\n");
  printf("             UTF-16LE in Windows. UTF-8 in Linux)\n");
  printf("            If use -u and -k same time, -kk is NOT effective.\n");
  printf("  -m STR    Extract or list ONLY files that\n");
  printf("            matched a part of filename with STRing.\n");
  printf("  -u        Use extracted data in **.\n");
  printf("  -o DIR    Extract in indicated DIRectory.\n");
  printf("  -t        List files. NOT extract.\n");
#ifdef VIOLATION_CODE
  printf("  -p -k     Make patch file \"update.gpf\" in version 2.\n");
  printf("  -P -k     Make patch file \"update.gpf\" in version 1.\n");
  printf("            It is needed files are in \"data\" directory.\n");
#endif // VIOLATION_CODE
  printf("  -C        Character viewer mode.\n");
  printf("  -E        Easy mode. But ...\n");
#ifdef VIOLATION_CODE
  printf("  -s GRF_O  Subtruct GRF/GPF from GRF_O with data comparison.\n");
  printf("  -S GRF_O  Subtrcut GRF/GPF from GRF_O without data comparison.\n\n");
#endif // VIOLATION_CODE
  printf("  -h        Show this.\n\n");
  exit(0);
}

void WriteFileList(MY_FLAGS *flag)
{
  for (int i=0; i<pak1->NumEntry; i++) {
    if (IsStrMatch(i, flag) == false) continue;
    if (flag->useMatch) {
      if (strstr(pak1->Entries[i].fn, flag->strMatch) == NULL) continue;
    }
    printf("%d\t%s\n", i+1, pak1->Entries[i].fn);
  }
}

void WriteFileListMsgFmt(MY_FLAGS *flag)
{
  FILE *fp;
  char buf[1024], buf2[1024];

  fp = fopen("filelist.txt", "wb");
  for (int i=0; i<pak1->NumEntry; i++) {
    if (IsStrMatch(i, flag) == false) continue;
    SNPRINTF(buf, sizeof(buf), "#%5d : \n#msgid \"%s\"\nmsgid \"", i+1, pak1->Entries[i].fn);
    for (unsigned int j=0; j<strlen(pak1->Entries[i].fn); j++) {
      SNPRINTF(buf2, sizeof(buf2), "\\%o", (unsigned char)(pak1->Entries[i].fn[j]));
      if (sizeof(buf) < strlen(buf)+strlen(buf2)) {
        printf("string buffer is full. At %d", __LINE__);
        exit(-1);
      }
      strcat(buf, buf2);
    }
    if (sizeof(buf) > strlen(buf)+15)  {
      strcat(buf, "\"\nmsgstr \"\"\n\n");
    }
    fwrite(buf, 1, strlen(buf), fp);
  }
  fclose(fp);
}

void WriteHangleHTML(MY_FLAGS *flag)
{
  FILE *fp;
  char buf[1024];

  fp = fopen("hangle.html", "wb");
  fprintf(fp, "<html>\n<head>\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=euc-kr\">");
  fprintf(fp, "</head>\n<body>\n");
  for (int i=0; i<pak1->NumEntry; i++) {
    if (IsStrMatch(i, flag) == false) continue;
    SNPRINTF(buf, sizeof(buf), "%5d : %s<br>\n", i+1, pak1->Entries[i].fn);
    fwrite(buf, strlen(buf), 1, fp);
  }
  fprintf(fp, "</body>\n</html>\n");
  fclose(fp);
}

void WritePosLen(char *filename)
{
  for (int i=0; i<pak1->NumEntry; i++) {
    if (strcmp(pak1->Entries[i].fn, filename) == 0) {
      printf("srcpos = %08x, srclen = %08x, declen = %08x\n", pak1->Entries[i].srcpos,
             pak1->Entries[i].srclen_aligned, pak1->Entries[i].declen);
    }
  }
}

bool INLINE IsStrMatch(int id, MY_FLAGS *flag)
{
  if (flag->useMatch) {
    if (strstr(pak1->Entries[id].fn, flag->strMatch) == NULL) return false;
  }
  return true;
}

#ifdef VIOLATION_CODE
#ifdef WIN32
void AddPatchDir(char *dir)
{
  struct _finddata_t c_file;
  long hFile;
  char curfn[FILENAME_MAX];
  char newdir[FILENAME_MAX];

  SNPRINTF(curfn, sizeof(curfn), "%s\\*.*", dir);
  if ((hFile = (long)_findfirst(curfn, &c_file)) == -1L) return;
  while (_findnext(hFile, &c_file) == 0) {
    if (c_file.attrib & _A_SUBDIR) {
      if (strcmp(".", c_file.name) == 0 || strcmp("..", c_file.name) == 0)
        continue;
      SNPRINTF(newdir, sizeof(newdir), "%s\\%s", dir, c_file.name);
      AddPatchDir(newdir);
    } else {
      //printf("%s\\%s\n", dir, c_file.name);
      SNPRINTF(curfn, sizeof(curfn), "%s\\%s", dir, c_file.name);
      AddPatchReadFile(curfn);
    }
  }
  _findclose(hFile);
}

void WAddPatchDir(wchar_t *dir)
{
  struct _wfinddata_t c_file;
  long hFile;
  wchar_t curfn[FILENAME_MAX];
  wchar_t newdir[FILENAME_MAX];

  _snwprintf(curfn, sizeof(curfn), _TEXT("%s\\*.*"), dir);
  if ((hFile = (long)_wfindfirst(curfn, &c_file)) == -1L) return;
  while (_wfindnext(hFile, &c_file) == 0) {
    if (c_file.attrib & _A_SUBDIR) {
      if (wcscmp(_TEXT("."), c_file.name) == 0 || wcscmp(_TEXT(".."), c_file.name) == 0)
        continue;
      _snwprintf(newdir, sizeof(newdir), _TEXT("%s\\%s"), dir, c_file.name);
      WAddPatchDir(newdir);
    } else {
      char na[FILENAME_MAX];
      _snwprintf((wchar_t *)na, sizeof(na), _TEXT("%s\\%s"), dir, c_file.name);
      WAddPatchReadFile(na);
    }
  }
  _findclose(hFile);
}
#endif // WIN32

#ifdef linux
void AddPatchDir(char *dir, char *dosname)
{
  DIR *dp;
  struct dirent *entry;
  struct stat statbuf;
  char newdir[FILENAME_MAX];
  char newdird[FILENAME_MAX];

  if ((dp = opendir(dir)) == NULL) return;
  while ((entry = readdir(dp)) != NULL) {
    if (entry->d_type == DT_DIR) {
      if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
        continue;
      SNPRINTF(newdir, sizeof(newdir), "%s/%s", dir, entry->d_name);
      SNPRINTF(newdird, sizeof(newdird), "%s\\%s", dosname, entry->d_name);
      AddPatchDir(newdir, newdird);
    } else {
      //printf("%s/%s\n", dir, entry->d_name);
      SNPRINTF(newdir, sizeof(newdir), "%s/%s", dir, entry->d_name);
      SNPRINTF(newdird, sizeof(newdird), "%s\\%s", dosname, entry->d_name);
      AddPatchReadFile(newdir, newdird);
    }
  }
  closedir(dp);
}
#endif // linux

#ifdef WIN32
void AddPatchReadFile(char *filename)
{
  FILE *fp;
  BYTE *dat;
  int len;

  fp = fopen(filename, "rb");
  fseek(fp, 0, SEEK_END);
  len = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  dat = (BYTE *)malloc(len);
  fread(dat, 1, len, fp);
  fclose(fp);

  printf("%s add.\n", filename);
  pak1->MakePatchAdd(filename, dat, len);

  free(dat);
}
#else
void AddPatchReadFile(char *filename, char *dosname)
{
  FILE *fp;
  BYTE *dat;
  int len;

  fp = fopen(filename, "rb");
  fseek(fp, 0, SEEK_END);
  len = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  dat = (BYTE *)malloc(len);
  fread(dat, 1, len, fp);
  fclose(fp);

  printf("%s add.\n", filename);
  pak1->MakePatchAdd(dosname, dat, len);

  free(dat);
}
#endif
#ifdef WIN32
void WAddPatchReadFile(char *filename)
{
  FILE *fp;
  BYTE *dat;
  int len;
  char kr[FILENAME_MAX];

  fp = _wfopen((wchar_t *)filename, _TEXT("rb"));
  fseek(fp, 0, SEEK_END);
  len = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  dat = (BYTE *)malloc(len);
  fread(dat, 1, len, fp);
  fclose(fp);

#ifdef WIN32
  UTF16toEUCKR(kr, sizeof(kr), filename, (wcslen((wchar_t *)filename)+1)*2);  // fix me '*2'. '+1' is for \0
#else
  UTF8toEUCKR(kr, sizeof(kr), filename, (wcslen((wchar_t *)filename)+1)*2);  // fix me '*2'. '+1' is for \0
#endif
  printf("%s add.\n", kr);
  pak1->MakePatchAdd(kr, dat, len);

  free(dat);
}
#endif // WIN32
#endif // VIOLATION_CODE

void SetTextDomain(char *progname)
{
  setlocale(LC_ALL, getenv("LANG"));
#ifndef DISTRIBUTE
  strcpy(NativeLANG, locale_charset());
#endif

#if WIN32
  set_program_name(progname);
#endif

  char *ddir;

  ddir = getenv("TEXTDOMAINDIR");
  if ((ddir = getenv("TEXTDOMAINDIR")) != NULL) {
  } else {
#if WIN32
    bindtextdomain(MYTEXTDOMAIN, relocate(".\\share\\locale\\"));
#else
    bindtextdomain(MYTEXTDOMAIN, "/usr/local/share/locale");
#endif
  }
  textdomain(MYTEXTDOMAIN);

#ifdef WIN32
#ifdef USE_GTK
#ifdef DISTRIBUTE
  bind_textdomain_codeset(MYTEXTDOMAIN, "UTF-8");
#endif
#endif
#endif

#ifndef WIN32
#ifdef DISTRIBUTE
  bind_textdomain_codeset(MYTEXTDOMAIN, "UTF-8");
#endif
#endif

}


bool GetAgreement(void)
{
  agreementstring = (char *)malloc(AGREEMENTSTRING_ORIG_LEN+1);
  uLong realdestlen;
  uncompress((Bytef *)agreementstring, &realdestlen,
             (const Bytef *)agreementstringcomp, AGREEMENTSTRING_COMP_LEN);
  agreementstring[AGREEMENTSTRING_ORIG_LEN] = '\0';

  printf(_(agreementstring));
  free(agreementstring);

  char buf[16];
  fgets(buf, sizeof(buf), stdin);

  bool ret;
  if (strncmp(buf, "yes", 3) == 0) {
    ret = true;
  } else {
    ret = false;
  }

  return ret;
}

void GetPrefFileName(char *fname, int fname_size)
{
  char *env_home = getenv("HOME");
  if (env_home) {
    strcpy(fname, env_home);
#ifdef WIN32
    strcat(fname, "\\");
#else
    strcat(fname, "/");
#endif
    strcat(fname, "."THIS_APP_NAME".pref");
  } else {
#ifdef WIN32
    GetModuleFileName(NULL, fname, fname_size);
    for (int i=(int)strlen(fname); i >= 0; i--) {
      if (fname[i] == '\\') {
        fname[i] = 0;
        break;
      }
    }
    strcat(fname, "\\."THIS_APP_NAME".pref");
#else
    strcpy(fname, "."THIS_APP_NAME".pref");
#endif
  }
}

void ReadPreference()
{
  agreement = false;

  char fname[FILENAME_MAX];
  GetPrefFileName(fname, sizeof(fname));

  FILE *fp;
  if ((fp = fopen(fname, "r")) == NULL) return;

  char buf[256];
  // 1st line is agreement
  fgets(buf, sizeof(buf), fp);
  atoi(buf) ? agreement = true : agreement = false;

  fclose(fp);
}

void WritePreference()
{
  char fname[FILENAME_MAX];
  GetPrefFileName(fname, sizeof(fname));

  FILE *fp;
  if ((fp = fopen(fname, "w")) == NULL) return;

  // 1st line is agreement
  if (agreement) {
    fprintf(fp, "1\n");
  } else {
    fprintf(fp, "0\n");
  }

  fclose(fp);
}
