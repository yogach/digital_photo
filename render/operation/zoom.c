
#include <config.h>
#include <pic_manager.h>


//���ź��� scaleΪ���ű��� Ϊ����
int PicZoom (PT_PhotoDesc ptOriginPic,PT_PhotoDesc ptZoomPic,int scale)
{
	unsigned long dwDstWidth = ptOriginPic->iWidth / scale; //�õ�Ŀ�곤��
	unsigned long x,y;
	unsigned long * pdwSrcXTable = malloc (sizeof (unsigned long) *dwDstWidth);

	/*
		if (ptOriginPic->iBpp != ptZoomPic->iBpp)
		{
			return - 1;
		}
	*/
	for (x = 0; x < dwDstWidth; x++) //���ɱ� pdwSrcXTable
	{
		pdwSrcXTable[x] = (x * scale);
	}



	free (pdwSrcXTable);
	return 0;

}

