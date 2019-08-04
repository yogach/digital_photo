
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

//获得默认显示设备的分辨率
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

//分配显存
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


	//对结点内容进行初始化
	ptNew->bDevFrameBuffer = 1; //等于1表示此块空间是显示设备显存
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


	//将此节点放入链表中 相当将新节点放在头位置上
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

//获取指定的显存：如果指定id在使用中则挑选一个空闲的内存块
PT_VideoMem GetVideoMem ( int iID, int bUseForCur )
{
	PT_VideoMem ptTmp;
	ptTmp = g_ptVideoMenListHead;

	/* 1. 优先: 取出空闲的、ID相同的videomem */

	while ( ptTmp )
	{
		if ( ( ptTmp->iID == iID ) && ( ptTmp->eVideoMemState ==VMS_FREE ))
	    {
		    ptTmp->eVideoMemState = ( bUseForCur )? VMS_FOR_CUR : VMS_FOR_PREPARE;
			return ptTmp;
		}
		ptTmp=ptTmp->ptNext;

	}

	ptTmp = g_ptVideoMenListHead;

	/* 2. 如果没有则取出任意一个空闲videomem */
	while ( ptTmp )
	{
		if ( ptTmp->eVideoMemState ==VMS_FREE )
		{
			ptTmp->eVideoMemState = ( bUseForCur ) ? VMS_FOR_CUR : VMS_FOR_PREPARE;
			return ptTmp;
		}

		ptTmp=ptTmp->ptNext;


	}

	return NULL;

}

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





int DisplayInit ( void )
{
	int iError;

	iError = FBInit ();

	return iError;
}


