#include <config.h>
#include <disp_manager.h>
#include <page_manager.h>
#include <pic_manager.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <input_manager.h>
#include <render.h>


static void MainPageRun ( void );
static int CalcMainPageLayout ( PT_Layout atLayout );


//��ҳ��Ҫ��ʾ��ͼ��
static T_Layout g_atMainPageIconsLayout[]=
{
	{0,0,0,0,"browse_mode.bmp"},
	{0,0,0,0,"continue_mod.bmp"},
	{0,0,0,0,"setting.bmp"},
	{0,0,0,0,NULL},
};


static T_PageDesc g_tMainPageDesc =
{
	.name = "main",
	.Run = MainPageRun,
	.atPageLayout = g_atMainPageIconsLayout,
	.CalcPageLayout = CalcMainPageLayout,
};


/**********************************************************************
 * �������ƣ� CalcMainPageLayout
 * ���������� ����ҳ���и�ͼ������ֵ
 * ��������� ��
 * ��������� atLayout - �ں���ͼ������Ͻ�/���½�����ֵ
 * �� �� ֵ�� ��

 ***********************************************************************/
static int CalcMainPageLayout ( PT_Layout atLayout )
{
	int iXres,iYres,iBpp;
	int iIconWidth,iIconHight,IconX,IconY;
	GetDispResolution ( &iXres,&iYres,&iBpp ); //��ȡLCD�ֱ���

	/*   ÿ��ͼ��߶�Ϊ2/10 Y�ֱ���
	 *   ���Ϊ�߶ȵ�����
	 *    ----------------------
	 *                           1/10 * iYres
	 *         browse_mode.bmp   2/10 * iYres
	 *                           1/10 * iYres
	 *         continue_mod.bmp  2/10 * iYres
	 *                           1/10 * iYres
	 *         setting.bmp       2/10 * iYres
	 *                           1/10 * iYres
	 *    ----------------------
	 *
	 */

	iIconHight =  iYres*2/10 ;   //ͼ��߶�
	iIconWidth =  iIconHight*2;  //ͼ����

	IconX =  ( iXres-iIconWidth ) /2; //ͼ�����
	IconY =  iYres /10 ;

	while ( atLayout->IconName )
	{
		//���ñ�ҳ����ͼ�����ʼ����x y ����
		atLayout->iTopLeftX    = IconX;                  //���Ͻ�X����
		atLayout->iTopLeftY    = IconY;                  //���Ͻ�X����
		atLayout->iLowerRightX = IconX + iIconWidth - 1; //���½�X����
		atLayout->iLowerRightY = IconY + iIconHight - 1; //���½�Y����

		//Y�������µ���
		IconY +=   iYres*3/10 ;
		atLayout++; //ָ��+1 ָ��������һ��
	}

	return 0;
}


//��ҳ����ʾ
/*
static int showMainPage ( PT_Layout atLayout )
{
	PT_VideoMem pt_VideoTmp;
	int iError;

	// 1. ����Դ�
	pt_VideoTmp = GetVideoMem ( ID ( g_tMainPageDesc.name ),VMS_FOR_CUR ); //��ȡ�Դ����ڵ�ǰҳ����ʾ
	if ( pt_VideoTmp == NULL )
	{
		DBG_PRINTF ( "GetVideoMem error!\r\n" );
		return -1 ;
	}

    // 2. ����ͼ������
    if(atLayout->iTopLeftX == 0)
    {
       CalcMainPageLayout(atLayout);
	}

	// 3. �軭����
	iError = GeneratePage(atLayout,pt_VideoTmp);

	// 3. ˢ���豸��ȥ
	FlushVideoMemToDev ( pt_VideoTmp );

	// 4. ���Դ��״̬����Ϊfree
	PutVideoMem ( pt_VideoTmp );

	return 0;

}
*/


static void MainPageRun ( void )
{
	T_InputEvent tInputEvent;
	int iIndex,iIndexPressured=-1,bPressure = 0,bLongPress;

	/* 1. ��ʾҳ�� */
	//showMainPage ( g_atMainPageIconsLayout );
	ShowPage ( &g_tMainPageDesc );

	/* 2. ͨ�������¼���ð��µ�icon �������� */
	while ( 1 )
	{
#if 1
		switch ( GenericGetPressedIcon ( g_atIntervalPageIconsLayout, &bLongPress ) )
		{
			case 0://���ģʽ
				Page ( "browse" )->Run(NULL);
				ShowPage ( &g_tMainPageDesc );
				break;

			case 1://����ҳ��

				break;

			case 2://����ҳ��
				//��ʾ���ý���
				Page ( "setting" )->Run(NULL);
				//�����ý��淵�غ�������ˢ����ʾ
				ShowPage ( &g_tMainPageDesc );
				break;

			default:
				break;
		}

#else
		//������ĸ�ͼ���а���
		iIndex = GenericGetInputEvent ( g_atMainPageIconsLayout,&tInputEvent );

		if ( tInputEvent.iPressure == 0 ) //������ɿ�״̬
		{
			if ( bPressure ) //�����������
			{
				//�ı䰴���������ɫ
				ReleaseButton ( &g_atMainPageIconsLayout[iIndex] );
				bPressure = 0;


				if ( iIndexPressured == iIndex ) //����������ɿ�����ͬһ������
				{

					switch ( iIndexPressured )
					{
						case 0://���ģʽ
							Page ( "browse" )->Run(NULL);
							ShowPage ( &g_tMainPageDesc );
							break;

						case 1://����ҳ��

							break;

						case 2://����ҳ��
							//��ʾ���ý���
							Page ( "setting" )->Run(NULL);
							//�����ý��淵�غ�������ˢ����ʾ
							ShowPage ( &g_tMainPageDesc );
							break;

						default:
							break;
					}

				}

				iIndexPressured = -1;
			}
		}
		else //����ǰ���״̬
		{
			if ( !bPressure ) // δ�����°�ť
			{
				bPressure = 1;
				iIndexPressured = iIndex;
				//�ı䰴���������ɫ
				PressButton ( &g_atMainPageIconsLayout[iIndex] );
			}

		}
#endif

	}


}


int MainPageInit ( void )
{
	return RegisterPageAction ( &g_tMainPageDesc );
}


