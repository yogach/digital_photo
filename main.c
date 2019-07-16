
#include <stdio.h>
#include <config.h>
#include <encoding_manager.h>
#include <fonts_manager.h>
#include <disp_manager.h>
#include <input_manager.h>
#include <pic_manager.h>

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
	int iRet;
	unsigned char * pucBMPmem;
	PT_PicFileParser ptBMPFileParser;

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
	pucBMPmem = (unsigned char *)
	mmap (NULL,tBMPstat.st_size,PROT_READ | PROT_WRITE,MAP_SHARED,iFdBmp,0);

	if (pucBMPmem == NULL)
	{
		DBG_PRINTF ("can't mmap bmp file \r\n");
		return - 1;
	}

	//��ʼ��ͼƬ����
	PicFileParserInit ();

	//��ȡBMP����
	ptBMPFileParser = GetPicFileParser ("bmp");

	if (ptBMPFileParser == NULL)
	{
		DBG_PRINTF ("can't find bmp File Parser \r\n");
		return - 1;
	}

	//�жϴ򿪵��ļ��Ƿ���bmp�ļ�
	iRet = ptBMPFileParser->isSupport (pucBMPmem);

	if (iRet == NULL)
	{
	    DBG_PRINTF ("this file is not bmp <%s> \r\n",argv[1]);
		return - 1;
	}

	// ��ȡBMP�ļ���RGB����, ����, ��LCD����ʾ���� 
	//����bmp�ļ�����
	g_tBMPFileParser
}



