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
		void FillImage_Stripes(MemAccessor_2D_REF(T) a_memAcc, T & a_val1, T & a_val2, int a_stripWidth)
		{
			T * valArr[2];
			valArr[0] = &a_val1;
			valArr[1] = &a_val2;

			int valIdx = 1;

			MemAccessor_1D_REF(T) acc_Y = a_memAcc->GenAccessor_1D_Unlocked_Y();
			MemAccessor_1D_REF(T) acc_X = a_memAcc->GenAccessor_1D_Unlocked_X();

			PtrIterator<T> ptrItr_Y = acc_Y->GenPtrIterator(0, 0);

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
			MemAccessor_1D_REF(T) acc_Y = a_memAcc->GenAccessor_1D_Unlocked_Y();
			MemAccessor_1D_REF(T) acc_X = a_memAcc->GenAccessor_1D_Unlocked_X();

			PtrIterator<T> ptrItr_Y = acc_Y->GenPtrIterator(0, 0);

			int i = 0;
			for (T * ptr_Y = ptrItr_Y.GetCurrent(); !ptrItr_Y.IsDone(); ptr_Y = ptrItr_Y.Next(), i++)
			{
				acc_X->SetDataPtr(ptr_Y);
				FillLine<T>(acc_X, a_val);
			}
		}

		template<class T>
		void FillLine(MemAccessor_1D_REF(T) a_memAcc, T & a_val)
		{
			PtrIterator<T> ptrItr = a_memAcc->GenPtrIterator(0, 0);

			for (T * ptr = ptrItr.GetCurrent(); !ptrItr.IsDone(); ptr = ptrItr.Next())
			{
				Type_Basic::CopyByPtr<T>(ptr, &a_val);
			}
		}

	};
}