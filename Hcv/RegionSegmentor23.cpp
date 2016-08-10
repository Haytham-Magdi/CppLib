#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\RegionSegmentor23.h>

namespace Hcv
{
	using namespace Hcpl;
	using namespace Hcpl::Math;

	//static MgrOfLinkTypes::CoreStuff m_core;
	//RegionSegmentor23::MgrOfLinkTypes::CoreStuff m_core;

	int RegionSegmentor23::RgnInfo::s_MergeOrder = 0;

	RegionSegmentor23::RegionSegmentor23(F32ImageRef a_src, S16ImageRef a_rootImg,
		int a_nDifThreshold, F32ImageRef a_rootValImg) : 
	m_rgnIndexCalc(a_src->GetWidth(), a_src->GetHeight()), m_nAprSize( GlobalStuff::AprSize1D )
	//m_rgnIndexCalc(a_src->GetWidth(), a_src->GetHeight()), m_nAprSize( 0 )
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

		m_medTraceQues.Init( m_linkActionMergeQues.GetNofQues() );


		m_edgeFwdRgnVect.SetCapacity(3000);

		m_edgeBkdRgnVect.SetCapacity(3000);
		
		InitRgnInfoVect();
		InitLinks();		


		//RgnInfo * pRgn81 = GetPointRgn(81, 271);

		m_rgnConflictVect.SetCapacity( 
			10000000);
		
		m_nofConflicts = 0;


