#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\RegionSegmentor2.h>

namespace Hcv
{
	using namespace Hcpl;
	using namespace Hcpl::Math;


	RegionSegmentor2::RegionSegmentor2(S16ImageRef a_src, int a_nDifThreshold) : 
	m_rgnIndexCalc(a_src->GetWidth(), a_src->GetHeight()) 
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

	void RegionSegmentor2::InitRgnInfoVect(void)
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

				pRgnInfo->nValCh0 = srcCh0->GetAt(x, y);
				pRgnInfo->nValCh1 = srcCh1->GetAt(x, y);
				pRgnInfo->nValCh2 = srcCh2->GetAt(x, y);

				pRgnInfo->nMeanValCh0 = 0;
				pRgnInfo->nMeanValCh1 = 0;
				pRgnInfo->nMeanValCh2 = 0;

				pRgnInfo->nPixCnt = 0;

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

	inline void RegionSegmentor2::CreateLink( RgnInfo * a_pRgn, 
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

		int nDist = CalcRgnDif( a_pRgn, pRgn2 );

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



	void RegionSegmentor2::InitLinks(void)
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

	S16ImageRef RegionSegmentor2::GenDifImg(bool a_bRC, bool a_bRB,
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



	
	HistoGramRef RegionSegmentor2::GenDifHisto(bool a_bRC, bool a_bRB,
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

	
	
	
	
	void RegionSegmentor2::Segment()
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

				if( pActRgn1 < pActRgn2 )	
				{
					pActRgn1->conflictList.TakeListElements(
						pActRgn2->conflictList );
					pActRgn2->pActRgn = pActRgn1;
				}
				else
				{
					pActRgn2->conflictList.TakeListElements(
						pActRgn1->conflictList );
					pActRgn1->pActRgn = pActRgn2;
				}

				PrepareRgnLinkActions( pLA->pRgn2 );
			}
		}	//	while

		for(int i=0; i < m_rgnInfoVect.size(); i++)
			UpdateActRgn( &m_rgnInfoVect[i] );
	}

	int RegionSegmentor2::CalcRgnDif(RgnInfo * a_pRgn1, RgnInfo * a_pRgn2)
	{
		int nDist = (int) sqrt( (double) (
			0.51 * Sqr( a_pRgn1->nValCh0 - a_pRgn2->nValCh0 ) +
			1.44 * Sqr( a_pRgn1->nValCh1 - a_pRgn2->nValCh1 ) +
			1.048 * Sqr( a_pRgn1->nValCh2 - a_pRgn2->nValCh2 ) ) );

		return nDist;
	}


	void RegionSegmentor2::UpdateActRgn(RgnInfo * a_pRgn)
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

	S16ImageRef RegionSegmentor2::GenSegmentedImage(void)
	{
		S16ImageRef ret = S16Image::Create(m_src->GetSize(), 3);

		for(int i=0; i < m_rgnInfoVect.size(); i++)
		{
			RgnInfo * pRgn = &m_rgnInfoVect[i];
			RgnInfo * pActRgn = pRgn->pActRgn;

			pActRgn->nMeanValCh0 += pRgn->nValCh0;
			pActRgn->nMeanValCh1 += pRgn->nValCh1;
			pActRgn->nMeanValCh2 += pRgn->nValCh2;

			pActRgn->nPixCnt++;
		}

		for(int i=0; i < m_rgnInfoVect.size(); i++)
		{
			RgnInfo * pRgn = &m_rgnInfoVect[i];
			RgnInfo * pActRgn = pRgn->pActRgn;

			if( pActRgn == pRgn )
			{
				pRgn->nMeanValCh0 /= pRgn->nPixCnt;
				pRgn->nMeanValCh1 /= pRgn->nPixCnt;
				pRgn->nMeanValCh2 /= pRgn->nPixCnt;
			}
		}

		S16ChannelRef retCh0 = ret->GetAt(0);
		S16ChannelRef retCh1 = ret->GetAt(1);
		S16ChannelRef retCh2 = ret->GetAt(2);

		for(int i=0; i < m_rgnInfoVect.size(); i++)
		{
			RgnInfo * pRgn = &m_rgnInfoVect[i];
			RgnInfo * pActRgn = pRgn->pActRgn;

			retCh0->SetAt( pRgn->x, pRgn->y, (short)pActRgn->nMeanValCh0 );
			retCh1->SetAt( pRgn->x, pRgn->y, (short)pActRgn->nMeanValCh1 );
			retCh2->SetAt( pRgn->x, pRgn->y, (short)pActRgn->nMeanValCh2 );
		}

		return ret;
	}


	void RegionSegmentor2::PrepareConflicts()
	{
		m_rgnConflictVect.resize( m_nofConflicts );
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

	void RegionSegmentor2::PrepareLinkAction( RgnInfo * a_pRgn, RgnInfo * a_pRgn2,
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

	bool RegionSegmentor2::HaveConflict( RgnInfo * a_pMinSizRgn, RgnInfo * a_pMaxSizRgn)
	{
		bool bRet = false;

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

			RgnConflict * pNext = minConfList.Next();
			
			if( pPeerRgn->nLastVisitID == m_nVisitID )
			{
				minConfList.RemovePtr( pConflict );
			}

			pPeerRgn->nLastVisitID = m_nVisitID;

			pConflict = pNext;
		}

		return false;
	}

	void RegionSegmentor2::RemoveDuplicateConflicts( RgnInfo * a_pRgn )
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


	void RegionSegmentor2::PrepareRgnLinkActions( RgnInfo * a_pRgn )
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

	S16ImageRef RegionSegmentor2::GenConflictImg(bool a_bRC, bool a_bRB,
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