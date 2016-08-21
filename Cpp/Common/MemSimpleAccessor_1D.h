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

		void Init(T * a_data, int a_nStepSize)
		{
			T * m_data = a_data;
			m_nStepSize = a_nStepSize;
		}

		T & operator[](int a_pos)
		{
			return m_data[a_pos * m_nStepSize];
		}

	protected:

		T * m_data;
		int m_nStepSize;
	};
	
}