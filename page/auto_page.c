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
#include <sys/time.h>
#include <pthread.h>

static void AutoPageRun ( PT_PageParams ptPageParams );

static pthread_t g_tAutoPlayThreadID;
static int g_bAutoPlayThreadShouldExit ;

static pthread_mutex_t g_tAutoPlayThreadMutex  = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  g_tAutoPlayThreadConVar = PTHREAD_COND_INITIALIZER;



static T_PageDesc g_tAutoPageDesc =
{
	.name   = "auto",
	.Run    = AutoPageRun,
};

static void* AutoPlayThreadFunction ( void* pVoid )
{
	int bExit;
	PT_VideoMem ptVideoMem;

	while ( 1 )
	{
		/* 1. 先判断是否要退出 */
		//获得互斥量
		pthread_mutex_lock ( &g_tAutoPlayThreadMutex );
		bExit = g_bAutoPlayThreadShouldExit;
		/* 释放互斥量 */
		pthread_mutex_unlock ( &g_tAutoPlayThreadMutex );

		/* 2. 准备要显示的图片 */

		/* 3. 时间到后就显示出来 */
		/* 刷到设备上去 */
		FlushVideoMemToDev ( ptVideoMem );

		/* 解放显存 */
		PutVideoMem ( ptVideoMem );

	}

	return NULL;

}


static void AutoPageRun ( PT_PageParams ptPageParams )
{
	int iRet;
	T_InputEvent tInputEvent;
	g_bAutoPlayThreadShouldExit = 0;

	//创建显示线程
	pthread_create ( &g_tAutoPlayThreadID,NULL,AutoPlayThreadFunction,NULL );

	/* 2. 当前线程等待触摸屏输入, 先做简单点: 如果点击了触摸屏, 让线程退出 */
	while ( 1 )
	{
		iRet = GetDeviceInput ( &tInputEvent );
		if ( iRet == 0 ) //只要有触摸就退出连播页面
		{
			//获得互斥量
			pthread_mutex_lock ( &g_tAutoPlayThreadMutex );
			g_bAutoPlayThreadShouldExit = 1;
			/* 释放互斥量 */
			pthread_mutex_unlock ( &g_tAutoPlayThreadMutex );
			pthread_join ( g_tAutoPlayThreadID, NULL ); /* 等待子线程退出 */

			return;
		}

	}

}

int BrowsePageInit ( void )
{
	return RegisterPageAction ( &g_tAutoPageDesc );
}


