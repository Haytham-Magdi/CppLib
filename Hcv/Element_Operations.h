#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <Lib\Hcv\funcs1.h>
#include <vector>
#include <Lib\Hcv\Image.h>



namespace Hcv
{
	using namespace Hcpl;

	namespace Element_Operations
	{

		template<class T>
		void CopyByPtr(T * a_pDest, T * a_pSrc)
		{
			memcpy(a_pDest, a_pSrc, sizeof(T));
		}

		template<class T>
		float CalcMagSqr(T * a_pArg)
		{
			Sqr<T>(*a_pArg);
		}

		template<class T>
		float CalcMag(T * a_pArg)
		{
			return sqrt(CalcMagSqr(a_pArg));
		}





	};
}