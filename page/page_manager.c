#include <page_manager.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <config.h>
#include <disp_manager.h>
#include <stdlib.h>
#include <sys/time.h>
#include <render.h>
static PT_PageDesc g_tPageActionHead;



//��DispOpr�����ڵ�
int RegisterPageAction ( PT_PageDesc ptPageDesc )
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

/**********************************************************************
 * �������ƣ� ID
 * ���������� �����������һ��Ψһ������,��������ʶVideoMem�е���ʾ����
 * ��������� strName - ����
 * ��������� ��
 * �� �� ֵ�� һ��Ψһ������
 ***********************************************************************/
int ID ( char* str )
{
	return ( int ) ( str[0] ) + ( int ) ( str[1] ) + ( int ) ( str[2] ) + ( int ) ( str[3] ) ;
}


/**********************************************************************
 * �������ƣ� GenericGetInputPositionInPageLayout
 * ���������� �жϰ��µ�λ�����ĸ�ͼ����
 * ��������� ptPageLayout - �ں����ͼ�����ʾ����
 * ��������� ptInputEvent - �ں��õ�����������
 * �� �� ֵ�� -1     - �������ݲ�λ���κ�һ��ͼ��֮��
 *            ����ֵ - �������������ڵ�ͼ��(ptLayout�������һ��)
**********************************************************************/
int GenericGetInputPositionInPageLayout ( PT_Layout atLayout,PT_InputEvent ptInputEvent )
{
	int i = 0;

	while ( atLayout[i].IconName )
	{
		//������µĴ�����ĳ��ͼ���� ����ͼ���������ڵ�λ��
		if ( ( ptInputEvent->iX >= atLayout[i].iTopLeftX ) && ( ptInputEvent->iX <= atLayout[i].iLowerRightX ) &&\
		        ( ptInputEvent->iY >= atLayout[i].iTopLeftY ) && ( ptInputEvent->iY <= atLayout[i].iLowerRightY ) )
		{
			//DBG_PRINTF ( "put\release status:%d , icon name:%s\r\n",tInputEvent.iPressure,atLayout[i].IconName );
			return i;
		}
		else
		{
			i ++;
		}
	}

	return -1;

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
	int iRet;

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
	/*
		while ( atLayout[i].IconName )
		{
			//������µĴ�����ĳ��ͼ���� ����ͼ���������ڵ�λ��
			if ( ( tInputEvent.iX >= atLayout[i].iTopLeftX ) && ( tInputEvent.iX <= atLayout[i].iLowerRightX ) &&\
			        ( tInputEvent.iY >= atLayout[i].iTopLeftY ) && ( tInputEvent.iY <= atLayout[i].iLowerRightY ) )
			{
				//DBG_PRINTF ( "put\release status:%d , icon name:%s\r\n",tInputEvent.iPressure,atLayout[i].IconName );
				return i;
			}
			else
			{
				i ++;
			}
		}

		//DBG_PRINTF ( "don't touch icon\r\n" );
		return -1;
		*/
	return GenericGetInputPositionInPageLayout ( atLayout,&tInputEvent );

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
	int iError, iBpp;
	iBpp = pt_VideoMem->tVideoMemDesc.iBpp;

	DBG_PRINTF ( "PicState :%d,VideoMem id:%d\r\n",pt_VideoMem->ePicState, pt_VideoMem->iID );
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
			    DBG_PRINTF("GetPixelDatasFormIcon error...\r\n");
				return -1;
			}

			tPhotoIconData.iBpp    = iBpp;
			tPhotoIconData.iHigh   = atLayout->iLowerRightY - atLayout->iTopLeftY + 1 ;
			tPhotoIconData.iWidth  = atLayout->iLowerRightX - atLayout->iTopLeftX + 1 ;
			tPhotoIconData.iLineBytes = tPhotoIconData.iWidth * tPhotoIconData.iBpp / 8;
			tPhotoIconData.iTotalBytes = tPhotoIconData.iLineBytes * tPhotoIconData.iHigh;
			tPhotoIconData.aucPhotoData = malloc ( tPhotoIconData.iTotalBytes );
			if ( tPhotoIconData.aucPhotoData == NULL )
			{
				DBG_PRINTF ( "malloc fail..\r\n" );
				FreePixelDatasForIcon ( &tPhotoIconOriData );
				return -1;
			}

			//��ԭʼͼƬ�������� ��������һ���ṹ����
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


/**********************************************************************
 * �������ƣ� ��ʾҳ��
 * ���������� ��ͼ���ļ��н�����ͼ�����ݲ�����ָ������,�Ӷ�����ҳ������
 * ��������� ptPageDesc - �ں�ҳ���� ҳ������ʾͼ���� �Լ��ص�����
 * ��������� ��
 * �� �� ֵ�� 0      - �ɹ�
 *            ����ֵ - ʧ��
 ***********************************************************************/
