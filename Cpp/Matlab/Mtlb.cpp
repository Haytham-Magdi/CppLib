
#include <Lib\Cpp\Matlab\Mtlb.h>
#include <string.h>
#include <stdio.h>

using namespace Hcpl;


Engine * Mtlb::m_pEng = NULL;
char Mtlb::m_cNameChr = 'a';
int Mtlb::m_nNameNum = 1; 


Engine * Mtlb::GetEngine(void)
{
	if(NULL == m_pEng)
	{
		/// H TODO
		m_pEng = engOpen(NULL);
	}

	return m_pEng;
}


void Mtlb::StartEngine(void)
{
	Mtlb::GetEngine();
}


void Mtlb::NewVarName(char * ao_buff, int a_nMaxBufSiz)
{
		/// H TODO

	ao_buff[0] = m_cNameChr;
	ao_buff[1] = 0;

	char sTemp[50];
	sprintf(sTemp, "%d", m_nNameNum);

	strncat(&ao_buff[1], sTemp, a_nMaxBufSiz - 2);

	m_cNameChr++;
	if(m_cNameChr > 'z')
	{
		m_cNameChr = 'a';
		m_nNameNum++;
	}
}

void Mtlb::DeleteVarName(char * a_sName)
{
}
