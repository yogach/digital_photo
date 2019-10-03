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
 * �������ƣ� MapFile
 * ���������� ʹ��mmap����ӳ��һ���ļ����ڴ�,�Ժ�Ϳ���ֱ��ͨ���ڴ��������ļ�
 * ��������� ptFileMap - �ں��ļ���strFileName
 * ��������� ptFileMap - tFp           : ���򿪵��ļ����
 *                        iFileSize     : �ļ���С
 *                        pucFileMapMem : ӳ���ڴ���׵�ַ
 * �� �� ֵ�� 0      - �ɹ�
 *            ����ֵ - ʧ��
 **********************************************************************/
int MapFile ( PT_MapFile ptFileMap )
{
	int iFd;
	FILE* tFp;
	struct stat tBMPstat;

	/* ���ļ� ʹ��fopen��ʽ���ļ� ��ǿ�������ֲ�� */
	tFp = fopen ( ptFileMap->FileName,"r+" ); //r+->�Զ�/д��ʽ���ļ������ļ��������
	if ( tFp == NULL )
	{
		DBG_PRINTF ( "can't open file %s\r\n",ptFileMap->FileName );
		return - 1;
	}

	ptFileMap->tFp = tFp;

	iFd = fileno ( tFp );

	//��ȡ�ļ���С ��bmp����mmap���ڴ���
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
 * �������ƣ� UnMapFile
 * ���������� MapFile������������
 * ��������� ptFileMap - �ں�����/ӳ����ļ���Ϣ
 * ��������� ��
 * �� �� ֵ�� ��
 ***********************************************************************/
void  unMapFile ( PT_MapFile ptFileMap )
{
	munmap ( ptFileMap->pucFileMapMem,ptFileMap->iFileSize );
	fclose ( ptFileMap->tFp ); //�ر��ļ����
}

/**********************************************************************
 * �������ƣ� isDir
 * ���������� �ж�һ���ļ��Ƿ�ΪĿ¼
 * ��������� strFilePath - �ļ���·��
 *            strFileName - �ļ�������
 * ��������� ��
 * �� �� ֵ�� 0 - ����Ŀ¼
 *            1 - ��Ŀ¼
 ***********************************************************************/
int  isDir ( char* strFilePath,char* strFileName )
{
	char  strTmp[256];
	struct stat tStat;


	snprintf ( strTmp,256,"%s%s",strFilePath,strFileName ); //�������������str��
	strTmp[255] = '\0';

	//�Ȼ�ȡ�ļ���Ϣ ֮���ж��ļ���tStat.st_mode�Ƿ�ΪĿ¼����
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
 * �������ƣ� isRegFile
 * ���������� �ж�һ���ļ��Ƿ�Ϊ�ļ�
 * ��������� strFilePath - �ļ���·��
 *            strFileName - �ļ�������
 * ��������� ��
 * �� �� ֵ�� 0 - �����ļ�
 *            1 - ���ļ�
 ***********************************************************************/
int isRegFile ( char* strFilePath,char* strFileName )
{
	char  strTmp[256];
	struct stat tStat;

	snprintf ( strTmp,256,"%s%s",strFilePath,strFileName ); //�������������str��
	strTmp[255] = '\0';

	//�Ȼ�ȡ�ļ���Ϣ ֮���ж��ļ���tStat.st_mode�Ƿ���һ���ļ�
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
 * �������ƣ� isRegDir
 * ���������� �ж�һ��Ŀ¼�Ƿ񳣹��Ŀ¼,�ڱ������а�sbin��Ŀ¼��������Ŀ¼���Դ�
 * ��������� strDirPath    - Ŀ¼��·��
 *            strSubDirName - Ŀ¼������
 * ��������� ��
 * �� �� ֵ�� 0 - ���ǳ���Ŀ¼
 *            1 - �ǳ���Ŀ¼
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
static int isRegDir ( char* strDirPath, char* strSubDirName )
{
	static const char* astrSpecailDirs[] = {"sbin", "bin", "usr", "lib", "proc", "tmp", "dev", "sys", NULL};
	int i;

	/* ���Ŀ¼������"astrSpecailDirs"�е�����һ��, �򷵻�0 */
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
 * �������ƣ� GetDirContents
 * ���������� ��ĳĿ¼�������Ķ�����Ŀ¼�������ļ�����¼����,���Ұ���������
 * ��������� strDirName - Ŀ¼��(������·��)
 * ��������� pptDirContents - (*pptDirContents)ָ��һ��PT_DirContent����,
 *                             (*pptDirContents)[0,1,...]ָ��T_DirContent�ṹ��,
 *                             T_DirContent�к���"Ŀ¼/�ļ�"�����ֵ���Ϣ
 *            piNumber       - strDirName�º��ж��ٸ�"������Ŀ¼/�����ļ�",
 *                             ������(*pptDirContents)[0,1,...]�ж�����
 * �� �� ֵ�� 0 - �ɹ�
 *            -1 - ʧ��
 ***********************************************************************/
int GetDirContents ( char* strDirName, PT_DirContent** pptDirContents, int* piNumber )
{
	int iNumber = 0, i,j;
	PT_DirContent* aptDirContents;
	struct dirent** aptNameList;

	/* ɨ��Ŀ¼,�������������,����aptNameList[0],aptNameList[1]��
	 * ����ֵΪ�ҵ���ƥ��ģʽ�ļ��ĸ���
	 * alphasortΪ������
	 */
	iNumber = scandir ( strDirName, &aptNameList, 0, alphasort ); //ע��aptNameList�ᱻ��̬�����ڴ� ��������ͷ�
	if ( iNumber < 0 )
	{
		DBG_PRINTF ( "scandir error : %s!\n", strDirName );
		return -1;
	}

	/* ����".", ".."������Ŀ¼ */
	aptDirContents = malloc ( sizeof ( PT_DirContent ) * ( iNumber - 2 ) );//�����ȷ�������ÿ����������ռ�
	if ( aptDirContents == NULL )
	{
		DBG_PRINTF ( "malloc error!\r\n" );
		//�������ʧ����Ҫ�ͷ�֮ǰ�����aptNameList
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

	//����ÿһ������Ŀռ�
	for ( i=0; i < ( iNumber - 2 ); i++ )
	{
		aptDirContents[i] = malloc ( sizeof ( T_DirContent ) ); //��������ÿһ��Ŀռ�
		if ( aptDirContents[i] == NULL )
		{
			DBG_PRINTF ( "malloc error!\r\n" );
			//�������ʧ�� ���֮ǰmalloc�ĸ����
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


	/* �Ȱ�Ŀ¼����������aptDirContents */
	for ( i = 0, j = 0 ; i <iNumber; i++ )
	{

		/* ����".",".."������Ŀ¼ */
		if ( ( strcmp ( aptNameList[i]->d_name,"." ) == 0 ) || ( strcmp ( aptNameList[i]->d_name,".." ) == 0 ) )
		{
			continue;
		}

		/* ���������е��ļ�ϵͳ��֧��d_type, ���Բ���ֱ���ж�d_type */
		if ( isDir ( strDirName, aptNameList[i]->d_name ) ) //�ж��Ƿ�ΪĿ¼  d_nameΪ�ļ���
		{
			strncpy ( aptDirContents[j]->strName,aptNameList[i]->d_name,256 );
			aptDirContents[j]->strName[255] = '\0';
			aptDirContents[j]->eFileType = FILETYPE_DIR;
			free ( aptNameList[i] );
			aptNameList[i] = NULL;
			j++;
		}

	}

	/* �Ȱ�Ŀ¼����������aptDirContents */
	for ( i = 0; i <iNumber; i++ )
	{
		if ( aptNameList[i] == NULL )
		{
			continue;
		}

		/* ����".",".."������Ŀ¼ */
		if ( ( strcmp ( aptNameList[i]->d_name,"." ) == 0 ) || ( strcmp ( aptNameList[i]->d_name,".." ) == 0 ) )
		{
			continue;
		}

		/* ���������е��ļ�ϵͳ��֧��d_type, ���Բ���ֱ���ж�d_type */
		if ( isRegFile ( strDirName, aptNameList[i]->d_name ) ) //�ж�����ļ����Ƿ�Ϊ�ļ�
		{
			strncpy ( aptDirContents[j]->strName,aptNameList[i]->d_name,256 );
			aptDirContents[j]->strName[255] = '\0';
			aptDirContents[j]->eFileType = FILETYPE_FILE;
			free ( aptNameList[i] );
			aptNameList[i] = NULL;
			j++;
		}

	}

	//�ͷ�aptDirContents��δʹ�õ���
	for ( i = j ; i < ( iNumber - 2 ); i++ )
	{
		free ( aptDirContents[i] );
	}

	//����ѷ����aptNameList
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
 * �������ƣ� FreeDirContents
 * ���������� GetDirContents��������,�����ͷ��ڴ�
 * ��������� aptDirContents - ָ��PT_DirContent����
 *            iNumber        - �ж���������
 * ��������� ��
 * �� �� ֵ�� ��
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
 * �������ƣ� GetFilesIndir
 * ���������� ��������ȵķ�ʽ���Ŀ¼�µ��ļ�
 *            ��: �Ȼ�ö���Ŀ¼�µ��ļ�, �ٽ���һ����Ŀ¼A
 *                �ٻ��һ����Ŀ¼A�µ��ļ�, �ٽ��������Ŀ¼AA, ...
 *                ������һ����Ŀ¼A��, �ٽ���һ����Ŀ¼B
 *
 * "����ģʽ"�µ��øú������Ҫ��ʾ���ļ�
 * �����ַ��������Щ�ļ�:
 * 1. ���Ȱ������ļ������ֱ��浽ĳ����������
 * 2. �õ�ʱ��ȥ����ȡ�����ɸ��ļ���
 * ��1�ַ����Ƚϼ�,���ǵ��ļ��ܶ�ʱ�п��ܵ����ڴ治��.
 * ����ʹ�õ�2�ַ���:
 * ����ĳĿ¼(����������Ŀ¼)�����е��ļ���������һ����
 *
 * ���������strDirName                 : Ҫ����ĸ�Ŀ¼�µ�����
 *           piCurFileNumber       : ��ǰ�Ѿ�ȡ���ڼ����ļ�
 *           piStartNumberToRecord : �ӵڼ����ļ���ʼȡ�����ǵ�����
 *           iFileCountTotal       : �ܹ�Ҫȡ�����ٸ��ļ�������
 * ���������piFileCountHaveGet    : �Ѿ��õ��˶��ٸ��ļ�������
 *           apstrFileNames[][256] : �����洢���������ļ���
 * ����/���������
 *           piCurFileNumber       : ��ǰ���������ļ����
 * �� �� ֵ��0 - �ɹ�
 *           -1 - ʧ��

 ***********************************************************************/
int GetFilesIndir ( char* strDirName, int* piStartNumberToRecord, int* piCurFileNumber, int* piFileCountHaveGet, int iFileCountTotal, char apstrFileNames[][256] )
{

	PT_DirContent* aptDirContents;  /* ����:����Ŀ¼��"������Ŀ¼","�ļ�"������ */
	int iDirContentsNumber;	  /* g_aptDirContents�����ж����� */
	int iError,i,iDirContentIndex;
	char strSubDirName[256];

	/* Ϊ������ʵ�Ŀ¼����Ƕ��, �����ܷ��ʵ�Ŀ¼���Ϊ10 */
#define MAX_DIR_DEEPNESS 10
	static int iDirDeepness = 0;

	if ( iDirDeepness > MAX_DIR_DEEPNESS )
	{
		return -1;
	}

	iDirDeepness++;


	//��ô���·���µ��ļ�������
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

		//ȡ�����е��ļ�������apstrFileNames�У���������·����
		if ( aptDirContents[i]->eFileType == FILETYPE_FILE )
		{
			if ( *piCurFileNumber > *piStartNumberToRecord ) //����ָ��λ��ȡ��
			{
				snpirntf ( apstrFileNames[*piFileCountHaveGet],256,"%s/%s",strDirName,aptDirContents[i]->strName );
				( *piCurFileNumber )++; //��ǰ�ļ�
				( *piFileCountHaveGet )++; //
				( *piStartNumberToRecord )++;
				if ( *piFileCountHaveGet >= iFileCountTotal ) //����Ѿ�ȡ���㹻�������ļ� ����
				{
					FreeDirContents ( aptDirContents, iDirContentsNumber ); //�ͷſռ�
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
		//���һ��Ŀ¼�µ��ļ��������� �����������һ��Ŀ¼ȡ���ļ�
		if ( ( aptDirContents[i]->eFileType ==  FILETYPE_DIR ) && ( isRegDir ( strDirName,aptDirContents[i]->strName ) ) )
		{
			//ȡ��Ŀ¼��
			snprintf ( strSubDirName,256,"%s/%s/",strDirName,aptDirContents[i]->strName );
			//�ݹ鴦��
			GetFilesIndir ( strSubDirName, piStartNumberToRecord, piCurFileNumber, piFileCountHaveGet, iFileCountTotal, apstrFileNames )
			if ( *piFileCountHaveGet >= iFileCountTotal ) //����Ѿ�ȡ���㹻�������ļ� ����
			{
				FreeDirContents ( aptDirContents, iDirContentsNumber ); //�ͷſռ�
				iDirDeepness--;
				return 0;
			}

		}
	}

	FreeDirContents ( aptDirContents, iDirContentsNumber ); //�ͷſռ�
	iDirDeepness--;
	return 0;
}

