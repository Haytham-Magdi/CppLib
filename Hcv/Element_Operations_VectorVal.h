#pragma once

#include <Lib\Cpp\Common\commonLib.h>
//#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <Lib\Hcv\funcs1.h>
//#include <vector>
//#include <Lib\Hcv\Image.h>

#include <Lib\Hcv\VectorVal.h>
#include <Lib\Hcv\Element_Operations.h>


namespace Hcv
{
	using namespace Hcpl;

	namespace Element_Operations
	{

		//template<> void Copy_ByPtr<VectorVal>(VectorVal * a_pDest, VectorVal * a_pSrc);

		template<class T, const int N>
		//template<>
		float CalcMagSqr_ByPtr(VectorVal<T, N> * a_pArg)
			//float CalcMagSqr_ByPtr(VectorVal<float, 3> * a_pArg)
		{
			float sum = 0;
			for (int i = 0; i < a_pArg->GetNofDims(); i++)
			{
				sum += (float)Sqr(a_pArg->Vals[i]);
			}

			return sum;
		}

		
		template<class T, const int N>
		void Add_ByPtr(VectorVal * a_pInp1, VectorVal * a_pInp2, VectorVal * a_pOut);

		
		template<class T, const int N>
		void Subtract_ByPtr(VectorVal * a_pInp1, VectorVal * a_pInp2, VectorVal * a_pOut);

		
		template<class T, const int N>
		void DivideByNum_ByPtr(VectorVal * a_pInp, float a_num, VectorVal * a_pOut);
		
		
		template<class T, const int N>
		void MultiplyByNum_ByPtr(VectorVal * a_pInp, float a_num, VectorVal * a_pOut);

		template<> void SetToZero_ByPtr(VectorVal * a_pArg);

	};
}