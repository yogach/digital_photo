#include <config.h>
#include <disp_manager.h>
#include <page_manager.h>
#include <pic_manager.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <input_manager.h>
#include <render.h>


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

static T_Layout g_NumDispLayout; //���ڱ�ʾ������ʾ�ڿ����ʼ��������
static int g_iIntervalSecond = 1;//������ʾʱ����

/**********************************************************************
 * �������ƣ� CalcIntervalPageLayout
 * ���������� ����ҳ���и�ͼ������ֵ
 * ��������� ��
 * ��������� ptPageLayout - �ں���ͼ������Ͻ�/���½�����ֵ
 * �� �� ֵ�� ��
 ***********************************************************************/
static int CalcIntervalPageLayout ( PT_Layout atLayout )
{
	int iXres,iYres,iBpp;
	int iIconWidth,iIconHight,IconX,IconY;
	GetDispResolution ( &iXres,&iYres,&iBpp ); //��ȡLCD�ֱ���


	/*
	 *    ----------------------
	 *                          1/6 iYres             1/2 * iHeight
	 *          inc.bmp         1/3 iYres  * 28 / 128 iHeight * 28 / 128
	 *         time.bmp         1/3 iYres  * 72 / 128 iHeight * 72 / 128
	 *          dec.bmp         1/3 iYres  * 28 / 128 iHeight * 28 / 128
	 *                          1/6 iYres             1/2 * iHeight
	 *    ok.bmp     cancel.bmp 1/6 iYres             1/2 * iHeight
	 *                          1/6 iYres             1/2 * iHeight
	 *    ----------------------
	 */

	//1��inc.bmp
	iIconHight = iYres/3 *28 /128;
	iIconWidth = iIconHight;
	IconX =  ( iXres-iIconWidth ) /2; //ͼ�����
	IconY =  iYres /6 ;
	atLayout[0]->iTopLeftX    = IconX;
	atLayout[0]->iTopLeftY    = IconY;
	atLayout[0]->iLowerRightX = IconX + iIconWidth - 1;
	atLayout[0]->iLowerRightY = IconY + iIconHight - 1;


	//2��time.bmp
	iIconHight = iYres/3 *72 /128;
	iIconWidth = iIconHight;
	IconX =  ( iXres-iIconWidth ) /2; //ͼ�����
	//IconY =  iYres /6 ;
	atLayout[1]->iTopLeftX    = IconX;
	atLayout[1]->iTopLeftY    = atLayout[0]->iLowerRightY + 1;
	atLayout[1]->iLowerRightX = IconX + iIconWidth - 1;
	atLayout[1]->iLowerRightY = atLayout[1]->iTopLeftY + iIconHight - 1;

	//3��dec.bmp
	iIconHight = iYres/3 *28 /128;
	iIconWidth = iIconHight;
	IconX =  ( iXres-iIconWidth ) /2; //ͼ�����
	//IconY =  iYres /6 ;
	atLayout[2]->iTopLeftX    = IconX;
	atLayout[2]->iTopLeftY    = atLayout[1]->iLowerRightY + 1;
	atLayout[2]->iLowerRightX = IconX + iIconWidth - 1;
	atLayout[2]->iLowerRightY = atLayout[2]->iTopLeftY + iIconHight - 1;

	//4��ok.bmp
	iIconHight = iYres/6;
	iIconWidth = iIconHight;
	IconX =  iXres /6; //
	//IconY =  iYres /6 ;
	atLayout[3]->iTopLeftX    = IconX;
	atLayout[3]->iTopLeftY    = atLayout[2]->iLowerRightY + iYres/6;
	atLayout[3]->iLowerRightX = IconX + iIconWidth - 1;
	atLayout[3]->iLowerRightY = atLayout[3]->iTopLeftY + iIconHight - 1;


	//5��cancel.bmp
	atLayout[4]->iTopLeftX    = atLayout[3]->iLowerRightX + iXres/3;
	atLayout[4]->iTopLeftY    = atLayout[3]->iTopLeftY;
	atLayout[4]->iLowerRightX = atLayout[4]->iTopLeftX + iIconWidth - 1;
	atLayout[4]->iLowerRightY = atLayout[3]->iLowerRightY;


	/* ������ʾ���ֵ�����Ƚ�����, ��������
	 * time.bmpԭͼ��СΪ128x72, ������������ִ�СΪ52x40
	 * ����CalcIntervalPageLayout���������� �������Ķ�Ӧ��С
	 */
	iIconHight = atLayout[1]->iLowerRightY - atLayout[1]->iTopLeftY + 1;
	iIconWidth = atLayout[1]->iLowerRightX - atLayout[1]->iTopLeftX + 1;

	g_NumDispLayout.iTopLeftX    = atLayout[1]->iTopLeftX + ( 128 - 52 ) /2 *iIconWidth/128;
	g_NumDispLayout.iLowerRightX = atLayout[1]->iLowerRightX - ( 128 - 52 ) /2 *iIconWidth/128 + 1;

	g_NumDispLayout.iTopLeftY    = atLayout[1]->iTopLeftY + ( 72 -	40 ) /2 *iIconHight/72;
	g_NumDispLayout.iLowerRightY = atLayout[1]->iLowerRightY - ( 72 -  40 ) /2 *iIconHight/72 + 1;


	return 0;

}

//����ˢ��������ʾ��
static int GenerateIntervalPageSpecialIcon ( int dwNumber, PT_VideoMem ptVideoMem )
{
    unsigned int dwFontSize;
    char strNumber[3];
    int iError;

    //������ֵĸ߶�
	dwFontSize = g_NumDispLayout.iLowerRightY - g_NumDispLayout.iTopLeftY;

	SetFontSize(dwFontSize); //�����������

    //�������ֵĴ�С
    if(dwNumber > 59)
    {
       // dwNumber = 59;
       return -1;
	}

	snprintf(strNumber, 3, "%02d", dwNumber);//������ת��Ϊ�ַ��� �������С��10ǰ�油0 

	//��ָ�������ھ�����ʾ
	iError = MergerStringToCenterOfRectangleInVideoMem(g_NumDispLayout.iTopLeftX,g_NumDispLayout.iTopLeftY,g_NumDispLayout.iLowerRightX,\
	                                                   g_NumDispLayout.iLowerRightY,(unsigned char* )strNumber,ptVideoMem);
	return iError;
  

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


						case 2://
						{
							//return;
						}
						break;
						case 3://
						{
							//return;
						}
						break;

						case 4://���ذ���
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

