#include <config.h>
#include <pic_manager.h>
#include <display_manager.h>
#include <string.h>
#include <fonts_manager.h>
#include <encoding_manager.h>


/**********************************************************************
 * �������ƣ� SetColorForPixelInVideoMem
 * ���������� ����VideoMem��ĳ���������ص���ɫ
 * ��������� iX,iY      - ��������
 *            ptVideoMem - ����VideoMem�е�����
 *            dwColor    - ����Ϊ�����ɫ,��ɫ��ʽΪ0x00RRGGBB
 * ��������� ��
 * �� �� ֵ�� �������ռ�ݶ����ֽ�
 ***********************************************************************/
int SetColorForPixelInVideoMem(int iX,int iY,PT_VideoMem ptVideoMem, unsigned int dwBackColor)
{
	unsigned char* pucFB;
	unsigned short* pwFB16bpp;
	unsigned int* pdwFB32bpp;
	unsigned short wColor16bpp; /* 565 */
	int iRed;
	int iGreen;
	int iBlue;

	//�õ�����λ��
	pucFB	   = ptVideoMem->tVideoMemDesc.aucPhotoData;
	pucFB     += iY * ptVideoMem->tVideoMemDesc.iLineBytes + iX * ptVideoMem->tVideoMemDesc.iBpp / 8;

	pwFB16bpp  = ( unsigned short* ) pucFB;
	pdwFB32bpp = ( unsigned int* ) pucFB;
	
	//���ݲ�ͬ��������Ƚ��д���
	switch ( ptVideoMem->tVideoMemDesc.iBpp )
	{
		case 8:
		{
			*pucFB = (unsigned char) dwBackColor;
			return 1;
			break;
		}
		case 16:
		{
			iRed   = ( dwBackColor >> ( 16+3 ) ) & 0x1f;
			iGreen = ( dwBackColor >> ( 8+2 ) ) & 0x3f;
			iBlue  = ( dwBackColor >> 3 ) & 0x1f;
			wColor16bpp = ( iRed << 11 ) | ( iGreen << 5 ) | iBlue;
			
			*pwFB16bpp	= wColor16bpp;

			return 2;
			break;
		}
		case 32:
		{
			
			*pdwFB32bpp = dwBackColor;
			
			return 4;
			break;
		}
		default :
		{
			DBG_PRINTF ( "can't support %d bpp\n", ptVideoMem->tVideoMemDesc.iBpp );
			return -1;
		}
	}
	return -1;

	
}

/**********************************************************************
 * �������ƣ� SetColorForAppointArea
 * ���������� �ı�ָ�������ڵ���ɫ
 *            �ο�: 03.freetype\02th_arm\06th_show_lines_center
 * ��������� iTopLeftX,iTopLeftY   - ������������Ͻ�����
 *            iBotRightX,iBotRightY - ������������½�����
 *            dwBackColor           - ����ɫ
 *            ptVideoMem            - VideoMem
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�,  ����ֵ - ʧ��
 ***********************************************************************/
int SetColorForAppointArea(int iTopLeftX, int iTopLeftY, int iBotRightX, int iBotRightY,  PT_VideoMem ptVideoMem, unsigned int dwBackColor)
{
   int x,y;

   for(y=iTopLeftY;y<=iBotRightY;y++)
   	for(x=iTopLeftX;x<=iBotRightX;x++)
		SetColorForPixelInVideoMem(x,y,ptVideoMem,dwBackColor);
}
/**********************************************************************
 * �������ƣ� MergerStringToCenterOfRectangleInVideoMem
 * ���������� ��VideoMem��ָ�����ξ�����ʾ�ַ���
 *            �ο�: 03.freetype\02th_arm\06th_show_lines_center
 * ��������� iTopLeftX,iTopLeftY   - ������������Ͻ�����
 *            iBotRightX,iBotRightY - ������������½�����
 *            pucTextString         - Ҫ��ʾ���ַ���
 *            ptVideoMem            - VideoMem
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�,  ����ֵ - ʧ��
 ***********************************************************************/
