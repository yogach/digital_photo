
#include <config.h>
#include <pic_manager.h>
#include <string.h>

/**********************************************************************
 * �������ƣ� PicMerge
 * ���������� ��СͼƬ�ϲ����ͼƬ��
 * ��������� iX,iY      - СͼƬ�ϲ����ͼƬ��ĳ������, iX/iYȷ�������������Ͻ�����
 *            ptSmallPic - �ں�СͼƬ����������
 *            ptBigPic   - �ں���ͼƬ����������
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 ***********************************************************************/
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


int PicMergeRegion(int iStartXofNewPic, int iStartYofNewPic, int iStartXofOldPic, int iStartYofOldPic, int iWidth, int iHeight, PT_PhotoDesc ptNewPic, PT_PhotoDesc ptOldPic)
{
    unsigned char * pucStartSrc;
	unsigned char * pucStartDst;
    int i;
	int iLineWidthByte =  iWidth * ptNewPic->iBpp /8;

    //�ж���ʾ��ͼƬ�Ƿ���������
	if ((iStartXofNewPic < 0 || iStartXofNewPic >= ptNewPic->iWidth) || \
        (iStartYofNewPic < 0 || iStartYofNewPic >= ptNewPic->iHigh)  || \
        (iStartXofOldPic < 0 || iStartXofOldPic >= ptOldPic->iWidth) || \
        (iStartYofOldPic < 0 || iStartYofOldPic >= ptOldPic->iHigh))
    {
        return -1;
    }
	
	pucStartSrc = ptNewPic->aucPhotoData +  iStartXofNewPic*ptNewPic->iBpp /8 + iStartYofNewPic * ptNewPic->iLineBytes;
	pucStartDst = ptOldPic->aucPhotoData +  iStartXofOldPic*ptOldPic->iBpp /8 + iStartYofOldPic * ptOldPic->iLineBytes;

 
    for(i=0;i<iHeight; i++)  
    {
       memcpy(pucStartDst,pucStartSrc,iLineWidthByte);
       pucStartSrc +=ptNewPic->iLineBytes;
       pucStartDst +=ptOldPic->iLineBytes;
	}
	
   return 0;
}

