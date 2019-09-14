#include <config.h>
#include <disp_manager.h>
#include <page_manager.h>
#include <pic_manager.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <input_manager.h>
#include <render.h>


static void IntervalPageRun ( void );
static int CalcIntervalPageLayout ( PT_Layout atLayout );
static int IntervalPageSpecialDis ( PT_VideoMem ptVideoMem );


static T_Layout g_atIntervalPageIconsLayout[] =
{
	{0, 0, 0, 0, "inc.bmp"},
	{0, 0, 0, 0, "time.bmp"},
	{0, 0, 0, 0, "dec.bmp"},
	{0, 0, 0, 0, "ok.bmp"},
	{0, 0, 0, 0, "cancel.bmp"},
	{0, 0, 0, 0, NULL},
};


static T_PageDesc g_tIntervalPageDesc =
{
	.name   = "interval",
	.Run    = IntervalPageRun,
	.CalcPageLayout = CalcIntervalPageLayout,
	.atPageLayout = g_atIntervalPageIconsLayout,
	.DispSpecialIcon = IntervalPageSpecialDis,
};

static T_Layout g_NumDispLayout; //用于表示数字显示内框的起始结束坐标
static int g_iIntervalSecond = 1;//用于显示时间间隔

/**********************************************************************
 * 函数名称： CalcIntervalPageLayout
 * 功能描述： 计算页面中各图标座标值
 * 输入参数： 无
 * 输出参数： ptPageLayout - 内含各图标的左上角/右下角座标值
 * 返 回 值： 无
 ***********************************************************************/
static int CalcIntervalPageLayout ( PT_Layout atLayout )
{
	int iXres,iYres,iBpp;
	int iIconWidth,iIconHight,IconX,IconY;
	GetDispResolution ( &iXres,&iYres,&iBpp ); //获取LCD分辨率


	/*
	 *    ----------------------
	 *                          1/6 iYres             1/2 * iHeight
	 *          inc.bmp         1/3 iYres  * 28 / 128 iHeight * 28 / 128
	 *         time.bmp         1/3 iYres  * 72 / 128 iHeight * 72 / 128
	 *          dec.bmp         1/3 iYres  * 28 / 128 iHeight * 28 / 128
	 *                          1/6 iYres             1/2 * iHeight
	 *    ok.bmp     cancel.bmp 1/6 iYres             1/2 * iHeight
	 *                          1/6 iYres             1/2 * iHeight
	 *    ----------------------
	 */

	//1、inc.bmp
	iIconHight = iYres/3 *28 /128;
	iIconWidth = iIconHight*2;
	IconX =  ( iXres-iIconWidth ) /2; //图标居中
	IconY =  iYres /6 ;

	atLayout[0].iTopLeftX    = IconX;
	atLayout[0].iTopLeftY    = IconY;
	atLayout[0].iLowerRightX = IconX + iIconWidth - 1;
	atLayout[0].iLowerRightY = IconY + iIconHight - 1;


	//2、time.bmp
	iIconHight = iYres/3 *72 /128;
	iIconWidth = iIconHight*2;
	IconX =  ( iXres-iIconWidth ) /2; //图标居中
	//IconY =  iYres /6 ;
	atLayout[1].iTopLeftX    = IconX;
	atLayout[1].iTopLeftY    = atLayout[0].iLowerRightY + 1;
	atLayout[1].iLowerRightX = IconX + iIconWidth - 1;
	atLayout[1].iLowerRightY = atLayout[1].iTopLeftY + iIconHight - 1;

	//3、dec.bmp
	iIconHight = iYres/3 *28 /128;
	iIconWidth = iIconHight*2;
	IconX =  ( iXres-iIconWidth ) /2; //图标居中
	//IconY =  iYres /6 ;
	atLayout[2].iTopLeftX    = IconX;
	atLayout[2].iTopLeftY    = atLayout[1].iLowerRightY + 1;
	atLayout[2].iLowerRightX = IconX + iIconWidth - 1;
	atLayout[2].iLowerRightY = atLayout[2].iTopLeftY + iIconHight - 1;

	//4、ok.bmp
	iIconHight = iYres/6;
	iIconWidth = iIconHight ;
	IconX =  ( iXres - iIconWidth ) / 3; //
	//IconY =  iYres /6 ;
	atLayout[3].iTopLeftX    = IconX;
	atLayout[3].iTopLeftY    = atLayout[2].iLowerRightY + iYres/6;
	atLayout[3].iLowerRightX = IconX + iIconWidth - 1;
	atLayout[3].iLowerRightY = atLayout[3].iTopLeftY + iIconHight - 1;


	//5、cancel.bmp
	atLayout[4].iTopLeftX    = atLayout[3].iLowerRightX + iXres/3;
	atLayout[4].iTopLeftY    = atLayout[3].iTopLeftY;
	atLayout[4].iLowerRightX = atLayout[4].iTopLeftX + iIconWidth - 1;
	atLayout[4].iLowerRightY = atLayout[3].iLowerRightY;


	/* 用来显示数字的区域比较特殊, 单独处理
	 * time.bmp原图大小为128x72, 里面的两个数字大小为52x40
	 * 经过CalcIntervalPageLayout后有所缩放 设置它的对应大小
	 */
	iIconHight = atLayout[1].iLowerRightY - atLayout[1].iTopLeftY + 1;
	iIconWidth = atLayout[1].iLowerRightX - atLayout[1].iTopLeftX + 1;

	g_NumDispLayout.iTopLeftX    = atLayout[1].iTopLeftX + ( 128 - 52 ) /2 *iIconWidth/128;
	g_NumDispLayout.iLowerRightX = atLayout[1].iLowerRightX - ( 128 - 52 ) /2 *iIconWidth/128 + 1;

	g_NumDispLayout.iTopLeftY    = atLayout[1].iTopLeftY + ( 72 -	40 ) /2 *iIconHight/72;
	g_NumDispLayout.iLowerRightY = atLayout[1].iLowerRightY - ( 72 -  40 ) /2 *iIconHight/72 + 1;


	return 0;

}



