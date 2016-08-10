#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\RegionSegmentor6.h>

namespace Hcv
{
	using namespace Hcpl;
	using namespace Hcpl::Math;


	RegionSegmentor6::RegionSegmentor6(S16ImageRef a_src, int a_nDifThreshold) : 
	m_rgnIndexCalc(a_src->GetWidth(), a_src->GetHeight())
	{	
		m_nDifThreshold = a_nDifThreshold;
		m_nofConflicts = 0;
		m_nofFarConflicts = 0;
		m_nofLinkActions = 0;
		m_nVisitID = 0;

		m_src = a_src;

		InitMaxDif();
		m_difQues.Init( GetMaxDif() + 1 );

		//m_rgnConflictVect.resize( 2000000 );
		m_rgnConflictVect.resize( 15000000 );

		m_nDifIndex = GetMaxDif();

		InitRgnInfoVect();
		InitLinks();
		PrepareConflicts();
	}

	void RegionSegmentor6::InitRgnInfoVect(void)
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

				pRgnInfo->colorSum.val0 = pRgnInfo->colorOrg.val0 = 
					srcCh0->GetAt(x, y);
				pRgnInfo->colorSum.val1 = pRgnInfo->colorOrg.val1 = 
					srcCh1->GetAt(x, y);
				pRgnInfo->colorSum.val2 = pRgnInfo->colorOrg.val2 = 
					srcCh2->GetAt(x, y);
				
				//pRgnInfo->colorSum.val0 = 0;
				//pRgnInfo->colorSum.val1 = 0;
				//pRgnInfo->colorSum.val2 = 0;

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

	inline void RegionSegmentor6::CreateLink( RgnInfo * a_pRgn, 
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

		//if(	IsConflictDist(	nDist ) )
		if(	IsConflictDist(	nDist ) && false )
		{
			if( ! a_pRgn->bHasConflicts )
			{
				m_RgnOfConflictQue.PushPtr( a_pRgn );
				a_pRgn->bHasConflicts = true;
			}
			m_nofConflicts += 2;
		}
	}



	void RegionSegmentor6::InitLinks(void)
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

