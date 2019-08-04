
#include <config.h>
#include <disp_manager.h>
#include <string.h>
#include <stdlib.h>


static PT_DispOpr g_ptDispOprHead ;
static PT_DispOpr g_ptDefaultDisp ;

static PT_VideoMem g_ptVideoMenListHead;


//��DispOpr�����ڵ�
int RegisterDispOpr ( PT_DispOpr ptDispOpr )
{
	PT_DispOpr ptTmp;

	if ( !g_ptDispOprHead ) //�����ͷ�ڵ�
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


//����ShowDispOpr����
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


//�������ֵõ���Ӧ�Ľڵ�
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


//����Ĭ���豸
int SelectAndInitDefaultDispDev ( char* name )
{
	g_ptDefaultDisp = GetDispOpr ( name );

	if ( g_ptDefaultDisp == NULL )
	{
		DBG_PRINTF ( "SetDefaultDisp error!\r\n" );
		return - 1;

	}
	g_ptDefaultDisp->DeviceInit();
	g_ptDefaultDisp->CleanScreen ( 0 ); //ʹ�ú�ɫ���lcd

    return 0;

}

//���Ĭ����ʾ�豸
PT_DispOpr GetDefaultDispDev ( void )
{
	if ( g_ptDefaultDisp != NULL )
	{
		return g_ptDefaultDisp;
	}

   return NULL;

}

//���Ĭ����ʾ�豸�ķֱ���
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

//�����Դ�
int AllocVideoMem ( int iNum )
{
	int iXres,iYres,iBpp;
	int iVMSize;
	int iLineBytes;
	int i;

	PT_VideoMem ptNew;

	//���ѡ��ķֱ���
	GetDispResolution ( &iXres, &iYres, &iBpp );

	iLineBytes = iXres * iBpp/8; //�õ�LCDһ������ռ�ݵ��ֽ���

	iVMSize = iYres * iLineBytes; //�õ�LCD����ҳ��ռ�ݵ��ֽڴ�С


	//����ʾ�豸�����MEN�ռ��������

	ptNew = malloc ( sizeof ( T_VideoMem ) ); //����ռ�
	if ( ptNew == NULL )
	{
		return - 1;
	}


	//�Խ�����ݽ��г�ʼ��
	ptNew->bDevFrameBuffer = 1; //����1��ʾ�˿�ռ�����ʾ�豸�Դ�
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


	//���˽ڵ���������� �൱���½ڵ����ͷλ����
	ptNew->ptNext = g_ptVideoMenListHead;
	g_ptVideoMenListHead = ptNew;


	if ( iNum ==0 )
	{
		ptNew->eVideoMemState = VMS_FOR_CUR;

	}
	else
	{

		//����num���ɶ���Դ�ռ�
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
			ptNew->tVideoMemDesc.aucPhotoData =  ( unsigned char* ) ( ptNew+1 ); //ָ���һ���ӵĳ�����ָ������������й�

			//���˽ڵ���������� �൱���½ڵ����ͷλ����
			ptNew->ptNext = g_ptVideoMenListHead;
			g_ptVideoMenListHead = ptNew;



		}

	}

   return 0;
}

//��ȡָ�����Դ棺���ָ��id��ʹ��������ѡһ�����е��ڴ��
PT_VideoMem GetVideoMem ( int iID, int bUseForCur )
{
	PT_VideoMem ptTmp;
	ptTmp = g_ptVideoMenListHead;

	/* 1. ����: ȡ�����еġ�ID��ͬ��videomem */

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

	/* 2. ���û����ȡ������һ������videomem */
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
	if ( !ptVideoMem->bDevFrameBuffer ) //������1�������Դ��豸���ڴ�
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


