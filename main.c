
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
	T_PhotoDesc tBMPDesc;
	T_PhotoDesc tBMPSmall;
	T_PhotoDesc tFBDesc;

	struct tBMPstat;

	if (argc != 2)
	{
		printf ("%s <BMPFile>\r\n",argv[0]);
		return - 1;
	}

	//初始化打印
	DebugInit ();
	InitDebugChanel ();


	DisplayInit ();


	ptDispOpr = GetDispOpr ("fb");

	ptDispOpr->DeviceInit ();
	ptDispOpr->CleanScreen ();

	/* 打开BMP文件 */
	iFdBmp = open (argv[1],O_RDONLY);

	if (iFdBmp == -1)
	{
		DBG_PRINTF ("can't open bmp file %s\r\n",argv[1]);
		return - 1;
	}

	//获取文件大小 将bmp数据mmap到内存上
	fstat (iFdBmp,&tBMPstat);
	pucBMPmem = (unsigned char *)
	mmap (NULL,tBMPstat.st_size,PROT_READ | PROT_WRITE,MAP_SHARED,iFdBmp,0);

	if (pucBMPmem == NULL)
	{
		DBG_PRINTF ("can't mmap bmp file \r\n");
		return - 1;
	}

	//初始化图片处理
	PicFileParserInit ();

	//获取BMP测试
	ptBMPFileParser = GetPicFileParser ("bmp");

	if (ptBMPFileParser == NULL)
	{
		DBG_PRINTF ("can't find bmp File Parser \r\n");
		return - 1;
	}

	//判断打开的文件是否是bmp文件
	iRet = ptBMPFileParser->isSupport (pucBMPmem);

	if (iRet == NULL)
	{
		DBG_PRINTF ("this file is not bmp <%s> \r\n",argv[1]);
		return - 1;
	}


	// 提取BMP文件的RGB数据, 缩放, 在LCD上显示出来 
	ptBMPFileParser->GetPixelDatas (pucBMPmem,&tBMPDesc,24);

	//设定fb为主页面
	tFBDesc.iWidth = ptDispOpr->iXres;
	tFBDesc.iHigh = ptDispOpr->iYres;
	tFBDesc.iBpp = ptDispOpr->iBpp;
	tFBDesc.iLineBytes = ptDispOpr->iYres;
	tFBDesc.aucPhotoData = ptDispOpr->pucDispMem;

	//合并图标到主页面上
	Pic_Merge(120,120,&tBMPDesc,&tFBDesc);

	//设置bmp文件缩放
    PicZoom(&tBMPDesc, &tBMPSmall,4);

	
}



