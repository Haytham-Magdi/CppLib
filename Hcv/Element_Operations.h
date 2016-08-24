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
		void Copy_ByPtr(T * a_pDest, T * a_pSrc)
		{
			memcpy(a_pDest, a_pSrc, sizeof(T));
		}

		template<class T>
		float CalcMagSqr_ByPtr(T * a_pArg)
		{
			Sqr<T>(*a_pArg);
		}

		template<class T>
		float CalcMag_ByPtr(T * a_pArg)
		{
			return sqrt((float)CalcMagSqr_ByPtr(a_pArg));
		}

		template<class T>
		void Add_ByPtr(T * a_pInp1, T * a_pInp2, T * a_pOut)
		{
			*a_pOut = *a_pInp1 + *a_pInp2;
		}

		template<class T>
		void Subtract_ByPtr(T * a_pInp1, T * a_pInp2, T * a_pOut)
		{
			*a_pOut = *a_pInp1 - *a_pInp2;
		}

		template<class T>
		void DivideSelfByNum_ByPtr(T * a_pArg, float a_num)
		{
			*a_pArg = *a_pArg / a_num;
		}

		template<class T>
		void SetToZero_ByPtr(T * a_pArg)
		{
			*a_pArg = 0;
		}

		template<class T>
		float CalcSqrt_ByPtr(T * a_pArg)
		{
			return sqrt((float)*a_pArg);
		}

		template<class T>
		float CalcStandev_ByPtr(T * a_pAvg, float a_avg_MagSqr)
		{
			float magSqr_Avg = CalcMagSqr_ByPtr(a_pAvg);

			float variance = fabs(a_avg_MagSqr - magSqr_Avg);
			float standev = sqrt(variance);

			return standev;
		}

		template<class T>
		float CalcConflict_ByPtr(T * a_pAvg, float a_avg_MagSqr)
		{
			float magSqr_Avg = CalcMagSqr_ByPtr(a_pAvg);

			float variance = fabs(a_avg_MagSqr - magSqr_Avg);
			float standev = sqrt(variance);

			return standev;
		}





	};
}