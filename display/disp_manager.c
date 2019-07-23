
#include <config.h>
#include <disp_manager.h>
#include <string.h>

static PT_DispOpr g_ptDispOprHead;
static PT_DispOpr g_ptDefaultDisp;

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
int SetDefaultDispDev ( char* name )
{
	g_ptDefaultDisp = GetDispOpr ( name );

	if ( g_ptDefaultDisp == NULL )
	{
		DBG_PRINTF ( "SetDefaultDisp error!\r\n" );
		return - 1;

	}


}

//获得默认显示设备
PT_DispOpr GetDefaultDispDev ( void )
{
	if ( g_ptDefaultDisp != NULL )
	{
		return PT_DispOpr;
	}

}

//获得默认显示设备的分辨率
int GetDispResolution ( int iXres,int iYres,int iBpp )
{
	if ( g_ptDefaultDisp != NULL )
	{
		iXres = g_ptDefaultDisp->iXres;
		iYres = g_ptDefaultDisp->i;
		iBpp = g_ptDefaultDisp->iXres;


	}
	else
	{
		return - 1;
	}

}




int AllocVideoMem ( int iNum )
{
	int iXres,iYres,iBpp;
	int iVMSize;
	int iLineBytes;
	int i;


	PT_VideoMem ptNew;

	//获得分辨率
	GetDispResolution ( iXres, iYres, iBpp );

	iLineBytes = iXres * iBpp/8; //得到LCD一行数据占据的字节数

	iVMSize = iYres * iLineBytes; //得到LCD整个页面占据的字节大小


	//将显示设备本身的MEN空间放入链表

	ptNew = malloc ( sizeof ( T_VideoMem ) ); //分配空间
	if ( ptNew == NULL )
	{
		return - 1;
	}


	//对结点内容进行初始化
	ptNew->bDevFrameBuffer = 1;
	ptNew->iID =0;
	ptNew->PicState = PIC_BLANK;
	ptNew->VideoMemState = VMS_FREE;
	ptNew->PhotoDesc->iBpp =iBpp;
	ptNew->PhotoDesc->iHigh =iYres;
	ptNew->PhotoDesc->iWidth =iXres;
	ptNew->PhotoDesc->iLineBytes =iLineBytes;
	ptNew->PhotoDesc->iTotalBytes =iVMSize;

	if ( g_ptDefaultDisp ==NULL )
	{
		DBG_PRINTF ( "g_ptDefaultDisp is NULL \r\n" );
		free ( ptNew );
		return -1;
	}
	ptNew->PhotoDesc->aucPhotoData =  g_ptDefaultDisp->pucDispMem; //


	//将此节点放入链表中 相当将新节点放在头位置上
	ptNew->ptnext = g_ptVideoMenListHead;
	g_ptVideoMenListHead = ptNew;


	if ( iNum ==0 )
	{
		ptNew->VideoMemState = VMS_FOR_CUR;

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
			ptNew->PicState = PIC_BLANK;
			ptNew->VideoMemState = VMS_FREE;
			ptNew->PhotoDesc->iBpp =iBpp;
			ptNew->PhotoDesc->iHigh =iYres;
			ptNew->PhotoDesc->iWidth =iXres;
			ptNew->PhotoDesc->iLineBytes =iLineBytes;
			ptNew->PhotoDesc->iTotalBytes =iVMSize;
			ptNew->PhotoDesc->aucPhotoData =  ( unsigned char* ) ( ptNew+1 ); //

			//将此节点放入链表中 相当将新节点放在头位置上
			ptNew->ptnext = g_ptVideoMenListHead;
			g_ptVideoMenListHead = ptNew;



		}

	}

}


int DisplayInit ( void )
{
	int iError;

	iError = FBInit ();

	return iError;
}


