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


typedef struct PageDesc
{
  char * name;                               //ҳ����
  PT_Layout atPageLayout;                    //ҳ������ ����ÿ��ͼ��4������ �Լ�ͼƬ��
  void (*Run)(void);                          //ҳ�����к���--���к����ʾͼƬ
  int (*CalcPageLayout)(PT_Layout atLayout); //����ÿ��ͼ�����꺯�� 
  int (*DispSpecialIcon)(PT_VideoMem ptVideoMem);
  struct PageDesc *ptNext;
}T_PageDesc , *PT_PageDesc;



int ID(char * str);
int RegisterPageAction ( PT_PageDesc ptPageAction );
void ShowPages ( void );
PT_PageDesc Page ( char* pcName );
int GenericGetInputEvent(PT_Layout ptLayout,PT_InputEvent ptInputEvent);

int GeneratePage(PT_Layout atLayout , PT_VideoMem pt_VideoMem);
int ShowPage ( PT_PageDesc ptPageDesc);


int PagesInit(void);
int IntervalPageInit ( void );


int MainPageInit ( void );
int SettingPageInit ( void );


#endif /*_PAGE_MANAGER_H__*/
