#include <page_manager.h>

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
PT_PageAction GetPage ( char* pcName )
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


int PagesInit(void)
{
  int iError;

  

  return iError;

}


