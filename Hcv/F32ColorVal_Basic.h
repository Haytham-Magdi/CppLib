#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <Lib\Hcv\funcs1.h>
#include <vector>
#include <Lib\Hcv\Image.h>

#include <Lib\Hcv\Type_Basic.h>


namespace Hcv
{
	using namespace Hcpl;

	//namespace F32ColorVal_Basic
	namespace Type_Basic
	{

		template<> void CopyByPtr<F32ColorVal>(F32ColorVal * a_pDest, F32ColorVal * a_pSrc);
		void CopyByPtr(F32ColorVal * a_pDest, F32ColorVal * a_pSrc)
		{
			memcpy(a_pDest, a_pSrc, sizeof(F32ColorVal));
		}

		template<> float CalcMagSqr(F32ColorVal * a_pArg);
		float CalcMagSqr(F32ColorVal * a_pArg)
		{
			return a_pArg->CalcMagSqr();
		}


	};
}