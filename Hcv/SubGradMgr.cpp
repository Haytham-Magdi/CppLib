#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\SubGradMgr.h>

namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;


	//void SubGradMgr::Process( ScanDirMgrRef a_scanMgr, F32ImageRef a_srcImg, 		 
	void SubGradMgr::Process( ScanDirMgr & a_rScanMgr, F32ImageRef a_srcImg, 
		int a_nAprSiz, float * a_retHead, int a_nStepCnt )
	{
		Hcpl_ASSERT( 1 == a_nAprSiz % 2 );

		CvSize srcSiz = a_srcImg->GetSize();

		{
			CvSize mgrSiz = a_rScanMgr.GetCvSize();

			Hcpl_ASSERT( mgrSiz.width == srcSiz.width &&
				mgrSiz.height == srcSiz.height );
		}



		F32ColorVal * srcColorArr = (F32ColorVal *) a_srcImg->GetPixAt( 0, 0 );

		FixedVector< int > & rBgnIndexArr = a_rScanMgr.GetBgnIndexArr();

		//FixedVector< F32Point > & rBgnPntArr = a_rScanMgr.GetBgnPntArr();

		FixedVector< int > & rLimArr = a_rScanMgr.GetLimArr(); 

		FixedVector< int > & rShiftStartArr = a_rScanMgr.GetShiftStartArr(); 

		

		FixedVector< int > & rShiftIndexArr = a_rScanMgr.GetShiftIndexArr(); 
		FixedVector< F32Point > & rShiftArr = a_rScanMgr.GetShiftArr(); 



		const int nMargOrg = a_nAprSiz / 2;


		int nMarg = -1;
		{
			F32Point dirPnt = a_rScanMgr.GetMainDirPnt();

			float distAct = dirPnt.CalcMag();

			float ratio = rShiftIndexArr.GetSize() / distAct;

			nMarg = (int) ( ( ratio * nMargOrg ) + 0.5555555 );
		}

		const int nAprSizA = 2 * nMarg + 1;




		//Hcpl::Int32 * nextIdxArr = (Hcpl::Int32 *)a_rScanMgr.GetNextMapImg()->GetPixAt( 0, 0 );
		//int * nextIdxArr = (int *)a_rScanMgr.GetNextMapImg()->GetPixAt( 0, 0 );

		//Accessor2D< Hcpl::Int32 > ac1( &nextIdxArr[ 0 ], srcSiz.width, srcSiz.height );

		//const int nTestIdx = ac1.GetIndex1D( 323, 125 );
		const int nTestIdx = 124;

		int nCnt = 0;
		int nInrCnt = 0;

		F32Point testPnt;

		F32Point testBgnPnt( 124, 0 );

		FixedDeque< Hcpl::Int32 > indexDeque( nAprSizA );

		IndexCalc2D index2D( srcSiz.width, srcSiz.height );

		for( int i=0; i < rBgnIndexArr.GetSize(); i++ )
		{
			//Hcpl::Int32 nCurIdx = rBgnIndexArr[ i ];
			Hcpl::Int32 nBgnIdx = rBgnIndexArr[ i ];

			//F32Point rBgnPnt = rBgnPntArr[ i ];

			Hcpl::Int32 nCurIdx = nBgnIdx;

			indexDeque.ResetSize();


			//F32Point curPnt0 = GetPntOfIndex( nCurIdx, srcSiz );

			//if( F32Point::Compare( testBgnPnt, curPnt0 ) )
				//curPnt0 = curPnt0;


			const int nLim = rLimArr[ i ];

			const int nStart = rShiftStartArr[ i ];			

			{
				const int nMarg2 = nStart + nMarg;

				const int nLim2 = ( nMarg2 < nLim ) ? nMarg2 : nLim;

				for( int j = nStart; j < nLim2; j++ )
				{
					nCurIdx = nBgnIdx + rShiftIndexArr[ j ];

					a_retHead[ nCurIdx * a_nStepCnt ] = 0;
				}
			}

			{
				int nBgn2 = nLim - nMarg;

				if( nBgn2 < nStart )
					nBgn2 = nStart;

				for( int j = nBgn2; j < nLim; j++ )
				{
					nCurIdx = nBgnIdx + rShiftIndexArr[ j ];

					a_retHead[ nCurIdx * a_nStepCnt ] = 0;
				}
			}

			//while( true )
			for( int j = nStart; j < nLim; j++ )
			{
				nCurIdx = nBgnIdx + rShiftIndexArr[ j ];

				//F32Point resPnt = F32Point::Add(
					//rBgnPnt, rShiftArr[ j ] );

				//if( -1 == nCurIdx )
					//break;

				//F32Point curPnt = GetPntOfIndex( nCurIdx, srcSiz );

				//if( F32Point::Compare( testBgnPnt, curPnt ) )
					//curPnt = curPnt;

/*
				if( nTestIdx == nCurIdx )
				{
					//testPnt = a_rScanMgr.GetBgnPntArr()[ i ];

					testPnt = GetPntOfIndex( nCurIdx, srcSiz );

					nCurIdx = nCurIdx;
				}
*/

				nCnt++;

				indexDeque.PushBack( nCurIdx );

				if( indexDeque.GetSize() == nAprSizA )
				{
					nInrCnt++;

					const int nLastIdx = indexDeque.GetBack();
					const int nFirstIdx = indexDeque.GetFront();

					F32ColorVal * pLastColor = &srcColorArr[ nLastIdx ];
					F32ColorVal * pFirstColor = &srcColorArr[ nFirstIdx ];

					float difMag = F32ColorVal::Sub( *pLastColor, *pFirstColor ).CalcMag();

					//float unitDif = CalcColorUnitDif( *pLastColor, *pFirstColor );

					//difMag *= ( 1 + unitDif );
					//difMag *= ( 0.6 + unitDif );

					//if( difMag > 440 )
						//difMag = 440;

					const int nMedIdx = indexDeque[ nMarg ];

					if( 185856 == nMedIdx )
						i = i;


					a_retHead[ nMedIdx * a_nStepCnt ] = difMag;

					indexDeque.PopFront();
				}

				//Hcpl::Int32 nNextIdx = nextIdxArr[ nCurIdx ];
				//Hcpl::Int32 nNextIdx = nextIdxArr[ nCurIdx ];

				//nCurIdx = nNextIdx;
			}





		}

		nCnt = nCnt;

	}


	F32Point SubGradMgr::GetPntOfIndex( int a_nIndex, CvSize a_siz )
	{
		F32Point retPnt;

		retPnt.x = a_nIndex % a_siz.width;
		retPnt.y = a_nIndex / a_siz.width;

		return retPnt;
	}


}