#ifndef _PAGE_MANAGER_H__
#define _PAGE_MANAGER_H__

#include <pic_manager.h>
#include <input_manager.h>
#include <disp_manager.h>
#include <sys/time.h>

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
  char * name;                               //页面名
  PT_Layout atPageLayout;                    //页面描述 包括每个图标4角坐标 以及图片名
  void (*Run)(void);                          //页面运行函数--运行后会显示图片
  int (*CalcPageLayout)(PT_Layout atLayout); //计算每个图标坐标函数 
  int (*DispSpecialIcon)(PT_VideoMem ptVideoMem);
  struct PageDesc *ptNext;
}T_PageDesc , *PT_PageDesc;



int ID(char * str);
int RegisterPageAction ( PT_PageDesc ptPageAction );
void ShowPages ( void );
PT_PageDesc Page ( char* pcName );
int GenericGetInputEvent(PT_Layout ptLayout,PT_InputEvent ptInputEvent);
int GenericGetPressedIcon ( PT_Layout atLayout ,int *bLongPress);
int TimeMSBetween(struct timeval tTimeStart, struct timeval tTimeEnd);


int GeneratePage(PT_Layout atLayout , PT_VideoMem pt_VideoMem);
int ShowPage ( PT_PageDesc ptPageDesc);


int PagesInit(void);
int IntervalPageInit ( void );


int MainPageInit ( void );
int SettingPageInit ( void );
int IntervalPageInit ( void );
//int ManualPageInit ( void );
int BrowsePageInit ( void );


#endif /*_PAGE_MANAGER_H__*/
