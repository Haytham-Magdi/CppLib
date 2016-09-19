
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

			a_pOut->val0 = a_pInp1->val0 + a_pInp2->val0;
			a_pOut->val1 = a_pInp1->val1 + a_pInp2->val1;
			a_pOut->val2 = a_pInp1->val2 + a_pInp2->val2;
		}

		void Subtract_ByPtr(VectorVal<T_Elm, N_Dims> * a_pInp1, VectorVal<T_Elm, N_Dims> * a_pInp2, VectorVal<T_Elm, N_Dims> * a_pOut)
		{
			a_pOut->val0 = a_pInp1->val0 - a_pInp2->val0;
			a_pOut->val1 = a_pInp1->val1 - a_pInp2->val1;
			a_pOut->val2 = a_pInp1->val2 - a_pInp2->val2;
		}

		void DivideByNum_ByPtr(VectorVal<T_Elm, N_Dims> * a_pInp, float a_num, VectorVal<T_Elm, N_Dims> * a_pOut)
		{
			a_pOut->val0 = a_pInp->val0 / a_num;
			a_pOut->val1 = a_pInp->val1 / a_num;
			a_pOut->val2 = a_pInp->val2 / a_num;
		}

		void MultiplyByNum_ByPtr(VectorVal<T_Elm, N_Dims> * a_pInp, float a_num, VectorVal<T_Elm, N_Dims> * a_pOut)
		{
			a_pOut->val0 = a_pInp->val0 * a_num;
			a_pOut->val1 = a_pInp->val1 * a_num;
			a_pOut->val2 = a_pInp->val2 * a_num;
		}

		void SetToZero_ByPtr(VectorVal<T_Elm, N_Dims> * a_pArg)
		{
			a_pArg->val0 = 0;
			a_pArg->val1 = 0;
			a_pArg->val2 = 0;
		}

//#undef T_Elm float
//#undef N_Dims 3


	};
}