
#include <config.h>
#include <pic_manager.h>
#include <string.h>

/**********************************************************************
 * 函数名称： PicMerge
 * 功能描述： 把小图片合并入大图片里
 * 输入参数： iX,iY      - 小图片合并入大图片的某个区域, iX/iY确定这个区域的左上角座标
 *            ptSmallPic - 内含小图片的象素数据
 *            ptBigPic   - 内含大图片的象素数据
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
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

    //判断显示的图片是否在区域内
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

