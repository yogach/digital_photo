#ifndef _INPUT_MANAGER_H
#define _INPUT_MANAGER_H
#include <sys/time.h>
#include <pthread.h>

#define INPUT_TYPE_STDIN        0 //标准输入
#define INPUT_TYPE_TOUCHSCREEN  1


typedef struct InputEvent {
	struct timeval tTime;
	int iType;  /* 输入事件类型 */
	int iX;   /*X Y坐标  */
	int iY;
	int iPressure;//按下 or 松开
	int iKeyValue;
}T_InputEvent, *PT_InputEvent;


typedef struct InputOpr {
	char *name;
	pthread_t tTreadID;
	int (*DeviceInit)(void);
	int (*DeviceExit)(void);
	int (*GetInputEvent)(PT_InputEvent ptInputEvent);
	struct InputOpr *ptNext;
}T_InputOpr, *PT_InputOpr;


int RegisterInputOpr(PT_InputOpr ptInputOpr);
void ShowInputOpr(void);
int GetDeviceInput(PT_InputEvent ptInputEvent);
int AllInputDevicesInit(void);

int InputInit(void);

int StdinInit(void);
int TouchScreenInit ( void );


#endif /* _INPUT_MANAGER_H */

