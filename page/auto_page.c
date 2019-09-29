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
		/* 1. ���ж��Ƿ�Ҫ�˳� */
		//��û�����
		pthread_mutex_lock ( &g_tAutoPlayThreadMutex );
		bExit = g_bAutoPlayThreadShouldExit;
		/* �ͷŻ����� */
		pthread_mutex_unlock ( &g_tAutoPlayThreadMutex );

		/* 2. ׼��Ҫ��ʾ��ͼƬ */

		/* 3. ʱ�䵽�����ʾ���� */
		/* ˢ���豸��ȥ */
		FlushVideoMemToDev ( ptVideoMem );

		/* ����Դ� */
		PutVideoMem ( ptVideoMem );

	}

	return NULL;

}


static void AutoPageRun ( PT_PageParams ptPageParams )
{
	int iRet;
	T_InputEvent tInputEvent;
	g_bAutoPlayThreadShouldExit = 0;

	//������ʾ�߳�
	pthread_create ( &g_tAutoPlayThreadID,NULL,AutoPlayThreadFunction,NULL );

	/* 2. ��ǰ�̵߳ȴ�����������, �����򵥵�: �������˴�����, ���߳��˳� */
	while ( 1 )
	{
		iRet = GetDeviceInput ( &tInputEvent );
		if ( iRet == 0 ) //ֻҪ�д������˳�����ҳ��
		{
			//��û�����
			pthread_mutex_lock ( &g_tAutoPlayThreadMutex );
			g_bAutoPlayThreadShouldExit = 1;
			/* �ͷŻ����� */
			pthread_mutex_unlock ( &g_tAutoPlayThreadMutex );
			pthread_join ( g_tAutoPlayThreadID, NULL ); /* �ȴ����߳��˳� */

			return;
		}

	}

}

int BrowsePageInit ( void )
{
	return RegisterPageAction ( &g_tAutoPageDesc );
}