				PrepareFarConflicts( pRgn, x, y,
					nSrcWidth, nSrcHeight);
			}
		}

		for(int y=0, x=0; y<nSrcHeight-1; y++)
		{
			//	RC, RB, CB

			RgnInfo * pRgn = GetPointRgn(x, y);

			CreateLink(pRgn, x, y, RC);
			CreateLink(pRgn, x, y, RB);
			CreateLink(pRgn, x, y, CB);

			PrepareFarConflicts( pRgn, x, y,
				nSrcWidth, nSrcHeight);
		}

		for(int y=nSrcHeight-1, x=0; x<nSrcWidth-1; x++)
		{	
			// RC

			RgnInfo * pRgn = GetPointRgn(x, y);

			CreateLink(pRgn, x, y, RC);

			PrepareFarConflicts( pRgn, x, y,
				nSrcWidth, nSrcHeight);
		}

		for(int y=0, x=nSrcWidth-1; y<nSrcHeight-1; y++)
		{
			//	CB, LB

			RgnInfo * pRgn = GetPointRgn(x, y);

			CreateLink(pRgn, x, y, CB);
			CreateLink(pRgn, x, y, LB);

			PrepareFarConflicts( pRgn, x, y,
				nSrcWidth, nSrcHeight);
		}

	}

	S16ImageRef RegionSegmentor6::GenDifImg(bool a_bRC, bool a_bRB,
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



	
	HistoGramRef RegionSegmentor6::GenDifHisto(bool a_bRC, bool a_bRB,
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

	
	
	
	
	void RegionSegmentor6::Segment()
	{
		while( m_nDifIndex < m_difQues.GetNofQues() )
		{
			LinkAction * pLA = m_difQues.PopPtr( m_nDifIndex );

			if( NULL == pLA )
			{
				m_nDifIndex++;
				continue;
			}

			pLA->pRgn1->UpdateActRgn();
			RgnInfo * pActRgn1 = pLA->pRgn1->pActRgn;

			pLA->pRgn2->UpdateActRgn();
			RgnInfo * pActRgn2 = pLA->pRgn2->pActRgn;

			if( pActRgn1 == pActRgn2 )	
				continue;

			/*int nMeanDist = CalcColorDif( 
				GetLocalMeanColor( pLA->pRgn1->colorOrg,				
				pActRgn1->GetMeanColor()),
				GetLocalMeanColor( pLA->pRgn2->colorOrg,				
				pActRgn2->GetMeanColor()));*/

			/*int nMeanDist = CalcColorDif( 
				pLA->pRgn1->GetLocalMeanColor(),
				pLA->pRgn2->GetLocalMeanColor());

			if( nMeanDist > m_nDifIndex )
			{
				//if( ! IsConflictDist( (nMeanDist * 2)/1 ) )
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
			}*/


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
			m_rgnInfoVect[i].UpdateActRgn();
	}

	void RegionSegmentor6::UpdateMasterRgn( 
		RgnInfo * a_pMaster, RgnInfo * a_pSlave )
	{
		a_pMaster->conflictList.TakeListElements(
			a_pSlave->conflictList );

		a_pMaster->colorSum.val0 += a_pSlave->colorSum.val0;
		a_pMaster->colorSum.val1 += a_pSlave->colorSum.val1;
		a_pMaster->colorSum.val2 += a_pSlave->colorSum.val2;

		a_pMaster->nPixCnt += a_pSlave->nPixCnt;
	}

	int RegionSegmentor6::CalcColorDif( 
		S32ColorVal & a_rColor1, S32ColorVal & a_rColor2)
	{
		int nDist = (int) sqrt( (double) (
			0.51 * Sqr( a_rColor1.val0 - a_rColor2.val0 ) +
			1.44 * Sqr( a_rColor1.val1 - a_rColor2.val1 ) +
			1.048 * Sqr( a_rColor1.val2 - a_rColor2.val2 ) ) );

		if( nDist > 441 )
			nDist = nDist;

		return nDist;
	}

/*	int RegionSegmentor6::CalcRgnDif(RgnInfo * a_pRgn1, RgnInfo * a_pRgn2)
	{
		int nDist = (int) sqrt( (double) (
			0.51 * Sqr( a_pRgn1->colorOrg.val0 - a_pRgn2->colorOrg.val0 ) +
			1.44 * Sqr( a_pRgn1->colorOrg.val1 - a_pRgn2->colorOrg.val1 ) +
			1.048 * Sqr( a_pRgn1->colorOrg.val2 - a_pRgn2->colorOrg.val2 ) ) );

		return nDist;
	}

	int RegionSegmentor6::CalcRgnMeanDif(RgnInfo * a_pActRgn1, RgnInfo * a_pActRgn2)
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

	void RegionSegmentor6::_RgnInfo::UpdateActRgn()
	{
		RgnInfo * a_pRgn = this;

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

	S16ImageRef RegionSegmentor6::GenSegmentedImage(void)
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

			S32ColorVal colorMean = pActRgn->GetMeanColor();

			retCh0->SetAt( pRgn->x, pRgn->y, (short)colorMean.val0);
			retCh1->SetAt( pRgn->x, pRgn->y, (short)colorMean.val1);
			retCh2->SetAt( pRgn->x, pRgn->y, (short)colorMean.val2);
		}

		return ret;
	}


	void RegionSegmentor6::PrepareConflicts()
	{
		// To be Revised
		const int nResiz = m_nofFarConflicts + m_nofConflicts * 10;
		if( nResiz > m_rgnConflictVect.size() )
			m_rgnConflictVect.resize( m_nofConflicts * 10 );
		m_nofConflicts = m_nofFarConflicts;

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

	void RegionSegmentor6::PrepareLinkAction( RgnInfo * a_pRgn, RgnInfo * a_pRgn2,
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

	bool RegionSegmentor6::HaveConflict( RgnInfo * a_pMinSizRgn, RgnInfo * a_pMaxSizRgn)
	{
		APtrList< RgnConflict > & minConfList = 
			a_pMinSizRgn->conflictList;

		RgnConflict * pConflict = minConfList.Last();

		while( NULL != pConflict )
		{
			RgnInfo * pPeerRgn = pConflict->pPeerRgn;

			pPeerRgn->UpdateActRgn();
			pPeerRgn = pPeerRgn->pActRgn;
 			pConflict->pPeerRgn = pPeerRgn;

			if( pPeerRgn == a_pMaxSizRgn )
				return true;

			pPeerRgn->nLastVisitID = m_nVisitID;

			pConflict = minConfList.Next();
		}

		return false;
	}

	void RegionSegmentor6::RemoveDuplicateConflicts( RgnInfo * a_pRgn )
	{
		APtrList< RgnConflict > & confList = 
			a_pRgn->conflictList;

		RgnConflict * pConflict = confList.Last();

		while( NULL != pConflict )
		{
			RgnInfo * pPeerRgn = pConflict->pPeerRgn;

			pPeerRgn->UpdateActRgn();
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


	void RegionSegmentor6::PrepareRgnLinkActions( RgnInfo * a_pRgn )
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

	S16ImageRef RegionSegmentor6::GenConflictImg(bool a_bRC, bool a_bRB,
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

	S32ColorVal RegionSegmentor6::GetLocalMeanColor( 
		S32ColorVal & a_rColorLocal, S32ColorVal & a_rColorMean)
	{
		S32ColorVal ret;

		int nLocPercent = 0;
				
		ret = S32ColorVal::Add( a_rColorLocal.MultBy( nLocPercent ),
			a_rColorMean.MultBy( 100 - nLocPercent ) ).DividBy(100);

		return ret;
	}


	S32ColorVal RegionSegmentor6::_RgnInfo::GetLocalMeanColor()
	{
		S32ColorVal ret;

		const int nAprSiz = 3;
		const int nAprLim1 = ((nAprSiz/2) + 1);

		ret.val0 = 0;
		ret.val1 = 0;
		ret.val2 = 0;

		int nWidth = 1;
		int nHeight = 1;

		RgnInfo * pRgn = this;
		int nLinkIndex;


		nLinkIndex =  (int)RC + 4;
		while( pRgn->links[ nLinkIndex ].bExists 
			&& nWidth < nAprLim1)
		{
			nWidth++;
			pRgn = pRgn->links[ nLinkIndex ].pPeerRgn;
		}

		nLinkIndex =  (int)CB + 4;
		while( pRgn->links[ nLinkIndex ].bExists 
			&& nHeight < nAprLim1)
		{
			nHeight++;
			pRgn = pRgn->links[ nLinkIndex ].pPeerRgn;
		}

		RgnInfo * pBgnRgn = pRgn;

		pRgn = this;

		if( 759 == this->x )
			pRgn = pRgn;

		nLinkIndex =  (int)RC;
		while( pRgn->links[ nLinkIndex ].bExists 
			&& nWidth < nAprSiz)
		{
			nWidth++;
			pRgn = pRgn->links[ nLinkIndex ].pPeerRgn;
		}

		nLinkIndex =  (int)CB;
		while( pRgn->links[ nLinkIndex ].bExists 
			&& nHeight < nAprSiz)
		{
			nHeight++;
			pRgn = pRgn->links[ nLinkIndex ].pPeerRgn;
		}

		RgnInfo * pBgnRgnX = pBgnRgn;

		const int nLinkX = (int)RC;
		const int nLinkY = (int)CB;

		int nRgnCnt = 0;

		for(int y=0; y < nHeight; y++)
		{
			RgnInfo * pRgn = pBgnRgnX;

			for(int x=0; x < nWidth; x++)
			{
				pRgn->UpdateActRgn();
				if( pRgn->pActRgn == this->pActRgn )
				{
					S32ColorVal & rgnColor = pRgn->colorOrg;

					ret.val0 += rgnColor.val0;
					ret.val1 += rgnColor.val1;
					ret.val2 += rgnColor.val2;

					nRgnCnt++;
				}

				RgnInfo * pRgnTmp = 
					pRgn->links[ nLinkX ].pPeerRgn;

				if( NULL == pRgnTmp )				
					pRgn = pRgn;

				pRgn = pRgnTmp;
			}

			pBgnRgnX = pBgnRgnX->links[ nLinkY ].pPeerRgn;
		}
		
		ret = ret.DividBy( nRgnCnt );

		return ret;
	}

	void RegionSegmentor6::PrepareFarConflicts( RgnInfo * a_pRgn, 
		int x, int y, const int nSrcWidth, const int nSrcHeight)
	{
		//return;

		static int scales[] = {10};

		static const int nofScales = sizeof(scales) / sizeof(int);

		for(int j=0; j < nofScales; j++)
		{
			//static int dx[] = {1, 1, 0, -1};
			//static int dy[] = {0, 1, 1, 1};

			static int dx[] = {1, 0};
			static int dy[] = {0, 1};

			static const int nofFarNbrs = sizeof(dx) / sizeof(int);

			for(int i=0; i < nofFarNbrs; i++)
			{
				const int nbrX = x + scales[j] * dx[i];
				if( nbrX < 0 || nbrX > (nSrcWidth - 1) )
					continue;

				const int nbrY = y + scales[j] * dy[i];
				if( nbrY < 0 || nbrY > (nSrcHeight - 1) )
					continue;

				RgnInfo * pNbrRgn = GetPointRgn( nbrX, nbrY);

				int nDist = CalcColorDif( 
					a_pRgn->colorOrg, pNbrRgn->colorOrg);

				if( IsConflictDist( nDist * 0.5 ) )
				//if( IsConflictDist( nDist ) )
				{
					RgnConflict * pRc = &m_rgnConflictVect[ m_nofFarConflicts++ ];
					pRc->pNext = pRc;
					pRc->pPrev = pRc;
					pRc->pPeerRgn = pNbrRgn;
					a_pRgn->conflictList.PushLast( pRc );

					RgnConflict * pRc2 = &m_rgnConflictVect[ m_nofFarConflicts++ ];
					pRc2->pNext = pRc2;
					pRc2->pPrev = pRc2;
					pRc2->pPeerRgn = a_pRgn;
					pNbrRgn->conflictList.PushLast( pRc2 );
				}

			}
		}
	}

}