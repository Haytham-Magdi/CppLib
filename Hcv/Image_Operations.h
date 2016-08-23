#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <Lib\Hcv\funcs1.h>
#include <vector>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\Element_Operations.h>
#include <Lib\Hcv\Line_Operations.h>
#include <Lib\Hcv\TempImageAccessor.h>



namespace Hcv
{
	using namespace Hcpl;
	using namespace Line_Operations;
	using namespace Element_Operations;

	namespace Image_Operations
	{
		template<class T>
		void FillImage_Stripes(MemAccessor_2D_REF(T) a_memAcc, T & a_val1, T & a_val2, int a_stripWidth)
		{
			T * valArr[2];
			valArr[0] = &a_val1;
			valArr[1] = &a_val2;

			int valIdx = 1;

			MemAccessor_1D_REF(T) acc_Y = a_memAcc->GenAccessor_1D_Y();
			MemAccessor_1D_REF(T) acc_X = a_memAcc->GenAccessor_1D_X();

			PtrIterator<T> ptrItr_Y = acc_Y->GenPtrIterator(0);

			int i = 0;
			for (T * ptr_Y = ptrItr_Y.GetCurrent(); !ptrItr_Y.IsDone(); ptr_Y = ptrItr_Y.Next(), i++)
			{
				if (0 == i % a_stripWidth)
				{
					valIdx = 1 - valIdx;
				}

				acc_X->SetDataPtr(ptr_Y);
				FillLine<T>(acc_X, *valArr[valIdx]);
			}
		}

		template<class T>
		void FillImage(MemAccessor_2D_REF(T) a_memAcc, T & a_val)
		{
			MemAccessor_1D_REF(T) acc_Y = a_memAcc->GenAccessor_1D_Y();
			MemAccessor_1D_REF(T) acc_X = a_memAcc->GenAccessor_1D_X();

			PtrIterator<T> ptrItr_Y = acc_Y->GenPtrIterator(0);

			int i = 0;
			for (T * ptr_Y = ptrItr_Y.GetCurrent(); !ptrItr_Y.IsDone(); ptr_Y = ptrItr_Y.Next(), i++)
			{
				acc_X->SetDataPtr(ptr_Y);
				FillLine<T>(acc_X, a_val);
			}
		}

		template<class T>
		void DivideImageByNum(MemAccessor_2D_REF(T) a_memAcc, float a_num)
		{
			MemAccessor_1D_REF(T) acc_Y = a_memAcc->GenAccessor_1D_Y();
			MemAccessor_1D_REF(T) acc_X = a_memAcc->GenAccessor_1D_X();

			PtrIterator<T> ptrItr_Y = acc_Y->GenPtrIterator(0);

			int i = 0;
			for (T * ptr_Y = ptrItr_Y.GetCurrent(); !ptrItr_Y.IsDone(); ptr_Y = ptrItr_Y.Next(), i++)
			{
				acc_X->SetDataPtr(ptr_Y);
				DivideLineByNum<T>(acc_X, a_num);
			}
		}

		template<class T>
		void CopyImage(MemAccessor_2D_REF(T) a_destAcc, MemAccessor_2D_REF(T) a_srcAcc)
		{
			MemAccessor_1D_REF(T) acc_Src_Y = a_srcAcc->GenAccessor_1D_Y();
			MemAccessor_1D_REF(T) acc_Src_X = a_srcAcc->GenAccessor_1D_X();

			MemAccessor_1D_REF(T) acc_Dest_Y = a_destAcc->GenAccessor_1D_Y();
			MemAccessor_1D_REF(T) acc_Dest_X = a_destAcc->GenAccessor_1D_X();

			Hcpl_ASSERT(acc_Src_Y->GetOffsetCalc()->GetMaxNofSteps() ==
				acc_Dest_Y->GetOffsetCalc()->GetMaxNofSteps());

			PtrIterator<T> ptrItr_Src_Y = acc_Src_Y->GenPtrIterator(0);
			PtrIterator<T> ptrItr_Dest_Y = acc_Dest_Y->GenPtrIterator(0);

			int i = 0;
			T * ptr_Src_Y = ptrItr_Src_Y.GetCurrent();
			T * ptr_Dest_Y = ptrItr_Dest_Y.GetCurrent();
			for (;
				!ptrItr_Src_Y.IsDone();
				ptr_Src_Y = ptrItr_Src_Y.Next(), ptr_Dest_Y = ptrItr_Dest_Y.Next(), i++)
			{
				acc_Src_X->SetDataPtr(ptr_Src_Y);
				acc_Dest_X->SetDataPtr(ptr_Dest_Y);

				CopyLine<T>(acc_Dest_X, acc_Src_X);
			}
		}

