#include <config.h>
#include <pic_manager.h>
#include <file.h>
#include <string.h>
#include <disp_manager.h>
#include <stdlib.h>


static PT_PicFileParser g_ptPicFileParserHead;
extern T_PicFileParser g_tBMPFileParser;


//��DispOpr�����ڵ�
int RegisterPicFileParser ( PT_PicFileParser ptPicFileParser )
{
	PT_PicFileParser ptTmp;

	if ( !g_ptPicFileParserHead ) //�����ͷ�ڵ�
	{
		g_ptPicFileParserHead   = ptPicFileParser;
		ptPicFileParser->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptPicFileParserHead;
		while ( ptTmp->ptNext )
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	  = ptPicFileParser;
		ptPicFileParser->ptNext = NULL;
	}

	return 0;
}

//����ShowDispOpr����
void ShowPicFileParser ( void )
{
	int i = 0;
	PT_PicFileParser ptTmp = g_ptPicFileParserHead;

	while ( ptTmp )
	{
		printf ( "%02d %s\n", i++, ptTmp->name );
		ptTmp = ptTmp->ptNext;
	}
}

//�������ֵõ���Ӧ�Ľڵ�
PT_PicFileParser GetPicFileParser ( char* pcName )
{
	PT_PicFileParser ptTmp = g_ptPicFileParserHead;

	while ( ptTmp )
	{
		if ( strcmp ( ptTmp->name, pcName ) == 0 )
		{
			return ptTmp;
		}
		ptTmp = ptTmp->ptNext;
	}
	return NULL;
}

PT_PicFileParser isSupport ( unsigned char* pucFileHead )
{
	PT_PicFileParser ptTmp = g_ptPicFileParserHead;
	
	while ( ptTmp )
	{
		if ( ptTmp->isSupport ( pucFileHead ) == 1 )
		{
			return ptTmp;
		}
		ptTmp = ptTmp->ptNext;
	}
	return NULL;


}

/**********************************************************************
 * �������ƣ� GetPixelDatasFormIcon
 * ���������� ȡ��BMP��ʽ��ͼ���ļ��е���������
 * ��������� strFileName - BMP��ʽ��ͼ���ļ���,��λ�� ICON_PATH Ŀ¼��
 * ��������� ptPhotoDesc - �ں���������,����ռ�Ŀռ���ͨ��malloc�����,
 *                          ����ʱ��Ҫ��FreePixelDatasForIcon���ͷ�
********************************************************************/

int GetPixelDatasFormIcon ( char* strFileName, PT_PhotoDesc ptPhotoDesc )
{
	T_MapFile tMapFile;
	PT_PicFileParser ptTargetFileParser;
    int iXres,iYres,iBpp;

	int iError;
	//�����ļ������ļ�
	//ͼ���ļ�������         ICON_PATH"/mnt/Icon/" Ŀ¼��
	snprintf ( tMapFile.FileName,128,"%s%s", ICON_PATH,strFileName );

	//��Ŀ���ļ� ��ʹ��mmapӳ�䵽�ڴ���
	iError = MapFile ( &tMapFile );
	if ( iError !=0 )
	{
		DBG_PRINTF ( "MapFile %s error!\n", strFileName );
		return -1;
	}

	//�õ�֧�ִ��ļ���ͼƬ����ڵ�
	ptTargetFileParser = isSupport ( tMapFile.pucFileMapMem );
	if ( ptTargetFileParser == NULL )
	{
		DBG_PRINTF ( "can't support :%s\n ",tMapFile.FileName );
		unMapFile(&tMapFile);//����ʱ ���ͷ�mmap���� ��������ڴ�й©
		return -1;
	}

    //�����Ļ�ֱ���
    GetDispResolution ( &iXres,&iYres,&iBpp ); //��ȡ�ֱ���

	//ʹ�ø�ͼƬ����ڵ�õ�ͼƬ����
	iError = ptTargetFileParser->GetPixelDatas(tMapFile.pucFileMapMem , ptPhotoDesc ,iBpp );
	if (iError)
	{
		DBG_PRINTF("GetPixelDatas for %s error!\n", tMapFile.FileName);
		unMapFile(&tMapFile);//����ʱ ���ͷ�mmap���� ��������ڴ�й©
		return -1;
	}
       
	unMapFile(&tMapFile);//��ɴ�������֮�� �ͷ�mmap�ռ�

    return 0;

}

void FreePixelDatasForIcon(PT_PhotoDesc ptPhotoDatas)
{
	//if(ptPhotoDatas->aucPhotoData)
		free(ptPhotoDatas->aucPhotoData); 

	//g_tBMPFileParser.FreePixelDatas(ptPhotoDatas);
    
}


int PicFileParserInit ( void )
{
	int iError;

	iError = BMPInit();

	return iError;
}


