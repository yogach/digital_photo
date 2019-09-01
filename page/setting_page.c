#include <config.h>
#include <disp_manager.h>
#include <page_manager.h>
#include <pic_manager.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <input_manager.h>



static T_PageDesc g_tSettingPageAction =
{
	.name = "setting",
	.Run = SettingPageRun,
	//.GetInputEvent = SettingPageGetInputEvent,
	//.Prepare = MainPagePrepare,
};

static T_Layout g_atSettingPageIconsLayout =
{

};


static int ShowSettingPage ( PT_Layout atLayout )
{
	PT_VideoMem pt_VideoTmp;
	int iError;

	/* 1. 获得显存 */
	pt_VideoTmp = GetVideoMem ( ID ( g_tSettingPageAction.name ),VMS_FOR_CUR ); //获取显存用于当前页面显示
	if ( pt_VideoTmp == NULL )
	{
		DBG_PRINTF ( "GetVideoMem error!\r\n" );
		return -1 ;
	}

	/* 2. 生成图标坐标 */
	if ( atLayout->iTopLeftX == 0 )
	{
		CalcMainPageLayout ( atLayout );
	}

	/* 3. 描画数据 */
	iError = GeneratePage ( atLayout,pt_VideoTmp );

	/* 3. 刷到设备上去	   */
	FlushVideoMemToDev ( pt_VideoTmp );

	/* 4. 将显存的状态设置为free */
	PutVideoMem ( pt_VideoTmp );

	return 0;


}

int SettingPageRun ( void )
{
	/* 1. 显示页面 */
	ShowSettingPage ( g_atSettingPageIconsLayout );

	/* 2.处理输入数据*/



}

