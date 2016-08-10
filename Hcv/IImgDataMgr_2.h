#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

#include <Lib\Hcv\ImgRotationMgr.h>


namespace Hcv
{
	//using namespace Hcpl::Math;

	class IImgCoreSrc : FRM_Object
	{
	public:

		//virtual F32ImageRef GetSrcImg() = 0;

		//virtual int GetAprSiz() = 0;

	};

	typedef Hcpl::ObjRef< IImgCoreSrc > IImgCoreSrcRef;


	class IImgDataMgr_2 : FRM_Object
	{
	public:

		virtual F32ImageRef GetSrcImg() = 0;

		virtual int GetAprSiz() = 0;

	};

	typedef Hcpl::ObjRef< IImgDataMgr_2 > IImgDataMgr_2Ref;


	class IImgDataMgr_2_Factory : FRM_Object
	{
	public:

		virtual IImgDataMgr_2 * CreateImgDataMgr( 
			 F32ImageRef a_srcImg, int a_nAprSiz, 
			 IImgCoreSrcRef a_dataSrc,
			 ImgRotationMgrRef a_rot ) = 0;

	};



	typedef Hcpl::ObjRef< IImgDataMgr_2_Factory > IImgDataMgr_2_FactoryRef;

}