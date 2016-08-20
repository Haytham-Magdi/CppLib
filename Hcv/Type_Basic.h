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

	namespace Type_Basic
	{

		template<class T>
		void CopyByPtr(T * a_pDest, T * a_pSrc)
		{
			memcpy(a_pDest, a_pSrc, sizeof(T));
		}

		template<class T>
		float CalcMagSqr(T * a_pArg)
		{
			throw "Not Implemented.";
		}

		template<class T>
		float CalcMag(T * a_pArg)
		{
			return sqrt(CalcMagSqr(a_pArg));
		}





	};
}