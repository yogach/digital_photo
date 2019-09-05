#include <config.h>
#include <disp_manager.h>
#include <page_manager.h>
#include <pic_manager.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <input_manager.h>
#include <render.h>

static void SettingPageRun ( void );
static int CalcSettingPageLayout ( PT_Layout atLayout );


static T_Layout g_atSettingPageIconsLayout[] =
{
	{0, 0, 0, 0, "select_fold.bmp"},
	{0, 0, 0, 0, "interval.bmp"},
	{0, 0, 0, 0, "return.bmp"},
	{0, 0, 0, 0, NULL},
};


static T_PageDesc g_tSettingPageDesc =
{
	.name   = "setting",
	.Run    = SettingPageRun,
	.CalcPageLayout = CalcSettingPageLayout,
	.atPageLayout = g_atSettingPageIconsLayout,
};


static int CalcSettingPageLayout ( PT_Layout atLayout )
{
	int iXres,iYres,iBpp;
	int iIconWidth,iIconHight,IconX,IconY;
	GetDispResolution ( &iXres,&iYres,&iBpp ); //��ȡLCD�ֱ���

	/*	 ÿ��ͼ��߶�Ϊ2/10 Y�ֱ���
	 *	 ���Ϊ�߶ȵ�����
	 *	  ----------------------
	 *							 1/10 * iYres
	 *		   select_fold.bmp	 2/10 * iYres
	 *							 1/10 * iYres
	 *		   interval.bmp      2/10 * iYres
	 *							 1/10 * iYres
	 *		   return.bmp		 2/10 * iYres
	 *							 1/10 * iYres
	 *	  ----------------------
	 *
	 */

	iIconHight =  iYres*2/10 ;	 //ͼ��߶�
	iIconWidth =  iIconHight*2;  //ͼ����

	IconX =  ( iXres-iIconWidth ) /2; //ͼ�����
	IconY =  iYres /10 ;

	while ( atLayout->IconName )
	{
		//���ñ�ҳ����ͼ�����ʼ����x y ����
		atLayout->iTopLeftX    = IconX; 				 //���Ͻ�X����
		atLayout->iTopLeftY    = IconY; 				 //���Ͻ�X����
		atLayout->iLowerRightX = IconX + iIconWidth - 1; //���½�X����
		atLayout->iLowerRightY = IconY + iIconHight - 1; //���½�Y����

		//Y�������µ���
		IconY +=   iYres*3/10 ;
		atLayout++; //ָ��+1 ָ��������һ��
	}

	return 0;

}


static void SettingPageRun ( void )
{
    T_InputEvent tInputEvent;
	int iIndex,iIndexPressured=-1,bPressure = 0;

	/* 1. ��ʾҳ�� */
	ShowPage ( &g_tSettingPageDesc );

	/* 2. ͨ�������¼���ð��µ�icon �������� */
	while ( 1 )
	{
		//������ĸ�ͼ���а���
		iIndex = GenericGetInputEvent ( g_atSettingPageIconsLayout,&tInputEvent );

		if ( tInputEvent.iPressure == 0 ) //������ɿ�״̬
		{
			if ( bPressure ) //�����������
			{
				//�ı䰴���������ɫ
				ReleaseButton ( &g_atSettingPageIconsLayout[iIndex] );
				bPressure = 0;


				if ( iIndexPressured == iIndex ) //����������ɿ�����ͬһ������
				{

					switch ( iIndexPressured )
					{
						case 0://���������ļ���ҳ��
						{
						
						}
						break;

						case 1://�����������ҳ��
						{
						   Page("interval")->Run();
						   ShowPage ( &g_tSettingPageDesc );
						}
						break;

						case 2://����
						{
                            return;
						}
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
				PressButton ( &g_atSettingPageIconsLayout[iIndex] );
			}

		}


	}


}


int SettingPageInit ( void )
{
	return RegisterPageAction ( &g_tSettingPageDesc );

}

