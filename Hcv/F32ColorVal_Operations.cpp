#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <Lib\Hcv\funcs1.h>
#include <vector>
#include <Lib\Hcv\Image.h>

#include <Lib\Hcv\Element_Operations.h>
#include <Lib\Hcv\F32ColorVal_Operations.h>


namespace Hcv
{
	using namespace Hcpl;

	//namespace F32ColorVal_Operations
	namespace Element_Operations
	{

		template<> void Copy_ByPtr<F32ColorVal>(F32ColorVal * a_pDest, F32ColorVal * a_pSrc);
		void Copy_ByPtr(F32ColorVal * a_pDest, F32ColorVal * a_pSrc)
		{
			memcpy(a_pDest, a_pSrc, sizeof(F32ColorVal));
		}

		template<> float CalcMagSqr_ByPtr(F32ColorVal * a_pArg);
		float CalcMagSqr(F32ColorVal * a_pArg)
		{
			return a_pArg->CalcMagSqr();
		}

		template<> void Add_ByPtr(F32ColorVal * a_pInp1, F32ColorVal * a_pInp2, F32ColorVal * a_pOut);
		void Add_ByPtr(F32ColorVal * a_pInp1, F32ColorVal * a_pInp2, F32ColorVal * a_pOut)
		{
			a_pOut->val0 = a_pInp1->val0 + a_pInp2->val0;
			a_pOut->val1 = a_pInp1->val1 + a_pInp2->val1;
			a_pOut->val2 = a_pInp1->val2 + a_pInp2->val2;
		}

		template<> void Subtract_ByPtr(F32ColorVal * a_pInp1, F32ColorVal * a_pInp2, F32ColorVal * a_pOut);
		void Subtract_ByPtr(F32ColorVal * a_pInp1, F32ColorVal * a_pInp2, F32ColorVal * a_pOut)
		{
			a_pOut->val0 = a_pInp1->val0 - a_pInp2->val0;
			a_pOut->val1 = a_pInp1->val1 - a_pInp2->val1;
			a_pOut->val2 = a_pInp1->val2 - a_pInp2->val2;
		}

		template<> void DivideSelfByNum_ByPtr(F32ColorVal * a_pArg, float a_num);
		void DivideSelfByNum_ByPtr(F32ColorVal * a_pArg, float a_num)
		{
			a_pArg->DividSelfBy(a_num);
		}


	};
}