		ScanImageLines();
	}

	void RegionSegmentor23::InitRgnInfoVect(void)
	{
		RgnInfo::s_MergeOrder = 0;

		CvSize srcSiz = m_src->GetSize();

		m_rgnInfoVect.SetSize(srcSiz.width * srcSiz.height);

		m_medTraceRgnPtrVect.SetCapacity( m_rgnInfoVect.GetSize() );

		//m_traceRootAllocVect.SetCapacity( m_rgnInfoVect.GetSize() );
		m_traceRootAllocVect.SetCapacity( 10000000 );

		m_edgeInfoVect.SetCapacity( m_rgnInfoVect.GetSize() / 2 );


/*		F32ChannelRef srcCh0 = m_src->GetAt(0);
		F32ChannelRef srcCh1 = m_src->GetAt(1);
		F32ChannelRef srcCh2 = m_src->GetAt(2);

		F32ChannelRef rvCh0 = m_rootValImg->GetAt(0);
		F32ChannelRef rvCh1 = m_rootValImg->GetAt(1);
		F32ChannelRef rvCh2 = m_rootValImg->GetAt(2);*/

		//S16ChannelRef rootCh0 = m_rootImg->GetAt(0);



		for(int y=0; y < srcSiz.height; y++)
		{
			for(int x=0; x < srcSiz.width; x++)
			{	
				RgnInfo * pRgnInfo = GetPointRgn(x, y);

				pRgnInfo->pPrev = pRgnInfo;

				



				pRgnInfo->pixColors = 
					(F32ColorVal *)m_src->GetPixAt(x, y);

				pRgnInfo->showColor = *pRgnInfo->pixColors;


				//pRgnInfo->bIsInConflict = false;

				pRgnInfo->accXYDistFromRoot = 0;
				pRgnInfo->maxAccXYDistFromRoot = -1;

				pRgnInfo->pTraceRootElm = NULL;

				pRgnInfo->pTraceFwdRoot = NULL;
				pRgnInfo->pTraceBkdRoot = NULL;

				pRgnInfo->nTraceID = -1;

				pRgnInfo->nMaxDistMet = 0;
				pRgnInfo->distMagFromRoot = 500 * m_difScale;

				//pRgnInfo->meanColors.val0 = 0;
				//pRgnInfo->meanColors.val1 = 0;
				//pRgnInfo->meanColors.val2 = 0;

				pRgnInfo->meanColors.val0 = pRgnInfo->pixColors->val0;
				pRgnInfo->meanColors.val1 = pRgnInfo->pixColors->val1;
				pRgnInfo->meanColors.val2 = pRgnInfo->pixColors->val2;
				

				//pRgnInfo->nPixCnt = 0;
				pRgnInfo->nPixCnt = 1;

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

	inline void RegionSegmentor23::CreateLink( RgnInfo * a_pRgn, 
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

/*
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
*/
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
		//if( dist > ( 1 * m_difScale )  )
		//if( dist > ( 4 * m_difScale )  )
		//if( dist > ( 3 * m_difScale )  ) 
//		if( dist > ( 8 * m_difScale )  ) 
		//if( dist > ( 10 * m_difScale )  ) 
		//if( dist > ( 35 * m_difScale )  )
		
		//if( dist > 0  )
		if( false  )
		{
			LinkAction * pLA = &m_linkActionVect[ m_nofLinkActions++ ];

			pLA->pRgn1 = a_pRgn;
			pLA->pRgn2 = pRgn2;
			pLA->pLink1 = &rLink;
			pLA->pLink2 = &rLinkR2;
			pLA->pPrev = pLA;

			//if( (15469 + 1 ) == m_nofLinkActions )
			//	this->pLADbg = pLA;

//			m_linkActionEdgeQues.PushPtr( (int)dist, pLA);

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




	void RegionSegmentor23::InitLinks(void)
	{
		const int nSrcWidth = m_src->GetWidth();
		const int nSrcHeight = m_src->GetHeight();

		//m_linkActionVect.SetSize(
		//	2 * (
		//	(4 * nSrcWidth * nSrcHeight) -
		//	nSrcHeight -	//	RC
		//	2 * (nSrcHeight + nSrcWidth - 1) -	//	RB + LB
		//	nSrcWidth	//	CB
		//	)
		//	);

		m_linkActionVect.SetSize(10000000);

		for( int i=0; i < m_linkActionVect.GetSize(); i++ )
			m_linkActionVect[ i ].nIndex = i;

		//m_linkActionVect.SetSize(50000000);

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

	S16ImageRef RegionSegmentor23::GenDifImg(bool a_bRC, bool a_bRB,
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

		for(int i=0; i < m_rgnInfoVect.GetSize(); i++)
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



	
	HistoGramRef RegionSegmentor23::GenDifHisto(bool a_bRC, bool a_bRB,
		bool a_bCB, bool a_bLB)
	{
		HistoGramRef hist = new HistoGram( GetMaxDif() + 1 );
		/*		FixedVector<Int32> & rHistVect = *hist->GetVectPtr();

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

	
	
	
	
	void RegionSegmentor23::Segment()
	{
		ListQue< LinkAction > tmpQue;

		//MultiListQue< LinkAction > m_difQues;
		
		MultiListQue< LinkAction > interRootQues;
		
		interRootQues.Init( m_linkActionMergeQues.GetNofQues() );

		//ListQue< LinkAction > interRootQue;




		//for(int e=0; e < 2; e++)
		//for(int e=0; e < 1; e++)
		{
			//for(int i=0; i < 2; i++)
			for(int i=0; i < 1; i++)
			{
				LinkAction * pLA = NULL;

				do
				{
					pLA = m_linkActionMergeQues.PopPtrMin();

					if( NULL == pLA )
					{
						continue;
					}


					RgnInfo * pRootRgn1 = pLA->pRgn1->GetRootRgn();

					if( NULL == pRootRgn1 )
					{
						// Bug
						ThrowHcvException();
					}

					RgnInfo * pRootRgn2 = pLA->pRgn2->GetRootRgn();

					bool bMergeRoots = false;

					//if( 1 != i )
					//if( 0 == i )
					//if( 0 == i || e >= 1 )
					//if( 0 == i || e >= 1 )
					{
						if( NULL != pRootRgn1 && NULL != pRootRgn2 )
						{
							if( pRootRgn1 == pRootRgn2 )
								continue;

							bMergeRoots = true;

/*
							float dist = CalcRgnDif( pRootRgn1, pRootRgn2 );

							interRootQues.PushPtr(dist, pLA );
							continue;
							*/


						}
						else		//	NULL == pRootRgn2
						{
							//if( NULL != pLA->pRgn1->pTraceRootElm )
								//continue;

							UpdateActRgn( pRootRgn1 );
							RgnInfo * pActRgn1 = pRootRgn1->GetActRgn();


							
							bool bAddRgn = true;

							if( NULL != pLA->pRgn2->pTraceRootElm )
							//if( false )
							{
								bAddRgn = false;

								RgnInfoListElm * pRootElm = pLA->pRgn2->pTraceRootElm;

								do
								{
									UpdateActRgn( pRootElm->pRgn );
									RgnInfo * pActRgnR = pRootElm->pRgn->GetActRgn();

									if(	pActRgnR == pActRgn1 )
									{
										bAddRgn = true;
										break;
									}

									pRootElm = pRootElm->pNext;
								}while( NULL != pRootElm );

								//pLA->pRgn2->pTraceRootElm
							}

							if( bAddRgn )
							{
								pLA->pRgn2->SetRootRgn( pRootRgn1 );

								pActRgn1->meanColors.IncBy( *pLA->pRgn2->pixColors );
								pActRgn1->nPixCnt++;

								PrepareRgnLinkActions( pLA->pRgn2, 0 );
							}

							continue;
						}

					}

					//else	//	i > 0
					//if( 1 == i )
					if( bMergeRoots )
					{
						UpdateActRgn( pLA->pRgn1 );
						RgnInfo * pActRgn1 = pLA->pRgn1->GetActRgn();

						if( NULL == pLA->pRgn2->GetRootRgn() )
						{
							// Bug
							ThrowHcvException();
						}

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


						if( HaveConflict( pMinSizRgn, pMaxSizRgn ) )
						//if(true)
						{
							continue;
						}
						else
						{
	//		NoConflict:
							if( pRootRgn2->bIsPassiveRoot )
							{
								*pRootRgn2->pixColors = *pRootRgn1->pixColors;
								pRootRgn2->bIsPassiveRoot = false;

								PrepareRgnLinkActions( pRootRgn2, 
									m_linkActionMergeQues.GetLastMinIndex() );
							}

							if( 0 != pMinSizRgn->conflictList.GetSize() )
								RemoveDuplicateConflicts( pMaxSizRgn );

							{
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

								pMasterRgn->conflictList.TakeListElements(
									pSlaveRgn->conflictList );
								pSlaveRgn->SetActRgn( pMasterRgn );

								pMasterRgn->meanColors.IncBy( pSlaveRgn->meanColors );
								pMasterRgn->nPixCnt += pSlaveRgn->nPixCnt;
							}


							//PrepareRgnLinkActions( pLA->pRgn2, 
							//	m_linkActionMergeQues.GetLastMinIndex() );
						}
					}
				}while( NULL != pLA );

/*
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
*/

				
			}

		}

		m_pAnyRoot = NULL;

		for(int i=0; i < m_rgnInfoVect.GetSize(); i++)
		{
			RgnInfo * pRgn = &m_rgnInfoVect[i];

			if( NULL == pRgn->GetRootRgn() )
			{
				if( NULL == m_pAnyRoot )
				{
					m_pAnyRoot = pRgn;
					pRgn->BeARoot();
				}
				else
				{
					pRgn->SetRootRgn( m_pAnyRoot );
				}
			}
		}

		for(int i=0; i < m_rgnInfoVect.GetSize(); i++)
		{
			RgnInfo * pRgn = &m_rgnInfoVect[i];
			UpdateActRgn( pRgn );
		}


		//ShowImage( GenMergeOrderImage(), "MergeOrderImage");	
		
		ShowImage( 
			GenColorDsp( 
			GenU8FromF32Image( GenMergeOrderImage() )), 			
			"MergeOrderImage");	

	}

	float RegionSegmentor23::CalcRgnDif(RgnInfo * a_pRgn1, RgnInfo * a_pRgn2)
	{
		float dist = m_difScale * F32ColorVal::Sub(
			*a_pRgn1->pixColors, *a_pRgn2->pixColors).CalcMag();

		dist += m_nAprSize;

		return dist;
	}

	void RegionSegmentor23::UpdateActRgn(RgnInfo * a_pRgn)
	{
		static FixedVector<RgnInfo *> rgnVect(10000);
		rgnVect.SetSize(10000);
		static int nMaxNofRgns = rgnVect.GetSize();

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

	S16ImageRef RegionSegmentor23::GenSegmentedImage( bool a_bShowMeanColor )
	{
		S16ImageRef ret = S16Image::Create(m_src->GetSize(), 3);

		for(int i=0; i < m_rgnInfoVect.GetSize(); i++)
		{
			RgnInfo * pRgn = &m_rgnInfoVect[i];
			RgnInfo * pActRgn = pRgn->GetActRgn();

			//pActRgn->meanColors.IncBy( *pActRgn->pixColors );

			//pActRgn->nPixCnt++;
		}

		for(int i=0; i < m_rgnInfoVect.GetSize(); i++)
		{
			RgnInfo * pRgn = &m_rgnInfoVect[i];
			RgnInfo * pActRgn = pRgn->GetActRgn();

			if( pActRgn == pRgn )
			{
				pRgn->meanColors.DividSelfBy( pRgn->nPixCnt );

				if( false == a_bShowMeanColor )
				{
					pRgn->meanColors.val0 = rand() % 256;
					pRgn->meanColors.val1 = rand() % 256;
					pRgn->meanColors.val2 = rand() % 256;
				}
			}
		}

		S16ChannelRef retCh0 = ret->GetAt(0);
		S16ChannelRef retCh1 = ret->GetAt(1);
		S16ChannelRef retCh2 = ret->GetAt(2);

		for(int i=0; i < m_rgnInfoVect.GetSize(); i++)
		{
			RgnInfo * pRgn = &m_rgnInfoVect[i];
			RgnInfo * pActRgn = pRgn->GetActRgn();
																			
				

			{
				if(pRgn->bShowLocalColor)
				//if(false)
				{
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

		m_outImg = ret;

		return ret;
	}



	void RegionSegmentor23::ShowEdgeOfConflict()
	{
		const float x1 = IOMgr::ReadInt("x1");
		const float y1 = IOMgr::ReadInt("y1");
		const float x2 = IOMgr::ReadInt("x2");
		const float y2 = IOMgr::ReadInt("y2");


		RgnInfo * pRgn1 = GetPointRgn(x1, y1);

		UpdateActRgn( pRgn1 );
		RgnInfo * pActRgn1 = pRgn1->GetActRgn();


		RgnInfo * pRgn2 = GetPointRgn(x2, y2);

		UpdateActRgn( pRgn2 );
		RgnInfo * pActRgn2 = pRgn2->GetActRgn();


		RgnConflict * pConflict = GetExistingConflict( pActRgn1, pActRgn2);

		if( NULL == pConflict )
			pConflict = pConflict;

		if( NULL != pConflict )
		{

			Hcv::S16ImageRef img1 = this->GenSegmentedImage(false);

			img1 = img1->Clone();

			{
				RgnInfo * pRgn = pConflict->pOrgEdge;

				HCV_CALL(

					cvCircle (
						img1->GetIplImagePtr(),
						//cvPoint( x1, y1 ),
						cvPoint( pRgn->pos.x, pRgn->pos.y ),
						3,
						//CV_RGB( a_color.val2, a_color.val1, a_color.val0 ),
						CV_RGB( 0, 255, 0 ),
						-1 // CV_FILL
					) );
			}

			{
				RgnInfo * pRgn = pConflict->pOrgR1;

				HCV_CALL(

					cvCircle (
						img1->GetIplImagePtr(),
						//cvPoint( x1, y1 ),
						cvPoint( pRgn->pos.x, pRgn->pos.y ),
						3,
						//CV_RGB( a_color.val2, a_color.val1, a_color.val0 ),
						CV_RGB( 255, 0, 0 ),
						-1 // CV_FILL
					) );
			}

			{
				RgnInfo * pRgn = pConflict->pOrgR2;

				HCV_CALL(

					cvCircle (
						img1->GetIplImagePtr(),
						//cvPoint( x1, y1 ),
						cvPoint( pRgn->pos.x, pRgn->pos.y ),
						3,
						//CV_RGB( a_color.val2, a_color.val1, a_color.val0 ),
						CV_RGB( 255, 0, 0 ),
						-1 // CV_FILL
					) );
			}

			ShowImage(img1, "RgnSgmImg");

		}







/*
		HCV_CALL(

			cvCircle (
				img1->GetIplImagePtr(),
				//cvPoint( x1, y1 ),
				cvPoint( x1, y1 ),
				3,
				//CV_RGB( a_color.val2, a_color.val1, a_color.val0 ),
				CV_RGB( 0, 255, 0 ),
				-1 // CV_FILL
			) );


		HCV_CALL(

			cvCircle (
				img1->GetIplImagePtr(),
				cvPoint( x2, y2 ),
				3,
				//CV_RGB( a_color.val2, a_color.val1, a_color.val0 ),
				CV_RGB( 255, 0, 0 ),
				-1 // CV_FILL
			) );
*/

		//ShowImage(img1, "RgnSgmImg");


	}



/*
	void RegionSegmentor23::PrepareConflicts()
	{
		//m_rgnConflictVect.SetSize( m_nofConflicts );
		m_rgnConflictVect.SetSize( 
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

	RegionSegmentor23::LinkAction * RegionSegmentor23::CloneLinkAction(LinkAction * a_pLA )
	{
		LinkAction * pLA = &m_linkActionVect[ m_nofLinkActions++ ];

		*pLA = *a_pLA;

		return pLA;
	}


	RegionSegmentor23::LinkAction * RegionSegmentor23::ProvideLinkAction(RgnInfo * a_pRgn, RgnLinkDir a_dir )
	{
		int x = a_pRgn->pos.x;
		int y = a_pRgn->pos.y;

		static int dx[] = {1, 1, 0, -1};
		static int dy[] = {0, 1, 1, 1};

		const int nLinkIndex = (int)a_dir;

		//RgnInfo * pRgn2 = GetPointRgn( x + dx[ nLinkIndex ], 
		//	y + dy[ nLinkIndex ]);

		RgnLink & rLink = a_pRgn->links[ nLinkIndex ];

		if( false == rLink.bExists )
		{
			return NULL;
		}

		RgnInfo * pRgn2 = rLink.pPeerRgn;

		RgnLink & rLinkR2 = pRgn2->links[ nLinkIndex + 4 ];


		LinkAction * pLA = &m_linkActionVect[ m_nofLinkActions++ ];

		pLA->pRgn1 = a_pRgn;
		pLA->pRgn2 = pRgn2;
		pLA->pLink1 = &rLink;
		pLA->pLink2 = &rLinkR2;
		pLA->pPrev = pLA;

		if( 2974074 == pLA->nIndex )
			pLA = pLA;


//		float distNew;

		{		
			//distNew = CalcRgnDif( a_pRgn->GetRootRgn(), pRgn2 );

			//distNew = CalcRgnDif( a_pRgn, pRgn2 );
			//distNew = a_rLink.DistMag;
/*
			float dist2 = CalcRgnDif( a_pRgn, pRgn2 );

			if ( distNew < dist2 )
				distNew = dist2;
*/
			//if( distNew >= ( GetMaxDif() - 1 ) )
			//	distNew = distNew;
		}



		//a_rLink.DistMag = a_rLink2.DistMag = distNew;
		
		//a_rLink.bProcessed = a_rLink2.bProcessed = true;

		//m_difQues.PushPtr( a_rLink.DistMag, pLA);
		//m_linkActionMergeQues.PushPtr( distNew, pLA);
		



		return pLA;
	}



	void RegionSegmentor23::PrepareLinkAction( RgnInfo * a_pRgn, RgnInfo * a_pRgn2,
			RgnLink & a_rLink, RgnLink & a_rLink2, float a_distBef, bool a_bSetProcessed )
	{
		if( NULL == a_pRgn->pTraceFwdRoot && NULL != a_pRgn2->pTraceFwdRoot )
		{	
			float dist1 = CalcRgnDif( a_pRgn->GetRootRgn(), a_pRgn2 );

			LinkAction * pLA = &m_linkActionVect[ m_nofLinkActions++ ];

			pLA->pRgn1 = a_pRgn;
			pLA->pRgn2 = a_pRgn2;
			pLA->pLink1 = &a_rLink;
			pLA->pLink2 = &a_rLink2;
			pLA->pPrev = pLA;

			m_medTraceQues.PushPtr( dist1, pLA );
		}

		if( a_rLink.bProcessed )
		{
			//a_rLink2.bProcessed = true;
			//return;
		}

		if( a_rLink2.bProcessed )
		{
			//a_rLink.bProcessed = true;
			//return;
		}


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

/*
			float dist2 = CalcRgnDif( a_pRgn, a_pRgn2 );

			if ( distNew < dist2 )
				distNew = dist2;
*/

			//if( distNew >= ( GetMaxDif() - 1 ) )
			//	distNew = distNew;
		}



		//a_rLink.DistMag = a_rLink2.DistMag = distNew;
		
		//if( a_bSetProcessed )
			a_rLink.bProcessed = a_rLink2.bProcessed = true;


		if( 2974074 == pLA->nIndex )
			pLA = pLA;


		//m_difQues.PushPtr( a_rLink.DistMag, pLA);
		m_linkActionMergeQues.PushPtr( distNew, pLA);
		
		
	}

	bool RegionSegmentor23::HaveConflict( RgnInfo * a_pMinSizRgn, RgnInfo * a_pMaxSizRgn)
	{
		RegionSegmentor23::RgnConflict * pConflict = GetExistingConflict( a_pMinSizRgn, a_pMaxSizRgn);

		if( NULL != pConflict )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	RegionSegmentor23::RgnConflict * RegionSegmentor23::GetExistingConflict( RgnInfo * a_pMinSizRgn, RgnInfo * a_pMaxSizRgn)
	{
		//bool bRet = false;

		APtrList< RgnConflict > & minConfList = 
			a_pMinSizRgn->conflictList;

		RegionSegmentor23::RgnConflict * pConflict = minConfList.Last();

		while( NULL != pConflict )
		{
			RgnInfo * pPeerRgn = pConflict->pPeerRgn;

			UpdateActRgn( pPeerRgn );
			pPeerRgn = pPeerRgn->GetActRgn();
 			pConflict->pPeerRgn = pPeerRgn;

			if( pPeerRgn == a_pMaxSizRgn )
				//return true;
				return pConflict;

			RgnConflict * pNext = minConfList.Next();
			
			if( pPeerRgn->nLastVisitID == m_nVisitID )
			{
				minConfList.RemovePtr( pConflict );
			}

			pPeerRgn->nLastVisitID = m_nVisitID;

			pConflict = pNext;
		}

		//return false;
		return NULL;
	}

	void RegionSegmentor23::RemoveDuplicateConflicts( RgnInfo * a_pRgn )
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


	void RegionSegmentor23::PrepareRgnLinkActions( RgnInfo * a_pRgn, float a_distBef )
	{
		RgnLink * links = a_pRgn->links;

		for(int i=0; i<4; i++)
		{
			RgnLink & rLink = links[i];

//			if( rLink.bProcessed )
			if( false == rLink.bExists )
				continue;

			RgnInfo * pRgn2 = rLink.pPeerRgn;
			RgnLink & rLink2 = pRgn2->links[ i + 4 ];

			PrepareLinkAction( a_pRgn, pRgn2, rLink, rLink2, a_distBef );
		}

		for(int i=4; i<8; i++)
		{
			RgnLink & rLink = links[i];

//			if( rLink.bProcessed )
			if( false == rLink.bExists )
				continue;

			RgnInfo * pRgn2 = rLink.pPeerRgn;
			RgnLink & rLink2 = pRgn2->links[ i - 4 ];

			PrepareLinkAction( a_pRgn, pRgn2, rLink, rLink2, a_distBef );
		}

	}

	S16ImageRef RegionSegmentor23::GenConflictImg(bool a_bRC, bool a_bRB,
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

		for(int i=0; i < m_rgnInfoVect.GetSize(); i++)
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


	void RegionSegmentor23::CreateConflict( RgnInfo * a_pRgn1, RgnInfo * a_pRgn2, RgnInfo * a_pOrgEdge )
	{
		if( NULL == a_pOrgEdge )
			a_pOrgEdge = a_pOrgEdge;

		RgnConflict * pRc = &m_rgnConflictVect[ m_nofConflicts++ ];
		pRc->pNext = pRc;
		pRc->pPrev = pRc;

		pRc->pOrgEdge = a_pOrgEdge;
		pRc->pOrgR1 = a_pRgn1;
		pRc->pOrgR2 = a_pRgn2;

		pRc->pPeerRgn = a_pRgn2;
		a_pRgn1->conflictList.PushLast( pRc );

		RgnConflict * pRc2 = &m_rgnConflictVect[ m_nofConflicts++ ];
		pRc2->pNext = pRc2;
		pRc2->pPrev = pRc2;

		pRc2->pOrgEdge = a_pOrgEdge;
		pRc2->pOrgR1 = a_pRgn1;
		pRc2->pOrgR2 = a_pRgn2;

		pRc2->pPeerRgn = a_pRgn1;
		a_pRgn2->conflictList.PushLast( pRc2 );
	}


//////////////////////////////////////////////////////////////


	void RegionSegmentor23::MgrOfLinkTypes::CoreStuff::InitLinkTypeMgrs()
	{
		static int dx[] = {1, 1, 0, -1,   -1, -1, 0, 1};
		static int dy[] = {0, 1, 1, 1,   0, -1, -1, -1};

		m_linkTypeMgrVect.SetSize(8);

		m_dxyIndexMap[ m_dxyIndexCalc.Calc( 0 + 1, 0 + 1) ] = -1;

		for(int i=0; i < m_linkTypeMgrVect.GetSize(); i++)
		{
			F32Point point(dx[i], dy[i]);
						
			m_linkTypeMgrVect[i].Init( &m_linkTypeMgrVect[0], i, point );


			m_dxyIndexMap[ m_dxyIndexCalc.Calc( dx[i] + 1, dy[i] + 1) ] = i;
		}

	}

	void RegionSegmentor23::MgrOfLinkTypes::CoreStuff::InitMaps()
	{
		m_linkTypeMgrVect.SetSize(8);

		{
			for(int i=0; i < m_linkTypeMgrVect.GetSize(); i++)
			{
				F32Point uDirXY_i = m_linkTypeMgrVect[i].GetUnitDirXY();

				for(int j=0; j < m_linkTypeMgrVect.GetSize(); j++)
				{
					F32Point uDirXY_j = m_linkTypeMgrVect[j].GetUnitDirXY();

					float dist = F32Point::Sub(uDirXY_i, uDirXY_j).CalcMag(); 
					//m_distMap[ m_mapIndexCalc.Calc(i, j) ] = dist;
					m_distMap[ m_mapIndexCalc.Calc(j, i) ] = dist;
				}
			}
		}

		{
			for(int i=0; i < m_linkTypeMgrVect.GetSize(); i++)
			{				
				for(int j=0; j < m_linkTypeMgrVect.GetSize(); j++)
				{					
					//m_nbrMap[ m_mapIndexCalc.Calc(i, j) ] = j;
					m_nbrMap[ m_mapIndexCalc.Calc(j, i) ] = j;
				}
			}
		}

		{
			for(int i=0; i < m_linkTypeMgrVect.GetSize(); i++)
			{		
				for(int k=0; k < m_linkTypeMgrVect.GetSize(); k++)
				{		
					int minIndex = k;
					//float distMinIndex = m_distMap[ m_mapIndexCalc.Calc(i, minIndex) ]; 
					float distMinIndex = m_distMap[ m_mapIndexCalc.Calc(
						//minIndex, 
						m_nbrMap[ m_mapIndexCalc.Calc(minIndex, i) ],
						i) ]; 

					for(int j=k+1; j < m_linkTypeMgrVect.GetSize(); j++)
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



	void RegionSegmentor23::ScanImageLines()
	{
		CvSize siz1 = m_src->GetSize();

		m_conflictScanner = new ConflictScanner2( m_src );

		const int nHalfAprSize = m_conflictScanner->GetAprSize() / 2;

		//m_conflictScanner = NULL;

		bool bDoHorz = true;
		bool bDoVert = true;

		bool bDoDiag = true;
		bool bDoNeDiag = true;


		bool bAlwaysInner = true;
		//bool bAlwaysInner = false;


/////////  Horz

		if( bDoHorz )
		{

			for(int y=0, k=0; y < siz1.height && k < 10000; y += 1, k++)			
			{
				LinePathRef lp = new LinePath( 0, y, siz1.width - 1, y );

		
				m_conflictScanner->ProcessLine( lp );

				RgnLinkDir dir1 = RgnLinkDir::RC;

				ProcessLineScanResult( dir1 );
			}

		}


/////////  Vert


		if( bDoVert )
		{

			for(int x=0; x < siz1.width; x ++)			
			{
				LinePathRef lp = new LinePath( x, 0, x, siz1.height - 1 );

				m_conflictScanner->ProcessLine( lp );

				RgnLinkDir dir1 = RgnLinkDir::CB;

				ProcessLineScanResult( dir1 );
			}


		}



/////////  Diag


		if( bDoDiag )
		{
			FixedVector< F32Point > bgnPointVect;

			{
				bgnPointVect.SetCapacity( siz1.width + siz1.height );

				for( int h = siz1.height - 1; h >= 0; h-- )
				{
					bgnPointVect.IncSize();

					bgnPointVect.GetBack().x = 0;
					bgnPointVect.GetBack().y = h;
				}

				for( int w = 1; w < siz1.width; w++ )
				{
					bgnPointVect.IncSize();

					bgnPointVect.GetBack().x = w;
					bgnPointVect.GetBack().y = 0;
				}

			}

			FixedVector< F32Point > endPointVect;

			{
				endPointVect.SetCapacity( siz1.width + siz1.height );

				for( int w = 0; w < siz1.width; w++ )
				{
					endPointVect.IncSize();

					endPointVect.GetBack().x = w;
					endPointVect.GetBack().y = siz1.height - 1;
				}

				for( int h = siz1.height - 2; h >= 0; h-- )
				{
					endPointVect.IncSize();

					endPointVect.GetBack().x = siz1.width - 1;
					endPointVect.GetBack().y = h;
				}

			}


			for(int u=0; u < endPointVect.GetSize(); u ++)			
			//for(int u=0; u < endPointVect.GetSize(); u += 2)			
			{
				{
					int dx = endPointVect[u].x - bgnPointVect[u].x;
					int dy = endPointVect[u].y - bgnPointVect[u].y;

					if( dx != dy )
						dx = dx;
				}

				//LinePathRef lp = new LinePath( bgnPointVect[u].x, bgnPointVect[u].y,
				LinePathRef lp = new DiagonalLinePath( bgnPointVect[u].x, bgnPointVect[u].y,					
					endPointVect[u].x, endPointVect[u].y );
		
		
				m_conflictScanner->ProcessLine( lp );

				RgnLinkDir dir1 = RgnLinkDir::RB;

				ProcessLineScanResult( dir1 );
			}


		}





/////////  NeDiag


		if( bDoNeDiag )
		{
			FixedVector< F32Point > bgnPointVect;

			{
				bgnPointVect.SetCapacity( siz1.width + siz1.height );

				for( int h = siz1.height - 1; h >= 0; h-- )
				{
					bgnPointVect.IncSize();

					bgnPointVect.GetBack().x = siz1.width - 1;
					bgnPointVect.GetBack().y = h;
				}

				for( int w = siz1.width - 2; w >= 0; w-- )
				{
					bgnPointVect.IncSize();

					bgnPointVect.GetBack().x = w;
					bgnPointVect.GetBack().y = 0;
				}

			}

			FixedVector< F32Point > endPointVect;

			{
				endPointVect.SetCapacity( siz1.width + siz1.height );

				for( int w = siz1.width - 1; w >= 0; w-- )
				{
					endPointVect.IncSize();

					endPointVect.GetBack().x = w;
					endPointVect.GetBack().y = siz1.height - 1;
				}

				for( int h = siz1.height - 2; h >= 0; h-- )
				{
					endPointVect.IncSize();

					endPointVect.GetBack().x = 0;
					endPointVect.GetBack().y = h;
				}

			}


			for(int u=0; u < endPointVect.GetSize(); u ++)			
			//for(int u=0; u < endPointVect.GetSize(); u += 2)			
			{
				{
					int dx = endPointVect[u].x - bgnPointVect[u].x;
					int dy = endPointVect[u].y - bgnPointVect[u].y;

					if( dx != -dy )
						dx = dx;
				}

				LinePathRef lp = new NegDiagonalLinePath( bgnPointVect[u].x, bgnPointVect[u].y,
					endPointVect[u].x, endPointVect[u].y );		
		
				m_conflictScanner->ProcessLine( lp );

				RgnLinkDir dir1 = RgnLinkDir::LB;


				ProcessLineScanResult( dir1 );
			}


		}

		PrepareLocalGrad();

		FilterByLocalGrad();

		MarkNearEdgeRgns();

		FilterByNearEdgeRoots();


		F32ImageRef scanImage = this->GenScanImage();


		AssignTraceIntersections();

		//GlobalStuff::SetLinePathImg( scanImage );


/////////////////////////




		static int dxArr[] = {1, 1, 0, -1};
		static int dyArr[] = {0, 1, 1, 1};

		int nCurTraceProcID = 0;


		for( int k=0; k < m_edgeInfoVect.GetSize(); k++ )
		{

			EdgeInfo & rEI = m_edgeInfoVect[ k ];





			//bool bRootInTrace = rEI.pRgnR1->bInTrace || rEI.pRgnR2->bInTrace;
			bool bRootInTrace = rEI.pRgnInrR1->bInTrace || rEI.pRgnInrR2->bInTrace;

			


			//if( bRootInTrace )
			if( rEI.IsCanceled() )
				continue;


			//if( 127 == k )
			//	k = k;


			RgnInfo * pRootRgn1 = rEI.pRgnR1;
			RgnInfo * pRootRgn2 = rEI.pRgnR2;

			//if( pRootRgn1->bIsPassiveRoot || pRootRgn2->bIsPassiveRoot )
				//continue;

			//RgnInfo * pRootRgn1 = rEI.pRgnInrR1;
			//RgnInfo * pRootRgn2 = rEI.pRgnInrR2;

			pRootRgn1->BeARoot();					
			pRootRgn2->BeARoot();

			CreateConflict( pRootRgn1, pRootRgn2, rEI.pEdgeRgn );


			//if( ! rEI.pEdgeRgn->bIsValleyRoot )
			//if( ! rEI.pRgnInrR1->bInTrace || ! rEI.pRgnInrR2->bInTrace )

			//if( ! bRootInTrace && ! rEI.bIntersected )

			{
				if( ! pRootRgn1->bIsPassiveRoot )
					PrepareRgnLinkActions( pRootRgn1, 0 );

				if( ! pRootRgn2->bIsPassiveRoot )
					PrepareRgnLinkActions( pRootRgn2, 0 );
			}

			//if( bRootInTrace )
			//	continue;

			//if( bRootInTrace || rEI.bIntersected )
				continue;


			//if( false == rEI.bMax ||
				//( RgnLinkDir::RC != rEI.Dir && RgnLinkDir::CB != rEI.Dir ) )
			//if( false == rEI.bMax )
				//continue;


			const RgnLinkDir dirOp = (RgnLinkDir) ((int) rEI.Dir + 4);

			const int dx = dxArr[ (int) rEI.Dir ];
			const int dy = dyArr[ (int) rEI.Dir ];

			//bool bFwdDone = false;
			//bool bBkdDone = false;

			nCurTraceProcID++;




			RgnInfo * pFwdRgn = this->GetPointRgn( 
						//pRootRgn1->pos.x + dx, pRootRgn1->pos.y + dy );
						rEI.pRgnR1->pos.x + dx, rEI.pRgnR1->pos.y + dy );

			RgnInfo * pBkdRgn = this->GetPointRgn( 
						//pRootRgn2->pos.x - dx, pRootRgn2->pos.y - dy );
						rEI.pRgnR2->pos.x + dx, rEI.pRgnR2->pos.y + dy );


			//for( int p=0; p < m_edgeInfoVect.GetSize(); p++ )


			int nFwdCycle = 0;
			int nBkdCycle = 0;

			{
				LinkAction * pLA = ProvideLinkAction( pRootRgn1, rEI.Dir );
				//LinkAction * pLA = ProvideLinkAction( rEI.pRgnR1, rEI.Dir );
				m_linkActionMergeQues.PushPtr( nFwdCycle++, pLA );
				pFwdRgn->nTraceProcID  = nCurTraceProcID;
			}

			{
				LinkAction * pLA = ProvideLinkAction( pRootRgn2, dirOp );
				//LinkAction * pLA = ProvideLinkAction( rEI.pRgnR2, rEI.Dir );
				m_linkActionMergeQues.PushPtr( nBkdCycle++, pLA );
				pBkdRgn->nTraceProcID = nCurTraceProcID;
			}

			//bool bFwdDone = false;

			//while( false == pFwdRgn->bInTraceAttachDone && false == pBkdRgn->bInTraceAttachDone )
			//while( nCurTraceProcID != pFwdRgn->nTraceProcID && nCurTraceProcID != pBkdRgn->nTraceProcID )
			//while(false)
			//if( ! bRootInTrace )

			do
			{
				float distFwd = CalcRgnDif( pRootRgn1, pFwdRgn );
				float distBkd = CalcRgnDif( pRootRgn2, pFwdRgn );

				if( distBkd < distFwd )
					break;

				//if( false == pFwdRgn->bInTraceAttachDone )
				//if( nCurTraceProcID != pFwdRgn->nTraceProcID )
				{
					//pFwdRgn->SetRootRgn( pRootRgn1 );

					//ProvideLinkAction

					pFwdRgn->bInTraceAttachDone = true;

					pFwdRgn->nTraceProcID  = nCurTraceProcID;


					//LinkAction * pLA = ProvideLinkAction( pFwdRgn, rEI.Dir );
					//m_linkActionMergeQues.PushPtr( nFwdCycle++, pLA );


					pFwdRgn = this->GetPointRgn( 
						pFwdRgn->pos.x + dx, pFwdRgn->pos.y + dy );
				}
			}
			while( nCurTraceProcID != pFwdRgn->nTraceProcID  );


			do
			{

				//if( false == pBkdRgn->bInTraceAttachDone )
				//if( nCurTraceProcID != pBkdRgn->nTraceProcID )
				{
					//pBkdRgn->SetRootRgn( pRootRgn2 );

					pBkdRgn->bInTraceAttachDone = true;

					pBkdRgn->nTraceProcID = nCurTraceProcID;


					//LinkAction * pLA = ProvideLinkAction( pBkdRgn, dirOp );
					//m_linkActionMergeQues.PushPtr( nBkdCycle++, pLA );


					pBkdRgn = this->GetPointRgn( 
						pBkdRgn->pos.x - dx, pBkdRgn->pos.y - dy );
				}

			}
			while( nCurTraceProcID != pBkdRgn->nTraceProcID );




		}
		




		//ShowImage( m_conflictScanner->GetSignalDspImg(), "Signals" );

		

		//ShowImage( m_conflictScanner->GetResultImg(), "LinePathImg");
		//ShowImage( m_conflictScanner->GetResultImg(), "RgnSgmt_Scans");

		//ShowImage( this->GenScanImage(), "RgnSgmt_Scans");

		ShowImage( scanImage, "RgnSgmt_Scans");

		ShowImage( GenLocalGradImage(), "LocalGrad");

/*
		{
			F32ImageRef localGradValImg = GenLocalGradValImage();
			//ShowImage( localGradValImg, "LocalGradVal");

			GlobalStuff::SetLinePathImg( localGradValImg );
			ShowImage( localGradValImg, "LinePathImg");
		}
*/

		ShowImage( GenNearEdgeImage(), "NearEdge");

		

		

		//ShowImage( GlobalStuff::GetLinePathImg(), "LinePathImg");


		//FiltSysDebug::SaveToFile( "" );

	}


	void RegionSegmentor23::FilterByLocalGrad()
	{
		for( int k=0; k < m_edgeInfoVect.GetSize(); k++ )
		{
			EdgeInfo & rEI = m_edgeInfoVect[ k ];

			if( rEI.IsCanceled() )
				continue;

			//if( rEI.Dir != rEI.pEdgeRgn->localGradDir ||  rEI.pEdgeRgn->localGradVal < 0.5 )
			//if( rEI.pEdgeRgn->localGradValArr[ (int)rEI.Dir ] < 0.5 ||  rEI.pEdgeRgn->localGradVal < 0.5 )
			if( rEI.pEdgeRgn->localGradValArr[ (int)rEI.Dir ] < 0.5  )
			{
				rEI.SetCanceled( true );
			}
		}
	}


	F32ImageRef RegionSegmentor23::GenScanImage()
	{
		F32ImageRef ret = this->m_src->Clone();

		ret->SetAll( 0 );


		U8ColorVal colorEdgeArr[ 4 ];

		{
			colorEdgeArr[0].AssignVal( 0, 180, 0 );
			colorEdgeArr[1].AssignVal( 255, 0, 0 );
			colorEdgeArr[2].AssignVal( 0, 0, 255 );
			colorEdgeArr[3].AssignVal( 0, 180, 255 );			
		}

		U8ColorVal colorRootClear( 255, 255, 255 );
		U8ColorVal colorRootInTrace( 255, 0, 200 );

		for( int k=0; k < m_edgeInfoVect.GetSize(); k++ )
		{

			EdgeInfo & rEI = m_edgeInfoVect[ k ];

			if( rEI.IsCanceled() )
				continue;

			{
				F32ColorVal * pix = (F32ColorVal *) ret->GetPixAt( rEI.pEdgeRgn->pos.x, rEI.pEdgeRgn->pos.y );

				U8ColorVal color1 = colorEdgeArr[ (int)rEI.Dir ];

				pix->val0 = color1.val0;
				pix->val1 = color1.val1;
				pix->val2 = color1.val2;
			}


			{
				RgnInfo * pRgn = rEI.pRgnR1;

				F32ColorVal * pix = (F32ColorVal *) ret->GetPixAt( pRgn->pos.x, pRgn->pos.y );
				
				U8ColorVal color1;

/*				if( rEI.Dir != pRgn->localGradDir && pRgn->localGradVal > 0.5 )
				{
					pRgn->bIsPassiveRoot = true;
					color1 = colorRootInTrace;
				}
				else*/
					color1 = colorRootClear;

				pix->val0 = color1.val0;
				pix->val1 = color1.val1;
				pix->val2 = color1.val2;
			}

			{
				RgnInfo * pRgn = rEI.pRgnR2;

				F32ColorVal * pix = (F32ColorVal *) ret->GetPixAt( pRgn->pos.x, pRgn->pos.y );
				
				U8ColorVal color1;

/*				if( rEI.Dir != pRgn->localGradDir && pRgn->localGradVal > 0.5 )
				{
					pRgn->bIsPassiveRoot = true;
					color1 = colorRootInTrace;
				}
				else*/
					color1 = colorRootClear;
					
				pix->val0 = color1.val0;
				pix->val1 = color1.val1;
				pix->val2 = color1.val2;
			}
		}

		return ret;
	}



	F32ImageRef RegionSegmentor23::GenNearEdgeImage()
	{
		F32ImageRef ret = this->m_src->Clone();

		ret->SetAll( 0 );

		//U8ColorVal colorNearEdge( 0, 200, 255 );

		{
			F32ColorVal colorNearEdge;
			colorNearEdge.AssignVal( 0, 200, 255 );

			F32ColorVal colorNearEdge_9;
			colorNearEdge_9.AssignVal( 255, 0, 0 );

			for( int i=0; i < m_rgnInfoVect.GetSize(); i++ )
			{
				RgnInfo * pRgn = &m_rgnInfoVect[ i ];

				F32ColorVal * pix = (F32ColorVal *) ret->GetPixAt( pRgn->pos.x, pRgn->pos.y );

				if( pRgn->bIsNearEdge_9 )
					*pix = colorNearEdge_9;

				if( pRgn->bIsNearEdge )
				{
					*pix = colorNearEdge;
				}
			}

		}

		{
			F32ColorVal colorNeAndE;
			//colorNeAndE.AssignVal( 0, 0, 255 );
			//colorNeAndE.AssignVal( 0, 180, 255 );
			colorNeAndE.AssignVal( 255, 0, 255 );

			for( int k=0; k < m_edgeInfoVect.GetSize(); k++ )
			{
				EdgeInfo & rEI = m_edgeInfoVect[ k ];

				if( rEI.IsCanceled() )
					continue;

				RgnInfo * pRgn = rEI.pEdgeRgn;

				F32ColorVal * pix = (F32ColorVal *) ret->GetPixAt( pRgn->pos.x, pRgn->pos.y );

				if( pRgn->bIsNearEdge )
				{
					*pix = colorNeAndE;
				}
			}


		}

		return ret;
	}

	void RegionSegmentor23::PrepareLocalGrad( )
	{

		{
			const int nRC = (int)RgnLinkDir::RC;
			const int nRB = (int)RgnLinkDir::RB;
			const int nCB = (int)RgnLinkDir::CB;
			const int nLB = (int)RgnLinkDir::LB;

			for( int i=0; i < m_rgnInfoVect.GetSize(); i++ )
			{
				RgnInfo * pRgn = &m_rgnInfoVect[ i ];

				pRgn->pMaxPostDif = 0;

				for( int j=0; j < 4; j++ )
				{
					//if( pRgn->localGradValArr[ j ] > pRgn->pMaxPostDif )
						//pRgn->pMaxPostDif = pRgn->localGradValArr[ j ];

					if( pRgn->postDif1Arr[ j ] > pRgn->pMaxPostDif )
						pRgn->pMaxPostDif = pRgn->postDif1Arr[ j ];
				}


				float * localGradValArr = pRgn->localGradValArr;

				int nDir1 = nCB;
				{
					float minVal = localGradValArr[ nRC ];

					if( minVal > localGradValArr[ nCB ] )
					{
						minVal = localGradValArr[ nCB ];
						nDir1 = nRC;
					}

					localGradValArr[ nRC ] -= minVal;
					localGradValArr[ nCB ] -= minVal;
				}

				int nDir2 = nLB;
				{
					float minVal = localGradValArr[ nRB ];

					if( minVal > localGradValArr[ nLB ] )
					{
						minVal = localGradValArr[ nLB ];
						nDir2 = nRB;
					}

					localGradValArr[ nRB ] -= minVal;
					localGradValArr[ nLB ] -= minVal;
				}

				int nDir3 = nDir2;
				{
					float minVal = localGradValArr[ nDir1 ];

					if( minVal > localGradValArr[ nDir2 ] )
					{
						minVal = localGradValArr[ nDir2 ];
						nDir3 = nDir1;
					}

					//if( minVal / localGradValArr[ nDir3 ] < 0.8 )
					//if( minVal / localGradValArr[ nDir3 ] < 0.65 )
					//if( minVal / localGradValArr[ nDir3 ] < 0.55 )
					//if( minVal / localGradValArr[ nDir3 ] < 0.4 )
					if( minVal / localGradValArr[ nDir3 ] < 0.286 )
					{
						localGradValArr[ nDir1 ] -= minVal;
						localGradValArr[ nDir2 ] -= minVal;

						localGradValArr[ nDir3 ] += minVal;
					}
					else
					{
						//localGradValArr[ nDir1 ] = 0;
						//localGradValArr[ nDir2 ] = 0;

						localGradValArr[ nDir1 ] = localGradValArr[ nDir3 ];
						localGradValArr[ nDir2 ] = localGradValArr[ nDir3 ];						

						//localGradValArr[ nDir1 ] = localGradValArr[ nDir3 ];
						//nDir3 = nDir1;

						//localGradValArr[ nDir2 ] = 0;						

					}

					if( localGradValArr[ nDir3 ] < 1 )
					//if( localGradValArr[ nDir3 ] < 3 )
						localGradValArr[ nDir3 ] = 0;

					pRgn->localGradVal = localGradValArr[ nDir3 ];

				}

				{
					int nMaxIdx = 0;

					for( int j=1; j < 4; j++ )
					{
						if( localGradValArr[ j ] > localGradValArr[ nMaxIdx ] )
							nMaxIdx = j;
					}

					//pRgn->localGradDir = (RgnLinkDir) nMaxIdx;
				}

			}
		}

	}


	void RegionSegmentor23::FilterByNearEdgeRoots()
	{
		for( int k=0; k < m_edgeInfoVect.GetSize(); k++ )
		{
			EdgeInfo & rEI = m_edgeInfoVect[ k ];

			if( rEI.IsCanceled() )
				continue;

			//if( rEI.pRgnR1->bIsNearEdge || rEI.pRgnR2->bIsNearEdge )
			if( rEI.pRgnR1->bIsNearEdge || rEI.pRgnR2->bIsNearEdge || 
				rEI.pRgnR1->bIsEdge || rEI.pRgnR2->bIsEdge || 
				rEI.pEdgeRgn->bIsNearEdge || 

				//( rEI.pRgnR1->localGradDir != rEI.Dir && rEI.pRgnR1->localGradVal > 0.5 ) ||
				//( rEI.pRgnR2->localGradDir != rEI.Dir && rEI.pRgnR2->localGradVal > 0.5 )
				
				//( rEI.pRgnInrR1->localGradDir != rEI.Dir && rEI.pRgnInrR1->localGradVal > 0.5 ) ||
				//( rEI.pRgnInrR2->localGradDir != rEI.Dir && rEI.pRgnInrR2->localGradVal > 0.5 )
				
				( rEI.pRgnInrR1->localGradValArr[ (int)rEI.Dir ] < 0.5 && rEI.pRgnInrR1->localGradVal > 0.5 ) ||
				( rEI.pRgnInrR2->localGradValArr[ (int)rEI.Dir ] < 0.5 && rEI.pRgnInrR2->localGradVal > 0.5 )
				
				)
				rEI.SetCanceled( true );
		}
	}


	void RegionSegmentor23::MarkNearEdgeRgns()
	{


		static int dxArr[] = {1, 1, 0, -1};
		static int dyArr[] = {0, 1, 1, 1};

		int nCurTraceProcID = 0;


		for( int k=0; k < m_edgeInfoVect.GetSize(); k++ )
		{
			EdgeInfo & rEI = m_edgeInfoVect[ k ];

			if( rEI.IsCanceled() )
				continue;

			if( 8789 == k )					
				k = k;

			// Hthm Tmp
			if( rEI.pEdgeRgn == rEI.pRgnR1 ||
				rEI.pEdgeRgn == rEI.pRgnR2 )
			{
				rEI.SetCanceled( true );
				continue;
			}


			if( rEI.ValueR1 < -90 )
				rEI.ValueR1 = rEI.pRgnR1->postDif1Arr[ (int) rEI.Dir ];

			if( rEI.ValueR2 < -90 )
				rEI.ValueR2 = rEI.pRgnR2->postDif1Arr[ (int) rEI.Dir ];

			
			//if( RgnLinkDir::CB != rEI.Dir )
			//	continue;

			RgnInfo * pRootRgn1 = rEI.pRgnR1;			
			RgnInfo * pRootRgn2 = rEI.pRgnR2;

			//RgnInfo * pRootRgn1 = rEI.pRgnInrR1;
			//RgnInfo * pRootRgn2 = rEI.pRgnInrR2;



			const RgnLinkDir dirOp = (RgnLinkDir) ((int) rEI.Dir + 4);

			const int dx = dxArr[ (int) rEI.Dir ];
			const int dy = dyArr[ (int) rEI.Dir ];


			int nAllCnt = 0;
			int nNearAllCnt = 0;

			RgnInfo * pRgn = rEI.pRgnR1;

			pRgn = this->GetPointRgn( 
						pRgn->pos.x + dx, pRgn->pos.y + dy );
			nAllCnt++;			

			{
				const float thr1 = ( rEI.Value + rEI.ValueR1 ) * 0.5;
				const float thr1_9 = ( rEI.Value + rEI.ValueR1 ) * 0.1;
				const float thr1_4 = ( rEI.Value + rEI.ValueR1 ) * 0.4;

				RgnInfo * pNewR1 = NULL;
				RgnInfo * pNewInrR1 = NULL;

				while( rEI.pEdgeRgn != pRgn )
				{
					if( pRgn->postDif1Arr[ (int) rEI.Dir ] > thr1_9 )
					{
						if( NULL == pNewR1 )
						{
							pNewR1 = pRgn;
						}

						pRgn->bIsNearEdge_9 = true;
					}

					if( pRgn->postDif1Arr[ (int) rEI.Dir ] > thr1_4 )
					{
						if( NULL == pNewInrR1 )
						{
							pNewInrR1 = pRgn;
						}
					}
					

					if( pRgn->postDif1Arr[ (int) rEI.Dir ] > thr1 )
					{
						pRgn->bIsNearEdge = true;
						nNearAllCnt++;
					}

					pRgn = this->GetPointRgn( 
								pRgn->pos.x + dx, pRgn->pos.y + dy );
					nAllCnt++;			

				}

				if( NULL != pNewR1 )
				{
					rEI.pRgnR1 = pNewR1;
				}

				if( NULL != pNewInrR1 )
				{
					rEI.pRgnInrR1 = pNewInrR1;
				}				
				
				pRgn->bIsEdge = true;				
			}

			//pRgn = this->GetPointRgn( 
			//			pRgn->pos.x + dx, pRgn->pos.y + dy );


			pRgn = rEI.pRgnR2;

			pRgn = this->GetPointRgn( 
						pRgn->pos.x - dx, pRgn->pos.y - dy );

			nAllCnt++;			

			{
				//if( 10366 == k )
				if( 8789 == k )					
					k = k;

				const float thr2 = ( rEI.Value + rEI.ValueR2 ) * 0.5;
				const float thr2_9 = ( rEI.Value + rEI.ValueR2 ) * 0.1;
				const float thr1_4 = ( rEI.Value + rEI.ValueR1 ) * 0.4;

				RgnInfo * pNewR2 = NULL;
				RgnInfo * pNewInrR2 = NULL;

				//while( rEI.pRgnR2 != pRgn )
				while( rEI.pEdgeRgn != pRgn )
				{
					if( 312 - 1 == nAllCnt )
						nAllCnt = nAllCnt;

					if( pRgn->postDif1Arr[ (int) rEI.Dir ] > thr2_9 )
					{
						if( NULL == pNewR2 )
						{
							pNewR2 = pRgn;
						}

						pRgn->bIsNearEdge_9 = true;
					}

					if( pRgn->postDif1Arr[ (int) rEI.Dir ] > thr1_4 )
					{
						if( NULL == pNewInrR2 )
						{
							pNewInrR2 = pRgn;
						}
					}

					if( pRgn->postDif1Arr[ (int) rEI.Dir ] > thr2 )
					{
						pRgn->bIsNearEdge = true;
						nNearAllCnt++;

						//pRgn->showColor.AssignVal( 0, 200, 0 );
						//pRgn->bShowLocalColor = true;
					}

					pRgn = this->GetPointRgn( 
								pRgn->pos.x - dx, pRgn->pos.y - dy );
					nAllCnt++;			
				}

				if( NULL != pNewR2 )
				{
					rEI.pRgnR2 = pNewR2;
				}
				
				if( NULL != pNewInrR2 )
				{
					rEI.pRgnInrR2 = pNewInrR2;
				}				

			}

			if( nNearAllCnt > 0.7 * nAllCnt )
				nAllCnt = nAllCnt;
		}


	}


	void RegionSegmentor23::ProcessLineScanResult( RgnLinkDir a_dir )
	{

		for( int i=0; i < m_conflictScanner->GetLineLen(); i++ )
		{
			RgnInfo * pRgn = &m_rgnInfoVect[ m_conflictScanner->GetIOImgeOfIOLine( i ) ];

			{
				float locGradVal = m_conflictScanner->GetLocalGradOfIOLine( i );

				pRgn->localGradValArr[ (int)a_dir ] = locGradVal;
			}

			{
				float postDif1 = m_conflictScanner->GetPostDif1OfIOLine( i );

				pRgn->postDif1Arr[ (int)a_dir ] = postDif1;
			}

			

/*

			if( locGradVal > pRgn->localGradVal )
			{
				pRgn->localGradVal = locGradVal;
				pRgn->localGradDir = a_dir;
			}
*/
		}


		const int nHalfAprSize = m_conflictScanner->GetAprSize() / 2;

		bool bAlwaysInner = true;

		for( int i=0; i < m_conflictScanner->GetNofConflicts(); i++ )
		{
			m_edgeInfoVect.IncSize();

			EdgeInfo & rEI = m_edgeInfoVect.GetBack();

			ConflictScanner2::RootIOLPair rip1 = m_conflictScanner->GetConflictPairAt( i );

			RgnInfo * pEdgeRgn = &m_rgnInfoVect[ m_conflictScanner->GetIOImgeOfIOLine( rip1.EdgeIOL ) ];
			RgnInfo * pRootRgn1 = &m_rgnInfoVect[ m_conflictScanner->GetIOImgeOfIOLine( rip1.IOL1 ) ];
			RgnInfo * pRootRgn2 = &m_rgnInfoVect[ m_conflictScanner->GetIOImgeOfIOLine( rip1.IOL2 ) ];

			rEI.pEdgeRgn = pEdgeRgn;
			rEI.pRgnR1 = pRootRgn1;
			rEI.pRgnR2 = pRootRgn2;

			if( rip1.IOL1 > rip1.EdgeIOL - nHalfAprSize && false == bAlwaysInner )
			{
				rEI.pRgnInrR1 = rEI.pRgnR1;
			}
			else
			{
				rEI.pRgnInrR1 = &m_rgnInfoVect[ m_conflictScanner->GetIOImgeOfIOLine( 
					rip1.EdgeIOL - nHalfAprSize ) ];
			}

			if( rip1.IOL2 > rip1.EdgeIOL + nHalfAprSize && false == bAlwaysInner )
			{
				rEI.pRgnInrR2 = rEI.pRgnR2;
			}
			else
			{
				rEI.pRgnInrR2 = &m_rgnInfoVect[ m_conflictScanner->GetIOImgeOfIOLine( 
					rip1.EdgeIOL + nHalfAprSize ) ];
			}

			rEI.Value = rip1.EdgeVal;

			rEI.ValueR1 = rip1.ValueR1;
			rEI.ValueR2 = rip1.ValueR2;

			rEI.Dir = a_dir;

			rEI.SetCanceled( false );

			rEI.bMax = true;

			rEI.bIntersected = false;

			//rEI.pRgnR1->bIsValleyRoot = true;
			//rEI.pRgnR2->bIsValleyRoot = true;

/*

			int nBgnIOL = rip1.IOL1 + 1;
			int nEndIOL = rip1.IOL2 - 1;


			//int nBgnIOL = rip1.EdgeIOL - nHalfAprSize + 1;
			//int nEndIOL = rip1.EdgeIOL + nHalfAprSize - 1;

			for(int j = nBgnIOL; j <= nEndIOL; j++ )
			{
				RgnInfo * pRgn = &m_rgnInfoVect[ m_conflictScanner->GetIOImgeOfIOLine( j ) ];

				pRgn->bInTrace = true;


				pRgn->edgeInfoArr[ (int) a_dir ] = &rEI;
			}

*/
		}
	}


	
	F32ImageRef RegionSegmentor23::GenLocalGradValImage()
	{
		F32ImageRef ret = this->m_src->Clone();

		for( int i=0; i < m_rgnInfoVect.GetSize(); i++ )
		{
			RgnInfo * pRgn = &m_rgnInfoVect[ i ];

			F32ColorVal * pix = (F32ColorVal *) ret->GetPixAt( pRgn->pos.x, pRgn->pos.y );

			//*pix = color1;

			//float val = pRgn->localGradVal;
			float val = pRgn->pMaxPostDif;			

			pix->AssignVal( val, val, val );
		}


		//ret = GenCvSmoothedImg( ret, 5 );


		return ret;
	}

	F32ImageRef RegionSegmentor23::GenLocalGradImage()
	{
		F32ColorVal colorEdgeArr[ 4 ];

		float maxVal = 0;

		{
			
			//colorEdgeArr[0].AssignVal( 0, 255, 0 );

			colorEdgeArr[0].AssignVal( 0, 200, 0 );
			colorEdgeArr[1].AssignVal( 255, 0, 0 );
			colorEdgeArr[2].AssignVal( 0, 0, 255 );
			colorEdgeArr[3].AssignVal( 0, 180, 255 );			
		}


		F32ImageRef ret = this->m_src->Clone();


		for( int i=0; i < m_rgnInfoVect.GetSize(); i++ )
		{
			RgnInfo * pRgn = &m_rgnInfoVect[ i ];

			//RgnLinkDir dir1;
			RgnLinkDir dirArr[ 2 ];

			int dirCnt = 0;

			for( int j=0; j < 4; j++ )
			{
				if( pRgn->localGradValArr[ j ] > 0.5 )
				{
					//dir1 = (RgnLinkDir) j;
					dirArr[ dirCnt ] = (RgnLinkDir) j;

					dirCnt++;
				}
			}

			//if( maxVal < pRgn->localGradVal )
			//	maxVal = pRgn->localGradVal;

			F32ColorVal color1;
			
			if( 0 == dirCnt )
			{
				color1.AssignVal( 0, 0, 0 );
			}
			else if( 1 == dirCnt )
			{
				//color1 = colorEdgeArr[ (int)dir1 ];
				color1 = colorEdgeArr[ (int)dirArr[ 0 ] ];				
			}
			else if( 2 == dirCnt )
			{
				//color1.AssignVal( 255, 255, 255 );

				color1 = F32ColorVal::Add(
					colorEdgeArr[ (int)dirArr[ 0 ] ],
					colorEdgeArr[ (int)dirArr[ 1 ] ] );

				color1.DividSelfBy( 2 );
			}
			else
				ThrowHcvException();

			

			float fact = 3;

			F32ColorVal * pix = (F32ColorVal *) ret->GetPixAt( pRgn->pos.x, pRgn->pos.y );

			*pix = color1;

/*
			pix->val0 = color1.val0 * pRgn->localGradVal * fact / 255;
			pix->val1 = color1.val1 * pRgn->localGradVal * fact / 255;
			pix->val2 = color1.val2 * pRgn->localGradVal * fact / 255;
*/
		}


		return ret;
	}




	void RegionSegmentor23::AssignTraceIntersections()
	{
		static int dxArr[] = {1, 1, 0, -1};
		static int dyArr[] = {0, 1, 1, 1};

		int nCurTraceProcID = 0;


		for( int k=0; k < m_edgeInfoVect.GetSize(); k++ )
		{
			EdgeInfo & rEI = m_edgeInfoVect[ k ];

			if( rEI.IsCanceled() )
				continue;



			RgnInfo * pRootRgn1 = rEI.pRgnR1;			
			RgnInfo * pRootRgn2 = rEI.pRgnR2;

			//RgnInfo * pRootRgn1 = rEI.pRgnInrR1;
			//RgnInfo * pRootRgn2 = rEI.pRgnInrR2;



			const RgnLinkDir dirOp = (RgnLinkDir) ((int) rEI.Dir + 4);

			const int dx = dxArr[ (int) rEI.Dir ];
			const int dy = dyArr[ (int) rEI.Dir ];


			nCurTraceProcID++;



			//RgnInfo * pRgn = this->GetPointRgn( 
						//pRootRgn1->pos.x + dx, pRootRgn1->pos.y + dy );
			//			rEI.pRgnR1->pos.x + dx, rEI.pRgnR1->pos.y + dy );

			RgnInfo * pRgn = rEI.pRgnR1;

			bool bFstInrDone = false;
			bool bSndInrDone = false;

			do
			{
/*				if( bFstInrDone && ! bSndInrDone )
				{
					pRgn->bInTrace = true;

					if( NULL != pRgn->pEdgeInfo )
					{
						 rEI.bIntersected = pRgn->pEdgeInfo->bIntersected = true;
					}

					pRgn->pEdgeInfo = &rEI;
				}
*/
				if( pRgn == rEI.pRgnInrR2 )
					bSndInrDone = true;

/*
				if( ! bSndInrDone )
				{
					m_traceRootAllocVect.IncSize();

					RgnInfoListElm * pElm = &m_traceRootAllocVect.GetBack();

					pElm->pRgn = pRootRgn1;

					pElm->pNext = pRgn->pTraceRootElm;
					pRgn->pTraceRootElm = pElm;
				}

				if( bFstInrDone )
				{
					m_traceRootAllocVect.IncSize();

					RgnInfoListElm * pElm = &m_traceRootAllocVect.GetBack();

					pElm->pRgn = pRootRgn2;

					pElm->pNext = pRgn->pTraceRootElm;
					pRgn->pTraceRootElm = pElm;
				}
*/

				if( pRgn == rEI.pRgnInrR1 )
					bFstInrDone = true;


				pRgn = this->GetPointRgn( 
							pRgn->pos.x + dx, pRgn->pos.y + dy );

			//}while( pRootRgn2 != pRgn );
			}while( rEI.pRgnR2 != pRgn );

			
		}


	}


	F32ImageRef RegionSegmentor23::GenMergeOrderImage()
	{
		F32ImageRef ret = this->m_src->Clone();

		//const float dspLim = ( 256 * 3 );
		const float dspLim = ( 256 );

		const float divFact = RgnInfo::s_MergeOrder / ( dspLim - 1 );

		for( int i=0; i < m_rgnInfoVect.GetSize(); i++ )
		{
			RgnInfo * pRgn = &m_rgnInfoVect[ i ];

			F32ColorVal * pix = (F32ColorVal *) ret->GetPixAt( pRgn->pos.x, pRgn->pos.y );


			const int nDspVal = (int) (pRgn->nMergeOrder / divFact);

			pix->val0 = nDspVal;
			pix->val1 = nDspVal;
			pix->val2 = nDspVal;


/*

			float val = nDspVal;

			if( nDspVal / 256 == 2 )			
			{
				pix->val1 = nDspVal % 256;
				pix->val2 = 255;
				pix->val0 = 255;
			}
			else
				pix->val1 = 0;


			if( nDspVal / 256 == 1 )			
			{
				pix->val2 = nDspVal % 256;
				pix->val0 = 255;
			}
			else
				pix->val2 = 0;


			if( nDspVal / 256 == 0 )			
				pix->val0 = nDspVal % 256;
*/
		}


		return ret;
	}



}