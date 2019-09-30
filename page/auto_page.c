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
 * g_iStartNumberToRecord : �ӵڼ����ļ���ʼȡ�����ǵ�����
 * g_iCurFileNumber       : ���κ���ִ��ʱ�����ĵ�1���ļ��ı��
 * g_iFileCountHaveGet    : �Ѿ��õ��˶��ٸ��ļ�������
 * g_iFileCountTotal      : ÿһ���ܹ�Ҫȡ�����ٸ��ļ�������
 * g_iNextProcessFileIndex: ��g_apstrFileNames�����м���Ҫ��ʾ��LCD�ϵ��ļ�
 */
static int g_iStartNumberToRecord = 0;
static int g_iCurFileNumber = 0;
static int g_iFileCountHaveGet = 0;
static int g_iFileCountTotal = 0;
static int g_iNextProcessFileIndex = 0;

#define FILE_COUNT 10
static char g_apstrFileNames[FILE_COUNT][256];//�ַ������� ������Ƕ�ȡ�����ļ��ľ���·��



static T_PageDesc g_tAutoPageDesc =
{
	.name   = "auto",
	.Run    = AutoPageRun,
};

/**********************************************************************
 * �������ƣ� GetNextAutoPlayFile
 * ���������� �����һ��Ҫ���ŵ�ͼƬ������
 * ��������� ��
 * ��������� strFileName - ���������һ��Ҫ���ŵ�ͼƬ������(������·��)
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 ***********************************************************************/
static int GetNextAutoPlayFile ( char* strFileName )
{
	int iError;

	//���������һ��Ҫ��ʾ��ͼƬ
	if ( g_iNextProcessFileIndex < g_iFileCountHaveGet )
	{
		//����
		strncpy ( strFileName,g_apstrFileNames[g_iNextProcessFileIndex],256 );
		g_iNextProcessFileIndex++;
		return 0;
	}
	else //��������ͼƬ������ʾ�� ���¼���
	{
	    //��ȡ�ļ�ǰ �����ò�����ʼֵ
		g_iCurFileNumber    = 0;
		g_iFileCountHaveGet = 0;
		g_iFileCountTotal   = FILE_COUNT;//��ֵ�����ó���g_apstrFileNames���鳤����ͬ ��Ȼ��segment fault
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
 * �������ƣ� PrepareNextPicture
 * ���������� ׼����ʾ��һͼƬ: ȡ����ͼƬ������,����VideoMem��
 * ��������� bCur : 0 - ��ʾ������׼���õ�, �п����޷����videomem
 *                   1 - ��ʾ������videomem, ��Ϊ�������Ͼ�Ҫ��LCD����ʾ������
 * ��������� ��
 * �� �� ֵ�� NULL   - �޷����VideoMem,������һ��ͼƬû��Ԥ�ȱ���
 *            ��NULL - ������һ��ͼƬ���ݵ�VideoMem��ָ��
 ***********************************************************************/
static PT_VideoMem PrepareNextPicture ( int bCur )
{
	PT_VideoMem ptVideoMen;
	PT_PhotoDesc ptPhotoDesc;
	int iError;
	char strFileName[256];

	//����Դ�
	ptVideoMen =  GetVideoMem ( -1, bCur );
	if ( ptVideoMen == NULL )
	{
		DBG_PRINTF ( "GetVideoMem error..\r\n" );
		return NULL;
	}

	//���videomen�ڵ�����
	ClearVideoMem ( ptVideoMen,COLOR_BACKGROUND );

	//�����һ��Ҫ���ŵ�ͼƬ�ľ���·��
	iError = GetNextAutoPlayFile ( strFileName );

	//��ȡͼƬ����
	GetOriPixelDatasFormFile ( strFileName, ptPhotoDesc );


	return ptVideoMen;

}


static void* AutoPlayThreadFunction ( void* pVoid )
{
	int bExit,bFirst;
	PT_VideoMem ptVideoMem;

	while ( 1 )
	{
		/* 1. ���ж��Ƿ�Ҫ�˳� */
		//��û�����
		pthread_mutex_lock ( &g_tAutoPlayThreadMutex );
		bExit = g_bAutoPlayThreadShouldExit;
		/* �ͷŻ����� */
		pthread_mutex_unlock ( &g_tAutoPlayThreadMutex );

		if ( bExit )
		{
			return NULL;
		}

		/* 2. ׼��Ҫ��ʾ��ͼƬ */
		ptVideoMem =PrepareNextPicture ( 0 );


		/* 3. ʱ�䵽�����ʾ���� */
		if ( !bFirst ) //����һ��ҳ��֮���ҳ����Ҫ���ߺ���ʾ
		{
			sleep ( g_iIntervalSecond );
		}
		bFirst = 0;

		ptVideoMem =PrepareNextPicture ( 1 );

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

	/* �������ֵ: ��ʾ��һ��Ŀ¼�µ��ļ�, ��ʾͼƬ�ļ�� */
	//��ʱʹ�ù̶�Ŀ¼����ʾ

	/* 1. ����һ���߳���������ʾͼƬ */
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

int AutoPageInit ( void )
{
	return RegisterPageAction ( &g_tAutoPageDesc );
}


