#include <config.h>
#include <fonts_manager.h>
#include <string.h>

static PT_FontOpr g_ptFontOprHead = NULL;
static unsigned int g_FontSize;

//���½ڵ����FontOpr�ṹ��
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

//����FontOpr����
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

//��FontOpr������ȡ����Ӧ���ֵĽڵ�
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
 * �������ƣ� SetFontSize
 * ���������� �����ַ��ĳߴ�(��λ:ɫ��)
 * ��������� dwFontSize - �ַ��ĳߴ�(��λ:ɫ��)
 * ��������� ��
 * �� �� ֵ�� ��
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
 * �������ƣ� GetFontSize
 * ���������� ����ַ��ĳߴ�(��λ:ɫ��)
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� �ַ��ߴ�
 ***********************************************************************/
unsigned int GetFontSize(void)
{
   return g_FontSize;

}


/**********************************************************************
 * �������ƣ� SetFontsDetail
 * ���������� ��������ģ�����Ϣ,����ָ���ֿ��ļ�,ָ���ַ��ߴ�
 * ��������� pcFontsName - Ҫ���õ�����ģ�������
 *            pcFontsFile - �ֿ��ļ�
 *            dwFontSize  - �ַ��ĳߴ�(��λ:ɫ��), ��������ʱ����SetFontSize�޸�
 * ��������� ��
 * �� �� ֵ�� 0      - �ɹ�
 *            ����ֵ - ʧ��
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

