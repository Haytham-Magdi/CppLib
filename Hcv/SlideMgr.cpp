#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\SlideMgr.h>

namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;


	SlideMgr::SlideMgr( F32ImageRef a_src, int a_nAprSiz , 
			FixedVector< SlideChainElm > * a_pSlideChainArr, 
			FixedVector< bool > * a_pIsRootArr)
	{
		m_src = a_src;
		m_nAprSiz = a_nAprSiz;

		if( NULL == a_pSlideChainArr )
			m_pSlideChainArr = &m_slideChainArrLc;
		else
			m_pSlideChainArr = a_pSlideChainArr;

		if( NULL == a_pIsRootArr )
			m_pIsRootArr = &m_isRootArrLc;
		else
			m_pIsRootArr = a_pIsRootArr;

		PrepareSlideChainArr();
	}

	void SlideMgr::InitSlideChainArr()
	{
		FixedVector< SlideChainElm > & rSlideChainArr = *m_pSlideChainArr;

		CvSize srcSiz = m_src->GetSize();
		const int nSrcSiz1D = srcSiz.width * srcSiz.height;

		rSlideChainArr.SetSize( nSrcSiz1D );

		memset( &rSlideChainArr[0], 0, 
			rSlideChainArr.GetSize() * sizeof( SlideChainElm ) );

		for( int i=0; i < rSlideChainArr.GetSize(); i++ )
		{
			rSlideChainArr[ i ].index = i;
		}
	}


	void  SlideMgr::InitIsRootArr()
	{
		FixedVector< bool > & rIsRootArr = *m_pIsRootArr;

		CvSize srcSiz = m_src->GetSize();
		const int nSrcSiz1D = srcSiz.width * srcSiz.height;

		rIsRootArr.SetSize( nSrcSiz1D );

		for( int i=0; i < rIsRootArr.GetSize(); i++ )
			rIsRootArr[ i ] = false;
	}

	void  SlideMgr::PrepareSlideChainArr()
	{
		InitIsRootArr();
		FixedVector< bool > & rIsRootArr = *m_pIsRootArr;

		InitSlideChainArr();
		FixedVector< SlideChainElm > & rSlideChainArr = *m_pSlideChainArr;

		m_chainHeadPtrArr.SetCapacity( rSlideChainArr.GetSize() );



		F32ImageRef src = m_src;
		CvSize srcSiz = m_src->GetSize();

		const int nSrcSiz1D = srcSiz.width * srcSiz.height;



		FixedVector< F32Point > pntArr;
		pntArr.SetSize( nSrcSiz1D );

		FillSlidePointArr( src, m_nAprSiz, &pntArr );



		Accessor2D< F32Point > acSlide( &pntArr[0], srcSiz.width, srcSiz.height );
		Accessor2D< SlideChainElm > acSlideCE( &rSlideChainArr[0], srcSiz.width, srcSiz.height );

		F32ImageRef avgImg = GenFastAvgImg( src, m_nAprSiz );


		for( int y=0; y < srcSiz.height; y++ )
		{
			for( int x=0; x < srcSiz.width; x++ )
			{			
				int i = acSlideCE.GetIndex1D( x, y );

				SlideChainElm & rSlideCE = rSlideChainArr[ i ];

				rSlideCE.index = i;
				//rSlideCE.pos = F32Point( x, y );

				F32Point & rPnt = acSlide.GetAt( x, y );
				F32Point pnt1 = rPnt.Round();

				//rSlideCE.pSlidePix = &rSlideChainArr[ acSlide.GetIndex1D( pnt1.x, pnt1.y ) ];
				rSlideCE.pNext = acSlideCE.GetPtrAt( pnt1.x, pnt1.y );

				rSlideCE.val = *src->GetPixAt( x, y );
				rSlideCE.avgVal = *avgImg->GetPixAt( x, y );

			}
		}


		const F32Point testPnt( 409, 28 );

/*
		for( int y=0; y < srcSiz.height; y++ )
		{
			for( int x=0; x < srcSiz.width; x++ )
			{
				SlideChainElm * pSrcCE = acSlideCE.GetPtrAt( x, y );
				SlideChainElm * pDstCE = pSrcCE->pNext;

				int a = pSrcCE->index;
				int b = pDstCE->index;

				{
					//F32Point & rDstPos = pDstCE->pos;
					F32Point & rDstPos = GetPosOfIndex( pDstCE->index, srcSiz.height );				

					if( testPnt.x == rDstPos.x && testPnt.y == rDstPos.y )
						x = x;
				}

				const float srcVal = pSrcCE->val;
				
				const float avgSrcVal = *avgImg->GetPixAt( x, y );			
				
				const float dstVal = pDstCE->val;

				const float avgDstVal = pDstCE->avgVal;


				
				//if( dstVal < avgDstVal * 0.8 )
				if( dstVal < avgDstVal * 0.8 &&
					dstVal < avgSrcVal 
					)
				{
					if( dstVal < srcVal )
					{
						//color1.AssignVal( 255, 70, 70 );
						rIsRootArr[ pDstCE->index ] = true;
					}
					else if( dstVal == srcVal )
					{
						//color1.AssignVal( 0, 0, 255 );
						rIsRootArr[ pDstCE->index ] = true;
					}
					else 
					{
						//color1.AssignVal( 0, 0, 0 );
						rIsRootArr[ pDstCE->index ] = false;
					}
				}
				else
				{
					//color1 = u8ColorVal( 0, 0, 0 );
					//color1.AssignVal( 0, 0, 0 );
					rIsRootArr[ pDstCE->index ] = false;
				}



			}
		}
*/



		class IndexLQM : public MultiListQueMember< IndexLQM >
		{
		public:
			int Index;
		};



		FixedVector< IndexLQM > indexLQMArr;
		indexLQMArr.SetSize( nSrcSiz1D );

		float * srcPixPtr = src->GetPixAt( 0, 0 );

		const int nValScale = 10;


		MultiListQueMgr< IndexLQM > mlqMgr;
		mlqMgr.InitSize( 500 * nValScale );

		for( int i=0; i < nSrcSiz1D; i++ )
		{	
			IndexLQM & indexLQM = indexLQMArr[ i ];

			indexLQM.Index = i;

			float val = srcPixPtr[ i ];

			mlqMgr.PushPtr( val * nValScale, &indexLQM );
		}

		int nCurrentAction = 1;

		while(true)
		{
			nCurrentAction++;

			IndexLQM * pIlqm = mlqMgr.PopPtrMax();

			if( NULL == pIlqm )
				break;


			SlideChainElm * pCE = &rSlideChainArr[ pIlqm->Index ];

			//if( NULL == pCE->pHead )
			//	pCE->pHead = pCE;
			//else
			//	continue;

			if( NULL != pCE->pHead )
				continue;

			//SlideChainElm * pHeadCE = pCE->pHead;
			SlideChainElm * pHeadCE = pCE;

			//if( pHeadCE->val < 5 )
				//continue;

			//if( pHeadCE->val <= pHeadCE->avgVal * 0.8 )
			//	continue;

			SlideChainElm * pDstCE = pCE->pNext;

			if( pDstCE == pCE )
			{
				//pHeadCE->pTail = pDstCE;

				continue;
			}



			bool bFlg = true;

			while( bFlg )
			{
				pCE->nLastAction = nCurrentAction;

				pDstCE = pCE->pNext;

				float srcVal = pCE->val;
				float avgVal = pCE->avgVal;			

				float dstVal = pDstCE->val;
				float dstAvgVal = pDstCE->avgVal;

				//GlobalStuff::SetPoint1( pnt1 );





				{
					//int nRadius = 2;
					//int nRadius = 3;
					//int nRadius = 4;

					//U8ColorVal color1;
					{

						if( nCurrentAction == pDstCE->nLastAction )
						{
							bFlg = false;
						}
						//else if( F32Point::Compare( pnt1, srcPnt ) )
						else if( pDstCE == pCE )
						{
							//color1 = u8ColorVal( 0, 0, 255 );
							pHeadCE->pTail = pDstCE;

							bFlg = false;
						}
						//else if( dstVal >= avgVal || dstVal >= srcVal )
						else if( dstVal * 0.8 > srcVal || dstAvgVal * 0.8 > avgVal )
						{
							//color1 = u8ColorVal( 255, 255, 255 );

							bFlg = false;
						}
						else if( dstVal > avgVal )
						{
							//color1 = u8ColorVal( 0, 150, 255 );

						}
						//else if( dstVal < avgVal )
						else if( dstVal <= avgVal )
						{
							//color1 = u8ColorVal( 0, 180, 0 );
						}
						else
						{
							bFlg = false;
						}

						//DrawCircle( dsp, pnt1, color1, nRadius );
					}

/*
					U8ColorVal color0;
					{

						if( F32Point::Compare( pnt1, srcPnt ) )
						{
							if( srcVal > avgVal )
								color0 = u8ColorVal( 0, 0, 100 );
							else
								color0 = u8ColorVal( 0, 0, 255 );
						}
						//else if( srcVal < avgVal * 0.6666 )
						else if( srcVal < avgVal * 0.8 )
							color0 = u8ColorVal( 255, 0, 0 );
						else
							color0 = u8ColorVal( 255, 0, 150 );

						DrawCircle( dsp, srcPnt, color0, nRadius );
					}
*/

/*
					{
						FixedVector< float > & rValArr = avgValArr;
						rValArr.PushBack( dstAvgVal );					

						FixedVector< U8ColorVal > & rColorArr = avgValColorArr;

						rColorArr.PushBack( u8ColorVal( dstAvgVal, dstAvgVal, dstAvgVal ) );					
					}

					{
						FixedVector< float > & rValArr = valArr;
						rValArr.PushBack( dstVal );

						FixedVector< U8ColorVal > & rColorArr = valColorArr;

						rColorArr.PushBack( color1 );
					}
*/

				}

				pCE->pHead = pHeadCE;

				//srcPnt = pnt1;
				if( ! bFlg )
				{
					pHeadCE->pTail = pCE;

					SlideChainElm * pTailCE = pHeadCE->pTail;

					pCE->bHasNext = false;

					if( pTailCE->val >= pHeadCE->val * 0.5 )
						continue;

					rIsRootArr[ pCE->index ] = true;
					m_chainHeadPtrArr.PushBack( pHeadCE );
				}
				else
				{
					pCE->bHasNext = true;

					pCE = pDstCE;					
				}

				//pCE = pDstCE;

			}

		}











		//FillIsEdgeRootArr( src, &rIsRootArr );


	}

}