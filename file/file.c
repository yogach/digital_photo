#include <file.h>
#include <config.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>


/**********************************************************************
 * 函数名称： MapFile
 * 功能描述： 使用mmap函数映射一个文件到内存,以后就可以直接通过内存来访问文件
 * 输入参数： ptFileMap - 内含文件名strFileName
 * 输出参数： ptFileMap - tFp           : 所打开的文件句柄
 *                        iFileSize     : 文件大小
 *                        pucFileMapMem : 映射内存的首地址
 * 返 回 值： 0      - 成功
 *            其他值 - 失败
 **********************************************************************/
int MapFile(PT_MapFile ptFileMap)
{
    int iFd;
	FILE *tFp;
    struct stat tBMPstat;

    /* 打开文件 使用fopen方式打开文件 加强程序可移植性 */
	tFp = fopen (ptFileMap->FileName,"r+"); //r+->以读/写方式打开文件，该文件必须存在
	if (tFp == NULL)
	{
		DBG_PRINTF ("can't open file %s\r\n",ptFileMap->FileName);
		return - 1;
	}

    ptFileMap->tFp = tFp;
  
    iFd = fileno(tFp);

	//获取文件大小 将bmp数据mmap到内存上
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
  fclose(ptFileMap->tFp);//关闭文件句柄
}

