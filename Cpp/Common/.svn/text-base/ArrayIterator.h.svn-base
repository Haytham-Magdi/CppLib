#pragma once

#pragma inline_depth( 255 )

#include <stdlib.h>



#include <Lib\Cpp\Common\Debug.h>
#include <Lib\Cpp\Common\IIterator.h>
#include <Lib\Cpp\Common\IReadOnlyArray.h>


namespace Hcpl
{
	template<class T>
	class ArrayIterator : IIterator< T >
	{
	public:

		ArrayIterator( IReadOnlyArray_REF(T) a_array )
		{
			m_array = a_array;
			m_index = 0;
		}

		virtual bool MoveNext()
		{
			if( m_index >= m_array->GetSize() - 1 )
				return false;
		}

		virtual T GetCurrent()
		{
			return m_array->GetAt(m_index);
		}

	protected:

		int m_index;
		IReadOnlyArray_REF(T) m_array;
	};


	#define ArrayIterator_REF(T) Hcpl::ObjRef< ArrayIterator<T> >



}