
#include <disp_manager.c>
#include "page_manager.h"



static T_Layout g_MainPageLayout[]=
{
   {0,0,0,0,"browse_mode.bmp"},
   {0,0,0,0,"continue_mod.bmp"},
   {0,0,0,0,"setting.bmp"},
   {0,0,0,NULL},
};



int MainPageGetInputEvent ()
{


}


int MainPagePrepare ()
{


}


int showMainPage (void)
{
    
   /* 1. ����Դ� */
   /* 2. �軭���� */

   /* 3. ˢ���豸��ȥ */

   /* 4. ����Դ� */

}


int MainPageRun (void)
{

	/* 1. ��ʾҳ�� */
	showMainPage () /* 2. ����Prepare�߳� */

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


static T_PageAction g_tMainPageAction =
{
	.name = "main",
	.Run = MainPageRun,
	.GetInputEvent = MainPageGetInputEvent,
	.Prepare = MainPagePrepare,
}


int MainPageInit (void)
{
	return RegisterPageAction (&g_tMainPageAction);
}


