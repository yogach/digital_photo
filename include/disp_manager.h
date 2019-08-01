#ifndef _DISP_MANAGER_H
#define _DISP_MANAGER_H



//表示显存的使用状态
typedef enum
{
	VMS_FREE,
	VMS_FOR_CUR,
	VMS_FOR_PREPARE,

} E_VideoMemState;

//表示显存块中图片的准备状态
typedef enum
{
	PIC_BLANK,
	PIC_GENERATING,
	PIC_GENERATED,

} E_PicState;

//
typedef struct VideoMem
{
	int iID;                        //用于标识用途
	int bDevFrameBuffer;            //用于标识是否是设备的空间
 	E_VideoMemState eVideoMemState;  //用于标识是否被使用
 	E_PicState  ePicState;           //用于标识图片数据是否已准备好
	T_PhotoDesc tVideoMemDesc;          //用于标识
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

