#include <file.h>
#include <config.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>


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
int MapFile ( PT_MapFile ptFileMap )
{
	int iFd;
	FILE* tFp;
	struct stat tBMPstat;

	/* 打开文件 使用fopen方式打开文件 加强程序可移植性 */
	tFp = fopen ( ptFileMap->FileName,"r+" ); //r+->以读/写方式打开文件，该文件必须存在
	if ( tFp == NULL )
	{
		DBG_PRINTF ( "can't open file %s\r\n",ptFileMap->FileName );
		return - 1;
	}

	ptFileMap->tFp = tFp;

	iFd = fileno ( tFp );

	//获取文件大小 将bmp数据mmap到内存上
	fstat ( iFd,&tBMPstat );
	ptFileMap->iFileSize = tBMPstat.st_size;
	//ptFileMap->iFd = iFd;
	ptFileMap->pucFileMapMem = ( unsigned char* ) mmap ( NULL,tBMPstat.st_size,PROT_READ | PROT_WRITE,MAP_SHARED,iFd,0 );

	if ( ptFileMap->pucFileMapMem == ( unsigned char* )-1 )
	{
		DBG_PRINTF ( "can't mmap %s file \r\n",ptFileMap->FileName );
		return - 1;
	}

	return 0;

}
/**********************************************************************
 * 函数名称： UnMapFile
 * 功能描述： MapFile函数的清理函数
 * 输入参数： ptFileMap - 内含所打开/映射的文件信息
 * 输出参数： 无
 * 返 回 值： 无
 ***********************************************************************/
void  unMapFile ( PT_MapFile ptFileMap )
{
	munmap ( ptFileMap->pucFileMapMem,ptFileMap->iFileSize );
	fclose ( ptFileMap->tFp ); //关闭文件句柄
}

/**********************************************************************
 * 函数名称： isDir
 * 功能描述： 判断一个文件是否为目录
 * 输入参数： strFilePath - 文件的路径
 *            strFileName - 文件的名字
 * 输出参数： 无
 * 返 回 值： 0 - 不是目录
 *            1 - 是目录
 ***********************************************************************/
int  isDir ( char* strFilePath,char* strFileName )
{
	char  strTmp[256];
	struct stat tStat;


	snprintf ( strTmp,256,"%s%s",strFilePath,strFileName ); //输出两个参数到str中
	strTmp[255] = '\0';

	//先获取文件信息 之后判断文件的tStat.st_mode是否为目录类型
	if ( ( stat ( strTmp, &tStat ) == 0 ) && S_ISDIR ( tStat.st_mode ) )
	{
		return 1;
	}
	else
	{
		return 0;
	}


}

/**********************************************************************
 * 函数名称： isRegFile
 * 功能描述： 判断一个文件是否为文件
 * 输入参数： strFilePath - 文件的路径
 *            strFileName - 文件的名字
 * 输出参数： 无
 * 返 回 值： 0 - 不是文件
 *            1 - 是文件
 ***********************************************************************/
int isRegFile ( char* strFilePath,char* strFileName )
{
	char  strTmp[256];
	struct stat tStat;

	snprintf ( strTmp,256,"%s%s",strFilePath,strFileName ); //输出两个参数到str中
	strTmp[255] = '\0';

	//先获取文件信息 之后判断文件的tStat.st_mode是否是一个文件
	if ( ( stat ( strTmp, &tStat ) == 0 ) && S_ISREG ( tStat.st_mode ) )
	{
		return 1;
	}
	else
	{
		return 0;
	}

}



/**********************************************************************
 * 函数名称： isRegDir
 * 功能描述： 判断一个目录是否常规的目录,在本程序中把sbin等目录当作特殊目录来对待
 * 输入参数： strDirPath    - 目录的路径
 *            strSubDirName - 目录的名字
 * 输出参数： 无
 * 返 回 值： 0 - 不是常规目录
 *            1 - 是常规目录
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int isRegDir ( char* strDirPath, char* strSubDirName )
{
	static const char* astrSpecailDirs[] = {"sbin", "bin", "usr", "lib", "proc", "tmp", "dev", "sys", NULL};
	int i;

	/* 如果目录名含有"astrSpecailDirs"中的任意一个, 则返回0 */
	if ( 0 == strcmp ( strDirPath, "/" ) )
	{
		while ( astrSpecailDirs[i] )
		{
			if ( 0 == strcmp ( strSubDirName, astrSpecailDirs[i] ) )
			{
				return 0;
			}
			i++;
		}
	}
	return 1;
}


/**********************************************************************
 * 函数名称： GetDirContents
 * 功能描述： 把某目录下所含的顶层子目录、顶层文件都记录下来,并且按名字排序
 * 输入参数： strDirName - 目录名(含绝对路径)
 * 输出参数： pptDirContents - (*pptDirContents)指向一个PT_DirContent数组,
 *                             (*pptDirContents)[0,1,...]指向T_DirContent结构体,
 *                             T_DirContent中含有"目录/文件"的名字等信息
 *            piNumber       - strDirName下含有多少个"顶层子目录/顶层文件",
 *                             即数组(*pptDirContents)[0,1,...]有多少项
 * 返 回 值： 0 - 成功
 *            -1 - 失败
 ***********************************************************************/
