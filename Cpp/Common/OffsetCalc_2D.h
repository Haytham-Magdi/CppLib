#pragma once

#pragma inline_depth( 255 )

#include <stdlib.h>


#include <Lib\Cpp\Common\Debug.h>
#include <Lib\Cpp\Common\OffsetCalc_1D.h>


namespace Hcpl
{
	class OffsetCalc_2D;
	typedef Hcpl::ObjRef< OffsetCalc_2D > OffsetCalc_2D_Ref;

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

		void Lock()
		{
			m_isLocked = true;
		}

		void Unlock()
		{
			m_isLocked = false;
		}

		OffsetCalc_2D * CloneUnlocked()
		{
			OffsetCalc_2D * pRet = new OffsetCalc_2D();

			pRet->m_offsetCalc_X = m_offsetCalc_X;
			pRet->m_offsetCalc_Y = m_offsetCalc_Y;

			pRet->m_nOuterLimOffset = m_nOuterLimOffset;

			pRet->m_isLocked = false;
			return pRet;
		}

		void Init(int a_nAbsoluteStepSize_X, int a_nOuterMaxNofSteps_X, int a_nOuterMaxNofSteps_Y)
		{
			if (m_isLocked)
				throw "m_isLocked";

			m_offsetCalc_X = new OffsetCalc_1D(a_nOuterMaxNofSteps_X, a_nAbsoluteStepSize_X);
			m_offsetCalc_X->LockForever();

			m_offsetCalc_Y = new OffsetCalc_1D(a_nOuterMaxNofSteps_Y, m_offsetCalc_X->GetOuterLimOffset());
			m_offsetCalc_Y->LockForever();

			//m_offsetCalc_X->Init(a_nOuterMaxNofSteps_X, a_nAbsoluteStepSize_X);
			//m_offsetCalc_Y->Init(a_nOuterMaxNofSteps_Y, m_offsetCalc_X->GetOuterLimOffset());

			m_nOuterLimOffset = m_offsetCalc_Y->GetOuterLimOffset();
		}

		int GetOffsetPart1()
		{
			return m_offsetCalc_X->GetOffsetPart1() + m_offsetCalc_Y->GetOffsetPart1();
		}

		int CalcPart2(int a_nStep_X, int a_nStep_Y)
		{
			return m_offsetCalc_X->CalcPart2(a_nStep_X) + m_offsetCalc_Y->CalcPart2(a_nStep_Y);
		}

		void SwitchXY()
		{
			if (m_isLocked)
				throw "m_isLocked";

			OffsetCalc_1D_Ref temp;

			temp = m_offsetCalc_X;
			m_offsetCalc_X = m_offsetCalc_Y;
			m_offsetCalc_Y = temp;
		}

		void SetRange_Relative_X(int a_nBgn_X, int a_nEnd_X)
		{
			if (m_isLocked)
				throw "m_isLocked";

			m_offsetCalc_X = m_offsetCalc_X->CloneUnlocked();
			m_offsetCalc_X->SetRange_Relative(a_nBgn_X, a_nEnd_X);
			m_offsetCalc_X->LockForever();
		}

		void SetRange_Relative_Y(int a_nBgn_Y, int a_nEnd_Y)
		{
			if (m_isLocked)
				throw "m_isLocked";

			m_offsetCalc_Y = m_offsetCalc_Y->CloneUnlocked();
			m_offsetCalc_Y->SetRange_Relative(a_nBgn_Y, a_nEnd_Y);
			m_offsetCalc_Y->LockForever();
		}

		void SetRange_Relative(int a_nBgn_X, int a_nEnd_X, int a_nBgn_Y, int a_nEnd_Y)
		{
			if (m_isLocked)
				throw "m_isLocked";

			SetRange_Relative_X(a_nBgn_X, a_nEnd_X);
			SetRange_Relative_Y(a_nBgn_Y, a_nEnd_Y);
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

}