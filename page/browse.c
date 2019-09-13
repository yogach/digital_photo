
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

static int g_iDirFileNumPerCol, g_iDirFileNumPerRow; //ÿ����ÿ����ʾ��ͼ�����
static T_Layout *g_atDirAndFileLayout;    //���ڱ����ļ���/�ļ���ʾҳ��ÿ��ͼ������ֵ
static T_PageLayOut g_tBrowsePageDirAndFileLayout; //���ڱ��������ļ���ʾҳ������� --Ҳ��������

//
static T_PhotoDesc g_tDirClosedIconPixelDatas;
static T_PhotoDesc g_tDirOpenedIconPixelDatas;
static T_PhotoDesc g_tFileIconPixelDatas;



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
	int iTopLeftX, iTopLeftY , iTopLeftXBak;
	int iBotRightX, iBotRightY;
	int iIconWidth, iIconHight;
	int iNumOfOneRowIcon , iNumOfOneLineIcon;
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
	iIconHight = DIR_FILE_NAME_WIDTH;
	iIconWidth = iIconHight;

	/*ͼ����ͼ��֮��ľ�����Ҫ����10���� */
	//�ȵõ�ÿ�������ʾ����
	iNumOfOneRowIcon = ( ( iBotRightX -iTopLeftX ) + 1 ) / iIconWidth; 
	while ( 1 )
	{
		iDeltaX = ( iBotRightX -iTopLeftX ) + 1 - iIconWidth * iNumOfOneRowIcon; //�õ���ͼ��֮�������x�᳤��
		if ( (iDeltaX /(iNumOfOneRowIcon+1)) < 10 )
		{
			iNumOfOneRowIcon -- ; //����ͼ�����
		}
		else
		{
			break;
		}
	}

	//�ȵõ�ÿ�������ʾ����
	iNumOfOneLineIcon = ( ( iBotRightY -iTopLeftY ) + 1 ) / iIconWidth; 
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

	g_iDirFileNumPerCol =  iNumOfOneLineIcon;
	g_iDirFileNumPerRow =  iNumOfOneRowIcon;


	/* ������ʾ iNumPerRow * iNumPerCol��"Ŀ¼���ļ�"
	 * ����"����+1"��T_Layout�ṹ��: һ��������ʾͼ��,��һ��������ʾ����
	 * ���һ��������NULL,�����жϽṹ�������ĩβ
	 */
	 g_atDirAndFileLayout = malloc(sizeof(T_Layout) * (2*g_iDirFileNumPerCol*g_iDirFileNumPerRow + 1))
	 if(g_atDirAndFileLayout == NULL)
	 {
        DBG_PRINTF("malloc error..\r\n");
        return -1;
	 }

     // ���"Ŀ¼���ļ�"������������Ͻǡ����½����� ��ֵ
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
     //���ȵõ���һ���ļ�����ʾ���
     iTopLeftX +=iDeltaX;
     iTopLeftY +=iDeltaY;
	 iTopLeftXBak = iTopLeftX;

	 for(i = 0; i< iNumOfOneRowIcon ;i++)
	 {
		 for(j = 0; j< iNumOfOneLineIcon ;j++)
		 {
			 g_atDirAndFileLayout[k].iTopLeftX = iTopLeftX + (DIR_FILE_ALL_WIDTH - DIR_FILE_ICON_WIDTH)/2;
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
static int GenerateDirAndFileIcons(PT_PageLayOut ptPageLayout) 
{

   

}

/**********************************************************************
 * �������ƣ� GenerateDirAndFileIcons
 * ���������� Ϊ"���ҳ��"���ɲ˵������е�ͼ��--�ļ���ͼ�����ļ�ͼ��
 * ��������� ptPageLayout - �ں����ͼ����ļ�������ʾ����
 * ��������� ��
 * �� �� ֵ�� 0      - �ɹ�
 *            ����ֵ - ʧ��
 ***********************************************************************/
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

    CalcBrowsePageDirAndFilesLayout();

    if(!g_tDirClosedIconPixelDatas.aucPhotoData)
		GenerateDirAndFileIcons(&g_tBrowsePageDirAndFileLayout);


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




