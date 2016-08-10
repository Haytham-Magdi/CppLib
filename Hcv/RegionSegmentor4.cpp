#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\RegionSegmentor4.h>

namespace Hcv
{
	using namespace Hcpl;
	using namespace Hcpl::Math;


	RegionSegmentor4::RegionSegmentor4(S16ImageRef a_src, int a_nDifThreshold) : 
	m_rgnIndexCalc(a_src->GetWidth(), a_src->GetHeight()), m_nColorScale(100) 
	{	
		m_nDifThreshold = a_nDifThreshold;
		m_nofConflicts = 0;
		m_nofLinkActions = 0;
		m_nVisitID = 0;

		m_src = a_src;

		InitMaxDif();
		m_difQues.Init( GetMaxDif() + 1 );

		m_nDifIndex = GetMaxDif();

		InitRgnInfoVect();
		InitLinks();
		PrepareConflicts();
	}

	void RegionSegmentor4::InitRgnInfoVect(void)
	{
		m_rgnInfoVect.resize(m_src->GetWidth() * m_src->GetHeight());

		S16ChannelRef srcCh0 = m_src->GetAt(0);
		S16ChannelRef srcCh1 = m_src->GetAt(1);
		S16ChannelRef srcCh2 = m_src->GetAt(2);

		for(int y=0; y<srcCh0->GetHeight(); y++)
		{
			for(int x=0; x<srcCh0->GetWidth(); x++)
			{	
				RgnInfo * pRgnInfo = GetPointRgn(x, y);

				pRgnInfo->pActRgn = pRgnInfo;
				pRgnInfo->pPrev = pRgnInfo;

				pRgnInfo->colorMean.val0 = pRgnInfo->colorOrg.val0 = 
					srcCh0->GetAt(x, y) * m_nColorScale;
				pRgnInfo->colorMean.val1 = pRgnInfo->colorOrg.val1 = 
					srcCh1->GetAt(x, y) * m_nColorScale;
				pRgnInfo->colorMean.val2 = pRgnInfo->colorOrg.val2 = 
					srcCh2->GetAt(x, y) * m_nColorScale;
				
				//pRgnInfo->colorMean.val0 = 0;
				//pRgnInfo->colorMean.val1 = 0;
				//pRgnInfo->colorMean.val2 = 0;

				pRgnInfo->nPixCnt = 1;

				pRgnInfo->x = x;
				pRgnInfo->y = y;

				pRgnInfo->nLastVisitID = 0;

				RgnLink * links = pRgnInfo->links;
				for(int i=0; i<8; i++)
				{
					links[i].bProcessed = true;
					links[i].bExists = false;					
				}

				pRgnInfo->bHasConflicts = false;
			}
		}

	}

	inline void RegionSegmentor4::CreateLink( RgnInfo * a_pRgn, 
		int x, int y, RgnLinkDir a_dir)
	{	
		static int dx[] = {1, 1, 0, -1};
		static int dy[] = {0, 1, 1, 1};

		const int nLinkIndex = (int)a_dir;

		RgnInfo * pRgn2 = GetPointRgn( x + dx[ nLinkIndex ], 
			y + dy[ nLinkIndex ]);

		RgnLink & rLink = a_pRgn->links[ nLinkIndex ];
		RgnLink & rLinkR2 = pRgn2->links[ nLinkIndex + 4 ];

		rLink.bProcessed = rLinkR2.bProcessed = false;
		rLink.bExists = rLinkR2.bExists = true;
		
		rLink.pPeerRgn = pRgn2;
		rLinkR2.pPeerRgn = a_pRgn;

		int nDist = CalcColorDif( a_pRgn->colorOrg, 
			pRgn2->colorOrg );

		rLink.nDist = rLinkR2.nDist = nDist;

		if(	IsConflictDist(	nDist ) )
		{
			if( ! a_pRgn->bHasConflicts )
			{
				m_RgnOfConflictQue.PushPtr( a_pRgn );
				a_pRgn->bHasConflicts = true;
			}
			m_nofConflicts += 2;
		}
	}



