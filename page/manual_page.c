
#include <config.h>
#include <disp_manager.h>
#include <page_manager.h>
#include <pic_manager.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <input_manager.h>
#include <render.h>
#include <file.h>
#include <string.h>

static int CalcManualPageLayout ( PT_Layout atLayout );
static int ManualPageSpecialDis ( PT_VideoMem ptVideoMem );
static void ManualPageRun ( PT_PageParams ptPageParams);

#define ZOOM_RATIO (0.9)

#define SLIP_MIN_DISTANCE (2*2)

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


/* ��ʾ��LCD�ϵ�ͼƬ, �������ĵ�, ��g_tZoomedPicPixelDatas������� */
static int g_iXofZoomedPicShowInCenter;
static int g_iYofZoomedPicShowInCenter;

static char g_cstrFileName[256];

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
	//int i = 0;
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
	if ( g_tZoomedPicPixelDatas.aucPhotoData !=NULL )
	{
		free ( g_tZoomedPicPixelDatas.aucPhotoData );
		g_tZoomedPicPixelDatas.aucPhotoData = NULL;
	}

	//�������ź�ͼƬ�Ĵ�С ʹ������ָ��������ʾ
	k = ( float ) ptOriginPicPixelDatas->iHigh / ptOriginPicPixelDatas->iWidth; // ��ȡԭ��ͼƬ�ĳ����

	g_tZoomedPicPixelDatas.iWidth  = iZoomedWidth ;
	g_tZoomedPicPixelDatas.iHigh   = iZoomedWidth * k ;

	//���ͼƬ�ĸ߶ȴ�������߶� ��������ͼƬ�߶�Ϊ����߶� ��Ȱ���������
	if ( g_tZoomedPicPixelDatas.iHigh > iZoomedHeight )
	{
		g_tZoomedPicPixelDatas.iHigh = iZoomedHeight;
		g_tZoomedPicPixelDatas.iWidth = g_tZoomedPicPixelDatas.iHigh / k ;
	}

	g_tZoomedPicPixelDatas.iBpp    = iBpp;
	g_tZoomedPicPixelDatas.iLineBytes = g_tZoomedPicPixelDatas.iWidth * g_tZoomedPicPixelDatas.iBpp / 8;
	g_tZoomedPicPixelDatas.iTotalBytes = g_tZoomedPicPixelDatas.iLineBytes * g_tZoomedPicPixelDatas.iHigh;
	g_tZoomedPicPixelDatas.aucPhotoData = malloc ( g_tZoomedPicPixelDatas.iTotalBytes );
	if ( g_tZoomedPicPixelDatas.aucPhotoData == NULL )
	{
		return NULL;
	}

	//ʹ�ú�����������
	PicZoom ( ptOriginPicPixelDatas, &g_tZoomedPicPixelDatas );
	return &g_tZoomedPicPixelDatas;

}
/**********************************************************************
 * �������ƣ� ShowPictureInManualPage
 * ���������� ��"manualҳ��"����ʾͼƬ
 * ��������� strFileName - Ҫ��ʾ���ļ�������(������·��)
 *            ptVideoMem  - �����VideoMem����ʾ
 * ��������� ��
 * �� �� ֵ�� 0      - �ɹ�
 *            ����ֵ - ʧ��
 ***********************************************************************/
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

	//���õ���ͼƬԭʼ���ݽ������� ���ź�����ݷ���ptZoomedPicPixelDatas
	ptZoomedPicPixelDatas = GetZoomedPicPixelDatas ( &g_tOriginPicPixelDatas,iPicWidth,iPicHight );
	if ( ptZoomedPicPixelDatas == NULL )
	{
		DBG_PRINTF ( "GetZoomedPicPixelDatas error..\r\n" );
		return -1;
	}

	/* ���ͼƬ������ʾʱ���Ͻ����� */
	iTopLeftX = g_tManualPagePictureLayout.iTopLeftX + ( iPicWidth - ptZoomedPicPixelDatas->iWidth ) /2;
	iTopLeftY = g_tManualPagePictureLayout.iTopLeftY + ( iPicHight - ptZoomedPicPixelDatas->iHigh ) /2;

	//��ʾ��LCD�ϵ�ͼƬ, �������ĵ�, ��g_tZoomedPicPixelDatas�������
	g_iXofZoomedPicShowInCenter = ptZoomedPicPixelDatas->iWidth / 2;
	g_iYofZoomedPicShowInCenter = ptZoomedPicPixelDatas->iHigh / 2 ;

	//���ָ���������ɫ
	SetColorForAppointArea ( g_tManualPagePictureLayout.iTopLeftX,g_tManualPagePictureLayout.iTopLeftY,\
	                         g_tManualPagePictureLayout.iLowerRightX,g_tManualPagePictureLayout.iLowerRightY,\
	                         ptVideoMem,COLOR_BACKGROUND );
	//����Ҫ��ʾ��ͼƬ�ϲ����Դ���
	PicMerge ( iTopLeftX, iTopLeftY, ptZoomedPicPixelDatas,&ptVideoMem->tVideoMemDesc );
	return 0;

}

