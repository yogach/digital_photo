#ifndef __FILE_H___
#define __FILE_H___

#include <stdio.h>


typedef struct MapFile{
  char FileName[256];      //文件名
//  int iFd;
  FILE * tFp;              /* 文件句柄 */
  int iFileSize;           //文件大小
  unsigned char *pucFileMapMem;//使用mmap函数映射文件得到的内存
}T_MapFile,*PT_MapFile;


/* 文件类别 */
typedef enum {
	FILETYPE_DIR = 0,  /* 目录 */
	FILETYPE_FILE,     /* 文件 */
}E_FileType;

/* 目录里的内容 */
typedef struct DirContent {
	char strName[256];     /* 名字 */
	E_FileType eFileType;  /* 类别 */	
}T_DirContent, *PT_DirContent;



int MapFile(PT_MapFile ptFileMap);
void  unMapFile(PT_MapFile ptFileMap);
int GetDirContents ( char* strDirName, PT_DirContent** pptDirContents, int* piNumber );
void FreeDirContents ( PT_DirContent* aptDirContents, int iNumber );
int GetFilesIndir(char *strDirName, int *piStartNumberToRecord, int *piCurFileNumber, int *piFileCountHaveGet, int iFileCountTotal, char apstrFileNames[][256]);




#endif //__FILE_H___
