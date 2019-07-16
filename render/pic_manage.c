#include <config.h>
#include <pic_manager.h>

static PT_PicFileParser g_ptPicFileParserHead;

//��DispOpr�����ڵ�
int RegisterPicFileParser(PT_PicFileParser ptPicFileParser)
{
	PT_PicFileParser ptTmp;

	if (!g_ptPicFileParserHead)//�����ͷ�ڵ�
	{
		g_ptPicFileParserHead   = ptPicFileParser;
		ptPicFileParser->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptPicFileParserHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	  = ptPicFileParser;
		ptPicFileParser->ptNext = NULL;
	}

	return 0;
}

//����ShowDispOpr����
void ShowPicFileParser(void)
{
	int i = 0;
	PT_PicFileParser ptTmp = g_ptPicFileParserHead;

	while (ptTmp)
	{
		printf("%02d %s\n", i++, ptTmp->name);
		ptTmp = ptTmp->ptNext;
	}
}

//�������ֵõ���Ӧ�Ľڵ�
PT_PicFileParser GetPicFileParser(char *pcName)
{
	PT_PicFileParser ptTmp = g_ptPicFileParserHead;
	
	while (ptTmp)
	{
		if (strcmp(ptTmp->name, pcName) == 0)
		{
			return ptTmp;
		}
		ptTmp = ptTmp->ptNext;
	}
	return NULL;
}

int PicFileParserInit(void)
{
	int iError;
	
	iError = BMPInit();

	return iError;
}


