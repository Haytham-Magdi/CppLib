#pragma once

#pragma inline_depth( 255 )

#include <stdlib.h>


#include <Lib\Cpp\Common\Debug.h>


namespace Hcpl
{
	class CircIndex : FRM_Object
	{
	public :

		CircIndex(int a_nSize) : m_nSize(a_nSize)
		{
		}

		int Calc(int a_idx)
		{
			return (a_idx + m_nSize) % m_nSize;
		}

		int GetSize()
		{
			return m_nSize;
		}

	protected:

		const int m_nSize;
	};

}