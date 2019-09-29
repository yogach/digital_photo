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

PT_PicFileParser isSupport ( PT_MapFile ptFileMap )
{
	PT_PicFileParser ptTmp = g_ptPicFileParserHead;
	
	while ( ptTmp )
	{
		if ( ptTmp->isSupport ( ptFileMap ) == 1 )
		{
			return ptTmp;
		}
		ptTmp = ptTmp->ptNext;
	}
	return NULL;

}


int PicFileParserInit ( void )
{
	int iError;

	iError = BMPInit();
	iError |=JPGParserInit();

	return iError;
}


