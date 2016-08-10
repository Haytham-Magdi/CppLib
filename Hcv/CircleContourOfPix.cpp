#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\CircleContourOfPix.h>

namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;

	CircleContourOfPix::CircleContourOfPix( F32ImageRef a_srcImg, int a_radius )		
	{
		Debug::Assert( 1 == a_srcImg->GetNofChannels() );

		m_srcImg = a_srcImg;
		m_radius = a_radius;

		m_goodDirArr.SetSize(6);
		m_dirShowMap.SetSize(4);


		m_pntArr.SetCapacity( a_radius * 15 );
		{
			CircleContPathRef cp1 = new CircleContPath( 0, 180, a_radius );

			do
			{
				F32Point pnt1 = cp1->GetCurrent();

				m_pntArr.PushBack( pnt1 );

			}while( cp1->MoveNext() );
		}

		//m_valArr.SetCapacity( a_radius * 15 );
		m_valArr.SetSize( m_pntArr.GetSize() * 2 );
		m_valArr2.SetSize( m_valArr.GetSize() );
		m_goodArr.SetSize( m_valArr.GetSize() );

		{
			int nAprSiz = ( 3 * m_valArr.GetSize()) / 16;
			m_avgFlt1 = LinearAvgFilter1D::Create( nAprSiz );
		}

		InitDirArr();
	}
	

	void CircleContourOfPix::InitDirArr()
	{
		const int nSize = m_valArr.GetSize();

		m_dirMap1.SetSize( nSize );
		m_dirMap2.SetSize( nSize );

		for( int i=0; i < nSize; i++ )
		{
			m_dirMap1[ i ] = 5;
			m_dirMap2[ i ] = 5;
		}

		//int nSize24 = m_valArr2.GetSize() / 24;
		int nSize_24 = nSize / 32;

		int nSize_4 = nSize / 4;

		const int nSize_8 = nSize_4 / 2;

		CircIndex ci1( nSize );

		for( int i=0, m=0; i < 4; i++, m += nSize_8 )
		{
			//int nBgn = m - 2 * nSize_24;
			//int nEnd = m + 2 * nSize_24;

			int nBgn = m;
			int nEnd = m;

			for( int j = nBgn; j <= nEnd; j++ )
			{
				{
					const int k = ci1.Calc( j );

					if( 5 == m_dirMap1[ k ] )
						m_dirMap1[ k ] = i;

					m_dirMap2[ k ] = i;
				}


				{
					const int p = ci1.Calc( j + nSize / 2 );

					if( 5 == m_dirMap1[ p ] )
						m_dirMap1[ p ] = i;

					m_dirMap2[ p ] = i;
				}
			}

		}


		for( int i=0; i < nSize; i++ )
		{
			if( 5 == m_dirMap1[ i ] )
				i = i;

			if( 5 == m_dirMap2[ i ] )
				i = i;
		}


		for( int i=0; i < 4; i++ )
		{
			const int k = ci1.Calc( i * nSize_8 );

			m_dirShowMap[ i ] = k;
		}

	}


	void CircleContourOfPix::PreparePix( int a_x, int a_y )
	{
		m_x = a_x;
		m_y = a_y;

		for( int i=0; i < m_goodDirArr.GetSize(); i++ )
			m_goodDirArr[ i ] = false;

		m_midVal = *m_srcImg->GetPixAt( m_x, m_y );

		for( int i=0; i < m_pntArr.GetSize(); i++ )
		{
			F32Point pntSrcMid( a_x, a_y );

			F32Point & pntI = m_pntArr[ i ];

			float valI;
			{
				F32Point pntSrcI = pntSrcMid;
				pntSrcI.IncBy( pntI );

				valI = *m_srcImg->GetPixAt( pntSrcI.x, pntSrcI.y );
			}

			F32Point pntJ = pntI;
			pntJ.MultSelfBy( -1 );

			float valJ;
			{
				F32Point pntSrcJ = pntSrcMid;
				pntSrcJ.IncBy( pntJ );

				valJ = *m_srcImg->GetPixAt( pntSrcJ.x, pntSrcJ.y );
			}

			m_valArr[ i ] = valI;
			m_valArr[ i + m_pntArr.GetSize() ] = valJ;

			//if( m_midVal < valI || m_midVal < valJ )
			{
				//valI = 0;
				//valJ = 0;					
			}

/*
			{
				F32Point & pnt1 = pntI;

				F32Point pnt2 = pnt1;
				pnt2.IncBy( orgPnt );

				//float * pPix = img1->GetPixAt( pnt2.x, pnt2.y );
				//*pPix = valI;
			}

			{
				F32Point & pnt1 = pntJ;

				F32Point pnt2 = pnt1;
				pnt2.IncBy( orgPnt );

				//float * pPix = img1->GetPixAt( pnt2.x, pnt2.y );
				//*pPix = valJ;
			}
*/
		}

		for( int i=0; i < m_goodArr.GetSize(); i++ )
			m_goodArr[i] = false;

		PrepareValArr2( );

		m_avgFlt1->ResetInput();
	}



	void CircleContourOfPix::ShowResult()
	{

		{
			for( int i=0; i < m_goodArr.GetSize(); i++ )
				m_goodArr[ i ] = false;


			//const int nSize_8 = m_goodArr.GetSize() / 8;

			//for( int i=0; i < m_goodDirArr.GetSize(); i++ )
			for( int i=0; i < 4; i++ )
			//for( int i=0; i < m_goodDirArr.GetSize(); i += nSize_8 )
			{
				if( m_goodDirArr[ i ] )
					m_goodArr[ m_dirShowMap[ i ] ] = true;
			}
		}


		//CvSize siz1 = cvSize( 400, 400 );
		//CvSize siz1 = cvSize( 40, 40 );
		CvSize siz1 = cvSize( m_radius * 4, m_radius * 4 );

		F32ImageRef img1 = F32Image::Create( siz1, 1 );

		img1->SetAll(0);

		F32Point orgPnt( siz1.width / 2, siz1.height / 2 ); 


		*img1->GetPixAt( orgPnt.x, orgPnt.y ) = 255;
		//*img1->GetPixAt( orgPnt.x, orgPnt.y ) = m_midVal;

		if( m_orderRatio > 0.333 )
			*img1->GetPixAt( orgPnt.x, orgPnt.y ) = 128;

/*		
		FixedVector< F32Point > pntArr(300);
		

		do
		{
			F32Point pnt1 = cp1->GetCurrent();

			pntArr.PushBack( pnt1 );

		}while( cp1->MoveNext() );
*/

		F32Point pntSrcMid( m_x, m_y );


		//FixedVector< float > valArr;
		//valArr.SetSize( pntArr.GetSize() * 2 );

		for( int i=0; i < m_pntArr.GetSize(); i++ )
		{
			int j = i + m_pntArr.GetSize();

			F32Point & pntI = m_pntArr[ i ];

			float valI;
			{
				F32Point pntSrcI = pntSrcMid;
				pntSrcI.IncBy( pntI );

				//valI = *m_srcImg->GetPixAt( pntSrcI.x, pntSrcI.y );
				valI = 255;
			}

			F32Point pntJ = pntI;
			pntJ.MultSelfBy( -1 );

			float valJ;
			{
				F32Point pntSrcJ = pntSrcMid;
				pntSrcJ.IncBy( pntJ );

				//valJ = *m_srcImg->GetPixAt( pntSrcJ.x, pntSrcJ.y );
				valJ = 255;
			}

			//valArr[ i ] = valI;
			//valArr[ i + m_pntArr.GetSize() ] = valJ;

			//if( m_midVal < valI || m_midVal < valJ )
			{
				//valI = 0;
				//valJ = 0;					
			}

			if( ! m_goodArr[i] )
			{
				valI = 0;

				if( m_goodArr[j] )
					valJ = 128;
			}


			if( ! m_goodArr[j] )
			{
				if( m_goodArr[i] )
					valI = 128;

				valJ = 0;
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

		//ShowValArrSignal( m_valArr, "CircSig" );

		ShowValArrSignal( m_valArr2, "CircSig2" );

		F32ImageRef img2 = GenUpSampledImage( img1, 10 );

		ShowImage( img2, "CircleContPath" );
	}





	void CircleContourOfPix::ShowValArrSignal( FixedVector< float > & a_valArr, char * a_sWndName )
	{
		const int nScaleW = 800 / a_valArr.GetSize() + 1;

		Signal1DViewer sv1;
		int i;

		{
			Signal1DBuilder sb1(1000);

			for( int j=0; j < a_valArr.GetSize(); j++ )
			{
				for( int k=0; k < nScaleW; k++ )
					sb1.AddValue( m_midVal );
			}

			sv1.AddSignal( sb1.GetResult(), u8ColorVal( 0, 170, 0 ) );
		}

		//bool bDone;

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

				//bDone = false;

				for( ; i < nLim; i++ )
				{
					for( int k=0; k < nScaleW; k++ )
					{
						sb1.AddValue( a_valArr[i] );
						//bDone = true;
					}
				}

				sv1.AddSignal( sb1.GetResult(), colorArr[ m % 4 ] );

/*
				if( m < 2 )
					sv1.AddSignal( sb1.GetResult(), u8ColorVal( 0, 0, 255 ) );
				else
					sv1.AddSignal( sb1.GetResult(), u8ColorVal( 200, 0, 200 ) );
*/
			}
/*
			{
				Signal1DBuilder sb1( 1000, i * nScaleW );

				int nLim = i + a_valArr.GetSize() / 8;
				if( nLim > a_valArr.GetSize() )
					nLim = a_valArr.GetSize();

				//bDone = false;

				for( ; i < nLim; i++ )
				{
					for( int k=0; k < nScaleW; k++ )
					{
						sb1.AddValue( a_valArr[i] );
						//bDone = true;
					}
				}

				if( m < 2 )
					//sv1.AddSignal( sb1.GetResult(), u8ColorVal( 200, 0, 200 ) );
					sv1.AddSignal( sb1.GetResult(), u8ColorVal( 200, 0, 0 ) );
				else
					sv1.AddSignal( sb1.GetResult(), u8ColorVal( 255, 255, 255 ) );
			}
*/
		}

		ShowImage( sv1.GenDisplayImage(), a_sWndName );


		//sv1

	}

	void CircleContourOfPix::PrepareValArr2( )
	{
		CircIndex ci1( m_valArr.GetSize() );

		//int nAprSiz = ( 3 * m_valArr.GetSize()) / 16;
		//IFilter1DRef avgFlt1 = LinearAvgFilter1D::Create( nAprSiz );

		IFilter1DRef avgFlt1 = m_avgFlt1;

		const int nBackShift = avgFlt1->GetBackShift();

		int nCnt = 0;

		const int nLim = m_valArr.GetSize() + avgFlt1->GetLength() - 1;
		for( int i=0; i < nLim; i++ )
		{
			avgFlt1->InputVal( m_valArr[ ci1.Calc( i ) ] );

			if( ! avgFlt1->HasOutput() )
				continue;


			float outVal = avgFlt1->CalcOutput();						

			m_valArr2[ ci1.Calc( i - nBackShift ) ] = outVal;

			if( outVal >= m_midVal )
				nCnt++;
		}

		m_orderRatio = (float) nCnt / m_valArr.GetSize();

		const int nDist1 = m_valArr.GetSize() / 8;
		const int nDist2 = m_valArr.GetSize() / 4;

		for( int i=0; i < m_goodArr.GetSize(); i++ )
		{
			float val0 = m_valArr2[ i ];

			if( val0 > m_midVal )
				continue;

/*
			if( val0 >= m_valArr2[ ci1.Calc( i - nDist1 ) ] )
			{
				if( val0 >= m_valArr2[ ci1.Calc( i - nDist2 ) ] )
					continue;
			}

			if( val0 >= m_valArr2[ ci1.Calc( i + nDist1 ) ] )
			{
				if( val0 >= m_valArr2[ ci1.Calc( i + nDist2 ) ] )
					continue;
			}

			//m_goodArr[i] = true;
*/

			if( val0 > m_valArr2[ ci1.Calc( i - nDist1 ) ] ||
				val0 > m_valArr2[ ci1.Calc( i + nDist1 ) ] )
			{
				continue;
			}

			if( val0 < m_valArr2[ ci1.Calc( i - nDist1 ) ] && 
				val0 < m_valArr2[ ci1.Calc( i + nDist1 ) ] )
			{
				m_goodArr[i] = true;
				continue;
			}

			if( val0 < m_valArr2[ ci1.Calc( i - nDist2 ) ] && 
				val0 < m_valArr2[ ci1.Calc( i + nDist2 ) ] )
			{
				m_goodArr[i] = true;
			}

		}

		const int nSize_2 = m_goodArr.GetSize() / 2;
		for( int i=0; i < nSize_2 ; i++ )
		{
			int j = i + nSize_2;

			int crIdxI = ci1.Calc( i );
			int crIdxJ = ci1.Calc( j );

			if( m_goodArr[ crIdxI ] && m_goodArr[ crIdxJ ] )
			{
				m_goodDirArr[ m_dirMap1[ crIdxI ] ] = true;
				m_goodDirArr[ m_dirMap2[ crIdxI ] ] = true;
			}

		}


	}

}