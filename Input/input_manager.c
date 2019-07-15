
#include <config.h>
#include <input_manager.h>
#include <string.h>

static PT_InputOpr g_ptInputOprHead;//���������ͷ
static T_InputEvent g_tInputEvent;

static pthread_mutex_t g_tMutex  = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  g_tConVar = PTHREAD_COND_INITIALIZER;


//�����ڵ㵽InputOpr������
int RegisterInputOpr(PT_InputOpr ptInputOpr)
{
	PT_InputOpr ptTmp;

	if (!g_ptInputOprHead)
	{
		g_ptInputOprHead   = ptInputOpr;
		ptInputOpr->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptInputOprHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	    = ptInputOpr;
		ptInputOpr->ptNext = NULL;
	}

	return 0;
}

//�̺߳���
static void* InputEventTreadFunction(void* pVoid)
{
   T_InputEvent tInputEvent;

   //���庯��ָ��
   int (*GetInputEvent)(PT_InputEvent ptInputEvent);

   (int (*)(PT_InputEvent))pVoid;

   GetInputEvent = (int(*)(PT_InputEvent))pVoid;//ת��void*ָ��


   while(1)
   {
         if( 0 == GetInputEvent(&tInputEvent)) //�ȴ������¼�
         {
			 /* �������߳�, ��tInputEvent��ֵ����һ��ȫ�ֱ��� */
			 /* �����ٽ���Դǰ���Ȼ�û����� */
			 pthread_mutex_lock(&g_tMutex);
			 g_tInputEvent = tInputEvent;
 
			 /*  �������߳� */
			 pthread_cond_signal(&g_tConVar);
 
			 /* �ͷŻ����� */
			 pthread_mutex_unlock(&g_tMutex);


		 }

   }



}


int AllInputDevicesInit(void)
{
   PT_InputOpr ptTmp = g_ptInputOprHead;
   int iError = -1;

	while (ptTmp)
	{
       if(g_ptInputOprHead->DeviceInit() == 0)
       {
       	   //�����߳� �����豸��GetInputEvent ��Ϊ�βδ���
		   pthread_create(&g_ptInputOprHead->tTreadID,NULL,InputEventTreadFunction,ptTmp->GetInputEvent);
           iError = 0;
	   }
	   ptTmp = ptTmp->ptNext;
	}

   return iError;
}


//��ȡ�����¼�
int GetDeviceInput(PT_InputEvent ptInputEvent)
{

    /* ���� */
	pthread_mutex_lock(&g_tMutex);
	pthread_cond_wait(&g_tConVar, &g_tMutex);	

	/* �����Ѻ�,�������� */
	*ptInputEvent = g_tInputEvent;
	pthread_mutex_unlock(&g_tMutex);

	return 0;
}

//����InputOpr����
void ShowInputOpr(void)
{
	int i = 0;
	PT_InputOpr ptTmp = g_ptInputOprHead;

	while (ptTmp)
	{
		printf("%02d %s\n", i++, ptTmp->name);
		ptTmp = ptTmp->ptNext;
	}
}



int InputInit(void)
{
    int iError;

	iError = StdinInit();

	return iError;

}


