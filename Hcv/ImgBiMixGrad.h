#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

#include <Lib\Hcv\IImgDataGradMgr.h>
#include <Lib\Hcv\ImgDataElm_CovMat.h>




namespace Hcv
{
	//using namespace Hcpl::Math;



	class ImgBiMixGrad : FRM_Object
	{
	public:
				

		ImgBiMixGrad( IImgDataGradMgrRef a_gradMgr,
			float a_mainRatio, float a_normRatio );

		F32ImageRef GetResImg_H()
		{
			return m_resImg_H;
		}

		F32ImageRef GetResImg_V()
		{
			return m_resImg_V;
		}

		F32ImageRef GetResImg_ByNormIdx( int a_nNormIdx );
		

	protected:

		void PrepareData();

		F32ImageRef GenResImg( F32ImageRef a_gradImg_Main, 
			F32ImageRef a_gradImg_Norm );


	protected:

		IImgDataGradMgrRef m_gradMgr;

		float m_mainRatio;
		float m_normRatio;

		F32ImageRef m_resImg_H;
		F32ImageRef m_resImg_V;
	};

	typedef Hcpl::ObjRef< ImgBiMixGrad > ImgBiMixGradRef;



}