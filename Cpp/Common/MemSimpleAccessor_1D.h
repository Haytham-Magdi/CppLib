#pragma once

#pragma inline_depth( 255 )

#include <stdlib.h>


#include <Lib\Cpp\Common\Debug.h>


namespace Hcpl
{
	template<class T>
	class MemSimpleAccessor_1D
	{
	public:

		MemSimpleAccessor_1D()
		{
		}

		void Init(T * a_data, int a_nMaxNofSteps, int a_nStepSize)
		{
			m_data = a_data;
			m_nMaxNofSteps = a_nMaxNofSteps;
			m_nStepSize = a_nStepSize;
		}

		T & operator[](int a_pos)
		{
			Hcpl_ASSERT(a_pos >= 0);
			Hcpl_ASSERT(a_pos < m_nMaxNofSteps);

			return m_data[a_pos * m_nStepSize];
		}

		int GetSize()
		{
			return m_nMaxNofSteps;
		}

	protected:

		T * m_data;
		int m_nStepSize;
		int m_nMaxNofSteps;
	};
	
}