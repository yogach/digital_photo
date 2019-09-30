#ifndef __FILE_H___
#define __FILE_H___

#include <stdio.h>


typedef struct MapFile{
  char FileName[256];      //�ļ���
//  int iFd;
  FILE * tFp;              /* �ļ���� */
  int iFileSize;           //�ļ���С
  unsigned char *pucFileMapMem;//ʹ��mmap����ӳ���ļ��õ����ڴ�
}T_MapFile,*PT_MapFile;


/* �ļ���� */
typedef enum {
	FILETYPE_DIR = 0,  /* Ŀ¼ */
	FILETYPE_FILE,     /* �ļ� */
}E_FileType;

/* Ŀ¼������� */
typedef struct DirContent {
	char strName[256];     /* ���� */
	E_FileType eFileType;  /* ��� */	
}T_DirContent, *PT_DirContent;



int MapFile(PT_MapFile ptFileMap);
void  unMapFile(PT_MapFile ptFileMap);
int GetDirContents ( char* strDirName, PT_DirContent** pptDirContents, int* piNumber );
void FreeDirContents ( PT_DirContent* aptDirContents, int iNumber );
int GetFilesIndir(char *strDirName, int *piStartNumberToRecord, int *piCurFileNumber, int *piFileCountHaveGet, int iFileCountTotal, char apstrFileNames[][256]);




#endif //__FILE_H___
