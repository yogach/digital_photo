
#include <config.h>
#include <pic_manager.h>
#include <string.h>
#include <stdlib.h>

//缩放函数 scale为缩放倍数 为正数

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
	
	//if (ptZoomPic->aucPhotoData == NULL) //结构体在初始化后 指针数值并不为空 应该已是一个地址
	//{
	ptZoomPic->aucPhotoData = malloc (ptZoomPic->iLineBytes * ptZoomPic->iHigh);

	if (ptZoomPic->aucPhotoData == NULL)
	{
		DBG_PRINTF ("malloc ptZoomPic aucPhotoData error \r\n");

		return - 1;
	}

    //}
	
		//if (ptOriginPic->iBpp != ptZoomPic->iBpp)
		//{
			//return - 1;
		//}

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




