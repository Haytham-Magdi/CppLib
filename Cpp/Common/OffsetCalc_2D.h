#pragma once

#pragma inline_depth( 255 )

#include <stdlib.h>


#include <Lib\Cpp\Common\Debug.h>
#include <Lib\Cpp\Common\OffsetCalc_1D.h>


namespace Hcpl
{

	class OffsetCalc_2D : FRM_Object
	{
	public:

		OffsetCalc_2D()
		{
			m_isLocked = false;
		}

		OffsetCalc_2D(int a_nAbsoluteStepSize_X, int a_nOuterMaxNofSteps_X, int a_nOuterMaxNofSteps_Y)
		{
			m_isLocked = false;
			Init(a_nAbsoluteStepSize_X, a_nOuterMaxNofSteps_X, a_nOuterMaxNofSteps_Y);
		}

		bool IsLocked()
		{
			return m_isLocked;
		}

		void LockForever()
		{
			m_isLocked = true;
		}

		void Init(int a_nAbsoluteStepSize_X, int a_nOuterMaxNofSteps_X, int a_nOuterMaxNofSteps_Y)
		{
			m_offsetCalc_X->Init(a_nOuterMaxNofSteps_X, a_nAbsoluteStepSize_X);
			m_offsetCalc_Y->Init(a_nOuterMaxNofSteps_Y, m_offsetCalc_X->GetOuterLimOffset());

			m_nOuterLimOffset = m_offsetCalc_Y->GetOuterLimOffset();
		}

		//void CopyTo(OffsetCalc_2D * a_pDest)
		//{
		//	*a_pDest = *this;
		//}

		int GetOffsetPart1()
		{
			return m_offsetCalc_X->GetOffsetPart1() + m_offsetCalc_Y->GetOffsetPart1();
		}

		int CalcPart2(int a_nStep_X, int a_nStep_Y)
		{
			return m_offsetCalc_X->CalcPart2(a_nStep_X) + m_offsetCalc_Y->CalcPart2(a_nStep_Y);
		}

		void SwapXY()
		{
			OffsetCalc_1D_Ref temp;

			temp = m_offsetCalc_X;
			m_offsetCalc_X = m_offsetCalc_Y;
			m_offsetCalc_Y = temp;
		}

		OffsetCalc_1D_Ref GetOffsetCalc_X()
		{
			return m_offsetCalc_X;
		}

		OffsetCalc_1D_Ref GetOffsetCalc_Y()
		{
			return m_offsetCalc_Y;
		}

	protected:

		OffsetCalc_1D_Ref m_offsetCalc_X;
		OffsetCalc_1D_Ref m_offsetCalc_Y;

		int m_nOuterLimOffset;

		bool m_isLocked;
	};

	typedef Hcpl::ObjRef< OffsetCalc_2D > OffsetCalc_2D_Ref;
}