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

	namespace LineOperations
	{

		template<class T>
		void FillLine(MemAccessor_1D_REF(T) a_memAcc, T & a_val)
		{
			PtrIterator<T> ptrItr = a_memAcc->GenPtrIterator(0, 0);

			for (T * ptr = ptrItr.GetCurrent(); !ptrItr.IsDone(); ptr = ptrItr.Next())
			{
				Element_Operations::CopyByPtr<T>(ptr, &a_val);
			}
		}

		template<class T>
		void CopyLine(MemAccessor_1D_REF(T) a_destAcc, MemAccessor_1D_REF(T) a_srcAcc)
		{
			Hcpl_ASSERT(a_srcAcc->GetOffsetCalc()->GetMaxNofSteps() ==
				a_destAcc->GetOffsetCalc()->GetMaxNofSteps());

			PtrIterator<T> ptrItr_Src = a_srcAcc->GenPtrIterator(0, 0);
			PtrIterator<T> ptrItr_Dest = a_destAcc->GenPtrIterator(0, 0);

			T * ptr_Src = ptrItr_Src.GetCurrent();
			T * ptr_Dest = ptrItr_Dest.GetCurrent();
			for (;
				!ptrItr_Src.IsDone();
				ptr_Src = ptrItr_Src.Next(), ptr_Dest = ptrItr_Dest.Next())
			{
				Element_Operations::CopyByPtr<T>(ptr_Dest, ptr_Src);
			}
		}

		template<class T>
		void CalcMagLine(MemAccessor_1D_REF(T) a_inpAcc, MemAccessor_1D_REF(float) a_outAcc)
		{
			Hcpl_ASSERT(a_inpAcc->GetOffsetCalc()->GetMaxNofSteps() ==
				a_outAcc->GetOffsetCalc()->GetMaxNofSteps());

			PtrIterator<T> ptrItr_Inp = a_inpAcc->GenPtrIterator(0, 0);
			PtrIterator<float> ptrItr_Out = a_outAcc->GenPtrIterator(0, 0);

			T * ptr_Inp = ptrItr_Inp.GetCurrent();
			float * ptr_Out = ptrItr_Out.GetCurrent();
			for (;
				!ptrItr_Inp.IsDone();
				ptr_Inp = ptrItr_Inp.Next(), ptr_Out = ptrItr_Out.Next())
			{
				*ptr_Out = Element_Operations::CalcMagByPtr<T>(ptr_Inp);
			}
		}

		template<class T>
		void CalcMagSqrLine(MemAccessor_1D_REF(T) a_inpAcc, MemAccessor_1D_REF(float) a_outAcc)
		{
			Hcpl_ASSERT(a_inpAcc->GetOffsetCalc()->GetMaxNofSteps() ==
				a_outAcc->GetOffsetCalc()->GetMaxNofSteps());

			PtrIterator<T> ptrItr_Inp = a_inpAcc->GenPtrIterator(0, 0);
			PtrIterator<float> ptrItr_Out = a_outAcc->GenPtrIterator(0, 0);

			T * ptr_Inp = ptrItr_Inp.GetCurrent();
			float * ptr_Out = ptrItr_Out.GetCurrent();
			for (;
				!ptrItr_Inp.IsDone();
				ptr_Inp = ptrItr_Inp.Next(), ptr_Out = ptrItr_Out.Next())
			{
				*ptr_Out = Element_Operations::CalcMagSqrByPtr<T>(ptr_Inp);
			}
		}



	};
}