		template<class T>
		void CalcMagImage(MemAccessor_2D_REF(T) a_inpAcc, MemAccessor_2D_REF(float) a_outAcc)
		{
			MemAccessor_1D_REF(T) acc_Inp_Y = a_inpAcc->GenAccessor_1D_Y();
			MemAccessor_1D_REF(T) acc_Inp_X = a_inpAcc->GenAccessor_1D_X();

			MemAccessor_1D_REF(float) acc_Out_Y = a_outAcc->GenAccessor_1D_Y();
			MemAccessor_1D_REF(float) acc_Out_X = a_outAcc->GenAccessor_1D_X();

			Hcpl_ASSERT(acc_Inp_Y->GetOffsetCalc()->GetMaxNofSteps() ==
				acc_Out_Y->GetOffsetCalc()->GetMaxNofSteps());

			PtrIterator<T> ptrItr_Inp_Y = acc_Inp_Y->GenPtrIterator(0);
			PtrIterator<float> ptrItr_Out_Y = acc_Out_Y->GenPtrIterator(0);

			int i = 0;
			T * ptr_Inp_Y = ptrItr_Inp_Y.GetCurrent();
			float * ptr_Out_Y = ptrItr_Out_Y.GetCurrent();
			for (;
				!ptrItr_Inp_Y.IsDone();
				ptr_Inp_Y = ptrItr_Inp_Y.Next(), ptr_Out_Y = ptrItr_Out_Y.Next(), i++)
			{
				acc_Inp_X->SetDataPtr(ptr_Inp_Y);
				acc_Out_X->SetDataPtr(ptr_Out_Y);

				CalcMagLine<T>(acc_Inp_X, acc_Out_X);
			}
		}

		template<class T>
		void CalcMagSqrImage(MemAccessor_2D_REF(T) a_inpAcc, MemAccessor_2D_REF(float) a_outAcc)
		{
			MemAccessor_1D_REF(T) acc_Inp_Y = a_inpAcc->GenAccessor_1D_Y();
			MemAccessor_1D_REF(T) acc_Inp_X = a_inpAcc->GenAccessor_1D_X();

			MemAccessor_1D_REF(float) acc_Out_Y = a_outAcc->GenAccessor_1D_Y();
			MemAccessor_1D_REF(float) acc_Out_X = a_outAcc->GenAccessor_1D_X();

			Hcpl_ASSERT(acc_Inp_Y->GetOffsetCalc()->GetMaxNofSteps() ==
				acc_Out_Y->GetOffsetCalc()->GetMaxNofSteps());

			PtrIterator<T> ptrItr_Inp_Y = acc_Inp_Y->GenPtrIterator(0);
			PtrIterator<float> ptrItr_Out_Y = acc_Out_Y->GenPtrIterator(0);

			int i = 0;
			T * ptr_Inp_Y = ptrItr_Inp_Y.GetCurrent();
			float * ptr_Out_Y = ptrItr_Out_Y.GetCurrent();
			for (;
				!ptrItr_Inp_Y.IsDone();
				ptr_Inp_Y = ptrItr_Inp_Y.Next(), ptr_Out_Y = ptrItr_Out_Y.Next(), i++)
			{
				acc_Inp_X->SetDataPtr(ptr_Inp_Y);
				acc_Out_X->SetDataPtr(ptr_Out_Y);

				CalcMagSqrLine<T>(acc_Inp_X, acc_Out_X);
			}
		}

