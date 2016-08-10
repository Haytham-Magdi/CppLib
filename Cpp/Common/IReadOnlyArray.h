#pragma once

#pragma inline_depth( 255 )

#include <stdlib.h>



#include <Lib\Cpp\Common\Debug.h>
#include <Lib\Cpp\Common\Object.h>


namespace Hcpl
{
	template<class T>
	class IReadOnlyArray : FRM_Object
	{
	public:
		virtual T GetAt(int a_index) = 0;
		virtual int GetSize() = 0;
	};


	#define IReadOnlyArray_REF(T) Hcpl::ObjRef< IReadOnlyArray<T> >



}