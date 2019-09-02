#include <config.h>
#include <disp_manager.h>
#include <page_manager.h>
#include <pic_manager.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <input_manager.h>

static void IntervalPageRun ( void );
static int CalcIntervalPageLayout ( PT_Layout atLayout );


static T_Layout g_atIntervalPageIconsLayout[] =
{
	{0, 0, 0, 0, "inc.bmp"},
	{0, 0, 0, 0, "time.bmp"},
	{0, 0, 0, 0, "dec.bmp"},
	{0, 0, 0, 0, "ok.bmp"},
	{0, 0, 0, 0, "cancel.bmp"},
	{0, 0, 0, 0, NULL},
};


static T_PageDesc g_tIntervalPageDesc =
{
	.name   = "setting",
	.Run    = IntervalPageRun,
	.CalcPageLayout = CalcIntervalPageLayout,
	.atPageLayout = g_atIntervalPageIconsLayout,
};


static int CalcIntervalPageLayout ( PT_Layout atLayout )
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



	return 0;

}


static void IntervalPageRun ( void )
{
    T_InputEvent tInputEvent;
	int iIndex,iIndexPressured=-1,bPressure = 0;

	/* 1. 显示页面 */
	ShowPage ( &g_tIntervalPageDesc );

	/* 2. 通过输入事件获得按下的icon 进而处理 */
	while ( 1 )
	{
		//获得在哪个图标中按下
		iIndex = GenericGetInputEvent ( g_atIntervalPageIconsLayout,&tInputEvent );

		if ( tInputEvent.iPressure == 0 ) //如果是松开状态
		{
			if ( bPressure ) //如果曾经按下
			{
				//改变按键区域的颜色
				ReleaseButton ( &g_atIntervalPageIconsLayout[iIndex] );
				bPressure = 0;


				if ( iIndexPressured == iIndex ) //如果按键和松开的是同一个按键
				{

					switch ( iIndexPressured )
					{
						case 0://
						{
						
						}
						break;

						case 1://
						{
						
						}
						break;

						case 2://
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
				PressButton ( &g_atIntervalPageIconsLayout[iIndex] );
			}

		}


	}


}


int IntervalPageInit ( void )
{
	return RegisterPageAction ( &g_tIntervalPageDesc );

}

