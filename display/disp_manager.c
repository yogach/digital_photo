
#include <config.h>
#include <disp_manager.h>
#include <string.h>
#include <stdlib.h>


static PT_DispOpr g_ptDispOprHead ;
static PT_DispOpr g_ptDefaultDisp ;

static PT_VideoMem g_ptVideoMenListHead;


//向DispOpr新增节点
int RegisterDispOpr ( PT_DispOpr ptDispOpr )
{
	PT_DispOpr ptTmp;

	if ( !g_ptDispOprHead ) //如果是头节点
	{
		g_ptDispOprHead = ptDispOpr;
		ptDispOpr->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptDispOprHead;

		while ( ptTmp->ptNext )
		{
			ptTmp = ptTmp->ptNext;
		}

		ptTmp->ptNext = ptDispOpr;
		ptDispOpr->ptNext = NULL;
	}

	return 0;
}


//遍历ShowDispOpr链表
void ShowDispOpr ( void )
{
	int i = 0;
	PT_DispOpr ptTmp = g_ptDispOprHead;

	while ( ptTmp )
	{
		printf ( "%02d %s\n",i++,ptTmp->name );
		ptTmp = ptTmp->ptNext;
	}
}


//根据名字得到对应的节点
PT_DispOpr GetDispOpr ( char* pcName )
{
	PT_DispOpr ptTmp = g_ptDispOprHead;

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


//设置默认设备
int SelectAndInitDefaultDispDev ( char* name )
{
	g_ptDefaultDisp = GetDispOpr ( name );

	if ( g_ptDefaultDisp == NULL )
	{
		DBG_PRINTF ( "SetDefaultDisp error!\r\n" );
		return - 1;

	}
	g_ptDefaultDisp->DeviceInit();
	g_ptDefaultDisp->CleanScreen ( 0 ); //使用黑色填充lcd

	return 0;

}

//获得默认显示设备
PT_DispOpr GetDefaultDispDev ( void )
{
	if ( g_ptDefaultDisp != NULL )
	{
		return g_ptDefaultDisp;
	}

	return NULL;

}

/**********************************************************************
 * 函数名称： GetDispResolution
 * 功能描述： 获得所使用的显示设备的分辨率和BPP
 * 输入参数： 无
 * 输出参数： iXres - 存X分辨率
 *            iYres - 存X分辨率
 *            iBpp  - 存BPP
 * 返 回 值： 0  - 成功
 *            -1 - 失败(未使用SelectAndInitDefaultDispDev来选择显示模块)
 ***********************************************************************/
int GetDispResolution ( int* iXres,int* iYres,int* iBpp )
{
	if ( g_ptDefaultDisp != NULL )
	{
		*iXres = g_ptDefaultDisp->iXres;
		*iYres = g_ptDefaultDisp->iYres;
		*iBpp = g_ptDefaultDisp->iBpp;
		return 0;
	}
	else
	{
		return - 1;
	}

}
/**********************************************************************
 * 函数名称： GetDevVideoMem
 * 功能描述： 获得显示设备的显存, 在该显存上操作就可以直接在LCD上显示出来
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 显存对应的VideoMem结构体指针 NULL-未找到
 ***********************************************************************/
PT_VideoMem GetDevVideoMen ( void )
{
	PT_VideoMem ptTmp = g_ptVideoMenListHead;

	while ( ptTmp )
	{
		if ( ptTmp->bDevFrameBuffer )
		{
			return ptTmp;
		}

		ptTmp = ptTmp->ptNext;

	}
	return 0;
}


/**********************************************************************
 * 函数名称： AllocVideoMem
 * 功能描述： 分配几个设备显存相同的空间--用于存储要显示的数据
 * 输入参数： iNum - 需要分配的显存数量
 * 输出参数： 无
 * 返 回 值： 0-成功 其他值-失败
 ***********************************************************************/
int AllocVideoMem ( int iNum )
{
	int iXres,iYres,iBpp;
	int iVMSize;
	int iLineBytes;
	int i;

	PT_VideoMem ptNew;

	//获得选择的分辨率
	GetDispResolution ( &iXres, &iYres, &iBpp );

	iLineBytes = iXres * iBpp/8; //得到LCD一行数据占据的字节数

	iVMSize = iYres * iLineBytes; //得到LCD整个页面占据的字节大小


	//将显示设备本身的MEN空间放入链表
	ptNew = malloc ( sizeof ( T_VideoMem ) ); //分配空间
	if ( ptNew == NULL )
	{
		return - 1;
	}


	//对节点内容进行初始化
	ptNew->bDevFrameBuffer = 1; //等于1 表示此块空间是显示设备显存
	ptNew->iID =0;
	ptNew->ePicState = PIC_BLANK;
	ptNew->eVideoMemState = VMS_FREE;
	ptNew->tVideoMemDesc.iBpp =iBpp;
	ptNew->tVideoMemDesc.iHigh =iYres;
	ptNew->tVideoMemDesc.iWidth =iXres;
	ptNew->tVideoMemDesc.iLineBytes =iLineBytes;
	ptNew->tVideoMemDesc.iTotalBytes =iVMSize;

	if ( g_ptDefaultDisp ==NULL )
	{
		DBG_PRINTF ( "g_ptDefaultDisp is NULL \r\n" );
		free ( ptNew );
		return -1;
	}
	ptNew->tVideoMemDesc.aucPhotoData =  g_ptDefaultDisp->pucDispMem; //


	//将此节点放入链表中 相当每次都将新节点放在头位置上
	ptNew->ptNext = g_ptVideoMenListHead;
	g_ptVideoMenListHead = ptNew;


	if ( iNum ==0 )
	{
		ptNew->eVideoMemState = VMS_FOR_CUR;

	}
	else
	{

		//根据num生成多个显存空间
		for ( i=0; i<iNum; i++ )
		{
			ptNew = malloc ( sizeof ( T_VideoMem ) + iVMSize );
			if ( ptNew == NULL )
			{

				return -1;
			}

			ptNew->bDevFrameBuffer = 0;
			ptNew->iID =0;
			ptNew->ePicState = PIC_BLANK;
			ptNew->eVideoMemState = VMS_FREE;
			ptNew->tVideoMemDesc.iBpp =iBpp;
			ptNew->tVideoMemDesc.iHigh =iYres;
			ptNew->tVideoMemDesc.iWidth =iXres;
			ptNew->tVideoMemDesc.iLineBytes =iLineBytes;
			ptNew->tVideoMemDesc.iTotalBytes =iVMSize;
			ptNew->tVideoMemDesc.aucPhotoData =  ( unsigned char* ) ( ptNew+1 ); //指针加一增加的长度与指针的数据类型有关

			//将此节点放入链表中 相当将新节点放在头位置上
			ptNew->ptNext = g_ptVideoMenListHead;
			g_ptVideoMenListHead = ptNew;



		}

	}

	return 0;
}

/**********************************************************************
 * 函数名称： GetVideoMem
 * 功能描述： 获得一块可操作的VideoMem(它用于存储要显示的数据), 
 *            用完后用PutVideoMem来释放
 * 输入参数： iID  - ID值,先尝试在众多VideoMem中找到ID值相同的
 *            bUseForCur - 1表示当前程序马上要使用VideoMem,无论如何都要返回一个VideoMem
 *                         0表示这是为了改进性能而提前取得VideoMem,不是必需的
 * 输出参数： 无
 * 返 回 值： NULL   - 失败,没有可用的VideoMem
 *            非NULL - 成功,返回PT_VideoMem结构体
 ***********************************************************************/
PT_VideoMem GetVideoMem ( int iID, int bUseForCur )
{
	PT_VideoMem ptTmp;
	ptTmp = g_ptVideoMenListHead;

	/* 1. 优先: 取出空闲的、ID相同的videomem */
	while ( ptTmp )
	{
		if ( ( ptTmp->iID == iID ) && ( ptTmp->eVideoMemState ==VMS_FREE ) )
		{
			ptTmp->eVideoMemState = ( bUseForCur ) ? VMS_FOR_CUR : VMS_FOR_PREPARE;
			return ptTmp;
		}
		ptTmp=ptTmp->ptNext;
	}

	/* 2. 如果没有ID相同的显存 则取出一个没有图片数据的videomem */
	ptTmp = g_ptVideoMenListHead;
	while ( ptTmp )
	{
		if ( ( ptTmp->eVideoMemState ==VMS_FREE ) && ( ptTmp->ePicState == PIC_BLANK ) )
		{
			ptTmp->iID = iID;
			ptTmp->eVideoMemState = ( bUseForCur ) ? VMS_FOR_CUR : VMS_FOR_PREPARE;
			return ptTmp;
		}

		ptTmp=ptTmp->ptNext;
	}

	/* 3. 如果没有ID相同的显存 也没有图片数据为空的videomem 则取出任意一个空闲videomem */
	ptTmp = g_ptVideoMenListHead;
	while ( ptTmp )
	{
		if ( ptTmp->eVideoMemState ==VMS_FREE )
		{
			ptTmp->iID = iID;
			ptTmp->ePicState = PIC_BLANK;
			ptTmp->eVideoMemState = ( bUseForCur ) ? VMS_FOR_CUR : VMS_FOR_PREPARE;
			return ptTmp;
		}

		ptTmp=ptTmp->ptNext;
	}

	/* 4. 如果什么都没有        则随意取出一个 */
	if ( bUseForCur )
	{
		ptTmp = g_ptVideoMenListHead;
		ptTmp->iID = iID;
		ptTmp->ePicState = PIC_BLANK;
		ptTmp->eVideoMemState = ( bUseForCur ) ? VMS_FOR_CUR : VMS_FOR_PREPARE;
		return ptTmp;
	}

	return NULL;

}

//释放显示
int PutVideoMem ( PT_VideoMem ptVideoMem )
{

	ptVideoMem->eVideoMemState = VMS_FREE;
	return 0;

}

void FlushVideoMemToDev ( PT_VideoMem ptVideoMem )
{
	if ( !ptVideoMem->bDevFrameBuffer ) //不等于1代表不是显存设备的内存
	{
		if ( g_ptDefaultDisp!=NULL )
		{
			g_ptDefaultDisp->ShowPage ( ptVideoMem );
		}
		else
		{
			DBG_PRINTF ( "not set DefaultDisp..\r\n" );
		}

	}

}

/**********************************************************************
 * 函数名称： ClearVideoMem
 * 功能描述： 把VideoMem中内存全部清为某种颜色
 * 输入参数： ptVideoMem   - VideoMem结构体指针, 内含要操作的内存
 *            dwBackColor  - 背景色
 * 输出参数： 无
 * 返 回 值： 0-成功 其他值-失败
 ***********************************************************************/
int ClearVideoMem ( PT_VideoMem ptVideoMem,unsigned int dwBackColor )
{
	unsigned char* pucFB;
	unsigned short* pwFB16bpp;
	unsigned int* pdwFB32bpp;
	unsigned short wColor16bpp; /* 565 */
	int iRed;
	int iGreen;
	int iBlue;
	int i = 0;
	int iScreenSize;

	pucFB      = ptVideoMem->tVideoMemDesc.aucPhotoData;
	pwFB16bpp  = ( unsigned short* ) pucFB;
	pdwFB32bpp = ( unsigned int* ) pucFB;
	iScreenSize = ptVideoMem->tVideoMemDesc.iTotalBytes;

	//根据不同的像素进行处理
	switch ( ptVideoMem->tVideoMemDesc.iBpp )
	{
		case 8:
		{
			memset ( pucFB, dwBackColor, iScreenSize );
			break;
		}
		case 16:
		{
			iRed   = ( dwBackColor >> ( 16+3 ) ) & 0x1f;
			iGreen = ( dwBackColor >> ( 8+2 ) ) & 0x3f;
			iBlue  = ( dwBackColor >> 3 ) & 0x1f;
			wColor16bpp = ( iRed << 11 ) | ( iGreen << 5 ) | iBlue;
			while ( i < iScreenSize )
			{
				*pwFB16bpp	= wColor16bpp;
				pwFB16bpp++;
				i += 2;
			}
			break;
		}
		case 32:
		{
			while ( i < iScreenSize )
			{
				*pdwFB32bpp	= dwBackColor;
				pdwFB32bpp++;
				i += 4;
			}
			break;
		}
		default :
		{
			DBG_PRINTF ( "can't support %d bpp\n", ptVideoMem->tVideoMemDesc.iBpp );
			return -1;
		}
	}

	return 0;
}



int DisplayInit ( void )
{
	int iError;

	iError = FBInit ();

	return iError;
}


