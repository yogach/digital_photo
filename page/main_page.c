
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


//本页需要显示的图标
static T_Layout g_MainPageLayout[]=
{
   {0,0,0,0,"browse_mode.bmp"},
   {0,0,0,0,"continue_mod.bmp"},
   {0,0,0,0,"setting.bmp"},
   {0,0,0,NULL},
};


//主页面
static int MainPageGetInputEvent ()
{


}

//主页面图片数据准备线程
static int MainPagePrepare ()
{


}

//主页面显示
static int showMainPage (void)
{
   PT_VideoMem pt_VideoTmp;
   int iXres,iYres,iBpp;
   
   /* 1. 获得显存 */
    pt_VideoTmp = GetVideoMem(ID(g_tMainPageAction.name),VMS_FOR_CUR);//获取显存用于当前页面显示
    if(pt_VideoTmp == NULL)
    {
      DBG_PRINTF("GetVideoMem error!\r\n");
	  return -1 ;
    }

    /* 2. 描画数据 */
    if(pt_VideoTmp->ePicState != PIC_GENERATED)
    {
          
	    
        GetDispResolution (&iXres,&iYres,&iBpp);//获取分辨率
        //确定图标的位置


		while()

		



	}


   /* 3. 刷到设备上去 */

   /* 4. 将显存的状态设置为free */

}


static int MainPageRun (void)
{

	/* 1. 显示页面 */
	showMainPage () 
	/* 2. 创建Prepare线程 */

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





int MainPageInit (void)
{
	return RegisterPageAction (&g_tMainPageAction);
}


