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
static T_Layout g_atMainPageIconsLayout[]=
{
	{0,0,0,0,"browse_mode.bmp"},
	{0,0,0,0,"continue_mod.bmp"},
	{0,0,0,0,"setting.bmp"},
	{0,0,0,0,NULL},
};


//��ҳ�������¼�
static int MainPageGetInputEvent (PT_Layout ptLayout,PT_InputEvent ptInputEvent )
{
   //��ô�����ԭʼ����
   return GenericGetInputEvent(ptLayout,ptInputEvent);
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
              //���õ�ǰͼ�����ʼ����x y ����
              atLayout->iTopLeftX    = IconX;
			  atLayout->iTopLeftY    = IconY;
			  atLayout->iLowerRightX = IconX + iIconWidth - 1; //���½�X����
			  atLayout->iLowerRightY = IconY + iIconHight - 1; //���½�Y����
			  
			  //��ȡͼ���ͼƬ����
			  GetPixelDatasFormIcon(atLayout->IconName ,&tPhotoOriData);

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
    T_InputEvent tInputEvent;
    int iIndex,iIndexPressured=-1,bPressure = 0;

	/* 1. ��ʾҳ�� */
	showMainPage (g_atMainPageIconsLayout);
	/* 2. ����Prepare�߳� */

	/* 3. ��������¼��õ����µ�icon���������� */
	while ( 1 )
	{
		//������ĸ�ͼ���а���
        iIndex = MainPageGetInputEvent (g_atMainPageIconsLayout,&tInputEvent);
		if (iIndex >= 0)
			DBG_PRINTF("put\release status:%d , icon num:%d\r\n",tInputEvent.iPressure,iIndex);

		if(tInputEvent.iPressure == 0)//������ɿ�״̬
		{
           if(bPressure)//����������� 
           {
            //�ı䰴���������ɫ

			bPressure = 0;
             if(iIndexPressured == iIndex)//����������ɿ�����ͬһ������
             {

                switch ( iIndexPressured )
				{
					case 0://

						break;

					case 1://

						break;

					case 2://

						break;

					default:
		                break;
				}


			 }
		   }
		}
		else //����ǰ���״̬
		{
          if(!bPressure)// δ�����°�ť 
          {
			  bPressure = 1;
              iIndexPressured = iIndex;
			  //�ı䰴���������ɫ
			  
		  }

		}

	}

	return 0;

}


int MainPageInit ( void )
{
	return RegisterPageAction ( &g_tMainPageAction );
}


