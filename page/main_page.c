#include <config.h>
#include <disp_manager.h>
#include <page_manager.h>
#include <pic_manager.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <input_manager.h>

static int MainPageGetInputEvent ( PT_Layout atLayout,PT_InputEvent ptInputEvent );
static int MainPageRun ( void );
//static PT_DispOpr g_tDispOpr;

static T_PageAction g_tMainPageAction =
{
	.name = "main",
	.Run = MainPageRun,
	.GetInputEvent = MainPageGetInputEvent,
	//.Prepare = MainPagePrepare,
};


//本页需要显示的图标
static T_Layout g_atMainPageIconsLayout[]=
{
	{0,0,0,0,"browse_mode.bmp"},
	{0,0,0,0,"continue_mod.bmp"},
	{0,0,0,0,"setting.bmp"},
	{0,0,0,0,NULL},
};


//主页面输入事件
static int MainPageGetInputEvent ( PT_Layout atLayout,PT_InputEvent ptInputEvent )
{
	//获得触摸屏原始数据
	return GenericGetInputEvent ( atLayout,ptInputEvent );
}

/**********************************************************************
 * 函数名称： CalcMainPageLayout
 * 功能描述： 计算页面中各图标座标值
 * 输入参数： 无
 * 输出参数： atLayout - 内含各图标的左上角/右下角座标值
 * 返 回 值： 无

 ***********************************************************************/
static int CalcMainPageLayout ( PT_Layout atLayout )
{
	int iXres,iYres,iBpp;
	int iIconWidth,iIconHight,IconX,IconY;
	GetDispResolution ( &iXres,&iYres,&iBpp ); //获取分辨率

	/*   每个图标高度为2/10 Y分辨率
	 *   宽度为高度的两倍
	 *    ----------------------
	 *                           1/10 * iYres
	 *         browse_mode.bmp   2/10 * iYres
	 *                           1/10 * iYres
	 *         continue_mod.bmp  2/10 * iYres
	 *                           1/10 * iYres
	 *         setting.bmp       2/10 * iYres
	 *                           1/10 * iYres
	 *    ----------------------
	 *
	 */

	iIconHight =  iYres*2/10 ;   //图标高度
	iIconWidth =  iIconHight*2;  //图标宽度

	IconX =  ( iXres-iIconWidth ) /2; //图标居中 
	IconY =  iYres /10 ;

	while ( atLayout->IconName )
	{
		//设置本页所有图标的起始结束x y 坐标
		atLayout->iTopLeftX    = IconX;                  //左上角X坐标     
		atLayout->iTopLeftY    = IconY;                  //左上角X坐标
		atLayout->iLowerRightX = IconX + iIconWidth - 1; //右下角X坐标
		atLayout->iLowerRightY = IconY + iIconHight - 1; //右下角Y坐标
		
		//Y坐标往下递增
		IconY +=   iYres*3/10 ;
		atLayout++; //指针+1 指向数组下一项
	}

}


//主页面显示
static int showMainPage ( PT_Layout atLayout )
{
	PT_VideoMem pt_VideoTmp;
	int iError;

	/* 1. 获得显存 */
	pt_VideoTmp = GetVideoMem ( ID ( g_tMainPageAction.name ),VMS_FOR_CUR ); //获取显存用于当前页面显示
	if ( pt_VideoTmp == NULL )
	{
		DBG_PRINTF ( "GetVideoMem error!\r\n" );
		return -1 ;
	}

    /* 2. 生成图标坐标 */
    if(atLayout->iTopLeftX == 0)
    {
       CalcMainPageLayout(atLayout);
	}

	/* 3. 描画数据 */
	iError = GeneratePage(atLayout,pt_VideoTmp);

	/* 3. 刷到设备上去       */
	FlushVideoMemToDev ( pt_VideoTmp );

	/* 4. 将显存的状态设置为free */
	PutVideoMem ( pt_VideoTmp );

	return 0;

}


static int MainPageRun ( void )
{
	T_InputEvent tInputEvent;
	int iIndex,iIndexPressured=-1,bPressure = 0;

	/* 1. 显示页面 */
	showMainPage ( g_atMainPageIconsLayout );
	/* 2. 创建Prepare线程 */

	/* 3. 通过输入事件获得按下的icon 进而处理 */
	while ( 1 )
	{
		//获得在哪个图标中按下
		iIndex = MainPageGetInputEvent ( g_atMainPageIconsLayout,&tInputEvent );
		/*if (iIndex >= 0)
			DBG_PRINTF("put\release status:%d , icon num:%d\r\n",tInputEvent.iPressure,iIndex);
		else
			DBG_PRINTF("don't touch icon\r\n");*/

		if ( tInputEvent.iPressure == 0 ) //如果是松开状态
		{
			if ( bPressure ) //如果曾经按下
			{
				//改变按键区域的颜色
				ReleaseButton ( &g_atMainPageIconsLayout[iIndex] );
				bPressure = 0;


				if ( iIndexPressured == iIndex ) //如果按键和松开的是同一个按键
				{

					switch ( iIndexPressured )
					{
						case 0://浏览模式

							break;

						case 1://连播页面

							break;

						case 2://设置页面

							break;

						default:
							break;
					}

				}

				iIndexPressured = -1;
			}
		}
		else //如果是按下状态
		{
			if ( !bPressure ) // 未曾按下按钮
			{
				bPressure = 1;
				iIndexPressured = iIndex;
				//改变按键区域的颜色
				PressButton ( &g_atMainPageIconsLayout[iIndex] );
			}

		}

	}

	return 0;

}


int MainPageInit ( void )
{
	return RegisterPageAction ( &g_tMainPageAction );
}