		template<class T>
		void AvgImage_H(MemAccessor_2D_REF(T) a_inpAcc, MemAccessor_2D_REF(T) a_outAcc,
			Range<int> & a_winRange_X)
		{
			MemAccessor_1D_REF(T) acc_Inp_Y = a_inpAcc->GenAccessor_1D_Y();
			MemAccessor_1D_REF(T) acc_Inp_X = a_inpAcc->GenAccessor_1D_X();

			MemAccessor_1D_REF(T) acc_Out_Y = a_outAcc->GenAccessor_1D_Y();
			MemAccessor_1D_REF(T) acc_Out_X = a_outAcc->GenAccessor_1D_X();

			Hcpl_ASSERT(acc_Inp_Y->GetOffsetCalc()->GetMaxNofSteps() ==
				acc_Out_Y->GetOffsetCalc()->GetMaxNofSteps());

			PtrIterator<T> ptrItr_Inp_Y = acc_Inp_Y->GenPtrIterator(0);
			PtrIterator<T> ptrItr_Out_Y = acc_Out_Y->GenPtrIterator(0);

			int i = 0;
			T * ptr_Inp_Y = ptrItr_Inp_Y.GetCurrent();
			T * ptr_Out_Y = ptrItr_Out_Y.GetCurrent();

			for (;
				!ptrItr_Inp_Y.IsDone();
				ptr_Inp_Y = ptrItr_Inp_Y.Next(), ptr_Out_Y = ptrItr_Out_Y.Next(), i++)
			{
				acc_Inp_X->SetDataPtr(ptr_Inp_Y);
				acc_Out_X->SetDataPtr(ptr_Out_Y);

				AvgLine(acc_Inp_X, acc_Out_X, a_winRange_X);
			}
		}

		template<class T>
		void AvgImage(MemAccessor_2D_REF(T) a_inpAcc, MemAccessor_2D_REF(T) a_outAcc,
			Window<int> & a_window)
		{
			TempImageAccessor_REF(T) tmpImgAcc = new TempImageAccessor<T>(a_outAcc);

			AvgImage_H<T>(a_inpAcc, tmpImgAcc->GetMemAccessor(), a_window.GetRange_X());
			//AvgImage_H<T>(a_inpAcc, a_outAcc, a_winRange_X);
		
			//return;

			MemAccessor_2D_REF(T) outAcc2 = a_outAcc->Clone();
			outAcc2->SwitchXY();

			tmpImgAcc->SwitchXY();

			AvgImage_H<T>(tmpImgAcc->GetMemAccessor(), outAcc2, a_window.GetRange_Y());
		}

		template<class T>
		void CalcSqrtImage(MemAccessor_2D_REF(T) a_inpAcc, MemAccessor_2D_REF(float) a_outAcc)
		{
			MemAccessor_1D_REF(T) acc_Inp_Y = a_inpAcc->GenAccessor_1D_Y();
			MemAccessor_1D_REF(T) acc_Inp_X = a_inpAcc->GenAccessor_1D_X();

			MemAccessor_1D_REF(float) acc_Out_Y = a_outAcc->GenAccessor_1D_Y();
			MemAccessor_1D_REF(float) acc_Out_X = a_outAcc->GenAccessor_1D_X();

			Hcpl_ASSERT(acc_Inp_Y->GetOffsetCalc()->GetMaxNofSteps() ==
				acc_Out_Y->GetOffsetCalc()->GetMaxNofSteps());

			PtrIterator<T> ptrItr_Inp_Y = acc_Inp_Y->GenPtrIterator(0);
			PtrIterator<float> ptrItr_Out_Y = acc_Out_Y->GenPtrIterator(0);

			int i = 0;
			T * ptr_Inp_Y = ptrItr_Inp_Y.GetCurrent();
			float * ptr_Out_Y = ptrItr_Out_Y.GetCurrent();
			for (;
				!ptrItr_Inp_Y.IsDone();
				ptr_Inp_Y = ptrItr_Inp_Y.Next(), ptr_Out_Y = ptrItr_Out_Y.Next(), i++)
			{
				acc_Inp_X->SetDataPtr(ptr_Inp_Y);
				acc_Out_X->SetDataPtr(ptr_Out_Y);

				CalcSqrtImage<T>(acc_Inp_X, acc_Out_X);
			}
		}




	};
}