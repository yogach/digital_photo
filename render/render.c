#include <config.h>
#include <pic_manager.h>
#include <display_manager.h>
#include <string.h>
#include <fonts_manager.h>
#include <encoding_manager.h>


/**********************************************************************
 * 函数名称： SetColorForPixelInVideoMem
 * 功能描述： 设置VideoMem中某个座标象素的颜色
 * 输入参数： iX,iY      - 象素座标
 *            ptVideoMem - 设置VideoMem中的象素
 *            dwColor    - 设置为这个颜色,颜色格式为0x00RRGGBB
 * 输出参数： 无
 * 返 回 值： 这个象素占据多少字节
 ***********************************************************************/
int SetColorForPixelInVideoMem ( int iX,int iY,PT_VideoMem ptVideoMem, unsigned int dwBackColor )
{
	unsigned char* pucFB;
	unsigned short* pwFB16bpp;
	unsigned int* pdwFB32bpp;
	unsigned short wColor16bpp; /* 565 */
	int iRed;
	int iGreen;
	int iBlue;

	//得到像素位置
	pucFB	   = ptVideoMem->tVideoMemDesc.aucPhotoData;
	pucFB     += iY * ptVideoMem->tVideoMemDesc.iLineBytes + iX * ptVideoMem->tVideoMemDesc.iBpp / 8;

	pwFB16bpp  = ( unsigned short* ) pucFB;
	pdwFB32bpp = ( unsigned int* ) pucFB;

	//根据不同的像素深度进行处理
	switch ( ptVideoMem->tVideoMemDesc.iBpp )
	{
		case 8:
		{
			*pucFB = ( unsigned char ) dwBackColor;
			return 1;
			break;
		}
		case 16:
		{
			iRed   = ( dwBackColor >> ( 16+3 ) ) & 0x1f;
			iGreen = ( dwBackColor >> ( 8+2 ) ) & 0x3f;
			iBlue  = ( dwBackColor >> 3 ) & 0x1f;
			wColor16bpp = ( iRed << 11 ) | ( iGreen << 5 ) | iBlue;

			*pwFB16bpp	= wColor16bpp;

			return 2;
			break;
		}
		case 32:
		{

			*pdwFB32bpp = dwBackColor;

			return 4;
			break;
		}
		default :
		{
			DBG_PRINTF ( "can't support %d bpp\n", ptVideoMem->tVideoMemDesc.iBpp );
			return -1;
		}
	}
	return -1;


}

/**********************************************************************
 * 函数名称： SetColorForAppointArea
 * 功能描述： 改变指定区域内的颜色
 *            参考: 03.freetype\02th_arm\06th_show_lines_center
 * 输入参数： iTopLeftX,iTopLeftY   - 矩形区域的左上角座标
 *            iBotRightX,iBotRightY - 矩形区域的右下角座标
 *            dwBackColor           - 背景色
 *            ptVideoMem            - VideoMem
 * 输出参数： 无
 * 返 回 值： 0 - 成功,  其他值 - 失败
 ***********************************************************************/
int SetColorForAppointArea ( int iTopLeftX, int iTopLeftY, int iBotRightX, int iBotRightY,  PT_VideoMem ptVideoMem, unsigned int dwBackColor )
{
	int x,y;

	for ( y=iTopLeftY; y<=iBotRightY; y++ )
		for ( x=iTopLeftX; x<=iBotRightX; x++ )
		{
			SetColorForPixelInVideoMem ( x,y,ptVideoMem,dwBackColor );
		}
}
/**********************************************************************
 * 函数名称： MergerStringToCenterOfRectangleInVideoMem
 * 功能描述： 在VideoMem的指定矩形居中显示字符串
 *            参考: 03.freetype\02th_arm\06th_show_lines_center
 * 输入参数： iTopLeftX,iTopLeftY   - 矩形区域的左上角座标
 *            iBotRightX,iBotRightY - 矩形区域的右下角座标
 *            pucTextString         - 要显示的字符串
 *            ptVideoMem            - VideoMem
 * 输出参数： 无
 * 返 回 值： 0 - 成功,  其他值 - 失败
 ***********************************************************************/
