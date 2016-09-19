
namespace Hcv
{
	using namespace Hcpl;

	namespace Element_Operations
	{

		//void Copy_ByPtr(VectorVal<T_Elm, N_Dims> * a_pDest, VectorVal<T_Elm, N_Dims> * a_pSrc)
		//{
		//	memcpy(a_pDest, a_pSrc, sizeof(VectorVal));
		//}

		float CalcMagSqr_ByPtr(VectorVal<T_Elm, N_Dims> * a_pArg)
		{
			float sum = 0;
			for (int i = 0; i < a_pArg->GetNofDims(); i++)
			{
				sum += Sqr(a_pArg->Vals[i]);
			}

			return sum;
		}

		void Add_ByPtr(VectorVal<T_Elm, N_Dims> * a_pInp1, VectorVal<T_Elm, N_Dims> * a_pInp2, VectorVal<T_Elm, N_Dims> * a_pOut)
		{
			for (int i = 0; i < a_pArg->GetNofDims(); i++)
			{
				a_pOut->Vals[i] = a_pInp1->Vals[i] + a_pInp2->Vals[i];
			}
		}

		void Subtract_ByPtr(VectorVal<T_Elm, N_Dims> * a_pInp1, VectorVal<T_Elm, N_Dims> * a_pInp2, VectorVal<T_Elm, N_Dims> * a_pOut)
		{
			for (int i = 0; i < a_pArg->GetNofDims(); i++)
			{
				a_pOut->Vals[i] = a_pInp1->Vals[i] - a_pInp2->Vals[i];
			}
		}

		void DivideByNum_ByPtr(VectorVal<T_Elm, N_Dims> * a_pInp, float a_num, VectorVal<T_Elm, N_Dims> * a_pOut)
		{
			for (int i = 0; i < a_pArg->GetNofDims(); i++)
			{
				a_pOut->Vals[i] = a_pInp->Vals[i] / a_num;
			}
		}

		void MultiplyByNum_ByPtr(VectorVal<T_Elm, N_Dims> * a_pInp, float a_num, VectorVal<T_Elm, N_Dims> * a_pOut)
		{
			for (int i = 0; i < a_pArg->GetNofDims(); i++)
			{
				a_pOut->Vals[i] = a_pInp->Vals[i] * a_num;
			}
		}

		void SetToZero_ByPtr(VectorVal<T_Elm, N_Dims> * a_pArg)
		{
			for (int i = 0; i < a_pArg->GetNofDims(); i++)
			{
				a_pArg->Vals[i] = 0;
			}
		}

//#undef T_Elm float
//#undef N_Dims 3


	};
}