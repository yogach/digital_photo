
#include <config.h>
#include <pic_manager.h>
#include <string.h>
#include <stdlib.h>

//缩放函数 scale为缩放倍数 为正数
/*
int PicZoom (PT_PhotoDesc ptOriginPic,PT_PhotoDesc ptZoomPic,int scale)
{
	unsigned long dwDstWidth = ptOriginPic->iWidth / scale; //得到目标长度
	unsigned long x,y;
	unsigned long * pdwSrcXTable = malloc (sizeof (unsigned long) *dwDstWidth);
	unsigned long dwSrcY;
	unsigned char * pucDest;
	unsigned char * pucSrc;
	unsigned long dwPixelBytes = ptOriginPic->iBpp / 8; //得到一个像素占据的长度



	//unsigned long dwDstHight =ptOriginPic->iHigh /scale; //目标高度
	ptZoomPic->iWidth = ptOriginPic->iWidth / scale; //得到目标宽度(像素)
	ptZoomPic->iHigh = ptOriginPic->iHigh / scale;	//目标高度
	ptZoomPic->iBpp = ptOriginPic->iBpp;
	ptZoomPic->iLineBytes = ptZoomPic->iWidth * ptZoomPic->iBpp / 8; //一行所占据的字节数等于一行的像素乘与一个像素所占字节

	if (ptZoomPic->aucPhotoData == NULL)
	{
		ptZoomPic->aucPhotoData = malloc (ptZoomPic->iLineBytes * ptZoomPic->iHigh);

		if (ptZoomPic->aucPhotoData == NULL)
		{
			DBG_PRINTF ("malloc ptZoomPic aucPhotoData error \r\n");

			return - 1;
		}
	}

	
		if (ptOriginPic->iBpp != ptZoomPic->iBpp)
		{
			return - 1;
		}

	//在拷贝数据之前事先生成原图与缩放图x上的对应位置 加快程序运行速度
	for (x = 0; x < dwDstWidth; x++)
	{
		pdwSrcXTable[x] = (x * scale);
	}


	//这样拷贝相当于丢掉一些数据，实现缩放
	for (y = 0; y < ptZoomPic->iHigh; y++)
	{

		dwSrcY = y * scale;

		pucDest = ptZoomPic->aucPhotoData + y * ptZoomPic->iLineBytes;
		pucSrc = ptOriginPic->aucPhotoData + dwSrcY * ptOriginPic->iLineBytes;

		//一个像素一个像素的拷贝进去
		for (x = 0; x < dwDstWidth; x++)
		{
			memcpy (pucDest + x * dwPixelBytes,pucSrc + pdwSrcXTable[x] *dwPixelBytes,dwPixelBytes);
		}
	}




	free (pdwSrcXTable);
	return 0;

}
*/

int PicZoom(PT_PhotoDesc ptOriginPic, PT_PhotoDesc ptZoomPic)
{
    unsigned long dwDstWidth = ptZoomPic->iWidth;
    unsigned long* pdwSrcXTable = malloc(sizeof(unsigned long) * dwDstWidth);
	unsigned long x;
	unsigned long y;
	unsigned long dwSrcY;
	unsigned char *pucDest;
	unsigned char *pucSrc;
	unsigned long dwPixelBytes = ptOriginPic->iBpp/8;

	if (ptOriginPic->iBpp != ptZoomPic->iBpp)
	{
		return -1;
	}
	
    for (x = 0; x < dwDstWidth; x++)//生成表 pdwSrcXTable
    {
        pdwSrcXTable[x]=(x*ptOriginPic->iWidth/ptZoomPic->iWidth);
    }

    for (y = 0; y < ptZoomPic->iHigh; y++)
    {			
        dwSrcY = (y * ptOriginPic->iHigh / ptZoomPic->iHigh);

		pucDest = ptZoomPic->aucPhotoData + y*ptZoomPic->iLineBytes;
		pucSrc  = ptOriginPic->aucPhotoData + dwSrcY*ptOriginPic->iLineBytes;
		
        for (x = 0; x <dwDstWidth; x++)
        {
            /* 原图座标: pdwSrcXTable[x]，srcy
             * 缩放座标: x, y
			 */
			 memcpy(pucDest+x*dwPixelBytes, pucSrc+pdwSrcXTable[x]*dwPixelBytes, dwPixelBytes);
        }
    }

    free(pdwSrcXTable);
	return 0;
}


