#include <stdio.h>
#include <map>
#include <string>

using namespace std;

typedef map<string, string> StringMap;
typedef map<int, string> GeneIntIDMap;

void LoadItemDescData(const string fileName, GeneIntIDMap *data);
void LoadResourceMapData(const string fileName, GeneIntIDMap *data);
void LoadEJ(const string fileName, StringMap *data);

int main(int argc, char **argv)
{
  GeneIntIDMap data, resource;
  GeneIntIDMap::iterator gi, gi2;
  StringMap ejstr;
  StringMap::iterator si;
  char monntmp[50], *find;
  string monname, resname;
  
  LoadItemDescData("num2itemdesctable.txt", &data);
  LoadResourceMapData("num2cardillustnametable.txt", &resource);
  LoadEJ("monster_E_J.txt", &ejstr);
  
  gi = data.begin();
  while (gi != data.end()) {
    int i;
    if (4000 < gi->first && gi->first < 5000) {
      find = strstr(gi->second.c_str(), "ÉJÅ[Éh");
      if (find) {
        for (i=0; i<(int)(find - gi->second.c_str() + 1); i++) {
          monntmp[i] = (char)*(gi->second.c_str()+i);
        }
        monntmp[i-1] = '\0';
      }
      si = ejstr.find(monntmp);
      gi2 = resource.find(gi->first);
      if (si != ejstr.end() && gi2 != resource.end() && strcmp(gi2->second.c_str(), "sorry") != 0) {
        //printf("%d %s %s\n", gi->first, si->second.c_str(), gi2->second.c_str());
        //printf("%d %s %s\n", gi->first, si->first.c_str(), gi2->second.c_str());
        char octs[FILENAME_MAX*4];
        char octtmp[10];
        char resstr[FILENAME_MAX];
        octs[0] = '\0';
        strcpy(resstr, gi2->second.c_str());
        for (int j=0; j<strlen(resstr); j++) {
          sprintf(octtmp, "\\%o", (unsigned char)resstr[j]);
          strcat(octs, octtmp);
        }
        printf("msgid \"%s\"\nmsgstr \"%s\"\n\n", octs, si->first.c_str());
      }
    }
    gi++;
  }
  return 0;
}

void LoadItemDescData(const string fileName, GeneIntIDMap *data)
{
  FILE *fp;
  char buf[256], key[256];
  unsigned int i, mode = 0, kid;
  string stbuf;

  if ((fp = fopen(fileName.c_str(), "rb")) == NULL) return;

  while (!feof(fp)) {
    fgets(buf, sizeof(buf), fp);
    if (buf[0] == '/') continue;
    if (!strcmp(buf, "\n")) continue;
    switch (mode) {
    case 0:  // start
      strcpy(key, buf);
      kid = atoi(buf);
      mode = 1;
      stbuf = "";
      break;
    case 1:  // in desc
      if (buf[0] == '#') {
        (*data)[kid] = stbuf;
        mode = 0;
        break;
      }
      for (i=0; i<strlen(buf); i++) {
        stbuf += buf[i];
      }
      break;
    }
  }
  fclose(fp);
}

void LoadResourceMapData(const string fileName, GeneIntIDMap *data)
{
  FILE *fp;
  char buf[256], key[256];
  string val1;
  unsigned int i, l, kid;

  if ((fp = fopen(fileName.c_str(), "rb")) == NULL) return;

  while (!feof(fp)) {
    fgets(buf, sizeof(buf), fp);
    if (buf[0] == '#' || buf[0] == '/') continue;
    if (!strcmp(buf, "\n")) continue;
    if (buf[strlen(buf)-1] == 0xa) buf[strlen(buf)-1] = '\0';
    for (i = 0; i < strlen(buf); i++) {
      if (buf[i] == '#') break;
    }
    strncpy(key, buf, i);
    key[i] = '\0';
    l = (int)strlen(buf) - i - 2;
    if (i+l < strlen(buf)) {
      val1.assign(&buf[i+1], l+1);
    } else {
      l = 0;
    }
    val1[l] = '\0';
    if (val1[l-1] == '#') val1[l-1] = '\0';
    kid = atoi(key);
    (*data)[kid] = val1;
  }

  fclose(fp);
}

void LoadEJ(const string fileName, StringMap *data)
{
  FILE *fp;
  char buf[256];
  char jname[256], ename[256];
  
  if ((fp = fopen(fileName.c_str(), "r")) == NULL) return;
  
  while (!feof(fp)) {
    fgets(buf, sizeof(buf), fp);
    if (sscanf(buf, "%s%s", ename, jname) == 0) continue;
    jname[strlen(jname)] = '\0';
    (*data)[jname] = ename;
  }
  
  fclose(fp);
}
