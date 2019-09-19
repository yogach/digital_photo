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

//两个图片数据
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
 * 函数名称： CalcManualPagePictureLayout
 * 功能描述： 计算"manual页面"中"图片的显示区域"
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0-成功 其他值-失败
 ***********************************************************************/
static int CalcManualPagePictureLayout ( void )
{
	//获得屏幕分辨率
	int iXres,iYres,iBpp;
	int iTopLeftX,iTopLeftY,iLowerRightX,iLowerRightY;
	int i = 0;
	GetDispResolution ( &iXres,&iYres,&iBpp ); //获取LCD分辨率

	if ( iXres < iYres )
	{
		/*	  iXres/6
		 *	  --------------------------------------------------------------
		 *	   return	zoomout zoomin	pre_pic next_pic continue_mod_small  (图标)
		 *	  --------------------------------------------------------------
		 *
		 *								图片
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
		 *    pre_pic            |                 图片
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
 * 函数名称： CalcManualPageMenusLayout
 * 功能描述： 计算页面中各图标座标值
 * 输入参数： 无
 * 输出参数： ptPageLayout - 内含各图标的左上角/右下角座标值
 * 返 回 值： 0-成功 其他值-失败
 ***********************************************************************/
static int CalcManualPageLayout ( PT_Layout atLayout )
{

	//获得屏幕分辨率
	int iXres,iYres,iBpp;
	int iIconWidth,iIconHight;
	int i = 0;
	GetDispResolution ( &iXres,&iYres,&iBpp ); //获取LCD分辨率


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

	//在获取另一个数据之前先释放之前分配的空间
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
    GetDispResolution ( &iXres,&iYres,&iBpp ); //获取LCD分辨率

    //重新分配之前先释放原先分配的空间 防止内存泄漏
    if(g_tZoomedPicPixelDatas.aucPhotoData !=NULL)
    {
        free(g_tZoomedPicPixelDatas.aucPhotoData);
		g_tZoomedPicPixelDatas.aucPhotoData = NULL;
	}

	//设置缩放后图片的大小 使其能在指定区域显示
	k = (float)ptOriginPicPixelDatas->iHigh / ptOriginPicPixelDatas->iWidth; // 获取原先图片的长宽比

	g_tZoomedPicPixelDatas.iWidth  = iZoomedWidth ;
    g_tZoomedPicPixelDatas.iHigh   = iZoomedWidth * k ;

    //如果图片的高度大于区域高度
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

	//使用函数进行缩放
    PicZoom(ptOriginPicPixelDatas, &g_tZoomedPicPixelDatas);
	return &g_tZoomedPicPixelDatas;

}

static int ShowPictureInManualPage ( PT_VideoMem ptVideoMem, char* strFileName )
{
	int iXres,iYres,iBpp;
	PT_PhotoDesc ptOriPicPixelDatas, ptZoomedPicPixelDatas;
	int iPicWidth,iPicHight;
	int iTopLeftX, iTopLeftY;

	GetDispResolution ( &iXres,&iYres,&iBpp ); //获取LCD分辨率

	//获取图片数据
	ptOriPicPixelDatas = GetOriginPictureFilePixelDatas ( strFileName );
	if ( ptOriPicPixelDatas == NULL )
	{
		DBG_PRINTF ( "GetOriginPictureFilePixelDatas error..\r\n" );
		return -1;
	}


	// 首先计算页面图片显示区域的总大小
	iPicWidth = g_tManualPagePictureLayout.iLowerRightX - g_tManualPagePictureLayout.iTopLeftX + 1;
	iPicHight = g_tManualPagePictureLayout.iLowerRightY - g_tManualPagePictureLayout.iTopLeftY + 1;

	//将得到的图片原始数据进行缩放 缩放后的数据放入g_tZoomedPicPixelDatas
	ptZoomedPicPixelDatas = GetZoomedPicPixelDatas ( ptOriPicPixelDatas,iPicWidth,iPicHight );

	/* 算出居中显示时左上角坐标 */

	//算出图标中间点

	//清除指定区域的颜色

	//将需要显示的图片合并到显存中

}

static int ManualPageSpecialDis ( PT_VideoMem ptVideoMem )
{
	return ShowPictureInManualPage ( ptVideoMem, );//可先写入一个固定文件名
}

static void ManualPageRun ( void )
{
	PT_VideoMem ptDevVideoMem;

	//获得显示设备显存
	ptDevVideoMem = GetDevVideoMen();

	/* 1. 显示页面 */
	ShowPage ( &g_tManualPageDesc );

	while ( 1 )
	{




	}


}


int ManualPageInit ( void )
{
	return RegisterPageAction ( &g_tManualPageDesc );
}