int ShowPage ( PT_PageDesc ptPageDesc )
{
	PT_VideoMem pt_VideoTmp;
	int iError;

	DBG_PRINTF ( "show page name :%s\r\n",ptPageDesc->name );
	/* 1. ����Դ� */
	pt_VideoTmp = GetVideoMem ( ID ( ptPageDesc->name ),VMS_FOR_CUR ); //��ȡ�Դ����ڵ�ǰҳ����ʾ
	DBG_PRINTF ( "get videomen ID is %d\r\n",pt_VideoTmp->iID );
	if ( pt_VideoTmp == NULL )
	{
		DBG_PRINTF ( "GetVideoMem error!\r\n" );
		return -1 ;
	}

	/* 2. ����ͼ������ */
	if ( ptPageDesc->atPageLayout[0].iTopLeftX == 0 )
	{
		// CalcMainPageLayout(atLayout);
		ptPageDesc->CalcPageLayout ( ptPageDesc->atPageLayout ); //���ø�ģ��ļ���ͼ�����꺯��
	}

	/* 3. �軭���� */
	iError = GeneratePage ( ptPageDesc->atPageLayout,pt_VideoTmp );
	if ( iError != 0 )
	{
	    DBG_PRINTF("GeneratePage error...\r\n");
		return -1;
	}
	
	if ( ptPageDesc->DispSpecialIcon )
	{
		iError = ptPageDesc->DispSpecialIcon ( pt_VideoTmp );
		if ( iError != 0 )
		{
		    DBG_PRINTF("ptPageDesc->DispSpecialIcon error...\r\n");
			return -1;
		}
	}

	/* 3. ˢ���豸��ȥ       */
	FlushVideoMemToDev ( pt_VideoTmp );

	/* 4. ���Դ��״̬����Ϊfree */
	PutVideoMem ( pt_VideoTmp );

	return 0;

}

/**********************************************************************
 * �������ƣ� TimeMSBetween
 * ���������� ����ʱ���ļ��:��λms
 * ��������� tTimeStart - ��ʼʱ���
 *            tTimeEnd   - ����ʱ���
 * ��������� ��
 * �� �� ֵ�� ���,��λms
 ***********************************************************************/
int TimeMSBetween ( struct timeval tTimeStart, struct timeval tTimeEnd )
{
	int iMs;
	//��ת���� ΢��ת����
	// iMs = (tTimeEnd.tv_sec - tTimeStart.tv_sec)*1000 - (tTimeEnd.tv_usec - tTimeStart.tv_usec) / 1000;
	iMs = ( tTimeEnd.tv_sec - tTimeStart.tv_sec ) * 1000 + ( tTimeEnd.tv_usec - tTimeStart.tv_usec ) / 1000;
	return iMs;
}



int GenericGetPressedIcon ( PT_Layout atLayout,int* bLongPress )
{
	T_InputEvent tInputEvent;
	static T_InputEvent tPreInputEvent;
	static int iIndex,iIndexPressured=-1,bPressure = 0, bFast = 0;

	//while ( 1 )
	//{
	//������ĸ�ͼ���а���
	iIndex = GenericGetInputEvent ( atLayout,&tInputEvent );
	if ( tInputEvent.iPressure == 0 ) //�����ʱ�Ķ������ɿ�
	{
		if ( bPressure ) //�����������
		{
			//�ı䰴���������ɫ
			ReleaseButton ( &atLayout[iIndexPressured] );
			bPressure = 0;
			bFast = 0;
			*bLongPress = 0;

			if ( iIndexPressured == iIndex ) //������º��ɿ�����ͬһ������
			{
				//iIndexPressured = -1;
				return iIndex;//���ش˴ΰ�����Ч
			}

			iIndexPressured = -1;
		}

	}
	else //��ʱ�Ķ����ǰ���
	{
		if ( iIndex != -1 ) //������µ��������ݵİ���
		{
		    
			if ( !bPressure ) //δ�����°�ť
			{
				bPressure = 1;
				iIndexPressured = iIndex;
				tPreInputEvent = tInputEvent;
				//�ı䰴���������ɫ
				PressButton ( &atLayout[iIndexPressured] );
			}
			else if ( iIndexPressured == iIndex ) //���ͬһ������һֱ���ڰ���״̬
			{
				//�Ƚ�ǰһ���뱾�εİ���ʱ�ļ��ʱ��
				if ( (TimeMSBetween ( tPreInputEvent.tTime,tInputEvent.tTime )  > 2000)&&( !bFast) ) //���2s֮���Ǵ��ڰ���״̬
				{
					bFast = 1;//���밴��״̬
					tPreInputEvent = tInputEvent;
					DBG_PRINTF ( "bFast:%d \r\n",bFast );
				}

				if ( ( bFast ) && ( TimeMSBetween ( tPreInputEvent.tTime,tInputEvent.tTime )  > 50 ) ) //���볤��״̬֮��ÿ50ms����һ��ֵ
				{
					*bLongPress = 1;
					tPreInputEvent = tInputEvent;
					return iIndexPressured;

				}
			}
		}
	}
	//}

	return -1;

}



/**********************************************************************
 * �������ƣ� ҳ���ʼ��
 * ���������� ����Ҫ��ʾ��ҳ����뵽�ƶ�������
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0      - �ɹ�
 *            ����ֵ - ʧ��
 ***********************************************************************/
int PagesInit ( void )
{
	int iError = 0;

	iError = MainPageInit();
	iError |= SettingPageInit();
	iError |= IntervalPageInit();
	iError |= BrowsePageInit ();
	iError |= ManualPageInit();

	return iError;

}


