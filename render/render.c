#include <config.h>
#include <pic_manager.h>
#include <display_manager.h>



int MergerStringToCenterOfRectangleInVideoMem(int iTopLeftX, int iTopLeftY, int iBotRightX, int iBotRightY, unsigned char *pucTextString, PT_VideoMem ptVideoMem)
{






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