	void RegionSegmentor4::InitLinks(void)
	{
		const int nSrcWidth = m_src->GetWidth();
		const int nSrcHeight = m_src->GetHeight();

		m_linkActionVect.resize( (4 * nSrcWidth * nSrcHeight) -
			nSrcHeight -	//	RC
			2 * (nSrcHeight + nSrcWidth - 1) -	//	RB + LB
			nSrcWidth	//	CB
			);

		for(int y=0; y<nSrcHeight-1; y++)
		{
			for(int x=1; x<nSrcWidth-1; x++)
			{	
				//	RC, RB, CB, LB

				RgnInfo * pRgn = GetPointRgn(x, y);

				CreateLink(pRgn, x, y, RC);
				CreateLink(pRgn, x, y, RB);
				CreateLink(pRgn, x, y, CB);
				CreateLink(pRgn, x, y, LB);
			}
		}

		for(int y=0, x=0; y<nSrcHeight-1; y++)
		{
			//	RC, RB, CB

			RgnInfo * pRgn = GetPointRgn(x, y);

			CreateLink(pRgn, x, y, RC);
			CreateLink(pRgn, x, y, RB);
			CreateLink(pRgn, x, y, CB);
		}

		for(int y=nSrcHeight-1, x=0; x<nSrcWidth-1; x++)
		{	
			// RC

			RgnInfo * pRgn = GetPointRgn(x, y);

			CreateLink(pRgn, x, y, RC);
		}

		for(int y=0, x=nSrcWidth-1; y<nSrcHeight-1; y++)
		{
			//	CB, LB

			RgnInfo * pRgn = GetPointRgn(x, y);

			CreateLink(pRgn, x, y, CB);
			CreateLink(pRgn, x, y, LB);
		}

	}

	S16ImageRef RegionSegmentor4::GenDifImg(bool a_bRC, bool a_bRB,
		bool a_bCB, bool a_bLB)
	{
		S16ImageRef ret = S16Image::Create(m_src->GetSize(), 1);
		ret->SetAll(0);

		S16ChannelRef retCh0 = ret->GetAt(0);

		bool dirFlags[] = {false, false, false, false};

		if(a_bRC)
		dirFlags[RC] = true;
		if(a_bRB)
		dirFlags[RB] = true;
		if(a_bCB)
		dirFlags[CB] = true;
		if(a_bLB)
		dirFlags[LB] = true;

		for(int i=0; i < m_rgnInfoVect.size(); i++)
		{
			RgnInfo * pRgn = &m_rgnInfoVect[i];

			RgnLink * links = pRgn->links;

			for(int j=0; j<4; j++)
			{
				if( links[j].bExists && dirFlags[j] )
				{
					if( pRgn->pActRgn != links[j].pPeerRgn->pActRgn )
					{
						retCh0->SetAt( pRgn->x, pRgn->y, 255);
					}
				}
			}
		}

		return ret;
	}



	
	HistoGramRef RegionSegmentor4::GenDifHisto(bool a_bRC, bool a_bRB,
		bool a_bCB, bool a_bLB)
	{
		HistoGramRef hist = new HistoGram( GetMaxDif() + 1 );
		/*		std::vector<Int32> & rHistVect = *hist->GetVectPtr();

		bool dirFlags[] = {false, false, false, false};

		if(a_bRC)
		dirFlags[RC] = true;
		if(a_bRB)
		dirFlags[RB] = true;
		if(a_bCB)
		dirFlags[CB] = true;
		if(a_bLB)
		dirFlags[LB] = true;


		ListQue< RgnLink > tmpQue;

		for(int i=0; i<m_difQues.GetNofQues(); i++)
		{
		RgnLink * pLink = m_difQues.PopPtr(i);

		while(NULL != pLink)
		{
		if( ! dirFlags[ pLink->dir ] )
		continue;

		rHistVect[ i ]++;

		tmpQue.PushPtr(pLink);
		pLink = m_difQues.PopPtr(i);
		}

		m_difQues.PushQue(i, &tmpQue);
		}*/

		return hist;
	}

	
	
	
	
