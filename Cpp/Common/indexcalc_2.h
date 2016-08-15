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

		OffsetCalc_1D(int a_nOuterLimStep, int a_nAbsAbsoluteStepSize)
		{
			Init(a_nOuterLimStep, a_nAbsAbsoluteStepSize);
		}

		void Init(int a_nOuterLimStep, int a_nAbsAbsoluteStepSize, int a_nInnerBgnStep, int a_nInnerEndStep)
		{
			m_nAbsoluteStepSize = a_nAbsAbsoluteStepSize;
			m_nOuterLimStep = a_nOuterLimStep;
			m_nOuterLimOffset = m_nOuterLimStep * m_nAbsoluteStepSize;
		}

		int GetOffsetPart1()
		{
			return m_nOffsetPart1;
		}

		void SetOffsetPart1(int a_nOffsetPart1)
		{
			m_nOffsetPart1 = a_nOffsetPart1;
		}

		int CalcPart2(int a_nStep)
		{
			return a_nStep * m_nAbsoluteStepSize;
		}

		int ReverseCalc(int a_nOffset)
		{
			Hcpl_ASSERT(0 == a_nOffset % m_nAbsoluteStepSize);
			return ((a_nOffset - m_nOffsetPart1) % m_nOuterLimOffset) / m_nAbsoluteStepSize;
		}

		int GetAbsoluteStepSize()
		{
			return m_nAbsoluteStepSize;
		}

		int GetActualStepSize()
		{
			return m_nActualStepSize;
		}

		int GetOuterLimOffset()
		{
			return m_nOuterLimOffset;
		}

		int GetOuterLimStep()
		{
			return m_nOuterLimStep;
		}

	protected:

		int m_nOffsetPart1;

		int m_nAbsoluteStepSize;
		int m_nActualStepSize;

		int m_nOuterLimStep;
		int m_nOuterLimOffset;
	};

	//class OffsetCalc_2D : FRM_Object
	//{
	//public:

	//	OffsetCalc_2D()
	//	{
	//	}

	//	OffsetCalc_2D(int a_nBgnOffset, int a_nAbsAbsoluteStepSize_X, int a_nAbsAbsoluteStepSize_Y)
	//	{
	//		SetBgnOffset(a_nBgnOffset);
	//		
	//		SetAbsoluteStepSize_X(a_nAbsAbsoluteStepSize_X);
	//		SetAbsoluteStepSize_Y(a_nAbsAbsoluteStepSize_Y);
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

	//	void SetAbsoluteStepSize_X(int a_nAbsAbsoluteStepSize_X)
	//	{
	//		m_nAbsoluteStepSize_X = a_nAbsAbsoluteStepSize_X;
	//	}

	//	int GetAbsoluteStepSize_Y()
	//	{
	//		return m_nAbsoluteStepSize_Y;
	//	}

	//	void SetAbsoluteStepSize_Y(int a_nAbsAbsoluteStepSize_Y)
	//	{
	//		m_nAbsoluteStepSize_Y = a_nAbsAbsoluteStepSize_Y;
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