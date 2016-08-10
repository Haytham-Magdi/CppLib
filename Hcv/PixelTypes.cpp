#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\PixelTypes.h>



namespace Hcv
{
	PixelTypeInfo g_Uint8PTI = 
	{
		IPL_DEPTH_8U
	};

	PixelTypeInfo g_Int16PTI = 
	{
		IPL_DEPTH_16S
	};

	PixelTypeInfo g_Int32PTI = 
	{
		IPL_DEPTH_32S
	};

	PixelTypeInfo g_FloatPTI = 
	{
		IPL_DEPTH_32F
	};

}