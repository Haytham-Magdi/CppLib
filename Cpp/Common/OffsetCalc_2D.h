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
		}

		OffsetCalc_2D(int a_nBgnOffset, int a_nAbsoluteStepSize_X, int a_nAbsoluteStepSize_Y)
		{
			throw "Not Implemented!";

			SetBgnOffset(a_nBgnOffset);
			
			SetAbsoluteStepSize_X(a_nAbsoluteStepSize_X);
			SetAbsoluteStepSize_Y(a_nAbsoluteStepSize_Y);
		}

		int Calc(int a_nStep_X, int a_nStep_Y)
		{
			throw "Not Implemented!";
			return m_nBgnOffset + CalcOffset_X(a_nStep_X) + CalcOffset_Y(a_nStep_Y);
		}

		int CalcOffset_X(int a_nStep_X)
		{
			throw "Not Implemented!";
			return a_nStep_X * m_nAbsoluteStepSize_X;
		}

		int CalcOffset_Y(int a_nStep_Y)
		{
			throw "Not Implemented!";
			return a_nStep_Y * m_nAbsoluteStepSize_Y;
		}

		//int ReverseCalc(int a_nOffset)
		//{
		//throw "Not Implemented!";
		//	return (a_nOffset - m_nBgnOffset) / m_nAbsoluteStepSize;
		//}

		int GetOffset()
		{
			throw "Not Implemented!";
			return m_nBgnOffset;
		}

		void SetBgnOffset(int a_nBgnOffset)
		{
			throw "Not Implemented!";
			m_nBgnOffset = a_nBgnOffset;
		}

		int GetAbsoluteStepSize_X()
		{
			throw "Not Implemented!";
			return m_nAbsoluteStepSize_X;
		}

		void SetAbsoluteStepSize_X(int a_nAbsoluteStepSize_X)
		{
			throw "Not Implemented!";
			m_nAbsoluteStepSize_X = a_nAbsoluteStepSize_X;
		}

		int GetAbsoluteStepSize_Y()
		{
			throw "Not Implemented!";
			return m_nAbsoluteStepSize_Y;
		}

		void SetAbsoluteStepSize_Y(int a_nAbsoluteStepSize_Y)
		{
			throw "Not Implemented!";
			m_nAbsoluteStepSize_Y = a_nAbsoluteStepSize_Y;
		}

	protected:

		int m_nBgnOffset;

		int m_nAbsoluteStepSize_X;
		int m_nAbsoluteStepSize_Y;

		//bool m_bInitDone;
	};


	class IndexCalc2D : FRM_Object
	{
	public:

		IndexCalc2D()
		{
			m_bInitDone = false;

			m_nSizX = -1;
			m_nSizY = -1;
			m_nSizTot = -1;
		}

		IndexCalc2D(int a_nSizX, int a_nSizY)
		{
			m_bInitDone = false;

			Init(a_nSizX, a_nSizY);
		}

		void Init(int a_nSizX, int a_nSizY)
		{
			Hcpl_ASSERT(!m_bInitDone);

			m_nSizX = a_nSizX;
			m_nSizY = a_nSizY;
			m_nSizTot = a_nSizX * a_nSizY;

			m_bInitDone = true;
		}

		int Calc(int a_x, int a_y)
		{
			return a_x + a_y * m_nSizX;
		}

		int Calc_X(int a_nIdx)
		{
			return a_nIdx % m_nSizX;
		}

		int Calc_Y(int a_nIdx)
		{
			return a_nIdx / m_nSizX;
		}

		OffsetCalc_1D & GetOffsetCalc_X()
		{
			return m_offsetCalc_X;
		}

		OffsetCalc_1D & GetOffsetCalc_Y()
		{
			return m_offsetCalc_Y;
		}

		//int GetSizeX()
		//{
		//	return m_offsetCalc_X.GetMaxNofSteps();
		//}

		//int GetSizeY()
		//{
		//	return m_nSizY;
		//}

		int GetTotalSize()
		{
			return m_nSizTot;
		}

	protected:

		OffsetCalc_1D m_offsetCalc_X;
		OffsetCalc_1D m_offsetCalc_Y;

		//int m_nSizX;
		//int m_nSizY;
		//int m_nSizTot;

		//bool m_bInitDone;
	};

}