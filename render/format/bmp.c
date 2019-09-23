
#include <stdio.h>
#include <config.h>
#include <pic_manager.h>
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




static int isSupportBMP(unsigned char * FileHead)
{
	//如果文件开头为424d 代表该文件为bmp格式
	if (FileHead[0] != 0x42 || FileHead[1] != 0x4d)
		return 0;

	else 
		return 1;
}


//根据bpp将图片数据拷贝到指定空间
static int CovertOneLine(int iWidth,int iSrcBpp,int iDstBpp,unsigned char * pudSrcDatas,unsigned char * pudDstDatas)
{
	unsigned int dwRed;
	unsigned int dwGreen;
	unsigned int dwBlue;
	unsigned int dwColor;
    int i,pos=0;

	unsigned short * pwDstDatas16bpp = (unsigned short *)pudDstDatas;
	unsigned int * pwDstDatas32bpp = (unsigned int *)pudDstDatas;



	if (iSrcBpp != 24)
	{
		DBG_PRINTF ("can't support this bpp %d\r\n",iSrcBpp);
		return - 1;
	}

	if (iDstBpp == 24) //当目标BPP也为24时，直接拷贝
	{
		memcpy (pudDstDatas,pudSrcDatas,iWidth * 3); // 3= 24/8
	}
	else 
	{
		for (i = 0; i < iWidth; i++)
		{
			//只有bpp为24时可以这样操作
			dwBlue = pudSrcDatas[pos++];
			dwGreen = pudSrcDatas[pos++];
			dwRed = pudSrcDatas[pos++];

			if (iDstBpp == 32)
			{
				dwColor = (dwRed << 16) | (dwGreen << 8) | dwBlue;
				*pwDstDatas32bpp = dwColor;
				pwDstDatas32bpp++;//指针加1 与数据类型有关
			}
			else if (iDstBpp == 16)
			{
				/* 565 */
				dwRed = dwRed >> 3;
				dwGreen = dwGreen >> 2;
				dwBlue = dwBlue >> 3;
				dwColor = (dwRed << 11) | (dwGreen << 5) | (dwBlue);
				*pwDstDatas16bpp = dwColor;
				pwDstDatas16bpp++;
			}



		}

	}

	return 0;

}


static int GetPixelDatasFrmBMP(unsigned char * FileHead,PT_PhotoDesc ptPhotoDesc,int iexpBpp)
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

	iWidth = PBMPInfo->biWidth;						//获取长宽高 像素
	iHeight = PBMPInfo->biHeight;
	iBMPBpp = PBMPInfo->biBitCount;

	//DBG_PRINTF("Expect bpp: %d bpp\r\n",iexpBpp);
    if(iexpBpp!=16 && iexpBpp!=24 && iexpBpp!=32)
    {
        DBG_PRINTF ("iexpBpp is %d\r\n",iexpBpp);	
		return -1;
	}


	if (iHeight < 0) //如果高度数据小于 表示BMP数据存储方式为正向
	{
		PixelDir = 1;
		iHeight = ~iHeight;
		iHeight += 1;

	}

	if (iBMPBpp != 24)
	{
		DBG_PRINTF ("bmp file bpp is %d\r\n",iBMPBpp);		
		DBG_PRINTF("sizeof(BITMAPFILEHEADER) = %d\n", sizeof(BITMAPFILEHEADER));
		return - 1;
	}



	ptPhotoDesc->iHigh = iHeight;
	ptPhotoDesc->iWidth = iWidth;
	ptPhotoDesc->iBpp = iexpBpp;
	ptPhotoDesc->iLineBytes = iWidth * ptPhotoDesc->iBpp / 8;

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

		pucDest += ptPhotoDesc->iLineBytes;
	}

	return 0;

}


static int FreePixelDatasForBMP (PT_PhotoDesc ptPhotoDesc)
{
	//释放分配的内存
	free (ptPhotoDesc->aucPhotoData);
	return 0;
}


T_PicFileParser g_tBMPFileParser =
{
	.name = "bmp",
	.isSupport = isSupportBMP,
	.GetPixelDatas = GetPixelDatasFrmBMP,
	.FreePixelDatas = FreePixelDatasForBMP,
};


int BMPInit(void)
{
	RegisterPicFileParser(&g_tBMPFileParser);

	return 0;

}