	void RegionSegmentor4::Segment()
	{
		while( m_nDifIndex < m_difQues.GetNofQues() )
		{
			LinkAction * pLA = m_difQues.PopPtr( m_nDifIndex );

			if( NULL == pLA )
			{
				m_nDifIndex++;
				continue;
			}

			UpdateActRgn( pLA->pRgn1 );
			RgnInfo * pActRgn1 = pLA->pRgn1->pActRgn;

			UpdateActRgn( pLA->pRgn2 );
			RgnInfo * pActRgn2 = pLA->pRgn2->pActRgn;

			if( pActRgn1 == pActRgn2 )	
				continue;

			int nMeanDist = CalcColorDif( pActRgn1->colorMean,
				pActRgn2->colorMean );

			if( nMeanDist > m_nDifIndex )
			{
				if( ! IsConflictDist( nMeanDist ) )
				{
					m_difQues.PushPtr( nMeanDist, pLA );
				}
				else
				{
					RgnConflict * pRc = &m_rgnConflictVect[ m_nofConflicts++ ];
					pRc->pNext = pRc;
					pRc->pPrev = pRc;
					pRc->pPeerRgn = pActRgn2;
					pActRgn1->conflictList.PushLast( pRc );

					RgnConflict * pRc2 = &m_rgnConflictVect[ m_nofConflicts++ ];
					pRc2->pNext = pRc2;
					pRc2->pPrev = pRc2;
					pRc2->pPeerRgn = pActRgn1;
					pActRgn2->conflictList.PushLast( pRc2 );

					if( 1 == pActRgn2->nPixCnt )
					{
						PrepareRgnLinkActions( pLA->pRgn2 );
					}
				}

				continue;
			}


			m_nVisitID++;

			RgnInfo * pMinSizRgn, * pMaxSizRgn;

			if( pActRgn1->conflictList.GetSize() <
				pActRgn2->conflictList.GetSize() )
			{
				pMinSizRgn = pActRgn1;
				pMaxSizRgn = pActRgn2;
			}
			else
			{
				pMinSizRgn = pActRgn2;
				pMaxSizRgn = pActRgn1;
			}

			if( HaveConflict( pMinSizRgn, pMaxSizRgn ) )
			{
				continue;
			}
			else
			{
				RemoveDuplicateConflicts( pMaxSizRgn );

				RgnInfo * pMasterRgn, * pSlaveRgn;

				if( pActRgn1 < pActRgn2 )	
				{
					pMasterRgn = pActRgn1;
					pSlaveRgn = pActRgn2;
				}
				else
				{
					pMasterRgn = pActRgn2;
					pSlaveRgn = pActRgn1;
				}

				UpdateMasterRgn( pMasterRgn, pSlaveRgn );

				pSlaveRgn->pActRgn = pMasterRgn;

				PrepareRgnLinkActions( pLA->pRgn2 );
			}
		}	//	while

		for(int i=0; i < m_rgnInfoVect.size(); i++)
			UpdateActRgn( &m_rgnInfoVect[i] );
	}

	void RegionSegmentor4::UpdateMasterRgn( 
		RgnInfo * a_pMaster, RgnInfo * a_pSlave )
	{
		a_pMaster->conflictList.TakeListElements(
			a_pSlave->conflictList );

		const int nPixCntTot = a_pMaster->nPixCnt + a_pSlave->nPixCnt;
		
		a_pMaster->colorMean.val0 = 
			( a_pMaster->colorMean.val0 * a_pMaster->nPixCnt +
				a_pSlave->colorMean.val0 * a_pSlave->nPixCnt ) / nPixCntTot;

		a_pMaster->colorMean.val1 = 
			( a_pMaster->colorMean.val1 * a_pMaster->nPixCnt +
				a_pSlave->colorMean.val1 * a_pSlave->nPixCnt ) / nPixCntTot;

		a_pMaster->colorMean.val2 = 
			( a_pMaster->colorMean.val2 * a_pMaster->nPixCnt +
				a_pSlave->colorMean.val2 * a_pSlave->nPixCnt ) / nPixCntTot;

		a_pMaster->nPixCnt += a_pSlave->nPixCnt;
	}

