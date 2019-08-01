#ifndef _DISP_MANAGER_H
#define _DISP_MANAGER_H



//��ʾ�Դ��ʹ��״̬
typedef enum
{
	VMS_FREE,
	VMS_FOR_CUR,
	VMS_FOR_PREPARE,

} E_VideoMemState;

//��ʾ�Դ����ͼƬ��׼��״̬
typedef enum
{
	PIC_BLANK,
	PIC_GENERATING,
	PIC_GENERATED,

} E_PicState;

//
typedef struct VideoMem
{
	int iID;                        //���ڱ�ʶ��;
	int bDevFrameBuffer;            //���ڱ�ʶ�Ƿ����豸�Ŀռ�
 	E_VideoMemState eVideoMemState;  //���ڱ�ʶ�Ƿ�ʹ��
 	E_PicState  ePicState;           //���ڱ�ʶͼƬ�����Ƿ���׼����
	T_PhotoDesc tVideoMemDesc;          //���ڱ�ʶ
	struct VideoMem* ptNext;
} T_VideoMem,*PT_VideoMem;


typedef struct DispOpr
{
	char* name;
	int iXres;
	int iYres;
	int iBpp;
	unsigned char* pucDispMem;
	int ( *DeviceInit ) ( void );
	int ( *ShowPixel ) ( int iPenX, int iPenY, unsigned int dwColor );
	int ( *CleanScreen ) ( unsigned int dwBackColor );
	int ( *ShowPage)(PT_VideoMem ptVideoMem);
	struct DispOpr* ptNext;
} T_DispOpr, *PT_DispOpr;


int RegisterDispOpr ( PT_DispOpr ptDispOpr );
void ShowDispOpr ( void );
int DisplayInit ( void );
int FBInit ( void );

int SelectAndInitDefaultDispDev ( char* name );
PT_DispOpr GetDefaultDispDev ( void );
int GetDispResolution ( int *iXres,int *iYres,int *iBpp );
int AllocVideoMem ( int iNum );
PT_VideoMem GetVideoMem ( int iID, int bUseForCur );
int PutVideoMem(PT_VideoMem ptVideoMem);
void FlushVideoMemToDev(PT_VideoMem pt_VideoTmp);
int SetVideoMemColor(PT_VideoMem ptVideoMem ,unsigned int dwColor);


#endif /* _DISP_MANAGER_H */

