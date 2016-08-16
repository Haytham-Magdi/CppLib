#pragma once

#pragma inline_depth( 255 )

#include <stdlib.h>


#include <Lib\Cpp\Common\Debug.h>


namespace Hcpl
{
	class OffsetCalc_1D : FRM_Object
	{
	public:

		OffsetCalc_1D()
		{
		}

		OffsetCalc_1D(int a_nOuterMaxNofSteps, int a_nAbsoluteStepSize)
		{
			Init(a_nOuterMaxNofSteps, a_nAbsoluteStepSize);
		}

		void CopyTo(OffsetCalc_1D * a_pDest)
		{
			*a_pDest = *this;
		}

		void SwapWith(OffsetCalc_1D & a_rOffCalc)
		{
			OffsetCalc_1D temp;

			temp = *this;
			*this = a_rOffCalc;
			a_rOffCalc = temp;
		}

		void CopyRelativeRangeTo(OffsetCalc_1D * a_pDest, int a_nRelativeBgnStep, int a_nRelativeEndStep)
		{
			Hcpl_ASSERT(a_nRelativeBgnStep >= 0);
			Hcpl_ASSERT(a_nRelativeBgnStep < m_nMaxNofSteps);

			Hcpl_ASSERT(a_nRelativeEndStep >= 0);
			Hcpl_ASSERT(a_nRelativeEndStep < m_nMaxNofSteps);

			int nRelativeRangeDiff = a_nRelativeEndStep - a_nRelativeBgnStep;
			Hcpl_ASSERT(0 != nRelativeRangeDiff);

			int nDir = m_nActualStepSize / m_nAbsoluteStepSize;

			int nNewInnerBgnStep = m_nInnerBgnStep + nDir * a_nRelativeBgnStep;
			int nNewInnerEndStep = m_nInnerBgnStep + nDir * a_nRelativeEndStep;

			this->CopyTo(a_pDest);
			a_pDest->SetInnerRange(nNewInnerBgnStep, nNewInnerEndStep);
		}

		void Init(int a_nOuterMaxNofSteps, int a_nAbsoluteStepSize)
		{
			Init(a_nOuterMaxNofSteps, a_nAbsoluteStepSize, 0, a_nOuterMaxNofSteps - 1);
		}

		void Init(int a_nOuterMaxNofSteps, int a_nAbsoluteStepSize, int a_nInnerBgnStep, int a_nInnerEndStep)
		{
			Hcpl_ASSERT(a_nOuterMaxNofSteps > 0);
			Hcpl_ASSERT(a_nAbsoluteStepSize > 0);

			m_nOuterMaxNofSteps = a_nOuterMaxNofSteps;
			m_nAbsoluteStepSize = a_nAbsoluteStepSize;
			m_nOuterLimOffset = m_nOuterMaxNofSteps * m_nAbsoluteStepSize;

			SetInnerRange(a_nInnerBgnStep, a_nInnerEndStep);
		}

		void SetInnerRange(int a_nInnerBgnStep, int a_nInnerEndStep)
		{
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
			int nStep = ((a_nOffset - m_nOffsetPart1) % m_nOuterLimOffset) / m_nAbsoluteStepSize;
			return nStep;
		}

		//int GetAbsoluteStepSize()
		//{
		//	return m_nAbsoluteStepSize;
		//}

		//int GetActualStepSize()
		//{
		//	return m_nActualStepSize;
		//}

		//int GetOuterLimOffset()
		//{
		//	return m_nOuterLimOffset;
		//}

		//int GetOuterMaxNofSteps()
		//{
		//	return m_nOuterMaxNofSteps;
		//}

		int GetMaxNofSteps()
		{
			return m_nMaxNofSteps;
		}


	protected:

		int m_nOffsetPart1;

		int m_nAbsoluteStepSize;
		int m_nActualStepSize;

		int m_nOuterMaxNofSteps;
		int m_nOuterLimOffset;

		int m_nMaxNofSteps;
		int m_nInnerBgnStep;
		int m_nInnerEndStep;
	};

