#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\RegionSegmentor15.h>

namespace Hcv
{
	using namespace Hcpl;
	using namespace Hcpl::Math;


	RegionSegmentor15::RegionSegmentor15(S16ImageRef a_src, S16ImageRef a_rootImg,
		int a_nDifThreshold, S16ImageRef a_rootValImg) : 
	m_rgnIndexCalc(a_src->GetWidth(), a_src->GetHeight()) 
	{	
		m_nDifThreshold = a_nDifThreshold;
		m_nofConflicts = 0;
		m_nofPrimConfRgns = 0;
		m_nofLinkActions = 0;
		m_nVisitID = 0;

		m_src = a_src;
		m_rootImg = a_rootImg;

		if( NULL == a_rootValImg )
			a_rootValImg = m_src;
		m_rootValImg = a_rootValImg;

		InitMaxDif();
		//m_difQues.Init( GetMaxDif() + 1 );
		m_difQues.Init( GetMaxDif() * 1000 + 1 );

		m_nDifIndex = GetMaxDif();

		m_nDifIndexLim = 0;

		InitRgnInfoVect();
		InitLinks();		

		//RgnInfo * pRgn81 = GetPointRgn(81, 271);

		m_rgnConflictVect.resize( 
			10000000);
		
		m_nofConflicts = 0;
	}

