#pragma once

#pragma inline_depth( 255 )

#include <stdlib.h>



#include <Lib\Cpp\Common\Debug.h>
#include <Lib\Cpp\Common\Object.h>


namespace Hcpl
{
	template<class T>
	class IIterator : FRM_Object
	{
	public:
		virtual bool MoveNext() = 0;
		virtual T GetCurrent() = 0;
	};


	#define IIterator_REF(T) Hcpl::ObjRef< IIterator<T> >



}