
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
    
   /* 1. 获得显存 */
   /* 2. 描画数据 */

   /* 3. 刷到设备上去 */

   /* 4. 解放显存 */

}


int MainPageRun (void)
{

	/* 1. 显示页面 */
	showMainPage () /* 2. 创建Prepare线程 */

	/* 3. 调用GetInputEvent获得输入事件，进而处理 */

	while (1)
	{
		//获取输入事件
		switch (MainPageGetInputEvent ())
		{
			case "浏览模式":
				StorePage (); //保存页面
				GetPage ("explore")->Run ();
				RestorePage ();
				break;

			case "联播模式":
				StorePage ();
				GetPage ("auto")->Run ();
				RestorePage ();
				break;

			case "设置":
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


