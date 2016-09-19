
		//template<> void Copy_ByPtr<VectorVal>(VectorVal<T_Elm, N_Dims> * a_pDest, VectorVal<T_Elm, N_Dims> * a_pSrc);

		//template<>
		//float CalcMagSqr_ByPtr(VectorVal<T_Elm, N_Dims> * a_pArg)


		//template<class T, int const V_NofChannels>
		//using VectorValImageAcc = ImageAccessor<T, VectorVal<T, V_NofChannels>, V_NofChannels>;

		//template<class T, int const V_NofChannels>
		//using VectorValImageAcc = ImageAccessor<T, VectorVal<T, V_NofChannels>, V_NofChannels>;



		template<> float CalcMagSqr_ByPtr(VectorVal<T_Elm, N_Dims> * a_pArg);

		template<> void Add_ByPtr(VectorVal<T_Elm, N_Dims> * a_pInp1, VectorVal<T_Elm, N_Dims> * a_pInp2, VectorVal<T_Elm, N_Dims> * a_pOut);

		template<> void Subtract_ByPtr(VectorVal<T_Elm, N_Dims> * a_pInp1, VectorVal<T_Elm, N_Dims> * a_pInp2, VectorVal<T_Elm, N_Dims> * a_pOut);

		template<> void DivideByNum_ByPtr(VectorVal<T_Elm, N_Dims> * a_pInp, float a_num, VectorVal<T_Elm, N_Dims> * a_pOut);

		template<> void MultiplyByNum_ByPtr(VectorVal<T_Elm, N_Dims> * a_pInp, float a_num, VectorVal<T_Elm, N_Dims> * a_pOut);

		template<> void SetToZero_ByPtr(VectorVal<T_Elm, N_Dims> * a_pArg);

