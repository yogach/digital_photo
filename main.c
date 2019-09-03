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

	//��ʼ�� ����ͨ��
	DebugInit ();
	InitDebugChanel ();

	//��ʼ����ʾ
	DisplayInit ();
	//ѡ��Ĭ����ʾ�豸
	SelectAndInitDefaultDispDev ( "fb" );
	//�����Դ�
	AllocVideoMem ( 5 );

	//�����豸��ʼ��
	InputInit();
	//���������豸�����߳�
	AllInputDevicesInit();

	//ע���ֿ�ģ��
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

	//��ʼ��ͼƬ֧��
	PicFileParserInit();
	ShowPicFileParser();

	//��ʼ��ҳ������
	PagesInit();
	Page ( "main" )->Run();



	return 0;
}



