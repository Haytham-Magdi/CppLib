#pragma once

//#include <Lib\Cpp\Common\common.h>
#include <Lib\Cpp\Common\Object.h>
#include <Lib\Cpp\Common\ObjRef.h>

namespace Hcpl
{

	class Exception : FRM_Object
	{
	public:

		static void Throw(ObjRef<Exception> a_exp)
		{
		}

		static void Throw(char * a_sText)
		{
		}

	};

}