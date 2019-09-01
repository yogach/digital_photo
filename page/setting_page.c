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

	/* 1. ����Դ� */
	pt_VideoTmp = GetVideoMem ( ID ( g_tSettingPageAction.name ),VMS_FOR_CUR ); //��ȡ�Դ����ڵ�ǰҳ����ʾ
	if ( pt_VideoTmp == NULL )
	{
		DBG_PRINTF ( "GetVideoMem error!\r\n" );
		return -1 ;
	}

	/* 2. ����ͼ������ */
	if ( atLayout->iTopLeftX == 0 )
	{
		CalcMainPageLayout ( atLayout );
	}

	/* 3. �軭���� */
	iError = GeneratePage ( atLayout,pt_VideoTmp );

	/* 3. ˢ���豸��ȥ	   */
	FlushVideoMemToDev ( pt_VideoTmp );

	/* 4. ���Դ��״̬����Ϊfree */
	PutVideoMem ( pt_VideoTmp );

	return 0;


}

int SettingPageRun ( void )
{
	/* 1. ��ʾҳ�� */
	ShowSettingPage ( g_atSettingPageIconsLayout );

	/* 2.������������*/



}

