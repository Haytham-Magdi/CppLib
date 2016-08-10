#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\CircleContourOfPix5.h>
#include <Lib\Hcv\LineStepper.h>


namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;

	CircleContourOfPix5::CircleContourOfPix5( F32ImageRef a_srcImg, int a_radius )		
	{
		Debug::Assert( 3 == a_srcImg->GetNofChannels() );

		m_srcImg = a_srcImg;

		m_radius = a_radius;


		m_pntArr.SetCapacity( a_radius * 15 );
		m_radianArr.SetCapacity( a_radius * 15 );

		{
			const int nDivs = 4;
			const int nDivSiz = 360 / nDivs;

			for( int i=0, nBgnDig = 0; i < nDivs; i++, nBgnDig += nDivSiz )
			{
				CircleContPathRef cp1 = new CircleContPath( nBgnDig, nBgnDig + nDivSiz, a_radius );				

				do
				{
					F32Point pnt1 = cp1->GetCurrent();
					m_pntArr.PushBack( pnt1 );

					float rad1 = -1;

					rad1 = atan2( pnt1.y, pnt1.x );						

					if( rad1 < 0 )
						rad1 += 2 * M_PI;

					m_radianArr.PushBack( rad1 );

				}while( cp1->MoveNext() );
			}

		}
/*
		const int nFixedSiz = 56;
		//const int nFixedSiz = (const int)m_pntArr.GetSize();
		F32Point fixPntArr[ nFixedSiz ];

		for( int i=0; i < nFixedSiz; i++ )
		{
			fixPntArr[ i ] = m_pntArr[ i ];
		}

		const int nHfFixedSiz = nFixedSiz / 2;
		const int nQtFixedSiz = nFixedSiz / 4;

		for( int i=0; i < nHfFixedSiz; i++ )
		{
			int nActI = i % nFixedSiz;
			int nActJ = ( i + nHfFixedSiz ) % nFixedSiz;

			F32Point pntI = m_pntArr[ nActI ];
			F32Point pntJ = m_pntArr[ nActJ ];

			pntJ.x *= -1;
			pntJ.y *= -1;

			if( ! F32Point::Compare( pntI, pntJ ) )
				i = i;
		}

		//for( int i=nQtFixedSiz; i < nHfFixedSiz + nQtFixedSiz; i++ )
		for( int i=0; i < nHfFixedSiz; i++ )
		{
			int nActI = i % nFixedSiz;
			int nActJ = ( i + nQtFixedSiz ) % nFixedSiz;

			F32Point pntI = m_pntArr[ nActI ];
			F32Point pntJ = m_pntArr[ nActJ ];

			//pntJ.x *= -1;
			//pntJ.y *= -1;

			//if( ! F32Point::Compare( pntI, pntJ ) )
				i = i;
		}
*/
		//m_valArr.SetCapacity( a_radius * 15 );
		m_valArr.SetSize( m_pntArr.GetSize() );


		m_spaceDistArr.SetSize( m_pntArr.GetSize() );
		m_diffArr.SetSize( m_pntArr.GetSize() );

		const int nValHalfSiz = m_pntArr.GetSize() / 2;

		for( int i=0; i < nValHalfSiz; i++ )			
		{
			const int j = i + nValHalfSiz;

			F32Point & pntI = m_pntArr[ i ];

			//F32Point pntJ = pntI;
			//pntJ.MultSelfBy( -1 );

			F32Point pntJ = m_pntArr[ j ];			

			float spcDist = F32Point::Sub( pntI, pntJ ).CalcMag();

			m_spaceDistArr[ i ] = m_spaceDistArr[ j ] = spcDist / m_radius;
		}


	}
	

	void CircleContourOfPix5::PreparePix( int a_x, int a_y )
	{
		m_x = a_x;
		m_y = a_y;

		const int nValHalfSiz = m_valArr.GetSize() / 2;
		Hcpl_ASSERT( m_valArr.GetSize() % 4 == 0 );

		for( int i=0; i < nValHalfSiz; i++ )			
		{
			const int j = i + nValHalfSiz;

			F32Point pntSrcMid( a_x, a_y );

			F32Point & pntI = m_pntArr[ i ];

			F32ColorVal * pColorI;
			{
				F32Point pntSrcI = pntSrcMid;
				pntSrcI.IncBy( pntI );

				pColorI = (F32ColorVal *) m_srcImg->GetPixAt( pntSrcI.x, pntSrcI.y );
			}

			//F32Point pntJ = pntI;
			//pntJ.MultSelfBy( -1 );

			F32Point pntJ = m_pntArr[ j ];			

			F32ColorVal * pColorJ;
			{
				F32Point pntSrcJ = pntSrcMid;
				pntSrcJ.IncBy( pntJ );

				pColorJ = (F32ColorVal *) m_srcImg->GetPixAt( pntSrcJ.x, pntSrcJ.y );
			}

			//float valI;
			//float valJ;

			float & valI = m_diffArr[ i ];
			float & valJ = m_diffArr[ j ];
			
			{
				float colorDist = F32ColorVal::Sub( *pColorI, *pColorJ ).CalcMag();

				valI = valJ = colorDist / m_spaceDistArr[ i ];
				//valI = valJ = colorDist;

			}

			m_valArr[ i ] = valI;
			m_valArr[ j ] = valJ;

		}

		PrepareValArr2( );
	}



	void CircleContourOfPix5::ShowResult()
	{
		CvSize siz1 = cvSize( m_radius * 4, m_radius * 4 );

		F32ImageRef img1 = F32Image::Create( siz1, 1 );

		img1->SetAll(0);

		F32Point orgPnt( siz1.width / 2, siz1.height / 2 ); 


		*img1->GetPixAt( orgPnt.x, orgPnt.y ) = 255;

		F32Point pntSrcMid( m_x, m_y );


		const int nValHalfSiz = m_valArr.GetSize() / 2;

		for( int i=0; i < nValHalfSiz; i++ )
		{
			int j = i + nValHalfSiz;

			F32Point & pntI = m_pntArr[ i ];

			float valI;
			{
				F32Point pntSrcI = pntSrcMid;
				pntSrcI.IncBy( pntI );

				valI = m_diffArr[ i ];				
			}

			F32Point pntJ = pntI;
			pntJ.MultSelfBy( -1 );

			float valJ;
			{
				F32Point pntSrcJ = pntSrcMid;
				pntSrcJ.IncBy( pntJ );

				valJ = m_diffArr[ j ];				
			}

			{
				F32Point & pnt1 = pntI;

				F32Point pnt2 = pnt1;
				pnt2.IncBy( orgPnt );

				float * pPix = img1->GetPixAt( pnt2.x, pnt2.y );

				*pPix = valI;
			}

			{
				F32Point & pnt1 = pntJ;

				F32Point pnt2 = pnt1;
				pnt2.IncBy( orgPnt );

				float * pPix = img1->GetPixAt( pnt2.x, pnt2.y );

				*pPix = valJ;
			}

		}

		ShowValArrSignal( m_valArr, "CircSig" );

		F32ImageRef img2 = GenUpSampledImage( img1, 10 );

		ShowImage( img2, "CircleContPath" );

		ShowDiffDir();
	}





	void CircleContourOfPix5::ShowValArrSignal( FixedVector< float > & a_valArr, char * a_sWndName )
	{
		const int nScaleW = 800 / a_valArr.GetSize() + 1;

		Signal1DViewer sv1;
		int i;


		U8ColorVal colorArr[ 4 ];
		{
			int nCnt = 0;

			colorArr[ nCnt++ ] = u8ColorVal( 0, 0, 255 );
			colorArr[ nCnt++ ] = u8ColorVal( 200, 0, 0 );
			colorArr[ nCnt++ ] = u8ColorVal( 200, 0, 200 );
			colorArr[ nCnt++ ] = u8ColorVal( 255, 255, 255 );
		}



		for( int i=0,m=0; m < 8; m++ )
		{				

			{
				Signal1DBuilder sb1( 1000, i * nScaleW );

				int nLim = i + a_valArr.GetSize() / 8;
				if( nLim > a_valArr.GetSize() )
					nLim = a_valArr.GetSize();


				for( ; i < nLim; i++ )
				{
					for( int k=0; k < nScaleW; k++ )
					{
						sb1.AddValue( a_valArr[i] );
					}
				}

				sv1.AddSignal( sb1.GetResult(), colorArr[ m % 4 ] );
			}
		}

		ShowImage( sv1.GenDisplayImage(), a_sWndName );
	}

	void CircleContourOfPix5::PrepareValArr2( )
	{
		CircIndex ci1( m_valArr.GetSize() );

		int nCnt = 0;

		const int valSiz = m_valArr.GetSize();
		const int valHahfSiz = valSiz / 2;
		const int valQtrSiz = valSiz / 4;

		int nMaxIdx = 0;

		// Cross elemination
		{

			for( int i=0; i < valQtrSiz; i++ )
			{
				int k = i + valQtrSiz;

				float valI = m_valArr[ i ];
				float valK = m_valArr[ k ];

				float valMin = ( valI < valK ) ? valI : valK;

				int nMaxIK = ( valI > valK ) ? i : k;

				m_valArr[ i ] = valI - valMin;
				m_valArr[ k ] = valK - valMin;

				if( m_valArr[ nMaxIK ] > m_valArr[ nMaxIdx ] )
					nMaxIdx = nMaxIK;
			}


			for( int i = valHahfSiz; i < valSiz; i++ )
			{
				m_valArr[ i ] = m_valArr[ i - valHahfSiz ];
			}
		}

				

		// Circ center of mass
		{
			int nBgn = nMaxIdx + valQtrSiz;
			int nLim = nBgn + valHahfSiz;

			float valSum = 0;
			float valByIdxSum = 0;
			float radByValSum = 0;

			float oldRad1 = -1;
			float oldRad2 = -1;

			for( int i = nBgn; i < nLim; i++ )
			{
				const int nActI = i % valSiz;

				float val = m_valArr[ nActI ];
				
				float rad = m_radianArr[ nActI ];


				//if( nActI < i )
					//rad += 2 * M_PI;

				if( rad < oldRad1 )
					rad += 2 * M_PI;
					//rad = rad;

				oldRad2 = oldRad1;
				oldRad1 = rad;

				valSum += val;
				valByIdxSum += i * val;
				radByValSum  += rad * val;
			}

			if( valSum > 0.01 )
			{
				m_nCenterIdx = (int) ( ( valByIdxSum / valSum ) + 0.55555 );
				m_centerRad = radByValSum / valSum;
			}
			else
			{
				m_nCenterIdx = 0;
				m_centerRad = 0;
			}

			m_nCenterIdx = m_nCenterIdx % valSiz;
			//int nCentOpIdx = ( m_nCenterIdx + valHahfSiz ) % valSiz;


			//for( int i=0; i < valSiz; i++ )
				//m_valArr[ i ] = 0;

			//m_valArr[ m_nCenterIdx ] = m_valArr[ nCentOpIdx ] = 130;
		}


		{
			F32Point srcPnt( m_x, m_y );

			F32Point difPnt;
			{
				difPnt.x = cos( m_centerRad );
				difPnt.y = sin( m_centerRad );

				//float difMag = sqrt( Sqr( difX ), Sqr( difY ) );

				float maxDifXY = ( fabs( difPnt.x ) > fabs( difPnt.y ) ) ? difPnt.x : difPnt.y;

				difPnt.DividSelfBy( maxDifXY );

				//difPnt.MultSelfBy( m_radius );				
			}
			//difPnt = m_pntArr[ m_nCenterIdx ];

			m_fwdPnt = F32Point::Add( srcPnt,  difPnt);
			
			//m_bkdPnt = F32Point::Add( srcPnt, difPnt.MultBy(-1) );
			m_bkdPnt = F32Point::Sub( srcPnt, difPnt );
		}


	}


	void CircleContourOfPix5::ShowDiffDir()
	{

		LineStepper ls1;
		{
			ls1.SetPoints( m_fwdPnt, m_bkdPnt );

			//for( int i=0; i < 20; i++ )
			for( int i=0; i < 35; i++ )
			{
				ls1.FwdMoveNext();
				ls1.BkdMoveNext();
			}

			F32Point newFwdPnt = ls1.GetCurrentFwd();
			F32Point newBkdPnt = ls1.GetCurrentBkd();

			F32ImageRef dspImg = m_srcImg->Clone();

			U8ColorVal color1( 0, 180, 0 );

			DrawCircle( dspImg, newFwdPnt, color1, 3 );
			DrawCircle( dspImg, newBkdPnt, color1, 3 );

			GlobalStuff::ShowLinePathImg( dspImg );
		}

	}

}