int GetDirContents ( char* strDirName, PT_DirContent** pptDirContents, int* piNumber )
{
	int iNumber = 0, i,j;
	PT_DirContent* aptDirContents;
	struct dirent** aptNameList;

	/* 扫描目录,结果按名字排序,存在aptNameList[0],aptNameList[1]中
	 * 返回值为找到的匹配模式文件的个数
	 * alphasort为排序函数
	 */
	iNumber = scandir ( strDirName, &aptNameList, 0, alphasort ); //注意aptNameList会被动态分配内存 用完后需释放
	if ( iNumber < 0 )
	{
		DBG_PRINTF ( "scandir error : %s!\n", strDirName );
		return -1;
	}

	/* 忽略".", ".."这两个目录 */
	aptDirContents = malloc ( sizeof ( PT_DirContent ) * ( iNumber - 2 ) );//首先先分配数组每项的数组名空间
	if ( aptDirContents == NULL )
	{
		DBG_PRINTF ( "malloc error!\r\n" );
		//如果分配失败需要释放之前分配的aptNameList
		for ( i = 0; i < iNumber; i++ )
		{
			if ( aptNameList[i] )
			{
				free ( aptNameList[i] );
			}
		}
		free ( aptNameList );
		return -1;
	}

	*pptDirContents = aptDirContents;

	//分配每一个数组的空间
	for ( i=0; i < ( iNumber - 2 ); i++ )
	{
		aptDirContents[i] = malloc ( sizeof ( T_DirContent ) ); //分配数组每一项的空间
		if ( aptDirContents[i] == NULL )
		{
			DBG_PRINTF ( "malloc error!\r\n" );
			//如果分配失败 清除之前malloc的各项东西
			for ( i = 0; i < iNumber; i++ )
			{
				if ( aptNameList[i] )
				{
					free ( aptNameList[i] );
				}
			}
			free ( aptNameList );

			while ( i>=0 )
			{
				if ( aptDirContents[i] )
				{
					free ( aptDirContents[i] );
				}

				i--;
			}

			return -1;
		}


	}


	/* 先把目录挑出来存入aptDirContents */
	for ( i = 0, j = 0 ; i <iNumber; i++ )
	{

		/* 忽略".",".."这两个目录 */
		if ( ( strcmp ( aptNameList[i]->d_name,"." ) == 0 ) || ( strcmp ( aptNameList[i]->d_name,".." ) == 0 ) )
		{
			continue;
		}

		/* 并不是所有的文件系统都支持d_type, 所以不能直接判断d_type */
		if ( isDir ( strDirName, aptNameList[i]->d_name ) ) //判断是否为目录  d_name为文件名
		{
			strncpy ( aptDirContents[j]->strName,aptNameList[i]->d_name,256 );
			aptDirContents[j]->strName[255] = '\0';
			aptDirContents[j]->eFileType = FILETYPE_DIR;
			free ( aptNameList[i] );
			aptNameList[i] = NULL;
			j++;
		}

	}

	/* 先把目录挑出来存入aptDirContents */
	for ( i = 0; i <iNumber; i++ )
	{
		if ( aptNameList[i] == NULL )
		{
			continue;
		}

		/* 忽略".",".."这两个目录 */
		if ( ( strcmp ( aptNameList[i]->d_name,"." ) == 0 ) || ( strcmp ( aptNameList[i]->d_name,".." ) == 0 ) )
		{
			continue;
		}

		/* 并不是所有的文件系统都支持d_type, 所以不能直接判断d_type */
		if ( isRegFile ( strDirName, aptNameList[i]->d_name ) ) //判断这个文件名是否为文件
		{
			strncpy ( aptDirContents[j]->strName,aptNameList[i]->d_name,256 );
			aptDirContents[j]->strName[255] = '\0';
			aptDirContents[j]->eFileType = FILETYPE_FILE;
			free ( aptNameList[i] );
			aptNameList[i] = NULL;
			j++;
		}

	}

	//释放aptDirContents中未使用的项
	for ( i = j ; i < ( iNumber - 2 ); i++ )
	{
		free ( aptDirContents[i] );
	}

	//清除已分配的aptNameList
	for ( i = 0; i < iNumber; i++ )
	{
		if ( aptNameList[i] )
		{
			free ( aptNameList[i] );
		}
	}
	free ( aptNameList );

	*piNumber = j;

	return 0;
}
/**********************************************************************
 * 函数名称： FreeDirContents
 * 功能描述： GetDirContents的清理函数,用来释放内存
 * 输入参数： aptDirContents - 指向PT_DirContent数组
 *            iNumber        - 有多少数组项
 * 输出参数： 无
 * 返 回 值： 无
 ***********************************************************************/
