#include "file.c"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>



int MapFile(PT_MapFile ptFileMap)
{
    int iFd;
    struct stat tBMPstat;

    /* ���ļ� */
	iFd = open (ptFileMap->FileName,O_RDWR); //�޷�ʹ��ֻ��ģʽ�򿪣�

	if (iFd == -1)
	{
		DBG_PRINTF ("can't open bmp file %s\r\n",ptFileMap->FileName);
		return - 1;
	}

	//��ȡ�ļ���С ��bmp����mmap���ڴ���
	fstat (iFd,&tBMPstat);
	ptFileMap->iFileSize = tBMPstat.st_size;
	ptFileMap->iFd = iFd;
	ptFileMap->pucFileMapMem = (unsigned char *)mmap (NULL,tBMPstat.st_size,PROT_READ | PROT_WRITE,MAP_SHARED,iFd,0);

	if (ptFileMap->pucFileMapMem == (unsigned char*)-1 )
	{
		DBG_PRINTF ("can't mmap %s file \r\n",ptFileMap->FileName);
		return - 1;
	}

    return 0;

}

void  unMapFile(PT_MapFile ptFileMap)
{
  unmap(ptFileMap->pucFileMapMem);
  close(ptFileMap->iFd);//�ر��ļ����
}

