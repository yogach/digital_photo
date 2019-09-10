
#include <config.h>
#include <disp_manager.h>
#include <page_manager.h>
#include <pic_manager.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <input_manager.h>
#include <render.h>

static int CalcBrowsePageLayout ( PT_Layout atLayout );
static int BrowsePageSpecialDis ( PT_VideoMem ptVideoMem );
static void BrowsePageRun ( void );


static T_Layout g_atBrowsePageIconsLayout[] =
{
	{0, 0, 0, 0, "up.bmp"},
	{0, 0, 0, 0, "select.bmp"},
	{0, 0, 0, 0, "pre_page.bmp"},
	{0, 0, 0, 0, "next_page.bmp"},
	{0, 0, 0, 0, NULL},
};


static T_PageDesc g_tBrowsePageDesc =
{
	.name   = "manual",
	.Run    = BrowsePageRun,
	.CalcPageLayout = CalcBrowsePageLayout,
	.atPageLayout = g_atBrowsePageIconsLayout,
	.DispSpecialIcon = BrowsePageSpecialDis,
};


static int CalcBrowsePageLayout ( PT_Layout atLayout )
{

	//�����Ļ�ֱ���
	int iXres,iYres,iBpp,i = 0;
	int iIconWidth,iIconHight,IconX,IconY;
	GetDispResolution ( &iXres,&iYres,&iBpp ); //��ȡLCD�ֱ���

	if ( iXres < iYres )
	{
		/*	 iXres/4
		 *	  ----------------------------------
		 *	   up	select	pre_page  next_page
		 *
		 *
		 *
		 *
		 *
		 *
		 *	  ----------------------------------
		 */
		iIconWidth = iXres/4;
		iIconHight = iIconWidth;

		while ( atLayout->IconName )
		{

			atLayout->iTopLeftX = 0 + iIconWidth*i;
			atLayout->iLowerRightX = atLayout->iTopLeftX + iIconWidth;

			atLayout->iTopLeftY = 0;
			atLayout->iLowerRightY = atLayout->iTopLeftY+iIconHight;

			i++;
			atLayout++;
		}

	}
	else
	{

		/*	 iYres/4
		 *	  ----------------------------------
		 *	   up
		 *
		 *    select
		 *
		 *    pre_page
		 *
		 *   next_page
		 *
		 *	  ----------------------------------
		 */

		iIconHight = iYres/4;
		iIconWidth = iIconHight;

		while ( atLayout->IconName )
		{

			atLayout->iTopLeftX = 0;
			atLayout->iLowerRightX = atLayout->iTopLeftX + iIconWidth;

			atLayout->iTopLeftY = 0 + iIconHight*i;
			atLayout->iLowerRightY = atLayout->iTopLeftY+iIconHight;

			i++;
			atLayout++;

		}
	}

	return 0;

}


static int BrowsePageSpecialDis ( PT_VideoMem ptVideoMem )
{




}

static void BrowsePageRun ( void )
{

	 PT_VideoMem ptDevVideoMem;
	 
	 //�����ʾ�豸�Դ�
	 ptDevVideoMem = GetDevVideoMen();

     /* 0. ���Ҫ��ʾ��Ŀ¼������ */


	 /* 1. ��ʾҳ�� */
	ShowPage ( &g_tBrowsePageDesc );
		 

	while ( 1 )
	{





	}


}


int BrowsePageInit ( void )
{
	return RegisterPageAction ( &g_tBrowsePageDesc );
}




