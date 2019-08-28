#include <config.h>
#include <disp_manager.h>
#include "page_manager.h"
#include <pic_manager.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>


static int MainPageGetInputEvent ();
static int MainPagePrepare ();
static int MainPageRun ( void );
//static PT_DispOpr g_tDispOpr;


static T_PageAction g_tMainPageAction =
{
	.name = "main",
	.Run = MainPageRun,
	.GetInputEvent = MainPageGetInputEvent,
	//.Prepare = MainPagePrepare,
};


//��ҳ��Ҫ��ʾ��ͼ��
static T_Layout g_MainPageLayout[]=
{
	{0,0,0,0,"browse_mode.bmp"},
	{0,0,0,0,"continue_mod.bmp"},
	{0,0,0,0,"setting.bmp"},
	{0,0,0,0,NULL},
};


//��ҳ�������¼�
static int MainPageGetInputEvent (void)
{
   //��ô�����ԭʼ����



   return 0;
}

//��ҳ��ͼƬ����׼���߳�
/*static int MainPagePrepare ()
{

 return 0;
}*/

//��ҳ����ʾ
static int showMainPage ( PT_Layout atLayout )
{
	PT_VideoMem pt_VideoTmp;
	int iXres,iYres,iBpp;
	int iIconWidth,iIconHight,IconX,IconY;
    //int iError;

	T_PhotoDesc tPhotoOriData;
	T_PhotoDesc tPhotoNew;

	/* 1. ����Դ� */
	pt_VideoTmp = GetVideoMem ( ID ( g_tMainPageAction.name ),VMS_FOR_CUR ); //��ȡ�Դ����ڵ�ǰҳ����ʾ
	if ( pt_VideoTmp == NULL )
	{
		DBG_PRINTF ( "GetVideoMem error!\r\n" );
		return -1 ;
	}

	/* 2. �軭���� */
	if ( pt_VideoTmp->ePicState != PIC_GENERATED )//���ͼƬδ׼����
	{

		GetDispResolution ( &iXres,&iYres,&iBpp ); //��ȡ�ֱ���
		//����ȷ���׸�ͼ�������
		iIconHight =  iYres*2/10 ;   //ͼ��߶�
		iIconWidth =  iIconHight*2;  //ͼ����

		IconX =  ( iXres-iIconWidth ) /2; //ͼ�����
		IconY =  iYres /10 ;

		//����������Ϣ����
		tPhotoNew.iBpp = iBpp;
		tPhotoNew.iHigh = iIconHight;
		tPhotoNew.iWidth = iIconWidth;
		tPhotoNew.iLineBytes = tPhotoNew.iWidth * tPhotoNew.iBpp / 8;
		tPhotoNew.iTotalBytes = tPhotoNew.iHigh * tPhotoNew.iLineBytes;
		tPhotoNew.aucPhotoData = malloc ( tPhotoNew.iTotalBytes );
		if ( tPhotoNew.aucPhotoData == NULL )
		{
			DBG_PRINTF ( "malloc tPhotoNew error\r\n" );
			return -1;
		}

		while(atLayout->IconName)
		{
              //�õ���ǰͼ�����ʼ������ַ
              atLayout->iTopLeftX    = IconX;
			  atLayout->iTopLeftY    = IconY;
			  atLayout->iLowerRightX = IconX + iIconWidth - 1; //���½�X����
			  atLayout->iLowerRightY = IconY + iIconHight - 1; //���½�Y����
			  
			  //��ȡͼ���ͼƬ����
			  GetPixelDatasForIcon(atLayout->IconName ,&tPhotoOriData);

              //��ԭʼͼƬ�������ŵ�ָ����С
              PicZoom(&tPhotoOriData, &tPhotoNew);
			  //��ͼƬ�ϲ����Դ���
			  PicMerge(atLayout->iTopLeftX, atLayout->iTopLeftY, &tPhotoNew, &pt_VideoTmp->tVideoMemDesc);

              //�ͷ�ͼƬ������ڴ� ��ֹ�ڴ�й¶ 
              FreePixelDatasForIcon(&tPhotoOriData);

			  //Y�������µ���
              IconY +=   iYres*3/10 ;
   
		      atLayout++; //ָ��+1 ���ӵĳ�����ָ������������й�
		}
		free(tPhotoNew.aucPhotoData); 
	}

	/* 3. ˢ���豸��ȥ       */
	FlushVideoMemToDev(pt_VideoTmp);
    
	/* 4. ���Դ��״̬����Ϊfree */
	PutVideoMem(pt_VideoTmp);

	return 0;

}


static int MainPageRun ( void )
{

	/* 1. ��ʾҳ�� */
	showMainPage (g_MainPageLayout);
	/* 2. ����Prepare�߳� */

	/* 3. ����GetInputEvent��������¼����������� */
	while ( 1 )
	{
		//��ȡ�����¼�
		switch ( MainPageGetInputEvent () )
		{
			case "���ģʽ":

				break;

			case "����ģʽ":

				break;

			case "����":

				break;
		}


	}

	return 0;

}





int MainPageInit ( void )
{
	return RegisterPageAction ( &g_tMainPageAction );
}


