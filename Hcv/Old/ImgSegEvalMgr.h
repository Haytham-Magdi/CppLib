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
#include <Lib\Hcv\ScanDirMgrColl.h>
#include <Lib\Hcv\Signal1D.h>

#include <Lib\Hcv\IImgDataMgr.h>
#include <Lib\Hcv\ImgGradMgr.h>


#include <Lib\Hcv\ImgSegDataMgr.h>




namespace Hcv
{
	//using namespace Hcpl::Math;


	class ImgSeg_EvalMgr : FRM_Object
	{
	public:

		ImgSeg_EvalMgr();



	protected:

		void Proceed();


	protected:

		char m_sFilePath[2000];
		char m_sFileName[1000];

		FixedVector< ImgSegDataMgrRef > m_human_SegDataMgr_Arr;

	};


	typedef Hcpl::ObjRef< ImgSeg_EvalMgr > ImgSeg_EvalMgrRef;

}

