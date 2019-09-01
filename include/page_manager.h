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


typedef struct PageDesc
{
  char * name;
  PT_Layout atPageLayout;
  int (*Run)(void);
  int (*CalcPageLayout)(PT_Layout atLayout);  
  struct PageDesc *ptNext;
}T_PageDesc , *PT_PageDesc;



int ID(char * str);
int RegisterPageAction ( PT_PageDesc ptPageAction );
void ShowPages ( void );
PT_PageDesc Page ( char* pcName );
int GenericGetInputEvent(PT_Layout ptLayout,PT_InputEvent ptInputEvent);
void ReleaseButton(PT_Layout ptLayout);
void PressButton(PT_Layout ptLayout);
int GeneratePage(PT_Layout atLayout , PT_VideoMem pt_VideoMem);
int ShowPage ( PT_PageDesc ptPageDesc);


int PagesInit(void);


int MainPageInit ( void );


#endif /*_PAGE_MANAGER_H__*/
