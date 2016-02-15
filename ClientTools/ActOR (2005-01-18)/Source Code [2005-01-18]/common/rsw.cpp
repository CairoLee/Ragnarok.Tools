#include <string.h>

#include "rsw.h"

CRSW::CRSW(BYTE *dat, int len)
{
  if (dat == NULL)  return;
  objs = NULL;
  nodes = NULL;

  parseOK = false;
  if (ParseData(dat, len) == true) {
    parseOK = true;
  }
}

CRSW::~CRSW(void)
{
  for (int i=0; i<numofobj; i++) {
    delete objs[i].rt1;
  }
  if (objs)  delete objs;
  if (nodes)  delete nodes;
}

bool CRSW::ParseData(BYTE *dat, int len)
{
  BYTE *p = dat;
  BYTE *p_start = p;

  if (strncmp((const char *)p, "GRSW", 4) != 0)  return false;
  memcpy(&header, p, sizeof(RswHeader));
  p += sizeof(RswHeader);

  WORD vertmp;
  vertmp = (header.version & 0xFF00) >> 8 | (header.version & 0x00FF) << 8;
  if (/*vertmp < 0x0109 ||*/ vertmp > 0x0201) {
    printf("Can't parse this version %04x\n", vertmp);
    return false;
  }
  if (vertmp < 0x0109) {
    p -= 0x18;
  }

  numofobj = *(DWORD *)p;  p += 4;
  objs = new RswObj[numofobj];
  nodes = new RswNode[MAX_RSW_NODE];

  for (int i=0; i<numofobj; i++) {
    objs[i].type = *(DWORD *)p;  p += 4;
    switch (objs[i].type) {
    case 1:
      objs[i].rt1 = new RswType1;
      memcpy(objs[i].rt1, p, sizeof(RswType1));
      p += sizeof(RswType1);
      break;
    case 2:
      objs[i].rt2 = new RswType2;
      memcpy(objs[i].rt2, p, sizeof(RswType2));
      p += sizeof(RswType2);
      if (header.version == 0x0601)  p += 0x64;
      break;
    case 3:
      objs[i].rt3 = new RswType3;
      memcpy(objs[i].rt3, p, sizeof(RswType3));
      p += sizeof(RswType3);
      if (header.version == 0x0601) {
        p += 0x84;
      } else if (header.version == 0x0901) {
        p -= 4;
      }
      break;
    case 4:
      objs[i].rt4 = new RswType4;
      memcpy(objs[i].rt4, p, sizeof(RswType4));
      p += sizeof(RswType4);
      if (header.version == 0x0601)  p += 0x50;
      break;
    case 5:
      if (header.version == 0x0601) {
        p += 0xB4;
      }
      break;
    default:
      printf("can't parse type %d @ %08X\n", objs[i].type, p-p_start);
      objs[i].rt1 = new RswType1;
      p += sizeof(RswType1);
      break;
    }
  }
  if (header.version == 0x0102) {
    for (int i=0; i<MAX_RSW_NODE; i++, p+=sizeof(float)*3) {
      memcpy(&nodes[i].xyz, p, sizeof(float)*3);
    }
  }
  printf("rest byte %d.", len-(p-p_start));

  return true;
}

void CRSW::WriteDelType1(char *fn)
{
  FILE *fp;
  fp = fopen(fn, "wb");

  fwrite(&header, 1, sizeof(header), fp);

  int numdel1 = 0;
  for (int i=0; i<numofobj; i++) {
    if (objs[i].type != 1)  numdel1++;
  }

  fwrite(&numdel1, 1, sizeof(DWORD), fp);
  for (int i=0; i<numofobj; i++) {
    switch (objs[i].type) {
    case 1:
      /*
      fwrite(&objs[i].type, 1, sizeof(DWORD), fp);
      fwrite(objs[i].rt1, 1, sizeof(RswType1), fp);
      */
      break;
    case 2:
      fwrite(&objs[i].type, 1, sizeof(DWORD), fp);
      fwrite(objs[i].rt2, 1, sizeof(RswType2), fp);
      break;
    case 3:
      fwrite(&objs[i].type, 1, sizeof(DWORD), fp);
      fwrite(objs[i].rt3, 1, sizeof(RswType3), fp);
      break;
    case 4:
      fwrite(&objs[i].type, 1, sizeof(DWORD), fp);
      fwrite(objs[i].rt4, 1, sizeof(RswType4), fp);
      break;
    }
  }

  if (header.version == 0x0102) {
    for (int i=0; i<MAX_RSW_NODE; i++) {
      fwrite(&nodes[i].xyz, 3, sizeof(float), fp);
    }
  }

  fclose(fp);
}

