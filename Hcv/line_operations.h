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

	namespace Line_Operations
	{

		template<class T>
		void FillLine(MemAccessor_1D_REF(T) a_memAcc, T & a_val)
		{
			PtrIterator<T> ptrItr = a_memAcc->GenPtrIterator();

			for (T * ptr = ptrItr.GetCurrent(); !ptrItr.IsDone(); ptr = ptrItr.Next())
			{
				Element_Operations::Copy_ByPtr<T>(ptr, &a_val);
			}
		}

		template<class T>
		void DivideLineByNum(MemAccessor_1D_REF(T) a_memAcc, float a_num)
		{
			PtrIterator<T> ptrItr = a_memAcc->GenPtrIterator();

			for (T * ptr = ptrItr.GetCurrent(); !ptrItr.IsDone(); ptr = ptrItr.Next())
			{
				Element_Operations::DivideSelfByNum_ByPtr<T>(ptr, a_num);
			}
		}

		template<class T>
		void CopyLine(MemAccessor_1D_REF(T) a_destAcc, MemAccessor_1D_REF(T) a_srcAcc)
		{
			Hcpl_ASSERT(a_srcAcc->GetIndexSize() ==
				a_destAcc->GetIndexSize());

			PtrIterator<T> ptrItr_Src = a_srcAcc->GenPtrIterator();
			PtrIterator<T> ptrItr_Dest = a_destAcc->GenPtrIterator();

			T * ptr_Src = ptrItr_Src.GetCurrent();
			T * ptr_Dest = ptrItr_Dest.GetCurrent();
			for (;
				!ptrItr_Src.IsDone();
				ptr_Src = ptrItr_Src.Next(), ptr_Dest = ptrItr_Dest.Next())
			{
				Element_Operations::Copy_ByPtr<T>(ptr_Dest, ptr_Src);
			}
		}

		template<class T>
		void CalcMagLine(MemAccessor_1D_REF(T) a_inpAcc, MemAccessor_1D_REF(float) a_outAcc)
		{
			Hcpl_ASSERT(a_inpAcc->GetIndexSize() ==
				a_outAcc->GetIndexSize());

			PtrIterator<T> ptrItr_Inp = a_inpAcc->GenPtrIterator();
			PtrIterator<float> ptrItr_Out = a_outAcc->GenPtrIterator();

			T * ptr_Inp = ptrItr_Inp.GetCurrent();
			float * ptr_Out = ptrItr_Out.GetCurrent();
			for (;
				!ptrItr_Inp.IsDone();
				ptr_Inp = ptrItr_Inp.Next(), ptr_Out = ptrItr_Out.Next())
			{
				*ptr_Out = Element_Operations::CalcMag_ByPtr<T>(ptr_Inp);
			}
		}

		template<class T>
		void CalcMagSqrLine(MemAccessor_1D_REF(T) a_inpAcc, MemAccessor_1D_REF(float) a_outAcc)
		{
			Hcpl_ASSERT(a_inpAcc->GetIndexSize() ==
				a_outAcc->GetIndexSize());

			PtrIterator<T> ptrItr_Inp = a_inpAcc->GenPtrIterator();
			PtrIterator<float> ptrItr_Out = a_outAcc->GenPtrIterator();

			T * ptr_Inp = ptrItr_Inp.GetCurrent();
			float * ptr_Out = ptrItr_Out.GetCurrent();
			for (;
				!ptrItr_Inp.IsDone();
				ptr_Inp = ptrItr_Inp.Next(), ptr_Out = ptrItr_Out.Next())
			{
				*ptr_Out = Element_Operations::CalcMagSqr_ByPtr<T>(ptr_Inp);
			}
		}

		template<class T>
		void AvgLine(MemAccessor_1D_REF(T) a_inpAcc, MemAccessor_1D_REF(T) a_outAcc, Range<int> & a_winRange)
		{
			Hcpl_ASSERT(a_inpAcc->GetIndexSize() == a_outAcc->GetIndexSize());
			Hcpl_ASSERT(a_winRange.GetBgn() <= 0);
			Hcpl_ASSERT(0 <= a_winRange.GetEnd());

			T zeroVal;
			SetToZero_ByPtr<T>(&zeroVal);
		
			FillLine<T>(a_outAcc, zeroVal);

			MemSimpleAccessor_1D<T> sac_Inp = a_inpAcc->GenSimpleAccessor();
			MemSimpleAccessor_1D<T> sac_Out = a_outAcc->GenSimpleAccessor();

			const int nSize_1D = a_inpAcc->GetIndexSize();
			
			const int nBefDiff = - a_winRange.GetBgn();
			const int nAftDiff = a_winRange.GetEnd();

			const int nCenterEnd = nSize_1D - 1 - nAftDiff;
			const int nRangeLen = nBefDiff + 1 + nAftDiff;



			T sum;
			//T avg;
			T * pDest;
			SetToZero_ByPtr<T>(&sum);

			for (int i = 0; i < nRangeLen; i++)
			{
				Add_ByPtr(&sum, &sac_Inp[i], &sum);
			}
			pDest = &sac_Out[nBefDiff];
			Copy_ByPtr(pDest, &sum);
			DivideSelfByNum_ByPtr(pDest, nRangeLen);
			
			for (int i = nBefDiff + 1; i <= nCenterEnd; i++)
			{
				pDest = &sac_Out[i];

				Subtract_ByPtr(&sum, &sac_Inp[(i - 1) - nBefDiff], &sum);
				Add_ByPtr(&sum, &sac_Inp[i + nAftDiff], &sum);

				Copy_ByPtr(pDest, &sum);
				DivideSelfByNum_ByPtr(pDest, nRangeLen);
			}

///////////////////////////////

			//	Fill bgn gap in output
			{
				T * pSrc = &sac_Out[nBefDiff];
				for (int i = 0; i < nBefDiff; i++)
				{
					pDest = &sac_Out[i];
					Copy_ByPtr(pDest, pSrc);
				}
			}

			//	Fill end gap in output
			{
				const int nSrcIdx = (nSize_1D - 1) - nAftDiff;
				T * pSrc = &sac_Out[nSrcIdx];
				for (int i = nSrcIdx + 1; i < nSize_1D; i++)
				{
					pDest = &sac_Out[i];
					Copy_ByPtr(pDest, pSrc);
				}
			}


		}

		template<class T>
		void CalcSqrtImage(MemAccessor_1D_REF(T) a_inpAcc, MemAccessor_1D_REF(float) a_outAcc)
		{
			Hcpl_ASSERT(a_inpAcc->GetIndexSize() ==
				a_outAcc->GetIndexSize());

			PtrIterator<T> ptrItr_Inp = a_inpAcc->GenPtrIterator();
			PtrIterator<float> ptrItr_Out = a_outAcc->GenPtrIterator();

			T * ptr_Inp = ptrItr_Inp.GetCurrent();
			float * ptr_Out = ptrItr_Out.GetCurrent();
			for (;
				!ptrItr_Inp.IsDone();
				ptr_Inp = ptrItr_Inp.Next(), ptr_Out = ptrItr_Out.Next())
			{
				*ptr_Out = Element_Operations::CalcSqrt_ByPtr<T>(ptr_Inp);
			}
		}

	};
}