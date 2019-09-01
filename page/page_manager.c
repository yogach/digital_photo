#include <page_manager.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <config.h>
#include <disp_manager.h>
#include <stdlib.h>

static PT_PageDesc g_tPageActionHead;



//��DispOpr�����ڵ�
int RegisterPageAction ( PT_PageDesc ptPageDesc)
{
	PT_PageDesc ptTmp;

	if ( !g_tPageActionHead ) //�����ͷ�ڵ�
	{
		g_tPageActionHead = ptPageDesc;
		ptPageDesc->ptNext = NULL;
	}
	else
	{
		ptTmp = g_tPageActionHead;

		while ( ptTmp->ptNext )
		{
			ptTmp = ptTmp->ptNext;
		}

		ptTmp->ptNext = ptPageDesc;
		ptPageDesc->ptNext = NULL;
	}

	return 0;
}


//����ShowDispOpr����
void ShowPages ( void )
{
	int i = 0;
	PT_PageDesc ptTmp = g_tPageActionHead;

	while ( ptTmp )
	{
		printf ( "%02d %s\n",i++,ptTmp->name );
		ptTmp = ptTmp->ptNext;
	}
}


/**********************************************************************
 * �������ƣ� Page
 * ���������� ��������ȡ��ָ����"ҳ��ģ��"
 * ��������� pcName - ����
 * ��������� ��
 * �� �� ֵ�� NULL   - ʧ��,û��ָ����ģ��, 
 *            ��NULL - "ҳ��ģ��"��PT_PageAction�ṹ��ָ��
 ***********************************************************************/

