
#include <config.h>
#include <pic_manager.h>
#include <string.h>


int PicMerge(int iX, int iY, PT_PhotoDesc ptSmallPic, PT_PhotoDesc ptBigPic)
{
	int i;
	unsigned char *pucSrc;
	unsigned char *pucDst;
	
	if ((ptSmallPic->iWidth > ptBigPic->iWidth)  ||
		(ptSmallPic->iHigh > ptBigPic->iHigh) ||
		(ptSmallPic->iBpp != ptBigPic->iBpp))
	{
		return -1;
	}

	pucSrc = ptSmallPic->aucPhotoData;
	pucDst = ptBigPic->aucPhotoData + iY * ptBigPic->iLineBytes + iX * ptBigPic->iBpp / 8;
	for (i = 0; i < ptSmallPic->iHigh; i++)
	{
		memcpy(pucDst, pucSrc, ptSmallPic->iLineBytes);
		pucSrc += ptSmallPic->iLineBytes;
		pucDst += ptBigPic->iLineBytes;
	}
	return 0;
}


