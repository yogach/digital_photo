
#ifndef __PIC_OPERATION__
#define __PIC_OPERATION__

typedef struct PhotoDesc
{
  int iHigh;
  int iWidth;
  int iBpp;
  int iLineBytes;
  unsigned char *aucPhotoDataStart;

}T_PhotoDesc,*PT_PhotoDesc;


typedef struct PicFileParser 
{
  char* name;
  int (*isSupport)(unsigned char *FileHead);
  int (*GetPixelDatas)(unsigned char *FileHead,PT_PhotoDesc ptPhotoDesc);
  int (*FreePixelDatas)(PT_PhotoDesc ptPhotoDesc);

}T_PicFileParser,*PT_PicFileParser;



#endif //__PIC_OPERATION__

