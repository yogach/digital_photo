#include <config.h>
#include <disp_manager.h>
#include <page_manager.h>
#include <pic_manager.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <input_manager.h>
#include <render.h>

static void SettingPageRun ( void );
static int CalcSettingPageLayout ( PT_Layout atLayout );


static T_Layout g_atSettingPageIconsLayout[] =
{
	{0, 0, 0, 0, "select_fold.bmp"},
	{0, 0, 0, 0, "interval.bmp"},
	{0, 0, 0, 0, "return.bmp"},
	{0, 0, 0, 0, NULL},
};


static T_PageDesc g_tSettingPageDesc =
{
	.name   = "setting",
	.Run    = SettingPageRun,
	.CalcPageLayout = CalcSettingPageLayout,
	.atPageLayout = g_atSettingPageIconsLayout,
};


static int CalcSettingPageLayout ( PT_Layout atLayout )
{
	int iXres,iYres,iBpp;
	int iIconWidth,iIconHight,IconX,IconY;
	GetDispResolution ( &iXres,&iYres,&iBpp ); //获取LCD分辨率

	/*	 每个图标高度为2/10 Y分辨率
	 *	 宽度为高度的两倍
	 *	  ----------------------
	 *							 1/10 * iYres
	 *		   select_fold.bmp	 2/10 * iYres
	 *							 1/10 * iYres
	 *		   interval.bmp      2/10 * iYres
	 *							 1/10 * iYres
	 *		   return.bmp		 2/10 * iYres
	 *							 1/10 * iYres
	 *	  ----------------------
	 *
	 */

	iIconHight =  iYres*2/10 ;	 //图标高度
	iIconWidth =  iIconHight*2;  //图标宽度

	IconX =  ( iXres-iIconWidth ) /2; //图标居中
	IconY =  iYres /10 ;

	while ( atLayout->IconName )
	{
		//设置本页所有图标的起始结束x y 坐标
		atLayout->iTopLeftX    = IconX; 				 //左上角X坐标
		atLayout->iTopLeftY    = IconY; 				 //左上角X坐标
		atLayout->iLowerRightX = IconX + iIconWidth - 1; //右下角X坐标
		atLayout->iLowerRightY = IconY + iIconHight - 1; //右下角Y坐标

		//Y坐标往下递增
		IconY +=   iYres*3/10 ;
		atLayout++; //指针+1 指向数组下一项
	}

	return 0;

}


static void SettingPageRun ( void )
{
    T_InputEvent tInputEvent;
	int iIndex,iIndexPressured=-1,bPressure = 0;

	/* 1. 显示页面 */
	ShowPage ( &g_tSettingPageDesc );

	/* 2. 通过输入事件获得按下的icon 进而处理 */
	while ( 1 )
	{
		//获得在哪个图标中按下
		iIndex = GenericGetInputEvent ( g_atSettingPageIconsLayout,&tInputEvent );

		if ( tInputEvent.iPressure == 0 ) //如果是松开状态
		{
			if ( bPressure ) //如果曾经按下
			{
				//改变按键区域的颜色
				ReleaseButton ( &g_atSettingPageIconsLayout[iIndex] );
				bPressure = 0;


				if ( iIndexPressured == iIndex ) //如果按键和松开的是同一个按键
				{

					switch ( iIndexPressured )
					{
						case 0://设置连播文件夹页面
						{
						
						}
						break;

						case 1://设置连播间隔页面
						{
						   Page("interval")->Run();
						   ShowPage ( &g_tSettingPageDesc );
						}
						break;

						case 2://返回
						{
                            return;
						}
						break;

						default:
							break;
					}

				}

				iIndexPressured = -1;
			}
		}
		else //如果是按下状态
		{
			if ( !bPressure ) // 未曾按下按钮
			{
				bPressure = 1;
				iIndexPressured = iIndex;
				//改变按键区域的颜色
				PressButton ( &g_atSettingPageIconsLayout[iIndex] );
			}

		}


	}


}


int SettingPageInit ( void )
{
	return RegisterPageAction ( &g_tSettingPageDesc );

}

