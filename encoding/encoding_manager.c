#include <config.h>
#include <encoding_manager.h>
#include <string.h>
#include <stdlib.h>

static PT_EncodingOpr g_ptEncodingOprHead;

//新增节点到EncodingOpr链表中
int RegisterEncodingOpr ( PT_EncodingOpr ptEncodingOpr )
{
	PT_EncodingOpr ptTmp;

	if ( !g_ptEncodingOprHead )
	{
		g_ptEncodingOprHead   = ptEncodingOpr;
		ptEncodingOpr->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptEncodingOprHead;
		while ( ptTmp->ptNext )
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	      = ptEncodingOpr;
		ptEncodingOpr->ptNext = NULL;
	}

	return 0;
}


//遍历EncodingOpr链表
void ShowEncodingOpr ( void )
{
	int i = 0;
	PT_EncodingOpr ptTmp = g_ptEncodingOprHead;

	while ( ptTmp )
	{
		printf ( "%02d %s\n", i++, ptTmp->name );
		ptTmp = ptTmp->ptNext;
	}
}

//根据传入的的文本文件内容确定支持的编码格式
PT_EncodingOpr SelectEncodingOprForFile ( unsigned char* pucFileBufHead )
{
	PT_EncodingOpr ptTmp = g_ptEncodingOprHead; //得到EncodingOpr链表头

	while ( ptTmp )
	{
		if ( ptTmp->isSupport ( pucFileBufHead ) )
		{
			return ptTmp;
		}
		else
		{
			ptTmp = ptTmp->ptNext;
		}
	}
	return NULL;
}
/**********************************************************************
 * 函数名称： GetEncodingOpr
 * 功能描述： 从encoding处理链表中根据名字找出指定模块
 * 输入参数： pucName - 模块名字
 * 输出参数： 无
 * 返 回 值： ptTmp--可用的处理模块
 *            NULL--未找到可用结点
 ***********************************************************************/
PT_EncodingOpr GetEncodingOpr ( unsigned char* pucName )
{
	PT_EncodingOpr ptTmp = g_ptEncodingOprHead; //得到EncodingOpr链表头

	while ( ptTmp )
	{
		if ( strcmp ( ptTmp->name,pucName ) == 0 )
		{
			return ptTmp;
		}
		else
		{
			ptTmp = ptTmp->ptNext;
		}
	}
	return NULL;
}


//
int AddFontOprForEncoding ( PT_EncodingOpr ptEncodingOpr, PT_FontOpr ptFontOpr )
{
	PT_FontOpr ptFontOprCpy;

	if ( !ptEncodingOpr || !ptFontOpr )
	{
		return -1;
	}
	else
	{
		ptFontOprCpy = malloc ( sizeof ( T_FontOpr ) );
		if ( !ptFontOprCpy )
		{
			return -1;
		}
		else
		{
			memcpy ( ptFontOprCpy, ptFontOpr, sizeof ( T_FontOpr ) );
			ptFontOprCpy->ptNext = ptEncodingOpr->ptFontOprSupportedHead;
			ptEncodingOpr->ptFontOprSupportedHead = ptFontOprCpy;
			return 0;
		}
	}
}

//
int DelFontOprFrmEncoding ( PT_EncodingOpr ptEncodingOpr, PT_FontOpr ptFontOpr )
{
	PT_FontOpr ptTmp;
	PT_FontOpr ptPre;

	if ( !ptEncodingOpr || !ptFontOpr )
	{
		return -1;
	}
	else
	{
		ptTmp = ptEncodingOpr->ptFontOprSupportedHead;
		if ( strcmp ( ptTmp->name, ptFontOpr->name ) == 0 )
		{
			/* 删除头节点 */
			ptEncodingOpr->ptFontOprSupportedHead = ptTmp->ptNext;
			free ( ptTmp );
			return 0;
		}

		ptPre = ptEncodingOpr->ptFontOprSupportedHead;
		ptTmp = ptPre->ptNext;
		while ( ptTmp )
		{
			if ( strcmp ( ptTmp->name, ptFontOpr->name ) == 0 )
			{
				/* 从链表里取出、释放 */
				ptPre->ptNext = ptTmp->ptNext;
				free ( ptTmp );
				return 0;
			}
			else
			{
				ptPre = ptTmp;
				ptTmp = ptTmp->ptNext;
			}
		}

		return -1;
	}
}
/**********************************************************************
 * 函数名称： GetCodeFrmBuf
 * 功能描述： 从字符串中取出编码值,这是一个简单的版本, 只能处理ASCII/GBK字符串
 * 输入参数： pucBufStart - 字符串起始位置
 *            pucBufEnd   - 字符串结束位置(这个位置的字符不处理)
 * 输出参数： pdwCode     - 所取出的编码值存在这里
 * 返 回 值： 0 - 字符串处理完毕,没有获得编码值
 *            1 - 得到一个ASCII码
 *            2 - 得过一个GBK码
 ***********************************************************************/
int GetCodeFrmBuf ( unsigned char* pucBufStart, unsigned char* pucBufEnd, unsigned int* pdwCode )
{
	PT_EncodingOpr ptEncodingOpr;


	//获得支持ascii的编码处理模块
	ptEncodingOpr = GetEncodingOpr ( "ascii" );

	//如果不能获取到处理模块
	if ( ptEncodingOpr==NULL )
	{
		DBG_PRINTF ( "SelectEncodingOprForFile error..\r\n" );
		return 0;
	}

	return ptEncodingOpr->GetCodeFrmBuf ( pucBufStart,pucBufEnd,pdwCode );
}

/**********************************************************************
 * 函数名称： EncodingInit
 * 功能描述： 调用各种编码方式的初始化函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 ***********************************************************************/
int EncodingInit ( void )
{
	int iError;

	iError = AsciiEncodingInit();
	if ( iError )
	{
		DBG_PRINTF ( "AsciiEncodingInit error!\n" );
		return -1;
	}

	iError = Utf16leEncodingInit();
	if ( iError )
	{
		DBG_PRINTF ( "Utf16leEncodingInit error!\n" );
		return -1;
	}

	iError = Utf16beEncodingInit();
	if ( iError )
	{
		DBG_PRINTF ( "Utf16beEncodingInit error!\n" );
		return -1;
	}

	iError = Utf8EncodingInit();
	if ( iError )
	{
		DBG_PRINTF ( "Utf8EncodingInit error!\n" );
		return -1;
	}

	return 0;
}

