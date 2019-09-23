
#include <stdio.h>
#include <config.h>
#include <pic_manager.h>
#include <stdlib.h>
#include <string.h>


// __attribute__((packed))��������ȡ���ṹ����� ��С����ʵ�����͵Ĵ�С
typedef struct tagBITMAPFILEHEADER
{													/* bmfh */
unsigned short	bfType;
unsigned long	bfSize; 							//λͼ�ļ���С
unsigned short	bfReserved1;
unsigned short	bfReserved2;
unsigned long	bfOffBits;							//���ļ�ͷ��ʼ�ĵ�ʵ��ͼ������֮���ƫ����
} __attribute__ ((packed))BITMAPFILEHEADER;


typedef struct tagBITMAPINFOHEADER
{													/* bmih */
unsigned long	biSize;
unsigned long	biWidth;							//ͼ��Ŀ��
unsigned long	biHeight;							//ͼ��ĸ߶Ȼ���һ����;��ָ��ͼƬ�������ǵ������Ϊ������������
unsigned short	biPlanes;
unsigned short	biBitCount; 						//���� ����һ�����������ü����ֽڱ�ʾ 
unsigned long	biCompression;
unsigned long	biSizeImage;
unsigned long	biXPelsPerMeter;
unsigned long	biYPelsPerMeter;
unsigned long	biClrUsed;
unsigned long	biClrImportant;
} __attribute__ ((packed))BITMAPINFOHEADER;




static int isSupportBMP(unsigned char * FileHead)
{
	//����ļ���ͷΪ424d ������ļ�Ϊbmp��ʽ
	if (FileHead[0] != 0x42 || FileHead[1] != 0x4d)
		return 0;

	else 
		return 1;
}


//����bpp��ͼƬ���ݿ�����ָ���ռ�
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

	if (iDstBpp == 24) //��Ŀ��BPPҲΪ24ʱ��ֱ�ӿ���
	{
		memcpy (pudDstDatas,pudSrcDatas,iWidth * 3); // 3= 24/8
	}
	else 
	{
		for (i = 0; i < iWidth; i++)
		{
			//ֻ��bppΪ24ʱ������������
			dwBlue = pudSrcDatas[pos++];
			dwGreen = pudSrcDatas[pos++];
			dwRed = pudSrcDatas[pos++];

			if (iDstBpp == 32)
			{
				dwColor = (dwRed << 16) | (dwGreen << 8) | dwBlue;
				*pwDstDatas32bpp = dwColor;
				pwDstDatas32bpp++;//ָ���1 �����������й�
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

	iWidth = PBMPInfo->biWidth;						//��ȡ����� ����
	iHeight = PBMPInfo->biHeight;
	iBMPBpp = PBMPInfo->biBitCount;

	//DBG_PRINTF("Expect bpp: %d bpp\r\n",iexpBpp);
    if(iexpBpp!=16 && iexpBpp!=24 && iexpBpp!=32)
    {
        DBG_PRINTF ("iexpBpp is %d\r\n",iexpBpp);	
		return -1;
	}


	if (iHeight < 0) //����߶�����С�� ��ʾBMP���ݴ洢��ʽΪ����
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

	/* BMP���ݵı���ÿ����4�ı��� ���������ֽ���Ҫ��4ȡ�� */
	iLineWidthAlign = (iLineWidthReal + 3) &~0x3;

	pucSrc = FileHead + pBMPHead->bfOffBits;		//�õ�BMP�ļ��������ݵ���ʼλ��

	//������ݲ�������������Ҫ�õ����һ�еĿ�ʼλ��
	if (!PixelDir)
	{
		pucSrc = pucSrc + (iHeight - 1) *iLineWidthAlign;
	}

	pucDest = ptPhotoDesc->aucPhotoData;			//�õ���Ҫ�������ݵ�ָ��


	//һ��һ�п�������
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
	//�ͷŷ�����ڴ�
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