	int RegionSegmentor4::CalcColorDif( 
		S32ColorVal & a_rColor1, S32ColorVal & a_rColor2)
	{
		int nDist = (int) sqrt( (double) (
			0.51 * Sqr( a_rColor1.val0 - a_rColor2.val0 ) +
			1.44 * Sqr( a_rColor1.val1 - a_rColor2.val1 ) +
			1.048 * Sqr( a_rColor1.val2 - a_rColor2.val2 ) ) ) / m_nColorScale;

		if( nDist > 441 )
			nDist = nDist;

		return nDist;
	}

/*	int RegionSegmentor4::CalcRgnDif(RgnInfo * a_pRgn1, RgnInfo * a_pRgn2)
	{
		int nDist = (int) sqrt( (double) (
			0.51 * Sqr( a_pRgn1->colorOrg.val0 - a_pRgn2->colorOrg.val0 ) +
			1.44 * Sqr( a_pRgn1->colorOrg.val1 - a_pRgn2->colorOrg.val1 ) +
			1.048 * Sqr( a_pRgn1->colorOrg.val2 - a_pRgn2->colorOrg.val2 ) ) );

		return nDist;
	}

	int RegionSegmentor4::CalcRgnMeanDif(RgnInfo * a_pActRgn1, RgnInfo * a_pActRgn2)
	{
		int nDist = (int) sqrt( (double) (
			0.51 * Sqr( a_pActRgn1->colorMean.val0 / a_pActRgn1->nPixCnt
				- a_pActRgn2->colorMean.val0 / a_pActRgn2->nPixCnt ) +
			1.44 * Sqr( a_pActRgn1->colorMean.val1 / a_pActRgn1->nPixCnt
				- a_pActRgn2->colorMean.val1 / a_pActRgn2->nPixCnt ) +
			1.048 * Sqr( a_pActRgn1->colorMean.val2 / a_pActRgn1->nPixCnt
				- a_pActRgn2->colorMean.val2 / a_pActRgn2->nPixCnt ) ) );

		return nDist;
	}*/

	void RegionSegmentor4::UpdateActRgn(RgnInfo * a_pRgn)
	{
		static std::vector<RgnInfo *> rgnVect(100);
		static int nMaxNofRgns = rgnVect.size();

		RgnInfo * pActRgn;
		do
		{
			pActRgn = a_pRgn;
			int nofRgns = 0;

			while(pActRgn != pActRgn->pActRgn && nofRgns < nMaxNofRgns)
			{
				rgnVect[nofRgns++] = pActRgn;
				pActRgn = pActRgn->pActRgn;
			}

			for(int i=0; i<nofRgns; i++)
				rgnVect[i]->pActRgn = pActRgn;

		}while( pActRgn != pActRgn->pActRgn );
	}

