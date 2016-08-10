#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\ScanDirMgr.h>

namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;

	void ScanDirMgr::Init( float a_angDig, CvSize a_siz )
	{
		int nMaxSizXY = ( a_siz.width > a_siz.height ) ? a_siz.width : a_siz.height;

		m_srcSiz = a_siz;

		//m_prevMapImg = S32Image::Create( m_srcSiz, 1 );
		//m_nextMapImg = S32Image::Create( m_srcSiz, 1 );   

		m_shiftArr.SetSize( nMaxSizXY );
		m_shiftIndexArr.SetSize( nMaxSizXY );

		m_angDig = a_angDig;
		m_angRad = a_angDig * M_PI / 180;

		{
			m_difPnt.x = cos( m_angRad );
			m_difPnt.y = sin( m_angRad );

			float maxDifXY = ( fabs( m_difPnt.x ) > fabs( m_difPnt.y ) ) ? m_difPnt.x : m_difPnt.y;

			m_difPnt.DividSelfBy( maxDifXY );
		}

		LineStepperByAngle ls1( 0, 0, a_angDig );

		for( int i=0; i < m_shiftArr.GetSize(); i++ )
		{
			m_shiftArr[ i ] = ls1.GetCurrentFwd();

			ls1.FwdMoveNext();
		}

		{
			F32Point & bgnPnt = m_shiftArr[ 0 ];
			F32Point & endPnt = m_shiftArr.GetBack();

			m_mainDirPnt = F32Point::Sub( endPnt, bgnPnt );
		}

		{
			IndexCalc2D index2D( m_srcSiz.width, m_srcSiz.height );

			for( int i=0; i < m_shiftArr.GetSize(); i++ )
			{
				if( 701 == i )
					i = i;

				F32Point & rPnt1 = m_shiftArr[ i ];

				int nIndex = index2D.Calc( rPnt1.x, rPnt1.y );


				m_shiftIndexArr[ i ] = nIndex;				
			}
		}

		PrepareBgnPntArr();

		PrepareMaps();



	}

	void ScanDirMgr::PrepareMaps()
	{
		F32Point limPnt_0;

		if( m_mainDirPnt.x > 0 && 0 == m_mainDirPnt.y )
		{
			limPnt_0.x = m_srcSiz.width;
			limPnt_0.y = m_srcSiz.height;
		}
		else if( 0 == m_mainDirPnt.x && m_mainDirPnt.y > 0  )
		{
			limPnt_0.x = m_srcSiz.width;
			limPnt_0.y = m_srcSiz.height;
		}
		else if( m_mainDirPnt.x > 0 && m_mainDirPnt.y > 0  )
		{
			limPnt_0.x = m_srcSiz.width;
			limPnt_0.y = m_srcSiz.height;
		}
		else if( m_mainDirPnt.x < 0 && m_mainDirPnt.y > 0  )
		{
			limPnt_0.x = -1;
			limPnt_0.y = m_srcSiz.height;
		}
		else
		{
			Hcpl_ASSERT( false );
		}

		const F32Point limPnt = limPnt_0;


		//Hcpl::Int32 * nextIndexBuf = m_nextMapImg->GetPixAt( 0, 0 );

		IndexCalc2D index2D( m_srcSiz.width, m_srcSiz.height );

		F32Point testPnt( 124, 0 );

		for( int i=0; i < m_bgnPntArr.GetSize(); i++ )
		{
			m_limArr[ i ] = m_shiftArr.GetSize();

			F32Point & rBgnPnt = m_bgnPntArr[ i ];

			//if( F32Point::Compare( testPnt, rBgnPnt ) )
				//i = i;

			//F32Point curPnt = rBgnPnt;

			//*m_prevMapImg->GetPixAt( curPnt.x, curPnt.y ) = -1;



			//F32Point nextPnt;

			//F32Point prevPnt = F32Point( -1, -1 );

			for( int j=1; j < m_shiftArr.GetSize(); j++ )
			{
				F32Point & rShifPnt = m_shiftArr[ j ];

				F32Point nextPnt = F32Point::Add(
					rBgnPnt, rShifPnt );

				if( limPnt.x == nextPnt.x || limPnt.y == nextPnt.y )
				{
					m_limArr[ i ] = j;

					break;
				}

				//const int nextIdx = index2D.Calc( nextPnt.x, nextPnt.y );

				//*m_nextMapImg->GetPixAt( curPnt.x, curPnt.y ) = nextIdx;

				//prevPnt = curPnt;

				//curPnt = nextPnt;
			}

			//*m_nextMapImg->GetPixAt( curPnt.x, curPnt.y ) = -1;

		}

		GetPixLineInfoArr();
	}

	void ScanDirMgr::PrepareBgnPntArr()
	{

		if( m_mainDirPnt.x > 0 && 0 == m_mainDirPnt.y )
		{
			PrepareBgnPntArr_1();
		}
		else if( 0 == m_mainDirPnt.x && m_mainDirPnt.y > 0  )
		{
			PrepareBgnPntArr_2();
		}
		else if( m_mainDirPnt.x > 0 && m_mainDirPnt.y > 0  )
		{
			PrepareBgnPntArr_3();
		}
		else if( m_mainDirPnt.x < 0 && m_mainDirPnt.y > 0  )
		{
			PrepareBgnPntArr_4();
		}
		else
		{
			Hcpl_ASSERT( false );
		}

		IndexCalc2D index2D( m_srcSiz.width, m_srcSiz.height );

		m_bgnIndexArr.SetSize( m_bgnPntArr.GetSize() );
		m_limArr.SetSize( m_bgnPntArr.GetSize() );

		for( int i=0; i < m_bgnPntArr.GetSize(); i++ )
		{
			F32Point & rPnt1 = m_bgnPntArr[ i ];

			m_bgnIndexArr[ i ] = index2D.Calc( rPnt1.x, rPnt1.y );
		}

	}


	void ScanDirMgr::PrepareBgnPntArr_1()
	{
		m_bgnPntArr.SetSize( m_srcSiz.height );

		for( int i=0; i < m_bgnPntArr.GetSize(); i++ )
		{
			F32Point & rPnt = m_bgnPntArr[ i ];

			rPnt.x = 0;
			rPnt.y = i;
		}

	}

	void ScanDirMgr::PrepareBgnPntArr_2()
	{
		m_bgnPntArr.SetSize( m_srcSiz.width );

		for( int i=0; i < m_bgnPntArr.GetSize(); i++ )
		{
			F32Point & rPnt = m_bgnPntArr[ i ];

			rPnt.x = i;
			rPnt.y = 0;
		}

	}

	void ScanDirMgr::PrepareBgnPntArr_3()
	{
		m_bgnPntArr.SetSize( m_srcSiz.width + m_srcSiz.height - 1 );

		int i = 0;
		for( int h = m_srcSiz.height - 1; h >= 0; h-- )
		{
			//m_bgnPntArr.IncSize();
			//F32Point & rPnt = m_bgnPntArr.GetBack();

			F32Point & rPnt = m_bgnPntArr[ i ];
			i++;

			rPnt.x = 0;
			rPnt.y = h;
		}

		for( int w = 1; w < m_srcSiz.width; w++ )
		{
			//m_bgnPntArr.IncSize();
			//F32Point & rPnt = m_bgnPntArr.GetBack();

			F32Point & rPnt = m_bgnPntArr[ i ];
			i++;

			rPnt.x = w;
			rPnt.y = 0;
		}

	}

	void ScanDirMgr::PrepareBgnPntArr_4()
	{
		m_bgnPntArr.SetSize( m_srcSiz.width + m_srcSiz.height - 1 );

		int i = 0;
		for( int h = m_srcSiz.height - 1; h >= 0; h-- )
		{
			//m_bgnPntArr.IncSize();
			//F32Point & rPnt = m_bgnPntArr.GetBack();

			F32Point & rPnt = m_bgnPntArr[ i ];
			i++;

			rPnt.x = m_srcSiz.width - 1;
			rPnt.y = h;
		}

		for( int w = m_srcSiz.width - 2; w >= 0; w-- )
		{
			//m_bgnPntArr.IncSize();
			//F32Point & rPnt = m_bgnPntArr.GetBack();

			F32Point & rPnt = m_bgnPntArr[ i ];
			i++;

			rPnt.x = w;
			rPnt.y = 0;
		}

	}


	FixedVector< ScanDirMgr::PixLineInfo > & ScanDirMgr::GetPixLineInfoArr()
	{	

		if( 0 == m_pixLineInfoArr.GetSize() )
		{
			int nCnt1 = 0;

			const int nSrcSiz1D = m_srcSiz.width * m_srcSiz.height;

			m_pixLineInfoArr.SetSize( nSrcSiz1D );

			for( int i=0; i < m_bgnIndexArr.GetSize(); i++ )
			{
				const int nBgnPntIdx = m_bgnIndexArr[ i ];
				const int nLimShiftIdx = m_limArr[ i ];

				for( int j=0; j < nLimShiftIdx; j++ )
				{
					const int nCurPntIdx = nBgnPntIdx + m_shiftIndexArr[ j ];

					PixLineInfo & pli = m_pixLineInfoArr[ nCurPntIdx ];

					if( -1 != pli.nLineNum )
						nCnt1 = nCnt1;

					pli.nLineNum = i;
					pli.nLOL = j;

					nCnt1++;
				}
			}


			nCnt1 = nCnt1;

		}


		return m_pixLineInfoArr;
	}

}