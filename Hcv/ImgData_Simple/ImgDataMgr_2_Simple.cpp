#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\ImgData_Simple\ImgDataMgr_2_Simple.h>

namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;


	IImgDataMgr_2 * ImgDataMgr_2_Simple_Factory::CreateImgDataMgr( 
			F32ImageRef a_srcImg, int a_nAprSiz,
			IImgCoreSrcRef a_dataSrc, ImgRotationMgrRef a_rot )
	{
		IImgDataMgr_2 * pIdm = new ImgDataMgr_2_Simple(
			a_srcImg, a_nAprSiz, a_dataSrc, a_rot );

		return pIdm;		
	}







/////----------------------------------------------------------------



	ImgDataMgr_2_Simple::ImgDataMgr_2_Simple( 
		F32ImageRef a_srcImg, int a_nAprSiz, IImgCoreSrcRef a_dataSrc,
		ImgRotationMgrRef a_rot )
	{
		m_srcImg = a_srcImg;
		m_nAprSiz = a_nAprSiz;

		m_rot = a_rot;

		PrepareData();
	}

	
	void ImgDataMgr_2_Simple::PrepareData()
	{
		//ThrowHcvException();

		m_meanImg = GenFastAvgImg( m_srcImg, m_nAprSiz );

		{
			F32ColorVal * src_Buf = (F32ColorVal *)m_srcImg->GetPixAt( 0, 0 );

			m_magSqrImg = F32Image::Create( 
				m_srcImg->GetSize(), 1 );

			float * magSqr_Buf = m_magSqrImg->GetPixAt( 0, 0 );

			int nImgSiz1D = m_srcImg->GetSize1D();

			for( int i=0; i < nImgSiz1D; i++ )
			{
				F32ColorVal & rSrcColor = src_Buf[ i ];
								
				magSqr_Buf[ i ] = rSrcColor.CalcMagSqr();
			}

			m_magSqrImg = GenFastAvgImg( m_magSqrImg, m_nAprSiz );
		}

		{
			F32ColorVal * mean_Buf = (F32ColorVal *)m_meanImg->GetPixAt( 0, 0 );

			m_stanDevImg = F32Image::Create( 
				m_srcImg->GetSize(), 1 );

			float * magSqr_Buf = m_magSqrImg->GetPixAt( 0, 0 );

			float * stanDev_Buf = m_stanDevImg->GetPixAt( 0, 0 );

			int nImgSiz1D = m_srcImg->GetSize1D();

			for( int i=0; i < nImgSiz1D; i++ )
			{
				F32ColorVal & rMeanColor = mean_Buf[ i ];

				float val = magSqr_Buf[ i ] - 
					rMeanColor.CalcMagSqr();

				if( val < 0 )
					val = 0;

				val = sqrt( (float) val );

				stanDev_Buf[ i ] = val;
			}
		}


	}







}