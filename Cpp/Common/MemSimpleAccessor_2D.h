#pragma once

#pragma inline_depth( 255 )

#include <stdlib.h>


#include <Lib\Cpp\Common\Debug.h>


namespace Hcpl
{
	template<class T>
	class MemSimpleAccessor_2D
	{
	public:

		MemSimpleAccessor_2D()
		{
		}

		void Init(T * a_data, int a_nStepSize_X, int a_nStepSize_Y)
		{
			T * m_data = a_data;
			m_nStepSize = a_nStepSize;
		}

		//T & operator[](int a_pos_X, int a_pos_Y)
		T & GetAt(int a_pos_X, int a_pos_Y)
		{
			return m_data[a_pos_X * m_nStepSize_X + a_pos_Y * m_nStepSize_Y];
		}

	protected:

		T * m_data;
		int m_nStepSize_X;
		int m_nStepSize_Y;
	};

}