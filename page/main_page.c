#include <config.h>
#include <disp_manager.h>
#include "page_manager.h"
#include <pic_manager.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>


static int MainPageGetInputEvent ();
static int MainPagePrepare ();
static int MainPageRun ( void );
//static PT_DispOpr g_tDispOpr;


static T_PageAction g_tMainPageAction =
{
	.name = "main",
	.Run = MainPageRun,
	.GetInputEvent = MainPageGetInputEvent,
	//.Prepare = MainPagePrepare,
};


//本页需要显示的图标
static T_Layout g_MainPageLayout[]=
{
	{0,0,0,0,"browse_mode.bmp"},
	{0,0,0,0,"continue_mod.bmp"},
	{0,0,0,0,"setting.bmp"},
	{0,0,0,0,NULL},
};


//主页面输入事件
static int MainPageGetInputEvent (void)
{
   //获得触摸屏原始数据



   return 0;
}

//主页面图片数据准备线程
/*static int MainPagePrepare ()
{

 return 0;
}*/

//主页面显示
static int showMainPage ( PT_Layout atLayout )
{
	PT_VideoMem pt_VideoTmp;
	int iXres,iYres,iBpp;
	int iIconWidth,iIconHight,IconX,IconY;
    //int iError;

	T_PhotoDesc tPhotoOriData;
	T_PhotoDesc tPhotoNew;

	/* 1. 获得显存 */
	pt_VideoTmp = GetVideoMem ( ID ( g_tMainPageAction.name ),VMS_FOR_CUR ); //获取显存用于当前页面显示
	if ( pt_VideoTmp == NULL )
	{
		DBG_PRINTF ( "GetVideoMem error!\r\n" );
		return -1 ;
	}

	/* 2. 描画数据 */
	if ( pt_VideoTmp->ePicState != PIC_GENERATED )//如果图片未准备好
	{

		GetDispResolution ( &iXres,&iYres,&iBpp ); //获取分辨率
		//首先确定首个图标的坐标
		iIconHight =  iYres*2/10 ;   //图标高度
		iIconWidth =  iIconHight*2;  //图标宽度

		IconX =  ( iXres-iIconWidth ) /2; //图标居中
		IconY =  iYres /10 ;

		//根据以上信息设置
		tPhotoNew.iBpp = iBpp;
		tPhotoNew.iHigh = iIconHight;
		tPhotoNew.iWidth = iIconWidth;
		tPhotoNew.iLineBytes = tPhotoNew.iWidth * tPhotoNew.iBpp / 8;
		tPhotoNew.iTotalBytes = tPhotoNew.iHigh * tPhotoNew.iLineBytes;
		tPhotoNew.aucPhotoData = malloc ( tPhotoNew.iTotalBytes );
		if ( tPhotoNew.aucPhotoData == NULL )
		{
			DBG_PRINTF ( "malloc tPhotoNew error\r\n" );
			return -1;
		}

		while(atLayout->IconName)
		{
              //得到当前图标的起始结束地址
              atLayout->iTopLeftX    = IconX;
			  atLayout->iTopLeftY    = IconY;
			  atLayout->iLowerRightX = IconX + iIconWidth - 1; //右下角X坐标
			  atLayout->iLowerRightY = IconY + iIconHight - 1; //右下角Y坐标
			  
			  //获取图标的图片数据
			  GetPixelDatasForIcon(atLayout->IconName ,&tPhotoOriData);

              //将原始图片数据缩放到指定大小
              PicZoom(&tPhotoOriData, &tPhotoNew);
			  //将图片合并到显存中
			  PicMerge(atLayout->iTopLeftX, atLayout->iTopLeftY, &tPhotoNew, &pt_VideoTmp->tVideoMemDesc);

              //释放图片分配的内存 防止内存泄露 
              FreePixelDatasForIcon(&tPhotoOriData);

			  //Y坐标往下递增
              IconY +=   iYres*3/10 ;
   
		      atLayout++; //指针+1 增加的长度与指针的数据类型有关
		}
		free(tPhotoNew.aucPhotoData); 
	}

	/* 3. 刷到设备上去       */
	FlushVideoMemToDev(pt_VideoTmp);
    
	/* 4. 将显存的状态设置为free */
	PutVideoMem(pt_VideoTmp);

	return 0;

}


static int MainPageRun ( void )
{

	/* 1. 显示页面 */
	showMainPage (g_MainPageLayout);
	/* 2. 创建Prepare线程 */

	/* 3. 调用GetInputEvent获得输入事件，进而处理 */
	while ( 1 )
	{
		//获取输入事件
		switch ( MainPageGetInputEvent () )
		{
			case "浏览模式":

				break;

			case "联播模式":

				break;

			case "设置":

				break;
		}


	}

	return 0;

}





int MainPageInit ( void )
{
	return RegisterPageAction ( &g_tMainPageAction );
}


