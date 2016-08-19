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
		void FillImage(MemAccessor_2D_REF(T) a_memAcc, T & a_val)
		{
			MemAccessor_1D_REF(T) acc_Y = a_memAcc->GenAccessor_1D_Unlocked_Y();
			auto offCalc_Y = acc_Y->GetOffsetCalc();

			MemAccessor_1D_REF(T) acc_X = a_memAcc->GenAccessor_1D_Unlocked_X();

			T * ptr_Y = &(acc_Y->GetDataPtr())[offCalc_Y->GetOffsetPart1()];
			for (int op2_y = 0; op2_y != offCalc_Y->GetLimOffsetPart2(); op2_y += offCalc_Y->GetActualStepSize())
			{
				acc_X->SetDataPtr(ptr_Y + op2_y);
			}
		}

		template<class T>
		void FillLine(MemAccessor_2D_REF(T) a_memAcc, T & a_val)
		{
			MemAccessor_1D_REF(T) acc = a_memAcc->GenAccessor_1D_Unlocked();
			auto offCalc = acc->GetOffsetCalc();

			T * ptr = &(acc->GetDataPtr())[offCalc->GetOffsetPart1()];
			for (int op2 = 0; op2 != offCalc->GetLimOffsetPart2(); op2 += offCalc->GetActualStepSize())
			{
				memcpy(&ptr[op2], &a_val, sizeof(T));
			}
		}

	};
}