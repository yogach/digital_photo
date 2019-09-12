
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

/* 用来描述某目录里的内容 */
static PT_DirContent* g_aptDirContents;  /* 数组:存有目录下"顶层子目录","文件"的名字 */
static int g_iDirContentsNumber;         /* g_aptDirContents数组有多少项 */

//当前路径
static char g_strCurDir[256] = DEFAULT_PATH;
static char g_strSelectDir[256] = DEFAULT_PATH;



/* 图标是一个正方体, "图标+名字"也是一个正方体
 *   --------
 *   |  图  |
 *   |  标  |
 * ------------
 * |   名字   |
 * ------------
 */

#define DIR_FILE_ICON_WIDTH    40                       //文件图标宽度
#define DIR_FILE_ICON_HEIGHT   DIR_FILE_ICON_WIDTH      //文件图标高度
#define DIR_FILE_NAME_HEIGHT   20                       //文件名高度
#define DIR_FILE_NAME_WIDTH   (DIR_FILE_ICON_HEIGHT + DIR_FILE_NAME_HEIGHT) //文件名宽度
#define DIR_FILE_ALL_WIDTH    DIR_FILE_NAME_WIDTH   //整个大正方形宽度
#define DIR_FILE_ALL_HEIGHT   DIR_FILE_ALL_WIDTH    //整个大正方形高



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
 * 函数名称： CalcBrowsePageDirAndFilesLayout
 * 功能描述： 计算"目录和文件"的显示区域
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 ***********************************************************************/
static int CalcBrowsePageDirAndFilesLayout ( void )
{
	int iXres, iYres, iBpp;
	int iTopLeftX, iTopLeftY;
	int iBotRightX, iBotRightY;
	int iIconWidth, iIconHight;
	int iNumOfOneRowIcon , iNumOfOneLineIcon;
	int iDeltaX,iDeltaY;

	GetDispResolution ( &iXres, &iYres, &iBpp ); //获取分辨率

	//获得目录和文件显示区域的大小
	if ( iXres < iYres )
	{
		/* --------------------------------------
		*	 up select pre_page next_page 图标
		* --------------------------------------
		*
		*			目录和文件
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
		 *             |     目录和文件
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

	/* 确定一行显示多少个"目录或文件", 显示多少行 */
	iIconHight = DIR_FILE_NAME_WIDTH;
	iIconWidth = iIconHight;

	//图标与图标之间的距离需要大于10像素
	iNumOfOneRowIcon = ( ( iBotRightX -iTopLeftX ) + 1 ) / iIconWidth; //先得到每行最多显示个数
	while ( 1 )
	{
		iDeltaX = ( iBotRightX -iTopLeftX ) + 1 - iIconWidth * iNumOfOneRowIcon; //得到除图标之外的所有x轴长度
		if ( (iDeltaX /(iNumOfOneRowIcon+1)) < 10 )
		{
			iNumOfOneRowIcon -- ;
		}
		else
		{
			break;
		}
	}

	iNumOfOneLineIcon = ( ( iBotRightY -iTopLeftY ) + 1 ) / iIconWidth; //先得到每列最多显示个数
	while ( 1 )
	{
		iDeltaY = ( iBotRightY -iTopLeftY ) + 1 - iIconWidth * iNumOfOneLineIcon; //得到除图标之外的所有Y轴长度
		if ( (iDeltaY /(iNumOfOneLineIcon+1)) < 10 )
		{
			iNumOfOneLineIcon -- ;
		}
		else
		{
			break;
		}
	}

	iDeltaX = iDeltaX / (iNumOfOneRowIcon + 1); //得到每个图标间隔
	iDeltaY = iDeltaY / (iNumOfOneRowIcon + 1); 




}

static int CalcBrowsePageLayout ( PT_Layout atLayout )
{

	//获得屏幕分辨率
	int iXres,iYres,iBpp,i = 0;
	int iIconWidth,iIconHight,IconX,IconY;
	GetDispResolution ( &iXres,&iYres,&iBpp ); //获取LCD分辨率

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

	//获得显示设备显存
	ptDevVideoMem = GetDevVideoMen();

	/* 0. 获得要显示的目录的内容 */
	iError = GetDirContents ( g_strCurDir, &g_aptDirContents, &g_iDirContentsNumber );
	if ( iError )
	{
		DBG_PRINTF ( "GetDirContents error ... \r\n" );
		return -1;
	}

	/* 1. 显示页面 */
	ShowPage ( &g_tBrowsePageDesc );


	while ( 1 )
	{





	}


}


int BrowsePageInit ( void )
{
	return RegisterPageAction ( &g_tBrowsePageDesc );
}




