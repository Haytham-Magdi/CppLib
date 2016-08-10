#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\ImgDataGradMgr_CovMat.h>

#include <Lib\Hcv\ImgDataMgr_2_CovMat.h>



namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;


	IImgDataGradMgr * ImgDataGradMgr_CovMat_Factory::CreateImgDataGradMgr( 
			IImgDataMgr_2Ref a_imgData, ImageNbrMgrRef a_nbrMgr )
	{
		IImgDataGradMgr * pIdm = new ImgDataGradMgr_CovMat(
			a_imgData, a_nbrMgr );

		return pIdm;		
	}


	ImgDataGradMgr_CovMat::ImgDataGradMgr_CovMat( 
			IImgDataMgr_2Ref a_imgData, ImageNbrMgrRef a_nbrMgr )
	{
		m_imgDataMgr = a_imgData;

		m_nbrMgr = a_nbrMgr;
				
		m_nAprSiz = m_nbrMgr->GetAprSiz();

		PrepareData();
	}


	F32ImageRef ImgDataGradMgr_CovMat::GetGradImg_ByNormIdx( int a_nNormIdx )
	{
		if( 0 == a_nNormIdx )
		{
			return m_gradImg_H;
		}
		else if( 1 == a_nNormIdx )
		{
			return m_gradImg_V;
		}
		else
		{
			ThrowHcvException();
			return m_gradImg_H;
		}
	}


	F32ImageRef ImgDataGradMgr_CovMat::GetConfImg_ByNormIdx( int a_nNormIdx )
	{
		if( 0 == a_nNormIdx )
		{
			return m_confImg_H;
		}
		else if( 1 == a_nNormIdx )
		{
			return m_confImg_V;
		}
		else
		{
			ThrowHcvException();
			return m_confImg_H;
		}
	}


	void ImgDataGradMgr_CovMat::PrepareData()
	{
		m_gradImg_H = GenGradImg( m_nbrMgr->GetNbr_H_Arr() );
		
		m_gradImg_V = GenGradImg( m_nbrMgr->GetNbr_V_Arr() );
	}

	F32ImageRef ImgDataGradMgr_CovMat::GenGradImg( 
		FixedVector< ImageNbrMgr::PixIndexNbr > & a_rNbrArr )
	{
		//return NULL;

		{
			CvSize imgSiz = m_imgDataMgr->GetSrcImg()->GetSize();

			int nImgSiz1D = imgSiz.width * imgSiz.height;

			Hcpl_ASSERT( a_rNbrArr.GetSize() == nImgSiz1D );
		}

		//ImgDataMgr_2_CovMat * dataMgr_CM = (ImgDataMgr_2_CovMat *) (
			//(IImgDataMgr_2 *) m_imgDataMgr );

		ImgDataMgr_2_CovMat * dataMgr_CM;
		{
			IImgDataMgr_2 * pDM = (IImgDataMgr_2 *) m_imgDataMgr;

			dataMgr_CM = dynamic_cast< ImgDataMgr_2_CovMat * >( pDM );

			if( NULL == dataMgr_CM )
				ThrowHcvException();
		}


		FixedVector< ImgDataElm_CovMat > & rDataArr = dataMgr_CM->GetDataArr();

		F32ImageRef gradImg = F32Image::Create(
			m_imgDataMgr->GetSrcImg()->GetSize(), 1 );

		float * gradImg_Buf = gradImg->GetPixAt( 0, 0 );


		for( int i=0; i < a_rNbrArr.GetSize(); i++ )
		{
			ImageNbrMgr::PixIndexNbr & rNbr = a_rNbrArr[ i ];
			
			if( -1 == rNbr.nIdxBef || -1 == rNbr.nIdxAft )
			{
				gradImg_Buf[ i ] = S_GetMaxDif();
				continue;
			}

			if( 11832 == i )
				i = i;

			float dif = ImgDataElm_CovMat::CalcDif(
				rDataArr[ rNbr.nIdxBef ], rDataArr[ rNbr.nIdxAft ] );

			//dif = rDataArr[ i ].StanDev;

			gradImg_Buf[ i ] = dif;
		}
				
		//ThrowHcvException();
		return gradImg;
	}


}