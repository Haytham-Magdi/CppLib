#pragma once

#pragma inline_depth( 255 )

#include <stdlib.h>


#include <Lib\Cpp\Common\Debug.h>
#include <Lib\Cpp\Common\OffsetCalc_2D.h>


namespace Hcpl
{

	template<class T>
	class MemAccessor_2D : FRM_Object
	{
	public:

		void CopyTo(MemAccessor_2D<T> a_pDest)
		{
			*pDest = *this;
		}

		void Init(T * a_data, int a_nAbsoluteStepSize_X, int a_nOuterMaxNofSteps_X, int a_nOuterMaxNofSteps_Y)
		{
			m_data = a_data;

			m_offsetCalc.Init(a_nAbsoluteStepSize_X, a_nOuterMaxNofSteps_X, a_nOuterMaxNofSteps_Y);
		}

		OffsetCalc_2D & GetOffsetCalc()
		{
			return m_offsetCalc_Y;
		}

		T * GetData()
		{
			return m_data;
		}

	protected:

		T * m_data;
		OffsetCalc_2D m_offsetCalc;
	};

}