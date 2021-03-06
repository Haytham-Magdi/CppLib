#pragma once

#pragma inline_depth( 255 )

#include <stdlib.h>


#include <Lib\Cpp\Common\Debug.h>
#include <Lib\Cpp\Common\Range.h>


namespace Hcpl
{
	template<class T>
	class Window
	{
	public:

		Window()
		{
		}

		void Init(T a_x1, T a_x2, T a_y1, T a_y2)
		{
			m_x1 = a_x1;
			m_x2 = a_x2;

			m_y1 = a_y1;
			m_y2 = a_y2;
		}

		static Window New(T a_x1, T a_x2, T a_y1, T a_y2)
		{
			Window ret;
			ret.Init(a_x1, a_x2, a_y1, a_y2);

			return ret;
		}

		T Get_X1()
		{
			return m_x1;
		}

		T Get_X2()
		{
			return m_x2;
		}

		T Get_Y1()
		{
			return m_y1;
		}

		T Get_Y2()
		{
			return m_y2;
		}

		Range<T> GetRange_X()
		{
			return Range<T>::New(m_x1, m_x2);
		}

		Range<T> GetRange_Y()
		{
			return Range<T>::New(m_y1, m_y2);
		}

	protected:

		T m_x1;
		T m_x2;

		T m_y1;
		T m_y2;
	};
	
}