
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
	
	int iFdBmp;
	int iRet;
	unsigned char * pucBMPmem;
	

	struct stat tBMPstat;
	PT_DispOpr ptDispOpr;

	PT_PicFileParser ptBMPFileParser;
	T_PhotoDesc tBMPDesc;
	
	T_PhotoDesc tBMPSmall;
	T_PhotoDesc tPixelDatasFB;



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
	ptDispOpr->CleanScreen (0);

	/* 打开BMP文件 */
	iFdBmp = open (argv[1],O_RDWR);

	if (iFdBmp == -1)
	{
		DBG_PRINTF ("can't open bmp file %s\r\n",argv[1]);
		return - 1;
	}

	//获取文件大小 将bmp数据mmap到内存上
	fstat (iFdBmp,&tBMPstat);
	pucBMPmem = (unsigned char *)mmap (NULL,tBMPstat.st_size,PROT_READ | PROT_WRITE,MAP_SHARED,iFdBmp,0);

	if (pucBMPmem == (unsigned char*)-1 )
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

	if (iRet == 0)
	{
		DBG_PRINTF ("this file is not bmp <%s> \r\n",argv[1]);
		return - 1;
	}


	// 提取BMP文件的RGB数据, 缩放, 在LCD上显示出来 
	//tBMPDesc.iBpp = ptDispOpr->iBpp;
	iRet = ptBMPFileParser->GetPixelDatas (pucBMPmem,&tBMPDesc,ptDispOpr->iBpp);
	if (iRet)
	{
		DBG_PRINTF("GetPixelDatas error!\n");
		return -1;		
	}

	//设定fb为主页面
	tPixelDatasFB.iWidth       = ptDispOpr->iXres;
	tPixelDatasFB.iHigh        = ptDispOpr->iYres;
	tPixelDatasFB.iBpp         = ptDispOpr->iBpp;
	tPixelDatasFB.iBpp         = ptDispOpr->iBpp;
	tPixelDatasFB.iLineBytes   = ptDispOpr->iYres *ptDispOpr->iBpp /8;
	tPixelDatasFB.aucPhotoData = ptDispOpr->pucDispMem;

    DBG_PRINTF("tPixelDatasFB.iBpp is %d!\n",tPixelDatasFB.iBpp);

	//合并图标到主页面上
	PicMerge(0,0,&tBMPDesc,&tPixelDatasFB);

	//设置bmp文件缩放
    PicZoom(&tBMPDesc, &tBMPSmall,4);

	//将小图也合并到主页面上
	PicMerge(120,120,&tBMPSmall,&tPixelDatasFB);

	
}



