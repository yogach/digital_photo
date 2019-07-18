
#include <config.h>
#include <pic_manager.h>


//缩放函数 scale为缩放倍数 为正数
int PicZoom (PT_PhotoDesc ptOriginPic,PT_PhotoDesc ptZoomPic,int scale)
{
	unsigned long dwDstWidth = ptOriginPic->iWidth / scale; //得到目标长度
	unsigned long x,y;
	unsigned long * pdwSrcXTable = malloc (sizeof (unsigned long) *dwDstWidth);

	/*
		if (ptOriginPic->iBpp != ptZoomPic->iBpp)
		{
			return - 1;
		}
	*/
	for (x = 0; x < dwDstWidth; x++) //生成表 pdwSrcXTable
	{
		pdwSrcXTable[x] = (x * scale);
	}



	free (pdwSrcXTable);
	return 0;

}

