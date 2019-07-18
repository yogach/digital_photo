
#include <config.h>
#include <pic_manager.h>
#include <string.h>


int Pic_Merge (int iX,int iY,PT_PhotoDesc ptSrc,PT_PhotoDesc ptDst)
{
	int pos = 0;
	int i;
	unsigned char * pucSrc;
	unsigned char * pucDst;

	if ((ptSrc->iHigh > ptDst->iHigh) || (ptSrc->iWidth > ptDst->iWidth) || (ptSrc->iBpp != ptDst->iBpp))
	{
		return - 1;
	}


	if ((iX > ptDst->iWidth) || (iY > ptDst->iHigh))
	{

		return - 1;
	}

	//�õ����ߵ�ͼƬ����������ʼλ��
	pucSrc = ptSrc->aucPhotoData;
	pucDst = ptDst->aucPhotoData + iX * ptSrc->iBpp / 8 + iY * ptSrc->iLineBytes;

	//���н���copy
	for (i = 0; i < ptSrc->iHigh; i++)
	{
		memcpy (pucDst,pucSrc,ptSrc->iLineBytes);
		pucSrc += ptSrc->iLineBytes;
		pucDst += ptDst->iLineBytes;

	}



}