PT_PageDesc Page ( char* pcName )
{
	PT_PageDesc ptTmp = g_tPageActionHead;

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
 * �������ƣ� GenericGetInputEvent
 * ���������� ��ȡ��������,���ж���λ����һ��ͼ����
 * ��������� ptPageLayout - �ں����ͼ�����ʾ����
 * ��������� ptInputEvent - �ں��õ�����������
 * �� �� ֵ�� -1     - �������ݲ�λ���κ�һ��ͼ��֮��
 *            ����ֵ - �������������ڵ�ͼ��(ptLayout�������һ��)
**********************************************************************/
int GenericGetInputEvent ( PT_Layout atLayout,PT_InputEvent ptInputEvent )
{
	T_InputEvent tInputEvent;
	int iRet, i = 0;

	/*��ȡ������ԭʼ����*/
	iRet = GetDeviceInput ( &tInputEvent );
	if ( iRet )
	{

		return -1;
	}

	*ptInputEvent = tInputEvent;

	//������Ǵ������¼�           ����-1
	if ( tInputEvent.iType !=INPUT_TYPE_TOUCHSCREEN )
	{
		return -1;
	}

	while ( atLayout[i].IconName )
	{
		//������µĴ�����ĳ��ͼ���� ����ͼ���������ڵ�λ��
		if ( ( tInputEvent.iX >= atLayout[i].iTopLeftX ) && ( tInputEvent.iX <= atLayout[i].iLowerRightX ) &&\
		        ( tInputEvent.iY >= atLayout[i].iTopLeftY ) && ( tInputEvent.iY <= atLayout[i].iLowerRightY ) )
		{
			DBG_PRINTF ( "put\release status:%d , icon name:%s\r\n",tInputEvent.iPressure,atLayout[i].IconName );
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
 * �������ƣ� InvertButton
 * ���������� �ǰ���ʾ�豸��ָ��������ÿ�����ص���ɫȡ��
 * ��������� ptLayout   - ��������
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���          �޸�����
 * -----------------------------------------------
**********************************************************************/
static void InvertButton ( PT_Layout ptLayout )
{
	int iBpp,iY,i;
	int iButtonWidth;
	unsigned char* pucDispMem;
	PT_DispOpr ptDefaultDisOpr;

	//���Ĭ�������豸
	ptDefaultDisOpr = GetDefaultDispDev();
	if ( ptDefaultDisOpr == NULL )
	{
		DBG_PRINTF ( "can't get default display device..\r\n" );
	}

	iBpp = ptDefaultDisOpr->iBpp;
	pucDispMem = ptDefaultDisOpr->pucDispMem;
	//�õ�ͼ����fbmen�ڵ���ʼλ��
	pucDispMem += ptLayout->iTopLeftY * ptDefaultDisOpr->iLineWidth + ptLayout->iTopLeftX *  iBpp / 8;
	//��ȡͼ�������ռ�ֽ���
	iButtonWidth = ( ptLayout->iLowerRightX - ptLayout->iTopLeftX + 1 ) * iBpp / 8;


	//��ͼ�����Ͻ�Ϊ��ʼ ��ÿһ�ж����д���
	for ( iY= ptLayout->iTopLeftY ; iY <=ptLayout->iLowerRightY ; iY++ )
	{
		//��ͼ����ʼλ�ÿ�ʼ ��ÿһ���ֽڽ���ȡ������
		for ( i = 0 ; i< iButtonWidth ; i++ )
		{
			pucDispMem[i] = ~pucDispMem[i];
		}
		pucDispMem += ptDefaultDisOpr->iLineWidth;
	}
}

/**********************************************************************
 * �������ƣ� ReleaseButton
 * ���������� �ɿ�ͼ��,ֻ�Ǹı���ʾ�豸�ϵ�ͼ�갴ť��ɫ
 * ��������� ptLayout   - ͼ�����ھ�������
 * ��������� ��
 * �� �� ֵ�� ��
 * -----------------------------------------------
**********************************************************************/
void ReleaseButton ( PT_Layout ptLayout )
{
	InvertButton ( ptLayout );
}

/**********************************************************************
 * �������ƣ� PressButton
 * ���������� ����ͼ��,ֻ�Ǹı���ʾ�豸�ϵ�ͼ�갴ť��ɫ
 * ��������� ptLayout   - ͼ�����ھ�������
 * ��������� ��
 * �� �� ֵ�� ��
 ***********************************************************************/
void PressButton ( PT_Layout ptLayout )
{
	InvertButton ( ptLayout );
}

/**********************************************************************
 * �������ƣ� GeneratePage
 * ���������� ��ͼ���ļ��н�����ͼ�����ݲ�����ָ������,�Ӷ�����ҳ������
 * ��������� ptPageLayout - �ں����ͼ����ļ�������ʾ����
 *            ptVideoMem   - �����VideoMem�ﹹ��ҳ������
 * ��������� ��
 * �� �� ֵ�� 0      - �ɹ�
 *            ����ֵ - ʧ��
 ***********************************************************************/
int GeneratePage ( PT_Layout atLayout, PT_VideoMem pt_VideoMem )
{
	T_PhotoDesc tPhotoIconOriData;
	T_PhotoDesc tPhotoIconData;
	int iError , iBpp;
    iBpp = pt_VideoMem->tVideoMemDesc.iBpp;

	if ( pt_VideoMem->ePicState != PIC_GENERATED ) //���ͼƬδ׼����
	{

		//���ñ���ɫ
		ClearVideoMem ( pt_VideoMem,COLOR_BACKGROUND );

		while ( atLayout->IconName )
		{
			//��ȡͼ���ͼƬ����
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

			//��ԭʼͼƬ�������ŵ�ָ����С
			PicZoom ( &tPhotoIconOriData, &tPhotoIconData );
			//��ͼƬ�ϲ����Դ���
			PicMerge ( atLayout->iTopLeftX, atLayout->iTopLeftY, &tPhotoIconData, &pt_VideoMem->tVideoMemDesc );

			//�ͷ�ͼƬ������ڴ� ��ֹ�ڴ�й¶
			FreePixelDatasForIcon ( &tPhotoIconOriData );
			free ( tPhotoIconData.aucPhotoData );
			atLayout++; //ָ��+1 ָ��������һ��
		}

		pt_VideoMem->ePicState = PIC_GENERATED;
	}

	return 0;
}


//��ҳ����ʾ
int ShowPage ( PT_PageDesc ptPageDesc)
{
	PT_VideoMem pt_VideoTmp;
	int iError;

	/* 1. ����Դ� */
	pt_VideoTmp = GetVideoMem ( ID ( ptPageDesc->name ),VMS_FOR_CUR ); //��ȡ�Դ����ڵ�ǰҳ����ʾ
	if ( pt_VideoTmp == NULL )
	{
		DBG_PRINTF ( "GetVideoMem error!\r\n" );
		return -1 ;
	}

    /* 2. ����ͼ������ */
    if(ptPageDesc->atPageLayout->iTopLeftX == 0)
    {
      // CalcMainPageLayout(atLayout);
      ptPageDesc->CalcPageLayout(ptPageDesc->atPageLayout);//���ø�ģ��ļ���ͼ�����꺯��
	}

	/* 3. �軭���� */
	iError = GeneratePage(ptPageDesc->atPageLayout,pt_VideoTmp);

	/* 3. ˢ���豸��ȥ       */
	FlushVideoMemToDev ( pt_VideoTmp );

	/* 4. ���Դ��״̬����Ϊfree */
	PutVideoMem ( pt_VideoTmp );

	return 0;

}



int PagesInit ( void )
{
	int iError = 0;

	iError |= MainPageInit();

	return iError;

}


