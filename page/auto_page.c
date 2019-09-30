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

static char g_acSelectDir[256] = "//mnt/";
static int g_iIntervalSecond = 10;

/*
 * g_iStartNumberToRecord : 从第几个文件开始取出它们的名字
 * g_iCurFileNumber       : 本次函数执行时读到的第1个文件的编号
 * g_iFileCountHaveGet    : 已经得到了多少个文件的名字
 * g_iFileCountTotal      : 每一次总共要取出多少个文件的名字
 * g_iNextProcessFileIndex: 在g_apstrFileNames数组中即将要显示在LCD上的文件
 */
static int g_iStartNumberToRecord = 0;
static int g_iCurFileNumber = 0;
static int g_iFileCountHaveGet = 0;
static int g_iFileCountTotal = 0;
static int g_iNextProcessFileIndex = 0;

#define FILE_COUNT 10
static char g_apstrFileNames[FILE_COUNT][256];//字符串数组 保存的是读取到的文件的绝对路径



static T_PageDesc g_tAutoPageDesc =
{
	.name   = "auto",
	.Run    = AutoPageRun,
};

/**********************************************************************
 * 函数名称： GetNextAutoPlayFile
 * 功能描述： 获得下一个要播放的图片的名字
 * 输入参数： 无
 * 输出参数： strFileName - 里面存有下一个要播放的图片的名字(含绝对路径)
 * 返 回 值： 0 - 成功, 其他值 - 失败
 ***********************************************************************/
static int GetNextAutoPlayFile ( char* strFileName )
{
	int iError;

	//如果还有下一个要显示的图片
	if ( g_iNextProcessFileIndex < g_iFileCountHaveGet )
	{
		//拷贝
		strncpy ( strFileName,g_apstrFileNames[g_iNextProcessFileIndex],256 );
		g_iNextProcessFileIndex++;
		return 0;
	}
	else //如果载入的图片都已显示完 重新加载
	{
	    //读取文件前 先设置参数初始值
		g_iCurFileNumber    = 0;
		g_iFileCountHaveGet = 0;
		g_iFileCountTotal   = FILE_COUNT;//此值需设置成与g_apstrFileNames数组长度相同 不然会segment fault
		g_iNextProcessFileIndex = 0;

		iError = GetFilesIndir ( strFileName, &g_iStartNumberToRecord, &g_iCurFileNumber,\
		                         &g_iFileCountHaveGet, g_iFileCountTotal, g_apstrFileNames ); 

		if( )
		{


		}



	}

	return -1;
}
/**********************************************************************
 * 函数名称： PrepareNextPicture
 * 功能描述： 准备显示下一图片: 取出下图片的数据,存入VideoMem中
 * 输入参数： bCur : 0 - 表示这是做准备用的, 有可能无法获得videomem
 *                   1 - 表示必须获得videomem, 因为这是马上就要在LCD上显示出来的
 * 输出参数： 无
 * 返 回 值： NULL   - 无法获得VideoMem,所以下一个图片没有预先备好
 *            非NULL - 存有下一个图片数据的VideoMem的指针
 ***********************************************************************/
static PT_VideoMem PrepareNextPicture ( int bCur )
{
	PT_VideoMem ptVideoMen;
	PT_PhotoDesc ptPhotoDesc;
	int iError;
	char strFileName[256];

	//获得显存
	ptVideoMen =  GetVideoMem ( -1, bCur );
	if ( ptVideoMen == NULL )
	{
		DBG_PRINTF ( "GetVideoMem error..\r\n" );
		return NULL;
	}

	//清除videomen内的内容
	ClearVideoMem ( ptVideoMen,COLOR_BACKGROUND );

	//获得下一个要播放的图片的绝对路径
	iError = GetNextAutoPlayFile ( strFileName );

	//提取图片数据
	GetOriPixelDatasFormFile ( strFileName, ptPhotoDesc );


	return ptVideoMen;

}


static void* AutoPlayThreadFunction ( void* pVoid )
{
	int bExit,bFirst;
	PT_VideoMem ptVideoMem;

	while ( 1 )
	{
		/* 1. 先判断是否要退出 */
		//获得互斥量
		pthread_mutex_lock ( &g_tAutoPlayThreadMutex );
		bExit = g_bAutoPlayThreadShouldExit;
		/* 释放互斥量 */
		pthread_mutex_unlock ( &g_tAutoPlayThreadMutex );

		if ( bExit )
		{
			return NULL;
		}

		/* 2. 准备要显示的图片 */
		ptVideoMem =PrepareNextPicture ( 0 );


		/* 3. 时间到后就显示出来 */
		if ( !bFirst ) //除第一个页面之外的页面需要休眠后显示
		{
			sleep ( g_iIntervalSecond );
		}
		bFirst = 0;

		ptVideoMem =PrepareNextPicture ( 1 );

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

	/* 获得配置值: 显示哪一个目录下的文件, 显示图片的间隔 */
	//暂时使用固定目录来显示

	/* 1. 启动一个线程来连续显示图片 */
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

int AutoPageInit ( void )
{
	return RegisterPageAction ( &g_tAutoPageDesc );
}


