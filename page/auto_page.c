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
#include <unistd.h>

static void AutoPageRun ( PT_PageParams ptPageParams );

static pthread_t g_tAutoPlayThreadID;
static int g_bAutoPlayThreadShouldExit ;

static pthread_mutex_t g_tAutoPlayThreadMutex  = PTHREAD_MUTEX_INITIALIZER; /* 互斥量 */
//static pthread_cond_t  g_tAutoPlayThreadConVar = PTHREAD_COND_INITIALIZER; //休眠唤醒

static char g_acSelectDir[256];// = "//mnt/Icon/";
static int g_iIntervalSecond = 10;

/* 以深度优先的方式获得目录下的文件
 * 即: 先获得顶层目录下的文件, 再进入一级子目录A
 *     先获得一级子目录A下的文件, 再进入二级子目录AA, ...
 *     处理完一级子目录A后, 再进入一级子目录B
 *
 * "连播模式"下调用该函数获得要显示的文件
 * 有两种方法获得这些文件:
 * 1. 事先只需要调用一次函数,把所有文件的名字保存到某个缓冲区中
 * 2. 要使用文件时再调用函数,只保存当前要使用的文件的名字
 * 第1种方法比较简单,但是当文件很多时有可能导致内存不足.
 * 我们使用第2种方法:
 * 假设某目录(包括所有子目录)下所有的文件都给它编一个号
 * g_iStartNumberToRecord : 从第几个文件开始取出它们的名字
 * g_iCurFileNumber       : 本次函数执行时读到的第1个文件的编号
 * g_iFileCountHaveGet    : 已经得到了多少个文件的名字
 * g_iFileCountTotal      : 每一次总共要取出多少个文件的名字
 * g_iNextProcessFileIndex: 在g_apstrFileNames数组中即将要显示在LCD上的文件
 *
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
 * 函数名称： ResetAutoPlayFile
 * 功能描述： 每次使用"连播"功能时,都调用此函数,它使得从第1个文件开始"连播"
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 ***********************************************************************/
static void ResetAutoPlayFile ( void )
{
	g_iStartNumberToRecord = 0;
	g_iCurFileNumber = 0;
	g_iFileCountHaveGet = 0;
	g_iFileCountTotal = 0;
	g_iNextProcessFileIndex = 0;
}

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
	else //如果载入的图片都已显示完 重新到文件夹中进行加载
	{
		//读取文件前 先设置参数初始值
		g_iCurFileNumber    = 0;
		g_iFileCountHaveGet = 0;
		g_iFileCountTotal   = FILE_COUNT;//此值需设置成与g_apstrFileNames数组长度相同 不然会数组长度不够 会造成segment fault
		g_iNextProcessFileIndex = 0;

        DBG_PRINTF("g_iStartNumberToRecord : %d\r\n",g_iStartNumberToRecord);
		iError = GetFilesIndir ( g_acSelectDir, &g_iStartNumberToRecord, &g_iCurFileNumber,&g_iFileCountHaveGet, g_iFileCountTotal, g_apstrFileNames );

		//如果出错从头开始重新加载
		if (  iError  || ( g_iNextProcessFileIndex >= g_iFileCountHaveGet ) )
		{
            DBG_PRINTF("if GetFilesIndir error or g_iNextProcessFileIndex >= g_iFileCountHaveGet reGetFilesIndir.\r\n");
			g_iStartNumberToRecord = 0;
			g_iCurFileNumber    = 0;
			g_iFileCountHaveGet = 0;
			g_iFileCountTotal   = FILE_COUNT;//此值需设置成与g_apstrFileNames数组长度相同 不然会数组长度不够 会造成segment fault
			g_iNextProcessFileIndex = 0;

			iError = GetFilesIndir ( g_acSelectDir, &g_iStartNumberToRecord, &g_iCurFileNumber,&g_iFileCountHaveGet, g_iFileCountTotal, g_apstrFileNames );
		}

		if ( iError == 0 )
		{
			if ( g_iNextProcessFileIndex < g_iFileCountHaveGet )
			{
				//拷贝
				strncpy ( strFileName,g_apstrFileNames[g_iNextProcessFileIndex],256 );
				g_iNextProcessFileIndex++;
				return 0;
			}
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
	float k;
	PT_VideoMem ptVideoMem;
	T_PhotoDesc tOriPhotoDesc ;
	T_PhotoDesc tZoomPhotoDesc;
	int iError;
	char strFileName[256];
	int iXres, iYres, iBpp;
	int iTopLeftX,iTopLeftY;

	GetDispResolution ( &iXres, &iYres, &iBpp ); //获取分辨率

	//获得显存
	ptVideoMem =  GetVideoMem ( -1, bCur );
	if ( ptVideoMem == NULL )
	{
		DBG_PRINTF ( "GetVideoMem error..\r\n" );
		return NULL;
	}

	//清除videomen内的内容
	ClearVideoMem ( ptVideoMem,COLOR_BACKGROUND );

	while ( 1 )
	{
		//获得下一个要播放的图片的绝对路径
		iError = GetNextAutoPlayFile ( strFileName );
		if ( iError!=0 )
		{
			DBG_PRINTF ( "GetNextAutoPlayFile error..\r\n" );
			PutVideoMem ( ptVideoMem );
			return NULL;
		}

        DBG_PRINTF("strFileName : %s\r\n",strFileName);

		//提取图片数据
		iError = GetOriPixelDatasFormFile ( strFileName, &tOriPhotoDesc );
		if ( iError==0 )
		{
			break; //如果获取成功就退出循环
		}
	}

	/* 把图片按比例缩放到VideoMem上, 居中显示
	 * 1. 先算出缩放后的大小
	 */
	k = ( float ) tOriPhotoDesc.iHigh / tOriPhotoDesc.iWidth; // 获取原先图片的长宽比

	tZoomPhotoDesc.iWidth  = iXres ;
	tZoomPhotoDesc.iHigh   = iXres * k ;

	//如果图片的高度大于区域高度 限制区域图片高度为区域高度 宽度按比例缩放
	if ( tZoomPhotoDesc.iHigh > iYres )
	{
		tZoomPhotoDesc.iHigh = iYres;
		tZoomPhotoDesc.iWidth = tZoomPhotoDesc.iHigh / k ;
	}

	tZoomPhotoDesc.iBpp    = iBpp;
	tZoomPhotoDesc.iLineBytes = tZoomPhotoDesc.iWidth * tZoomPhotoDesc.iBpp / 8;
	tZoomPhotoDesc.iTotalBytes = tZoomPhotoDesc.iLineBytes * tZoomPhotoDesc.iHigh;
	tZoomPhotoDesc.aucPhotoData = malloc ( tZoomPhotoDesc.iTotalBytes );
	if ( tZoomPhotoDesc.aucPhotoData == NULL )
	{
		DBG_PRINTF ( "malloc error...\r\n" );
		PutVideoMem ( ptVideoMem );
		return NULL;
	}

	/* 2. 再进行缩放 */
	PicZoom ( &tOriPhotoDesc, &tZoomPhotoDesc );

	/* 3. 接着算出居中显示时起始坐标 */
	iTopLeftX = ( iXres - tZoomPhotoDesc.iWidth ) /2 ;
	iTopLeftY = ( iYres - tZoomPhotoDesc.iHigh ) /2;

	/* 4. 最后把得到的图片合并入VideoMem */
	iError = PicMerge ( iTopLeftX, iTopLeftY,  &tZoomPhotoDesc, &ptVideoMem->tVideoMemDesc );

	/* 5. 释放图片原始数据 */
	FreePixelDatasForIcon ( &tOriPhotoDesc );

	/* 6. 释放缩放后的数据 */
	free ( tZoomPhotoDesc.aucPhotoData );

	return ptVideoMem;

}


static void* AutoPlayThreadFunction ( void* pVoid )
{
	int bExit = 0,bFirst = 1;
	PT_VideoMem ptVideoMem;

	ResetAutoPlayFile();
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
		ptVideoMem = PrepareNextPicture ( 0 );

		/* 3. 时间到后就显示出来 */
		//除第一个页面之外的页面需要休眠后显示
		if ( !bFirst ) 
		{
			sleep ( g_iIntervalSecond );
		}
		bFirst = 0;

		if ( ptVideoMem == NULL )
		{
			ptVideoMem = PrepareNextPicture ( 1 );
		}

		/* 刷到设备上去 */
		FlushVideoMemToDev ( ptVideoMem );

		/* 解放显存 */
		PutVideoMem ( ptVideoMem );

	}

	return NULL;

}

/*
*连播模式要显示的文件夹有两种方式 ：
* 1.通过设置页面设置要显示的文件夹
* 2.在browse页面中按连播按键进入连播
*/
static void AutoPageRun ( PT_PageParams ptPageParams )
{
	int iRet;
	T_InputEvent tInputEvent;
	g_bAutoPlayThreadShouldExit = 0;

	/* 获得配置值: 显示哪一个目录下的文件, 显示图片的间隔 */
	GetPageCfg(&g_iIntervalSecond,g_acSelectDir);
	g_acSelectDir[255] = '\0';
    
	//当传入的目录值不为空时,显示此文件夹下的内容
	if (ptPageParams->strCurPictureFile[0] != '\0')
    {
		strncpy(g_acSelectDir,ptPageParams->strCurPictureFile,256);
    	g_acSelectDir[255] = '\0';
	}
	DBG_PRINTF("IntervalSecond:%d SelectDir:%s\r\n",g_iIntervalSecond,g_acSelectDir);

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


