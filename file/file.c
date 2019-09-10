#include <file.h>
#include <config.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include  <dirent.h>


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

	//��ȡ�ļ���Ϣ ֮���ж��ļ���tStat.st_mode�Ƿ�ΪĿ¼����
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
 * �������ƣ� GetDirContents
 * ���������� ��ĳĿ¼�������Ķ�����Ŀ¼�������ļ�����¼����,���Ұ���������
 * ��������� strDirName - Ŀ¼��(������·��)
 * ��������� pptDirContents - (*pptDirContents)ָ��һ��PT_DirContent����,
 *                             (*pptDirContents)[0,1,...]ָ��T_DirContent�ṹ��,
 *                             T_DirContent�к���"Ŀ¼/�ļ�"�����ֵ���Ϣ
 *            piNumber       - strDirName�º��ж��ٸ�"������Ŀ¼/�����ļ�",
 *                             ������(*pptDirContents)[0,1,...]�ж�����
 * �� �� ֵ�� 0 - �ɹ�
 *            1 - ʧ��
 ***********************************************************************/
int GetDirContents ( char* strDirName, PT_DirContent** pptDirContents, int* piNumber )
{
	int iNumber = 0, i,j;
	PT_DirContent* aptDirContents;
	struct dirent** aptNameList;

	/* ɨ��Ŀ¼,�������������,����aptNameList[0],aptNameList[1],...
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
		//�������ʧ����Ҫ�ͷ�֮������aptNameList
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
		if ( ( strcmp ( aptNameList[i]->d_name,'.' ) == 0 ) || ( strcmp ( aptNameList[i]->d_name,'..' ) == 0 ) )
		{
			continue;
		}

		/* ���������е��ļ�ϵͳ��֧��d_type, ���Բ���ֱ���ж�d_type */

		if ( isDir ( strDirName, aptNameList[i]->d_name ) ) //�ж��Ƿ�ΪĿ¼
		{


		}


	}





	return 0;
}