	S16ImageRef RegionSegmentor4::GenSegmentedImage(void)
	{
		S16ImageRef ret = S16Image::Create(m_src->GetSize(), 3);

/*		for(int i=0; i < m_rgnInfoVect.size(); i++)
		{
			RgnInfo * pRgn = &m_rgnInfoVect[i];
			RgnInfo * pActRgn = pRgn->pActRgn;

			pActRgn->colorMean.val0 += pRgn->colorOrg.val0;
			pActRgn->colorMean.val1 += pRgn->colorOrg.val1;
			pActRgn->colorMean.val2 += pRgn->colorOrg.val2;

			pActRgn->nPixCnt++;
		}

		for(int i=0; i < m_rgnInfoVect.size(); i++)
		{
			RgnInfo * pRgn = &m_rgnInfoVect[i];
			RgnInfo * pActRgn = pRgn->pActRgn;

			if( pActRgn == pRgn )
			{
				pRgn->colorMean.val0 /= pRgn->nPixCnt;
				pRgn->colorMean.val1 /= pRgn->nPixCnt;
				pRgn->colorMean.val2 /= pRgn->nPixCnt;
			}
		}*/

		S16ChannelRef retCh0 = ret->GetAt(0);
		S16ChannelRef retCh1 = ret->GetAt(1);
		S16ChannelRef retCh2 = ret->GetAt(2);

		for(int i=0; i < m_rgnInfoVect.size(); i++)
		{
			RgnInfo * pRgn = &m_rgnInfoVect[i];
			RgnInfo * pActRgn = pRgn->pActRgn;

			retCh0->SetAt( pRgn->x, pRgn->y, (short)pActRgn->colorMean.val0
				/ m_nColorScale );
			retCh1->SetAt( pRgn->x, pRgn->y, (short)pActRgn->colorMean.val1
				/ m_nColorScale );
			retCh2->SetAt( pRgn->x, pRgn->y, (short)pActRgn->colorMean.val2
				/ m_nColorScale );
		}

		return ret;
	}


	void RegionSegmentor4::PrepareConflicts()
	{
		// To be Revised
		m_rgnConflictVect.resize( m_nofConflicts * 10 );
		m_nofConflicts = 0;

		RgnInfo * pRgn = m_RgnOfConflictQue.PopPtr();

		while( NULL != pRgn )
		{
			RgnLink * links = pRgn->links;

			for(int i=0; i<4; i++)
			{
				RgnLink & rLink = links[i];

				if( rLink.bProcessed )
					continue;

				RgnInfo * pRgn2 = rLink.pPeerRgn;
				RgnLink & rLink2 = pRgn2->links[ i + 4 ];

				if( IsConflictDist(	rLink.nDist ) )
				{
					RgnConflict * pRc = &m_rgnConflictVect[ m_nofConflicts++ ];
					pRc->pNext = pRc;
					pRc->pPrev = pRc;
					pRc->pPeerRgn = pRgn2;
					pRgn->conflictList.PushLast( pRc );

					RgnConflict * pRc2 = &m_rgnConflictVect[ m_nofConflicts++ ];
					pRc2->pNext = pRc2;
					pRc2->pPrev = pRc2;
					pRc2->pPeerRgn = pRgn;
					pRgn2->conflictList.PushLast( pRc2 );

					rLink.bProcessed = rLink2.bProcessed = true;
				}
				else
				{
					PrepareLinkAction( pRgn, pRgn2, rLink, rLink2 );
				}
			}

			for(int i=4; i<8; i++)
			{
				RgnLink & rLink = links[i];

				if( rLink.bProcessed )
					continue;

				if( IsConflictDist(	rLink.nDist ) )
					continue;

				RgnInfo * pRgn2 = rLink.pPeerRgn;
				RgnLink & rLink2 = pRgn2->links[ i - 4 ];

				PrepareLinkAction( pRgn, pRgn2, rLink, rLink2 );
			}

			pRgn = m_RgnOfConflictQue.PopPtr();

//			if( NULL == pRgn->pActRgn )
//				pRgn = pRgn;

		}	//	while
	}

	void RegionSegmentor4::PrepareLinkAction( RgnInfo * a_pRgn, RgnInfo * a_pRgn2,
			RgnLink & a_rLink, RgnLink & a_rLink2)
	{
		LinkAction * pLA = &m_linkActionVect[ m_nofLinkActions++ ];

		pLA->pRgn1 = a_pRgn;
		pLA->pRgn2 = a_pRgn2;
		pLA->pPrev = pLA;

		m_difQues.PushPtr( a_rLink.nDist, pLA);

		if( a_rLink.nDist < m_nDifIndex )
			m_nDifIndex = a_rLink.nDist;

		a_rLink.bProcessed = a_rLink2.bProcessed = true;
	}