	void RegionSegmentor15::InitRgnInfoVect(void)
	{
		m_rgnInfoVect.resize(m_src->GetWidth() * m_src->GetHeight());

		S16ChannelRef srcCh0 = m_src->GetAt(0);
		S16ChannelRef srcCh1 = m_src->GetAt(1);
		S16ChannelRef srcCh2 = m_src->GetAt(2);

		S16ChannelRef rvCh0 = m_rootValImg->GetAt(0);
		S16ChannelRef rvCh1 = m_rootValImg->GetAt(1);
		S16ChannelRef rvCh2 = m_rootValImg->GetAt(2);

		S16ChannelRef rootCh0 = m_rootImg->GetAt(0);



		for(int y=0; y<srcCh0->GetHeight(); y++)
		{
			for(int x=0; x<srcCh0->GetWidth(); x++)
			{	
				RgnInfo * pRgnInfo = GetPointRgn(x, y);

				pRgnInfo->pPrev = pRgnInfo;

				

				{
					int nRootImgVal = rootCh0->GetAt(x, y);

					if(nRootImgVal > 1)
					{
						pRgnInfo->pRootRgn = pRgnInfo;
						pRgnInfo->SetActRgn( pRgnInfo );
						pRgnInfo->bIsRoot = true;

						pRgnInfo->nValCh0 = rvCh0->GetAt(x, y);
						pRgnInfo->nValCh1 = rvCh1->GetAt(x, y);
						pRgnInfo->nValCh2 = rvCh2->GetAt(x, y);
					}
					else
					{
						pRgnInfo->pRootRgn = NULL;
						pRgnInfo->bIsRoot = false;

						pRgnInfo->nValCh0 = srcCh0->GetAt(x, y);
						pRgnInfo->nValCh1 = srcCh1->GetAt(x, y);
						pRgnInfo->nValCh2 = srcCh2->GetAt(x, y);
					}					

					if(nRootImgVal < 0)
					{
						pRgnInfo->bIsEdge = true;
					}
					else
					{
						pRgnInfo->bIsEdge = false;
					}
				}

				pRgnInfo->bIsN2E = false;
				pRgnInfo->bIsE2E = false;

				pRgnInfo->nMaxDistMet = 0;

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

	inline void RegionSegmentor15::CreateLink( RgnInfo * a_pRgn, 
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

/*		int nDist;
		
		if( pRgn2->bIsEdge )
		{
			nDist = CalcRgnDif( a_pRgn, pRgn2 );		
			//nDist = 0;
			//nDist = GetMaxDif();
			//nDist = 5000;
		}
		else
		{		
			nDist = CalcRgnDif( a_pRgn, pRgn2 );		
		}

		// For a Diagonal Link
		if( 0 != dx[nLinkIndex] && 0 != dy[nLinkIndex] )
			nDist = nDist;
			//nDist /= 1.4;
			//nDist *= 1.4;

		// hthm
		//nDist = -1;

		//rLink.nDist = rLinkR2.nDist = nDist;
		rLink.nDist = rLinkR2.nDist = -1;

		if( nDist > (m_nDifIndexLim - 1) )
			m_nDifIndexLim = nDist + 1;*/

		rLink.nDist = rLinkR2.nDist = -1;

		if( a_pRgn->bIsRoot )
		{			
			PrepareLinkAction( a_pRgn, pRgn2, rLink, rLinkR2, 0);
		}
		else if( pRgn2->bIsRoot )
		{
			PrepareLinkAction( pRgn2, a_pRgn, rLinkR2, rLink, 0);
		}

	}




	void RegionSegmentor15::InitLinks(void)
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

	S16ImageRef RegionSegmentor15::GenDifImg(bool a_bRC, bool a_bRB,
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
					if( pRgn->GetActRgn() != links[j].pPeerRgn->GetActRgn() )
					{
						retCh0->SetAt( pRgn->x, pRgn->y, 255);
					}
				}
			}
		}

		return ret;
	}



	
	HistoGramRef RegionSegmentor15::GenDifHisto(bool a_bRC, bool a_bRB,
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

	
	
	
	
	void RegionSegmentor15::Segment()
	{
		int nMaxConfCnt = 0;

		int nConfCntSumMax = 0;
		int nConfCntSumMin = 0;

		int nConfCntCnt = 0;
		int nZeroConfCnt = 0;

		ListQue< LinkAction > tmpQue;

		ListQue< LinkAction > interRootQue;

		for(int i=0; i < 2; i++)
		//for(int i=0; i < 1; i++)
		{

			//while( m_nDifIndex < m_difQues.GetNofQues() )
			while( m_nDifIndex < m_nDifIndexLim )			
			{
				bool bFromTmp = false; 

				LinkAction * pLA = m_difQues.PopPtr( m_nDifIndex );

				if( NULL == pLA )
				{
/*					if(	NULL != ( pLA = tmpQue.PopPtr() ) )
					{
						bFromTmp = true; 
						goto ValidLA;
					}
					else*/
					{
						m_nDifIndex++;
						continue;
					}
				}


	ValidLA :

				RgnInfo * pRootRgn1 = pLA->pRgn1->pRootRgn;
				RgnInfo * pRootRgn2 = pLA->pRgn2->pRootRgn;

				if( 0 == i )
				{
					if( NULL != pRootRgn1 && NULL != pRootRgn2 )
					{
						if( pRootRgn1 == pRootRgn2 )
							continue;

						int nDist = CalcRgnDif( pRootRgn1, pRootRgn2 );

						if( IsConflictDist(	nDist ) )
						{		
							int nStepThr = 0;
							
							if( pLA->pRgn1->nMaxDistMet >= nStepThr ||
								pLA->pRgn2->nMaxDistMet >= nStepThr )
							{
								//CreateConflict( pActRgn1, pActRgn2 );
								CreateConflict( pRootRgn1, pRootRgn2 );
								continue;
							}
						}
						else // if( 0 == i )
						{
							// No Conflict;

							interRootQue.PushPtr( pLA );

							// Rev
							continue;
						}


					}
					//else if( NULL == pRootRgn1 && NULL == pRootRgn2 )
					else if( NULL == pRootRgn1 )
					{
						// Bug
					}
					else		//	NULL == pRootRgn2
					{
						if( pLA->pRgn2->bIsN2E && 
							(GetMaxDif() - 1) != m_nDifIndex )
							m_nDifIndex = m_nDifIndex;

						if( pLA->pRgn2->bIsE2E && 
							(GetMaxDif()) != m_nDifIndex )
							m_nDifIndex = m_nDifIndex;

						pLA->pRgn2->pRootRgn = pRootRgn1;

						int nDist = m_nDifIndex;
						if( nDist > pLA->pRgn1->nMaxDistMet )
							pLA->pRgn2->nMaxDistMet = nDist;
						
						PrepareRgnLinkActions( pLA->pRgn2, m_nDifIndex );
						continue;
					}

				}
				else	//	i > 0
				{
					UpdateActRgn( pLA->pRgn1 );
					RgnInfo * pActRgn1 = pLA->pRgn1->GetActRgn();

					UpdateActRgn( pLA->pRgn2 );
					RgnInfo * pActRgn2 = pLA->pRgn2->GetActRgn();

					if( pActRgn1 == pActRgn2 )	
						continue;

		/*			if( 0 == pActRgn2->conflictList.GetSize() && ! bFromTmp )
					{
						tmpQue.PushPtr( pLA );
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

					int nTmpCnfCnt = pMaxSizRgn->conflictList.GetSize();
					//int nTmpCnfCnt = pMinSizRgn->conflictList.GetSize();

					if( nTmpCnfCnt > nMaxConfCnt )
						nMaxConfCnt = nTmpCnfCnt;

					nConfCntCnt ++;

					nConfCntSumMax += pMaxSizRgn->conflictList.GetSize();
					nConfCntSumMin += pMinSizRgn->conflictList.GetSize();

					if( 0 == pMinSizRgn->conflictList.GetSize() )
						nZeroConfCnt++;

					
					if( HaveConflict( pMinSizRgn, pMaxSizRgn ) )
					{
						continue;
					}
					else
					{
		NoConflict:

						if( 0 != pMinSizRgn->conflictList.GetSize() )
							RemoveDuplicateConflicts( pMaxSizRgn );

						if( pActRgn1 < pActRgn2 )	
						{
							pActRgn1->conflictList.TakeListElements(
								pActRgn2->conflictList );
							pActRgn2->SetActRgn( pActRgn1 );
						}
						else
						{
							pActRgn2->conflictList.TakeListElements(
								pActRgn1->conflictList );
							pActRgn1->SetActRgn( pActRgn2 );
						}

						if( NULL == pRootRgn1 )
						{
							//	Bug
							pRootRgn1 = pRootRgn1;
							//pLA->pRgn1->pRootRgn = pRootRgn2;
							
						}				
						else if( NULL == pRootRgn2 )
						{
							//	Bug
							pRootRgn1 = pRootRgn1;
							//pLA->pRgn2->pRootRgn = pRootRgn1;
						}
						

						PrepareRgnLinkActions( pLA->pRgn2, m_nDifIndex );
					}
				}
			}	//	while



			LinkAction * pLA = interRootQue.PopPtr();

			while( NULL != pLA )
			{
				m_difQues.PushPtr(0, pLA);

				pLA = interRootQue.PopPtr();
			}

			m_nDifIndex = 0;
			m_nDifIndexLim = 1;
		}

		for(int i=0; i < m_rgnInfoVect.size(); i++)
			UpdateActRgn( &m_rgnInfoVect[i] );

		// 81, 271
		//RgnInfo * pRgn81 = GetPointRgn(81, 271);
	}

	int RegionSegmentor15::CalcRgnDif(RgnInfo * a_pRgn1, RgnInfo * a_pRgn2)
	{
		int nDist = (int) sqrt( (double) (
			0.51 * Sqr( a_pRgn1->nValCh0 - a_pRgn2->nValCh0 ) +
			1.44 * Sqr( a_pRgn1->nValCh1 - a_pRgn2->nValCh1 ) +
			1.048 * Sqr( a_pRgn1->nValCh2 - a_pRgn2->nValCh2 ) ) );

		return nDist;
	}

	// With Space
	int RegionSegmentor15::CalcRgnDifWS(RgnInfo * a_pRgn1, RgnInfo * a_pRgn2)
	{
		double spaceFact = 1;

		double dx = a_pRgn1->x - a_pRgn2->x;
		double dy = a_pRgn1->y - a_pRgn2->y;

		dx *= spaceFact;
		dy *= spaceFact;

/*		double distSpace = sqrt( (double) (         
			Sqr( dx ) + Sqr( dy ) ) );

		if( distSpace > 15 )
			return GetMaxDif() - 1;*/

		int nDist = (int) sqrt( (double) (
			0.51 * Sqr( a_pRgn1->nValCh0 - a_pRgn2->nValCh0 ) +
			1.44 * Sqr( a_pRgn1->nValCh1 - a_pRgn2->nValCh1 ) +
			1.048 * Sqr( a_pRgn1->nValCh2 - a_pRgn2->nValCh2 )
			+ Sqr( dx ) + Sqr( dy ) ) );
			 //) );

		return nDist;
	}


	void RegionSegmentor15::UpdateActRgn(RgnInfo * a_pRgn)
	{
		static std::vector<RgnInfo *> rgnVect(10000);
		static int nMaxNofRgns = rgnVect.size();

		RgnInfo * pActRgn;
		do
		{
			pActRgn = a_pRgn ;
			int nofRgns = 0;

//			while(pActRgn != pActRgn->GetActRgn() && nofRgns < nMaxNofRgns)
			while(pActRgn != pActRgn->GetActRgn())
			{
				rgnVect[nofRgns++] = pActRgn;
				pActRgn = pActRgn->GetActRgn();
			}

			for(int i=0; i<nofRgns; i++)
				rgnVect[i]->SetActRgn( pActRgn );

		}while( pActRgn != pActRgn->GetActRgn() );
	}

	S16ImageRef RegionSegmentor15::GenSegmentedImage(void)
	{
		S16ImageRef ret = S16Image::Create(m_src->GetSize(), 3);

		for(int i=0; i < m_rgnInfoVect.size(); i++)
		{
			RgnInfo * pRgn = &m_rgnInfoVect[i];
			RgnInfo * pActRgn = pRgn->GetActRgn();

			pActRgn->nMeanValCh0 += pRgn->nValCh0;
			pActRgn->nMeanValCh1 += pRgn->nValCh1;
			pActRgn->nMeanValCh2 += pRgn->nValCh2;

			pActRgn->nPixCnt++;
		}

		for(int i=0; i < m_rgnInfoVect.size(); i++)
		{
			RgnInfo * pRgn = &m_rgnInfoVect[i];
			RgnInfo * pActRgn = pRgn->GetActRgn();

			if( pActRgn == pRgn )
			{
/*				pRgn->nMeanValCh0 /= pRgn->nPixCnt;
				pRgn->nMeanValCh1 /= pRgn->nPixCnt;
				pRgn->nMeanValCh2 /= pRgn->nPixCnt;*/


				pRgn->nMeanValCh0 = rand() % 256;
				pRgn->nMeanValCh1 = rand() % 256;
				pRgn->nMeanValCh2 = rand() % 256;
			}
		}

		S16ChannelRef retCh0 = ret->GetAt(0);
		S16ChannelRef retCh1 = ret->GetAt(1);
		S16ChannelRef retCh2 = ret->GetAt(2);

		for(int i=0; i < m_rgnInfoVect.size(); i++)
		{
			RgnInfo * pRgn = &m_rgnInfoVect[i];
			RgnInfo * pActRgn = pRgn->GetActRgn();

				

			/*if( pRgn->bIsEdge )
			{
				retCh0->SetAt( pRgn->x, pRgn->y, 255 );
				retCh1->SetAt( pRgn->x, pRgn->y, 255 );
				retCh2->SetAt( pRgn->x, pRgn->y, 255 );
			}
			else*/

			/*if( pRgn->bIsN2E )
			{
				retCh0->SetAt( pRgn->x, pRgn->y, 0 );
				retCh1->SetAt( pRgn->x, pRgn->y, 255 );
				retCh2->SetAt( pRgn->x, pRgn->y, 255 );
			}
			else if( pRgn->bIsE2E )
			{
				retCh0->SetAt( pRgn->x, pRgn->y, 0 );
				retCh1->SetAt( pRgn->x, pRgn->y, 255 );
				retCh2->SetAt( pRgn->x, pRgn->y, 255 );
			}
			else*/
			{
				retCh0->SetAt( pRgn->x, pRgn->y, (short)pActRgn->nMeanValCh0 );
				retCh1->SetAt( pRgn->x, pRgn->y, (short)pActRgn->nMeanValCh1 );
				retCh2->SetAt( pRgn->x, pRgn->y, (short)pActRgn->nMeanValCh2 );
			}
		}

		return ret;
	}

/*
	void RegionSegmentor15::PrepareConflicts()
	{
		//m_rgnConflictVect.resize( m_nofConflicts );
		m_rgnConflictVect.resize( 
			//m_nofPrimConfRgns * m_confCheckPtMgr.GetNofCheckPoints() * 2 );
			10000000);
		
		m_nofConflicts = 0;

		RgnInfo * pRgn = m_RgnOfConflictQue.PopPtr();

		while( NULL != pRgn )
		{
			PrepareRgnConflicts( pRgn );

			pRgn = m_RgnOfConflictQue.PopPtr();

//			if( NULL == pRgn->GetActRgn() )
//				pRgn = pRgn;

		}	//	while
	}
	*/

	void RegionSegmentor15::PrepareLinkAction( RgnInfo * a_pRgn, RgnInfo * a_pRgn2,
			RgnLink & a_rLink, RgnLink & a_rLink2, int a_nDistBef)
	{
		LinkAction * pLA = &m_linkActionVect[ m_nofLinkActions++ ];

		pLA->pRgn1 = a_pRgn;
		pLA->pRgn2 = a_pRgn2;
		pLA->pPrev = pLA;


		//int nDistNew = a_rLink.nDist;
		//int nDistNew = a_rLink.nDist + a_nDistBef;
		//int nDistNew = CalcRgnDif( a_pRgn->pRootRgn, a_pRgn2 );
		//int nDistNew = CalcRgnDifWS( a_pRgn->pRootRgn, a_pRgn2 );
		//nDistNew += a_nDistBef + 1;
		//nDistNew += a_nDistBef;

		int nDistNew;

		if( a_pRgn2->bIsEdge )
		{
			if( a_pRgn->bIsEdge )
			{
				nDistNew = GetMaxDif();

				if( false == a_pRgn2->bIsN2E )
					a_pRgn2->bIsE2E = true;
			}
			else
			{
				//nDistNew = CalcRgnDif( a_pRgn->pRootRgn, a_pRgn2 );
				//nDistNew = 0;
				nDistNew = GetMaxDif() - 1;
				//nDistNew = 5000;

				a_pRgn2->bIsN2E = true;
			}
		}
		else
		{		
			nDistNew = CalcRgnDif( a_pRgn->pRootRgn, a_pRgn2 );
			//nDistNew = CalcRgnDifWS( a_pRgn->pRootRgn, a_pRgn2 );

			if( nDistNew >= ( GetMaxDif() - 1 ) )
				nDistNew = nDistNew;
		}






		/*int nDistNew = (int) ( 10 *
			sqrt( (double)
				( 
					Sqr( a_pRgn->pRootRgn->x - a_pRgn2->x ) +
					Sqr( a_pRgn->pRootRgn->y - a_pRgn2->y ) )
					)
					);
					*/


		a_rLink.nDist = a_rLink2.nDist = nDistNew;
		
		if( nDistNew < m_nDifIndex )
			m_nDifIndex = nDistNew;


		//m_difQues.PushPtr( a_rLink.nDist, pLA);
		m_difQues.PushPtr( nDistNew, pLA);

		

		if( nDistNew > (m_nDifIndexLim - 1) )						
			m_nDifIndexLim = nDistNew + 1;

		a_rLink.bProcessed = a_rLink2.bProcessed = true;
	}

	bool RegionSegmentor15::HaveConflict( RgnInfo * a_pMinSizRgn, RgnInfo * a_pMaxSizRgn)
	{
		bool bRet = false;

		APtrList< RgnConflict > & minConfList = 
			a_pMinSizRgn->conflictList;

		RgnConflict * pConflict = minConfList.Last();

		while( NULL != pConflict )
		{
			RgnInfo * pPeerRgn = pConflict->pPeerRgn;

			UpdateActRgn( pPeerRgn );
			pPeerRgn = pPeerRgn->GetActRgn();
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

	void RegionSegmentor15::RemoveDuplicateConflicts( RgnInfo * a_pRgn )
	{
		APtrList< RgnConflict > & confList = 
			a_pRgn->conflictList;

		RgnConflict * pConflict = confList.Last();

		while( NULL != pConflict )
		{
			RgnInfo * pPeerRgn = pConflict->pPeerRgn;

			UpdateActRgn( pPeerRgn );
			pPeerRgn = pPeerRgn->GetActRgn();
 			pConflict->pPeerRgn = pPeerRgn;


			RgnConflict * pNext = confList.Next();
			
			if( pPeerRgn->nLastVisitID == m_nVisitID )
			{
				confList.RemovePtr( pConflict );
			}

			pPeerRgn->nLastVisitID = m_nVisitID;

			pConflict = pNext;
		}
	}


	void RegionSegmentor15::PrepareRgnLinkActions( RgnInfo * a_pRgn, int a_nDistBef )
	{
		RgnLink * links = a_pRgn->links;

		for(int i=0; i<4; i++)
		{
			RgnLink & rLink = links[i];

			if( rLink.bProcessed )
				continue;

			RgnInfo * pRgn2 = rLink.pPeerRgn;
			RgnLink & rLink2 = pRgn2->links[ i + 4 ];

			PrepareLinkAction( a_pRgn, pRgn2, rLink, rLink2, a_nDistBef );
		}

		for(int i=4; i<8; i++)
		{
			RgnLink & rLink = links[i];

			if( rLink.bProcessed )
				continue;

			RgnInfo * pRgn2 = rLink.pPeerRgn;
			RgnLink & rLink2 = pRgn2->links[ i - 4 ];

			PrepareLinkAction( a_pRgn, pRgn2, rLink, rLink2, a_nDistBef );
		}

	}

	S16ImageRef RegionSegmentor15::GenConflictImg(bool a_bRC, bool a_bRB,
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


	void RegionSegmentor15::CreateConflict( RgnInfo * a_pRgn1, RgnInfo * a_pRgn2 )
	{
		RgnConflict * pRc = &m_rgnConflictVect[ m_nofConflicts++ ];
		pRc->pNext = pRc;
		pRc->pPrev = pRc;
		pRc->pPeerRgn = a_pRgn2;
		a_pRgn1->conflictList.PushLast( pRc );

		RgnConflict * pRc2 = &m_rgnConflictVect[ m_nofConflicts++ ];
		pRc2->pNext = pRc2;
		pRc2->pPrev = pRc2;
		pRc2->pPeerRgn = a_pRgn1;
		a_pRgn2->conflictList.PushLast( pRc2 );
	}

}