void CRSW::WriteAsText_Header(FILE *fp)
{
  fwrite(header.magic, 1, sizeof(header.magic), fp);  fprintf(fp, "\n");
  fprintf(fp, "%04X\n", header.version);
  fwrite(header.ini_filename, 1, sizeof(header.ini_filename), fp);  fprintf(fp, "\n");
  fwrite(header.gnd_filename, 1, sizeof(header.gnd_filename), fp);  fprintf(fp, "\n");
  fwrite(header.gat_filename, 1, sizeof(header.gat_filename), fp);  fprintf(fp, "\n");
  fwrite(header.scr_filename, 1, sizeof(header.scr_filename), fp);  fprintf(fp, "\n");
  fprintf(fp, "%08X %08X %08X %08X\n", header.xxx1[0], header.xxx1[1], header.xxx1[2], header.xxx1[3], header.xxx1[4]);
  fprintf(fp, "%08X %08X %08X\n", header.xxx2[0], header.xxx2[1], header.xxx2[2]);
  for (int i=0; i<2; i++) {
    for (int j=0; j<12; j++) {
      fprintf(fp, "%02X", header.xxxb[i][j]);
    }
    fprintf(fp, "\n");
  }
  fprintf(fp, "%08X %08X %08X %08X %08X\n", header.xxx3[0], header.xxx3[1], header.xxx3[2], header.xxx3[3], header.xxx3[4]);
}

void CRSW::WriteAsText(char *fn)
{
  FILE *fp;
  fp = fopen(fn, "w");

  WriteAsText_Header(fp);
  fprintf(fp, "\nnum: %08X\n\n", numofobj);

  for (int i=0; i<numofobj; i++) {
    fprintf(fp, "type: %d\n", objs[i].type);
    switch (objs[i].type) {
    case 1:
      fprintf(fp, "%s\n", objs[i].rt1->objname);
      fprintf(fp, "%08X\n", objs[i].rt1->xxx1);
      fprintf(fp, "%08X\n", *(DWORD *)&objs[i].rt1->xxxf);
      fprintf(fp, "%08X\n", objs[i].rt1->xxx2);
      for (int j=0; j<sizeof(objs[i].rt1->xxx); j++) {
        fprintf(fp, "%02X", objs[i].rt1->xxx[j]);
      }
      fprintf(fp, "\n");
      fprintf(fp, "%f, %f, %f\n", objs[i].rt1->mul_xyz[0],
        objs[i].rt1->mul_xyz[1], objs[i].rt1->mul_xyz[2]);
      break;
    case 2:
      for (int j=0; j<sizeof(objs[i].rt2->xxx); j++) {
        fprintf(fp, "%02X", objs[i].rt2->xxx[j]);
      }
      break;
    case 3:
      for (int j=0; j<sizeof(objs[i].rt3->xxx); j++) {
        fprintf(fp, "%02X", objs[i].rt3->xxx[j]);
      }
      break;
    case 4:
      for (int j=0; j<sizeof(objs[i].rt4->xxx); j++) {
        fprintf(fp, "%02X", objs[i].rt4->xxx[j]);
      }
      break;
    }
    fprintf(fp, "\n\n");
  }

  if (header.version == 0x0102) {
    fprintf(fp, "below UNV format\n");
    fprintf(fp, "    -1\n");
    fprintf(fp, "  2411\n");
    for (int i=0; i<MAX_RSW_NODE; i++) {
      fprintf(fp, "%10d         1         1        11\n", i+1);
      fprintf(fp, "%25.16e%25.16e%25.16e\n", nodes[i].xyz[0], nodes[i].xyz[1], nodes[i].xyz[2]);
    }
    fprintf(fp, "    -1\n");
  }

  fclose(fp);
}

void CRSW::WriteNumOfType(void)
{
  int nt[4];
  for (int i=0; i<4; i++) nt[i] = 0;
  for (int i=0; i<numofobj; i++) {
    switch (objs[i].type) {
      case 1:  nt[0]++; break;
      case 2:  nt[1]++; break;
      case 3:  nt[2]++; break;
      case 4:  nt[3]++; break;
    }
  }
  printf("%d, %d, %d, %d\n", nt[0], nt[1], nt[2], nt[3]);
}

