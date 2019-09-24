
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

/* 用来描述某目录里的内容 */
static PT_DirContent* g_aptDirContents;  /* 数组:存有目录下"顶层子目录","文件"的名字 */
static int g_iDirContentsNumber;         /* g_aptDirContents数组有多少项 */
static int g_iStartIndex = 0;            /* 在屏幕上显示的第1个"目录和文件"是g_aptDirContents数组里的哪一项 */


//当前路径
static char g_strCurDir[256] = DEFAULT_PATH;
//static char g_strSelectDir[256] = DEFAULT_PATH;

static int g_iDirFileNumPerCol, g_iDirFileNumPerRow; //每行与每列显示的图标个数 COL - 列 ROW - 行
static T_Layout* g_atDirAndFileLayout = NULL;    //用于保存文件夹/文件显示页面每个图标坐标值
static T_PageLayOut g_tBrowsePageDirAndFileLayout; //用于保存整个文件显示页面的坐标 -- 也包含g_atDirAndFileLayout内容

//用于存储文件夹/文件图标
static T_PhotoDesc g_tDirClosedIconPixelDatas;
static T_PhotoDesc g_tDirOpenedIconPixelDatas;
static T_PhotoDesc g_tFileIconPixelDatas;
//图标名
static char* g_strDirClosedIconName  = "fold_closed.bmp";
static char* g_strDirOpenedIconName  = "fold_opened.bmp";
static char* g_strFileIconName = "file.bmp";



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
 * 函数名称： CalcBrowsePageDirAndFilesLayout
 * 功能描述： 计算"目录和文件"的显示区域
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 ***********************************************************************/
static int CalcBrowsePageDirAndFilesLayout ( void )
{
	int iXres, iYres, iBpp;
	int iTopLeftX, iTopLeftY, iTopLeftXBak;
	int iBotRightX, iBotRightY;
	int iIconWidth, iIconHight;
	int iNumOfOneColIcon, iNumOfOneRowIcon;//COL - 列 ROW - 行
	int iDeltaX,iDeltaY;
	int i,j,k = 0;

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
		 *             |     目录和文件
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

	/* 确定一行显示多少个"目录或文件", 显示多少行 */
	iIconWidth  = DIR_FILE_NAME_WIDTH;
	iIconHight  = iIconWidth;

	/*图标与图标之间的距离需要大于10像素 */
	//先得到每行最多显示个数
	iNumOfOneRowIcon = (  iBotRightX -iTopLeftX  + 1 ) / iIconWidth;
	while ( 1 )
	{
		iDeltaX =  iBotRightX -iTopLeftX  + 1 - iIconWidth * iNumOfOneRowIcon; //得到除图标之外的所有x轴长度
		if ( ( iDeltaX / ( iNumOfOneRowIcon+1 ) ) < 10 )
		{
			iNumOfOneRowIcon -- ; //调整图标个数
		}
		else
		{
			break;
		}
	}

	//先得到每列最多显示个数
	iNumOfOneColIcon = (  iBotRightY -iTopLeftY  + 1 ) / iIconHight;
	while ( 1 )
	{
		iDeltaY =  iBotRightY - iTopLeftY  + 1 - iIconHight * iNumOfOneColIcon; //得到除图标之外的所有Y轴长度
		if ( ( iDeltaY / ( iNumOfOneColIcon+1 ) ) < 10 )
		{
			iNumOfOneColIcon -- ;
		}
		else
		{
			break;
		}
	}

	iDeltaX = iDeltaX / ( iNumOfOneRowIcon + 1 ); //得到每个图标间隔
	iDeltaY = iDeltaY / ( iNumOfOneColIcon + 1 );

	g_iDirFileNumPerCol =  iNumOfOneColIcon;//一列显示多少图标
	g_iDirFileNumPerRow =  iNumOfOneRowIcon;//一行显示多少个图标


	/* 可以显示 iNumPerRow * iNumPerCol个"目录或文件"
	 * 分配"两倍+1"的T_Layout结构体: 一个用来表示图标,另一个用来表示名字
	 * 最后一个用来存NULL,借以判断结构体数组的末尾
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
	// 获得"目录和文件"整体区域的左上角、右下角坐标
	g_tBrowsePageDirAndFileLayout.iTopLeftX = iTopLeftX;
	g_tBrowsePageDirAndFileLayout.iTopLeftY = iTopLeftY;
	g_tBrowsePageDirAndFileLayout.iLowerRightX = iBotRightX;
	g_tBrowsePageDirAndFileLayout.iLowerRightY = iBotRightY;
	g_tBrowsePageDirAndFileLayout.iBpp = iBpp;
	g_tBrowsePageDirAndFileLayout.iMaxTotalBytes = DIR_FILE_ALL_WIDTH * DIR_FILE_ALL_HEIGHT * iBpp / 8;
	g_tBrowsePageDirAndFileLayout.atLayOut = g_atDirAndFileLayout;


	/* 确定图标和名字的位置
	*
	* 图标是一个正方体, "图标+名字"也是一个正方体
	*   --------
	*   |  图  |
	*   |  标  |
	* ------------
	* |   名字   |
	* ------------
	*/
	//首先得到第一个文件的起点
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
			iTopLeftX +=DIR_FILE_ALL_WIDTH + iDeltaX;//得到同一行第二个图标的显示位置
		}

		iTopLeftX = iTopLeftXBak;//换行后x坐标回到起始
		iTopLeftY += DIR_FILE_ALL_HEIGHT + iDeltaY;//Y坐标加上图标整体高度与间隔

	}


	/* 结尾 */
	g_atDirAndFileLayout[k].iTopLeftX	 = 0;
	g_atDirAndFileLayout[k].iLowerRightX  = 0;
	g_atDirAndFileLayout[k].iTopLeftY	 = 0;
	g_atDirAndFileLayout[k].iLowerRightY  = 0;
	g_atDirAndFileLayout[k].IconName = NULL;


	return 0;

}

