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

#define T_Elm float
#define N_Dims 2

#include <Lib\Hcv\Element_Operations_VectorVal_CoreFile.h>

#undef T_Elm
#undef N_Dims


#define T_Elm float
#define N_Dims 3

#include <Lib\Hcv\Element_Operations_VectorVal_CoreFile.h>

#undef T_Elm
#undef N_Dims


#define T_Elm float
#define N_Dims 4

#include <Lib\Hcv\Element_Operations_VectorVal_CoreFile.h>

#undef T_Elm
#undef N_Dims


		//template<> void Copy_ByPtr<VectorVal>(VectorVal * a_pDest, VectorVal * a_pSrc);

		//template<>
		//float CalcMagSqr_ByPtr(VectorVal<T, N> * a_pArg)


		//template<class T, int const V_NofChannels>
		//using VectorValImageAcc = ImageAccessor<T, VectorVal<T, V_NofChannels>, V_NofChannels>;

		//template<class T, int const V_NofChannels>
		//using VectorValImageAcc = ImageAccessor<T, VectorVal<T, V_NofChannels>, V_NofChannels>;



		//template<class T2, const int N>
		////template<>
		//template
		////template<class T2, const int N>
		//float CalcMagSqr_ByPtr<VectorVal<T2, N>>(VectorVal<T2, N> * a_pArg)
		//{
		//	float sum = 0;
		//	for (int i = 0; i < a_pArg->GetNofDims(); i++)
		//	{
		//		sum += (float)Sqr(a_pArg->Vals[i]);
		//	}

		//	return sum;
		//}


		//template<class T, const int N>
		//void Add_ByPtr(VectorVal<T, N> * a_pInp1, VectorVal<T, N> * a_pInp2, VectorVal<T, N> * a_pOut)
		//{
		//	for (int i = 0; i < a_pArg->GetNofDims(); i++)
		//	{
		//		a_pOut->Vals[i] = a_pInp1->Vals[i] + a_pInp2->Vals[i];
		//	}
		//}


		//template<class T, const int N>
		//void Subtract_ByPtr(VectorVal<T, N> * a_pInp1, VectorVal<T, N> * a_pInp2, VectorVal<T, N> * a_pOut)
		//{
		//	for (int i = 0; i < a_pArg->GetNofDims(); i++)
		//	{
		//		a_pOut->Vals[i] = a_pInp1->Vals[i] - a_pInp2->Vals[i];
		//	}
		//}


		//template<class T, const int N>
		//void DivideByNum_ByPtr(VectorVal<T, N> * a_pInp, float a_num, VectorVal<T, N> * a_pOut)
		//{
		//	for (int i = 0; i < a_pArg->GetNofDims(); i++)
		//	{
		//		a_pOut->Vals[i] = a_pInp->Vals[i] / a_num;
		//	}
		//}


		//template<class T, const int N>
		//void MultiplyByNum_ByPtr(VectorVal<T, N> * a_pInp, float a_num, VectorVal<T, N> * a_pOut)
		//{
		//	for (int i = 0; i < a_pArg->GetNofDims(); i++)
		//	{
		//		a_pOut->Vals[i] = a_pInp->Vals[i] * a_num;
		//	}
		//}



		//template<class T, const int N>
		//void SetToZero_ByPtr(VectorVal<T, N> * a_pArg)
		//{
		//	for (int i = 0; i < a_pArg->GetNofDims(); i++)
		//	{
		//		a_pArg->Vals[i] = 0;
		//	}
		//}


	};
}