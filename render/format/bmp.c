
#include <stdio.h>
#include <config.h>
#include <pic_operation.h>
#include <stdlib.h>
#include <string.h>


// __attribute__((packed))的作用是取消结构体对齐 大小等于实际类型的大小
typedef struct tagBITMAPFILEHEADER
{													/* bmfh */
unsigned short	bfType;
unsigned long	bfSize; 							//位图文件大小
unsigned short	bfReserved1;
unsigned short	bfReserved2;
unsigned long	bfOffBits;							//从文件头开始的到实际图像数据之间的偏移量
} __attribute__ ((packed))BITMAPFILEHEADER;


typedef struct tagBITMAPINFOHEADER
{													/* bmih */
unsigned long	biSize;
unsigned long	biWidth;							//图像的宽度
unsigned long	biHeight;							//图像的高度还有一个用途是指明图片是正向还是倒向，如果为负代表是正向
unsigned short	biPlanes;
unsigned short	biBitCount; 						//像素 代表一个像素数据用几个字节表示 
unsigned long	biCompression;
unsigned long	biSizeImage;
unsigned long	biXPelsPerMeter;
unsigned long	biYPelsPerMeter;
unsigned long	biClrUsed;
unsigned long	biClrImportant;
} __attribute__ ((packed))BITMAPINFOHEADER;


T_PicFileParser g_tBMPFileParser =
{
	.name = "bmp",
	.isSupport = isSupportBMP,
	.GetPixelDatas = GetPixelDatasFrmBMP,
	.FreePixelDatas = FreePixelDatasForBMP,
};



static int isSupportBMP (unsigned char * FileHead)
{
	//如果文件开头为424d 代表该文件为bmp格式
	if (FileHead[0] != 0x42 || FileHead[1] != 0x4d)
		return 0;

	else 
		return 1;
}


static int CovertOneLine (int iWidth,int iSrcBpp,int iDstBpp,unsigned char * pudSrcDatas,unsigned char * pudDstDatas)
{




}


static int GetPixelDatasFrmBMP (unsigned char * FileHead,PT_PhotoDesc ptPhotoDesc,int iexpBpp)
{
	BITMAPFILEHEADER * pBMPHead;
	BITMAPINFOHEADER * PBMPInfo;
	int iWidth;
	int iHeight;
	int iBMPBpp;
	int PixelDir = 0;
	int iLineWidthReal;
	int iLineWidthAlign;
	unsigned char * pucSrc;
	unsigned char * pucDest;
	int y;

	pBMPHead = (BITMAPFILEHEADER *) (FileHead);

	PBMPInfo = (BITMAPINFOHEADER *) (FileHead + sizeof (BITMAPFILEHEADER));

	iWidth = PBMPInfo.biWidth;						//获取长宽高 像素
	iHeight = PBMPInfo.biHeight;
	iBMPBpp = PBMPInfo.biBitCount;

	if (iHeight < 0) //如果高度数据小于 表示BMP数据存储方式为正向
	{
		PixelDir = 1;
		iHeight = ~iHeight;
		iHeight += 1;

	}

	if (iBMPBpp != 24)
	{
		DBG_PRINTF ("bmp file bpp not 24\r\n");
		return - 1;
	}



	ptPhotoDesc->iHigh = iHeight;
	ptPhotoDesc->iWidth = iWidth;
	ptPhotoDesc->iBpp = iexpBpp;
	ptPhotoDesc->iLineBytes = iWidth * ptPhotoDesc.iBpp / 8;

	ptPhotoDesc->aucPhotoData = malloc (iWidth * iHeight * ptPhotoDesc->iBpp / 8);

	if (ptPhotoDesc->aucPhotoData == NULL)
	{
		DBG_PRINTF ("malloc PhotoData Error!\r\n");
		return - 1;
	}

	iLineWidthReal = iWidth * iBMPBpp / 8;

	/* BMP数据的保持每行是4的倍数 所以行数字节需要向4取整 */
	iLineWidthAlign = (iLineWidthReal + 3) &~0x3;

	pucSrc = FileHead + pBMPHead->bfOffBits;		//得到BMP文件像素数据的起始位置

	//如果数据不是正向排列需要得到最后一行的开始位置
	if (!PixelDir)
	{
		pucSrc = pucSrc + (iHeight - 1) *iLineWidthAlign;
	}

	pucDest = ptPhotoDesc->aucPhotoData;			//得到需要拷贝数据的指针


	//一行一行拷贝数据
	for (y = 0; y < iHeight; y++)
	{

		CovertOneLine (iWidth,iBMPBpp,ptPhotoDesc->iBpp,pucSrc,pucDest);

		if (!PixelDir)
			pucSrc -= iLineWidthAlign;
		else 
			pucSrc += iLineWidthAlign;

		pucDest += iLineWidthReal;

	}

}


static int FreePixelDatasForBMP (PT_PhotoDesc ptPhotoDesc)
{
	//释放分配的内存
	free (ptPhotoDesc->aucPhotoDataStart);
}





