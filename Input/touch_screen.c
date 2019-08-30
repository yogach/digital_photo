#include <config.h>
#include <input_manager.h>
#include <stdlib.h>

#include <tslib.h>
//#include <disp_manager.h>

static struct tsdev* g_tTSDev;
static int giXres,giYres;

static int TouchScreenDevInit ( void )
{
	char* pcTSName;
    int iBpp;
 
	if ( ( pcTSName = getenv ( "TSLIB_TSDEVICE" ) ) != NULL ) //�ӻ���������ȷ���������豸������
	{
		g_tTSDev = ts_open ( pcTSName, 0 ); /* ��������ʽ�� */
	}
	else
	{
		g_tTSDev = ts_open ( "/dev/event0", 1 ); //����ӻ����������޷���ȡ���������豸 ��������ʽ��/dev/event0
	}


	if ( !g_tTSDev )
	{
		DBG_PRINTF ( "ts_open error!\n" );
		return -1;
	}

	if ( ts_config ( g_tTSDev ) ) //����
	{
		DBG_PRINTF ( "ts_config error!\n" );
		return -1;
	}

	//if ( GetDispResolution ( &giXres, &giYres ,&iBpp ) ) //��ȡ��ʾ�豸��X Y �ֱ��� ����
	//{
	//	return -1;
	//}

	return 0;

}

static int TouchScreenDevExit ( void )
{

	return 0;
}

static int TouchScreenGetInputEvent ( PT_InputEvent ptInputEvent )
{
	int iRet ;
	struct ts_sample tSamp;//����ֵ
	
	/*
	*
	static struct timeval tPreTime;
	*
	*/

	while ( 1 )
	{
		iRet = ts_read ( g_tTSDev, &tSamp, 1 ); /* ��������������� */
		//��ô��������ݺ�ֱ�ӷ���ԭʼ����
		if ( iRet == 1 )
		{
          ptInputEvent->iType = INPUT_TYPE_TOUCHSCREEN;
		  ptInputEvent->iX = tSamp.x;
		  ptInputEvent->iY = tSamp.y;
		  ptInputEvent->tTime = tSamp.tv;
		  ptInputEvent->iPressure = tSamp.pressure;
		}

	}
	return 0;
}


static T_InputOpr g_TouchScreen =
{
	.name = "touchscreen",
	.DeviceInit = TouchScreenDevInit,
	.DeviceExit = TouchScreenDevExit,
	.GetInputEvent = TouchScreenGetInputEvent,

};

//��g_TouchScreen���������豸������
int TouchScreenInit ( void )
{
	return RegisterInputOpr ( &g_TouchScreen );

}
