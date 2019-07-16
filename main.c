
#include <stdio.h>
#include <config.h>
#include <encoding_manager.h>
#include <fonts_manager.h>
#include <disp_manager.h>
#include <input_manager.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>




int main (int argc,char * *argv)
{
	PT_DispOpr ptDispOpr;
	int iFdBmp;
	unsigned char *pucBMPmem;

	struct tBMPstat;

	if (argc != 2)
	{
		printf ("%s <BMPFile>\r\n",argv[0]);
		return - 1;
	}

	//��ʼ����ӡ
	DebugInit ();
	InitDebugChanel ();

	
	DisplayInit ();
	

	ptDispOpr = GetDispOpr ("fb");

	ptDispOpr->DeviceInit ();
	ptDispOpr->CleanScreen ();

	/* ��BMP�ļ� */
	iFdBmp = open (argv[1],O_RDONLY);

	if (iFdBmp == -1)
	{
		DBG_PRINTF ("can't open bmp file %s\r\n",argv[1]);
		return - 1;
	}

	//��ȡ�ļ���С ��bmp����mmap���ڴ���
	fstat (iFdBmp,&tBMPstat);
    pucBMPmem = (unsigned char *)mmap(NULL , tBMPstat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, iFdBmp, 0);
	if(pucBMPmem == NULL)
    {
		DBG_PRINTF ("can't mmap bmp file \r\n");
		return - 1;
	}

    //��ʼ��ͼƬ����
	PicFileParserInit();
		
    //�жϴ򿪵��ļ��Ƿ���bmp�ļ�
    

    // ��ȡBMP�ļ���RGB����, ����, ��LCD����ʾ���� 
    

	//����bmp�ļ�����
	
	g_tBMPFileParser
}



