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
 
	if ( ( pcTSName = getenv ( "TSLIB_TSDEVICE" ) ) != NULL ) //从环境变量里确定触摸屏设备的名字
	{
		g_tTSDev = ts_open ( pcTSName, 0 ); /* 以阻塞方式打开 */
	}
	else
	{
		g_tTSDev = ts_open ( "/dev/event0", 1 ); //如果从环境变量中无法获取到触摸屏设备 以阻塞方式打开/dev/event0
	}


	if ( !g_tTSDev )
	{
		DBG_PRINTF ( "ts_open error!\n" );
		return -1;
	}

	if ( ts_config ( g_tTSDev ) ) //配置
	{
		DBG_PRINTF ( "ts_config error!\n" );
		return -1;
	}

	//if ( GetDispResolution ( &giXres, &giYres ,&iBpp ) ) //获取显示设备的X Y 分辨率 像素
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
	struct ts_sample tSamp;//采样值
	
	/*
	*
	static struct timeval tPreTime;
	*
	*/

	while ( 1 )
	{
		iRet = ts_read ( g_tTSDev, &tSamp, 1 ); /* 如果无数据则休眠 */
		//获得触摸屏数据后直接返回原始数据
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

//将g_TouchScreen放入输入设备链表中
int TouchScreenInit ( void )
{
	return RegisterInputOpr ( &g_TouchScreen );

}
