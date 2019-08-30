#include <file.h>
#include <config.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>


/**********************************************************************
 * �������ƣ� MapFile
 * ���������� ʹ��mmap����ӳ��һ���ļ����ڴ�,�Ժ�Ϳ���ֱ��ͨ���ڴ��������ļ�
 * ��������� ptFileMap - �ں��ļ���strFileName
 * ��������� ptFileMap - tFp           : ���򿪵��ļ����
 *                        iFileSize     : �ļ���С
 *                        pucFileMapMem : ӳ���ڴ���׵�ַ
 * �� �� ֵ�� 0      - �ɹ�
 *            ����ֵ - ʧ��
 **********************************************************************/
int MapFile(PT_MapFile ptFileMap)
{
    int iFd;
	FILE *tFp;
    struct stat tBMPstat;

    /* ���ļ� ʹ��fopen��ʽ���ļ� ��ǿ�������ֲ�� */
	tFp = fopen (ptFileMap->FileName,"r+"); //r+->�Զ�/д��ʽ���ļ������ļ��������
	if (tFp == NULL)
	{
		DBG_PRINTF ("can't open file %s\r\n",ptFileMap->FileName);
		return - 1;
	}

    ptFileMap->tFp = tFp;
  
    iFd = fileno(tFp);

	//��ȡ�ļ���С ��bmp����mmap���ڴ���
	fstat (iFd,&tBMPstat);
	ptFileMap->iFileSize = tBMPstat.st_size;
	//ptFileMap->iFd = iFd;
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
  munmap(ptFileMap->pucFileMapMem,ptFileMap->iFileSize);
  fclose(ptFileMap->tFp);//�ر��ļ����
}

