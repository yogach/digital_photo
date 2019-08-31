#ifndef _PAGE_MANAGER_H__
#define _PAGE_MANAGER_H__

#include <pic_manager.h>
#include <input_manager.h>
#include <disp_manager.h>

//用于确定需要显示的图标在页面上的位置
typedef struct Layout
{  
   int iTopLeftX;       //左上角X坐标
   int iTopLeftY;       //左上角Y坐标
   int iLowerRightX;    //右上角X坐标
   int iLowerRightY;    //右上角Y坐标
   char *IconName;      //图标名称
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
