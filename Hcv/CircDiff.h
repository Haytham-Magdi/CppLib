#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

namespace Hcv
{
	//using namespace Hcpl::Math;


	class CircDiff : FRM_Object
	{
	private:
		CircDiff();

	public:
		static F32ImageRef GenResult( F32ImageRef a_src, int a_nAprSiz );
	};


	//typedef Hcpl::ObjRef< CircDiff > CircDiffRef;

}