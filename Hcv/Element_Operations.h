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

		//template<class T>
		//void DivideByNum_ByPtr(T * a_pArg, float a_num)
		//{
		//	*a_pArg = *a_pArg / a_num;
		//}

		template<class T>
		void DivideByNum_ByPtr(T * a_pInp, float a_num, T * a_pOut)
		{
			*a_pOut = *a_pInp / a_num;
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

			float variance = a_avg_MagSqr - magSqr_Avg;
			float standev = sqrt(fabs(variance));

			return standev;
		}

		template<class T>
		float CalcConflict_ByPtr(T * a_pAvg_1, float a_avg_MagSqr_1,
			T * a_pAvg_2, float a_avg_MagSqr_2)
		{
			T avg_12;
			Add_ByPtr(a_pAvg_1, a_pAvg_2, &avg_12);
			DivideByNum_ByPtr(&avg_12, 2);

			float avg_MagSqr_12 = (a_avg_MagSqr_1 + a_avg_MagSqr_2) / 2;

			float magSqr_Avg_1 = CalcMagSqr_ByPtr(a_pAvg_1);
			float magSqr_Avg_2 = CalcMagSqr_ByPtr(a_pAvg_2);
			float magSqr_Avg_12 = CalcMagSqr_ByPtr(&avg_12);

			float standev_1 = CalcStandev_ByPtr(a_pAvg_1, a_avg_MagSqr_1);
			float standev_2 = CalcStandev_ByPtr(a_pAvg_2, a_avg_MagSqr_2);
			float standev_12 = CalcStandev_ByPtr(&avg_12, avg_MagSqr_12);

			float standev_MaxSide = (standev_1 > standev_2) ? standev_1 : standev_2;

			if (standev_12 > 20 && standev_12 > standev_MaxSide * 2)
			{
				return 255;
			}
			else
			{
				return 0;
			}
		}





	};
}