
#ifndef __PIC_OPERATION__
#define __PIC_OPERATION__

typedef struct PhotoDesc
{
  int iHigh;
  int iWidth;
  int iBpp;
  int iLineBytes;
  unsigned char *aucPhotoData;

}T_PhotoDesc,*PT_PhotoDesc;


typedef struct PicFileParser 
{
  char* name;
  int (*isSupport)(unsigned char *FileHead);
  int (*GetPixelDatas)(unsigned char *FileHead,PT_PhotoDesc ptPhotoDesc,int iexpBpp);
  int (*FreePixelDatas)(PT_PhotoDesc ptPhotoDesc);
  struct PicFileParser *ptNext;
}T_PicFileParser,*PT_PicFileParser;

int RegisterPicFileParser(PT_PicFileParser ptPicFileParser);
void ShowPicFileParser(void);
PT_PicFileParser GetPicFileParser(char *pcName);
int PicFileParserInit(void);
int BMPInit(void);
int PicMerge(int iX, int iY, PT_PhotoDesc ptSmallPic, PT_PhotoDesc ptBigPic);

int PicZoom (PT_PhotoDesc ptOriginPic,PT_PhotoDesc ptZoomPic,int scale);







#endif //__PIC_OPERATION__

