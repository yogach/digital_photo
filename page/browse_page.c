
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
#include <render.h>
#include <string.h>

static int CalcBrowsePageLayout ( PT_Layout atLayout );
static int BrowsePageSpecialDis ( PT_VideoMem ptVideoMem );
static void BrowsePageRun ( PT_PageParams ptPageParams );

/* ��������ĳĿ¼������� */
static PT_DirContent* g_aptDirContents;  /* ����:����Ŀ¼��"������Ŀ¼","�ļ�"������ */
static int g_iDirContentsNumber;         /* g_aptDirContents�����ж����� */
static int g_iStartIndex = 0;            /* ����Ļ����ʾ�ĵ�1��"Ŀ¼���ļ�"��g_aptDirContents���������һ�� */


//��ǰ·��
static char g_strCurDir[256] = DEFAULT_PATH;
//static char g_strSelectDir[256] = DEFAULT_PATH;

static int g_iDirFileNumPerCol, g_iDirFileNumPerRow; //ÿ����ÿ����ʾ��ͼ����� COL - �� ROW - ��
static T_Layout* g_atDirAndFileLayout = NULL;    //���ڱ����ļ���/�ļ���ʾҳ��ÿ��ͼ������ֵ
static T_PageLayOut g_tBrowsePageDirAndFileLayout; //���ڱ��������ļ���ʾҳ������� -- Ҳ����g_atDirAndFileLayout����

//���ڴ洢�ļ���/�ļ�ͼ��
static T_PhotoDesc g_tDirClosedIconPixelDatas;
static T_PhotoDesc g_tDirOpenedIconPixelDatas;
static T_PhotoDesc g_tFileIconPixelDatas;
//ͼ����
static char* g_strDirClosedIconName  = "fold_closed.bmp";
static char* g_strDirOpenedIconName  = "fold_opened.bmp";
static char* g_strFileIconName = "file.bmp";



/* ͼ����һ��������, "ͼ��+����"Ҳ��һ��������
 *   --------
 *   |  ͼ  |
 *   |  ��  |
 * ------------
 * |   ����   |
 * ------------
 */

#define DIR_FILE_ICON_WIDTH    40                       //�ļ�ͼ����
#define DIR_FILE_ICON_HEIGHT   DIR_FILE_ICON_WIDTH      //�ļ�ͼ��߶�
#define DIR_FILE_NAME_HEIGHT   20                       //�ļ����߶�
#define DIR_FILE_NAME_WIDTH   (DIR_FILE_ICON_HEIGHT + DIR_FILE_NAME_HEIGHT) //�ļ������
#define DIR_FILE_ALL_WIDTH    DIR_FILE_NAME_WIDTH   //�����������ο��
#define DIR_FILE_ALL_HEIGHT   DIR_FILE_ALL_WIDTH    //�����������θ�

#define DIRFILE_ICON_INDEX_BASE 1000

static T_Layout g_atBrowsePageIconsLayout[] =
{
	{0, 0, 0, 0, "up.bmp"},
	{0, 0, 0, 0, "select.bmp"},
	{0, 0, 0, 0, "pre_page.bmp"},
	{0, 0, 0, 0, "next_page.bmp"},
	{0, 0, 0, 0, NULL},
};


static T_PageDesc g_tBrowsePageDesc =
{
	.name   = "browse",
	.Run    = BrowsePageRun,
	.CalcPageLayout = CalcBrowsePageLayout,
	.atPageLayout = g_atBrowsePageIconsLayout,
	.DispSpecialIcon = BrowsePageSpecialDis,
};

