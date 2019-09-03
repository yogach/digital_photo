#include <stdio.h>
#include <config.h>
#include <encoding_manager.h>
#include <fonts_manager.h>
#include <disp_manager.h>
#include <input_manager.h>
#include <pic_manager.h>
#include <page_manager.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>




int main ( int argc,char * *argv )
{
	int iError;

	//初始化 调试通道
	DebugInit ();
	InitDebugChanel ();

	//初始化显示
	DisplayInit ();
	//选择默认显示设备
	SelectAndInitDefaultDispDev ( "fb" );
	//分配显存
	AllocVideoMem ( 5 );

	//输入设备初始化
	InputInit();
	//根据输入设备分配线程
	AllInputDevicesInit();

	//注册字库模块
	iError = FontsInit();
	if ( iError )
	{
		DBG_PRINTF ( "FontsInit error! \r\n" );
	}

	//
	iError = SetFontsDetail ( "freetype",argv[1],24 );
	if ( iError )
	{
		DBG_PRINTF ( "SetFontsDetail error! \r\n" );
	}

	//初始化图片支持
	PicFileParserInit();
	ShowPicFileParser();

	//初始化页面链表
	PagesInit();
	Page ( "main" )->Run();



	return 0;
}



