#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\CircDiff2.h>

namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;

	CircDiff2::CircDiff2( F32ImageRef a_srcImg, int a_radius ) :
	m_fwdBwdPntArr( NULL, a_srcImg->GetSize().width, a_srcImg->GetSize().height )
	{
		m_srcImg = a_srcImg;
		m_radius = a_radius;

		m_circCont = new CircleContourOfPix5( a_srcImg, a_radius );

		Proceed();
	}



	F32ImageRef CircDiff2::GenResult( F32ImageRef a_src, int a_nAprSiz )
	{
		Hcpl_ASSERT( 3 == a_src->GetNofChannels() );
		Hcpl_ASSERT( 1 == a_nAprSiz % 2 );

		FixedVector< int > shifArrX;
		FixedVector< int > shifArrY;
		{
			const int nRadius = a_nAprSiz / 2;

			CircleContPathRef cp1 = new CircleContPath( 0, 180, nRadius );

			shifArrX.SetCapacity( nRadius * 15 );
			shifArrY.SetCapacity( nRadius * 15 );

			do
			{
				F32Point pnt1 = cp1->GetCurrent();

				shifArrX.PushBack( (int)pnt1.x );
				shifArrY.PushBack( (int)pnt1.y );

			}while( cp1->MoveNext() );
		}



		F32ImageRef ret;

		const int nMarg = a_nAprSiz / 2;

		CvSize srcSiz = a_src->GetSize();
		//CvSize retSiz = cvSize( srcSiz.width - 2 * nMarg, srcSiz.height);
		CvSize retSiz = cvSize( srcSiz.width, srcSiz.height);

		ret = F32Image::Create( retSiz, 1 );

		{
			const float maxDif = sqrt( 3.0 * Sqr( 255 ) );

			FillBorder( ret->GetAt(0), (float)maxDif, 
				nMarg, nMarg, nMarg, nMarg);
		}

		const int nBgnX = nMarg;
		const int nLimX = retSiz.width - nMarg;

		const int nBgnY = nMarg;
		const int nLimY = retSiz.height - nMarg;

		const int nShiftSize = shifArrX.GetSize();


		for(int y = nBgnY; y < nLimY; y++)
		{
			for(int x = nBgnX; x < nLimX; x++)
			{
				float maxSqrDif = 0;

				for( int i=0; i < nShiftSize; i++ )
				{
					const int shiftX = shifArrX[ i ];
					const int shiftY = shifArrY[ i ];

					F32ColorVal * pC1 = (F32ColorVal *) a_src->GetPixAt( 
						x + shiftX, y + shiftY );

					F32ColorVal * pC2 = (F32ColorVal *) a_src->GetPixAt( 
						x - shiftX, y - shiftY );

					const float sqrDif = 
						Sqr( pC1->val0 - pC2->val0 ) +
						Sqr( pC1->val1 - pC2->val1 ) +
						Sqr( pC1->val2 - pC2->val2 );

					if( sqrDif > maxSqrDif )
						maxSqrDif = sqrDif;
				}

				float val = sqrt( maxSqrDif );
				*ret->GetPixAt( x, y ) = val;
			}
		}

		return ret;
	}


	void CircDiff2::Proceed()
	{
		const int nMarg = m_radius;

		CvSize srcSiz = m_srcImg->GetSize();
		CvSize retSiz = cvSize( srcSiz.width, srcSiz.height);

		m_difMagImg = F32Image::Create( retSiz, 1 );
		m_difAngleImg = F32Image::Create( retSiz, 1 );

		{
			const float maxDif = sqrt( 3.0 * Sqr( 255 ) );

			FillBorder( m_difMagImg->GetAt(0), (float)maxDif, 
				nMarg, nMarg, nMarg, nMarg);

			FillBorder( m_difAngleImg->GetAt(0), (float)0, 
				nMarg, nMarg, nMarg, nMarg);
		}






		const int nBgnX = nMarg;
		const int nLimX = retSiz.width - nMarg;

		const int nBgnY = nMarg;
		const int nLimY = retSiz.height - nMarg;

		//IndexCalc2D indexCalc( retSiz.width, retSiz.height );

		for(int y = nBgnY; y < nLimY; y++)
		{
			for(int x = nBgnX; x < nLimX; x++)
			{
				//const int i = indexCalc.Calc( x, y );

				m_circCont->PreparePix( x, y );

				{
					float magVal = m_circCont->GetDiffMag();
					*m_difMagImg->GetPixAt( x, y ) = magVal;
				}

				{
					int angDig = m_circCont->GetAngleByDigree();
					//angDig = ( angDig * 255 ) / 360;

					*m_difAngleImg->GetPixAt( x, y ) = angDig;
				}

				{
					FwdBkdPoints & fbPnts = m_fwdBwdPntArr.GetAt( x, y );

					fbPnts.FwdPnt = m_circCont->GetFwdPnt();
					fbPnts.BkdPnt = m_circCont->GetBkdPnt();
				}

			}
		}


	}





	F32ImageRef CircDiff2::GenColorShowImg()
	{
		CvSize srcSiz = m_srcImg->GetSize();

		//F32ImageRef hsvImg = F32Image::Create( srcSiz, 3 );
		U8ImageRef hsvImg = U8Image::Create( srcSiz, 3 );

		const float maxDif = sqrt( 3.0 * Sqr( 255 ) );

		//const float magRatio = 255 / maxDif;
		const float magRatio = 155 / maxDif;

		for(int y = 0; y < srcSiz.height; y++)
		{
			for(int x = 0; x < srcSiz.width; x++)
			{
				float magVal = *m_difMagImg->GetPixAt( x, y );
				//magVal = 50 + magVal * magRatio;

				//magVal /= 255;

				if( magVal > 200 )
					magVal = 200;

				//float angDig = *m_difAngleImg->GetPixAt( x, y ) / 2;				
				float angDig = *m_difAngleImg->GetPixAt( x, y );

				if( angDig > 180 )
					angDig = angDig;

				//F32ColorVal * pColor = (F32ColorVal *)hsvImg->GetPixAt( x, y );
				U8ColorVal * pColor = (U8ColorVal *)hsvImg->GetPixAt( x, y );

				//pColor->AssignVal( angDig, 200, magVal );
				//pColor->AssignVal( angDig, magVal, magVal );				
				//pColor->AssignVal( angDig, 0.6, 0.6 );				
				//pColor->AssignVal( angDig, 0.0, 0.0 );				

				//pColor->AssignVal( 70, 
				pColor->AssignVal( angDig, 
					//GlobalStuff::AprSize1D, magVal );				
					magVal, magVal );				

				//pColor->AssignVal( GlobalStuff::AprSize1D, 
					//200, 200 );				

				//pColor->AssignVal( GlobalStuff::AprSize1D, 
					//GlobalStuff::AprSize1D,
					//GlobalStuff::AprSize1D );				
			}
		}


		//F32ImageRef bgrImg = F32Image::Create( srcSiz, 3 );
		U8ImageRef bgrImg = U8Image::Create( srcSiz, 3 );

		

		HCV_CALL(
			cvCvtColor( hsvImg->GetIplImagePtr(), 
			bgrImg->GetIplImagePtr(), CV_HSV2BGR )
			);


		return GenF32FromU8Image( bgrImg );
	}





}