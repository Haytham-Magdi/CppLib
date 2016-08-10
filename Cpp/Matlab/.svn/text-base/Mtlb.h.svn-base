#pragma once

#pragma inline_depth( 255 )


#include <Lib\Cpp\Common\common.h>
#include <engine.h>



namespace Hcpl
{
	
	template<class T>
		inline mxClassID GetMxClassID(T a)
	{
		return mxUNKNOWN_CLASS;
	}
	
	
	inline mxClassID GetMxClassID(Uint8 a)
	{
		return mxUINT8_CLASS;
	}
	
/*	inline mxClassID GetMxClassID(Int16 a)
	{
		return mxINT16_CLASS;
	}*/
	
	inline mxClassID GetMxClassID(Int32 a)
	{
		return mxINT32_CLASS;
	}
	
	inline mxClassID GetMxClassID(Double a)
	{
		return mxDOUBLE_CLASS;
	}
	
	
	
	
	/////////////////////////////////////////////////////
	
	class Mtlb
	{
	public:
		static void StartEngine(void);
		static Engine * GetEngine(void);
		static void NewVarName(char * ao_buff, int a_nMaxBufSiz);
		static void DeleteVarName(char * a_sName);

		
	
	protected:
		static Engine * m_pEng;
		
		static char m_cNameChr;
		static int m_nNameNum; 

		
	};
	
	
}