#include <page_manager.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <config.h>

static PT_PageAction g_tPageActionHead;



//��DispOpr�����ڵ�
int RegisterPageAction ( PT_PageAction ptPageAction )
{
	PT_PageAction ptTmp;

	if ( !g_tPageActionHead ) //�����ͷ�ڵ�
	{
		g_tPageActionHead = ptPageAction;
		ptPageAction->ptNext = NULL;
	}
	else
	{
		ptTmp = g_tPageActionHead;

		while ( ptTmp->ptNext )
		{
			ptTmp = ptTmp->ptNext;
		}

		ptTmp->ptNext = ptPageAction;
		ptPageAction->ptNext = NULL;
	}

	return 0;
}


//����ShowDispOpr����
void ShowPages ( void )
{
	int i = 0;
	PT_PageAction ptTmp = g_tPageActionHead;

	while ( ptTmp )
	{
		printf ( "%02d %s\n",i++,ptTmp->name );
		ptTmp = ptTmp->ptNext;
	}
}


//�������ֵõ���Ӧ�Ľڵ�
PT_PageAction Page ( char* pcName )
{
	PT_PageAction ptTmp = g_tPageActionHead;

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

int ID(char * str)
{
   return (int)(str[0]) + (int)(str[1]) + (int)(str[2]) + (int)(str[3]) ;
}


/**********************************************************************
 * �������ƣ� GenericGetInputEvent
 * ���������� ��ȡ��������,���ж���λ����һ��ͼ����
 * ��������� ptPageLayout - �ں����ͼ�����ʾ����
 * ��������� ptInputEvent - �ں��õ�����������
 * �� �� ֵ�� -1     - �������ݲ�λ���κ�һ��ͼ��֮��
 *            ����ֵ - �������������ڵ�ͼ��(ptLayout�������һ��)
**********************************************************************/
int GenericGetInputEvent(PT_Layout ptLayout,PT_InputEvent ptInputEvent)
{
   T_InputEvent tInputEvent;
   int iRet , i = 0;

   

   /*��ȡ������ԭʼ����*/
   iRet = GetDeviceInput(&tInputEvent);   
   if(iRet)
   {
   
     return -1;
   }
   
   *ptInputEvent = tInputEvent;
   
   //������Ǵ������¼�           ����-1
   if(tInputEvent.iType !=INPUT_TYPE_TOUCHSCREEN)
   {
     return -1;
   }

   while(ptLayout[i].IconName)
   {
     //������µĴ�����ĳ��ͼ���� ����ͼ���������ڵ�λ��
     if((tInputEvent.iX >= ptLayout[i].iTopLeftX)&&(tInputEvent.iX <= ptLayout[i].iLowerRightX) &&\
	 	(tInputEvent.iY >= ptLayout[i].iTopLeftY)&&(tInputEvent.iY <= ptLayout[i].iLowerRightY))
     	{
           return i;
		}
		else
		{
            i ++;  
		}
   }

   return -1;

}


int PagesInit(void)
{
  int iError = 0;
  
  iError |= MainPageInit();

  return iError;

}