static int ManualPageSpecialDis ( PT_VideoMem ptVideoMem )
{
	//return ShowPictureInManualPage ( ptVideoMem,"//mnt/zoomin.bmp" );//��д��һ���̶�����·�� ���ڲ���
	return ShowPictureInManualPage ( ptVideoMem,g_cstrFileName );
}

static void ShowZoomedPictureInLayout ( PT_PhotoDesc ptZoomedPicPixelDatas, PT_VideoMem ptVideoMem )
{

	int iStartXofPicData, iStartYofPicData;   //����ͼƬ�������ݵ���ʼ���� ��g_tZoomedPicPixelDatas�������
	int iStartXofVideoMen, iStartYofVideoMen; //�������Դ��ڵ�x,y��ʼ����
	int iPictureLayoutWidth, iPictureLayoutHeight;
	int iWidthPictureInPlay, iHeightPictureInPlay; //����ͼƬʵ����ʾ��Ⱥ͸߶�
	int iDeltaX, iDeltaY;

	//�õ�ͼƬ��ʾ����Ŀ�͸�
	iPictureLayoutWidth = g_tManualPagePictureLayout.iLowerRightX - g_tManualPagePictureLayout.iTopLeftX + 1;
	iPictureLayoutHeight = g_tManualPagePictureLayout.iLowerRightY - g_tManualPagePictureLayout.iTopLeftY + 1;

	/* ��ʾX����Ϣ
	 *�õ��µ���ʼX���� ������g_tZoomedPicPixelDatas�������
	 *��ͼƬ��һ������ʾ�����һ�����Ƚ�
	*/
	iStartXofPicData = g_iXofZoomedPicShowInCenter - iPictureLayoutWidth / 2;
	if ( iStartXofPicData <0 )
	{
		iStartXofPicData = 0;
	}
	if ( iStartXofPicData > ptZoomedPicPixelDatas->iWidth )
	{
		iStartXofPicData = ptZoomedPicPixelDatas->iWidth;
	}
	/*
	 * ���ݵõ��ļ���õ�ͼƬ��ʾ�������ʼ����
	 * g_iXofZoomedPicShowInCenterͼƬ���ĵ� - iStartXofPicDataͼƬ��ʼ�� = PictureLayout���ĵ�X���� - iStartXofVideoMen
	 *
	 */
	iDeltaX = g_iXofZoomedPicShowInCenter - iStartXofPicData;
	iStartXofVideoMen = (g_tManualPagePictureLayout.iTopLeftX + iPictureLayoutWidth / 2) - iDeltaX;
	//���Ҫ��ʾ��ͼƬ��ʼλ�ô���
	if ( iStartXofVideoMen < g_tManualPagePictureLayout.iTopLeftX )
	{
		iStartXofVideoMen = g_tManualPagePictureLayout.iTopLeftX;
	}
	if ( iStartXofVideoMen > g_tManualPagePictureLayout.iLowerRightX )
	{
		iStartXofVideoMen = g_tManualPagePictureLayout.iLowerRightX + 1;
	}

	//����ʵ�ʵ���ʾ���
	if ( ( ptZoomedPicPixelDatas->iWidth - iStartXofPicData ) > ( g_tManualPagePictureLayout.iLowerRightX - iStartXofVideoMen + 1 ) )
	{
		iWidthPictureInPlay = (g_tManualPagePictureLayout.iLowerRightX - iStartXofVideoMen + 1);
	}
	else
	{
		iWidthPictureInPlay = (ptZoomedPicPixelDatas->iWidth - iStartXofPicData);
	}


	/* Y��ͬ��
	 *�õ��µ���ʼX���� ������g_tZoomedPicPixelDatas�������
	 *��ͼƬ��һ������ʾ�����һ�����Ƚ�
	*/
	iStartYofPicData = g_iYofZoomedPicShowInCenter - iPictureLayoutHeight / 2;
	if ( iStartYofPicData <0 )
	{
		iStartYofPicData = 0;
	}
	if ( iStartYofPicData > ptZoomedPicPixelDatas->iHigh )
	{
		iStartYofPicData = ptZoomedPicPixelDatas->iHigh;
	}

	/*
	 * ���ݵõ��ļ���õ�ͼƬ��ʾ�������ʼ����
	 * g_iXofZoomedPicShowInCenterͼƬ���ĵ� - iStartXofPicDataͼƬ��ʼ�� = PictureLayout���ĵ�y���� - iStartXofVideoMen
	 *
	 */
	iDeltaY = g_iYofZoomedPicShowInCenter - iStartYofPicData;
	iStartYofVideoMen = (g_tManualPagePictureLayout.iTopLeftY + iPictureLayoutHeight / 2)- iDeltaY;
	//���Ҫ��ʾ��ͼƬ��ʼλ�ô���
	if ( iStartYofVideoMen < g_tManualPagePictureLayout.iTopLeftY )
	{
		iStartYofVideoMen = g_tManualPagePictureLayout.iTopLeftY;
	}
	if ( iStartYofVideoMen > g_tManualPagePictureLayout.iLowerRightY )
	{
		iStartYofVideoMen = g_tManualPagePictureLayout.iLowerRightY + 1;
	}

	//����ʵ�ʵ���ʾ���
	if ( ( ptZoomedPicPixelDatas->iHigh - iStartYofPicData ) > ( g_tManualPagePictureLayout.iLowerRightY - iStartYofVideoMen + 1 ) )
	{
		iHeightPictureInPlay = (g_tManualPagePictureLayout.iLowerRightY - iStartYofVideoMen + 1);
	}
	else
	{
		iHeightPictureInPlay = (ptZoomedPicPixelDatas->iHigh - iStartYofPicData);
	}

	//���ָ���������ɫ
	SetColorForAppointArea ( g_tManualPagePictureLayout.iTopLeftX,g_tManualPagePictureLayout.iTopLeftY,\
	                         g_tManualPagePictureLayout.iLowerRightX,g_tManualPagePictureLayout.iLowerRightY,\
	                         ptVideoMem,COLOR_BACKGROUND );
	//����ͼƬ���Դ���
	PicMergeRegion ( iStartXofPicData, iStartYofPicData, iStartXofVideoMen, iStartYofVideoMen, iWidthPictureInPlay, iHeightPictureInPlay, \
	                 ptZoomedPicPixelDatas, &ptVideoMem->tVideoMemDesc );

}

