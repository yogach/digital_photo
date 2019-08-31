#include <page_manager.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <config.h>
#include <disp_manager.h>
#include <stdlib.h>

static PT_PageAction g_tPageActionHead;



//向DispOpr新增节点
int RegisterPageAction ( PT_PageAction ptPageAction )
{
	PT_PageAction ptTmp;

	if ( !g_tPageActionHead ) //如果是头节点
	{
		g_tPageActionHead = ptPageAction;
		ptPageAction->ptNext = NULL;
	}
	else
	{
		ptTmp = g_tPageActionHead;

		while ( ptTmp->ptNext )
		{
			ptTmp = ptTmp->ptNext;
		}

		ptTmp->ptNext = ptPageAction;
		ptPageAction->ptNext = NULL;
	}

	return 0;
}


//遍历ShowDispOpr链表
void ShowPages ( void )
{
	int i = 0;
	PT_PageAction ptTmp = g_tPageActionHead;

	while ( ptTmp )
	{
		printf ( "%02d %s\n",i++,ptTmp->name );
		ptTmp = ptTmp->ptNext;
	}
}


//根据名字得到对应的节点
PT_PageAction Page ( char* pcName )
{
	PT_PageAction ptTmp = g_tPageActionHead;

	while ( ptTmp )
	{
		if ( strcmp ( ptTmp->name,pcName ) == 0 )
		{
			return ptTmp;
		}

		ptTmp = ptTmp->ptNext;
	}

	return NULL;
}

int ID ( char* str )
{
	return ( int ) ( str[0] ) + ( int ) ( str[1] ) + ( int ) ( str[2] ) + ( int ) ( str[3] ) ;
}


/**********************************************************************
 * 函数名称： GenericGetInputEvent
 * 功能描述： 读取输入数据,并判断它位于哪一个图标上
 * 输入参数： ptPageLayout - 内含多个图标的显示区域
 * 输出参数： ptInputEvent - 内含得到的输入数据
 * 返 回 值： -1     - 输入数据不位于任何一个图标之上
 *            其他值 - 输入数据所落在的图标(ptLayout数组的哪一项)
**********************************************************************/
int GenericGetInputEvent ( PT_Layout ptLayout,PT_InputEvent ptInputEvent )
{
	T_InputEvent tInputEvent;
	int iRet, i = 0;



	/*获取触摸屏原始数据*/
	iRet = GetDeviceInput ( &tInputEvent );
	if ( iRet )
	{

		return -1;
	}

	*ptInputEvent = tInputEvent;

	//如果不是触摸屏事件           返回-1
	if ( tInputEvent.iType !=INPUT_TYPE_TOUCHSCREEN )
	{
		return -1;
	}

	while ( ptLayout[i].IconName )
	{
		//如果按下的触点在某个图标内 返回图标在数组内的位置
		if ( ( tInputEvent.iX >= ptLayout[i].iTopLeftX ) && ( tInputEvent.iX <= ptLayout[i].iLowerRightX ) &&\
		        ( tInputEvent.iY >= ptLayout[i].iTopLeftY ) && ( tInputEvent.iY <= ptLayout[i].iLowerRightY ) )
		{
			DBG_PRINTF ( "put\release status:%d , icon name:%s\r\n",tInputEvent.iPressure,ptLayout[i].IconName );
			return i;
		}
		else
		{
			i ++;
		}
	}

	DBG_PRINTF ( "don't touch icon\r\n" );
	return -1;

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

/**********************************************************************
 * 函数名称： GeneratePage
 * 功能描述： 从图标文件中解析出图像数据并放在指定区域,从而生成页面数据
 * 输入参数： ptPageLayout - 内含多个图标的文件名和显示区域
 *            ptVideoMem   - 在这个VideoMem里构造页面数据
 * 输出参数： 无
 * 返 回 值： 0      - 成功
 *            其他值 - 失败
 ***********************************************************************/
int GeneratePage ( PT_Layout atLayout, PT_VideoMem pt_VideoMem )
{
	T_PhotoDesc tPhotoIconOriData;
	T_PhotoDesc tPhotoIconData;
	int iError , iBpp;
    iBpp = pt_VideoMem->tVideoMemDesc.iBpp;

	if ( pt_VideoMem->ePicState != PIC_GENERATED ) //如果图片未准备好
	{

		//设置背景色
		ClearVideoMem ( pt_VideoMem,COLOR_BACKGROUND );

		while ( atLayout->IconName )
		{
			//获取图标的图片数据
			iError = GetPixelDatasFormIcon ( atLayout->IconName,&tPhotoIconOriData );
			if ( iError !=0 )
			{
				return -1;
			}

			tPhotoIconData.iBpp    = iBpp;
            tPhotoIconData.iHigh   = atLayout->iLowerRightY - atLayout->iTopLeftY + 1 ;
            tPhotoIconData.iWidth  = atLayout->iLowerRightX - atLayout->iTopLeftX + 1 ;
			tPhotoIconData.iLineBytes = tPhotoIconData.iWidth * tPhotoIconData.iBpp / 8;
			tPhotoIconData.iTotalBytes = tPhotoIconData.iLineBytes * tPhotoIconData.iHigh;
			tPhotoIconData.aucPhotoData = malloc(tPhotoIconData.iTotalBytes);
            if(tPhotoIconData.aucPhotoData == NULL )
            {
              DBG_PRINTF("malloc fail..\r\n");
			  FreePixelDatasForIcon ( &tPhotoIconOriData );
			  return -1;
			}

			//将原始图片数据缩放到指定大小
			PicZoom ( &tPhotoIconOriData, &tPhotoIconData );
			//将图片合并到显存中
			PicMerge ( atLayout->iTopLeftX, atLayout->iTopLeftY, &tPhotoIconData, &pt_VideoMem->tVideoMemDesc );

			//释放图片分配的内存 防止内存泄露
			FreePixelDatasForIcon ( &tPhotoIconOriData );
			free ( tPhotoIconData.aucPhotoData );
			atLayout++; //指针+1 增加的长度与指针的数据类型有关
		}

		pt_VideoMem->ePicState = PIC_GENERATED;
	}

	return 0;
}


int PagesInit ( void )
{
	int iError = 0;

	iError |= MainPageInit();

	return iError;

}


