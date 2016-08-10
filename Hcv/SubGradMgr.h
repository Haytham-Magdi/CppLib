#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

#include <Lib\Hcv\ScanDirMgr.h>

namespace Hcv
{
	//using namespace Hcpl::Math;


	class SubGradMgr : FRM_Object
	{
	public:

		//static void Process( ScanDirMgrRef a_scanMgr, F32ImageRef a_srcImg, 
		static void Process( ScanDirMgr & a_rScanMgr, F32ImageRef a_srcImg, 
			int a_nAprSiz, float * a_retHead, int nStepCnt );

		static F32Point GetPntOfIndex( int a_nIndex, CvSize a_siz );


	protected:


	};


	typedef Hcpl::ObjRef< SubGradMgr > SubGradMgrRef;

}