int MergerStringToCenterOfRectangleInVideoMem(int iTopLeftX, int iTopLeftY, int iBotRightX, int iBotRightY, unsigned char *pucTextString, PT_VideoMem ptVideoMem)
{
  unsigned char * pucStrStart;
  unsigned char * pucStrEnd;
  int ilen,bHasGetCode = 0,iError;
  unsigned int dwCode;
  T_FontBitMap tFontBitMap;
  int iMinX = 32000,iMinY = 32000 ;
  int iMaxX = -1,iMaxY =-1;
  int iWidth , iHight;
  

  /* 1. �����������ĵ�ɫ */
  SetColorForAppointArea(iTopLeftX,iTopLeftY,iBotRightX,iBotRightY,ptVideoMem,COLOR_BACKGROUND);
  
  /* 2.�ȼ����ַ���λͼ�������ȡ��߶� */
  pucStrStart = pucTextString;
  pucStrEnd = pucStrStart + strlen(pucTextString);

  while(1)
  {
    ilen = GetCodeFrmBuf(pucStrStart,pucStrEnd,&dwCode);
	if(ilen == 0)
	{
     
      if(bHasGetCode)
      {
       //����˴������ַ����ѽ���
        DBG_PRINTF("string end ..\r\n");
		break;
	  }
	  else
	  {
	    // ����˴������ȡdwCode ʧ��
	    DBG_PRINTF("GetCodeFrmBuf error..\r\n");
		return -1;
	  }
	  
	}

    bHasGetCode = 1;
	pucStrStart +=ilen;

	//����ַ�λͼ
	iError = GetFontBitmap(dwCode,&tFontBitMap);
    if(iError == 0)
    {
      //�õ������ַ�����ռ�ݵĴ�С 
      if(iMinX > tFontBitMap.iXLeft)
	  	iMinX = tFontBitMap.iXLeft;
	  if(iMaxX < tFontBitMap.iXMax)
	  	iMaxX = tFontBitMap.iXMax;

	  if(iMinY > tFontBitMap.iYTop)
	    iMinY = tFontBitMap.iYTop;
	  if(iMaxY < tFontBitMap.iYMax)
	    iMaxY = tFontBitMap.iYMax;

      //�õ���һ����ʾ�ַ���λ��
	  tFontBitMap.iCurOriginX = tFontBitMap.iNextOriginX;
      tFontBitMap.iCurOriginY = tFontBitMap.iNextOriginY;

	}
	else
	{
      DBG_PRINTF("GetFontBitmap Error...\r\n");
	}

  }

  /*3.�ж�Ҫ��ʾ���ַ����Ƿ񳬹���ʾ����*/
  //�õ��ַ�����С
  iWidth = iMaxX - iMinX;
  iHight = iMaxY - iMinY;

  if(iWidth > (iBotRightX -iTopLeftX))
  {
     iWidth = iBotRightX -iTopLeftX;
  }

  if(iHight > iBotRightY -iTopLeftY )
  {
     DBG_PRINTF("string hight exceed the limit ..\r\n ");
     return -1;
  }

  /*4.ȷ����һ���ַ���ԭ������*/
  


}

/**********************************************************************
 * �������ƣ� InvertButton
 * ���������� �ǰ���ʾ�豸��ָ��������ÿ�����ص���ɫȡ��
 * ��������� ptLayout   - ��������
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���          �޸�����
 * -----------------------------------------------
**********************************************************************/
static void InvertButton ( PT_Layout ptLayout )
{
	int iBpp,iY,i;
	int iButtonWidth;
	unsigned char* pucDispMem;
	PT_DispOpr ptDefaultDisOpr;

	//���Ĭ�������豸
	ptDefaultDisOpr = GetDefaultDispDev();
	if ( ptDefaultDisOpr == NULL )
	{
		DBG_PRINTF ( "can't get default display device..\r\n" );
	}

	iBpp = ptDefaultDisOpr->iBpp;
	pucDispMem = ptDefaultDisOpr->pucDispMem;
	//�õ�ͼ����fbmen�ڵ���ʼλ��
	pucDispMem += ptLayout->iTopLeftY * ptDefaultDisOpr->iLineWidth + ptLayout->iTopLeftX *  iBpp / 8;
	//��ȡͼ�������ռ�ֽ���
	iButtonWidth = ( ptLayout->iLowerRightX - ptLayout->iTopLeftX + 1 ) * iBpp / 8;


	//��ͼ�����Ͻ�Ϊ��ʼ ��ÿһ�ж����д���
	for ( iY= ptLayout->iTopLeftY ; iY <=ptLayout->iLowerRightY ; iY++ )
	{
		//��ͼ����ʼλ�ÿ�ʼ ��ÿһ���ֽڽ���ȡ������
		for ( i = 0 ; i< iButtonWidth ; i++ )
		{
			pucDispMem[i] = ~pucDispMem[i];
		}
		pucDispMem += ptDefaultDisOpr->iLineWidth;
	}
}

/**********************************************************************
 * �������ƣ� ReleaseButton
 * ���������� �ɿ�ͼ��,ֻ�Ǹı���ʾ�豸�ϵ�ͼ�갴ť��ɫ
 * ��������� ptLayout   - ͼ�����ھ�������
 * ��������� ��
 * �� �� ֵ�� ��
 * -----------------------------------------------
**********************************************************************/
void ReleaseButton ( PT_Layout ptLayout )
{
	InvertButton ( ptLayout );
}

/**********************************************************************
 * �������ƣ� PressButton
 * ���������� ����ͼ��,ֻ�Ǹı���ʾ�豸�ϵ�ͼ�갴ť��ɫ
 * ��������� ptLayout   - ͼ�����ھ�������
 * ��������� ��
 * �� �� ֵ�� ��
 ***********************************************************************/
void PressButton ( PT_Layout ptLayout )
{
	InvertButton ( ptLayout );
}