int MergerStringToCenterOfRectangleInVideoMem ( int iTopLeftX, int iTopLeftY, int iBotRightX, int iBotRightY, unsigned char* pucTextString, PT_VideoMem ptVideoMem )
{
	unsigned char* pucStrStart;
	unsigned char* pucStrEnd;
	int ilen,bHasGetCode = 0,iError;
	unsigned int dwCode;
	T_FontBitMap tFontBitMap;
	int iMinX = 32000,iMinY = 32000 ;
	int iMaxX = -1,iMaxY =-1;
	int iWidth, iHight;
	int iStartY,iStartX;

	/* 1. 设置这个区域的底色 */
	SetColorForAppointArea ( iTopLeftX,iTopLeftY,iBotRightX,iBotRightY,ptVideoMem,COLOR_BACKGROUND );

	/* 2.先计算字符串位图的总体宽度、高度 */
	pucStrStart = pucTextString;
	pucStrEnd = pucStrStart + strlen ( pucTextString );

	while ( 1 )
	{
		ilen = GetCodeFrmBuf ( pucStrStart,pucStrEnd,&dwCode );
		if ( ilen == 0 )
		{

			if ( bHasGetCode )
			{
				//进入此处代表字符串已结束
				DBG_PRINTF ( "string end ..\r\n" );
				break;
			}
			else
			{
				// 进入此处代表获取dwCode 失败
				DBG_PRINTF ( "GetCodeFrmBuf error..\r\n" );
				return -1;
			}

		}

		bHasGetCode = 1;
		pucStrStart +=ilen;

		//获得字符位图
		iError = GetFontBitmap ( dwCode,&tFontBitMap );
		if ( iError == 0 )
		{
			//得到整个字符串所占据的大小
			if ( iMinX > tFontBitMap.iXLeft )
			{
				iMinX = tFontBitMap.iXLeft;
			}
			if ( iMaxX < tFontBitMap.iXMax )
			{
				iMaxX = tFontBitMap.iXMax;
			}

			if ( iMinY > tFontBitMap.iYTop )
			{
				iMinY = tFontBitMap.iYTop;
			}
			if ( iMaxY < tFontBitMap.iYMax )
			{
				iMaxY = tFontBitMap.iYMax;
			}

			//得到下一个显示字符的位置
			tFontBitMap.iCurOriginX = tFontBitMap.iNextOriginX;
			tFontBitMap.iCurOriginY = tFontBitMap.iNextOriginY;

		}
		else
		{
			DBG_PRINTF ( "GetFontBitmap Error...\r\n" );
		}

	}

	DBG_PRINTF ( "iMinx = %d , iMax = %d, iMinY = %d , iMaxY = %d",iMinX,iMaxX,iMinY,iMaxY );
	/*3.判断要显示的字符串是否超过显示区域*/
	//得到字符串大小
	iWidth = iMaxX - iMinX;
	iHight = iMaxY - iMinY;

	if ( iWidth > ( iBotRightX -iTopLeftX ) )
	{
		iWidth = iBotRightX -iTopLeftX;
	}

	if ( iHight > iBotRightY -iTopLeftY )
	{
		DBG_PRINTF ( "string hight exceed the limit ..\r\n " );
		return -1;
	}

	/*4.确定第一个字符的原点坐标*/
	iStartX = iTopLeftX+ ( iBotRightX - iTopLeftX - iWidth ) /2; //区域起始地址 加除显示框之后剩下的一半 其实就是居中显示
	iStartY = iTopLeftY+ ( iBotRightY - iTopLeftY - iHight ) /2;

	/*	 
	 * 2.2 再计算第1个字符原点坐标
	 * iMinX - 原来的iCurOriginX(0) = iStrTopLeftX - 新的iCurOriginX
	 * iMinY - 原来的iCurOriginY(0) = iStrTopLeftY - 新的iCurOriginY
	 */
	tFontBitMap.iCurOriginX = iStartX -iMinX;
	tFontBitMap.iCurOriginY = iStartY -iMinY;

	DBG_PRINTF ( "iCurOriginX = %d, iCurOriginY = %d\n", tFontBitMap.iCurOriginX, tFontBitMap.iCurOriginY );

	bHasGetCode = 0;
	pucStrStart = pucTextString;

	while ( 1 )
	{
		ilen = GetCodeFrmBuf ( pucStrStart,pucStrEnd,&dwCode );
		if ( ilen == 0 )
		{

			if ( bHasGetCode )
			{
				//进入此处代表字符串已结束
				DBG_PRINTF ( "string end ..\r\n" );
				break;
			}
			else
			{
				// 进入此处代表获取dwCode 失败
				DBG_PRINTF ( "GetCodeFrmBuf error..\r\n" );
				return -1;
			}

		}

		bHasGetCode = 1;
		pucStrStart +=ilen;
		//获得字符位图
		iError = GetFontBitmap ( dwCode,&tFontBitMap );
		if ( iError == 0 )
		{
			/* 显示一个字符 */
			//判断是否当前矩形内还能否显示内容
			if ( isFontInArea ( iTopLeftX, iTopLeftY, iBotRightX, iBotRightY, &tFontBitMap ) )
			{
				if ( MergeOneFontToVideoMem ( &tFontBitMap, ptVideoMem ) ) //将位图合并和指定位置
				{
					return -1;
				}
			}
			else
			{
				return 0;
			}

			tFontBitMap.iCurOriginX = tFontBitMap.iNextOriginX;
			tFontBitMap.iCurOriginY = tFontBitMap.iNextOriginY;
		}
		else
		{
			DBG_PRINTF ( "GetFontBitmap Error...\r\n" );
		}

	}

	return 0;
}

