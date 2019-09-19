#include <config.h>
#include <disp_manager.h>
#include <page_manager.h>
#include <pic_manager.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <input_manager.h>
#include <render.h>

static int CalcManualPageLayout ( PT_Layout atLayout );
static int ManualPageSpecialDis ( PT_VideoMem ptVideoMem );
static void ManualPageRun ( void );


static T_Layout g_atManualPageIconsLayout[] =
{
	{0, 0, 0, 0, "return.bmp"},
	{0, 0, 0, 0, "zoomout.bmp"},
	{0, 0, 0, 0, "zoomin.bmp"},
	{0, 0, 0, 0, "pre_pic.bmp"},
	{0, 0, 0, 0, "next_pic.bmp"},
	{0, 0, 0, 0, "continue_mod_small.bmp"},
	{0, 0, 0, 0, NULL},
};

static T_Layout g_tManualPagePictureLayout;

//����ͼƬ����
static T_PhotoDesc g_tOriginPicPixelDatas;
static T_PhotoDesc g_tZoomedPicPixelDatas;




static T_PageDesc g_tManualPageDesc =
{
	.name   = "manual",
	.Run    = ManualPageRun,
	.CalcPageLayout = CalcManualPageLayout,
	.atPageLayout = g_atManualPageIconsLayout,
	.DispSpecialIcon = ManualPageSpecialDis,
};

/**********************************************************************
 * �������ƣ� CalcManualPagePictureLayout
 * ���������� ����"manualҳ��"��"ͼƬ����ʾ����"
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0-�ɹ� ����ֵ-ʧ��
 ***********************************************************************/
static int CalcManualPagePictureLayout ( void )
{
	//�����Ļ�ֱ���
	int iXres,iYres,iBpp;
	int iTopLeftX,iTopLeftY,iLowerRightX,iLowerRightY;
	int i = 0;
	GetDispResolution ( &iXres,&iYres,&iBpp ); //��ȡLCD�ֱ���

	if ( iXres < iYres )
	{
		/*	  iXres/6
		 *	  --------------------------------------------------------------
		 *	   return	zoomout zoomin	pre_pic next_pic continue_mod_small  (ͼ��)
		 *	  --------------------------------------------------------------
		 *
		 *								ͼƬ
		 *
		 *
		 *	  --------------------------------------------------------------
		 */
		iTopLeftX = 0;
		iTopLeftY = g_atManualPageIconsLayout[0].iLowerRightY + 1;
		iLowerRightX = iXres - 1;
		iLowerRightY = iYres - 1;
	}
	else
	{
		/*	  iYres/6
		 *	  --------------------------------------------------------------
		 *	   up		         |
		 *                       |
		 *    zoomout	         |
		 *                       |
		 *    zoomin             |
		 *                       |
		 *    pre_pic            |                 ͼƬ
		 *                       |
		 *    next_pic           |
		 *                       |
		 *    continue_mod_small |
		 *                       |
		 *	  --------------------------------------------------------------
		 */
		iTopLeftX = g_atManualPageIconsLayout[0].iLowerRightX + 1;
		iTopLeftY = 0;
		iLowerRightX = iXres - 1;
		iLowerRightY = iYres - 1;


	}

	g_tManualPagePictureLayout.iTopLeftX = iTopLeftX;
	g_tManualPagePictureLayout.iTopLeftY = iTopLeftY;
	g_tManualPagePictureLayout.iLowerRightX = iLowerRightX;
	g_tManualPagePictureLayout.iLowerRightY = iLowerRightY;

	return 0;
}
/**********************************************************************
 * �������ƣ� CalcManualPageMenusLayout
 * ���������� ����ҳ���и�ͼ������ֵ
 * ��������� ��
 * ��������� ptPageLayout - �ں���ͼ������Ͻ�/���½�����ֵ
 * �� �� ֵ�� 0-�ɹ� ����ֵ-ʧ��
 ***********************************************************************/
static int CalcManualPageLayout ( PT_Layout atLayout )
{

	//�����Ļ�ֱ���
	int iXres,iYres,iBpp;
	int iIconWidth,iIconHight;
	int i = 0;
	GetDispResolution ( &iXres,&iYres,&iBpp ); //��ȡLCD�ֱ���


	if ( iXres < iYres )
	{
		/*	 iXres/6
		 *	  --------------------------------------------------------------
		 *	   return	zoomout	zoomin  pre_pic next_pic continue_mod_small
		 *
		 *
		 *
		 *
		 *
		 *
		 *	  --------------------------------------------------------------
		 */
		iIconWidth = iXres/6;
		iIconHight = iIconWidth;

		while ( atLayout->IconName )
		{

			atLayout->iTopLeftX = 0 + iIconWidth*i;
			atLayout->iLowerRightX = atLayout->iTopLeftX + iIconWidth;

			atLayout->iTopLeftY = 0;
			atLayout->iLowerRightY = atLayout->iTopLeftY+iIconHight;

			i++;
			atLayout++;
		}

	}
	else
	{

		/*	 iYres/6
		 *	  ----------------------------------
		 *	   up
		 *
		 *    zoomout
		 *
		 *    zoomin
		 *
		 *    pre_pic
		 *
		 *    next_pic
		 *
		 *    continue_mod_small
		 *
		 *	  ----------------------------------
		 */
		iIconHight = iYres/6;
		iIconWidth = iIconHight;

		while ( atLayout->IconName )
		{

			atLayout->iTopLeftX = 0;
			atLayout->iLowerRightX = atLayout->iTopLeftX + iIconWidth;

			atLayout->iTopLeftY = 0 + iIconHight*i;
			atLayout->iLowerRightY = atLayout->iTopLeftY+iIconHight;

			i++;
			atLayout++;

		}

	}

	CalcManualPagePictureLayout();
	return 0;

}

