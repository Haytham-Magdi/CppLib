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

	class ConflictInfo //: FRM_Object
	{
	public:
		bool Exists;
		int Offset_Side_1;
		int Offset_Side_2;
	};

	class ConflictInfo_Ex : public ConflictInfo//: FRM_Object
	{
	public:
		int Dir;
		//float NormVal;
	};

	//typedef Hcpl::ObjRef< ConflictInfo > ConflictInfoRef;
}