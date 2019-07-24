#ifndef _DISP_MANAGER_H
#define _DISP_MANAGER_H

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
	struct DispOpr* ptNext;
} T_DispOpr, *PT_DispOpr;


typedef enum
{
	VMS_FREE,
	VMS_FOR_CUR,
	VMS_FOR_PREPARE,

} E_VideoMemState;


typedef enum
{
	PIC_BLANK,
	PIC_GENERATING,
	PIC_GENERATED,

} E_PicState;


typedef struct VideoMem
{
	int iID;//用于标识用途
	int bDevFrameBuffer;//用于标识是否是设备的空间
	E_VideoMemState VideoMemState; //表示是否被使用
	E_PicState  PicState;
	T_PhotoDesc PhotoDesc;
	struct VideoMem* ptnext;
} T_VideoMem,*PT_VideoMem;


int RegisterDispOpr ( PT_DispOpr ptDispOpr );
void ShowDispOpr ( void );
int DisplayInit ( void );
int FBInit ( void );

int SetDefaultDispDev ( char* name );
PT_DispOpr GetDefaultDispDev ( void );
int GetDispResolution ( int iXres,int iYres,int iBpp );
int AllocVideoMem ( int iNum );
PT_VideoMem GetVideoMem ( int iID, int bUseForCur );
int PutVideoMem(PT_VideoMem ptVideoMem);



#endif /* _DISP_MANAGER_H */

