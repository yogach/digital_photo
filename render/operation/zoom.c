
#include <config.h>
#include <pic_manager.h>
#include <string.h>
#include <stdlib.h>

//���ź��� scaleΪ���ű��� Ϊ����

int PicZoom (PT_PhotoDesc ptOriginPic,PT_PhotoDesc ptZoomPic,int scale)
{
	unsigned long dwDstWidth = ptOriginPic->iWidth / scale; //�õ�Ŀ�곤��
	unsigned long x,y;
	unsigned long * pdwSrcXTable = malloc (sizeof (unsigned long) *dwDstWidth);
	unsigned long dwSrcY;
	unsigned char * pucDest;
	unsigned char * pucSrc;
	unsigned long dwPixelBytes = ptOriginPic->iBpp / 8; //�õ�һ������ռ�ݵĳ���



	//unsigned long dwDstHight =ptOriginPic->iHigh /scale; //Ŀ��߶�
	ptZoomPic->iWidth = ptOriginPic->iWidth / scale; //�õ�Ŀ����(����)
	ptZoomPic->iHigh = ptOriginPic->iHigh / scale;	//Ŀ��߶�
	ptZoomPic->iBpp = ptOriginPic->iBpp;
	ptZoomPic->iLineBytes = ptZoomPic->iWidth * ptZoomPic->iBpp / 8; //һ����ռ�ݵ��ֽ�������һ�е����س���һ��������ռ�ֽ�
	
	//if (ptZoomPic->aucPhotoData == NULL) //�ṹ���ڳ�ʼ���� ָ����ֵ����Ϊ�� Ӧ������һ����ַ
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

	//�ڿ�������֮ǰ��������ԭͼ������ͼx�ϵĶ�Ӧλ�� �ӿ���������ٶ�
	for (x = 0; x < dwDstWidth; x++)
	{
		pdwSrcXTable[x] = (x * scale);
	}


	//���������൱�ڶ���һЩ���ݣ�ʵ������
	for (y = 0; y < ptZoomPic->iHigh; y++)
	{

		dwSrcY = y * scale;

		pucDest = ptZoomPic->aucPhotoData + y * ptZoomPic->iLineBytes;
		pucSrc = ptOriginPic->aucPhotoData + dwSrcY * ptOriginPic->iLineBytes;

		//һ������һ�����صĿ�����ȥ
		for (x = 0; x < dwDstWidth; x++)
		{
			memcpy (pucDest + x * dwPixelBytes,pucSrc + pdwSrcXTable[x] *dwPixelBytes,dwPixelBytes);
		}
	}




	free (pdwSrcXTable);
	return 0;

}




