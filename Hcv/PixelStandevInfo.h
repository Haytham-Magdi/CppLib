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

	class PixelStandevInfo //: FRM_Object
	{
	public:
		int Dir;
		float Val;
		float NormVal;
	};

	//typedef Hcpl::ObjRef< PixelStandevInfo > PixelStandevInfoRef;
}