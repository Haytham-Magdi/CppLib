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
			//m_bInitDone = false;

			//m_nBgnOffset = -1;
			//m_nStep = -1;
		}

		OffsetCalc_1D(int a_nBgnOffset, int a_nStep)
		{
			//m_bInitDone = false;

			//Init(a_nBgnOffset, a_nStep);
			//Prepare(a_nBgnOffset, a_nStep);

			SetBgnOffset(a_nBgnOffset);
			SetStep(a_nStep);
		}

		int Calc(int a_nSimpleIdx)
		{
			return m_nBgnOffset + a_nSimpleIdx * m_nStep;
		}

		int ReverseCalc(int a_nOffset)
		{
			return (a_nOffset - m_nBgnOffset) / m_nStep;
		}

		int GetOffset()
		{
			return m_nBgnOffset;
		}

		void SetBgnOffset(int a_nBgnOffset)
		{
			m_nBgnOffset = a_nBgnOffset;
		}

		int GetStep()
		{
			return m_nStep;
		}

		void SetStep(int a_nStep)
		{
			m_nStep = a_nStep;
		}

	protected:

		int m_nBgnOffset;
		int m_nStep;

		//bool m_bInitDone;
	};

	class OffsetCalc_2D : FRM_Object
	{
	public:

		OffsetCalc_2D()
		{
		}

		OffsetCalc_2D(int a_nBgnOffset, int a_nStep_X, int a_nStep_Y)
		{
			SetBgnOffset(a_nBgnOffset);
			
			SetStep_X(a_nStep_X);
			SetStep_Y(a_nStep_Y);
		}

		int Calc(int a_nSimpleIdx_X, int a_nSimpleIdx_Y)
		{
			return m_nBgnOffset + CalcOffsetDiff_X(a_nSimpleIdx_X) + CalcOffsetDiff_Y(a_nSimpleIdx_Y);
		}

		int CalcOffsetDiff_X(int a_nSimpleIdx_X)
		{
			return a_nSimpleIdx_X * m_nStep_X;
		}

		int CalcOffsetDiff_Y(int a_nSimpleIdx_Y)
		{
			return a_nSimpleIdx_Y * m_nStep_Y;
		}

		//int ReverseCalc(int a_nOffset)
		//{
		//	return (a_nOffset - m_nBgnOffset) / m_nStep;
		//}

		int GetOffset()
		{
			return m_nBgnOffset;
		}

		void SetBgnOffset(int a_nBgnOffset)
		{
			m_nBgnOffset = a_nBgnOffset;
		}

		int GetStep_X()
		{
			return m_nStep_X;
		}

		void SetStep_X(int a_nStep_X)
		{
			m_nStep_X = a_nStep_X;
		}

		int GetStep_Y()
		{
			return m_nStep_Y;
		}

		void SetStep_Y(int a_nStep_Y)
		{
			m_nStep_Y = a_nStep_Y;
		}

	protected:

		int m_nBgnOffset;

		int m_nStep_X;
		int m_nStep_Y;

		//bool m_bInitDone;
	};


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