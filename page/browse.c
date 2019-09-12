
#include <config.h>
#include <disp_manager.h>
#include <page_manager.h>
#include <pic_manager.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <input_manager.h>
#include <render.h>

static int CalcBrowsePageLayout ( PT_Layout atLayout );
static int BrowsePageSpecialDis ( PT_VideoMem ptVideoMem );
static void BrowsePageRun ( void );

/* ��������ĳĿ¼������� */
static PT_DirContent* g_aptDirContents;  /* ����:����Ŀ¼��"������Ŀ¼","�ļ�"������ */
static int g_iDirContentsNumber;         /* g_aptDirContents�����ж����� */

//��ǰ·��
static char g_strCurDir[256] = DEFAULT_PATH;
static char g_strSelectDir[256] = DEFAULT_PATH;



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
	.name   = "manual",
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
	int iTopLeftX, iTopLeftY;
	int iBotRightX, iBotRightY;
	int iIconWidth, iIconHight;
	int iNumOfOneRowIcon , iNumOfOneLineIcon;
	int iDeltaX,iDeltaY;

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
		iTopLeftY = 0 + g_atBrowsePageIconsLayout.iLowerRightY + 1;
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
		iTopLeftX = 0 + g_atBrowsePageIconsLayout.iLowerRightX + 1;
		iTopLeftY = 0;
		iBotRightX = iXres - 1;
		iBotRightY = iYres - 1;

	}

	/* ȷ��һ����ʾ���ٸ�"Ŀ¼���ļ�", ��ʾ������ */
	iIconHight = DIR_FILE_NAME_WIDTH;
	iIconWidth = iIconHight;

	//ͼ����ͼ��֮��ľ�����Ҫ����10����
	iNumOfOneRowIcon = ( ( iBotRightX -iTopLeftX ) + 1 ) / iIconWidth; //�ȵõ�ÿ�������ʾ����
	while ( 1 )
	{
		iDeltaX = ( iBotRightX -iTopLeftX ) + 1 - iIconWidth * iNumOfOneRowIcon; //�õ���ͼ��֮�������x�᳤��
		if ( (iDeltaX /(iNumOfOneRowIcon+1)) < 10 )
		{
			iNumOfOneRowIcon -- ;
		}
		else
		{
			break;
		}
	}

	iNumOfOneLineIcon = ( ( iBotRightY -iTopLeftY ) + 1 ) / iIconWidth; //�ȵõ�ÿ�������ʾ����
	while ( 1 )
	{
		iDeltaY = ( iBotRightY -iTopLeftY ) + 1 - iIconWidth * iNumOfOneLineIcon; //�õ���ͼ��֮�������Y�᳤��
		if ( (iDeltaY /(iNumOfOneLineIcon+1)) < 10 )
		{
			iNumOfOneLineIcon -- ;
		}
		else
		{
			break;
		}
	}

	iDeltaX = iDeltaX / (iNumOfOneRowIcon + 1); //�õ�ÿ��ͼ����
	iDeltaY = iDeltaY / (iNumOfOneRowIcon + 1); 




}

static int CalcBrowsePageLayout ( PT_Layout atLayout )
{

	//�����Ļ�ֱ���
	int iXres,iYres,iBpp,i = 0;
	int iIconWidth,iIconHight,IconX,IconY;
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

	return 0;

}


static int BrowsePageSpecialDis ( PT_VideoMem ptVideoMem )
{




}

static void BrowsePageRun ( void )
{
	int iError;
	PT_VideoMem ptDevVideoMem;

	//�����ʾ�豸�Դ�
	ptDevVideoMem = GetDevVideoMen();

	/* 0. ���Ҫ��ʾ��Ŀ¼������ */
	iError = GetDirContents ( g_strCurDir, &g_aptDirContents, &g_iDirContentsNumber );
	if ( iError )
	{
		DBG_PRINTF ( "GetDirContents error ... \r\n" );
		return -1;
	}

	/* 1. ��ʾҳ�� */
	ShowPage ( &g_tBrowsePageDesc );


	while ( 1 )
	{





	}


}


int BrowsePageInit ( void )
{
	return RegisterPageAction ( &g_tBrowsePageDesc );
}




