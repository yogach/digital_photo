
#include <stdio.h>
#include <config.h>
#include <pic_operation.h>
#include <stdlib.h>
#include <string.h>


// __attribute__((packed))��������ȡ���ṹ����� ��С����ʵ������
typedef struct tagBITMAPFILEHEADER
{													/* bmfh */
unsigned short	bfType;
unsigned long	bfSize;
unsigned short	bfReserved1;
unsigned short	bfReserved2;
unsigned long	bfOffBits;
} __attribute__ ((packed))

BITMAPFILEHEADER;


typedef struct tagBITMAPINFOHEADER
{													/* bmih */
unsigned long	biSize;
unsigned long	biWidth;
unsigned long	biHeight;
unsigned short	biPlanes;
unsigned short	biBitCount;
unsigned long	biCompression;
unsigned long	biSizeImage;
unsigned long	biXPelsPerMeter;
unsigned long	biYPelsPerMeter;
unsigned long	biClrUsed;
unsigned long	biClrImportant;
} __attribute__ ((packed))

BITMAPINFOHEADER;


T_PicFileParser g_tBMPFileParser =
{
	.name = "bmp",
	.isSupport = isSupportBMP,
	.GetPixelDatas = GetPixelDatasFrmBMP,
	.FreePixelDatas = FreePixelDatasForBMP,
};



int isSupportBMP (unsigned char * FileHead)
{
	//����ļ���ͷΪ424d ������ļ�Ϊbmp��ʽ
	if (FileHead[0] != 0x42 || FileHead[1] != 0x4d) return 0;

	else return 1;
}


int GetPixelDatasFrmBMP (unsigned char * FileHead,PT_PhotoDesc ptPhotoDesc)
{

}


int FreePixelDatasForBMP (PT_PhotoDesc ptPhotoDesc)
{
  free(ptPhotoDesc->aucPhotoDataStart);

}





