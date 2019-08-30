#include <page_manager.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <config.h>

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
 * 函数名称： GenericGetInputEvent
 * 功能描述： 读取输入数据,并判断它位于哪一个图标上
 * 输入参数： ptPageLayout - 内含多个图标的显示区域
 * 输出参数： ptInputEvent - 内含得到的输入数据
 * 返 回 值： -1     - 输入数据不位于任何一个图标之上
 *            其他值 - 输入数据所落在的图标(ptLayout数组的哪一项)
**********************************************************************/
int GenericGetInputEvent(PT_Layout ptLayout,PT_InputEvent ptInputEvent)
{
   T_InputEvent tInputEvent;
   int iRet , i = 0;

   

   /*获取触摸屏原始数据*/
   iRet = GetDeviceInput(&tInputEvent);   
   if(iRet)
   {
   
     return -1;
   }
   
   *ptInputEvent = tInputEvent;
   
   //如果不是触摸屏事件           返回-1
   if(tInputEvent.iType !=INPUT_TYPE_TOUCHSCREEN)
   {
     return -1;
   }

   while(ptLayout[i].IconName)
   {
     //如果按下的触点在某个图标内 返回图标在数组内的位置
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