	bool RegionSegmentor4::HaveConflict( RgnInfo * a_pMinSizRgn, RgnInfo * a_pMaxSizRgn)
	{
		APtrList< RgnConflict > & minConfList = 
			a_pMinSizRgn->conflictList;

		RgnConflict * pConflict = minConfList.Last();

		while( NULL != pConflict )
		{
			RgnInfo * pPeerRgn = pConflict->pPeerRgn;

			UpdateActRgn( pPeerRgn );
			pPeerRgn = pPeerRgn->pActRgn;
 			pConflict->pPeerRgn = pPeerRgn;

			if( pPeerRgn == a_pMaxSizRgn )
				return true;

			pPeerRgn->nLastVisitID = m_nVisitID;

			pConflict = minConfList.Next();
		}

		return false;
	}

	void RegionSegmentor4::RemoveDuplicateConflicts( RgnInfo * a_pRgn )
	{
		APtrList< RgnConflict > & confList = 
			a_pRgn->conflictList;

		RgnConflict * pConflict = confList.Last();

		while( NULL != pConflict )
		{
			RgnInfo * pPeerRgn = pConflict->pPeerRgn;

			UpdateActRgn( pPeerRgn );
			pPeerRgn = pPeerRgn->pActRgn;
 			pConflict->pPeerRgn = pPeerRgn;

			if( pPeerRgn->nLastVisitID == m_nVisitID )
			{
				confList.RemovePtr( pConflict );
				pConflict = confList.Current();
			}
			else
			{
				pConflict = confList.Next();
			}
			//pPeerRgn->nLastVisitID = m_nVisitID;
		}
	}


	void RegionSegmentor4::PrepareRgnLinkActions( RgnInfo * a_pRgn )
	{
		RgnLink * links = a_pRgn->links;

		for(int i=0; i<4; i++)
		{
			RgnLink & rLink = links[i];

			if( rLink.bProcessed )
				continue;

			if( IsConflictDist(	rLink.nDist ) )
				continue;

			RgnInfo * pRgn2 = rLink.pPeerRgn;
			RgnLink & rLink2 = pRgn2->links[ i + 4 ];

			PrepareLinkAction( a_pRgn, pRgn2, rLink, rLink2 );
		}

		for(int i=4; i<8; i++)
		{
			RgnLink & rLink = links[i];

			if( rLink.bProcessed )
				continue;

			if( IsConflictDist(	rLink.nDist ) )
				continue;

			RgnInfo * pRgn2 = rLink.pPeerRgn;
			RgnLink & rLink2 = pRgn2->links[ i - 4 ];

			PrepareLinkAction( a_pRgn, pRgn2, rLink, rLink2 );
		}

	}

	S16ImageRef RegionSegmentor4::GenConflictImg(bool a_bRC, bool a_bRB,
			bool a_bCB, bool a_bLB)
	{
		S16ImageRef ret = S16Image::Create(m_src->GetSize(), 1);
		ret->SetAll(0);

		S16ChannelRef retCh0 = ret->GetAt(0);

		bool dirFlags[] = {false, false, false, false};

		if(a_bRC)
		dirFlags[RC] = true;
		if(a_bRB)
		dirFlags[RB] = true;
		if(a_bCB)
		dirFlags[CB] = true;
		if(a_bLB)
		dirFlags[LB] = true;

		for(int i=0; i < m_rgnInfoVect.size(); i++)
		{
			RgnInfo * pRgn = &m_rgnInfoVect[i];

			RgnLink * links = pRgn->links;

			for(int j=0; j<4; j++)
			{
				if( links[j].bExists && dirFlags[j] )
				{
					if( IsConflictDist( links[j].nDist ) )
					{
						retCh0->SetAt( pRgn->x, pRgn->y, 255);
					}
				}
			}
		}

		return ret;
	}


}