/**********************************************************************
 * 函数名称： GenerateDirAndFileIcons
 * 功能描述： 为"浏览页面"生成菜单区域中的图标--文件夹图标与文件图标
 * 输入参数： ptPageLayout - 内含多个图标的文件名和显示区域
 * 输出参数： 无
 * 返 回 值： 0      - 成功
 *            其他值 - 失败
 ***********************************************************************/
static int GenerateDirAndFileIcons ( PT_PageLayOut ptPageLayout )
{
	int iXres, iYres, iBpp;
	int iError;
	T_PhotoDesc tOriPhotoDesc;
	PT_Layout atLayOut =  ptPageLayout->atLayOut;

	GetDispResolution ( &iXres, &iYres, &iBpp ); //获取分辨率

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

	/* 从BMP文件里提取图像数据 */
	/* 1. 提取"fold_closed图标" */
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


	/* 2. 提取"fold_opened图标" */
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


	/* 3. 提取"file图标" */
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
 * 函数名称： GenerateDirAndFileIcons
 * 功能描述： 为"浏览页面"生成菜单区域中的图标--控制按钮页面
 * 输入参数： ptPageLayout - 内含多个图标的文件名和显示区域
 * 输出参数： 无
 * 返 回 值： 0      - 成功
 *            其他值 - 失败
 ***********************************************************************/
static int CalcBrowsePageLayout ( PT_Layout atLayout )
{

	//获得屏幕分辨率
	int iXres,iYres,iBpp,i = 0;
	int iIconWidth,iIconHight;
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

	CalcBrowsePageDirAndFilesLayout();

	if ( !g_tDirClosedIconPixelDatas.aucPhotoData )
	{
		GenerateDirAndFileIcons ( &g_tBrowsePageDirAndFileLayout );
	}


	return 0;

}


/**********************************************************************
 * 函数名称： GenerateBrowsePageDirAndFile
 * 功能描述： 为"浏览页面"生成"目录或文件"区域中的图标和文字,就是显示目录内容
 * 输入参数： iStartIndex        - 在屏幕上显示的第1个"目录和文件"是aptDirContents数组里的哪一项
 *            iDirContentsNumber - aptDirContents数组有多少项
 *            aptDirContents     - 数组:存有目录下"顶层子目录","文件"的名字
 *            ptVideoMem         - 在这个VideoMem中构造页面
 * 输出参数： 无
 * 返 回 值： 0      - 成功
 *            其他值 - 失败
 ***********************************************************************/
static int GenerateBrowsePageDirAndFile ( int iStartIndex, int iDirContentsNumber, PT_DirContent* aptDirContents, PT_VideoMem ptVideoMem )
{

	PT_PageLayOut ptPageLayout= &g_tBrowsePageDirAndFileLayout;

	PT_Layout  atFileAndDirLayout = ptPageLayout->atLayOut;
	int i,j,k = 0;
	int iDirContentIndex = iStartIndex;
	int iError;

	//1、首先清除指定区域的显示数据
	SetColorForAppointArea ( ptPageLayout->iTopLeftX,ptPageLayout->iTopLeftY,ptPageLayout->iLowerRightX,ptPageLayout->iLowerRightY,ptVideoMem,COLOR_BACKGROUND );

	//2、设置文件夹/文件 名称字体大小
	SetFontSize ( atFileAndDirLayout[1].iLowerRightY - atFileAndDirLayout[1].iTopLeftY + 1 - 5 );

    DBG_PRINTF("iDirContentsNumber:%d \r\n",iDirContentsNumber); //打印搜索到的文件个数
	//3、往指定位置拷贝数据
	for ( i = 0; i< g_iDirFileNumPerCol ; i++ )
	{
		for ( j = 0; j< g_iDirFileNumPerRow ; j++ )
		{
			if ( iDirContentIndex < iDirContentsNumber )
			{

				if ( aptDirContents[iDirContentIndex]->eFileType == FILETYPE_DIR ) //如果目标目录是文件夹 则在指定位置显示文件夹图标
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
 * 函数名称： BrowsePageSpecialDis
 * 功能描述： 用于显示特殊图标
 * 输入参数： ptVideoMem         - 在这个VideoMem中构造页面
 * 输出参数： 无
 * 返 回 值： 0      - 成功
 *            其他值 - 失败
 ***********************************************************************/
static int BrowsePageSpecialDis ( PT_VideoMem ptVideoMem )
{
	return GenerateBrowsePageDirAndFile ( g_iStartIndex,g_iDirContentsNumber,g_aptDirContents,ptVideoMem );
}
/**********************************************************************
 * 函数名称： FlushDirAndFile
 * 功能描述： 刷新文件夹/文件区域的显示
 * 输入参数： ptVideoMem         - 在这个VideoMem中构造页面
 * 输出参数： 无
 * 返 回 值： 0      - 成功
 *            其他值 - 失败
 ***********************************************************************/
static int FlushDirAndFile ( PT_VideoMem ptVideoMem )
{
	int iError;

	//在获取新文件夹内容之前 先释放原先分配的内存
	FreeDirContents ( g_aptDirContents,g_iDirContentsNumber );
	iError = GetDirContents ( g_strCurDir, &g_aptDirContents, &g_iDirContentsNumber );
	//DBG_PRINTF ( "get dir or file num is %d\r\n",g_iDirContentsNumber );
	if ( iError )
	{
		DBG_PRINTF ( "GetDirContents error ... \r\n" );
		return -1;
	}

	g_iStartIndex = 0;
	//生成新的页面
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
	int iIndex = g_iStartIndex + iDirFileIndex /2;//得到文件夹所在位置


	//如果是目录 改变文件夹为打开状态
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
	else//如果是文件 文件图标与文字区域颜色取反
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

	//获得显示设备显存
	ptDevVideoMem = GetDevVideoMen();

	/* 0. 获得要显示的目录的内容 */
	iError = GetDirContents ( g_strCurDir, &g_aptDirContents, &g_iDirContentsNumber );
	if ( iError )
	{
		DBG_PRINTF ( "GetDirContents error ... \r\n" );
		//return -1;
	}

	/* 1. 显示页面 */
	ShowPage ( &g_tBrowsePageDesc );

	//读取输入事件后处理
	while ( 1 )
	{

		iIndex = GenericGetInputEvent ( g_atBrowsePageIconsLayout,&tInputEvent );

		if ( iIndex == -1 ) //如果按下的地方没有在浏览页面控制按键上
		{
			iIndex =  GenericGetInputPositionInPageLayout ( g_atDirAndFileLayout,&tInputEvent );
			//DBG_PRINTF("press Icon Number is %d\r\n",iIndex); 

			if ( iIndex != -1 ) //如果按下的地方在文件夹/文件区域 此时得到的索引是按下区域的索引
			{
				//判断这个触点上是否有图标 除2是因为每个显示图标包含一个图标和一个文件名
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

		if ( tInputEvent.iPressure == 0 ) //如果是松开状态
		{
			if ( bPressure ) //如果曾经按下
			{

				if ( iIndex < DIRFILE_ICON_INDEX_BASE ) //代表是控制区按键
				{
					//改变按键区域的颜色
					ReleaseButton ( &g_atBrowsePageIconsLayout[iIndex] );
					bPressure = 0;

					if ( iIndexPressured == iIndex ) //如果按键和松开的是同一个按键
					{

						switch ( iIndexPressured )
						{
							case 0://向上
								if ( strcmp ( g_strCurDir,"/" ) ==0 ) //如果已经是顶层目录 返回
								{
									FreeDirContents ( g_aptDirContents,g_iDirContentsNumber );
									return ;
								}

								/*从g_strCurDir末尾开始查找到字符"/"的位置 返回位置所代表的指针 如果未能找到返回NULL
								 *需要执行两次是因为g_strCurDir的内容//mnt/
								 *
								 */
								ptTmp = strrchr ( g_strCurDir,'/' );
								*ptTmp = '\0'; //将'/'替换成结束符
								ptTmp = strrchr ( g_strCurDir,'/' );
								*ptTmp = '\0'; //将'/'替换成结束符

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

							case 1://选择 --暂时无用

								break;

							case 2://上一页
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
							case 3://下一页
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
				else//目录与文件区域
				{
					/*
					 * 如果按下和松开时, 触点不处于同一个图标上, 则释放图标
					 */
					if ( iIndexPressured != iIndex )
					{
						ChangeDirOrFileArenStatus ( iIndexPressured - DIRFILE_ICON_INDEX_BASE,0, ptDevVideoMem );
						bPressure = 0;

					}
					else if ( bHaveClickSelectIcon ) /* 按下和松开都是同一个按钮, 并且"选择"按钮是按下状态 */
					{
						bPressure = 0;

					}
					else /* "选择"按钮不被按下时, 单击目录则进入, bUsedToSelectDir为0时单击文件则显示它 */
					{
						bPressure = 0;
						/* 如果是目录, 进入这个目录 */
						iPressIndex = g_iStartIndex + ( iIndexPressured - DIRFILE_ICON_INDEX_BASE ) /2;
						if ( g_aptDirContents[iPressIndex]->eFileType == FILETYPE_DIR )
						{
							//如果g_strCurDir值为"/"      strtmp的值会是 "//mnt/" 此时可成功获取到文件夹内容
							snprintf ( strtmp,256,"%s/%s/",g_strCurDir,g_aptDirContents[iPressIndex]->strName ); //生成绝对路径

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
						else//如果是文件则进入显示页面
						{
							//获取点下文件的绝对路径
							//snprintf ( strtmp,256,"%s/%s/",g_strCurDir,g_aptDirContents[iPressIndex]->strName ); //生成绝对路径

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
		else //如果是按下状态
		{
			if ( iIndex != -1 ) //如果按下的是有内容的按键
			{
				if ( !bPressure ) // 未曾按下按钮
				{
					bPressure = 1;
					iIndexPressured = iIndex;
					tPreInputEvent = tInputEvent;
					if ( iIndexPressured < DIRFILE_ICON_INDEX_BASE ) //代表按下的是控制区按键
					{
						//改变按键区域的颜色
						PressButton ( &g_atBrowsePageIconsLayout[iIndex] );
					}
					else
					{
						//选中指定文件
						ChangeDirOrFileArenStatus ( iIndex - DIRFILE_ICON_INDEX_BASE,1, ptDevVideoMem );
					}
				}

				//如果长按向上按键2秒 直接返回上一界面
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