/**********************************************************************
 * 函数名称： GenerateIntervalPageSpecialIcon
 * 功能描述： 为"interval页面"生成特殊图标: 绘制图标中的数字
 * 输入参数： dwNumber   - 要显示出来的数字
 *            ptVideoMem - 在这个VideoMem中显示
 * 输出参数： 无
 * 返 回 值： 0      - 成功
 *            其他值 - 失败
 ***********************************************************************/
static int GenerateIntervalPageSpecialIcon ( int dwNumber, PT_VideoMem ptVideoMem )
{
	unsigned int dwFontSize;
	char strNumber[3];
	int iError;

	//获得显示数字的高度
	dwFontSize = g_NumDispLayout.iLowerRightY - g_NumDispLayout.iTopLeftY;

	SetFontSize ( dwFontSize ); //设置字体大小

	//限制数字的大小
	if ( dwNumber > 59 )
	{
		// dwNumber = 59;
		return -1;
	}

	//将数字转化为字符串 长度为3 如果数字小于10前面补0 包含结束符
	snprintf ( strNumber, 3, "%02d", dwNumber );

	//在指定区域内居中显示
	iError = MergerStringToCenterOfRectangleInVideoMem ( g_NumDispLayout.iTopLeftX,g_NumDispLayout.iTopLeftY,g_NumDispLayout.iLowerRightX,\
	                                                     g_NumDispLayout.iLowerRightY, ( unsigned char* ) strNumber,ptVideoMem );
	return iError;


}

static int IntervalPageSpecialDis ( PT_VideoMem ptVideoMem )
{
	return GenerateIntervalPageSpecialIcon ( g_iIntervalSecond,ptVideoMem );
}


/**********************************************************************
 * 函数名称： GetIntervalSecond
 * 功能描述： 用于获取设定的间隔时间
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： g_iIntervalSecond - 图片显示间隔
 ***********************************************************************/
int GetIntervalSecond(void)
{
  return g_iIntervalSecond;
}