/**********************************************************************
 * �������ƣ� CalcBrowsePageDirAndFilesLayout
 * ���������� ����"Ŀ¼���ļ�"����ʾ����
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 ***********************************************************************/
static int CalcBrowsePageDirAndFilesLayout ( void )
{
	int iXres, iYres, iBpp;
	int iTopLeftX, iTopLeftY, iTopLeftXBak;
	int iBotRightX, iBotRightY;
	int iIconWidth, iIconHight;
	int iNumOfOneColIcon, iNumOfOneRowIcon;//COL - �� ROW - ��
	int iDeltaX,iDeltaY;
	int i,j,k = 0;

	GetDispResolution ( &iXres, &iYres, &iBpp ); //��ȡ�ֱ���

	//���Ŀ¼���ļ���ʾ����Ĵ�С
	if ( iXres < iYres )
	{
		/* --------------------------------------
		*	 up select pre_page next_page ͼ��
		* --------------------------------------
		*
		*			Ŀ¼���ļ�
		*
		*
		* --------------------------------------
		*/

		iTopLeftX = 0;
		iTopLeftY = 0 + g_atBrowsePageIconsLayout[0].iLowerRightY + 1;
		iBotRightX = iXres - 1;
		iBotRightY = iYres - 1;
	}
	else
	{
		/*
		 *	  ----------------------------------
		 *	   up      |
		 *             |
		 *    select   |
		 *             |     Ŀ¼���ļ�
		 *    pre_page |
		 *             |
		 *   next_page |
		 *             |
		 *	  ----------------------------------
		 */
		iTopLeftX = 0 + g_atBrowsePageIconsLayout[0].iLowerRightX + 1;
		iTopLeftY = 0;
		iBotRightX = iXres - 1;
		iBotRightY = iYres - 1;

	}

	/* ȷ��һ����ʾ���ٸ�"Ŀ¼���ļ�", ��ʾ������ */
	iIconWidth  = DIR_FILE_NAME_WIDTH;
	iIconHight  = iIconWidth;

	/*ͼ����ͼ��֮��ľ�����Ҫ����10���� */
	//�ȵõ�ÿ�������ʾ����
	iNumOfOneRowIcon = (  iBotRightX -iTopLeftX  + 1 ) / iIconWidth;
	while ( 1 )
	{
		iDeltaX =  iBotRightX -iTopLeftX  + 1 - iIconWidth * iNumOfOneRowIcon; //�õ���ͼ��֮�������x�᳤��
		if ( ( iDeltaX / ( iNumOfOneRowIcon+1 ) ) < 10 )
		{
			iNumOfOneRowIcon -- ; //����ͼ�����
		}
		else
		{
			break;
		}
	}

	//�ȵõ�ÿ�������ʾ����
	iNumOfOneColIcon = (  iBotRightY -iTopLeftY  + 1 ) / iIconHight;
	while ( 1 )
	{
		iDeltaY =  iBotRightY - iTopLeftY  + 1 - iIconHight * iNumOfOneColIcon; //�õ���ͼ��֮�������Y�᳤��
		if ( ( iDeltaY / ( iNumOfOneColIcon+1 ) ) < 10 )
		{
			iNumOfOneColIcon -- ;
		}
		else
		{
			break;
		}
	}

	iDeltaX = iDeltaX / ( iNumOfOneRowIcon + 1 ); //�õ�ÿ��ͼ����
	iDeltaY = iDeltaY / ( iNumOfOneColIcon + 1 );

	g_iDirFileNumPerCol =  iNumOfOneColIcon;//һ����ʾ����ͼ��
	g_iDirFileNumPerRow =  iNumOfOneRowIcon;//һ����ʾ���ٸ�ͼ��


	/* ������ʾ iNumPerRow * iNumPerCol��"Ŀ¼���ļ�"
	 * ����"����+1"��T_Layout�ṹ��: һ��������ʾͼ��,��һ��������ʾ����
	 * ���һ��������NULL,�����жϽṹ�������ĩβ
	 */
	if(g_atDirAndFileLayout == NULL)
	{
		g_atDirAndFileLayout = malloc ( sizeof ( T_Layout ) * ( 2*g_iDirFileNumPerCol*g_iDirFileNumPerRow + 1 ) );
        DBG_PRINTF("malloc g_atDirAndFileLayout...\r\n");
		if ( g_atDirAndFileLayout == NULL )
		{
			DBG_PRINTF ( "malloc error..\r\n" );
			return -1;
		}
	}
	// ���"Ŀ¼���ļ�"������������Ͻǡ����½�����
	g_tBrowsePageDirAndFileLayout.iTopLeftX = iTopLeftX;
	g_tBrowsePageDirAndFileLayout.iTopLeftY = iTopLeftY;
	g_tBrowsePageDirAndFileLayout.iLowerRightX = iBotRightX;
	g_tBrowsePageDirAndFileLayout.iLowerRightY = iBotRightY;
	g_tBrowsePageDirAndFileLayout.iBpp = iBpp;
	g_tBrowsePageDirAndFileLayout.iMaxTotalBytes = DIR_FILE_ALL_WIDTH * DIR_FILE_ALL_HEIGHT * iBpp / 8;
	g_tBrowsePageDirAndFileLayout.atLayOut = g_atDirAndFileLayout;


	/* ȷ��ͼ������ֵ�λ��
	*
	* ͼ����һ��������, "ͼ��+����"Ҳ��һ��������
	*   --------
	*   |  ͼ  |
	*   |  ��  |
	* ------------
	* |   ����   |
	* ------------
	*/
	//���ȵõ���һ���ļ������
	iTopLeftX +=iDeltaX;
	iTopLeftY +=iDeltaY;
	iTopLeftXBak = iTopLeftX;

	for ( i = 0; i< g_iDirFileNumPerCol ; i++ )
	{
		for ( j = 0; j< g_iDirFileNumPerRow ; j++ )
		{
			g_atDirAndFileLayout[k].iTopLeftX = iTopLeftX + ( DIR_FILE_ALL_WIDTH - DIR_FILE_ICON_WIDTH ) /2;
			g_atDirAndFileLayout[k].iTopLeftY = iTopLeftY;
			g_atDirAndFileLayout[k].iLowerRightX = g_atDirAndFileLayout[k].iTopLeftX + DIR_FILE_ICON_WIDTH - 1;
			g_atDirAndFileLayout[k].iLowerRightY = g_atDirAndFileLayout[k].iTopLeftY + DIR_FILE_ICON_HEIGHT- 1;

			g_atDirAndFileLayout[k+1].iTopLeftX = iTopLeftX;
			g_atDirAndFileLayout[k+1].iTopLeftY = g_atDirAndFileLayout[k].iLowerRightY + 1;
			g_atDirAndFileLayout[k+1].iLowerRightX = g_atDirAndFileLayout[k+1].iTopLeftX + DIR_FILE_NAME_WIDTH - 1;
			g_atDirAndFileLayout[k+1].iLowerRightY = g_atDirAndFileLayout[k+1].iTopLeftY + DIR_FILE_NAME_HEIGHT- 1;

			k +=2;
			iTopLeftX +=DIR_FILE_ALL_WIDTH + iDeltaX;//�õ�ͬһ�еڶ���ͼ�����ʾλ��
		}

		iTopLeftX = iTopLeftXBak;//���к�x����ص���ʼ
		iTopLeftY += DIR_FILE_ALL_HEIGHT + iDeltaY;//Y�������ͼ������߶�����

	}


	/* ��β */
	g_atDirAndFileLayout[k].iTopLeftX	 = 0;
	g_atDirAndFileLayout[k].iLowerRightX  = 0;
	g_atDirAndFileLayout[k].iTopLeftY	 = 0;
	g_atDirAndFileLayout[k].iLowerRightY  = 0;
	g_atDirAndFileLayout[k].IconName = NULL;


	return 0;

}

