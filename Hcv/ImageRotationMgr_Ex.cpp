#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\ImageRotationMgr_Ex.h>

namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;


	ImageRotationMgr_ExParams::ImageRotationMgr_ExParams( 
		F32ImageRef a_srcImg, IImgCoreSrcRef a_ids,
		float a_angDig, int a_nAprSiz_Far, int a_nAprSiz_Loc,
			IImgDataMgr_2_FactorySetRef a_imgFactorySet )
	{
		SrcImg = a_srcImg;
		m_ids = a_ids;
		AngDig = a_angDig;
		nAprSiz_Far = a_nAprSiz_Far;
		nAprSiz_Loc = a_nAprSiz_Loc;
		ImgFactorySet = a_imgFactorySet;
	}


	void ImageRotationMgr_Ex::Init( F32ImageRef a_srcImg, IImgCoreSrcRef a_ids1, 
		float a_angDig, int a_nAprSiz_Far, int a_nAprSiz_Loc,
			IImgDataMgr_2_FactorySetRef a_imgFactorySet )
	{
		m_nAprSiz_Far = a_nAprSiz_Far;
		m_nAprSiz_Loc = a_nAprSiz_Loc;
		
		m_imgFactorySet = a_imgFactorySet;		

		m_coreRotMgr = new ImgRotationMgr( a_srcImg, a_angDig );
		
		m_ids = a_ids1;

		Prepare();
	}


	void ImageRotationMgr_Ex::Prepare()
	{
		m_imgDataMgr = m_imgFactorySet->GetDataMgr_Factory(
			)->CreateImgDataMgr( m_coreRotMgr->GetResImg(), 
			m_nAprSiz_Loc, m_ids, m_coreRotMgr );

		m_gradMgr_Far = m_imgFactorySet->GetGradMgr_Factory(
			)->CreateImgDataGradMgr( m_imgDataMgr, 
			new ImageNbrMgr( m_coreRotMgr->GetImageItrMgr(), m_nAprSiz_Far ));

		m_imgNbrMgr_Loc = new ImageNbrMgr( 
			m_coreRotMgr->GetImageItrMgr(), m_nAprSiz_Loc );

		m_gradMgr_Loc = m_imgFactorySet->GetGradMgr_Factory(
			)->CreateImgDataGradMgr( m_imgDataMgr, m_imgNbrMgr_Loc );

		//m_biMixGrad_Loc = new ImgBiMixGrad( m_gradMgr_Loc, 1, 2 );
		m_biMixGrad_Loc = new ImgBiMixGrad( m_gradMgr_Loc, 1, 1 );

		//if( false )
		//{
		//	//F32ImageRef img1 = m_gradMgr_Far->GetGradImg_H();
		//	//F32ImageRef img1 = m_gradMgr_Loc->GetGradImg_H();

		//	F32ImageRef img1 = m_biMixGrad_Loc->GetResImg_H();
		//	
		//					
		//	img1 = GenTriChGrayImg( img1 );
		//	img1 = GenMultByNumImg( img1, 0.5 );

		//	GenClippedPixValImg( img1, 255, 0 );

		//	//HCV_CALL(				
		//	//	cvDiv( 2, img1->GetIplImagePtr(), img1->GetIplImagePtr() )
		//	//	);

		//	//ShowImage( img1, "m_gradMgr_Far H" );

		//	GlobalStuff::SetLinePathImg( img1 );
		//	GlobalStuff::ShowLinePathImg();
		//}

	}


}