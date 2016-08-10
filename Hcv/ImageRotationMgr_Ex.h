#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

#include <Lib\Hcv\ImageLineItr.h>
#include <Lib\Hcv\ImageLineItrProvider.h>
#include <Lib\Hcv\ImgRotationMgr.h>
#include <Lib\Hcv\IImgDataMgr_2_FactorySet.h>
#include <Lib\Hcv\ImgBiMixGrad.h>



namespace Hcv
{
	//using namespace Hcpl::Math;


	class ImageRotationMgr_ExParams : FRM_Object
	{
	public:

		ImageRotationMgr_ExParams( F32ImageRef a_srcImg, IImgCoreSrcRef a_ids,
			float a_angDig, int a_nAprSiz_Far, int a_nAprSiz_Loc,
			IImgDataMgr_2_FactorySetRef a_imgFactorySet );


	//protected:

		F32ImageRef SrcImg;
		IImgCoreSrcRef m_ids;
		float AngDig;
		int nAprSiz_Far;
		int nAprSiz_Loc;
		IImgDataMgr_2_FactorySetRef ImgFactorySet;

	};

	typedef Hcpl::ObjRef< ImageRotationMgr_ExParams > ImageRotationMgr_ExParamsRef;


	class ImageRotationMgr_Ex : FRM_Object
	{
	public:

		ImageRotationMgr_Ex( ImageRotationMgr_ExParamsRef a_params )
		{
			Init( a_params->SrcImg, a_params->m_ids, a_params->AngDig,
				a_params->nAprSiz_Far, a_params->nAprSiz_Loc,
				a_params->ImgFactorySet );
		}
		
		ImageRotationMgr_Ex( F32ImageRef a_srcImg, float a_angDig,
			int a_nAprSiz_Far, int a_nAprSiz_Loc,
			IImgDataMgr_2_FactorySetRef a_imgFactorySet )
		{
			ThrowHcvException();

			Init( a_srcImg, NULL, a_angDig,
				a_nAprSiz_Far, a_nAprSiz_Loc,
				a_imgFactorySet );
		}




		ImgRotationMgrRef GetCoreRotMgr()
		{
			return m_coreRotMgr;
		}

		int GetAprSiz_Far()
		{
			return m_nAprSiz_Far;
		}

		int GetAprSiz_Loc()
		{
			return m_nAprSiz_Loc;
		}
		
		IImgDataGradMgrRef GetGradMgr_Far()
		{
			return m_gradMgr_Far;
		}

		IImgDataGradMgrRef GetGradMgr_Loc()
		{
			return m_gradMgr_Loc;
		}		

		ImageNbrMgrRef GetImgNbrMgr_Loc()
		{
			return m_imgNbrMgr_Loc;
		}

		ImgBiMixGradRef GetBiMixGrad_Loc()
		{
			return m_biMixGrad_Loc;
		}



	protected:

		void Init( F32ImageRef a_srcImg, IImgCoreSrcRef a_ids1, 
			float a_angDig, int a_nAprSiz_Far, int a_nAprSiz_Loc,
			IImgDataMgr_2_FactorySetRef a_imgFactorySet );

		void Prepare();

	protected:

		ImgRotationMgrRef m_coreRotMgr;
		IImgDataMgr_2_FactorySetRef m_imgFactorySet;

		IImgCoreSrcRef m_ids;

		int m_nAprSiz_Far;
		int m_nAprSiz_Loc;

		IImgDataMgr_2Ref m_imgDataMgr;

		IImgDataGradMgrRef m_gradMgr_Far; 
		IImgDataGradMgrRef m_gradMgr_Loc; 

		ImageNbrMgrRef m_imgNbrMgr_Loc;

		ImgBiMixGradRef m_biMixGrad_Loc;
	};


	typedef Hcpl::ObjRef< ImageRotationMgr_Ex > ImageRotationMgr_ExRef;

}

