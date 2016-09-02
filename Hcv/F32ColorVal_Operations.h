#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <Lib\Hcv\funcs1.h>
#include <vector>
#include <Lib\Hcv\Image.h>

#include <Lib\Hcv\Element_Operations.h>


namespace Hcv
{
	using namespace Hcpl;

	namespace Element_Operations
	{

		template<> void Copy_ByPtr<F32ColorVal>(F32ColorVal * a_pDest, F32ColorVal * a_pSrc);

		template<> float CalcMagSqr_ByPtr(F32ColorVal * a_pArg);

		template<> void Add_ByPtr(F32ColorVal * a_pInp1, F32ColorVal * a_pInp2, F32ColorVal * a_pOut);

		template<> void Subtract_ByPtr(F32ColorVal * a_pInp1, F32ColorVal * a_pInp2, F32ColorVal * a_pOut);

		template<> void DivideByNum_ByPtr(F32ColorVal * a_pInp, float a_num, F32ColorVal * a_pOut);
		
		template<> void MultiplyByNum_ByPtr(F32ColorVal * a_pInp, float a_num, F32ColorVal * a_pOut);

		template<> void SetToZero_ByPtr(F32ColorVal * a_pArg);

	};
}