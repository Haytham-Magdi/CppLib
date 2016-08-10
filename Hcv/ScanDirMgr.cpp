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

	int ScanDirMgr::s_id = 0;

	void ScanDirMgr::Init( float a_angDig, CvSize a_siz )
	{
		Hcpl_ASSERT( a_angDig >= 0 && a_angDig < 180 );

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

		//hthm tmp
		//return;


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


		

		if( m_mainDirPnt.x > 0 && m_mainDirPnt.y >= 0 &&
			 m_mainDirPnt.x > m_mainDirPnt.y )
		{
			m_dirType = DirType::RC;
		}
		else if( m_mainDirPnt.x > 0 && m_mainDirPnt.y > 0 &&
			 m_mainDirPnt.x <= m_mainDirPnt.y )
		{
			m_dirType = DirType::RB;
		}
		else if( - m_mainDirPnt.x >= 0 && m_mainDirPnt.y > 0 &&
			 - m_mainDirPnt.x < m_mainDirPnt.y )
		{
			m_dirType = DirType::CB;
		}
		else if( - m_mainDirPnt.x > 0 && m_mainDirPnt.y > 0 &&
			 - m_mainDirPnt.x >= m_mainDirPnt.y )
		{
			m_dirType = DirType::LB;
		}
		else
		{
			Hcpl_ASSERT( false );
		}

		PrepareShiftStuff();

		GetPixLineInfoArr();

		Prepare_Grand_IOA_Arr();
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

				//const int nStartShiftIdx = m_shiftStartArr[ i ];
				const int nSSI = m_shiftStartArr[ i ];

				
				//for( int j=0; j < nLimShiftIdx; j++ )
				for( int j = nSSI; j < nLimShiftIdx; j++ )
				{
					const int nCurPntIdx = nBgnPntIdx + m_shiftIndexArr[ j ];

					PixLineInfo & pli = m_pixLineInfoArr[ nCurPntIdx ];

					if( 203 == i &&
						2 == this->m_id )
						j = j;



					if( -1 != pli.nLineNum )
						nCnt1 = nCnt1;

					pli.nLineNum = i;
					pli.nIOL = j;

					nCnt1++;
				}
			}


			nCnt1 = nCnt1;

			Hcpl_ASSERT( nSrcSiz1D == nCnt1 );
		}


		return m_pixLineInfoArr;
	}





	void ScanDirMgr::PrepareShiftStuff()
	{
		switch( m_dirType )
		{
		case DirType::RC:
			PrepareShiftStuff_RC();
			break;

		case DirType::RB:
			PrepareShiftStuff_RB();
			break;

		case DirType::CB:
			PrepareShiftStuff_CB();
			break;

		case DirType::LB:
			PrepareShiftStuff_LB();
			break;

		default:
			Hcpl_ASSERT( false );
		}
	}

	void ScanDirMgr::PrepareShiftStuff_RC()
	{
		IndexCalc2D i2D( m_srcSiz.width, m_srcSiz.height );
		

		//	Prepare start data
		{
			m_bgnIndexArr.SetCapacity( m_srcSiz.width + m_srcSiz.height );

			m_shiftStartArr.SetCapacity( m_srcSiz.width + m_srcSiz.height );

			int nOldBPI = -1;
			int nOldSI = -1;

			bool bFst = true;

			for( int x = m_srcSiz.width - 1, y=0; ; x-- )
			{
				if( x < 0 )
				{
					m_bgnIndexArr.PushBack( nOldBPI );
					m_shiftStartArr.PushBack( nOldSI );

					break;
				}

				const int nSI = x;
				
				const int nCPI = i2D.Calc( x, y );

				const int nBPI = nCPI - m_shiftIndexArr[ nSI ];

				if( bFst )
				{
					bFst = false;
				}
				else
				{
					if( nBPI != nOldBPI )
					{
						m_bgnIndexArr.PushBack( nOldBPI );
						m_shiftStartArr.PushBack( nOldSI );
					}

				}

				nOldBPI = nBPI;
				nOldSI = nSI;
			}

			for( int x=0, y = 1; y < m_srcSiz.height; y++ )
			{
				const int nSI = 0;
				
				const int nCPI = i2D.Calc( x, y );

				const int nBPI = nCPI - m_shiftIndexArr[ nSI ];

				m_bgnIndexArr.PushBack( nBPI );
				m_shiftStartArr.PushBack( nSI );
			}
		}


		//	Prepare end data
		{
			m_limArr.SetCapacity( m_srcSiz.width + m_srcSiz.height );

			//for( int y=0, x = m_srcSiz.width - 1; y < m_srcSiz.height; y++ )
			for( int y=0; y < m_srcSiz.height; y++ )
			{
				m_limArr.PushBack( m_srcSiz.width );
			}

			
			int x = m_srcSiz.width - 2;
			int y = m_srcSiz.height - 1;

			for( int nAI = m_srcSiz.height; nAI < m_bgnIndexArr.GetSize(); nAI++ )
			{
				const int nBPI = m_bgnIndexArr[ nAI ];

				for( ; x >= 0; x-- )
				{
					const int nSI = x;

					const int nSV = m_shiftIndexArr[ nSI ];

					const int nCPI = i2D.Calc( x, y );

					const int nBPI2 = nCPI - nSV;

					if( nBPI == nBPI2 )
					{
						m_limArr.PushBack( nSI + 1 );
						x--;						

						break;
					}
				}

			}

		}

		int a;

		a = 0;


		if( m_bgnIndexArr.GetSize() != m_limArr.GetSize() )
			Hcpl_ASSERT( false );
	}



	void ScanDirMgr::PrepareShiftStuff_RB()
	{
		IndexCalc2D i2D( m_srcSiz.width, m_srcSiz.height );
		

		//	Prepare start data
		{
			m_bgnIndexArr.SetCapacity( m_srcSiz.width + m_srcSiz.height );

			m_shiftStartArr.SetCapacity( m_srcSiz.width + m_srcSiz.height );

			int nOldBPI = -1;
			int nOldSI = -1;

			bool bFst = true;

			for( int x=0, y = m_srcSiz.height - 1; ; y--  )
			{
				if( y < 0 )
				{
					m_bgnIndexArr.PushBack( nOldBPI );
					m_shiftStartArr.PushBack( nOldSI );

					break;
				}

				const int nSI = y;
				
				const int nCPI = i2D.Calc( x, y );

				const int nBPI = nCPI - m_shiftIndexArr[ nSI ];

				
				if( bFst )
				{
					bFst = false;
				}
				else
				{
					if( nBPI != nOldBPI )
					{
						m_bgnIndexArr.PushBack( nOldBPI );
						m_shiftStartArr.PushBack( nOldSI );
					}

				}

				nOldBPI = nBPI;
				nOldSI = nSI;
			}

			for( int y=0, x = 1; x < m_srcSiz.width; x++ )
			{
				const int nSI = 0;
				
				const int nCPI = i2D.Calc( x, y );

				const int nBPI = nCPI - m_shiftIndexArr[ nSI ];

				m_bgnIndexArr.PushBack( nBPI );
				m_shiftStartArr.PushBack( nSI );
			}
		}


		//	Prepare end data
		{
			m_limArr.SetCapacity( m_srcSiz.width + m_srcSiz.height );

			for( int x=0; x < m_srcSiz.width; x++ )
			{
				m_limArr.PushBack( m_srcSiz.height );
			}

			
			int x = m_srcSiz.width - 1;
			int y = m_srcSiz.height - 2;

			for( int nAI = m_srcSiz.width; nAI < m_bgnIndexArr.GetSize(); nAI++ )
			{
				const int nBPI = m_bgnIndexArr[ nAI ];

				for( ; y >= 0; y-- )
				{
					const int nSI = y;

					const int nSV = m_shiftIndexArr[ nSI ];

					const int nCPI = i2D.Calc( x, y );

					const int nBPI2 = nCPI - nSV;

					if( nBPI == nBPI2 )
					{
						m_limArr.PushBack( nSI + 1 );
						y--;						

						break;
					}
				}

			}

		}

		int a;

		a = 0;


		if( m_bgnIndexArr.GetSize() != m_limArr.GetSize() )
			Hcpl_ASSERT( false );
	}

	void ScanDirMgr::PrepareShiftStuff_CB()
	{
		IndexCalc2D i2D( m_srcSiz.width, m_srcSiz.height );
		

		//	Prepare start data
		{
			m_bgnIndexArr.SetCapacity( m_srcSiz.width + m_srcSiz.height );

			m_shiftStartArr.SetCapacity( m_srcSiz.width + m_srcSiz.height );

			int nOldBPI = -1;
			int nOldSI = -1;

			bool bFst = true;

			for( int x = m_srcSiz.width - 1, y = m_srcSiz.height - 1; ; y--  )
			{
				if( y < 0 )
				{
					m_bgnIndexArr.PushBack( nOldBPI );
					m_shiftStartArr.PushBack( nOldSI );

					break;
				}

				const int nSI = y;
				
				const int nCPI = i2D.Calc( x, y );

				const int nBPI = nCPI - m_shiftIndexArr[ nSI ];

				if( bFst )
				{
					bFst = false;
				}
				else
				{
					if( nBPI != nOldBPI )
					{
						m_bgnIndexArr.PushBack( nOldBPI );
						m_shiftStartArr.PushBack( nOldSI );
					}

				}

				nOldBPI = nBPI;
				nOldSI = nSI;
			}

			for( int y = 0, x = m_srcSiz.width - 2; x >= 0; x-- )
			{
				const int nSI = 0;
				
				const int nCPI = i2D.Calc( x, y );

				const int nBPI = nCPI - m_shiftIndexArr[ nSI ];

				m_bgnIndexArr.PushBack( nBPI );
				m_shiftStartArr.PushBack( nSI );
			}
		}


		//	Prepare end data
		{
			m_limArr.SetCapacity( m_srcSiz.width + m_srcSiz.height );

			{
				for( int x = m_srcSiz.width - 1; x >= 0; x-- )
				{
					m_limArr.PushBack( m_srcSiz.height );
				}
			}

			
			int y = m_srcSiz.height - 2;
			//int x = m_srcSiz.width - 1;
			int x = 0;

			y = y;

			for( int nAI = m_srcSiz.width; nAI < m_bgnIndexArr.GetSize(); nAI++ )
			{
				const int nBPI = m_bgnIndexArr[ nAI ];

				for( ; y >= 0; y-- )
				{
					const int nSI = y;

					const int nSV = m_shiftIndexArr[ nSI ];

					const int nCPI = i2D.Calc( x, y );

					const int nBPI2 = nCPI - nSV;

					if( nBPI == nBPI2 )
					{
						m_limArr.PushBack( nSI + 1 );
						y--;						

						break;
					}
				}

			}

		}

		int a;

		a = 0;


		if( m_bgnIndexArr.GetSize() != m_limArr.GetSize() )
			Hcpl_ASSERT( false );
	}

	void ScanDirMgr::PrepareShiftStuff_LB()
	{
		IndexCalc2D i2D( m_srcSiz.width, m_srcSiz.height );
		

		//	Prepare start data
		{
			m_bgnIndexArr.SetCapacity( m_srcSiz.width + m_srcSiz.height );

			m_shiftStartArr.SetCapacity( m_srcSiz.width + m_srcSiz.height );

			int nOldBPI = -1;
			int nOldSI = -1;

			bool bFst = true;

			for( int x=0, y=0; ; x++ )
			{
				if( x >= m_srcSiz.width )
				{
					m_bgnIndexArr.PushBack( nOldBPI );
					m_shiftStartArr.PushBack( nOldSI );

					break;
				}

				//const int nSI = x;
				const int nSI = m_srcSiz.width - 1 - x;
				
				const int nCPI = i2D.Calc( x, y );

				const int nBPI = nCPI - m_shiftIndexArr[ nSI ];

				if( bFst )
				{
					bFst = false;
				}
				else
				{
					if( nBPI != nOldBPI )
					{
						m_bgnIndexArr.PushBack( nOldBPI );
						m_shiftStartArr.PushBack( nOldSI );
					}

				}

				nOldBPI = nBPI;
				nOldSI = nSI;
			}

			for( int x = m_srcSiz.width - 1, y = 1; y < m_srcSiz.height; y++ )
			{
				const int nSI = 0;
				
				const int nCPI = i2D.Calc( x, y );

				const int nBPI = nCPI - m_shiftIndexArr[ nSI ];

				m_bgnIndexArr.PushBack( nBPI );
				m_shiftStartArr.PushBack( nSI );
			}
		}


		//	Prepare end data
		{
			m_limArr.SetCapacity( m_srcSiz.width + m_srcSiz.height );

			{
				//for( int y=0, x = m_srcSiz.width - 1; y < m_srcSiz.height; y++ )
				for( int y=0; y < m_srcSiz.height; y++ )
				{
					m_limArr.PushBack( m_srcSiz.width );
				}
			}

			
			//int x = m_srcSiz.width - 2;
			int x = 1;
			int y = m_srcSiz.height - 1;

			for( int nAI = m_srcSiz.height; nAI < m_bgnIndexArr.GetSize(); nAI++ )
			{
				const int nBPI = m_bgnIndexArr[ nAI ];

				//for( ; x >= 0; x-- )
				for( ; x < m_srcSiz.width; x++ )
				{
					if( 175 == x )
						x = x;

					const int nSI = m_srcSiz.width - 1 - x;

					const int nSV = m_shiftIndexArr[ nSI ];

					const int nCPI = i2D.Calc( x, y );

					const int nBPI2 = nCPI - nSV;

					if( nBPI == nBPI2 )
					{
						m_limArr.PushBack( nSI + 1 );
						x++;						

						break;
					}
					else
					{
						x = x;
					}
				}

			}

		}

		int a;

		a = 0;


		if( m_bgnIndexArr.GetSize() != m_limArr.GetSize() )
			Hcpl_ASSERT( false );
	}


	int ScanDirMgr::CalcAprSizAct( int a_nAprSizOrg )
	{
		FixedVector< int > & rShiftIndexArr = this->GetShiftIndexArr(); 

		const int nMargOrg = a_nAprSizOrg / 2;

		int nMarg = -1;
		{
			F32Point dirPnt = this->GetMainDirPnt();

			float distAct = dirPnt.CalcMag();

			float ratio = rShiftIndexArr.GetSize() / distAct;

			nMarg = (int) ( ( ratio * nMargOrg ) + 0.5555555 );
		}

		int nAprSizA = 2 * nMarg + 1;

		return nAprSizA;
	}



	void ScanDirMgr::Prepare_Grand_IOA_Arr()
	{
		const int nofLines = m_bgnIndexArr.GetSize();

		//Set_ioaArrCollCapacity( m_srcSiz.width * m_srcSiz.height, nofLines );
		m_ioaArrColl.SetCapacity( m_srcSiz.width * m_srcSiz.height, nofLines );

		int nGrandIOA_Cnt = 0;

		for( int i=0; i < nofLines; i++ )
		{
			const int nBPI = m_bgnIndexArr[ i ];

			const int nSSI = m_shiftStartArr[ i ];

			const int nLimI = m_limArr[ i ];


			int nIOA_Size = nLimI - nSSI;

			//Add_ioaArr( nIOA_Size );
			m_ioaArrColl.AddArr( nIOA_Size );

			//int * ioa_Head = & m_grand_IOA_Arr[ m_grand_IOA_Arr.GetSize() ];
			int * ioa_Head = Get_ioaHeadAt( i );
				//& m_grand_IOA_Arr[ m_grand_IOA_Arr.GetSize() ];

			//m_ioaHeadArr[ i ] =
				//& m_grand_IOA_Arr[ m_grand_IOA_Arr.GetSize() ];

			//m_ioaSizeArr[ i ] = nLimI - nSSI;

			//for( int j = nSSI; j < nLimI; j++ )
			for( int j = nSSI, k=0; j < nLimI; j++, k++ )
			{
				const int nCPI = nBPI + m_shiftIndexArr[ j ];

				//m_grand_IOA_Arr.PushBack( nCPI );
				//m_grand_IOA_Arr.PushBack( nCPI );
				
				//m_grand_IOA_Arr[ nGrandIOA_Cnt++ ] ;				
				ioa_Head[ k ] = nCPI;
			}

		}

		//Hcpl_ASSERT( m_grand_IOA_Arr.GetSize() == 
		//Hcpl_ASSERT( nGrandIOA_Cnt == 
			//m_grand_IOA_Arr.GetCapacity() );
	}

}