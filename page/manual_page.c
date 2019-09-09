#include <config.h>
#include <disp_manager.h>
#include <page_manager.h>
#include <pic_manager.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <input_manager.h>
#include <render.h>

static int CalcManualPageLayout(PT_Layout atLayout);
static int ManualPageSpecialDis(PT_VideoMem ptVideoMem);
static void ManualPageRun(void);


static T_Layout g_atManualPageIconsLayout[] =
{
	{0, 0, 0, 0, "return.bmp"},
	{0, 0, 0, 0, "zoomout.bmp"},
	{0, 0, 0, 0, "zoomin.bmp"},
	{0, 0, 0, 0, "pre_pic.bmp"},
    {0, 0, 0, 0, "next_pic.bmp"},
    {0, 0, 0, 0, "continue_mod_small.bmp"},
	{0, 0, 0, 0, NULL},
};


static T_PageDesc g_tManualPageDesc =
{
	.name   = "manual",
	.Run    = ManualPageRun,
	.CalcPageLayout = CalcManualPageLayout,
	.atPageLayout = g_atManualPageIconsLayout,
	.DispSpecialIcon = ManualPageSpecialDis,
};


static int CalcManualPageLayout(PT_Layout atLayout)
{

   //获得屏幕分辨率
   	int iXres,iYres,iBpp;
	int iIconWidth,iIconHight,IconX,IconY;
	GetDispResolution ( &iXres,&iYres,&iBpp ); //获取LCD分辨率


    if(iXres < iYres)
    {
		/*	 iXres/6
		 *	  --------------------------------------------------------------
		 *	   return	zoomout	zoomin  pre_pic next_pic continue_mod_small
		 *
		 *
		 *
		 *
		 *
		 *
		 *	  --------------------------------------------------------------
		 */





	}
	else
	{

		/*	 iYres/6
		 *	  ----------------------------------
		 *	   up		  
		 *
		 *    zoomout	    
		 *
		 *    zoomin
		 *  
		 *    pre_pic
		 *
		 *    next_pic
		 *
		 *    continue_mod_small
		 *
		 *	  ----------------------------------
		 */





	}


}


static int ManualPageSpecialDis(PT_VideoMem ptVideoMem)
{




}

static void ManualPageRun(void)
{
  /* 1. 显示页面 */
  ShowPage ( &g_tManualPageDesc );

  while(1)
  {



  }


}


int ManualPageInit ( void )
{
	return RegisterPageAction ( &g_tManualPageDesc );
}



