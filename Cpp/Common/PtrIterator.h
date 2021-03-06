#pragma once

#pragma inline_depth( 255 )

#include <stdlib.h>


#include <Lib\Cpp\Common\Debug.h>
#include <Lib\Cpp\Common\OffsetCalc_1D.h>


namespace Hcpl
{

	template<class T>
	class PtrIterator //: FRM_Object
	{
	public:

		PtrIterator(T * a_bgn, T * a_end, int a_nStepSize)
		{
			m_current = a_bgn;
			m_lim = a_end + a_nStepSize;
			m_nStepSize = a_nStepSize;
		}

		bool IsDone()
		{
			return m_current == m_lim;
		}

		//T * Next()
		void Next()
		{
			Hcpl_ASSERT(!IsDone());

			m_current += m_nStepSize;
			//return m_current;
		}

		T * GetCurrent()
		{
			return m_current;
		}

	protected:

		T * m_current;
		T * m_lim;
		int m_nStepSize;
	};

}