#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

#include <Lib\Hcv\IImgDataMgr_2.h>

#include <Lib\Hcv\ImageNbrMgr.h>


namespace Hcv
{
	//using namespace Hcpl::Math;

	class IImgDataGradMgr : FRM_Object
	{
	public:

		virtual IImgDataMgr_2Ref GetSrcImgDataMgr() = 0;

		virtual F32ImageRef GetGradImg_H() = 0;

		virtual F32ImageRef GetGradImg_V() = 0;

		virtual F32ImageRef GetConflictImg_H() = 0;

		virtual F32ImageRef GetConflictImg_V() = 0;

		virtual F32ImageRef GetGradImg_ByNormIdx( int a_nNormIdx ) = 0;

		virtual F32ImageRef GetConfImg_ByNormIdx( int a_nNormIdx ) = 0;
		
		virtual float GetMaxDif() = 0;

		virtual ImageNbrMgrRef GetNbrMgr() = 0;

		virtual int GetAprSiz() = 0;

	};

	typedef Hcpl::ObjRef< IImgDataGradMgr > IImgDataGradMgrRef;


	class IImgDataGradMgr_Factory : FRM_Object
	{
	public:

		virtual IImgDataGradMgr * CreateImgDataGradMgr( 
			 IImgDataMgr_2Ref a_imgData, ImageNbrMgrRef a_nbrMgr ) = 0;
	};

	typedef Hcpl::ObjRef< IImgDataGradMgr_Factory > IImgDataGradMgr_FactoryRef;

}