/**********************************************************************
 * �������ƣ� DistanceBetweenTwoPoint
 * ���������� ������������ľ���, Ϊ�򻯼���, ���ؾ����ƽ��ֵ
 * ��������� ptInputEvent1 - ����1
              ptInputEvent1 - ����2
 * ��������� ��
 * �� �� ֵ�� ��������ƽ��ֵ
 ***********************************************************************/
static int DistanceBetweenTwoPoint ( PT_InputEvent ptInputEvent1, PT_InputEvent ptInputEvent2 )
{
	return ( ptInputEvent1->iX - ptInputEvent2->iX ) * ( ptInputEvent1->iX - ptInputEvent2->iX ) + \
	       ( ptInputEvent1->iY - ptInputEvent2->iY ) * ( ptInputEvent1->iY - ptInputEvent2->iY );

}


static void ManualPageRun ( PT_PageParams ptPageParams )
{
	PT_VideoMem ptDevVideoMem;
	int iZoomedWidth, iZoomedHeight;
	PT_PhotoDesc ptZoomedPicPixelDatas = &g_tZoomedPicPixelDatas;
	T_InputEvent tInputEvent,tInputEventPrePress;
	int iIndex,iIndexPressed=-1,bPressed = 0,bSlide = 0,iError,iPicFileIndex;
    char strDirName[256];
	char strFileName[256];
    char strFullPathName[256]; 
    char * pcTmp;
	T_PageParams tPageParams;
	
    PT_DirContent* aptDirContents;  /* ����:����Ŀ¼��"������Ŀ¼","�ļ�"������ */
	int iDirContentsNumber;		 /* g_aptDirContents�����ж����� */

	//�����ʾ�豸�Դ�
	ptDevVideoMem = GetDevVideoMen();
	

	/* 1. ��ʾҳ�� */	
    //�����Ҫ��ʾ�ļ��ľ���·��
    strncpy(g_cstrFileName,ptPageParams->strCurPictureFile,256);
	ShowPage ( &g_tManualPageDesc );

   
    //��ȡ�ļ�Ŀ¼��               �����Ŀ¼����ʽӦ���� //mnt//tmp//lib//pkgconfig/xxx
    strncpy(strDirName,ptPageParams->strCurPictureFile,256);
	pcTmp = strrchr ( strDirName,'/' );
	*pcTmp = '\0'; //��'/'�滻�ɽ�����
	DBG_PRINTF("strDirName : %s\r\n",strDirName);

	/* ȡ���ļ��� */
	strcpy(strFileName,pcTmp+1);
	DBG_PRINTF("strFileName : %s\r\n",strFileName);

	/* ��õ�ǰĿ¼������Ŀ¼���ļ������� */
    *pcTmp = '/';    //��strDirName���ݴ�//mnt//Icon����Ϊ //mnt//Icon/ ��Ȼ�޷�����ļ�������(ԭ����)
	*(pcTmp+1) = '\0';
    iError = GetDirContents ( strDirName, &aptDirContents, &iDirContentsNumber );
	
	/* ȷ����ǰ��ʾ������һ���ļ� */
	for(iPicFileIndex=0;iPicFileIndex<iDirContentsNumber;iPicFileIndex++)
	{
		if(strcmp(strFileName,aptDirContents[iPicFileIndex]->strName)==0 )
		{
          break;
		}
	}

	DBG_PRINTF("iPicFileIndex = %d\r\n",iPicFileIndex);

	while ( 1 )
	{


		iIndex = GenericGetInputEvent ( g_atManualPageIconsLayout, &tInputEvent );
		if ( tInputEvent.iPressure == 0 )
		{
			/* ������ɿ� */
			if ( bPressed )
			{
				//bFast = 0;

				/* �����а�ť������ */
				ReleaseButton ( &g_atManualPageIconsLayout[iIndexPressed] );
				bPressed = 0;

				if ( iIndexPressed == iIndex ) /* ���º��ɿ�����ͬһ����ť */
				{
					switch ( iIndexPressed )
					{
						case 0: //���ذ���

							return ;
							break;

						case 1://��С
							//��ȡ��С��ĳ���
							iZoomedWidth  = ( float ) g_tZoomedPicPixelDatas.iWidth * ZOOM_RATIO;
							iZoomedHeight = ( float ) g_tZoomedPicPixelDatas.iHigh * ZOOM_RATIO ;

							//���¶�ͼƬԭʼ���ݽ�������
							ptZoomedPicPixelDatas = GetZoomedPicPixelDatas ( &g_tOriginPicPixelDatas,iZoomedWidth,iZoomedHeight );

							//���¼������ĵ�����
							g_iXofZoomedPicShowInCenter = ( float ) g_iXofZoomedPicShowInCenter * ZOOM_RATIO;
							g_iYofZoomedPicShowInCenter = ( float ) g_iYofZoomedPicShowInCenter * ZOOM_RATIO;

							//��ʾͼƬ
							ShowZoomedPictureInLayout ( ptZoomedPicPixelDatas,ptDevVideoMem );

							break;

						case 2://�Ŵ�
							//��ȡ��С��ĳ���
							iZoomedWidth  = ( float ) g_tZoomedPicPixelDatas.iWidth / ZOOM_RATIO;
							iZoomedHeight = ( float ) g_tZoomedPicPixelDatas.iHigh / ZOOM_RATIO ;

							//���¶�ͼƬԭʼ���ݽ�������
							ptZoomedPicPixelDatas = GetZoomedPicPixelDatas ( &g_tOriginPicPixelDatas,iZoomedWidth,iZoomedHeight );

							//���¼������ĵ�����
							g_iXofZoomedPicShowInCenter = ( float ) g_iXofZoomedPicShowInCenter / ZOOM_RATIO;
							g_iYofZoomedPicShowInCenter = ( float ) g_iYofZoomedPicShowInCenter / ZOOM_RATIO;

							//��ʾͼƬ
							ShowZoomedPictureInLayout ( ptZoomedPicPixelDatas,ptDevVideoMem );


							break;

						case 3://��һ�� ��Ŀ¼����һ�� 
						    //�ҵ�һ�ŷ���������ͼƬ����ʾ
						    while(iPicFileIndex >0)
						    {
							   iPicFileIndex--;
							   //��ȡ�ļ���
                               snprintf ( strFullPathName,255,"%s/%s",strDirName,aptDirContents[iPicFileIndex]->strName ); //���ɾ���·��

							   DBG_PRINTF("strFullPathName : %s\r\n",strFullPathName);
							   strFullPathName[255] = '\0';
							   //�ж��Ƿ�֧�ִ˸�ʽ
							   if(isPictureFileSupported(strFullPathName)==0)
							   {
							     ShowPictureInManualPage(ptDevVideoMem,strFullPathName);
								 break;
							   }
						    }
							break;
						case 4://��һ�� ��Ŀ¼����һ��
							while(iPicFileIndex < iDirContentsNumber - 1)
						    {

							   iPicFileIndex++;
							   snprintf ( strFullPathName,255,"%s/%s",strDirName,aptDirContents[iPicFileIndex]->strName ); //���ɾ���·��
							   DBG_PRINTF("strFullPathName : %s\r\n",strFullPathName);

							   strFullPathName[255] = '\0';
							   
							   //�ж��Ƿ�֧�ִ˸�ʽ
							   if(isPictureFileSupported(strFullPathName)==0)
							   {
							     ShowPictureInManualPage(ptDevVideoMem,strFullPathName);
								 break;
							   }
							   
						    }
							break;

						case 5://�������Զ����Ŵ�Ŀ¼�µ�����ͼƬ�ļ�
                            strncpy(tPageParams.strCurPictureFile,strDirName,256);
                            tPageParams.PageID = ID(g_tManualPageDesc.name);
							Page ( "auto" )->Run(&tPageParams);
						    ShowPage ( &g_tManualPageDesc );
							break;


						default: 

							break;
					}
				}

				iIndexPressed = -1;
			}
		}
		else
		{
			/* ������������ǿ��ư������� */
			if ( iIndex != -1 )
			{
				if ( !bPressed  )
				{
					/* δ�����°�ť */
					bPressed = 1;
					iIndexPressed = iIndex;
					//tInputEventPrePress = tInputEvent;  /* ��¼���� */
					PressButton ( &g_atManualPageIconsLayout[iIndexPressed] );
				}

			}
			else //���µ�������ͼƬ��ʾ����
			{
				if ( ( !bPressed ) && ( !bSlide ) )
				{
					bSlide = 1;
					tInputEventPrePress = tInputEvent;
				}

				if ( bSlide )
				{   
				    //�Ƚ����ΰ��µ�����ֵ���� ������Ͼͽ���ͼƬ��������
					if ( DistanceBetweenTwoPoint ( &tInputEvent,&tInputEventPrePress ) > SLIP_MIN_DISTANCE ) 
					{
						//���¼������ĵ�����
						g_iXofZoomedPicShowInCenter -= (tInputEvent.iX - tInputEventPrePress.iX) ;
						g_iYofZoomedPicShowInCenter -= (tInputEvent.iY - tInputEventPrePress.iY) ;

						ShowZoomedPictureInLayout ( ptZoomedPicPixelDatas,ptDevVideoMem );
						tInputEventPrePress = tInputEvent;

					}

				}

			}
		}

	}


}


int ManualPageInit ( void )
{
	return RegisterPageAction ( &g_tManualPageDesc );
}



