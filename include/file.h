#ifndef __FILE_H___
#define __FILE_H___

#include <stdio.h>


typedef struct MapFile{
  char FileName[128];
//  int iFd;
  FILE * tFp;              /* нд╪Ч╬Д╠З */
  int iFileSize;
  unsigned char *pucFileMapMem;
}T_MapFile,*PT_MapFile;


int MapFile(PT_MapFile ptFileMap);
void  unMapFile(PT_MapFile ptFileMap);


#endif //__FILE_H___
