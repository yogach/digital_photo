#include <page_manager.h>

static PT_PageAction g_tPageActionHead;



//向DispOpr新增节点
int RegisterPageAction ( PT_PageAction ptPageAction )
{
	PT_PageAction ptTmp;

	if ( !g_tPageActionHead ) //如果是头节点
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


//遍历ShowDispOpr链表
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


//根据名字得到对应的节点
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


