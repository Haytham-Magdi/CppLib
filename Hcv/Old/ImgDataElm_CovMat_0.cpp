#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\ImgDataElm_CovMat.h>

namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;


	int ImgDataElm_CovMat::NofColors = IMGDATAELM_CovMat_NOFCOLORS; 
	
	ImgDataElm_CovMat_ConverterRef ImgDataElm_CovMat::m_converter;



	ImgDataElm_CovMat_Converter::ImgDataElm_CovMat_Converter()
	{
		Init();
	}

	void ImgDataElm_CovMat_Converter::Init()
	{
		F32ColorVal c1;

		m_mainColorArr.SetCapacity( 500 );
		m_dispColorArr.SetCapacity( 500 );

		if( m_mainColorArr.GetSize() < ImgDataElm_CovMat::GetNofColors() )
		{
			c1.AssignVal( 255, 0, 0 );
			m_mainColorArr.PushBack( c1 );
			m_dispColorArr.PushBack( c1 );
		}

		if( m_mainColorArr.GetSize() < ImgDataElm_CovMat::GetNofColors() )
		{
			c1.AssignVal( 0, 255, 0 );
			m_mainColorArr.PushBack( c1 );
			m_dispColorArr.PushBack( c1 );
		}

		if( m_mainColorArr.GetSize() < ImgDataElm_CovMat::GetNofColors() )
		{
			c1.AssignVal( 0, 0, 255 );
			m_mainColorArr.PushBack( c1 );
			m_dispColorArr.PushBack( c1 );
		}

		if( m_mainColorArr.GetSize() < ImgDataElm_CovMat::GetNofColors() )
		{
			c1.AssignVal( 255, 255, 0 );
			m_mainColorArr.PushBack( c1 );
			m_dispColorArr.PushBack( c1 );
		}

		if( m_mainColorArr.GetSize() < ImgDataElm_CovMat::GetNofColors() )
		{
			c1.AssignVal( 255, 0, 255 );
			m_mainColorArr.PushBack( c1 );
			m_dispColorArr.PushBack( c1 );
		}

		if( m_mainColorArr.GetSize() < ImgDataElm_CovMat::GetNofColors() )
		{
			c1.AssignVal( 0, 255, 255 );
			m_mainColorArr.PushBack( c1 );
			m_dispColorArr.PushBack( c1 );
		}

		if( m_mainColorArr.GetSize() < ImgDataElm_CovMat::GetNofColors() )
		{
			c1.AssignVal( 255, 255, 255 );
			m_mainColorArr.PushBack( c1 );
			m_dispColorArr.PushBack( c1 );
		}

		while( m_mainColorArr.GetSize() < ImgDataElm_CovMat::GetNofColors() )
		{
			c1.AssignVal( 255, 0, 0 );
			m_mainColorArr.PushBack( c1 );
			m_dispColorArr.PushBack( c1 );
		}



		m_distArr.SetSize( m_mainColorArr.GetSize() );

		for( int i=0; i < m_mainColorArr.GetSize(); i++ )
		{
			F32ColorVal & rColor = m_mainColorArr[ i ];

			//rColor.DividSelfBy( rColor.CalcMag() / 100 );

			rColor = rColor.GetUnitVect();
		}

	}

	void ImgDataElm_CovMat_Converter::Convert( F32ColorVal & a_rSrc, ImgDataElm_CovMat * a_pDst )
	{
/*		a_pDst->SetZeroVal();

		int nLim = ( ImgDataElm_CovMat::GetNofColors() < m_mainColorArr.GetSize() ) ?
			ImgDataElm_CovMat::GetNofColors() : m_mainColorArr.GetSize();

		MaxFinder< float > maxFnd;

		F32ColorVal modSrc = a_rSrc;

		modSrc = modSrc.IncLessToNum( 40 );

		//modSrc.DividSelfBy( modSrc.CalcMag() / 100 );

		modSrc = modSrc.GetUnitVect();

		for( int i=0; i < nLim; i++ )
		{
			F32ColorVal & rColor = m_mainColorArr[ i ];

			float dist = F32ColorVal::Sub( rColor, modSrc ).CalcMag();

			if( dist > 0.91940165 )
				dist = 0.91940165;

			maxFnd.AddValue( dist );			

			m_distArr[ i ] = dist;
		}

		float * dstColorBuf = a_pDst->ColorBuff;
		
		float srcMag = a_rSrc.CalcMag();
		//float srcMag = a_rSrc.IncLessToNum(40).CalcMag();

		for( int i=0; i < nLim; i++ )
		{
			dstColorBuf[ i ] = srcMag * 
				( maxFnd.FindMax() - m_distArr[ i ] ) / maxFnd.FindMax();
		}

*/

	}



			


	float ImgDataElm_CovMat::CalcMag()
	{
		static ImgDataElm_CovMat zeroElm;
		static bool bZeroReady = false;

		if( ! bZeroReady )
		{
			zeroElm.SetZeroVal();
			bZeroReady = true;
		}

		float val = ImgDataElm_CovMat::CalcDif( *this, zeroElm );

		return val;
	}



	//float ImgDataElm_CovMat::Get_FarUnitDif()


}