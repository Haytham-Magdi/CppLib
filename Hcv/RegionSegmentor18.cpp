#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\RegionSegmentor18.h>

namespace Hcv
{
	using namespace Hcpl;
	using namespace Hcpl::Math;

	//static MgrOfLinkTypes::CoreStuff m_core;
	//RegionSegmentor18::MgrOfLinkTypes::CoreStuff m_core;

	RegionSegmentor18::RegionSegmentor18(F32ImageRef a_src, S16ImageRef a_rootImg,
		int a_nDifThreshold, F32ImageRef a_rootValImg) : 
	m_rgnIndexCalc(a_src->GetWidth(), a_src->GetHeight()) 
	{	
		m_difThreshold = a_nDifThreshold;
		m_nofConflicts = 0;
		m_nofPrimConfRgns = 0;
		m_nofLinkActions = 0;
		m_nVisitID = 0;

		m_orgSrc = a_src;

		{
			F32ColorVal colorFact;

			colorFact.val0 = 0.51 * 0.5;
			colorFact.val1 = 1.44 * 0.5;
			colorFact.val2 = 1.048 * 0.5;

			//m_orgSrc = GenMultByColorImg( m_orgSrc, colorFact);
			m_src = m_orgSrc;
		}


		m_rootImg = a_rootImg;

		if( NULL == a_rootValImg )
			a_rootValImg = m_src;
		m_rootValImg = a_rootValImg;

		InitMaxDif();
		//m_difQues.Init( GetMaxDif() + 1 );
		//m_difQues.Init( GetMaxDif() * 1000 + 1 );
		//m_difQues.Init( GetMaxDif() + 2 );
		m_linkActionMergeQues.InitSize( GetMaxDif() + 2 );
		m_linkActionEdgeQues.InitSize( GetMaxDif() + 2 );

		InitRgnInfoVect();
		InitLinks();		


		//RgnInfo * pRgn81 = GetPointRgn(81, 271);

		m_rgnConflictVect.resize( 
			10000000);
		
		m_nofConflicts = 0;

		ProcessEdgeQues();
	}

	void RegionSegmentor18::InitRgnInfoVect(void)
	{
		CvSize srcSiz = m_src->GetSize();

		m_rgnInfoVect.resize(srcSiz.width * srcSiz.height);

/*		F32ChannelRef srcCh0 = m_src->GetAt(0);
		F32ChannelRef srcCh1 = m_src->GetAt(1);
		F32ChannelRef srcCh2 = m_src->GetAt(2);

		F32ChannelRef rvCh0 = m_rootValImg->GetAt(0);
		F32ChannelRef rvCh1 = m_rootValImg->GetAt(1);
		F32ChannelRef rvCh2 = m_rootValImg->GetAt(2);*/

		S16ChannelRef rootCh0 = m_rootImg->GetAt(0);



		for(int y=0; y < srcSiz.height; y++)
		{
			for(int x=0; x < srcSiz.width; x++)
			{	
				RgnInfo * pRgnInfo = GetPointRgn(x, y);

				pRgnInfo->pPrev = pRgnInfo;

				

				{
					int nRootImgVal = rootCh0->GetAt(x, y);

/*					if(nRootImgVal > 1)
					{
						pRgnInfo->BeARoot();
						pRgnInfo->SetActRgn( pRgnInfo );
						

						pRgnInfo->pixColors = 
							(F32ColorVal *)m_rootValImg->GetPixAt(x, y);
					}
					else*/
					{
						//pRgnInfo->BeNotRoot();

						pRgnInfo->pixColors = 
							(F32ColorVal *)m_src->GetPixAt(x, y);
					}					

				}


				//pRgnInfo->bIsInConflict = false;

				pRgnInfo->nMaxDistMet = 0;

				pRgnInfo->meanColors.val0 = 0;
				pRgnInfo->meanColors.val1 = 0;
				pRgnInfo->meanColors.val2 = 0;

				pRgnInfo->nPixCnt = 0;

				pRgnInfo->pos.x = x;
				pRgnInfo->pos.y = y;

				pRgnInfo->nLastVisitID = 0;

				RgnLink * links = pRgnInfo->links;
				for(int i=0; i<8; i++)
				{
					links[i].bProcessed = true;
					links[i].bExists = false;					
					links[i].dir = (RgnLinkDir)i;
				}

				pRgnInfo->bHasConflicts = false;
			}
		}

	}

