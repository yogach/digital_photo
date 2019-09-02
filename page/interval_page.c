#include <config.h>
#include <disp_manager.h>
#include <page_manager.h>
#include <pic_manager.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <input_manager.h>

static void IntervalPageRun ( void );
static int CalcIntervalPageLayout ( PT_Layout atLayout );


static T_Layout g_atIntervalPageIconsLayout[] =
{
	{0, 0, 0, 0, "inc.bmp"},
	{0, 0, 0, 0, "time.bmp"},
	{0, 0, 0, 0, "dec.bmp"},
	{0, 0, 0, 0, "ok.bmp"},
	{0, 0, 0, 0, "cancel.bmp"},
	{0, 0, 0, 0, NULL},
};


static T_PageDesc g_tIntervalPageDesc =
{
	.name   = "setting",
	.Run    = IntervalPageRun,
	.CalcPageLayout = CalcIntervalPageLayout,
	.atPageLayout = g_atIntervalPageIconsLayout,
};


static int CalcIntervalPageLayout ( PT_Layout atLayout )
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



	return 0;

}


static void IntervalPageRun ( void )
{
    T_InputEvent tInputEvent;
	int iIndex,iIndexPressured=-1,bPressure = 0;

	/* 1. ��ʾҳ�� */
	ShowPage ( &g_tIntervalPageDesc );

	/* 2. ͨ�������¼���ð��µ�icon �������� */
	while ( 1 )
	{
		//������ĸ�ͼ���а���
		iIndex = GenericGetInputEvent ( g_atIntervalPageIconsLayout,&tInputEvent );

		if ( tInputEvent.iPressure == 0 ) //������ɿ�״̬
		{
			if ( bPressure ) //�����������
			{
				//�ı䰴���������ɫ
				ReleaseButton ( &g_atIntervalPageIconsLayout[iIndex] );
				bPressure = 0;


				if ( iIndexPressured == iIndex ) //����������ɿ�����ͬһ������
				{

					switch ( iIndexPressured )
					{
						case 0://
						{
						
						}
						break;

						case 1://
						{
						
						}
						break;

						case 2://
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
				PressButton ( &g_atIntervalPageIconsLayout[iIndex] );
			}

		}


	}


}


int IntervalPageInit ( void )
{
	return RegisterPageAction ( &g_tIntervalPageDesc );

}

