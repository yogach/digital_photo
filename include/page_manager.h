#ifndef _PAGE_MANAGER_H__
#define _PAGE_MANAGER_H__

#include<pic_manager.h>

typedef struct PageAction
{
  char * name;
  int (*Run)(void);
  int (*GetInputEvent)();
  int (*Prepare)();
  struct PageAction *ptNext;
}T_PageAction , *PT_PageAction;



//����ȷ����Ҫ��ʾ��ͼ����ҳ���ϵ�λ��
typedef struct Layout
{
   int iTopLeftX;
   int iTopLeftY;
   int iLowerRightX;
   int iLowerRightY;
   char *IconName;
}T_Layout ,*PT_Layout;


int ID(char * str);
int RegisterPageAction ( PT_PageAction ptPageAction );
void ShowPages ( void );
PT_PageAction Page ( char* pcName );
int GenericGetInputEvent(PT_Layout ptLayout,PT_InputEvent ptInputEvent);


int PagesInit(void);


int MainPageInit ( void );


#endif /*_PAGE_MANAGER_H__*/