static PT_PhotoDesc GetOriginPictureFilePixelDatas ( char* strFileName )
{
	int iError;

	//�ڻ�ȡ��һ������֮ǰ���ͷ�֮ǰ����Ŀռ�
	if ( g_tOriginPicPixelDatas.aucPhotoData != NULL )
	{
		free ( g_tOriginPicPixelDatas.aucPhotoData );
		g_tOriginPicPixelDatas.aucPhotoData = NULL;
	}

	iError =  GetOriPixelDatasFormFile ( strFileName, &g_tOriginPicPixelDatas );
	if ( iError==0 )
	{
		return &g_tOriginPicPixelDatas;
	}
	else
	{
		return NULL;

	}

}

static PT_PhotoDesc GetZoomedPicPixelDatas ( PT_PhotoDesc ptOriginPicPixelDatas, int iZoomedWidth, int iZoomedHeight )
{
    int iXres,iYres,iBpp;
	float k;
    GetDispResolution ( &iXres,&iYres,&iBpp ); //��ȡLCD�ֱ���

    //���·���֮ǰ���ͷ�ԭ�ȷ���Ŀռ� ��ֹ�ڴ�й©
    if(g_tZoomedPicPixelDatas.aucPhotoData !=NULL)
    {
        free(g_tZoomedPicPixelDatas.aucPhotoData);
		g_tZoomedPicPixelDatas.aucPhotoData = NULL;
	}

	//�������ź�ͼƬ�Ĵ�С ʹ������ָ��������ʾ
	k = (float)ptOriginPicPixelDatas->iHigh / ptOriginPicPixelDatas->iWidth; // ��ȡԭ��ͼƬ�ĳ����

	g_tZoomedPicPixelDatas.iWidth  = iZoomedWidth ;
    g_tZoomedPicPixelDatas.iHigh   = iZoomedWidth * k ;

    //���ͼƬ�ĸ߶ȴ�������߶�
	if(g_tZoomedPicPixelDatas.iHigh > iZoomedHeight)
	{
      g_tZoomedPicPixelDatas.iHigh = iZoomedHeight;
      g_tZoomedPicPixelDatas.iWidth = g_tZoomedPicPixelDatas.iHigh / k ;
	}

	g_tZoomedPicPixelDatas.iBpp    = iBpp;
	g_tZoomedPicPixelDatas.iLineBytes = g_tZoomedPicPixelDatas.iWidth * g_tZoomedPicPixelDatas.iBpp / 8;
	g_tZoomedPicPixelDatas.iTotalBytes = g_tZoomedPicPixelDatas.iLineBytes * g_tZoomedPicPixelDatas.iHigh;
	g_tZoomedPicPixelDatas.aucPhotoData = malloc ( g_tZoomedPicPixelDatas.iTotalBytes );
    if(g_tZoomedPicPixelDatas.aucPhotoData == NULL)
		return NULL;

	//ʹ�ú�����������
    PicZoom(ptOriginPicPixelDatas, &g_tZoomedPicPixelDatas);
	return &g_tZoomedPicPixelDatas;

}

static int ShowPictureInManualPage ( PT_VideoMem ptVideoMem, char* strFileName )
{
	int iXres,iYres,iBpp;
	PT_PhotoDesc ptOriPicPixelDatas, ptZoomedPicPixelDatas;
	int iPicWidth,iPicHight;
	int iTopLeftX, iTopLeftY;

	GetDispResolution ( &iXres,&iYres,&iBpp ); //��ȡLCD�ֱ���

	//��ȡͼƬ����
	ptOriPicPixelDatas = GetOriginPictureFilePixelDatas ( strFileName );
	if ( ptOriPicPixelDatas == NULL )
	{
		DBG_PRINTF ( "GetOriginPictureFilePixelDatas error..\r\n" );
		return -1;
	}


	// ���ȼ���ҳ��ͼƬ��ʾ������ܴ�С
	iPicWidth = g_tManualPagePictureLayout.iLowerRightX - g_tManualPagePictureLayout.iTopLeftX + 1;
	iPicHight = g_tManualPagePictureLayout.iLowerRightY - g_tManualPagePictureLayout.iTopLeftY + 1;

	//���õ���ͼƬԭʼ���ݽ������� ���ź�����ݷ���g_tZoomedPicPixelDatas
	ptZoomedPicPixelDatas = GetZoomedPicPixelDatas ( ptOriPicPixelDatas,iPicWidth,iPicHight );

	/* ���������ʾʱ���Ͻ����� */

	//���ͼ���м��

	//���ָ���������ɫ

	//����Ҫ��ʾ��ͼƬ�ϲ����Դ���

}

static int ManualPageSpecialDis ( PT_VideoMem ptVideoMem )
{
	return ShowPictureInManualPage ( ptVideoMem, );//����д��һ���̶��ļ���
}

static void ManualPageRun ( void )
{
	PT_VideoMem ptDevVideoMem;

	//�����ʾ�豸�Դ�
	ptDevVideoMem = GetDevVideoMen();

	/* 1. ��ʾҳ�� */
	ShowPage ( &g_tManualPageDesc );

	while ( 1 )
	{




	}


}


int ManualPageInit ( void )
{
	return RegisterPageAction ( &g_tManualPageDesc );
}



