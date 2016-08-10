#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <queue>      


namespace Hcv
{
	using namespace Hcpl::Math;
	using namespace std;

	class IFilt1D2 : FRM_Object
	{
	public:

		virtual void Proceed() = 0;
		virtual int GetAprSizeOrg() = 0;
		virtual int GetAprSize() = 0;
		virtual void SetAprSize( int a_nAprSiz ) = 0;
		virtual int GetProcCnt() = 0;


	protected:

	};

	typedef Hcpl::ObjRef< IFilt1D2 > IFilt1D2Ref;



}