static void IntervalPageRun ( void )
{
	//T_InputEvent tInputEvent ,tInputEventPrePress;
	//int iIndex,iIndexPressed=-1,bPressed = 0 ,bFast = 0;
	int iIntervalSecond = g_iIntervalSecond , bLongPress;
	PT_VideoMem ptDevVideoMem;
    
	//获得显示设备显存
	ptDevVideoMem = GetDevVideoMen();




	/* 1. 显示页面 */
	ShowPage ( &g_tIntervalPageDesc );

	/* 2. 通过输入事件获得按下的icon 进而处理 */
	while ( 1 )
	{
#if 1
		switch ( GenericGetPressedIcon ( g_atIntervalPageIconsLayout, &bLongPress) )
		{
			case 0://数字增加
			{
                if(bLongPress)
                    iIntervalSecond+=10;
				else
					iIntervalSecond++;
				
				iIntervalSecond = iIntervalSecond%60;
				
				GenerateIntervalPageSpecialIcon ( iIntervalSecond,ptDevVideoMem );
			}
			break;


			case 2://数字减少
			{
			    if(bLongPress)
                    iIntervalSecond-=10;
				else
					iIntervalSecond--;
				
				if ( iIntervalSecond <= 0 )
				{
					iIntervalSecond = 59 + iIntervalSecond;
				}
				GenerateIntervalPageSpecialIcon ( iIntervalSecond,ptDevVideoMem );
			}
			break;

			case 3://确认按键
			{
				g_iIntervalSecond = iIntervalSecond;											
				return;

			}
			break;

			case 4://返回按键
			{
				return;
			}
			break;


			default:
				break;
		}
#else 
		iIndex = GenericGetInputEvent(g_atIntervalPageIconsLayout, &tInputEvent);
 		if (tInputEvent.iPressure == 0)
		{
			/* 如果是松开 */
			if (bPressed)
			{
				bFast = 0;
				
				/* 曾经有按钮被按下 */
				ReleaseButton(&g_atIntervalPageIconsLayout[iIndexPressed]);
				bPressed = 0;

				if (iIndexPressed == iIndex) /* 按下和松开都是同一个按钮 */
				{
					switch (iIndexPressed)
					{
						case 0: /* inc按钮 */
						{
							iIntervalSecond++;
							if (iIntervalSecond == 60)
							{
								iIntervalSecond = 0;
							}
							GenerateIntervalPageSpecialIcon(iIntervalSecond, ptDevVideoMem);
							break;
						}
						case 2: /* dec按钮 */
						{
							iIntervalSecond--;
							if (iIntervalSecond == -1)
							{
								iIntervalSecond = 59;
							}
							GenerateIntervalPageSpecialIcon(iIntervalSecond, ptDevVideoMem);
							break;
						}
						case 3: /* ok按钮 */
						{
							g_iIntervalSecond = iIntervalSecond;
							
							return;
							break;
						}
						case 4: /* cancel按钮 */
						{
							return;
							break;
						}
						default:
						{
							break;
						}
					}
				}
				
				iIndexPressed = -1;
			}
		}
		else
		{
			/* 按下状态 */
			if (iIndex != -1)
			{
				if (!bPressed && (iIndex != 1))
				{
					/* 未曾按下按钮 */
					bPressed = 1;
					iIndexPressed = iIndex;
					tInputEventPrePress = tInputEvent;  /* 记录下来 */
					PressButton(&g_atIntervalPageIconsLayout[iIndexPressed]);
				}

				/* 如果按下的是"inc.bmp"或"dec.bmp" 
				 * 连按2秒后, 飞快的递增或减小: 每50ms变化一次
				 */
				if ((iIndexPressed == 0) || (iIndexPressed == 2))
				{
					if (bFast && (TimeMSBetween(tInputEventPrePress.tTime, tInputEvent.tTime) > 50))
					{
						iIntervalSecond = iIndexPressed ? (iIntervalSecond - 1) : (iIntervalSecond + 1);
						if (iIntervalSecond == 60)
						{
							iIntervalSecond = 0;
						}
						else if (iIntervalSecond == -1)
						{
							iIntervalSecond = 59;
						}
						GenerateIntervalPageSpecialIcon(iIntervalSecond, ptDevVideoMem);
						tInputEventPrePress = tInputEvent;
					}
					
					if (TimeMSBetween(tInputEventPrePress.tTime, tInputEvent.tTime) > 2000)
					{
						bFast = 1;
						tInputEventPrePress = tInputEvent;
					}
					
				}
			}
		}
#endif

	}

}


int IntervalPageInit ( void )
{
	return RegisterPageAction ( &g_tIntervalPageDesc );

}

