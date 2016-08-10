#pragma once

#include <Lib\Hcv\CvIncludes.h>



namespace Hcv
{
	typedef struct _PixelTypeInfo
	{
		int IplDepthID;		
	}PixelTypeInfo;


	extern PixelTypeInfo g_Uint8PTI;
	extern PixelTypeInfo g_Int16PTI;
	extern PixelTypeInfo g_Int32PTI;
	extern PixelTypeInfo g_FloatPTI;

	template<class T>
		inline PixelTypeInfo * GetPixTInfo(T a_dmy)
	{
		Hcpl_ASSERT(false);
		return NULL;
	}

	inline PixelTypeInfo * GetPixTInfo(Hcpl::Int16 a_dmy)		
	{
		return &g_Int16PTI;
	}

	inline PixelTypeInfo * GetPixTInfo(Hcpl::Uint8 a_dmy)		
	{
		return &g_Uint8PTI;
	}

	inline PixelTypeInfo * GetPixTInfo(Hcpl::Int32 a_dmy)		
	{
		return &g_Int32PTI;
	}

	inline PixelTypeInfo * GetPixTInfo(Hcpl::Float a_dmy)		
	{
		return &g_FloatPTI;
	}

#define HCV_DEPTH_ID(T)		(GetPixTInfo((T)0))->IplDepthID
}