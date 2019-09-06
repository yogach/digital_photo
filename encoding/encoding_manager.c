#include <config.h>
#include <encoding_manager.h>
#include <string.h>
#include <stdlib.h>

static PT_EncodingOpr g_ptEncodingOprHead;

//�����ڵ㵽EncodingOpr������
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


//����EncodingOpr����
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

//���ݴ���ĵ��ı��ļ�����ȷ��֧�ֵı����ʽ
PT_EncodingOpr SelectEncodingOprForFile ( unsigned char* pucFileBufHead )
{
	PT_EncodingOpr ptTmp = g_ptEncodingOprHead; //�õ�EncodingOpr����ͷ

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
 * �������ƣ� GetEncodingOpr
 * ���������� ��encoding���������и��������ҳ�ָ��ģ��
 * ��������� pucName - ģ������
 * ��������� ��
 * �� �� ֵ�� ptTmp--���õĴ���ģ��
 *            NULL--δ�ҵ����ý��
 ***********************************************************************/
PT_EncodingOpr GetEncodingOpr ( unsigned char* pucName )
{
	PT_EncodingOpr ptTmp = g_ptEncodingOprHead; //�õ�EncodingOpr����ͷ

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
			/* ɾ��ͷ�ڵ� */
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
				/* ��������ȡ�����ͷ� */
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
 * �������ƣ� GetCodeFrmBuf
 * ���������� ���ַ�����ȡ������ֵ,����һ���򵥵İ汾, ֻ�ܴ���ASCII/GBK�ַ���
 * ��������� pucBufStart - �ַ�����ʼλ��
 *            pucBufEnd   - �ַ�������λ��(���λ�õ��ַ�������)
 * ��������� pdwCode     - ��ȡ���ı���ֵ��������
 * �� �� ֵ�� 0 - �ַ����������,û�л�ñ���ֵ
 *            1 - �õ�һ��ASCII��
 *            2 - �ù�һ��GBK��
 ***********************************************************************/
int GetCodeFrmBuf ( unsigned char* pucBufStart, unsigned char* pucBufEnd, unsigned int* pdwCode )
{
	PT_EncodingOpr ptEncodingOpr;


	//���֧��ascii�ı��봦��ģ��
	ptEncodingOpr = GetEncodingOpr ( "ascii" );

	//������ܻ�ȡ������ģ��
	if ( ptEncodingOpr==NULL )
	{
		DBG_PRINTF ( "SelectEncodingOprForFile error..\r\n" );
		return 0;
	}

	return ptEncodingOpr->GetCodeFrmBuf ( pucBufStart,pucBufEnd,pdwCode );
}

/**********************************************************************
 * �������ƣ� EncodingInit
 * ���������� ���ø��ֱ��뷽ʽ�ĳ�ʼ������
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
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

