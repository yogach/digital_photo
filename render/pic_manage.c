#include <config.h>
#include <pic_manager.h>
#include <file.h>
#include <string.h>
#include <disp_manager.h>
#include <stdlib.h>


static PT_PicFileParser g_ptPicFileParserHead;
extern T_PicFileParser g_tBMPFileParser;


//向DispOpr新增节点
int RegisterPicFileParser ( PT_PicFileParser ptPicFileParser )
{
	PT_PicFileParser ptTmp;

	if ( !g_ptPicFileParserHead ) //如果是头节点
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

//遍历ShowDispOpr链表
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

//根据名字得到对应的节点
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




int GetPixelDatasForIcon ( char* strFileName, PT_PhotoDesc ptPhotoDesc )
{
	T_MapFile tMapFile;
	PT_PicFileParser ptTargetFileParser;
    int iXres,iYres,iBpp;

	int iError;
	//根据文件名打开文件
	//图标文件放置在"/etc/digitpic/icons"下
	snprintf ( tMapFile.FileName,128,"%s%s", ICON_PATH,strFileName );

	//mmap文件
	iError = MapFile ( &tMapFile );
	if ( iError !=0 )
	{
		DBG_PRINTF ( "MapFile %s error!\n", strFileName );
		return -1;
	}

	//得到支持此文件的图片处理节点
	ptTargetFileParser = isSupport ( tMapFile.pucFileMapMem );
	if ( ptTargetFileParser == NULL )
	{
		DBG_PRINTF ( "can't support :%s ",tMapFile.FileName );
		return -1;

	}

	

    //获得屏幕分辨率
    GetDispResolution ( &iXres,&iYres,&iBpp ); //获取分辨率

	//使用该图片处理节点得到图片数据
	iError = ptTargetFileParser->GetPixelDatas(tMapFile.pucFileMapMem , ptPhotoDesc ,iBpp );
	if (iError)
	{
		DBG_PRINTF("GetPixelDatas for %s error!\n", tMapFile.FileName);
		return -1;
	}
       
/*    
    iError = g_tBMPFileParser.isSupport(tMapFile.pucFileMapMem);
	if (iError == 0)
	{
		DBG_PRINTF("%s is not bmp file\n", strFileName);
		return -1;
	}

	GetDispResolution(&iXres, &iYres, &iBpp);
	ptPhotoDesc->iBpp = iBpp;
	iError = g_tBMPFileParser.GetPixelDatas(tMapFile.pucFileMapMem, ptPhotoDesc , iBpp);
	if (iError)
	{
		DBG_PRINTF("GetPixelDatas for %s error!\n", strFileName);
		return -1;
	}
	*/


    return 0;

}

void FreePixelDatasForIcon(PT_PhotoDesc ptPhotoDatas)
{
	//if(ptPhotoDatas->aucPhotoData)
		//free(ptPhotoDatas->aucPhotoData); 

	g_tBMPFileParser.FreePixelDatas(ptPhotoDatas);
    
}


int PicFileParserInit ( void )
{
	int iError;

	iError = BMPInit();

	return iError;
}


