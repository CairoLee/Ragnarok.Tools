#include <stdio.h>
#include <map>
#include <string>

using namespace std;

typedef map<string, string> StringMap;
void LoadEJ(const string fileName, StringMap *data);

int main(int argc, char **argv)
{
  StringMap ejstr;
  StringMap::iterator si;
  
  LoadEJ("monster_E_J.txt", &ejstr);
  
  si = ejstr.begin();
  while (si != ejstr.end()) {
    printf("msgid \"%s\"\nmsgstr \"%s\"\n\n",
           si->second.c_str(), si->first.c_str());
    si++;
  }
  
  return 0;
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