void FreeDirContents ( PT_DirContent* aptDirContents, int iNumber )
{
	int i ;
	for ( i = 0; i < iNumber; i++ )
	{
		free ( aptDirContents[i] );
	}
	free ( aptDirContents );

}

/**********************************************************************
 * 函数名称： GetFilesIndir
 * 功能描述： 以深度优先的方式获得目录下的文件
 *            即: 先获得顶层目录下的文件, 再进入一级子目录A
 *                再获得一级子目录A下的文件, 再进入二级子目录AA, ...
 *                处理完一级子目录A后, 再进入一级子目录B
 *
 * "连播模式"下调用该函数获得要显示的文件
 * 有两种方法获得这些文件:
 * 1. 事先把所有文件的名字保存到某个缓冲区中
 * 2. 用到时再去搜索取出若干个文件名
 * 第1种方法比较简单,但是当文件很多时有可能导致内存不足.
 * 我们使用第2种方法:
 * 假设某目录(包括所有子目录)下所有的文件都给它编一个号
 *
 * 输入参数：strDirName                 : 要获得哪个目录下的内容
 *           piCurFileNumber       : 当前已经取到第几个文件
 *           piStartNumberToRecord : 从第几个文件开始取出它们的名字
 *           iFileCountTotal       : 总共要取出多少个文件的名字
 * 输出参数：piFileCountHaveGet    : 已经得到了多少个文件的名字
 *           apstrFileNames[][256] : 用来存储搜索到的文件名
 * 输入/输出参数：
 *           piCurFileNumber       : 当前搜索到的文件编号
 * 返 回 值：0 - 成功
 *           -1 - 失败

 ***********************************************************************/
int GetFilesIndir ( char* strDirName, int* piStartNumberToRecord, int* piCurFileNumber, int* piFileCountHaveGet, int iFileCountTotal, char apstrFileNames[][256] )
{

	PT_DirContent* aptDirContents;  /* 数组:存有目录下"顶层子目录","文件"的名字 */
	int iDirContentsNumber;	  /* g_aptDirContents数组有多少项 */
	int iError,i,iDirContentIndex;
	char strSubDirName[256];

	/* 为避免访问的目录互相嵌套, 设置能访问的目录深度为10 */
#define MAX_DIR_DEEPNESS 10
	static int iDirDeepness = 0;

	if ( iDirDeepness > MAX_DIR_DEEPNESS )
	{
		return -1;
	}

	iDirDeepness++;


	//获得传入路径下的文件夹内容
	iError = GetDirContents ( strDirName, &aptDirContents, &iDirContentsNumber );
	DBG_PRINTF ( "iDirContentsNumber :%d\r\n",iDirContentsNumber );
	if ( iError == -1 )
	{
		DBG_PRINTF ( "GetDirContents error...\r\n" );
		iDirDeepness--;
		return -1;
	}

	for ( i=0; i<iDirContentsNumber; i++ )
	{

		//取出其中的文件拷贝到apstrFileNames中（包含绝对路径）
		if ( aptDirContents[i]->eFileType == FILETYPE_FILE )
		{
			if ( *piCurFileNumber > *piStartNumberToRecord ) //待到指定位置取出
			{
				snpirntf ( apstrFileNames[*piFileCountHaveGet],256,"%s/%s",strDirName,aptDirContents[i]->strName );
				( *piCurFileNumber )++; //当前文件
				( *piFileCountHaveGet )++; //
				( *piStartNumberToRecord )++;
				if ( *piFileCountHaveGet >= iFileCountTotal ) //如果已经取得足够数量的文件 返回
				{
					FreeDirContents ( aptDirContents, iDirContentsNumber ); //释放空间
					iDirDeepness--;
					return 0;
				}

			}
			else
			{
				*piCurFileNumber++;
			}

		}

	}


	for ( i=0; i<iDirContentsNumber; i++ )
	{
		//如果一个目录下的文件数量不够 则继续进入下一级目录取出文件
		if ( ( aptDirContents[i]->eFileType ==  FILETYPE_DIR ) && ( isRegDir ( strDirName,aptDirContents[i]->strName ) ) )
		{
			//取出目录名
			snprintf ( strSubDirName,256,"%s/%s/",strDirName,aptDirContents[i]->strName );
			//递归处理
			GetFilesIndir ( strSubDirName, piStartNumberToRecord, piCurFileNumber, piFileCountHaveGet, iFileCountTotal, apstrFileNames )
			if ( *piFileCountHaveGet >= iFileCountTotal ) //如果已经取得足够数量的文件 返回
			{
				FreeDirContents ( aptDirContents, iDirContentsNumber ); //释放空间
				iDirDeepness--;
				return 0;
			}

		}
	}

	FreeDirContents ( aptDirContents, iDirContentsNumber ); //释放空间
	iDirDeepness--;
	return 0;
}