	inline void RegionSegmentor18::CreateLink( RgnInfo * a_pRgn, 
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

		F32ColorVal colorDist1 = F32ColorVal::Sub(
			*pRgn2->pixColors, *a_pRgn->pixColors);

		float dist = colorDist1.CalcMag();

		rLink.UnitDist = colorDist1.DividBy(dist);
		rLinkR2.UnitDist = colorDist1.DividBy(-dist);

		if( 0 != dx[nLinkIndex] && 0 != dy[nLinkIndex] )
			//dist = dist;
			dist /= 1.4;
			//dist *= 1.4;

		dist *= m_difScale;

		if(dist > 1)
			dist = dist;


		// For a Diagonal Link

		// hthm
		//dist = -1;

		rLink.DistMag = rLinkR2.DistMag = dist;

		//rLink.DistMag = rLinkR2.DistMag = -1;

/*
		if( a_pRgn->pos.x == 249 &&
			a_pRgn->pos.y == 11 )
			a_pRgn = a_pRgn;

		if( pRgn2->pos.x == 249 &&
			pRgn2->pos.y == 11 )
			pRgn2 = pRgn2;
*/




		//if( IsConflictDist( dist * 2 ) )
		if( dist > ( 10 * m_difScale )  )
		//if( dist > ( 35 * m_difScale )  )
		//if( dist > 0  )
		{
			LinkAction * pLA = &m_linkActionVect[ m_nofLinkActions++ ];

			pLA->pRgn1 = a_pRgn;
			pLA->pRgn2 = pRgn2;
			pLA->pLink1 = &rLink;
			pLA->pLink2 = &rLinkR2;
			pLA->pPrev = pLA;

			if( (15469 + 1 ) == m_nofLinkActions )
				this->pLADbg = pLA;

			m_linkActionEdgeQues.PushPtr( (int)dist, pLA);

			//a_pRgn->BeARoot();
			//pRgn2->BeARoot();
		}

/*
		if( a_pRgn->IsRoot() )
		{			
			PrepareLinkAction( a_pRgn, pRgn2, rLink, rLinkR2, 0);
		}
		else if( pRgn2->IsRoot() )
		{
			PrepareLinkAction( pRgn2, a_pRgn, rLinkR2, rLink, 0);
		}
*/
	} 




