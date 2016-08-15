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

		OffsetCalc_1D(int a_nLimStep, int a_nStepLen)
		{
			Init(a_nLimStep, a_nStepLen);
		}

		void Init(int a_nLimStep, int a_nStepLen)
		{
			m_nStepLen = a_nStepLen;
			m_nLimStep = a_nLimStep;
			m_nLimOffset = m_nLimStep * m_nStepLen;
		}

		int Calc(int a_nStep)
		{
			return a_nStep * m_nStepLen;
		}

		int ReverseCalc(int a_nOffset)
		{
			Hcpl_ASSERT(0 == a_nOffset % m_nStepLen);
			return (a_nOffset % m_nLimOffset) / m_nStepLen;
		}

		int GetStepLen()
		{
			return m_nStepLen;
		}

		int GetLimOffset()
		{
			return m_nLimOffset;
		}

		int GetLimStep()
		{
			return m_nLimStep;
		}

	protected:

		int m_nStepLen;
		int m_nLimStep;
		int m_nLimOffset;
	};

	//class OffsetCalc_2D : FRM_Object
	//{
	//public:

	//	OffsetCalc_2D()
	//	{
	//	}

	//	OffsetCalc_2D(int a_nBgnOffset, int a_nStepLen_X, int a_nStepLen_Y)
	//	{
	//		SetBgnOffset(a_nBgnOffset);
	//		
	//		SetStepLen_X(a_nStepLen_X);
	//		SetStepLen_Y(a_nStepLen_Y);
	//	}

	//	int Calc(int a_nStep_X, int a_nStep_Y)
	//	{
	//		return m_nBgnOffset + CalcOffset_X(a_nStep_X) + CalcOffset_Y(a_nStep_Y);
	//	}

	//	int CalcOffset_X(int a_nStep_X)
	//	{
	//		return a_nStep_X * m_nStepLen_X;
	//	}

	//	int CalcOffset_Y(int a_nStep_Y)
	//	{
	//		return a_nStep_Y * m_nStepLen_Y;
	//	}

	//	//int ReverseCalc(int a_nOffset)
	//	//{
	//	//	return (a_nOffset - m_nBgnOffset) / m_nStepLen;
	//	//}

	//	int GetOffset()
	//	{
	//		return m_nBgnOffset;
	//	}

	//	void SetBgnOffset(int a_nBgnOffset)
	//	{
	//		m_nBgnOffset = a_nBgnOffset;
	//	}

	//	int GetStepLen_X()
	//	{
	//		return m_nStepLen_X;
	//	}

	//	void SetStepLen_X(int a_nStepLen_X)
	//	{
	//		m_nStepLen_X = a_nStepLen_X;
	//	}

	//	int GetStepLen_Y()
	//	{
	//		return m_nStepLen_Y;
	//	}

	//	void SetStepLen_Y(int a_nStepLen_Y)
	//	{
	//		m_nStepLen_Y = a_nStepLen_Y;
	//	}

	//protected:

	//	int m_nBgnOffset;

	//	int m_nStepLen_X;
	//	int m_nStepLen_Y;

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