
#include <config.h>
#include <disp_manager.h>
#include <string.h>

static PT_DispOpr g_ptDispOprHead;
static PT_DispOpr g_ptDefaultDisp;

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
int SetDefaultDispDev ( char* name )
{
	g_ptDefaultDisp = GetDispOpr ( name );

	if ( g_ptDefaultDisp == NULL )
	{
		DBG_PRINTF ( "SetDefaultDisp error!\r\n" );
		return - 1;

	}


}

//���Ĭ����ʾ�豸
PT_DispOpr GetDefaultDispDev ( void )
{
	if ( g_ptDefaultDisp != NULL )
	{
		return PT_DispOpr;
	}

}

//���Ĭ����ʾ�豸�ķֱ���
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

	//��÷ֱ���
	GetDispResolution ( iXres, iYres, iBpp );

	iLineBytes = iXres * iBpp/8; //�õ�LCDһ������ռ�ݵ��ֽ���

	iVMSize = iYres * iLineBytes; //�õ�LCD����ҳ��ռ�ݵ��ֽڴ�С


	//����ʾ�豸�����MEN�ռ��������

	ptNew = malloc ( sizeof ( T_VideoMem ) ); //����ռ�
	if ( ptNew == NULL )
	{
		return - 1;
	}


	//�Խ�����ݽ��г�ʼ��
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


	//���˽ڵ���������� �൱���½ڵ����ͷλ����
	ptNew->ptnext = g_ptVideoMenListHead;
	g_ptVideoMenListHead = ptNew;


	if ( iNum ==0 )
	{
		ptNew->VideoMemState = VMS_FOR_CUR;

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
			ptNew->PicState = PIC_BLANK;
			ptNew->VideoMemState = VMS_FREE;
			ptNew->PhotoDesc->iBpp =iBpp;
			ptNew->PhotoDesc->iHigh =iYres;
			ptNew->PhotoDesc->iWidth =iXres;
			ptNew->PhotoDesc->iLineBytes =iLineBytes;
			ptNew->PhotoDesc->iTotalBytes =iVMSize;
			ptNew->PhotoDesc->aucPhotoData =  ( unsigned char* ) ( ptNew+1 ); //

			//���˽ڵ���������� �൱���½ڵ����ͷλ����
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


