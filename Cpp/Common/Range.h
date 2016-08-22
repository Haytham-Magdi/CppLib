#pragma once

#pragma inline_depth( 255 )

#include <stdlib.h>


#include <Lib\Cpp\Common\Debug.h>


namespace Hcpl
{
	template<class T>
	class Range
	{
	public:

		Range()
		{
		}

		void Init(T a_bgn, T a_end)
		{
			m_bgn = a_bgn;
			m_end = a_end;
		}

		static Range & New(T a_bgn, T a_end)
		{
			Range ret;
			ret.Init(a_bgn, a_end);

			return ret;
		}

	protected:

		T m_bgn;
		T m_end;
	};
	
	//class IntRange : public Range<int>
	//{
	//public:
	//	int GetLength()
	//	{
	//		return 
	//	}
	//};

}