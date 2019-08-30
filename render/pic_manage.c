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

/**********************************************************************
 * 函数名称： GetPixelDatasFormIcon
 * 功能描述： 取出BMP格式的图标文件中的象素数据
 * 输入参数： strFileName - BMP格式的图标文件名,它位于 ICON_PATH 目录下
 * 输出参数： ptPhotoDesc - 内含象素数据,它所占的空间是通过malloc分配的,
 *                          不用时需要用FreePixelDatasForIcon来释放
********************************************************************/

int GetPixelDatasFormIcon ( char* strFileName, PT_PhotoDesc ptPhotoDesc )
{
	T_MapFile tMapFile;
	PT_PicFileParser ptTargetFileParser;
    int iXres,iYres,iBpp;

	int iError;
	//根据文件名打开文件
	//图标文件放置在         ICON_PATH"/mnt/Icon/" 目录下
	snprintf ( tMapFile.FileName,128,"%s%s", ICON_PATH,strFileName );

	//打开目标文件 并使用mmap映射到内存上
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
		DBG_PRINTF ( "can't support :%s\n ",tMapFile.FileName );
		unMapFile(&tMapFile);//出错时 需释放mmap数据 以免造成内存泄漏
		return -1;
	}

    //获得屏幕分辨率
    GetDispResolution ( &iXres,&iYres,&iBpp ); //获取分辨率

	//使用该图片处理节点得到图片数据
	iError = ptTargetFileParser->GetPixelDatas(tMapFile.pucFileMapMem , ptPhotoDesc ,iBpp );
	if (iError)
	{
		DBG_PRINTF("GetPixelDatas for %s error!\n", tMapFile.FileName);
		unMapFile(&tMapFile);//出错时 需释放mmap数据 以免造成内存泄漏
		return -1;
	}
       
	unMapFile(&tMapFile);//完成处理任务之后 释放mmap空间

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


