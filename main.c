
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

	//��ʼ����ӡ
	DebugInit ();
	InitDebugChanel ();


	DisplayInit ();


	ptDispOpr = GetDispOpr ("fb");

	ptDispOpr->DeviceInit ();
	ptDispOpr->CleanScreen (0);

	/* ��BMP�ļ� */
	iFdBmp = open (argv[1],O_RDWR); //�޷�ʹ��ֻ��ģʽ�򿪣�

	if (iFdBmp == -1)
	{
		DBG_PRINTF ("can't open bmp file %s\r\n",argv[1]);
		return - 1;
	}

	//��ȡ�ļ���С ��bmp����mmap���ڴ���
	fstat (iFdBmp,&tBMPstat);
	pucBMPmem = (unsigned char *)mmap (NULL,tBMPstat.st_size,PROT_READ | PROT_WRITE,MAP_SHARED,iFdBmp,0);

	if (pucBMPmem == (unsigned char*)-1 )
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

	if (iRet == 0)
	{
		DBG_PRINTF ("this file is not bmp <%s> \r\n",argv[1]);
		return - 1;
	}


	// ��ȡBMP�ļ���RGB����, ����, ��LCD����ʾ���� 
	//tBMPDesc.iBpp = ptDispOpr->iBpp;
	iRet = ptBMPFileParser->GetPixelDatas (pucBMPmem,&tBMPDesc,ptDispOpr->iBpp);
	if (iRet)
	{
		DBG_PRINTF("GetPixelDatas error!\n");
		return -1;		
	}

	//�趨fbΪ��ҳ��
	tPixelDatasFB.iWidth       = ptDispOpr->iXres;
	tPixelDatasFB.iHigh        = ptDispOpr->iYres;
	tPixelDatasFB.iBpp         = ptDispOpr->iBpp;
	//tPixelDatasFB.iBpp         = ptDispOpr->iBpp;
	tPixelDatasFB.iLineBytes   = ptDispOpr->iXres *ptDispOpr->iBpp /8;
	tPixelDatasFB.aucPhotoData = ptDispOpr->pucDispMem;

    DBG_PRINTF("tPixelDatasFB.iBpp is %d!\n",tPixelDatasFB.iBpp);

	//�ϲ�ͼ�굽��ҳ����
	PicMerge(0,0,&tBMPDesc,&tPixelDatasFB);
   // DBG_PRINTF("after PicMerge\n");

	//����bmp�ļ�����
    tBMPSmall.iWidth = tBMPDesc.iWidth/2;
	tBMPSmall.iHigh = tBMPDesc.iHigh/2;
	
	DBG_PRINTF("tBMPDesc.iWidth = %d tBMPDesc.iHigh =%d\r\n",tBMPDesc.iWidth,tBMPDesc.iHigh);
    DBG_PRINTF("tBMPSmall.iWidth = %d tBMPSmall.iHigh =%d\r\n",tBMPSmall.iWidth,tBMPSmall.iHigh);

	tBMPSmall.iBpp	= tBMPDesc.iBpp;
	tBMPSmall.iLineBytes = tBMPSmall.iWidth * tBMPSmall.iBpp / 8;
	tBMPSmall.aucPhotoData = malloc(tBMPSmall.iLineBytes * tBMPSmall.iHigh);

    PicZoom(&tBMPDesc, &tBMPSmall);

	//��СͼҲ�ϲ�����ҳ����
	PicMerge(160,160,&tBMPSmall,&tPixelDatasFB);

	
}



