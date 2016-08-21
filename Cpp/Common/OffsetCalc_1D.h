#pragma once

#pragma inline_depth( 255 )

#include <stdlib.h>


#include <Lib\Cpp\Common\Debug.h>


namespace Hcpl
{
	class OffsetCalc_1D;
	typedef Hcpl::ObjRef< OffsetCalc_1D > OffsetCalc_1D_Ref;

	class OffsetCalc_1D : FRM_Object
	{
	public:

		OffsetCalc_1D()
		{
			m_isLocked = false;
		}

		OffsetCalc_1D(int a_nOuterMaxNofSteps, int a_nAbsoluteStepSize)
		{
			m_isLocked = false;
			Init(a_nOuterMaxNofSteps, a_nAbsoluteStepSize);
		}

		OffsetCalc_1D_Ref CloneUnlocked()
		{
			OffsetCalc_1D_Ref pRet = new OffsetCalc_1D();

			//	This could be dangerous in case that any ref member exists!
			*pRet = *this;

			pRet->m_isLocked = false;
			return pRet;
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

		void SetRange_Relative(int a_nRelativeBgnStep, int a_nRelativeEndStep)
		{
			if (m_isLocked)
				throw "m_isLocked";

			Hcpl_ASSERT(a_nRelativeBgnStep >= 0);
			Hcpl_ASSERT(a_nRelativeBgnStep < m_nMaxNofSteps);

			Hcpl_ASSERT(a_nRelativeEndStep >= 0);
			Hcpl_ASSERT(a_nRelativeEndStep < m_nMaxNofSteps);

			int nRelativeRangeDiff = a_nRelativeEndStep - a_nRelativeBgnStep;
			Hcpl_ASSERT(0 != nRelativeRangeDiff);

			int nDir = m_nActualStepSize / m_nAbsoluteStepSize;

			int nNewInnerBgnStep = m_nInnerBgnStep + nDir * a_nRelativeBgnStep;
			int nNewInnerEndStep = m_nInnerBgnStep + nDir * a_nRelativeEndStep;

			SetRange_Absolute(nNewInnerBgnStep, nNewInnerEndStep);
		}

		void Init(int a_nOuterMaxNofSteps, int a_nAbsoluteStepSize)
		{
			if (m_isLocked)
				throw "m_isLocked";

			Init(a_nOuterMaxNofSteps, a_nAbsoluteStepSize, 0, a_nOuterMaxNofSteps - 1);
		}

		void Init(int a_nOuterMaxNofSteps, int a_nAbsoluteStepSize, int a_nInnerBgnStep, int a_nInnerEndStep)
		{
			if (m_isLocked)
				throw "m_isLocked";

			Hcpl_ASSERT(a_nOuterMaxNofSteps > 0);
			Hcpl_ASSERT(a_nAbsoluteStepSize > 0);

			m_nOuterMaxNofSteps = a_nOuterMaxNofSteps;
			m_nAbsoluteStepSize = a_nAbsoluteStepSize;
			m_nOuterLimOffset = m_nOuterMaxNofSteps * m_nAbsoluteStepSize;

			SetRange_Absolute(a_nInnerBgnStep, a_nInnerEndStep);
		}

		int GetOffsetPart1()
		{
			return m_nOffsetPart1;
		}

		int CalcPart2(int a_nStep)
		{
			return a_nStep * m_nActualStepSize;
		}

		int ReverseCalc(int a_nOffset)
		{
			Hcpl_ASSERT(0 == a_nOffset % m_nAbsoluteStepSize);
			//int nStep = ((a_nOffset - m_nOffsetPart1) % m_nOuterLimOffset) / m_nAbsoluteStepSize;

			int nStep = ReverseCalcPart2(a_nOffset - m_nOffsetPart1);
			return nStep;
		}

		int ReverseCalcPart2(int a_nOffsetPart2)
		{
			Hcpl_ASSERT(0 == a_nOffsetPart2 % m_nAbsoluteStepSize);
			
			int nStep = (a_nOffsetPart2 % m_nOuterLimOffset) / m_nAbsoluteStepSize;
			return nStep;
		}

		//int GetAbsoluteStepSize()
		//{
		//	return m_nAbsoluteStepSize;
		//}

		int GetActualStepSize()
		{
			return m_nActualStepSize;
		}

		int GetLimOffsetPart2()
		{
			return m_nLimOffsetPart2;
		}

		int GetOuterLimOffset()
		{
			return m_nOuterLimOffset;
		}

		//int GetOuterMaxNofSteps()
		//{
		//	return m_nOuterMaxNofSteps;
		//}

		int GetMaxNofSteps()
		{
			return m_nMaxNofSteps;
		}

		void ResetRange()
		{
			SetRange_Absolute(0, m_nOuterMaxNofSteps - 1);
		}

	protected:

		void SetRange_Absolute(int a_nInnerBgnStep, int a_nInnerEndStep)
		{
			if (m_isLocked)
				throw "m_isLocked";

			Hcpl_ASSERT(a_nInnerBgnStep >= 0);
			Hcpl_ASSERT(a_nInnerBgnStep < m_nOuterMaxNofSteps);

			Hcpl_ASSERT(a_nInnerEndStep >= 0);
			Hcpl_ASSERT(a_nInnerEndStep < m_nOuterMaxNofSteps);

			int nInnerRangeDiff = a_nInnerEndStep - a_nInnerBgnStep;
			Hcpl_ASSERT(0 != nInnerRangeDiff);

			m_nMaxNofSteps = abs(nInnerRangeDiff);

			m_nInnerBgnStep = a_nInnerBgnStep;
			m_nInnerEndStep = a_nInnerEndStep;

			m_nOffsetPart1 = m_nInnerBgnStep * m_nAbsoluteStepSize;

			if (nInnerRangeDiff > 0)
			{
				m_nActualStepSize = m_nAbsoluteStepSize;
			}
			else
			{
				m_nActualStepSize = -m_nAbsoluteStepSize;
			}

			m_nLimOffsetPart2 = CalcPart2(m_nInnerEndStep + 1);
		}

	protected:

		int m_nOffsetPart1;
		int m_nLimOffsetPart2;

		int m_nAbsoluteStepSize;
		int m_nActualStepSize;

		int m_nOuterMaxNofSteps;
		int m_nOuterLimOffset;

		int m_nMaxNofSteps;
		int m_nInnerBgnStep;
		int m_nInnerEndStep;

		bool m_isLocked;
	};

}