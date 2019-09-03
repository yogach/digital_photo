#include <config.h>
#include <fonts_manager.h>
#include <string.h>

static PT_FontOpr g_ptFontOprHead = NULL;
static unsigned int g_FontSize;

//将新节点放入FontOpr结构体
int RegisterFontOpr ( PT_FontOpr ptFontOpr )
{
	PT_FontOpr ptTmp;

	if ( !g_ptFontOprHead )
	{
		g_ptFontOprHead   = ptFontOpr;
		ptFontOpr->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptFontOprHead;
		while ( ptTmp->ptNext )
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext     = ptFontOpr;
		ptFontOpr->ptNext = NULL;
	}

	return 0;
}

//遍历FontOpr链表
void ShowFontOpr ( void )
{
	int i = 0;
	PT_FontOpr ptTmp = g_ptFontOprHead;

	while ( ptTmp )
	{
		printf ( "%02d %s\n", i++, ptTmp->name );
		ptTmp = ptTmp->ptNext;
	}
}

//从FontOpr链表中取出对应名字的节点
PT_FontOpr GetFontOpr ( char* pcName )
{
	PT_FontOpr ptTmp = g_ptFontOprHead;

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

/**********************************************************************
 * 函数名称： SetFontSize
 * 功能描述： 设置字符的尺寸(单位:色素)
 * 输入参数： dwFontSize - 字符的尺寸(单位:色素)
 * 输出参数： 无
 * 返 回 值： 无
 ***********************************************************************/
int SetFontSize ( unsigned int dwFontSize )
{
	PT_FontOpr ptTmp = g_ptFontOprHead;

	g_FontSize = dwFontSize;

	
	while ( ptTmp )
	{
		if ( ptTmp->SetFontSize)
		{
			ptTmp->SetFontSize(dwFontSize);
		}
		ptTmp = ptTmp->ptNext;
	}
	return NULL;

}

/**********************************************************************
 * 函数名称： GetFontSize
 * 功能描述： 获得字符的尺寸(单位:色素)
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 字符尺寸
 ***********************************************************************/
unsigned int GetFontSize(void)
{
   return g_FontSize;

}


/**********************************************************************
 * 函数名称： SetFontsDetail
 * 功能描述： 设置字体模块的信息,比如指定字库文件,指定字符尺寸
 * 输入参数： pcFontsName - 要设置的字体模块的名字
 *            pcFontsFile - 字库文件
 *            dwFontSize  - 字符的尺寸(单位:色素), 程序运行时可用SetFontSize修改
 * 输出参数： 无
 * 返 回 值： 0      - 成功
 *            其他值 - 失败
 ***********************************************************************/
int SetFontsDetail ( char* pcFontsName,char* pcFontFile,int dwFontSize )
{
	PT_FontOpr ptFontOpr;
	int iError = 0;

	ptFontOpr = GetFontOpr ( pcFontsName );
	if ( ptFontOpr == NULL )
	{
		DBG_PRINTF ( "GetFontOpr error!\r\n" );
		return -1;
	}

	iError =  ptFontOpr->FontInit ( pcFontFile,dwFontSize );

	g_FontSize = dwFontSize;
	
	return iError;

}

int FontsInit ( void )
{
	int iError;
	/*
	iError = ASCIIInit();
	if (iError)
	{
		DBG_PRINTF("ASCIIInit error!\n");
		return -1;
	}

	iError = GBKInit();
	if (iError)
	{
		DBG_PRINTF("GBKInit error!\n");
		return -1;
	}
	*/
	iError = FreeTypeInit();
	if ( iError )
	{
		DBG_PRINTF ( "FreeTypeInit error!\n" );
		return -1;
	}

	return 0;
}

