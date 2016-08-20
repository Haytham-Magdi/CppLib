#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <Lib\Hcv\funcs1.h>
#include <vector>
#include <Lib\Hcv\Image.h>
//#include <Lib\Cpp\Common\commonLib.h>



namespace Hcv
{
	using namespace Hcpl;

	namespace ImageUtil
	{

		template<class T>
		void FillImage_Stripes_0(MemAccessor_2D_REF(T) a_memAcc, T & a_val1, T & a_val2)
		{
			T * valArr[2];
			valArr[0] = &a_val1;
			valArr[1] = &a_val2;

			MemAccessor_1D_REF(T) acc_Y = a_memAcc->GenAccessor_1D_Unlocked_Y();
			OffsetCalc_1D_Ref offCalc_Y = acc_Y->GetOffsetCalc();

			MemAccessor_1D_REF(T) acc_X = a_memAcc->GenAccessor_1D_Unlocked_X();

			int stripLen = 10;
			int i = 0;
			int valIdx = 0;

			T * ptr_Y = &(acc_Y->GetDataPtr())[offCalc_Y->GetOffsetPart1()];
			for (int op2_y = 0; op2_y != offCalc_Y->GetLimOffsetPart2(); op2_y += offCalc_Y->GetActualStepSize(), i++)
			{
				if (0 == i % stripLen)
				{
					valIdx = 1 - valIdx;
				}

				acc_X->SetDataPtr(ptr_Y + op2_y);
				FillLine<T>(acc_X, *valArr[valIdx]);
				//FillLine_0<T>(acc_X, *valArr[valIdx]);
			}
		}

		template<class T>
		void FillImage_Stripes(MemAccessor_2D_REF(T) a_memAcc, T & a_val1, T & a_val2)
		{
			T * valArr[2];
			valArr[0] = &a_val1;
			valArr[1] = &a_val2;

			int stripLen = 10;
			int valIdx = 0;

			MemAccessor_1D_REF(T) acc_Y = a_memAcc->GenAccessor_1D_Unlocked_Y();
			MemAccessor_1D_REF(T) acc_X = a_memAcc->GenAccessor_1D_Unlocked_X();

			PtrIterator<T> ptrItr_Y = acc_Y->GenPtrIterator(0, 0);

			int i = 0;
			for (T * ptr_Y = ptrItr_Y.GetCurrent(); !ptrItr_Y.IsDone(); ptr_Y = ptrItr_Y.Next(), i++)
			{
				if (0 == i % stripLen)
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
			MemAccessor_1D_REF(T) acc_Y = a_memAcc->GenAccessor_1D_Unlocked_Y();
			MemAccessor_1D_REF(T) acc_X = a_memAcc->GenAccessor_1D_Unlocked_X();

			PtrIterator<T> ptrItr_Y = acc_Y->GenPtrIterator(0, 0);

			int i = 0;
			for (T * ptr_Y = ptrItr_Y.GetCurrent(); !ptrItr_Y.IsDone(); ptr_Y = ptrItr_Y.Next(), i++)
			{
				acc_X->SetDataPtr(ptr_Y);
				FillLine<T>(acc_X, *valArr[valIdx]);
			}
		}

		template<class T>
		void FillLine_0(MemAccessor_1D_REF(T) a_memAcc, T & a_val)
		{
			OffsetCalc_1D_Ref offCalc = a_memAcc->GetOffsetCalc();

			T * ptr = &(a_memAcc->GetDataPtr())[offCalc->GetOffsetPart1()];
			for (int op2 = 0; op2 != offCalc->GetLimOffsetPart2(); op2 += offCalc->GetActualStepSize())
			{
				memcpy(&ptr[op2], &a_val, sizeof(T));
			}
		}

		template<class T>
		void FillLine(MemAccessor_1D_REF(T) a_memAcc, T & a_val)
		{
			PtrIterator<T> ptrItr = a_memAcc->GenPtrIterator(0, 0);

			for (T * ptr = ptrItr.GetCurrent(); !ptrItr.IsDone(); ptr = ptrItr.Next())
			{
				memcpy(ptr, &a_val, sizeof(T));
			}
		}

	};
}