int MergeOneFontToVideoMem ( PT_FontBitMap ptFontBitMap, PT_VideoMem ptVideoMem )
{
	int iBpp = ptFontBitMap->iBpp;
    int x,y,i;

    //根据不同的像素分类处理
	switch ( iBpp )
	{
		case 1:
		{

		

		}

		break;


		case 8:
		{

		}

		break;

		default:
			DBG_PRINTF ( "can't support %d bpp",iBpp );
			break;

	}




}
/**********************************************************************
 * 函数名称： isFontInArea
  * 功能描述： 要显示的字符是否完全在指定矩形区域内
  * 输入参数： iTopLeftX,iTopLeftY   - 矩形区域的左上角座标
  * 		   iBotRightX,iBotRightY - 矩形区域的右下角座标
  * 		   ptFontBitMap 		 - 内含字符的位图信息
  * 输出参数： 无
  * 返 回 值： 0 - 超出了矩形区域,  1 - 完全在区域内
  * 修改日期		版本号	  修改人 	   修改内容
 * -----------------------------------------------
**********************************************************************/
int isFontInArea ( int iTopLeftX, int iTopLeftY,int iBotRightX, int iBotRightY,PT_FontBitMap ptFontBitMap )
{
	if ( ( ptFontBitMap->iXLeft >= iTopLeftX ) && ( ptFontBitMap->iXMax <= iBotRightX ) \
	        && ( ptFontBitMap->iYTop >= iTopLeftY ) && ( ptFontBitMap->iYMax<= iBotRightY ) )
	{
		return 1;
	}
	else
	{

		return 0;
	}


}

/**********************************************************************
 * 函数名称： InvertButton
 * 功能描述： 是把显示设备上指定区域里每个象素的颜色取反
 * 输入参数： ptLayout   - 矩形区域
 * 输出参数： 无
 * 返 回 值： 无
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