	//class OffsetCalc_2D : FRM_Object
	//{
	//public:

	//	OffsetCalc_2D()
	//	{
	//	}

	//	OffsetCalc_2D(int a_nBgnOffset, int a_nAbsoluteStepSize_X, int a_nAbsoluteStepSize_Y)
	//	{
	//		SetBgnOffset(a_nBgnOffset);
	//		
	//		SetAbsoluteStepSize_X(a_nAbsoluteStepSize_X);
	//		SetAbsoluteStepSize_Y(a_nAbsoluteStepSize_Y);
	//	}

	//	int Calc(int a_nStep_X, int a_nStep_Y)
	//	{
	//		return m_nBgnOffset + CalcOffset_X(a_nStep_X) + CalcOffset_Y(a_nStep_Y);
	//	}

	//	int CalcOffset_X(int a_nStep_X)
	//	{
	//		return a_nStep_X * m_nAbsoluteStepSize_X;
	//	}

	//	int CalcOffset_Y(int a_nStep_Y)
	//	{
	//		return a_nStep_Y * m_nAbsoluteStepSize_Y;
	//	}

	//	//int ReverseCalc(int a_nOffset)
	//	//{
	//	//	return (a_nOffset - m_nBgnOffset) / m_nAbsoluteStepSize;
	//	//}

	//	int GetOffset()
	//	{
	//		return m_nBgnOffset;
	//	}

	//	void SetBgnOffset(int a_nBgnOffset)
	//	{
	//		m_nBgnOffset = a_nBgnOffset;
	//	}

	//	int GetAbsoluteStepSize_X()
	//	{
	//		return m_nAbsoluteStepSize_X;
	//	}

	//	void SetAbsoluteStepSize_X(int a_nAbsoluteStepSize_X)
	//	{
	//		m_nAbsoluteStepSize_X = a_nAbsoluteStepSize_X;
	//	}

	//	int GetAbsoluteStepSize_Y()
	//	{
	//		return m_nAbsoluteStepSize_Y;
	//	}

	//	void SetAbsoluteStepSize_Y(int a_nAbsoluteStepSize_Y)
	//	{
	//		m_nAbsoluteStepSize_Y = a_nAbsoluteStepSize_Y;
	//	}

	//protected:

	//	int m_nBgnOffset;

	//	int m_nAbsoluteStepSize_X;
	//	int m_nAbsoluteStepSize_Y;

	//	//bool m_bInitDone;
	//};


	//class IndexCalc2D : FRM_Object
	//{
	//public:

	//	IndexCalc2D()
	//	{
	//		m_bInitDone = false;

	//		m_nSizX = -1;
	//		m_nSizY = -1;
	//		m_nSizTot = -1;
	//	}

	//	IndexCalc2D(int a_nSizX, int a_nSizY)
	//	{
	//		m_bInitDone = false;

	//		Init(a_nSizX, a_nSizY);
	//	}

	//	void Init(int a_nSizX, int a_nSizY)
	//	{
	//		Hcpl_ASSERT(!m_bInitDone);

	//		m_nSizX = a_nSizX;
	//		m_nSizY = a_nSizY;
	//		m_nSizTot = a_nSizX * a_nSizY;

	//		m_bInitDone = true;
	//	}

	//	int Calc(int a_x, int a_y)
	//	{
	//		return a_x + a_y * m_nSizX;
	//	}

	//	int Calc_X(int a_nIdx)
	//	{
	//		return a_nIdx % m_nSizX;
	//	}

	//	int Calc_Y(int a_nIdx)
	//	{
	//		return a_nIdx / m_nSizX;
	//	}

	//	int GetSizeX()
	//	{
	//		return m_nSizX;
	//	}

	//	int GetSizeY()
	//	{
	//		return m_nSizY;
	//	}

	//	int GetTotalSize()
	//	{
	//		return m_nSizTot;
	//	}

	//protected:

	//	int m_nSizX;
	//	int m_nSizY;
	//	int m_nSizTot;

	//	bool m_bInitDone;
	//};

}