/**********************************************************************
 * �������ƣ� GenerateDirAndFileIcons
 * ���������� Ϊ"���ҳ��"���ɲ˵������е�ͼ��--�ļ���ͼ�����ļ�ͼ��
 * ��������� ptPageLayout - �ں����ͼ����ļ�������ʾ����
 * ��������� ��
 * �� �� ֵ�� 0      - �ɹ�
 *            ����ֵ - ʧ��
 ***********************************************************************/
static int GenerateDirAndFileIcons ( PT_PageLayOut ptPageLayout )
{
	int iXres, iYres, iBpp;
	int iError;
	T_PhotoDesc tOriPhotoDesc;
	PT_Layout atLayOut =  ptPageLayout->atLayOut;

	GetDispResolution ( &iXres, &iYres, &iBpp ); //��ȡ�ֱ���

	g_tDirClosedIconPixelDatas.iBpp =iBpp ;
	g_tDirClosedIconPixelDatas.aucPhotoData = malloc ( ptPageLayout ->iMaxTotalBytes );
	if ( g_tDirClosedIconPixelDatas.aucPhotoData == NULL )
	{
		DBG_PRINTF ( "malloc error.. \r\n" );
		return -1;
	}


	g_tDirOpenedIconPixelDatas.iBpp =iBpp ;
	g_tDirOpenedIconPixelDatas.aucPhotoData = malloc ( ptPageLayout ->iMaxTotalBytes );
	if ( g_tDirOpenedIconPixelDatas.aucPhotoData == NULL )
	{
		DBG_PRINTF ( "malloc error.. \r\n" );
		return -1;
	}

	g_tFileIconPixelDatas.iBpp =iBpp ;
	g_tFileIconPixelDatas.aucPhotoData = malloc ( ptPageLayout ->iMaxTotalBytes );
	if ( g_tFileIconPixelDatas.aucPhotoData == NULL )
	{
		DBG_PRINTF ( "malloc error.. \r\n" );
		return -1;
	}

	/* ��BMP�ļ�����ȡͼ������ */
	/* 1. ��ȡ"fold_closedͼ��" */
	iError = GetPixelDatasFormIcon ( g_strDirClosedIconName,&tOriPhotoDesc );
	if ( iError!=0 )
	{
		DBG_PRINTF ( "GetPixelDatasFormIcon error.. \r\n" );
		return -1;
	}
	g_tDirClosedIconPixelDatas.iHigh = atLayOut[0].iLowerRightY - atLayOut[0].iTopLeftY + 1;
	g_tDirClosedIconPixelDatas.iWidth = atLayOut[0].iLowerRightX - atLayOut[0].iTopLeftX + 1;
	g_tDirClosedIconPixelDatas.iLineBytes = g_tDirClosedIconPixelDatas.iWidth * g_tDirClosedIconPixelDatas.iBpp / 8;
	g_tDirClosedIconPixelDatas.iTotalBytes = g_tDirClosedIconPixelDatas.iLineBytes * g_tDirClosedIconPixelDatas.iHigh;
	PicZoom ( &tOriPhotoDesc,&g_tDirClosedIconPixelDatas );
	FreePixelDatasForIcon ( &tOriPhotoDesc );


	/* 2. ��ȡ"fold_openedͼ��" */
	iError = GetPixelDatasFormIcon ( g_strDirOpenedIconName,&tOriPhotoDesc );
	if ( iError!=0 )
	{
		DBG_PRINTF ( "GetPixelDatasFormIcon error.. \r\n" );
		return -1;
	}
	g_tDirOpenedIconPixelDatas.iHigh = atLayOut[0].iLowerRightY - atLayOut[0].iTopLeftY + 1;
	g_tDirOpenedIconPixelDatas.iWidth = atLayOut[0].iLowerRightX - atLayOut[0].iTopLeftX + 1;
	g_tDirOpenedIconPixelDatas.iLineBytes = g_tDirOpenedIconPixelDatas.iWidth * g_tDirOpenedIconPixelDatas.iBpp / 8;
	g_tDirOpenedIconPixelDatas.iTotalBytes = g_tDirOpenedIconPixelDatas.iLineBytes * g_tDirOpenedIconPixelDatas.iHigh;
	PicZoom ( &tOriPhotoDesc,&g_tDirOpenedIconPixelDatas );
	FreePixelDatasForIcon ( &tOriPhotoDesc );


	/* 3. ��ȡ"fileͼ��" */
	iError = GetPixelDatasFormIcon ( g_strFileIconName,&tOriPhotoDesc );
	if ( iError!=0 )
	{
		DBG_PRINTF ( "GetPixelDatasFormIcon error.. \r\n" );
		return -1;
	}
	g_tFileIconPixelDatas.iHigh = atLayOut[0].iLowerRightY - atLayOut[0].iTopLeftY + 1;
	g_tFileIconPixelDatas.iWidth = atLayOut[0].iLowerRightX - atLayOut[0].iTopLeftX + 1;
	g_tFileIconPixelDatas.iLineBytes = g_tFileIconPixelDatas.iWidth * g_tFileIconPixelDatas.iBpp / 8;
	g_tFileIconPixelDatas.iTotalBytes = g_tFileIconPixelDatas.iLineBytes * g_tFileIconPixelDatas.iHigh;
	PicZoom ( &tOriPhotoDesc,&g_tFileIconPixelDatas );
	FreePixelDatasForIcon ( &tOriPhotoDesc );

	return 0;
}

