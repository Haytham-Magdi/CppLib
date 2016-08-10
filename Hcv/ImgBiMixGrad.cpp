#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\ImgBiMixGrad.h>

#include <Lib\Hcv\ImgDataMgr_2_CovMat.h>



namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;



	ImgBiMixGrad::ImgBiMixGrad( IImgDataGradMgrRef a_gradMgr,
		float a_mainRatio, float a_normRatio )
	{
		m_gradMgr = a_gradMgr;

		Hcpl_ASSERT( a_mainRatio >= 0 );
		Hcpl_ASSERT( a_normRatio >= 0 );

		float ratioSum = a_mainRatio + a_normRatio;

		Hcpl_ASSERT( ratioSum > 0.1 );

		m_mainRatio = a_mainRatio / ratioSum;

		m_normRatio = a_normRatio / ratioSum;

		PrepareData();
	}

	
	void ImgBiMixGrad::PrepareData()
	{
		m_resImg_H = GenResImg( m_gradMgr->GetGradImg_H(),
			m_gradMgr->GetGradImg_V() );
		
		m_resImg_V = GenResImg( m_gradMgr->GetGradImg_V(),
			m_gradMgr->GetGradImg_H() );
	}

	F32ImageRef ImgBiMixGrad::GenResImg( F32ImageRef a_gradImg_Main, 
			F32ImageRef a_gradImg_Norm )
	{

		CvSize imgSiz_Main = a_gradImg_Main->GetSize();

		CvSize imgSiz_Norm = a_gradImg_Main->GetSize();

		Hcpl_ASSERT( imgSiz_Main.width = imgSiz_Norm.width );
		Hcpl_ASSERT( imgSiz_Main.height = imgSiz_Norm.height );

		int nImgSiz1D = imgSiz_Main.width * imgSiz_Main.height;

		F32ImageRef resImg = F32Image::Create( imgSiz_Main, 1 );

		float * resImg_Buf = resImg->GetPixAt( 0, 0 );

		float * mainGrad_Buf = a_gradImg_Main->GetPixAt( 0, 0 );
		float * normGrad_Buf = a_gradImg_Norm->GetPixAt( 0, 0 );


		for( int i=0; i < nImgSiz1D; i++ )
		{
			float mainVal = mainGrad_Buf[ i ];
			float normVal = normGrad_Buf[ i ];

			float resVal = mainVal * m_mainRatio +
				normVal * m_normRatio;

			resImg_Buf[ i ] = resVal;
		}
				
		return resImg;
	}


	F32ImageRef ImgBiMixGrad::GetResImg_ByNormIdx( int a_nNormIdx )
	{
		if( 0 == a_nNormIdx )
		{
			return m_resImg_H;
		}
		else if( 1 == a_nNormIdx )
		{
			return m_resImg_V;
		}
		else
		{
			ThrowHcvException();
			return m_resImg_H;
		}
	}

}