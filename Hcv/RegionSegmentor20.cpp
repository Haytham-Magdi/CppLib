#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\RegionSegmentor20.h>

namespace Hcv
{
	using namespace Hcpl;
	using namespace Hcpl::Math;

	//static MgrOfLinkTypes::CoreStuff m_core;
	//RegionSegmentor20::MgrOfLinkTypes::CoreStuff m_core;

	RegionSegmentor20::RegionSegmentor20(F32ImageRef a_src, S16ImageRef a_rootImg,
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

		m_medTraceQues.Init( m_linkActionMergeQues.GetNofQues() );


		m_edgeFwdRgnVect.SetCapacity(3000);

		m_edgeBkdRgnVect.SetCapacity(3000);
		

		InitRgnInfoVect();
		InitLinks();		


		//RgnInfo * pRgn81 = GetPointRgn(81, 271);

		m_rgnConflictVect.SetCapacity( 
			10000000);
		
		m_nofConflicts = 0;

		// Hthm Tmp
		//ProcessEdgeQues();


		ScanImageLines();
	}

	void RegionSegmentor20::InitRgnInfoVect(void)
	{
		CvSize srcSiz = m_src->GetSize();

		m_rgnInfoVect.SetSize(srcSiz.width * srcSiz.height);

		m_medTraceRgnPtrVect.SetCapacity( m_rgnInfoVect.GetSize() );


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

				

				{
					//int nRootImgVal = rootCh0->GetAt(x, y);

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

						pRgnInfo->showColor = *pRgnInfo->pixColors;
					}					

				}


				//pRgnInfo->bIsInConflict = false;

				pRgnInfo->accXYDistFromRoot = 0;
				pRgnInfo->maxAccXYDistFromRoot = -1;

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