	void RegionSegmentor18::InitLinks(void)
	{
		const int nSrcWidth = m_src->GetWidth();
		const int nSrcHeight = m_src->GetHeight();

		m_linkActionVect.resize(
			2 * (
			(4 * nSrcWidth * nSrcHeight) -
			nSrcHeight -	//	RC
			2 * (nSrcHeight + nSrcWidth - 1) -	//	RB + LB
			nSrcWidth	//	CB
			)
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

	S16ImageRef RegionSegmentor18::GenDifImg(bool a_bRC, bool a_bRB,
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
						retCh0->SetAt( pRgn->pos.x, pRgn->pos.y, 255);
					}
				}
			}
		}

		return ret;
	}



	
	HistoGramRef RegionSegmentor18::GenDifHisto(bool a_bRC, bool a_bRB,
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

	
	
	
	
	void RegionSegmentor18::Segment()
	{
		int cnt1 = 0;
		int cnt2 = 0;


		int nMaxConfCnt = 0;

		int nConfCntSumMax = 0;
		int nConfCntSumMin = 0;

		int nConfCntCnt = 0;
		int nZeroConfCnt = 0;

		ListQue< LinkAction > tmpQue;

		//MultiListQue< LinkAction > m_difQues;
		
		MultiListQue< LinkAction > interRootQues;
		
		interRootQues.Init( m_linkActionMergeQues.GetNofQues() );

		//ListQue< LinkAction > interRootQue;

		for(int i=0; i < 2; i++)
		//for(int i=0; i < 1; i++)
		{
			LinkAction * pLA = NULL;

			do
			{
				pLA = m_linkActionMergeQues.PopPtrMin();

				if( NULL == pLA )
				{
					continue;
				}


	ValidLA :

				RgnInfo * pRootRgn1 = pLA->pRgn1->GetRootRgn();
				RgnInfo * pRootRgn2 = pLA->pRgn2->GetRootRgn();

				if( 0 == i )
				{
					if( NULL != pRootRgn1 && NULL != pRootRgn2 )
					{
						if( pRootRgn1 == pRootRgn2 )
							continue;

						cnt1++;

						float dist = CalcRgnDif( pRootRgn1, pRootRgn2 );

/*						if( IsConflictDist(	dist ) )
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
						else // if( 0 == i )*/
						{
							// No Conflict;


							interRootQues.PushPtr(dist, pLA );
							//interRootQues.PushPtr(0, pLA );

							// Rev
							continue;
						}


					}
					//else if( NULL == pRootRgn1 && NULL == pRootRgn2 )
					else if( NULL == pRootRgn1 )
					{
						pRootRgn1 = pRootRgn1;
						// Bug
					}
					else		//	NULL == pRootRgn2
					{
						cnt2++;

						pLA->pRgn2->SetRootRgn( pRootRgn1 );

						float dist = m_linkActionMergeQues.GetLastMinIndex();
						if( dist > pLA->pRgn1->nMaxDistMet )
							pLA->pRgn2->nMaxDistMet = dist;
						
						PrepareRgnLinkActions( pLA->pRgn2, dist );
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
					//if(true)
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
						

						PrepareRgnLinkActions( pLA->pRgn2, 
							m_linkActionMergeQues.GetLastMinIndex() );
					}
				}
			}while( NULL != pLA );


			for(int k=0; k<interRootQues.GetNofQues(); k++)
			{
				//LinkAction * pLA = interRootQues.PopPtr();
				LinkAction * pLA = interRootQues.PopPtr(k);

				while( NULL != pLA )
				{
					//m_difQues.PushPtr(0, pLA);
					m_linkActionMergeQues.PushPtr(k, pLA);

					//pLA = interRootQues.PopPtr();
					pLA = interRootQues.PopPtr(k);
				}
			}
			
		}

		cnt1 = 0;
		cnt2 = 0;

		for(int i=0; i < m_rgnInfoVect.size(); i++)
		{
			RgnInfo * pRgn = &m_rgnInfoVect[i];
			UpdateActRgn( pRgn );

			if( pRgn == pRgn->GetActRgn() )
			{
				cnt1++;
			}
			else
			{
				cnt2++;
			}

		}

		// 81, 271
		//RgnInfo * pRgn81 = GetPointRgn(81, 271);
	}

	float RegionSegmentor18::CalcRgnDif(RgnInfo * a_pRgn1, RgnInfo * a_pRgn2)
	{
		float dist = m_difScale * F32ColorVal::Sub(
			*a_pRgn1->pixColors, *a_pRgn2->pixColors).CalcMag();

		return dist;
	}

	void RegionSegmentor18::UpdateActRgn(RgnInfo * a_pRgn)
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

	S16ImageRef RegionSegmentor18::GenSegmentedImage(void)
	{
		S16ImageRef ret = S16Image::Create(m_src->GetSize(), 3);

		for(int i=0; i < m_rgnInfoVect.size(); i++)
		{
			RgnInfo * pRgn = &m_rgnInfoVect[i];
			RgnInfo * pActRgn = pRgn->GetActRgn();

			pActRgn->meanColors.IncBy( *pActRgn->pixColors );

			pActRgn->nPixCnt++;
		}

		for(int i=0; i < m_rgnInfoVect.size(); i++)
		{
			RgnInfo * pRgn = &m_rgnInfoVect[i];
			RgnInfo * pActRgn = pRgn->GetActRgn();

			if( pActRgn == pRgn )
			{
				//pRgn->meanColors.DividSelfBy( pRgn->nPixCnt );

				pRgn->meanColors.val0 = rand() % 256;
				pRgn->meanColors.val1 = rand() % 256;
				pRgn->meanColors.val2 = rand() % 256;
			}
		}

		S16ChannelRef retCh0 = ret->GetAt(0);
		S16ChannelRef retCh1 = ret->GetAt(1);
		S16ChannelRef retCh2 = ret->GetAt(2);

		for(int i=0; i < m_rgnInfoVect.size(); i++)
		{
			RgnInfo * pRgn = &m_rgnInfoVect[i];
			RgnInfo * pActRgn = pRgn->GetActRgn();
																			
				

			{
				//if(pRgn->bShowLocalColor)
				if(false) 				{
					/*
					retCh0->SetAt( pRgn->pos.x, pRgn->pos.y, (short)pRgn->pixColors->val0 );
					retCh1->SetAt( pRgn->pos.x, pRgn->pos.y, (short)pRgn->pixColors->val1 );
					retCh2->SetAt( pRgn->pos.x, pRgn->pos.y, (short)pRgn->pixColors->val2 );
					*/

					retCh0->SetAt( pRgn->pos.x, pRgn->pos.y, (short)pRgn->showColor.val0 );
					retCh1->SetAt( pRgn->pos.x, pRgn->pos.y, (short)pRgn->showColor.val1 );
					retCh2->SetAt( pRgn->pos.x, pRgn->pos.y, (short)pRgn->showColor.val2 );
					
				}
				else
				{
					retCh0->SetAt( pRgn->pos.x, pRgn->pos.y, (short)pActRgn->meanColors.val0 );
					retCh1->SetAt( pRgn->pos.x, pRgn->pos.y, (short)pActRgn->meanColors.val1 );
					retCh2->SetAt( pRgn->pos.x, pRgn->pos.y, (short)pActRgn->meanColors.val2 );
				}

			}
		}

		return ret;
	}

/*
	void RegionSegmentor18::PrepareConflicts()
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



	void RegionSegmentor18::PrepareLinkAction( RgnInfo * a_pRgn, RgnInfo * a_pRgn2,
			RgnLink & a_rLink, RgnLink & a_rLink2, float a_distBef)
	{
		LinkAction * pLA = &m_linkActionVect[ m_nofLinkActions++ ];

		pLA->pRgn1 = a_pRgn;
		pLA->pRgn2 = a_pRgn2;
		pLA->pLink1 = &a_rLink;
		pLA->pLink2 = &a_rLink2;
		pLA->pPrev = pLA;


		//float distNew = a_rLink.DistMag;
		//float distNew = a_rLink.DistMag + a_distBef;
		//float distNew = CalcRgnDif( a_pRgn->pRootRgn, a_pRgn2 );
		//distNew += a_distBef + 1;
		//distNew += a_distBef;

		float distNew;

		{		
			distNew = CalcRgnDif( a_pRgn->GetRootRgn(), a_pRgn2 );
			//distNew = a_rLink.DistMag;

			//if( distNew >= ( GetMaxDif() - 1 ) )
			//	distNew = distNew;
		}



		//a_rLink.DistMag = a_rLink2.DistMag = distNew;
		

		//m_difQues.PushPtr( a_rLink.DistMag, pLA);
		m_linkActionMergeQues.PushPtr( distNew, pLA);
		
		

		a_rLink.bProcessed = a_rLink2.bProcessed = true;
	}

	bool RegionSegmentor18::HaveConflict( RgnInfo * a_pMinSizRgn, RgnInfo * a_pMaxSizRgn)
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

	void RegionSegmentor18::RemoveDuplicateConflicts( RgnInfo * a_pRgn )
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


	void RegionSegmentor18::PrepareRgnLinkActions( RgnInfo * a_pRgn, float a_distBef )
	{
		RgnLink * links = a_pRgn->links;

		for(int i=0; i<4; i++)
		{
			RgnLink & rLink = links[i];

			if( rLink.bProcessed )
				continue;

			RgnInfo * pRgn2 = rLink.pPeerRgn;
			RgnLink & rLink2 = pRgn2->links[ i + 4 ];

			PrepareLinkAction( a_pRgn, pRgn2, rLink, rLink2, a_distBef );
		}

		for(int i=4; i<8; i++)
		{
			RgnLink & rLink = links[i];

			if( rLink.bProcessed )
				continue;

			RgnInfo * pRgn2 = rLink.pPeerRgn;
			RgnLink & rLink2 = pRgn2->links[ i - 4 ];

			PrepareLinkAction( a_pRgn, pRgn2, rLink, rLink2, a_distBef );
		}

	}

	S16ImageRef RegionSegmentor18::GenConflictImg(bool a_bRC, bool a_bRB,
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
					if( IsConflictDist( links[j].DistMag ) )
					{
						retCh0->SetAt( pRgn->pos.x, pRgn->pos.y, 255);
					}
				}
			}
		}

		return ret;
	}


	void RegionSegmentor18::CreateConflict( RgnInfo * a_pRgn1, RgnInfo * a_pRgn2 )
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


	void RegionSegmentor18::ProcessEdgeQues()
	{
			LinkAction * pLA = NULL;

			do
			{
				pLA = m_linkActionEdgeQues.PopPtrMax(); 

				if( NULL == pLA )
				{
					continue;
				}

				//if( pLA == pLADbg )
				//	pLA = pLA;

/*				
				if( pLA->pRgn2->pos.x == 249 &&
					pLA->pRgn2->pos.y == 11 )
					pLA->pRgn2 = pLA->pRgn2;
*/


				RgnInfo * pRoot1 = NULL;
				RgnInfo * pRoot2 = NULL;

				TraceLinkActionRoots( pLA, &pRoot1, &pRoot2 );

				float dist = CalcRgnDif( pRoot1, pRoot2 );
					//pLA->pLink1->DistMag;

				//if( IsConflictDist( dist * 2 ) )
				if( IsConflictDist( dist ) )
				//if( dist >= 20 )
				{
					pRoot1->BeARoot();					
					pRoot2->BeARoot();

					CreateConflict( pRoot1, pRoot2 );

					PrepareRgnLinkActions(pRoot1, 0);
					PrepareRgnLinkActions(pRoot2, 0);
				}

			}while( NULL != pLA );

	}

	void RegionSegmentor18::TraceLinkActionRoots( LinkAction * a_pLA,
		RgnInfo ** a_ppRoot1, RgnInfo ** a_ppRoot2 )
	{
		RgnInfo * pFwdRgn = a_pLA->pRgn1;
		RgnInfo * pBkdRgn = a_pLA->pRgn2;

		const bool bShowTrace = ((rand() % 100) == 0);

		int fwdLinkIndex = (int)a_pLA->pLink1->dir;
		int bkdLinkIndex = MgrOfLinkTypes::GetCore()->GetInverseLinkIndex(
			fwdLinkIndex);

		RgnInfo * pNextFwdRgn = NULL;
		RgnInfo * pNextBkdRgn = NULL;

		int nextFwdLinkIndex = -1;
		int nextBkdLinkIndex = -1;

		F32ColorVal unitColorDist = a_pLA->pLink1->UnitDist;

		bool bFwdFinished = false;
		bool bBkdFinished = false;

		F32ColorVal traceColor = {0, 255, 0};
		//F32ColorVal traceColor = {0, 0, 255};

		

		do
		{
			if(bShowTrace)
			{
				pFwdRgn->bShowLocalColor = true;
				pFwdRgn->showColor = traceColor;

				pBkdRgn->bShowLocalColor = true;
				pBkdRgn->showColor = traceColor;
			}

			if( bFwdFinished != true)
			{
				float minUcdDif = 1000000;
				int minLinkIndex = fwdLinkIndex;

				for(int i=0; i<3; i++)
				{
					nextFwdLinkIndex = MgrOfLinkTypes::GetCore()->GetNbr(
						fwdLinkIndex, i);

					RgnLink & rLink = pFwdRgn->links[nextFwdLinkIndex];

					if(rLink.bExists == false)
						continue;

					pNextFwdRgn = rLink.pPeerRgn;

					F32ColorVal nextUcd = F32ColorVal::Sub(
						*pNextFwdRgn->pixColors, *pFwdRgn->pixColors);

					nextUcd.DividSelfBy( nextUcd.CalcMag() );

					float ucdDif = F32ColorVal::Sub(
						unitColorDist, nextUcd).CalcMag();

					if( ucdDif < minUcdDif )
					{
						minUcdDif = ucdDif;
						minLinkIndex = nextFwdLinkIndex;
					}
				}

				if(minUcdDif > 0.5)
				{
					bFwdFinished = true;
				}
				else
				{
					//nextFwdLinkIndex = minLinkIndex;
					//pNextFwdRgn = pFwdRgn->links[nextFwdLinkIndex].pPeerRgn;

					fwdLinkIndex = minLinkIndex;
					pFwdRgn = pFwdRgn->links[minLinkIndex].pPeerRgn;
				}
			}



			if( bBkdFinished != true)
			{
				float minUcdDif = 1000000;
				int minLinkIndex = bkdLinkIndex;

				for(int i=0; i<3; i++)
				{
					nextBkdLinkIndex = MgrOfLinkTypes::GetCore()->GetNbr(
						bkdLinkIndex, i);

					RgnLink & rLink = pBkdRgn->links[nextBkdLinkIndex];

					if(rLink.bExists == false)
						continue;

					pNextBkdRgn = rLink.pPeerRgn;

					F32ColorVal nextUcd = F32ColorVal::Sub(
						*pBkdRgn->pixColors, *pNextBkdRgn->pixColors);

					nextUcd.DividSelfBy( nextUcd.CalcMag() );

					float ucdDif = F32ColorVal::Sub(
						unitColorDist, nextUcd).CalcMag();

					if( ucdDif < minUcdDif )
					{
						minUcdDif = ucdDif;
						minLinkIndex = nextBkdLinkIndex;
					}
				}

				if(minUcdDif > 0.5)
				{
					bBkdFinished = true;
				}
				else
				{
					//nextBkdLinkIndex = minLinkIndex;
					//pNextBkdRgn = pBkdRgn->links[nextBkdLinkIndex].pPeerRgn;

					bkdLinkIndex = minLinkIndex;
					pBkdRgn = pBkdRgn->links[minLinkIndex].pPeerRgn;
				}
			}

			if( bFwdFinished == true && bBkdFinished == true )
			{
				break;
			}
			else
			{
				unitColorDist = F32ColorVal::Sub(
					*pFwdRgn->pixColors, *pBkdRgn->pixColors);

				unitColorDist.DividSelfBy( unitColorDist.CalcMag() );
			}

		//}while( bFwdFinished == false && bBkdFinished == false );
		}while( true );



		if(bShowTrace)
		{
		F32ColorVal traceLimColor = { 0, 0, 255 };

		pFwdRgn->bShowLocalColor = true;
			//pFwdRgn->showColor = traceColor;
			pFwdRgn->showColor = traceLimColor;

			pBkdRgn->bShowLocalColor = true;
			//pBkdRgn->showColor = traceColor;
			pBkdRgn->showColor = traceLimColor;
		}


		*a_ppRoot1 = pFwdRgn;
		*a_ppRoot2 = pBkdRgn;
	}

