#include <stdio.h>
#include <map>
#include <string>

using namespace std;

typedef map<int, string> GeneIntIDMap;

void ResourceDupDelete(GeneIntIDMap *resource);
void LoadIDData(const string fileName, GeneIntIDMap *data);

int main(int argc, char **argv)
{
  GeneIntIDMap real, resource;
  GeneIntIDMap::iterator gi, gi2;
  char resbuf[256], buf[256], buf2[256];
  
  LoadIDData("idnum2itemdisplaynametable.txt", &real);
  LoadIDData("idnum2itemresnametable.txt", &resource);

  gi = real.begin();
  while (gi != real.end()) {  
    printf("msgid \"item%d\"\n", gi->first);
    printf("msgstr \"%s\"\n\n", gi->second.c_str());
    gi++;
  }

  ResourceDupDelete(&resource);
  real.erase(real.find(0));
  gi = real.begin();
  while (gi != real.end()) {
  	// except card
  	if (4000 < gi->first && gi->first < 5000) { gi++; continue; }
    gi2 = resource.find(gi->first);
    if (gi2 == resource.end()) { gi++; continue; }
    strcpy(resbuf, gi2->second.c_str());
    strcpy(buf, "msgid \"");
    for (int j=0; j<strlen(resbuf); j++) {
      sprintf(buf2, "\\%o", (unsigned char)resbuf[j]);
      strcat(buf, buf2);
    }
    /*printf("#ID:%d\n%s\"\n%s\n\n", gi->first, buf, gi->second.c_str());*/
    printf("%s\"\nmsgstr \"%s\"\n\n", buf, gi->second.c_str());
    
    gi++;
  }
  return 0;
}

void ResourceDupDelete(GeneIntIDMap *resource)
{
  int *delarray = new int[resource->size()];
  int deloffs = 0;
  GeneIntIDMap::iterator gi, gi2;
  int i;
  
  for (int i=0; i<resource->size(); i++) {
    delarray[i] = -1;
  }
  
  gi = resource->begin();
  gi++;
  while (gi != resource->end()) {
    gi2 = gi;
    gi2++;
    while (gi2 != resource->end()) {
      if (strcmp(gi->second.c_str(), gi2->second.c_str()) == 0) {
        for (i=0; i<resource->size(); i++) {
          if (delarray[i] == gi2->first) break;
        }
        //not found
        if (i == resource->size()) {
          delarray[deloffs++] = gi2->first;
        }
      }
      gi2++;
    }
    gi++;
  }
  for (int i=0; i<deloffs; i++) {
    resource->erase(resource->find(delarray[i]));
  }
  
  delete delarray;
}


void LoadIDData(const string fileName, GeneIntIDMap *data)
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
