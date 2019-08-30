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
static T_Layout g_atMainPageIconsLayout[]=
{
	{0,0,0,0,"browse_mode.bmp"},
	{0,0,0,0,"continue_mod.bmp"},
	{0,0,0,0,"setting.bmp"},
	{0,0,0,0,NULL},
};


//主页面输入事件
static int MainPageGetInputEvent (PT_Layout ptLayout,PT_InputEvent ptInputEvent )
{
   //获得触摸屏原始数据
   return GenericGetInputEvent(ptLayout,ptInputEvent);
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
              //设置当前图标的起始结束x y 坐标
              atLayout->iTopLeftX    = IconX;
			  atLayout->iTopLeftY    = IconY;
			  atLayout->iLowerRightX = IconX + iIconWidth - 1; //右下角X坐标
			  atLayout->iLowerRightY = IconY + iIconHight - 1; //右下角Y坐标
			  
			  //获取图标的图片数据
			  GetPixelDatasFormIcon(atLayout->IconName ,&tPhotoOriData);

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
    T_InputEvent tInputEvent;
    int iIndex,iIndexPressured=-1,bPressure = 0;

	/* 1. 显示页面 */
	showMainPage (g_atMainPageIconsLayout);
	/* 2. 创建Prepare线程 */

	/* 3. 获得输入事件得到按下的icon，进而处理 */
	while ( 1 )
	{
		//获得在哪个图标中按下
        iIndex = MainPageGetInputEvent (g_atMainPageIconsLayout,&tInputEvent);
		if (iIndex >= 0)
			DBG_PRINTF("put\release status:%d , icon num:%d\r\n",tInputEvent.iPressure,iIndex);

		if(tInputEvent.iPressure == 0)//如果是松开状态
		{
           if(bPressure)//如果曾经按下 
           {
            //改变按键区域的颜色

			bPressure = 0;
             if(iIndexPressured == iIndex)//如果按键和松开的是同一个按键
             {

                switch ( iIndexPressured )
				{
					case 0://

						break;

					case 1://

						break;

					case 2://

						break;

					default:
		                break;
				}


			 }
		   }
		}
		else //如果是按下状态
		{
          if(!bPressure)// 未曾按下按钮 
          {
			  bPressure = 1;
              iIndexPressured = iIndex;
			  //改变按键区域的颜色
			  
		  }

		}

	}

	return 0;

}


int MainPageInit ( void )
{
	return RegisterPageAction ( &g_tMainPageAction );
}