	inline void RegionSegmentor20::CreateLink( RgnInfo * a_pRgn, 
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




	void RegionSegmentor20::InitLinks(void)
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

	S16ImageRef RegionSegmentor20::GenDifImg(bool a_bRC, bool a_bRB,
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



	
	HistoGramRef RegionSegmentor20::GenDifHisto(bool a_bRC, bool a_bRB,
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

	
	
	
	
	void RegionSegmentor20::Segment()
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




		//for(int e=0; e < 2; e++)
		for(int e=0; e < 1; e++)
		{
			for(int i=0; i < 3; i++)
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


	//	ValidLA :

					RgnInfo * pRootRgn1 = pLA->pRgn1->GetRootRgn();
					RgnInfo * pRootRgn2 = pLA->pRgn2->GetRootRgn();

					if( 1 != i )
					//if( 0 == i || e >= 1 )
					//if( 0 == i || e >= 1 )
					{
						if( NULL != pRootRgn1 && NULL != pRootRgn2 )
						{
							if( pRootRgn1 == pRootRgn2 )
								continue;


							//if( 0 == i )
							{

								cnt1++;

								float dist = CalcRgnDif( pRootRgn1, pRootRgn2 );

		/*						if( IsConflictDist(	dist ) )
								{		
									int nStepThr = 0;
									
									//if( pLA->pRgn1->nMaxDistMet >= nStepThr ||
									//	pLA->pRgn2->nMaxDistMet >= nStepThr )
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

						}
						//else if( NULL == pRootRgn1 && NULL == pRootRgn2 )
						else if( NULL == pRootRgn1 )
						{
							pRootRgn1 = pRootRgn1;
							// Bug

							//continue;
						}
						else		//	NULL == pRootRgn2
						{
							cnt2++;

							float accXYDistFR = pLA->pRgn1->accXYDistFromRoot +
								F32Point::Sub(pLA->pRgn1->pos, pLA->pRgn2->pos).CalcMag();

							if( pLA->pRgn2->IsInMidTrace() && 
								accXYDistFR > pLA->pRgn2->maxAccXYDistFromRoot &&
								m_linkActionMergeQues.GetLastMinIndex() < this->GetMaxDif() - 1 
								)
							{
								//m_linkActionMergeQues.PushPtr( this->GetMaxDif() - 1, pLA );
								//continue;
							}


							{
								int a = 0;


								if( 68 == pLA->pRgn2->pos.x && 86 == pLA->pRgn2->pos.y )
									a = 1;
							}


							
							//if( false )

							if( 2 == i )
							{
								bool bInspect = false;

								if( NULL != pLA->pRgn2->pTraceFwdRoot ||
									 NULL != pLA->pRgn2->pTraceBkdRoot )
									 bInspect = true;

								// Hthm Tmp
								//bInspect = false;

								if(bInspect)
								{
									// By Hthm Tmp								
									//continue;


/*
									if( NULL == pRootRgn1->pTraceFwdRoot )
									{
										continue;
									}
									else
									{
										goto NoTraceProblem;
									}
*/

									if( NULL == pLA->pRgn2->pTraceFwdRoot ||
										 NULL == pLA->pRgn2->pTraceBkdRoot )
									{
										 // Bug
										 bInspect = bInspect;
									}

									//if( e < 1 )
									////if( false )
									//{
									//	m_medTraceQues.PushPtr(
									//		m_linkActionMergeQues.GetLastMinIndex(), pLA );
									//	continue;
									//}

									UpdateActRgn( pRootRgn1 );
									RgnInfo * pActRgn1 = pRootRgn1->GetActRgn();

									UpdateActRgn( pLA->pRgn2->pTraceFwdRoot );
									RgnInfo * pActTFR = pLA->pRgn2->pTraceFwdRoot->GetActRgn();

									UpdateActRgn( pLA->pRgn2->pTraceBkdRoot );
									RgnInfo * pActTBR = pLA->pRgn2->pTraceBkdRoot->GetActRgn();

									if( pActTFR == NULL || pActTBR == NULL )
										 bInspect = bInspect;

									if( pActTFR != pActRgn1 && pActTBR != pActRgn1 )
										//&& pLA->pRgn2->IsRoot() == false && pLA->pRgn2->bInTrace)
										//&&  (m_linkActionMergeQues.GetLastMinIndex() < this->GetMaxDif() - 1))
									{

										////if( e < 1 )
										//if( false )
										//{
										//	m_medTraceQues.PushPtr(
										//		m_linkActionMergeQues.GetLastMinIndex(), pLA );
										//	continue;
										//}


										//m_linkActionMergeQues.PushPtr( this->GetMaxDif() - 1, pLA );
										continue;
									}
								}

							}


NoTraceProblem:


							pLA->pRgn2->accXYDistFromRoot = accXYDistFR;

							pLA->pRgn2->SetRootRgn( pRootRgn1 );

							//uuuuuuuuuuu

							UpdateActRgn( pRootRgn1 );
							RgnInfo * pActRgn1 = pRootRgn1->GetActRgn();

							pActRgn1->meanColors.IncBy( *pLA->pRgn2->pixColors );
							pActRgn1->nPixCnt++;

							float dist = m_linkActionMergeQues.GetLastMinIndex();
							if( dist > pLA->pRgn1->nMaxDistMet )
								pLA->pRgn2->nMaxDistMet = dist;
							
							PrepareRgnLinkActions( pLA->pRgn2, dist );
							continue;
						}

					}

					//else	//	i > 0
					if( 1 == i )
					{
						UpdateActRgn( pLA->pRgn1 );
						RgnInfo * pActRgn1 = pLA->pRgn1->GetActRgn();

						if( NULL == pLA->pRgn2->GetRootRgn() )
						{
							pLA->pRgn2->SetRootRgn( pLA->pRgn1->GetRootRgn() );
							PrepareRgnLinkActions( pLA->pRgn2, 0 );
							continue;
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

						int nTmpCnfCnt = pMaxSizRgn->conflictList.GetSize();
						//int nTmpCnfCnt = pMinSizRgn->conflictList.GetSize();

						if( nTmpCnfCnt > nMaxConfCnt )
							nMaxConfCnt = nTmpCnfCnt;

						nConfCntCnt ++;

						nConfCntSumMax += pMaxSizRgn->conflictList.GetSize();
						nConfCntSumMin += pMinSizRgn->conflictList.GetSize();

						if( 0 == pMinSizRgn->conflictList.GetSize() )
							nZeroConfCnt++;

					

						float dist0 = CalcRgnDif( pRootRgn1, pRootRgn2 );

	/*
						//if( dist0 > 7 * m_difScale )
						//if( dist0 > 4 * m_difScale )
						if( IsConflictDist(	dist0 ) )						
						{
							float dist1 = F32ColorVal::Sub(
								pActRgn1->meanColors.DividBy( pActRgn1->nPixCnt ),
								pActRgn2->meanColors.DividBy( pActRgn2->nPixCnt )
								).CalcMag() * m_difScale;


							if( IsConflictDist(	dist1 ) )
							//if( IsConflictDist(	dist1 / 2 ) )
							{		
								CreateConflict( pActRgn1, pActRgn2 );
								continue;
							}
						}
	*/



						if( HaveConflict( pMinSizRgn, pMaxSizRgn ) )
						//if(true)
						{
							continue;
						}
						else
						{
	//		NoConflict:

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


				if( 1 == i )
				{
					for(int k=0; k < m_medTraceRgnPtrVect.GetSize(); k++)
					{
						RgnInfo * pRgn = m_medTraceRgnPtrVect[k];
						if( false == pRgn->IsRoot() )
							pRgn->SetRootRgn( NULL );
					}
					

					for(int k=0; k < m_medTraceQues.GetNofQues(); k++)
					{
						//LinkAction * pLA = m_medTraceQues.PopPtr();
						LinkAction * pLA = m_medTraceQues.PopPtr(k);

						while( NULL != pLA )
						{
							//m_difQues.PushPtr(0, pLA);
							m_linkActionMergeQues.PushPtr(k, pLA);

							//pLA = m_medTraceQues.PopPtr();
							pLA = m_medTraceQues.PopPtr(k);
						}
					}
				}


				
			}

		}

		cnt1 = 0;
		cnt2 = 0;

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

	float RegionSegmentor20::CalcRgnDif(RgnInfo * a_pRgn1, RgnInfo * a_pRgn2)
	{
		float dist = m_difScale * F32ColorVal::Sub(
			*a_pRgn1->pixColors, *a_pRgn2->pixColors).CalcMag();

		return dist;
	}

	void RegionSegmentor20::UpdateActRgn(RgnInfo * a_pRgn)
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

	S16ImageRef RegionSegmentor20::GenSegmentedImage( bool a_bShowMeanColor )
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



	void RegionSegmentor20::ShowEdgeOfConflict()
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
	void RegionSegmentor20::PrepareConflicts()
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

	RegionSegmentor20::LinkAction * RegionSegmentor20::CloneLinkAction(LinkAction * a_pLA )
	{
		LinkAction * pLA = &m_linkActionVect[ m_nofLinkActions++ ];

		*pLA = *a_pLA;

		return pLA;
	}


	RegionSegmentor20::LinkAction * RegionSegmentor20::ProvideLinkAction(RgnInfo * a_pRgn, RgnLinkDir a_dir )
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


		float distNew;

		{		
			distNew = CalcRgnDif( a_pRgn->GetRootRgn(), pRgn2 );
			//distNew = a_rLink.DistMag;

			//if( distNew >= ( GetMaxDif() - 1 ) )
			//	distNew = distNew;
		}



		//a_rLink.DistMag = a_rLink2.DistMag = distNew;
		
		//a_rLink.bProcessed = a_rLink2.bProcessed = true;

		//m_difQues.PushPtr( a_rLink.DistMag, pLA);
		//m_linkActionMergeQues.PushPtr( distNew, pLA);
		



		return pLA;
	}



	void RegionSegmentor20::PrepareLinkAction( RgnInfo * a_pRgn, RgnInfo * a_pRgn2,
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

			//if( distNew >= ( GetMaxDif() - 1 ) )
			//	distNew = distNew;
		}



		//a_rLink.DistMag = a_rLink2.DistMag = distNew;
		
		//if( a_bSetProcessed )
			a_rLink.bProcessed = a_rLink2.bProcessed = true;



		//m_difQues.PushPtr( a_rLink.DistMag, pLA);
		m_linkActionMergeQues.PushPtr( distNew, pLA);
		
		
	}

	bool RegionSegmentor20::HaveConflict( RgnInfo * a_pMinSizRgn, RgnInfo * a_pMaxSizRgn)
	{
		RegionSegmentor20::RgnConflict * pConflict = GetExistingConflict( a_pMinSizRgn, a_pMaxSizRgn);

		if( NULL != pConflict )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	RegionSegmentor20::RgnConflict * RegionSegmentor20::GetExistingConflict( RgnInfo * a_pMinSizRgn, RgnInfo * a_pMaxSizRgn)
	{
		//bool bRet = false;

		APtrList< RgnConflict > & minConfList = 
			a_pMinSizRgn->conflictList;

		RegionSegmentor20::RgnConflict * pConflict = minConfList.Last();

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

	void RegionSegmentor20::RemoveDuplicateConflicts( RgnInfo * a_pRgn )
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


	void RegionSegmentor20::PrepareRgnLinkActions( RgnInfo * a_pRgn, float a_distBef )
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

	S16ImageRef RegionSegmentor20::GenConflictImg(bool a_bRC, bool a_bRB,
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


	void RegionSegmentor20::CreateConflict( RgnInfo * a_pRgn1, RgnInfo * a_pRgn2, RgnInfo * a_pOrgEdge )
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


	void RegionSegmentor20::ProcessEdgeQues()
	{
			LinkAction * pLA = NULL;

			do
			{
				pLA = m_linkActionEdgeQues.PopPtrMax(); 

				if( NULL == pLA )
				{
					continue;
				}

//				if(pLA->pLink1->bInTrace ||
//					pLA->pLink2->bInTrace)
				if( (pLA->pRgn1->bInTrace && !pLA->pRgn1->IsRoot())  ||
					(pLA->pRgn2->bInTrace && !pLA->pRgn2->IsRoot()) )
//				if( pLA->pRgn1->bInTrace  ||
//					pLA->pRgn2->bInTrace )
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



				TraceLinkActionRoots( pLA );

				ProcessLinkActionTrace(pLA);
/*
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
*/
			}while( NULL != pLA );	
	}


	void RegionSegmentor20::AddFwdTraceUnit(RgnInfo * a_pRgn, int a_nFwdIndex, int a_nBkdIndex)
	{
		TraceUnit tu;

		tu.pRgn = a_pRgn;		
		tu.pFwdLink = &a_pRgn->links[a_nFwdIndex];
		tu.pBkdLink = &a_pRgn->links[a_nBkdIndex];

		tu.Type = 'F';
		tu.Index = m_edgeFwdRgnVect.GetSize();

		int a = 0;

		if( 257 == a_pRgn->pos.x && 4 == a_pRgn->pos.y )
			a = 1;

		m_edgeFwdRgnVect.PushBack(tu);
	}

	void RegionSegmentor20::AddBkdTraceUnit(RgnInfo * a_pRgn, int a_nFwdIndex, int a_nBkdIndex)
	{
		TraceUnit tu;

		tu.pRgn = a_pRgn;		
		tu.pFwdLink = &a_pRgn->links[a_nFwdIndex];
		tu.pBkdLink = &a_pRgn->links[a_nBkdIndex];

		tu.Type = 'B';
		tu.Index = m_edgeBkdRgnVect.GetSize();

		int a = 0;

		if( 257 == a_pRgn->pos.x && 4 == a_pRgn->pos.y )
			a = 1;

		m_edgeBkdRgnVect.PushBack(tu);
	}

	void RegionSegmentor20::PrepareTraceListPtrs()
	{
		{
			m_edgeFwdRgnVect[0].pBkdTU = &m_edgeBkdRgnVect[0];

			for(int i=0; i < m_edgeFwdRgnVect.GetSize()-1; i++)
			{
				m_edgeFwdRgnVect[i].pFwdTU = &m_edgeFwdRgnVect[i+1];
				m_edgeFwdRgnVect[i+1].pBkdTU = &m_edgeFwdRgnVect[i];
			}
			m_edgeFwdRgnVect.GetBack().pFwdTU = NULL;
		}



		{
			m_edgeBkdRgnVect[0].pFwdTU = &m_edgeFwdRgnVect[0];

			for(int i=0; i < m_edgeBkdRgnVect.GetSize()-1; i++)
			{
				m_edgeBkdRgnVect[i].pBkdTU = &m_edgeBkdRgnVect[i+1];
				m_edgeBkdRgnVect[i+1].pFwdTU = &m_edgeBkdRgnVect[i];
			}
			m_edgeBkdRgnVect.GetBack().pBkdTU = NULL;
		}


	}


	void RegionSegmentor20::TraceLinkActionRoots( LinkAction * a_pLA)
		//RgnInfo ** a_ppRoot1, RgnInfo ** a_ppRoot2 )
	{
		m_edgeFwdRgnVect.ResetSize();
		m_edgeBkdRgnVect.ResetSize();

		//RgnInfo * pFwdRgn = a_pLA->pRgn1;
		//RgnInfo * pBkdRgn = a_pLA->pRgn2;

		RgnInfo * pBkdRgn = a_pLA->pRgn1;
		RgnInfo * pFwdRgn = a_pLA->pRgn2;

		//a_pLA->bShowTrace = ((rand() % 50) == 0);
		//a_pLA->bShowTrace = ((rand() % 20) == 0);
		//a_pLA->bShowTrace = ((rand() % 5) == 0);
		//a_pLA->bShowTrace = true;
		a_pLA->bShowTrace = false;



		int fwdLinkIndex = (int)a_pLA->pLink1->dir;
		int bkdLinkIndex = MgrOfLinkTypes::GetCore()->GetInverseLinkIndex(
			fwdLinkIndex);		


		//int bkdLinkIndex = (int)a_pLA->pLink1->dir;
		//int fwdLinkIndex = MgrOfLinkTypes::GetCore()->GetInverseLinkIndex(
		//	bkdLinkIndex);		

		AddFwdTraceUnit(pFwdRgn, fwdLinkIndex, bkdLinkIndex);

		//tu.pRgn = pFwdRgn;		
		//m_edgeFwdRgnVect.PushBack(tu);

		if(pFwdRgn->bInTrace)
			a_pLA->bIntersected = true;			
		

		AddBkdTraceUnit(pBkdRgn, fwdLinkIndex, bkdLinkIndex);

		//tu.pRgn = pBkdRgn;
		//m_edgeBkdRgnVect.PushBack(tu);

		if(pBkdRgn->bInTrace)
			a_pLA->bIntersected = true;			


		RgnInfo * pNextFwdRgn = NULL;
		RgnInfo * pNextBkdRgn = NULL;

		int nextFwdLinkIndex = -1;
		int nextBkdLinkIndex = -1;

		F32ColorVal unitColorDist = a_pLA->pLink1->UnitDist;

		bool bFwdFinished = false;
		bool bBkdFinished = false;

		//F32ColorVal traceColor = {0, 255, 0};
		//F32ColorVal traceColor = {0, 0, 255};

		
		float fwdDistMag = pFwdRgn->links[ fwdLinkIndex ].DistMag;
		float bkdDistMag = pBkdRgn->links[ bkdLinkIndex ].DistMag;

		do
		{
			if( bFwdFinished != true)
			{
				float minUcdDif = 1000000;
				int minLinkIndex = fwdLinkIndex;

				//for(int i=0; i < 3; i++)
				//for(int i=0; i < 5; i++)
				for(int i=0; i < 1; i++)
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
				//if(minUcdDif > 0.3)
				//if(minUcdDif > 0.1)
				//if(minUcdDif > 0.9)
				{
					bFwdFinished = true;

					// TmpTest
					//break;
				}
				else
				{
					//nextFwdLinkIndex = minLinkIndex;
					//pNextFwdRgn = pFwdRgn->links[nextFwdLinkIndex].pPeerRgn;

					RgnLink & rLink1 = pFwdRgn->links[minLinkIndex];

					if( rLink1.DistMag > fwdDistMag )
					{
//						bFwdFinished = true;
//						continue;

						//a_pLA->bActionCanceled = true;
						//return;
					}
					else
					{
						//fwdDistMag = rLink1.DistMag;
					}


					fwdLinkIndex = minLinkIndex;


					pFwdRgn = rLink1.pPeerRgn;
					rLink1.bInTrace = true;

					AddFwdTraceUnit(pFwdRgn, minLinkIndex, GetInverseLinkIndex( minLinkIndex ));

					//tu.pRgn = pFwdRgn;
					//m_edgeFwdRgnVect.PushBack(tu);

					if(pFwdRgn->bInTrace)
						a_pLA->bIntersected = true;			



				}
			}



			if( bBkdFinished != true)
			{
				float minUcdDif = 1000000;
				int minLinkIndex = bkdLinkIndex;

				//for(int i=0; i < 3; i++)
				//for(int i=0; i < 5; i++)
				for(int i=0; i < 1; i++)
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
				//if(minUcdDif > 0.3)
				//if(minUcdDif > 0.1)
				//if(minUcdDif > 0.9)
				{
					bBkdFinished = true;

					// TmpTest
					//break;
				}
				else
				{
					//nextBkdLinkIndex = minLinkIndex;
					//pNextBkdRgn = pBkdRgn->links[nextBkdLinkIndex].pPeerRgn;

					RgnLink & rLink1 = pBkdRgn->links[minLinkIndex];

					if( rLink1.DistMag > bkdDistMag )
					{
//						bBkdFinished = true;
//						continue;

						//a_pLA->bActionCanceled = true;
						//return;
					}
					else
					{
						//bkdDistMag = rLink1.DistMag;
					}


					bkdLinkIndex = minLinkIndex;
					pBkdRgn = rLink1.pPeerRgn;
					rLink1.bInTrace = true;

					AddBkdTraceUnit(pBkdRgn, minLinkIndex, GetInverseLinkIndex( minLinkIndex ));

					//tu.pRgn = pBkdRgn;
					//m_edgeBkdRgnVect.PushBack(tu);

					if(pBkdRgn->bInTrace)
						a_pLA->bIntersected = true;			
				}
			}

			if( bFwdFinished == true && bBkdFinished == true )
			{
				break;
			}
			else
			{
/*				unitColorDist = F32ColorVal::Sub(
					*pFwdRgn->pixColors, *pBkdRgn->pixColors);

				unitColorDist.DividSelfBy( unitColorDist.CalcMag() );*/
			}

		//}while( bFwdFinished == false && bBkdFinished == false );
		}while( true );

		if( a_pLA->bIntersected )
			a_pLA->bShowTrace = false;
	}



	void RegionSegmentor20::ProcessLinkActionTrace( LinkAction * a_pLA )
	{
		if(a_pLA->bActionCanceled)
			return;

		RgnInfo * pFwdRoot = m_edgeFwdRgnVect.GetBack().pRgn;
		RgnInfo * pBkdRoot = m_edgeBkdRgnVect.GetBack().pRgn;


		if( 
			( (pFwdRoot->IsRoot() == false) && (pFwdRoot->bInTrace) ) ||
			( (pBkdRoot->IsRoot() == false) && (pBkdRoot->bInTrace) )
//			( (pFwdRoot->bInTrace) ) ||
//			( (pBkdRoot->bInTrace) )
			)
		{
			return;
		}


		//const bool bShowTrace = ((rand() % 100) == 0);
		//const bool bShowTrace = ((rand() % 50) == 0);
		//const bool bShowTrace = true;
		//const bool bShowTrace = false;
		const bool bShowTrace = a_pLA->bShowTrace;

		



		//F32ColorVal traceColor = {0, 255, 0};
		//F32ColorVal traceColor = {0, 0, 255};

		const int nofColors = 6;

		F32ColorVal colors[nofColors] = {
									//{0, 255, 0},
									//{255, 0, 0},

									//{0, 255, 255},
									//{128, 128, 0}

									{0, 255, 0},
									{70, 0, 0},
									{0, 0, 150},

									{70, 255, 0},
									{0, 255, 150},
									{70, 0, 150}
		};

		F32ColorVal traceColorF = {0, 255, 0};
		F32ColorVal traceColorB = {255, 0, 0};


		F32ColorVal traceColorF2 = {0, 255, 255};
		F32ColorVal traceColorB2 = {255, 255, 0};

		F32ColorVal tmpColor;

		tmpColor = *pFwdRoot->pixColors;
		{
			int maxIndex = 0;
			float maxDist = F32ColorVal::Sub(tmpColor, colors[ maxIndex ]).CalcMag();

			for(int i=0; i < nofColors; i++)
			{
				float dist0 = F32ColorVal::Sub(tmpColor, colors[ i ]).CalcMag();
				if( dist0 > maxDist )
				{
					maxIndex = i;
					maxDist = dist0;
				}
			}

			traceColorF2 = colors[ maxIndex ];
		}

		/*     
		tmpColor = *pBkdRoot->pixColors;
		{
			int maxIndex = 0;
			float maxDist = F32ColorVal::Sub(tmpColor, colors[ maxIndex ]).CalcMag();

			for(int i=0; i < nofColors; i++)
			{
				float dist0 = F32ColorVal::Sub(tmpColor, colors[ i ]).CalcMag();
				if( dist0 > maxDist )
				{
					maxIndex = i;
					maxDist = dist0;
				}
			}

			traceColorB2 = colors[ maxIndex ];
		}
*/
		traceColorB2 = F32ColorVal::Add( F32ColorVal::FromNum(255),
			traceColorF2.MultBy(-1) );




		float dist = CalcRgnDif( pFwdRoot, pBkdRoot );


		//if( IsConflictDist( dist * 2 ) )
		if( IsConflictDist( dist ) )
		//if( dist >= 20 )
		{
			PrepareTraceListPtrs();

			pFwdRoot->bInTrace = true;
			pBkdRoot->bInTrace = true;

/*
			if(bShowTrace)
			{
				for(int i=0; i < m_edgeFwdRgnVect.GetSize()-1; i++)
				{
					RgnInfo * pFwdRgn = m_edgeFwdRgnVect[i].pRgn;

					//if(false == pFwdRgn->bShowLocalColor)
					{
						pFwdRgn->bShowLocalColor = true;
						//pFwdRgn->showColor = traceColorF;
						pFwdRgn->showColor = traceColorF2;
					}
				}

				for(int i=0; i < m_edgeBkdRgnVect.GetSize()-1; i++)
				{
					RgnInfo * pBkdRgn = m_edgeBkdRgnVect[i].pRgn;

					//if(false == pBkdRgn->bShowLocalColor)
					{
						pBkdRgn->bShowLocalColor = true;
						//pBkdRgn->showColor = traceColorB;
						pBkdRgn->showColor = traceColorB2;
					}
				}
			}

			if(bShowTrace)
			{
				F32ColorVal traceLimColor = { 0, 0, 255 };

				pFwdRoot->bShowLocalColor = true;
				//pFwdRoot->showColor = traceColor;
				//pFwdRoot->showColor = traceLimColor;
				//pFwdRoot->showColor = traceColorF;
				pFwdRoot->showColor = traceColorF2;

				pBkdRoot->bShowLocalColor = true;
				//pBkdRoot->showColor = traceColor;
				//pBkdRoot->showColor = traceLimColor;
				//pBkdRoot->showColor = traceColorB;
				pBkdRoot->showColor = traceColorB2;
			}
*/



		//float dist = CalcRgnDif( pFwdRoot, pBkdRoot );
			//pLA->pLink1->DistMag;

			pFwdRoot->BeARoot();					
			pBkdRoot->BeARoot();

			CreateConflict( pFwdRoot, pBkdRoot );


//	nnnnnnnnnnnnn

			{
				TraceUnit * pFwdRootTU = &m_edgeFwdRgnVect.GetBack();
				TraceUnit * pBkdRootTU = &m_edgeBkdRgnVect.GetBack();

				TraceUnit * pFwdBgn2 = NULL;

				if(m_edgeFwdRgnVect.GetSize() > 1)
				{
					pFwdBgn2 = &m_edgeFwdRgnVect[0];
				}
				else if(m_edgeBkdRgnVect.GetSize() > 1)
				{
					pFwdBgn2 = &m_edgeBkdRgnVect[0];
				}
				else
				{
					goto EndClassification;
				}


				float distFwd, distBkd; 

				distFwd = CalcRgnDif(pFwdRoot, pFwdBgn2->pRgn);
				distBkd = CalcRgnDif(pBkdRoot, pFwdBgn2->pRgn);

				if( distFwd > distBkd )
				{
					//	Moving Fwd

					while(true)
					{
						if( pFwdBgn2 == pFwdRootTU )
							break;

						if( distFwd <= distBkd )
							break;

						pFwdBgn2 = pFwdBgn2->pFwdTU;

						distFwd = CalcRgnDif(pFwdRoot, pFwdBgn2->pRgn);
						distBkd = CalcRgnDif(pBkdRoot, pFwdBgn2->pRgn);
					}

				}
				else
				{
					//	Moving Bkd

					while(true)
					{
						if( pFwdBgn2->pBkdTU == pBkdRootTU )
							break;

						if( distFwd > distBkd )
							break;

						pFwdBgn2 = pFwdBgn2->pBkdTU;

						distFwd = CalcRgnDif(pFwdRoot, pFwdBgn2->pRgn);
						distBkd = CalcRgnDif(pBkdRoot, pFwdBgn2->pRgn);
					}
				}


/*
				for(int i=m_edgeFwdRgnVect.GetSize()-1; i >= 1; i--)
				{
					TraceUnit * pTU1 = &m_edgeFwdRgnVect[i];
					TraceUnit * pTU2 = &m_edgeFwdRgnVect[i-1];

					LinkAction * pLA = &m_linkActionVect[ m_nofLinkActions++ ];

					pLA->pRgn1 = pTU1->pRgn;
					pLA->pRgn2 = pTU2->pBkdTU->pRgn;
					pLA->pLink1 = pTU1->pBkdLink;
					pLA->pLink2 = pTU2->pFwdLink;
					pLA->pPrev = pLA;

					//m_linkActionEdgeQues.PushPtr( 0, pLA);
					m_linkActionMergeQues.PushPtr( 0, pLA);
				}

				for(int i=m_edgeBkdRgnVect.GetSize()-1; i >= 1; i--)
				{
					TraceUnit * pTU1 = &m_edgeBkdRgnVect[i];
					TraceUnit * pTU2 = &m_edgeBkdRgnVect[i-1];

					LinkAction * pLA = &m_linkActionVect[ m_nofLinkActions++ ];

					pLA->pRgn1 = pTU1->pRgn;
					pLA->pRgn2 = pTU2->pFwdTU->pRgn;
					pLA->pLink1 = pTU1->pFwdLink;
					pLA->pLink2 = pTU2->pBkdLink;
					pLA->pPrev = pLA;

					//m_linkActionEdgeQues.PushPtr( 0, pLA);
					m_linkActionMergeQues.PushPtr( 0, pLA);
				}
*/


				if( pFwdRootTU->pRgn != pFwdRoot )
				{
					// Bug
					pFwdRoot = pFwdRoot;
				}

				if( pBkdRootTU->pRgn != pBkdRoot )
				{
					// Bug
					pBkdRoot = pBkdRoot;
				}

				//TmpTest
				//pFwdBgn2 = &m_edgeFwdRgnVect[0];

				TraceUnit * pBkdBgn2 = pFwdBgn2->pBkdTU;

				

				TraceUnit * pTU = NULL;
				float accXYDistFR = -1;
				RgnInfo * pRoot1 = NULL;

				F32ColorVal color1 = { 0, 255, 255 };
				bool bColorMidTrace = false;

///*	
				float scale1 = 5;

				pTU = pFwdRootTU;
				accXYDistFR = 0;

				pRoot1 = pTU->pRgn;

				while( pTU != pFwdBgn2 )
				{
					TraceUnit * pTU2 = pTU->pBkdTU;

					if(bShowTrace)
					{
						pTU2->pRgn->bShowLocalColor = true;
						pTU2->pRgn->showColor = traceColorF;
					}

					//if(pTU2->pRgn->bInTrace)
					//	break;

					//if( a_pLA->bIntersected == false )
					//{
					//	if( NULL != pTU2->pRgn->GetRootRgn() )
					//	{
					//		//pRoot1 = pTU2->pRgn->GetRootRgn();
					//		break;
					//	}
					//	else
					//	{
					//		pTU2->pRgn->SetRootRgn( pRoot1 );
					//		PrepareRgnLinkActions( pTU2->pRgn, 0 );
					//	}
					//}


					//LinkAction * pLA = &m_linkActionVect[ m_nofLinkActions++ ];

					//pLA->pRgn1 = pTU->pRgn;
					//pLA->pRgn2 = pTU2->pRgn;
					//pLA->pLink1 = pTU->pBkdLink;
					//pLA->pLink2 = pTU2->pFwdLink;
					//pLA->pPrev = pLA;

					//m_linkActionMergeQues.PushPtr( 0, pLA);

					accXYDistFR += F32Point::Sub(pTU->pRgn->pos, 
						pTU2->pRgn->pos).CalcMag();

					if( scale1 * accXYDistFR > pTU2->pRgn->maxAccXYDistFromRoot )
						pTU2->pRgn->maxAccXYDistFromRoot = scale1 * accXYDistFR;

					//if( bColorMidTrace )
					//{
					//	pTU2->pRgn->bShowLocalColor = true;
					//	pTU2->pRgn->showColor = color1;
					//}

					pTU = pTU2;
				}


				pTU = pBkdRootTU;
				accXYDistFR = 0;

				pRoot1 = pTU->pRgn;

				while( pTU != pBkdBgn2 )
				{
					TraceUnit * pTU2 = pTU->pFwdTU;

					if(bShowTrace)
					{
						pTU2->pRgn->bShowLocalColor = true;
						pTU2->pRgn->showColor = traceColorB;
					}

					//if(pTU2->pRgn->bInTrace)
					//	break;

					//if( a_pLA->bIntersected == false )
					//{
					//	if( NULL != pTU2->pRgn->GetRootRgn() )
					//	{
					//		//pRoot1 = pTU2->pRgn->GetRootRgn();
					//		break;
					//	}
					//	else
					//	{
					//		pTU2->pRgn->SetRootRgn( pRoot1 );
					//		PrepareRgnLinkActions( pTU2->pRgn, 0 );
					//	}
					//}


					//LinkAction * pLA = &m_linkActionVect[ m_nofLinkActions++ ];

					//pLA->pRgn1 = pTU->pRgn;
					//pLA->pRgn2 = pTU2->pRgn;
					//pLA->pLink1 = pTU->pFwdLink;
					//pLA->pLink2 = pTU2->pBkdLink;
					//pLA->pPrev = pLA;

					//m_linkActionMergeQues.PushPtr( 0, pLA);

					accXYDistFR += F32Point::Sub(pTU->pRgn->pos, 
						pTU2->pRgn->pos).CalcMag();

					if( scale1 *  accXYDistFR > pTU2->pRgn->maxAccXYDistFromRoot )
						pTU2->pRgn->maxAccXYDistFromRoot = scale1 *  accXYDistFR;

					//if( bColorMidTrace )
					//{
					//	pTU2->pRgn->bShowLocalColor = true;
					//	pTU2->pRgn->showColor = color1;
					//}

					pTU = pTU2;

					// ttttttttttmp
					pTU->pRgn->bInTrace = true;
				}

//*/


EndClassification:
				;
			}


			PrepareRgnLinkActions(pFwdRoot, 0);
			PrepareRgnLinkActions(pBkdRoot, 0);


			float x0 = -100;
			float y0 = -100;

			static bool bFstFlg1 = true;

			if(bFstFlg1)
			{
				static char buff[100];

				//buff[0] = 0;
				//IOMgr::ReadString("x1", buff);
				//x0 = atof(buff);

				x0 = IOMgr::ReadFloat("x1");

				//buff[0] = 0;
				//IOMgr::ReadString("y1", buff);
				//y0 = atof(buff);

				y0 = IOMgr::ReadFloat("y1");
			}


			//const float x1 = 68;
			//const float y1 = 86;

			const float x1 = 1014;
			const float y1 = 108;

			//const float x1 = x0;
			//const float y1 = y0;


			static int st_traceID = 1;

			if( a_pLA->bIntersected == false )
			{
				st_traceID++;
			}

			//int traceID0 = 583;
			int traceID0 = -10;
			if( a_pLA->bIntersected == false )
			{
				{
					//for(int i=0; i < m_edgeFwdRgnVect.GetSize()-1; i++)
					for(int i=0; i < m_edgeFwdRgnVect.GetSize(); i++)
					{
						F32Point pos = m_edgeFwdRgnVect[i].pRgn->pos;

						if( x1 == pos.x && y1 == pos.y )
							traceID0 = st_traceID;
					}

					//for(int i=0; i < m_edgeBkdRgnVect.GetSize()-1; i++)
					for(int i=0; i < m_edgeBkdRgnVect.GetSize(); i++)
					{
						F32Point pos = m_edgeBkdRgnVect[i].pRgn->pos;

						if( x1 == pos.x && y1 == pos.y )
							traceID0 = st_traceID;
					}
				}
			}



			const int traceID1 = traceID0;




			for(int i=m_edgeFwdRgnVect.GetSize()-2; i >= 0 ; i--)
			{
				RgnInfo * pFwdRgn = m_edgeFwdRgnVect[i].pRgn;

				pFwdRgn->bInTrace = true;

				if( a_pLA->bIntersected == false )
				{
					pFwdRgn->pTraceFwdRoot = pFwdRoot;
					pFwdRgn->pTraceBkdRoot = pBkdRoot;
					pFwdRgn->nTraceID = st_traceID;
				}

				if( traceID1 == st_traceID )
				{
					F32ColorVal c1 = { 0, 255, 0 };

					pFwdRgn->bShowLocalColor = true;
					pFwdRgn->showColor = c1;
				}

				//if( pFwdRgn->GetRootRgn() != NULL )
					//break;

				//RgnInfo * pRoot1 = NULL;

				//if( CalcRgnDif( pFwdRoot, pFwdRgn) <=
				//	CalcRgnDif( pBkdRoot, pFwdRgn) )
/*  //mmmmmmmmmmm
				float distF = CalcRgnDif( pFwdRoot, pFwdRgn);
				float distB = CalcRgnDif( pBkdRoot, pFwdRgn);

				//if( dist1 < pFwdRgn->distMagFromRoot )
				if( distF < distB )
				{
					float dist1 = distF;
					pFwdRgn->SetRootRgn( pFwdRoot );
					pFwdRgn->distMagFromRoot = dist1;
					PrepareRgnLinkActions( pFwdRgn, 0 );
					if(bShowTrace)
						pFwdRgn->showColor = traceColorF2;
				}				
				//if( dist1 < pFwdRgn->distMagFromRoot ) 
				else
				{
					float dist1 = distB;
					pFwdRgn->SetRootRgn( pBkdRoot );
					pFwdRgn->distMagFromRoot = dist1;
					PrepareRgnLinkActions( pFwdRgn, 0 );
					if(bShowTrace)
						pFwdRgn->showColor = traceColorB2;
				}
*/

				

				//float dist = m_linkActionMergeQues.GetLastMinIndex();
				//if( dist > pLA->pRgn1->nMaxDistMet )
					//pLA->pRgn2->nMaxDistMet = dist;
				
				//PrepareRgnLinkActions( pFwdRgn, dist );
			}

			for(int i=m_edgeBkdRgnVect.GetSize()-2; i >= 0 ; i--)
			{
				RgnInfo * pBkdRgn = m_edgeBkdRgnVect[i].pRgn;

				pBkdRgn->bInTrace = true;

				if( a_pLA->bIntersected == false )
				{
					pBkdRgn->pTraceFwdRoot = pFwdRoot;
					pBkdRgn->pTraceBkdRoot = pBkdRoot;
				}

				if( traceID1 == st_traceID )
				{
					F32ColorVal c1 = { 0, 255, 0 };

					pBkdRgn->bShowLocalColor = true;
					pBkdRgn->showColor = c1;
				}

				//if( pBkdRgn->GetRootRgn() != NULL )
					//break;

				//RgnInfo * pRoot1 = NULL;

/*			//mmmmmmmmmmm

				float distF = CalcRgnDif( pFwdRoot, pBkdRgn);
				float distB = CalcRgnDif( pBkdRoot, pBkdRgn);

				
				//if( dist1 < pBkdRgn->distMagFromRoot )
				if( distF < distB )
				{
					float dist1 = distF;
					pBkdRgn->SetRootRgn( pFwdRoot );
					pBkdRgn->distMagFromRoot = dist1;
					PrepareRgnLinkActions( pBkdRgn, 0 );
					if(bShowTrace)
						pBkdRgn->showColor = traceColorF2;
				}
				//if( dist1 < pBkdRgn->distMagFromRoot ) 
				else
				{
					float dist1 = distB;
					pBkdRgn->SetRootRgn( pBkdRoot );
					pBkdRgn->distMagFromRoot = dist1;
					PrepareRgnLinkActions( pBkdRgn, 0 );
					if(bShowTrace)
						pBkdRgn->showColor = traceColorB2;
				}
*/

				//float dist = m_linkActionMergeQues.GetLastMinIndex();
				//if( dist > pLA->pRgn1->nMaxDistMet )
					//pLA->pRgn2->nMaxDistMet = dist;
				
				//PrepareRgnLinkActions( pBkdRgn, dist );
			}


			if( traceID1 == st_traceID )
			{
				F32ColorVal c2 = { 0, 0, 255 };
				F32ColorVal c3 = { 0, 255, 255 };

				pFwdRoot->bShowLocalColor = true;
				pFwdRoot->showColor = c2;

				pBkdRoot->bShowLocalColor = true;
				pBkdRoot->showColor = c2;

				m_edgeFwdRgnVect[0].pRgn->bShowLocalColor = true;
				m_edgeFwdRgnVect[0].pRgn->showColor = c3;

				m_edgeBkdRgnVect[0].pRgn->bShowLocalColor = true;
				m_edgeBkdRgnVect[0].pRgn->showColor = c3;
			}



		}





	}



//////////////////////////////////////////////////////////////


	void RegionSegmentor20::MgrOfLinkTypes::CoreStuff::InitLinkTypeMgrs()
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

	void RegionSegmentor20::MgrOfLinkTypes::CoreStuff::InitMaps()
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



	void RegionSegmentor20::ScanImageLines()
	{
		CvSize siz1 = m_src->GetSize();

		m_conflictScanner = new ConflictScanner( m_src );

		bool bDoHorz = true;
		bool bDoVert = true;

/////////  Horz

		if( bDoHorz )
		{

			//for(int y=0; y < siz1.height; y++)
			//for(int y=0; y < siz1.height; y+=50)

			//for(int y=0, k=0; y < siz1.height && k < 5; y += 50, k++)			
			//for(int y=200, k=0; y < siz1.height && k < 1; y += 50, k++)			
			//for(int y=0, k=0; y <= 200 && k < 100; y += 20, k++)			
			for(int y=0, k=0; y < siz1.height && k < 10000; y += 1, k++)			
			
			//for(int y=150, k=0; y < siz1.height && k < 2; y += 50, k++)			
			//for(int y=0, k=0; y < siz1.height && k < 14; y += 50, k++)			
			//for(int y=0, k=0; y < siz1.height && k < 40000; y += 1, k++)			
			//for(int y= siz1.height / 2, k=0; y < siz1.height && k < 2; y += 50, k++)			
			//for(int y=0; y < 4; y++)
			//for(int y=0; y < siz1.height; y += siz1.height / 20)
			{
				//LinePathRef lp = new LinePath( 0, siz1.height / 2, siz1.width - 1, siz1.height / 2 );
				LinePathRef lp = new LinePath( 0, y, siz1.width - 1, y );


		
				//m_conflictScanner = new ConflictScanner( m_src );
				m_conflictScanner->ProcessLine( lp );

				//FiltSysDebug::MoveNext();



				for( int i=0; i < m_conflictScanner->GetNofConflicts(); i++ )
				{
					ConflictScanner::RootIOLPair rip1 = m_conflictScanner->GetConflictPairAt( i );

					//int siz21 = m_conflictScanner->GetIOImgeOfIOLine( rip1.IOL1 );
					//int siz22 = m_rgnInfoVect.GetSize();

					RgnInfo * pEdgeRgn = &m_rgnInfoVect[ m_conflictScanner->GetIOImgeOfIOLine( rip1.EdgeIOL ) ];
					RgnInfo * pRootRgn1 = &m_rgnInfoVect[ m_conflictScanner->GetIOImgeOfIOLine( rip1.IOL1 ) ];
					RgnInfo * pRootRgn2 = &m_rgnInfoVect[ m_conflictScanner->GetIOImgeOfIOLine( rip1.IOL2 ) ];

					pRootRgn1->BeARoot();					
					pRootRgn2->BeARoot();

					if( 0 == rand() % 10 )
						m_pAnyRoot = pRootRgn1;


					CreateConflict( pRootRgn1, pRootRgn2, pEdgeRgn );

					{
						int nBgnIOL = rip1.IOL1 + 1;
						//int nEndIOL = rip1.IOL2 - 1;
						int nEndIOL = - 1;

						int nMidIOL = ( nBgnIOL + nEndIOL ) / 2;

						RgnInfo * pTfr = NULL;
						RgnInfo * pTbr = NULL;

						int i;
						for( i=nBgnIOL; i <= nEndIOL; i++ )
						{
							RgnInfo * pRgn = &m_rgnInfoVect[ m_conflictScanner->GetIOImgeOfIOLine( i ) ];

							m_medTraceRgnPtrVect.PushBack( pRgn );

/*
							if( NULL != pRgn->pTraceFwdRoot )
							{
								pTfr = pRgn->pTraceFwdRoot;
								pTbr = pRgn->pTraceBkdRoot;

								break;
							}
*/
							if( false == pRgn->IsRoot() )
							{
								pRgn->pTraceFwdRoot = pRootRgn1;
								pRgn->pTraceBkdRoot = pRootRgn2;
							}


							if( pRgn->IsRoot() )
								pRgn = pRgn;

							RgnInfo * pRootRgnA = NULL; 

							if( i < nMidIOL )
								pRootRgnA = pRootRgn1; 
							else
								pRootRgnA = pRootRgn2; 

							pRgn->SetRootRgn( pRootRgnA );

							

							
							//UpdateActRgn( pRootRgn1 );
							//RgnInfo * pActRgn1 = pRootRgn1->GetActRgn();

							//pActRgn1->meanColors.IncBy( pRgn->pixColors );
							//pActRgn1->nPixCnt++;
						
							//PrepareRgnLinkActions( pRgn, 0 );

						}

						if( i <= nEndIOL )
						{
							//for( int j = nBgnIOL; j < i; j++ )
							for( int j = nBgnIOL; j <= nEndIOL; j++ )
							{
								RgnInfo * pRgn = &m_rgnInfoVect[ m_conflictScanner->GetIOImgeOfIOLine( j ) ];

								pRgn->pTraceFwdRoot = pTfr;
								pRgn->pTraceBkdRoot = pTbr;
							}
						}

					}

					PrepareRgnLinkActions( pRootRgn1, 0 );
					PrepareRgnLinkActions( pRootRgn2, 0 );

				}
				


			}

		}


/////////  Vert


		if( bDoVert )
		{

			for(int x=0; x < siz1.width; x ++)			
			{
				LinePathRef lp = new LinePath( x, 0, x, siz1.height - 1 );

		
				//m_conflictScanner = new ConflictScanner( m_src );
				m_conflictScanner->ProcessLine( lp );

				//FiltSysDebug::MoveNext();



				for( int i=0; i < m_conflictScanner->GetNofConflicts(); i++ )
				{
					ConflictScanner::RootIOLPair rip1 = m_conflictScanner->GetConflictPairAt( i );

					//int siz21 = m_conflictScanner->GetIOImgeOfIOLine( rip1.IOL1 );
					//int siz22 = m_rgnInfoVect.GetSize();

					RgnInfo * pEdgeRgn = &m_rgnInfoVect[ m_conflictScanner->GetIOImgeOfIOLine( rip1.EdgeIOL ) ];
					RgnInfo * pRootRgn1 = &m_rgnInfoVect[ m_conflictScanner->GetIOImgeOfIOLine( rip1.IOL1 ) ];
					RgnInfo * pRootRgn2 = &m_rgnInfoVect[ m_conflictScanner->GetIOImgeOfIOLine( rip1.IOL2 ) ];

					pRootRgn1->BeARoot();					
					pRootRgn2->BeARoot();

					if( 0 == rand() % 10 )
						m_pAnyRoot = pRootRgn1;


					CreateConflict( pRootRgn1, pRootRgn2, pEdgeRgn );

					{
						int nBgnIOL = rip1.IOL1 + 1;
						//int nEndIOL = rip1.IOL2 - 1;
						int nEndIOL = - 1;

						int nMidIOL = ( nBgnIOL + nEndIOL ) / 2;

						RgnInfo * pTfr = NULL;
						RgnInfo * pTbr = NULL;

						int i;
						for( i=nBgnIOL; i <= nEndIOL; i++ )
						{
							RgnInfo * pRgn = &m_rgnInfoVect[ m_conflictScanner->GetIOImgeOfIOLine( i ) ];

							m_medTraceRgnPtrVect.PushBack( pRgn );

/*
							if( NULL != pRgn->pTraceFwdRoot )
							{
								pTfr = pRgn->pTraceFwdRoot;
								pTbr = pRgn->pTraceBkdRoot;

								break;
							}
*/
							if( false == pRgn->IsRoot() )
							{
								pRgn->pTraceFwdRoot = pRootRgn1;
								pRgn->pTraceBkdRoot = pRootRgn2;
							}

							if( pRgn->IsRoot() )
								pRgn = pRgn;

							RgnInfo * pRootRgnA = NULL; 

							if( i < nMidIOL )
								pRootRgnA = pRootRgn1; 
							else
								pRootRgnA = pRootRgn2; 

							pRgn->SetRootRgn( pRootRgnA );

							

							
							//UpdateActRgn( pRootRgn1 );
							//RgnInfo * pActRgn1 = pRootRgn1->GetActRgn();

							//pActRgn1->meanColors.IncBy( pRgn->pixColors );
							//pActRgn1->nPixCnt++;
						
							//PrepareRgnLinkActions( pRgn, 0 );

						}

						if( i <= nEndIOL )
						{
							//for( int j = nBgnIOL; j < i; j++ )
							for( int j = nBgnIOL; j <= nEndIOL; j++ )
							{
								RgnInfo * pRgn = &m_rgnInfoVect[ m_conflictScanner->GetIOImgeOfIOLine( j ) ];

								pRgn->pTraceFwdRoot = pTfr;
								pRgn->pTraceBkdRoot = pTbr;
							}
						}

					}

					PrepareRgnLinkActions( pRootRgn1, 0 );
					PrepareRgnLinkActions( pRootRgn2, 0 );

				}
				
			}


		}



		//ShowImage( m_conflictScanner->GetSignalDspImg(), "Signals" );

		

		ShowImage( m_conflictScanner->GetResultImg(), "LinePathImg");

		//FiltSysDebug::SaveToFile( "" );

	}

}