/**********************************************************************
 * �������ƣ� GenerateDirAndFileIcons
 * ���������� Ϊ"���ҳ��"���ɲ˵������е�ͼ��--���ư�ťҳ��
 * ��������� ptPageLayout - �ں����ͼ����ļ�������ʾ����
 * ��������� ��
 * �� �� ֵ�� 0      - �ɹ�
 *            ����ֵ - ʧ��
 ***********************************************************************/
static int CalcBrowsePageLayout ( PT_Layout atLayout )
{

	//�����Ļ�ֱ���
	int iXres,iYres,iBpp,i = 0;
	int iIconWidth,iIconHight;
	GetDispResolution ( &iXres,&iYres,&iBpp ); //��ȡLCD�ֱ���

	if ( iXres < iYres )
	{
		/*	 iXres/4
		 *	  ----------------------------------
		 *	   up	select	pre_page  next_page
		 *
		 *
		 *
		 *
		 *
		 *
		 *	  ----------------------------------
		 */
		iIconWidth = iXres/4;
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

		/*	 iYres/4
		 *	  ----------------------------------
		 *	   up
		 *
		 *    select
		 *
		 *    pre_page
		 *
		 *   next_page
		 *
		 *	  ----------------------------------
		 */

		iIconHight = iYres/4;
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

	CalcBrowsePageDirAndFilesLayout();

	if ( !g_tDirClosedIconPixelDatas.aucPhotoData )
	{
		GenerateDirAndFileIcons ( &g_tBrowsePageDirAndFileLayout );
	}


	return 0;

}


/**********************************************************************
 * �������ƣ� GenerateBrowsePageDirAndFile
 * ���������� Ϊ"���ҳ��"����"Ŀ¼���ļ�"�����е�ͼ�������,������ʾĿ¼����
 * ��������� iStartIndex        - ����Ļ����ʾ�ĵ�1��"Ŀ¼���ļ�"��aptDirContents���������һ��
 *            iDirContentsNumber - aptDirContents�����ж�����
 *            aptDirContents     - ����:����Ŀ¼��"������Ŀ¼","�ļ�"������
 *            ptVideoMem         - �����VideoMem�й���ҳ��
 * ��������� ��
 * �� �� ֵ�� 0      - �ɹ�
 *            ����ֵ - ʧ��
 ***********************************************************************/
static int GenerateBrowsePageDirAndFile ( int iStartIndex, int iDirContentsNumber, PT_DirContent* aptDirContents, PT_VideoMem ptVideoMem )
{

	PT_PageLayOut ptPageLayout= &g_tBrowsePageDirAndFileLayout;

	PT_Layout  atFileAndDirLayout = ptPageLayout->atLayOut;
	int i,j,k = 0;
	int iDirContentIndex = iStartIndex;
	int iError;

	//1���������ָ���������ʾ����
	SetColorForAppointArea ( ptPageLayout->iTopLeftX,ptPageLayout->iTopLeftY,ptPageLayout->iLowerRightX,ptPageLayout->iLowerRightY,ptVideoMem,COLOR_BACKGROUND );

	//2�������ļ���/�ļ� ���������С
	SetFontSize ( atFileAndDirLayout[1].iLowerRightY - atFileAndDirLayout[1].iTopLeftY + 1 - 5 );

    DBG_PRINTF("iDirContentsNumber:%d \r\n",iDirContentsNumber); //��ӡ���������ļ�����
	//3����ָ��λ�ÿ�������
	for ( i = 0; i< g_iDirFileNumPerCol ; i++ )
	{
		for ( j = 0; j< g_iDirFileNumPerRow ; j++ )
		{
			if ( iDirContentIndex < iDirContentsNumber )
			{

				if ( aptDirContents[iDirContentIndex]->eFileType == FILETYPE_DIR ) //���Ŀ��Ŀ¼���ļ��� ����ָ��λ����ʾ�ļ���ͼ��
				{
					PicMerge ( atFileAndDirLayout[k].iTopLeftX, atFileAndDirLayout[k].iTopLeftY,&g_tDirClosedIconPixelDatas,&ptVideoMem->tVideoMemDesc );
				}
				else
				{
					PicMerge ( atFileAndDirLayout[k].iTopLeftX, atFileAndDirLayout[k].iTopLeftY,&g_tFileIconPixelDatas,&ptVideoMem->tVideoMemDesc );
				}

				k++;

				iError = MergerStringToCenterOfRectangleInVideoMem ( atFileAndDirLayout[k].iTopLeftX, atFileAndDirLayout[k].iTopLeftY,atFileAndDirLayout[k].iLowerRightX, atFileAndDirLayout[k].iLowerRightY, ( unsigned char* ) aptDirContents[iDirContentIndex]->strName,ptVideoMem );
				k++;
				iDirContentIndex += 1;

			}
			else
			{
				break;
			}

		}

		if ( iDirContentIndex >= iDirContentsNumber )
		{
			break;
		}

	}

	return 0;

}


/**********************************************************************
 * �������ƣ� BrowsePageSpecialDis
 * ���������� ������ʾ����ͼ��
 * ��������� ptVideoMem         - �����VideoMem�й���ҳ��
 * ��������� ��
 * �� �� ֵ�� 0      - �ɹ�
 *            ����ֵ - ʧ��
 ***********************************************************************/
static int BrowsePageSpecialDis ( PT_VideoMem ptVideoMem )
{
	return GenerateBrowsePageDirAndFile ( g_iStartIndex,g_iDirContentsNumber,g_aptDirContents,ptVideoMem );
}
/**********************************************************************
 * �������ƣ� FlushDirAndFile
 * ���������� ˢ���ļ���/�ļ��������ʾ
 * ��������� ptVideoMem         - �����VideoMem�й���ҳ��
 * ��������� ��
 * �� �� ֵ�� 0      - �ɹ�
 *            ����ֵ - ʧ��
 ***********************************************************************/
static int FlushDirAndFile ( PT_VideoMem ptVideoMem )
{
	int iError;

	//�ڻ�ȡ���ļ�������֮ǰ ���ͷ�ԭ�ȷ�����ڴ�
	FreeDirContents ( g_aptDirContents,g_iDirContentsNumber );
	iError = GetDirContents ( g_strCurDir, &g_aptDirContents, &g_iDirContentsNumber );
	//DBG_PRINTF ( "get dir or file num is %d\r\n",g_iDirContentsNumber );
	if ( iError )
	{
		DBG_PRINTF ( "GetDirContents error ... \r\n" );
		return -1;
	}

	g_iStartIndex = 0;
	//�����µ�ҳ��
	iError = GenerateBrowsePageDirAndFile ( g_iStartIndex,g_iDirContentsNumber,g_aptDirContents,ptVideoMem );
	if ( iError!=0 )
	{
		DBG_PRINTF ( "GenerateBrowsePageDirAndFile error..\r\n" );
		return -1;
	}
	return 0;

}

static void ChangeDirOrFileArenStatus ( int iDirFileIndex,int bSelect,PT_VideoMem ptVideoMem )
{
	int iIndex = g_iStartIndex + iDirFileIndex /2;//�õ��ļ�������λ��


	//�����Ŀ¼ �ı��ļ���Ϊ��״̬
	if ( g_aptDirContents[iIndex]->eFileType == FILETYPE_DIR )
	{
		if ( bSelect )
		{
			PicMerge ( g_atDirAndFileLayout[iDirFileIndex].iTopLeftX, g_atDirAndFileLayout[iDirFileIndex].iTopLeftY, &g_tDirOpenedIconPixelDatas, &ptVideoMem->tVideoMemDesc );
		}
		else
		{
			PicMerge ( g_atDirAndFileLayout[iDirFileIndex].iTopLeftX, g_atDirAndFileLayout[iDirFileIndex].iTopLeftY, &g_tDirClosedIconPixelDatas, &ptVideoMem->tVideoMemDesc );
		}
	}
	else//������ļ� �ļ�ͼ��������������ɫȡ��
	{
		if ( bSelect )
		{
			PressButton ( &g_atDirAndFileLayout[iDirFileIndex] );
			PressButton ( &g_atDirAndFileLayout[iDirFileIndex + 1] );
		}
		else
		{
			ReleaseButton ( &g_atDirAndFileLayout[iDirFileIndex] );
			ReleaseButton ( &g_atDirAndFileLayout[iDirFileIndex + 1] );
		}

	}
}

static void BrowsePageRun ( PT_PageParams ptPageParams )
{
	int iError;
	PT_VideoMem ptDevVideoMem;
	T_InputEvent tInputEvent,tPreInputEvent;
	int iIndex,iIndexPressured=-1,bPressure = 0, bHaveClickSelectIcon = 0;
	int iPressIndex;
	char strtmp[256] ;
	char* ptTmp;

	//�����ʾ�豸�Դ�
	ptDevVideoMem = GetDevVideoMen();

	/* 0. ���Ҫ��ʾ��Ŀ¼������ */
	iError = GetDirContents ( g_strCurDir, &g_aptDirContents, &g_iDirContentsNumber );
	if ( iError )
	{
		DBG_PRINTF ( "GetDirContents error ... \r\n" );
		//return -1;
	}

	/* 1. ��ʾҳ�� */
	ShowPage ( &g_tBrowsePageDesc );

	//��ȡ�����¼�����
	while ( 1 )
	{

		iIndex = GenericGetInputEvent ( g_atBrowsePageIconsLayout,&tInputEvent );

		if ( iIndex == -1 ) //������µĵط�û�������ҳ����ư�����
		{
			iIndex =  GenericGetInputPositionInPageLayout ( g_atDirAndFileLayout,&tInputEvent );
			//DBG_PRINTF("press Icon Number is %d\r\n",iIndex); 

			if ( iIndex != -1 ) //������µĵط����ļ���/�ļ����� ��ʱ�õ��������ǰ������������
			{
				//�ж�����������Ƿ���ͼ�� ��2����Ϊÿ����ʾͼ�����һ��ͼ���һ���ļ���
				if ( ( g_iStartIndex + iIndex/2 ) < g_iDirContentsNumber )
				{
					iIndex += DIRFILE_ICON_INDEX_BASE;
					//DBG_PRINTF("press Icon Number is %d\r\n",iIndex);
				}
				else
				{
					iIndex = -1;
				}
			}

		}

		if ( tInputEvent.iPressure == 0 ) //������ɿ�״̬
		{
			if ( bPressure ) //�����������
			{

				if ( iIndex < DIRFILE_ICON_INDEX_BASE ) //�����ǿ���������
				{
					//�ı䰴���������ɫ
					ReleaseButton ( &g_atBrowsePageIconsLayout[iIndex] );
					bPressure = 0;

					if ( iIndexPressured == iIndex ) //����������ɿ�����ͬһ������
					{

						switch ( iIndexPressured )
						{
							case 0://����
								if ( strcmp ( g_strCurDir,"/" ) ==0 ) //����Ѿ��Ƕ���Ŀ¼ ����
								{
									FreeDirContents ( g_aptDirContents,g_iDirContentsNumber );
									return ;
								}

								/*��g_strCurDirĩβ��ʼ���ҵ��ַ�"/"��λ�� ����λ���������ָ�� ���δ���ҵ�����NULL
								 *��Ҫִ����������Ϊg_strCurDir������//mnt/
								 *
								 */
								ptTmp = strrchr ( g_strCurDir,'/' );
								*ptTmp = '\0'; //��'/'�滻�ɽ�����
								ptTmp = strrchr ( g_strCurDir,'/' );
								*ptTmp = '\0'; //��'/'�滻�ɽ�����

								/*
								FreeDirContents ( g_aptDirContents,g_iDirContentsNumber );
								iError = GetDirContents ( g_strCurDir, &g_aptDirContents, &g_iDirContentsNumber );
								if ( iError )
								{
									DBG_PRINTF ( "GetDirContents error ... \r\n" );
									//return -1;
								}

								g_iStartIndex = 0;
								iError = GenerateBrowsePageDirAndFile ( g_iStartIndex,g_iDirContentsNumber,g_aptDirContents,ptDevVideoMem );
								if ( iError!=0 )
								{
									DBG_PRINTF ( "GenerateBrowsePageDirAndFile error..\r\n" );
								}*/
								FlushDirAndFile ( ptDevVideoMem );

								break;

							case 1://ѡ�� --��ʱ����

								break;

							case 2://��һҳ
								g_iStartIndex -= g_iDirFileNumPerCol* g_iDirFileNumPerRow ;
								if ( g_iStartIndex <0 )
								{
									g_iStartIndex += g_iDirFileNumPerCol* g_iDirFileNumPerRow ;
								}
								else
								{
									iError = GenerateBrowsePageDirAndFile ( g_iStartIndex,g_iDirContentsNumber,g_aptDirContents,ptDevVideoMem );
									if ( iError!=0 )
									{
										DBG_PRINTF ( "GenerateBrowsePageDirAndFile error..\r\n" );
									}
								}
								break;
							case 3://��һҳ
								g_iStartIndex += g_iDirFileNumPerCol* g_iDirFileNumPerRow ;
								if ( g_iStartIndex >= g_iDirContentsNumber )
								{
									g_iStartIndex -= g_iDirFileNumPerCol* g_iDirFileNumPerRow ;
								}
								else
								{
									iError = GenerateBrowsePageDirAndFile ( g_iStartIndex,g_iDirContentsNumber,g_aptDirContents,ptDevVideoMem );
									if ( iError!=0 )
									{
										DBG_PRINTF ( "GenerateBrowsePageDirAndFile error..\r\n" );
									}
								}
								break;


							default:
								break;
						}

					}
				}
				else//Ŀ¼���ļ�����
				{
					/*
					 * ������º��ɿ�ʱ, ���㲻����ͬһ��ͼ����, ���ͷ�ͼ��
					 */
					if ( iIndexPressured != iIndex )
					{
						ChangeDirOrFileArenStatus ( iIndexPressured - DIRFILE_ICON_INDEX_BASE,0, ptDevVideoMem );
						bPressure = 0;

					}
					else if ( bHaveClickSelectIcon ) /* ���º��ɿ�����ͬһ����ť, ����"ѡ��"��ť�ǰ���״̬ */
					{
						bPressure = 0;

					}
					else /* "ѡ��"��ť��������ʱ, ����Ŀ¼�����, bUsedToSelectDirΪ0ʱ�����ļ�����ʾ�� */
					{
						bPressure = 0;
						/* �����Ŀ¼, �������Ŀ¼ */
						iPressIndex = g_iStartIndex + ( iIndexPressured - DIRFILE_ICON_INDEX_BASE ) /2;
						if ( g_aptDirContents[iPressIndex]->eFileType == FILETYPE_DIR )
						{
							//���g_strCurDirֵΪ"/"      strtmp��ֵ���� "//mnt/" ��ʱ�ɳɹ���ȡ���ļ�������
							snprintf ( strtmp,256,"%s/%s/",g_strCurDir,g_aptDirContents[iPressIndex]->strName ); //���ɾ���·��

							//DBG_PRINTF("%s\r\n",strtmp);
							strtmp[255]='\0';
							strcpy ( g_strCurDir, strtmp );
							FlushDirAndFile ( ptDevVideoMem );

							/*
							FreeDirContents ( g_aptDirContents,g_iDirContentsNumber );
							iError = GetDirContents ( g_strCurDir, &g_aptDirContents, &g_iDirContentsNumber );
							if ( iError )
							{
								DBG_PRINTF ( "GetDirContents error ... \r\n" );
								//return -1;
							}

							g_iStartIndex = 0;
							iError = GenerateBrowsePageDirAndFile ( g_iStartIndex,g_iDirContentsNumber,g_aptDirContents,ptDevVideoMem );
							if ( iError!=0 )
							{
								DBG_PRINTF ( "GenerateBrowsePageDirAndFile error..\r\n" );
							}
							*/


						}
						else//������ļ��������ʾҳ��
						{
							//��ȡ�����ļ��ľ���·��
							//snprintf ( strtmp,256,"%s/%s/",g_strCurDir,g_aptDirContents[iPressIndex]->strName ); //���ɾ���·��

							//if(isPictureFileSupported(strtmp)==0)
							//{
								Page ( "manual" )->Run(NULL);
								ShowPage ( &g_tBrowsePageDesc );
							//}
						}

					}


				}

				iIndexPressured = -1;
			}
		}
		else //����ǰ���״̬
		{
			if ( iIndex != -1 ) //������µ��������ݵİ���
			{
				if ( !bPressure ) // δ�����°�ť
				{
					bPressure = 1;
					iIndexPressured = iIndex;
					tPreInputEvent = tInputEvent;
					if ( iIndexPressured < DIRFILE_ICON_INDEX_BASE ) //�����µ��ǿ���������
					{
						//�ı䰴���������ɫ
						PressButton ( &g_atBrowsePageIconsLayout[iIndex] );
					}
					else
					{
						//ѡ��ָ���ļ�
						ChangeDirOrFileArenStatus ( iIndex - DIRFILE_ICON_INDEX_BASE,1, ptDevVideoMem );
					}
				}

				//����������ϰ���2�� ֱ�ӷ�����һ����
				if ( ( TimeMSBetween ( tPreInputEvent.tTime,tInputEvent.tTime )  > 2000 ) && ( iIndex == 0 ) )
				{
					FreeDirContents ( g_aptDirContents,g_iDirContentsNumber );
					return ;
				}

			}
		}


	}


}


int BrowsePageInit ( void )
{
	return RegisterPageAction ( &g_tBrowsePageDesc );
}