//////////////////////////////////////////////////////////////


	void RegionSegmentor18::MgrOfLinkTypes::CoreStuff::InitLinkTypeMgrs()
	{
		static int dx[] = {1, 1, 0, -1,   -1, -1, 0, 1};
		static int dy[] = {0, 1, 1, 1,   0, -1, -1, -1};

		m_linkTypeMgrVect.resize(8);

		for(int i=0; i < m_linkTypeMgrVect.size(); i++)
		{
			F32Point point(dx[i], dy[i]);
						
			m_linkTypeMgrVect[i].Init( &m_linkTypeMgrVect[0], i, point );
		}

	}

	void RegionSegmentor18::MgrOfLinkTypes::CoreStuff::InitMaps()
	{
		m_linkTypeMgrVect.resize(8);

		{
			for(int i=0; i < m_linkTypeMgrVect.size(); i++)
			{
				F32Point uDirXY_i = m_linkTypeMgrVect[i].GetUnitDirXY();

				for(int j=0; j < m_linkTypeMgrVect.size(); j++)
				{
					F32Point uDirXY_j = m_linkTypeMgrVect[j].GetUnitDirXY();

					float dist = F32Point::Sub(uDirXY_i, uDirXY_j).CalcMag(); 
					//m_distMap[ m_mapIndexCalc.Calc(i, j) ] = dist;
					m_distMap[ m_mapIndexCalc.Calc(j, i) ] = dist;
				}
			}
		}

		{
			for(int i=0; i < m_linkTypeMgrVect.size(); i++)
			{				
				for(int j=0; j < m_linkTypeMgrVect.size(); j++)
				{					
					//m_nbrMap[ m_mapIndexCalc.Calc(i, j) ] = j;
					m_nbrMap[ m_mapIndexCalc.Calc(j, i) ] = j;
				}
			}
		}

		{
			for(int i=0; i < m_linkTypeMgrVect.size(); i++)
			{		
				for(int k=0; k < m_linkTypeMgrVect.size(); k++)
				{		
					int minIndex = k;
					//float distMinIndex = m_distMap[ m_mapIndexCalc.Calc(i, minIndex) ]; 
					float distMinIndex = m_distMap[ m_mapIndexCalc.Calc(
						//minIndex, 
						m_nbrMap[ m_mapIndexCalc.Calc(minIndex, i) ],
						i) ]; 

					for(int j=k+1; j < m_linkTypeMgrVect.size(); j++)
					{					
						//float distJ = m_distMap[ m_mapIndexCalc.Calc(i, j) ]; 
						float distJ = m_distMap[ m_mapIndexCalc.Calc(							
							//j, 
							m_nbrMap[ m_mapIndexCalc.Calc(j, i) ],
							i) ]; 

						if( distJ < distMinIndex )
						{
							distMinIndex = distJ;
							minIndex = j;
						}
					}

					// Swap
					//int tmp = m_nbrMap[ m_mapIndexCalc.Calc(i, k) ];
					int tmp = m_nbrMap[ m_mapIndexCalc.Calc(k, i) ];

					//m_nbrMap[ m_mapIndexCalc.Calc(i, k) ] = 
					m_nbrMap[ m_mapIndexCalc.Calc(k, i) ] = 
						//m_nbrMap[ m_mapIndexCalc.Calc(i, minIndex) ];
						m_nbrMap[ m_mapIndexCalc.Calc(minIndex, i) ];

					//m_nbrMap[ m_mapIndexCalc.Calc(i, minIndex) ] = tmp;
					m_nbrMap[ m_mapIndexCalc.Calc(minIndex, i) ] = tmp;
				}
			}
		}

	}
//////////////////////////////////////////////////////////////

}