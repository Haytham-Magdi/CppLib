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

		//template<> void Copy_ByPtr<VectorVal>(VectorVal<T_Elm, N_Dims> * a_pDest, VectorVal<T_Elm, N_Dims> * a_pSrc);

		//template<>
		//float CalcMagSqr_ByPtr(VectorVal<T_Elm, N_Dims> * a_pArg)


		//template<class T, int const V_NofChannels>
		//using VectorValImageAcc = ImageAccessor<T, VectorVal<T, V_NofChannels>, V_NofChannels>;

		//template<class T, int const V_NofChannels>
		//using VectorValImageAcc = ImageAccessor<T, VectorVal<T, V_NofChannels>, V_NofChannels>;



		template<> float CalcMagSqr_ByPtr(VectorVal<T_Elm, N_Dims> * a_pArg);


		template<>
		void Add_ByPtr(VectorVal<T_Elm, N_Dims> * a_pInp1, VectorVal<T_Elm, N_Dims> * a_pInp2, VectorVal<T_Elm, N_Dims> * a_pOut)
		{
			for (int i = 0; i < a_pArg->GetNofDims(); i++)
			{
				a_pOut->Vals[i] = a_pInp1->Vals[i] + a_pInp2->Vals[i];
			}
		}


		template<>
		void Subtract_ByPtr(VectorVal<T_Elm, N_Dims> * a_pInp1, VectorVal<T_Elm, N_Dims> * a_pInp2, VectorVal<T_Elm, N_Dims> * a_pOut)
		{
			for (int i = 0; i < a_pArg->GetNofDims(); i++)
			{
				a_pOut->Vals[i] = a_pInp1->Vals[i] - a_pInp2->Vals[i];
			}
		}


		template<>
		void DivideByNum_ByPtr(VectorVal<T_Elm, N_Dims> * a_pInp, float a_num, VectorVal<T_Elm, N_Dims> * a_pOut)
		{
			for (int i = 0; i < a_pArg->GetNofDims(); i++)
			{
				a_pOut->Vals[i] = a_pInp->Vals[i] / a_num;
			}
		}


		template<>
		void MultiplyByNum_ByPtr(VectorVal<T_Elm, N_Dims> * a_pInp, float a_num, VectorVal<T_Elm, N_Dims> * a_pOut)
		{
			for (int i = 0; i < a_pArg->GetNofDims(); i++)
			{
				a_pOut->Vals[i] = a_pInp->Vals[i] * a_num;
			}
		}



		template<>
		void SetToZero_ByPtr(VectorVal<T_Elm, N_Dims> * a_pArg)
		{
			for (int i = 0; i < a_pArg->GetNofDims(); i++)
			{
				a_pArg->Vals[i] = 0;
			}
		}


	};
}