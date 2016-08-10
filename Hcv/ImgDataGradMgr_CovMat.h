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


	class ImgDataGradMgr_CovMat_Factory : public IImgDataGradMgr_Factory
	{
	public:

		virtual IImgDataGradMgr * CreateImgDataGradMgr( 
			 IImgDataMgr_2Ref a_imgData, ImageNbrMgrRef a_nbrMgr );
	};

	typedef Hcpl::ObjRef< ImgDataGradMgr_CovMat_Factory > ImgDataGradMgr_CovMat_FactoryRef;



	class ImgDataGradMgr_CovMat : public IImgDataGradMgr
	{
	public:

		ImgDataGradMgr_CovMat( 
			IImgDataMgr_2Ref a_imgData, ImageNbrMgrRef a_nbrMgr );

		virtual IImgDataMgr_2Ref GetSrcImgDataMgr()
		{
			return m_imgDataMgr;
		}

		virtual F32ImageRef GetGradImg_H()
		{
			return m_gradImg_H;
		}

		virtual F32ImageRef GetGradImg_V()
		{
			return m_gradImg_V;
		}

		virtual F32ImageRef GetConflictImg_H()
		{
			ThrowHcvException();

			return NULL;
		}

		virtual F32ImageRef GetConflictImg_V()
		{
			ThrowHcvException();

			return NULL;
		}

		virtual ImageNbrMgrRef GetNbrMgr()
		{
			return m_nbrMgr;
		}

		virtual int GetAprSiz()
		{
			return m_nAprSiz;
		}

		virtual F32ImageRef GetGradImg_ByNormIdx( int a_nNormIdx );

		virtual F32ImageRef GetConfImg_ByNormIdx( int a_nNormIdx );

		virtual float GetMaxDif()
		{
			return S_GetMaxDif();
		}

		static float S_GetMaxDif()
		{
			return 550;
		}


	protected:

		void PrepareData();

		F32ImageRef GenGradImg( 
			FixedVector< ImageNbrMgr::PixIndexNbr > & a_rNbrArr );


	protected:

		IImgDataMgr_2Ref m_imgDataMgr;

		F32ImageRef m_gradImg_H;
		F32ImageRef m_gradImg_V;

		F32ImageRef m_confImg_H;
		F32ImageRef m_confImg_V;

		ImageNbrMgrRef m_nbrMgr;
		
		int m_nAprSiz;
	};

	typedef Hcpl::ObjRef< ImgDataGradMgr_CovMat > ImgDataGradMgr_CovMatRef;



}