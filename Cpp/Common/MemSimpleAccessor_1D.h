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

		void Init(T * a_data, int a_nIndexSize, int a_nStepSize)
		{
			m_data = a_data;
			m_nIndexSize = a_nIndexSize;
			m_nStepSize = a_nStepSize;
		}

		T & operator[](int a_pos)
		{
			Hcpl_ASSERT(a_pos >= 0);
			Hcpl_ASSERT(a_pos < m_nIndexSize);

			return m_data[a_pos * m_nStepSize];
		}

		int GetSize()
		{
			return m_nIndexSize;
		}

	protected:

		T * m_data;
		int m_nStepSize;
		int m_nIndexSize;
	};
	
}