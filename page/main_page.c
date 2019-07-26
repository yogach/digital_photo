
#include <disp_manager.c>
#include "page_manager.h"

static int MainPageGetInputEvent ();
static int MainPagePrepare ();
static int MainPageRun (void);
static PT_DispOpr g_tDispOpr;


static T_PageAction g_tMainPageAction =
{
	.name = "main",
	.Run = MainPageRun,
	.GetInputEvent = MainPageGetInputEvent,
	.Prepare = MainPagePrepare,
}


//��ҳ��Ҫ��ʾ��ͼ��
static T_Layout g_MainPageLayout[]=
{
   {0,0,0,0,"browse_mode.bmp"},
   {0,0,0,0,"continue_mod.bmp"},
   {0,0,0,0,"setting.bmp"},
   {0,0,0,NULL},
};


//��ҳ��
static int MainPageGetInputEvent ()
{


}

//��ҳ��ͼƬ����׼���߳�
static int MainPagePrepare ()
{


}

//��ҳ����ʾ
static int showMainPage (void)
{
   PT_VideoMem pt_VideoTmp;
   int iXres,iYres,iBpp;
   
   /* 1. ����Դ� */
    pt_VideoTmp = GetVideoMem(ID(g_tMainPageAction.name),VMS_FOR_CUR);//��ȡ�Դ����ڵ�ǰҳ����ʾ
    if(pt_VideoTmp == NULL)
    {
      DBG_PRINTF("GetVideoMem error!\r\n");
	  return -1 ;
    }

    /* 2. �軭���� */
    if(pt_VideoTmp->ePicState != PIC_GENERATED)
    {
          
	    
        GetDispResolution (&iXres,&iYres,&iBpp);//��ȡ�ֱ���
        //ȷ��ͼ���λ��


		while()

		



	}


   /* 3. ˢ���豸��ȥ */

   /* 4. ���Դ��״̬����Ϊfree */

}


static int MainPageRun (void)
{

	/* 1. ��ʾҳ�� */
	showMainPage () 
	/* 2. ����Prepare�߳� */

	/* 3. ����GetInputEvent��������¼����������� */
	while (1)
	{
		//��ȡ�����¼�
		switch (MainPageGetInputEvent ())
		{
			case "���ģʽ":
				StorePage (); //����ҳ��
				GetPage ("explore")->Run ();
				RestorePage ();
				break;

			case "����ģʽ":
				StorePage ();
				GetPage ("auto")->Run ();
				RestorePage ();
				break;

			case "����":
				StorePage ();
				GetPage ("setting")->Run ();
				RestorePage ();
				break;
		}


	}

	return 0;

}





int MainPageInit (void)
{
	return RegisterPageAction (&g_tMainPageAction);
}


