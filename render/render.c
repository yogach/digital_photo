#include <config.h>
#include <pic_manager.h>
#include <display_manager.h>



int MergerStringToCenterOfRectangleInVideoMem(int iTopLeftX, int iTopLeftY, int iBotRightX, int iBotRightY, unsigned char *pucTextString, PT_VideoMem ptVideoMem)
{






}


/**********************************************************************
 * 函数名称： InvertButton
 * 功能描述： 是把显示设备上指定区域里每个象素的颜色取反
 * 输入参数： ptLayout   - 矩形区域
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人          修改内容
 * -----------------------------------------------
**********************************************************************/
static void InvertButton ( PT_Layout ptLayout )
{
	int iBpp,iY,i;
	int iButtonWidth;
	unsigned char* pucDispMem;
	PT_DispOpr ptDefaultDisOpr;

	//获得默认输入设备
	ptDefaultDisOpr = GetDefaultDispDev();
	if ( ptDefaultDisOpr == NULL )
	{
		DBG_PRINTF ( "can't get default display device..\r\n" );
	}

	iBpp = ptDefaultDisOpr->iBpp;
	pucDispMem = ptDefaultDisOpr->pucDispMem;
	//得到图标在fbmen内的起始位置
	pucDispMem += ptLayout->iTopLeftY * ptDefaultDisOpr->iLineWidth + ptLayout->iTopLeftX *  iBpp / 8;
	//获取图标横向所占字节数
	iButtonWidth = ( ptLayout->iLowerRightX - ptLayout->iTopLeftX + 1 ) * iBpp / 8;


	//以图标左上角为起始 对每一行都进行处理
	for ( iY= ptLayout->iTopLeftY ; iY <=ptLayout->iLowerRightY ; iY++ )
	{
		//从图标起始位置开始 对每一个字节进行取反操作
		for ( i = 0 ; i< iButtonWidth ; i++ )
		{
			pucDispMem[i] = ~pucDispMem[i];
		}
		pucDispMem += ptDefaultDisOpr->iLineWidth;
	}
}

/**********************************************************************
 * 函数名称： ReleaseButton
 * 功能描述： 松开图标,只是改变显示设备上的图标按钮颜色
 * 输入参数： ptLayout   - 图标所在矩形区域
 * 输出参数： 无
 * 返 回 值： 无
 * -----------------------------------------------
**********************************************************************/
void ReleaseButton ( PT_Layout ptLayout )
{
	InvertButton ( ptLayout );
}

/**********************************************************************
 * 函数名称： PressButton
 * 功能描述： 按下图标,只是改变显示设备上的图标按钮颜色
 * 输入参数： ptLayout   - 图标所在矩形区域
 * 输出参数： 无
 * 返 回 值： 无
 ***********************************************************************/
void PressButton ( PT_Layout ptLayout )
{
	InvertButton ( ptLayout );
}



