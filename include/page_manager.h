#ifndef _PAGE_MANAGER_H__
#define _PAGE_MANAGER_H__

#include <pic_manager.h>
#include <input_manager.h>
#include <disp_manager.h>

//����ȷ����Ҫ��ʾ��ͼ����ҳ���ϵ�λ��
typedef struct Layout
{  
   int iTopLeftX;       //���Ͻ�X����
   int iTopLeftY;       //���Ͻ�Y����
   int iLowerRightX;    //���Ͻ�X����
   int iLowerRightY;    //���Ͻ�Y����
   char *IconName;      //ͼ������
}T_Layout ,*PT_Layout;


typedef struct PageAction
{
  char * name;
  int (*Run)(void);
  int (*GetInputEvent)(PT_Layout atLayout,PT_InputEvent ptInputEvent);
  //int (*Prepare)();
  struct PageAction *ptNext;
}T_PageAction , *PT_PageAction;




int ID(char * str);
int RegisterPageAction ( PT_PageAction ptPageAction );
void ShowPages ( void );
PT_PageAction Page ( char* pcName );
int GenericGetInputEvent(PT_Layout ptLayout,PT_InputEvent ptInputEvent);
void ReleaseButton(PT_Layout ptLayout);
void PressButton(PT_Layout ptLayout);
int GeneratePage(PT_Layout atLayout , PT_VideoMem pt_VideoMem);


int PagesInit(void);


int MainPageInit ( void );


#endif /*_PAGE_MANAGER_H__*/
