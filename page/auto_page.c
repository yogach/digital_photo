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

static pthread_mutex_t g_tAutoPlayThreadMutex  = PTHREAD_MUTEX_INITIALIZER; /* ������ */
//static pthread_cond_t  g_tAutoPlayThreadConVar = PTHREAD_COND_INITIALIZER; //���߻���

static char g_acSelectDir[256];// = "//mnt/Icon/";
static int g_iIntervalSecond = 10;

/* ��������ȵķ�ʽ���Ŀ¼�µ��ļ�
 * ��: �Ȼ�ö���Ŀ¼�µ��ļ�, �ٽ���һ����Ŀ¼A
 *     �Ȼ��һ����Ŀ¼A�µ��ļ�, �ٽ��������Ŀ¼AA, ...
 *     ������һ����Ŀ¼A��, �ٽ���һ����Ŀ¼B
 *
 * "����ģʽ"�µ��øú������Ҫ��ʾ���ļ�
 * �����ַ��������Щ�ļ�:
 * 1. ����ֻ��Ҫ����һ�κ���,�������ļ������ֱ��浽ĳ����������
 * 2. Ҫʹ���ļ�ʱ�ٵ��ú���,ֻ���浱ǰҪʹ�õ��ļ�������
 * ��1�ַ����Ƚϼ�,���ǵ��ļ��ܶ�ʱ�п��ܵ����ڴ治��.
 * ����ʹ�õ�2�ַ���:
 * ����ĳĿ¼(����������Ŀ¼)�����е��ļ���������һ����
 * g_iStartNumberToRecord : �ӵڼ����ļ���ʼȡ�����ǵ�����
 * g_iCurFileNumber       : ���κ���ִ��ʱ�����ĵ�1���ļ��ı��
 * g_iFileCountHaveGet    : �Ѿ��õ��˶��ٸ��ļ�������
 * g_iFileCountTotal      : ÿһ���ܹ�Ҫȡ�����ٸ��ļ�������
 * g_iNextProcessFileIndex: ��g_apstrFileNames�����м���Ҫ��ʾ��LCD�ϵ��ļ�
 *
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
 * �������ƣ� ResetAutoPlayFile
 * ���������� ÿ��ʹ��"����"����ʱ,�����ô˺���,��ʹ�ôӵ�1���ļ���ʼ"����"
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� ��
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
	else //��������ͼƬ������ʾ�� ���µ��ļ����н��м���
	{
		//��ȡ�ļ�ǰ �����ò�����ʼֵ
		g_iCurFileNumber    = 0;
		g_iFileCountHaveGet = 0;
		g_iFileCountTotal   = FILE_COUNT;//��ֵ�����ó���g_apstrFileNames���鳤����ͬ ��Ȼ�����鳤�Ȳ��� �����segment fault
		g_iNextProcessFileIndex = 0;

        DBG_PRINTF("g_iStartNumberToRecord : %d\r\n",g_iStartNumberToRecord);
		iError = GetFilesIndir ( g_acSelectDir, &g_iStartNumberToRecord, &g_iCurFileNumber,&g_iFileCountHaveGet, g_iFileCountTotal, g_apstrFileNames );

		//��������ͷ��ʼ���¼���
		if (  iError  || ( g_iNextProcessFileIndex >= g_iFileCountHaveGet ) )
		{
            DBG_PRINTF("if GetFilesIndir error or g_iNextProcessFileIndex >= g_iFileCountHaveGet reGetFilesIndir.\r\n");
			g_iStartNumberToRecord = 0;
			g_iCurFileNumber    = 0;
			g_iFileCountHaveGet = 0;
			g_iFileCountTotal   = FILE_COUNT;//��ֵ�����ó���g_apstrFileNames���鳤����ͬ ��Ȼ�����鳤�Ȳ��� �����segment fault
			g_iNextProcessFileIndex = 0;

			iError = GetFilesIndir ( g_acSelectDir, &g_iStartNumberToRecord, &g_iCurFileNumber,&g_iFileCountHaveGet, g_iFileCountTotal, g_apstrFileNames );
		}

		if ( iError == 0 )
		{
			if ( g_iNextProcessFileIndex < g_iFileCountHaveGet )
			{
				//����
				strncpy ( strFileName,g_apstrFileNames[g_iNextProcessFileIndex],256 );
				g_iNextProcessFileIndex++;
				return 0;
			}
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
	float k;
	PT_VideoMem ptVideoMem;
	T_PhotoDesc tOriPhotoDesc ;
	T_PhotoDesc tZoomPhotoDesc;
	int iError;
	char strFileName[256];
	int iXres, iYres, iBpp;
	int iTopLeftX,iTopLeftY;

	GetDispResolution ( &iXres, &iYres, &iBpp ); //��ȡ�ֱ���

	//����Դ�
	ptVideoMem =  GetVideoMem ( -1, bCur );
	if ( ptVideoMem == NULL )
	{
		DBG_PRINTF ( "GetVideoMem error..\r\n" );
		return NULL;
	}

	//���videomen�ڵ�����
	ClearVideoMem ( ptVideoMem,COLOR_BACKGROUND );

	while ( 1 )
	{
		//�����һ��Ҫ���ŵ�ͼƬ�ľ���·��
		iError = GetNextAutoPlayFile ( strFileName );
		if ( iError!=0 )
		{
			DBG_PRINTF ( "GetNextAutoPlayFile error..\r\n" );
			PutVideoMem ( ptVideoMem );
			return NULL;
		}

        DBG_PRINTF("strFileName : %s\r\n",strFileName);

		//��ȡͼƬ����
		iError = GetOriPixelDatasFormFile ( strFileName, &tOriPhotoDesc );
		if ( iError==0 )
		{
			break; //�����ȡ�ɹ����˳�ѭ��
		}
	}

	/* ��ͼƬ���������ŵ�VideoMem��, ������ʾ
	 * 1. ��������ź�Ĵ�С
	 */
	k = ( float ) tOriPhotoDesc.iHigh / tOriPhotoDesc.iWidth; // ��ȡԭ��ͼƬ�ĳ����

	tZoomPhotoDesc.iWidth  = iXres ;
	tZoomPhotoDesc.iHigh   = iXres * k ;

	//���ͼƬ�ĸ߶ȴ�������߶� ��������ͼƬ�߶�Ϊ����߶� ��Ȱ���������
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

	/* 2. �ٽ������� */
	PicZoom ( &tOriPhotoDesc, &tZoomPhotoDesc );

	/* 3. �������������ʾʱ��ʼ���� */
	iTopLeftX = ( iXres - tZoomPhotoDesc.iWidth ) /2 ;
	iTopLeftY = ( iYres - tZoomPhotoDesc.iHigh ) /2;

	/* 4. ���ѵõ���ͼƬ�ϲ���VideoMem */
	iError = PicMerge ( iTopLeftX, iTopLeftY,  &tZoomPhotoDesc, &ptVideoMem->tVideoMemDesc );

	/* 5. �ͷ�ͼƬԭʼ���� */
	FreePixelDatasForIcon ( &tOriPhotoDesc );

	/* 6. �ͷ����ź������ */
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
		ptVideoMem = PrepareNextPicture ( 0 );

		/* 3. ʱ�䵽�����ʾ���� */
		//����һ��ҳ��֮���ҳ����Ҫ���ߺ���ʾ
		if ( !bFirst ) 
		{
			sleep ( g_iIntervalSecond );
		}
		bFirst = 0;

		if ( ptVideoMem == NULL )
		{
			ptVideoMem = PrepareNextPicture ( 1 );
		}

		/* ˢ���豸��ȥ */
		FlushVideoMemToDev ( ptVideoMem );

		/* ����Դ� */
		PutVideoMem ( ptVideoMem );

	}

	return NULL;

}

/*
*����ģʽҪ��ʾ���ļ��������ַ�ʽ ��
* 1.ͨ������ҳ������Ҫ��ʾ���ļ���
* 2.��browseҳ���а�����������������
*/
static void AutoPageRun ( PT_PageParams ptPageParams )
{
	int iRet;
	T_InputEvent tInputEvent;
	g_bAutoPlayThreadShouldExit = 0;

	/* �������ֵ: ��ʾ��һ��Ŀ¼�µ��ļ�, ��ʾͼƬ�ļ�� */
	GetPageCfg(&g_iIntervalSecond,g_acSelectDir);
	g_acSelectDir[255] = '\0';
    
	//�������Ŀ¼ֵ��Ϊ��ʱ,��ʾ���ļ����µ�����
	if (ptPageParams->strCurPictureFile[0] != '\0')
    {
		strncpy(g_acSelectDir,ptPageParams->strCurPictureFile,256);
    	g_acSelectDir[255] = '\0';
	}
	DBG_PRINTF("IntervalSecond:%d SelectDir:%s\r\n",g_iIntervalSecond,g_acSelectDir);

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


