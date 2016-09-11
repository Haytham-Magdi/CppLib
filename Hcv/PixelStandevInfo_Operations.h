#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <Lib\Hcv\PixelStandevInfo.h>
#include <vector>
#include <Lib\Hcv\Image.h>

#include <Lib\Hcv\Element_Operations.h>


namespace Hcv
{
	using namespace Hcpl;

	namespace Element_Operations
	{

		//template<> void Copy_ByPtr<PixelStandevInfo>(PixelStandevInfo * a_pDest, PixelStandevInfo * a_pSrc);

		//template<> void SetToZero_ByPtr(PixelStandevInfo * a_pArg);

	};
}