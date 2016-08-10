#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\RegionSegmentor31.h>

#define M_PI 3.14159265358979323846

namespace Hcv
{
	using namespace Hcpl;
	using namespace Hcpl::Math;

	//static MgrOfLinkTypes::CoreStuff m_core;
	//RegionSegmentor31::MgrOfLinkTypes::CoreStuff m_core;

	int RegionSegmentor31::RgnInfo::s_MergeOrder = 0;

	RegionSegmentor31::RegionSegmentor31(F32ImageRef a_src, S16ImageRef a_rootImg,
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

		{
			PrepareSrcGradImg();
		}



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

		{
			m_rgnInTraceArr.SetSize( m_rgnInfoVect.GetSize() );

			for( int i=0; i < m_rgnInTraceArr.GetSize(); i++ )
				m_rgnInTraceArr[ i ] = false;
		}


		InitLinks();		


		//RgnInfo * pRgn81 = GetPointRgn(81, 271);

		m_rgnConflictVect.SetCapacity( 
			10000000);
		
		m_nofConflicts = 0;


		ScanImageLines();

		ActivateNonConfRoots();


	}

	void RegionSegmentor31::InitRgnInfoVect(void)
	{
		RgnInfo::s_MergeOrder = 0;

		CvSize srcSiz = m_src->GetSize();

		m_rgnInfoVect.SetSize(srcSiz.width * srcSiz.height);

		m_medTraceRgnPtrVect.SetCapacity( m_rgnInfoVect.GetSize() );

		//m_traceRootAllocVect.SetCapacity( m_rgnInfoVect.GetSize() );
		m_traceRootAllocVect.SetCapacity( 10000000 );


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

				pRgnInfo->nIndex = m_rgnIndexCalc.Calc(x, y);

				pRgnInfo->pPrev = pRgnInfo;

				



				pRgnInfo->pixColors = 
					(F32ColorVal *)m_src->GetPixAt(x, y);

				pRgnInfo->showColor = *pRgnInfo->pixColors;


				//pRgnInfo->bIsInConflict = false;

				pRgnInfo->accXYDistFromRoot = 0;
				pRgnInfo->maxAccXYDistFromRoot = -1;

				pRgnInfo->minDistFromRoot = 0;

				pRgnInfo->m_pMinDist_EI = NULL;

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

	inline void RegionSegmentor31::CreateLink( RgnInfo * a_pRgn, 
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
			//pLA->pPrev = pLA;

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




	void RegionSegmentor31::InitLinks(void)
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

	S16ImageRef RegionSegmentor31::GenDifImg(bool a_bRC, bool a_bRB,
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



	
	HistoGramRef RegionSegmentor31::GenDifHisto(bool a_bRC, bool a_bRB,
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

	
	
	
	
	void RegionSegmentor31::Segment()
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

					//const int nTestIdx = 181157;
					const int nTestIdx = 173209;

					
					if( nTestIdx == pLA->pRgn1->nIndex ||
						 nTestIdx == pLA->pRgn2->nIndex )
					{
						i = i;
					}


					bool bMergeRoots = false;

					//if( 1 != i )
					if( 0 == i )
					//if( 0 == i || e >= 1 )
					//if( 0 == i || e >= 1 )
					{
						if( NULL != pRootRgn1 && NULL != pRootRgn2 )
						{
							if( pRootRgn1 == pRootRgn2 )
								continue;

							bMergeRoots = true;

							float distCur = m_linkActionMergeQues.GetLastMinIndex();

							//if( distCur < pLA->pRgn2->minDistFromRoot * 1 )
							//if( distCur < pLA->pRgn2->minDistFromRoot * 0.8 )

/*
							if( pRootRgn2->bIsPassiveRoot &&
								distCur < pRootRgn2->minDistFromRoot * 0.5 )
							{								
								//m_linkActionMergeQues.PushPtr( 
									//m_linkActionMergeQues.GetNofQues() - 1, pLA );

								CreateConf_WithTrace( pRootRgn1, pRootRgn2->m_pMinDist_EI );

								continue;
							}
*/



							//float dist = CalcRgnDif( pRootRgn1, pRootRgn2 );

						}
						else		//	NULL == pRootRgn2
						{
							//if( NULL != pLA->pRgn1->pTraceRootElm )
								//continue;

							UpdateActRgn( pRootRgn1 );
							RgnInfo * pActRgn1 = pRootRgn1->GetActRgn();


/*							
							float distCur = m_linkActionMergeQues.GetLastMinIndex();

							//if( distCur < pLA->pRgn2->minDistFromRoot * 1 )
							//if( distCur < pLA->pRgn2->minDistFromRoot * 0.8 )
							//if( distCur < pLA->pRgn2->minDistFromRoot * 0.5 )
							if( distCur < pLA->pRgn2->minDistFromRoot * 0.3 )
							{								
								//m_linkActionMergeQues.PushPtr( 
									//m_linkActionMergeQues.GetNofQues() - 1, pLA );

								continue;
							}
*/

							bool bAddRgn = true;

							if( bAddRgn )
							{	
								//*pLA->pRgn2->pixColors = *pLA->pRgn1->pixColors;
								pLA->pRgn2->SetRootRgn( pRootRgn1 );
								pLA->pRgn2->pSrcRgn = pLA->pRgn1;


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



						bool bHaveConflict = false;

						m_nVisitID++;

						RgnInfo * pMinSizRgn, * pMaxSizRgn;

						if( pActRgn1 == pActRgn2 )	
						{
							continue;
						}
						else
						{
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

							bHaveConflict = HaveConflict( pMinSizRgn, pMaxSizRgn );
						}


						if( bHaveConflict )
						//if(true)
						{
							continue;
						}
						else
						{
	//		NoConflict:
						
								
							if( pActRgn1 == pActRgn2 )	
								continue;							
							
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


							PrepareRgnLinkActions( pLA->pRgn2, 
								m_linkActionMergeQues.GetLastMinIndex() );
						}
					}
				}while( NULL != pLA );


				for(int k=0; k < interRootQues.GetNofQues(); k++)
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

		{
			F32ImageRef sgmImg2 = GenSegmentedImage2();
			ShowImage( sgmImg2, "GenSegmentedImage_2");	
			//SaveImage( sgmImg2, "sgmImg2.jpg" );
		}

		ShowImage( 
			GenColorDsp( 
			GenU8FromF32Image( GenMergeOrderImage() )), 			
			"MergeOrderImage");	

		ShowImage( GenRootRgnImage(), "RootRgnImage" );

		{
			m_rootRgnImg = GenRgnRootDifImage();
			F32ImageRef img1 = m_rootRgnImg;

			ShowImage( img1, "GenRgnRootDifImage" );

			GlobalStuff::SetLinePathImg( img1 );
			GlobalStuff::ShowLinePathImg();
		}
	}

	float RegionSegmentor31::CalcRgnDif(RgnInfo * a_pRgn1, RgnInfo * a_pRgn2)
	{
		float dist = m_difScale * F32ColorVal::Sub(
			*a_pRgn1->pixColors, *a_pRgn2->pixColors).CalcMag();

		dist += m_nAprSize;

		return dist;
	}

	void RegionSegmentor31::UpdateActRgn(RgnInfo * a_pRgn)
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

	S16ImageRef RegionSegmentor31::GenSegmentedImage( bool a_bShowMeanColor )
	{
		S16ImageRef ret = S16Image::Create(m_src->GetSize(), 3);
		//S16ImageRef ret = GenS16FromF32Image(
			//GenSegmentedImage2());

		//return ret;

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




	F32ImageRef RegionSegmentor31::GenSegmentedImage2( bool a_bShowMeanColor )
	{
		S16ImageRef difImg = GenDifImg( true, true, true, true );

		//F32ImageRef ret = F32Image::Create(m_src->GetSize(), 3);
		F32ImageRef ret = m_src->Clone();




		for(int i=0; i < m_rgnInfoVect.GetSize(); i++)
		{
			RgnInfo * pRgn = &m_rgnInfoVect[i];
			RgnInfo * pActRgn = pRgn->GetActRgn();

		}

		F32ChannelRef retCh0 = ret->GetAt(0);
		F32ChannelRef retCh1 = ret->GetAt(1);
		F32ChannelRef retCh2 = ret->GetAt(2);

		for(int i=0; i < m_rgnInfoVect.GetSize(); i++)
		{
			RgnInfo * pRgn = &m_rgnInfoVect[i];
			RgnInfo * pActRgn = pRgn->GetActRgn();

			F32ColorVal * retPix = (F32ColorVal *)ret->GetPixAt( pRgn->pos.x, pRgn->pos.y );

			short * pDifPix = difImg->GetPixAt( pRgn->pos.x, pRgn->pos.y );

			if( *pDifPix > 0 )
			{
				retPix->AssignVal( 255, 255, 255 );
			}
			else
			{
				retPix->MultSelfBy( 0.8 );
			}
				

		}

		return ret;
	}



	void RegionSegmentor31::ShowValleyPath(int a_x, int a_y)
	{
		Hcv::S16ImageRef img0 = this->GenSegmentedImage(false);
		//Hcv::F32ImageRef img1 = this->GenSegmentedImage2(false);

		//img1 = img1->Clone();
		Hcv::F32ImageRef img1 = GenF32FromS16Image( img0 );


		RgnInfo * pRgn1 = GetPointRgn( a_x, a_y );


		ShowImage(img1, "RgnSgmImg");

	}


	void RegionSegmentor31::ShowSrcPath(int a_x, int a_y)
	{
		//a_x = 266;
		//a_y = 426;

		Hcv::S16ImageRef img0 = this->GenSegmentedImage(false);
		//Hcv::F32ImageRef img1 = this->GenSegmentedImage2(false);

		//img1 = img1->Clone();
		Hcv::F32ImageRef img1 = GenF32FromS16Image( img0 );


		RgnInfo * pRgn1 = GetPointRgn( a_x, a_y );

		Signal1DBuilderRef sb0 = new Signal1DBuilder( 1300 );
		Signal1DBuilderRef sb1 = new Signal1DBuilder( 1300 );


		do
		{
			F32ColorVal * pColor = (F32ColorVal *)img1->GetPixAt(
				pRgn1->pos.x, pRgn1->pos.y );

			{
				float gradVal = *m_srcGrad->GetPixAt(
					pRgn1->pos.x, pRgn1->pos.y );

				sb0->AddValue( gradVal );
			}

			{
				float rootDist = *m_rootRgnImg->GetPixAt(
					pRgn1->pos.x, pRgn1->pos.y );

				sb1->AddValue( rootDist );
			}

			pColor->AssignVal( 0, 255, 0 );

			pRgn1 = pRgn1->pSrcRgn;

		}while( NULL != pRgn1 );

		ShowImage(img1, "RgnSgmImg");

		{
			Signal1DViewer sv1;

			sv1.AddSignal( sb0->GetResult(), u8ColorVal( 255, 0, 0 ) );
			sv1.AddSignal( sb1->GetResult(), u8ColorVal( 0, 255, 0 ) );

			ShowImage( sv1.GenDisplayImage(), "Src Path" );
		}

	}


	void RegionSegmentor31::ShowEdgeOfConflict()
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
			//Hcv::F32ImageRef img1 = this->GenSegmentedImage2(false);

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
	void RegionSegmentor31::PrepareConflicts()
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

	RegionSegmentor31::LinkAction * RegionSegmentor31::CloneLinkAction(LinkAction * a_pLA )
	{
		LinkAction * pLA = &m_linkActionVect[ m_nofLinkActions++ ];

		*pLA = *a_pLA;

		return pLA;
	}

	RegionSegmentor31::LinkAction * RegionSegmentor31::ProvideLinkAction( 
		//RegionSegmentor31::RgnInfo * a_pRgn, RegionSegmentor31::RgnInfo * a_pRgn2 )
		RgnInfo * a_pRgn, RgnInfo * a_pRgn2 )
	{
		static int dxArr[] = { 1, 1, 0, -1,		-1, -1, 0, 1 };
		static int dyArr[] = { 0, 1, 1, 1,		0, -1, -1, -1 };

		static int xyToDirMap[ 3 ][ 3 ]; 

		static bool bMapNotReady = true;


		if( bMapNotReady )
		{
			int * mapBuf1D = & xyToDirMap[ 0 ][ 0 ];

			for( int i=0; i < 9; i++ )
				mapBuf1D[ i ] = -1;

			for( int i=0; i < 8; i++ )
			{
				int dx = dxArr[ i ];
				int dy = dyArr[ i ];

				//int * pVal = & xyToDirMap[ dx + 1 ][ dy + 1 ];
				xyToDirMap[ dx + 1 ][ dy + 1 ] = i;
			}



			bMapNotReady = false;
		}

		F32Point difPnt = F32Point::Sub( a_pRgn2->pos, a_pRgn->pos );

		Hcpl_ASSERT( fabs( difPnt.x ) <= 1 && fabs( difPnt.y ) <= 1 );

		RgnLinkDir dir = 
			(RgnLinkDir) xyToDirMap[ (int)(difPnt.x + 1) ][ (int)(difPnt.y + 1) ];

		Hcpl_ASSERT( (int)dir < 8 && (int)dir >= 0 );


		return ProvideLinkAction( a_pRgn, dir );
	}


	RegionSegmentor31::LinkAction * RegionSegmentor31::ProvideLinkAction(RgnInfo * a_pRgn, RgnLinkDir a_dir )
	{
		// Hthm tmp
		//ThrowHcvException();

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




		return pLA;
	}



	void RegionSegmentor31::PrepareLinkAction( RgnInfo * a_pRgn, RgnInfo * a_pRgn2,
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


		float distNew;

		{		
			float distRoot;
			{
				RgnInfo * pRgnTmp;
				/*if( NULL != a_pRgn2->GetRootRgn() )
					pRgnTmp = a_pRgn2->GetRootRgn();
				else*/
					pRgnTmp = a_pRgn2;

				distRoot = CalcRgnDif( a_pRgn->GetRootRgn(), pRgnTmp );
			}
			
			distNew = distRoot;
			//distNew = a_rLink.DistMag;



			distNew = distRoot;

		}



		//a_rLink.DistMag = a_rLink2.DistMag = distNew;
		
		//if( a_bSetProcessed )
			a_rLink.bProcessed = a_rLink2.bProcessed = true;


		m_linkActionMergeQues.PushPtr( distNew, pLA);
		
	}

	bool RegionSegmentor31::HaveConflict( RgnInfo * a_pMinSizRgn, RgnInfo * a_pMaxSizRgn)
	{
		RegionSegmentor31::RgnConflict * pConflict = GetExistingConflict( a_pMinSizRgn, a_pMaxSizRgn);

		if( NULL != pConflict )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	RegionSegmentor31::RgnConflict * RegionSegmentor31::GetExistingConflict( RgnInfo * a_pMinSizRgn, RgnInfo * a_pMaxSizRgn)
	{
		//bool bRet = false;

		APtrList< RgnConflict > & minConfList = 
			a_pMinSizRgn->conflictList;

		RegionSegmentor31::RgnConflict * pConflict = minConfList.Last();

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

	void RegionSegmentor31::RemoveDuplicateConflicts( RgnInfo * a_pRgn )
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


	void RegionSegmentor31::PrepareRgnLinkActions( RgnInfo * a_pRgn, float a_distBef )
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

			//if( m_rgnInTraceArr[ pRgn2->nIndex ] )
				//continue;

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

			//if( m_rgnInTraceArr[ pRgn2->nIndex ] )
				//continue;

			PrepareLinkAction( a_pRgn, pRgn2, rLink, rLink2, a_distBef );
		}

	}

	S16ImageRef RegionSegmentor31::GenConflictImg(bool a_bRC, bool a_bRB,
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

	void RegionSegmentor31::CreateConf_WithTrace( RgnInfo * a_pRgn1, EdgeScan::EdgeInfo * a_pEI )
	{
		EdgeScan::EdgeInfo & rEI = *a_pEI;

		EdgeInfoAccessor eiAcc( this );

		eiAcc.SetEdgeInfo( & rEI );

		for( int i = rEI.InrR1_IOL + 1; i < rEI.InrR2_IOL; i++ )
		{
			RgnInfo * pRgn = eiAcc.GetRgnOf_IOL( i );

			CreateConflict( a_pRgn1, pRgn );
		}

	}

	void RegionSegmentor31::CreateConflict( RgnInfo * a_pRgn1, RgnInfo * a_pRgn2, RgnInfo * a_pOrgEdge )
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


	void RegionSegmentor31::MgrOfLinkTypes::CoreStuff::InitLinkTypeMgrs()
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

	void RegionSegmentor31::MgrOfLinkTypes::CoreStuff::InitMaps()
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


	void RegionSegmentor31::ActivateNonConfRoots()
	{
		GlobalStuff::SetLinePathImg( m_srcGrad );
		GlobalStuff::ShowLinePathImg();

		//ShowImage(m_srcGrad, "m_srcGrad");


		//ShowImage(m_srcGrad, "Image Roots");

		//F32ImageRef dspImg = GenImageFromChannel( m_srcGrad, 0 );
		//F32ImageRef dspImg = GenTriChGrayImg( m_srcGrad );
		F32ImageRef dspImg = F32Image::Create( m_srcGrad->GetSize(), 3 );

		F32ColorVal * dspColorData = (F32ColorVal *)dspImg->GetPixAt(0, 0);


		F32ImageRef src = m_srcGrad;
		CvSize srcSiz = m_srcGrad->GetSize();

		const int nSrcSiz1D = srcSiz.width * srcSiz.height;

		FixedVector< bool > isRootArr;
		isRootArr.SetSize( nSrcSiz1D );

		//FillIsEdgeRootArr( src, &isRootArr );
		SlideMgrRef sm1 = new SlideMgr( src, m_nAprSize, NULL, 
			&isRootArr );

		m_slideMgr = sm1;

			//(FixedVector< bool > *) &isRootArr );
			//NULL );





		

		//F32ImageRef src = m_src;
		F32ImageRef srcGrad = m_srcGrad;


		FixedVector< SlideMgr::SlideChainElm * > & chainHeadPtrArr = sm1->GetChainHeadPtrArr();


		//F32ImageRef res = F32Image::Create( srcSiz, 3 );
		//res->SetAll(0);

		//F32ImageRef res2 = F32Image::Create( srcSiz, 3 );
		F32ImageRef res2 = F32Image::Create( srcSiz, 1 );
		m_slideRootDifImg = res2;

		//res2->SetAll(0);
		res2->SetAll(-1);



		for( int i=0; i < chainHeadPtrArr.GetSize(); i++ )
		{
			const int nDrawRad = 3;

			SlideMgr::SlideChainElm * pHeadCE = chainHeadPtrArr[ i ];

			//if( 0 != rand() % 100 )
			//	continue;


			{
				SlideMgr::SlideChainElm * pCE = pHeadCE;
				SlideMgr::SlideChainElm * pTailCE = pHeadCE->pTail;

				F32ColorVal color1;

				//U8ColorVal color1;

				//color1.val0 = rand() % 256;
				//color1.val1 = rand() % 256;
				//color1.val2 = rand() % 256;

				F32Point pntTail = sm1->GetPosOfElm( *pTailCE );
				F32ColorVal * pixTail = (F32ColorVal *) m_src->GetPixAt( pntTail.x, pntTail.y );

				RgnInfo * pRgn = &m_rgnInfoVect[ pTailCE->index ];
				pRgn->BeARoot();					
				PrepareRgnLinkActions( pRgn, 0 );

				do
				{
					//float val = fabs( pCE->val - pTailCE->val );

					F32Point pnt1 = sm1->GetPosOfElm( *pCE );				 
					F32ColorVal * pixCE = (F32ColorVal *) m_src->GetPixAt( pnt1.x, pnt1.y );
					//*resPix = color1;

					float val = F32ColorVal::Sub( *pixCE, *pixTail ).CalcMag();					

					//F32ColorVal * resPix = (F32ColorVal *) res2->GetPixAt( pnt1.x, pnt1.y );
					//resPix->AssignVal( val, val, val );

					*res2->GetPixAt( pnt1.x, pnt1.y ) = val;

					//DrawCircle( res, pnt1, color1, nDrawRad );

					if( pCE->bHasNext )
						pCE = pCE->pNext;
					else
						break;
				}
				//while( pCE->bHasNext );
				while( true );
			}


		}

		GlobalStuff::SetLinePathImg( res2 );
		GlobalStuff::ShowLinePathImg();



////////////////

/*
		for( int i=0; i < chainHeadPtrArr.GetSize(); i++ )
		{
			const int nDrawRad = 3;

			SlideMgr::SlideChainElm * pHeadCE = chainHeadPtrArr[ i ];

			//if( 0 != rand() % 100 )
			//	continue;


			{
				SlideMgr::SlideChainElm * pCE = pHeadCE;
				SlideMgr::SlideChainElm * pTailCE = pHeadCE->pTail;

				F32ColorVal color1;

				//U8ColorVal color1;

				//color1.val0 = rand() % 256;
				//color1.val1 = rand() % 256;
				//color1.val2 = rand() % 256;

				F32Point pntTail = sm1->GetPosOfElm( *pTailCE );
				F32ColorVal * pixTail = (F32ColorVal *) m_src->GetPixAt( pntTail.x, pntTail.y );

				RgnInfo * pTaiRgn = &m_rgnInfoVect[ pTailCE->index ];
				//pRgn->BeARoot();					
				//PrepareRgnLinkActions( pRgn, 0 );

				do
				{
					//float val = fabs( pCE->val - pTailCE->val );

					RgnInfo * pRgn = &m_rgnInfoVect[ pCE->index ];

					if( NULL == pRgn->GetRootRgn() )
					{
						pRgn->SetRootRgn( pTaiRgn );
						PrepareRgnLinkActions( pRgn, 0 );
					}
						
					if( pCE->bHasNext && pCE->pNext != pTailCE )
						pCE = pCE->pNext;
					else
						break;
				}
				//while( pCE->bHasNext );
				while( true );
			}


		}
*/








		//FixedVector< bool > & isRootArr = sm1->GetIsRootArr();

/*
		for( int i=0; i < isRootArr.GetSize(); i++ )
		{
			F32ColorVal & rColor = dspColorData[ i ];

			RgnInfo * pRgn = &m_rgnInfoVect[ i ];

			if( isRootArr[ i ] )
			{
				rColor.AssignVal( 255, 70, 70 );
			}
			else if( ! isRootArr[ i ] )
			{
				rColor.AssignVal( 0, 0, 0 );
			}


		}

		ShowImage(dspImg, "Image Roots 0");
*/

		//FillIsEdgeRootArr( src, &isRootArr );

		//Accessor2D< bool > ac1( &isRootArr[0], srcSiz.width, srcSiz.height );

		for( int i=0; i < isRootArr.GetSize(); i++ )
		{
			F32ColorVal & rColor = dspColorData[ i ];

			RgnInfo * pRgn = &m_rgnInfoVect[ i ];

			if( pRgn->IsRoot() && ! isRootArr[ i ] )
			{
				rColor.AssignVal( 0, 180, 0 );
				continue;
			}
			else if( pRgn->IsRoot() && isRootArr[ i ] )
			{
				rColor.AssignVal( 0, 0, 220 );
				continue;
			}
			else if( ! isRootArr[ i ] )
			{
				rColor.AssignVal( 0, 0, 0 );
				continue;
			}
			else if( isRootArr[ i ] )
			{
				rColor.AssignVal( 255, 70, 70 );
				//pRgn->BeARoot();					
				//PrepareRgnLinkActions( pRgn, 0 );
				continue;
			}
			else
			{
				ThrowHcvException();
			}


		}


/*
		{

		for( int y=0; y < srcSiz.height; y++ )
		{
			for( int x=0; x < srcSiz.width; x++ )
			{



			}
		}

		}
*/

		ShowImage(dspImg, "Image Roots");
	}


	void RegionSegmentor31::ScanImageLines()
	{
		CvSize siz1 = m_src->GetSize();

		//m_conflictScanner = new ConflictScanner2( m_src );

		//const int nHalfAprSize = m_conflictScanner->GetAprSize() / 2;

		//m_conflictScanner = NULL;



		//GlobalStuff::SetLinePathImg( scanImage );


/////////////////////////



		m_scanTryMgr = new ScanTryMgr2( m_src );





		static int dxArr[] = {1, 1, 0, -1};
		static int dyArr[] = {0, 1, 1, 1};

		int nCurTraceProcID = 0;

		FixedVector< EdgeScan::EdgeInfo > & rEdgeInfoArr = 
			m_scanTryMgr->GetEdgeInfoArr();

		EdgeInfoAccessor eiAcc( this );

		for( int k=0; k < rEdgeInfoArr.GetSize(); k++ )
		{
			EdgeScan::EdgeInfo & rEI = rEdgeInfoArr[ k ];


			if( rEI.IsCanceled )
				continue;


			if( 20658 == rEI.Index )
				k = k;


			//eiAcc.SetEdgeInfo( & rEdgeInfoArr[ k ] );
			eiAcc.SetEdgeInfo( & rEI );

			RgnInfo * pRootRgn1 = eiAcc.GetR1_Rgn();
			RgnInfo * pRootRgn2 = eiAcc.GetR2_Rgn();

			pRootRgn1->BeARoot();					
			pRootRgn2->BeARoot();

			CreateConflict( pRootRgn1, pRootRgn2, eiAcc.GetEdge_Rgn() );

///*
			{
				RgnInfo * pOldRgn = pRootRgn1;

				float maxDist = 0;

				for( int i = rEI.InrR1_IOL + 1; i <= rEI.Med2_IOL; i++ )
				{
					RgnInfo * pRgn = eiAcc.GetRgnOf_IOL( i );
/*
					if( ! pRgn->IsRoot() )
					{
						pRgn->BeARoot();
						pRgn->bIsPassiveRoot = true;
					}
*/

					float dist = CalcRgnDif( pRootRgn1, pRgn );

					if( pRgn->minDistFromRoot <= 0.2 )
					{
						pRgn->minDistFromRoot = dist;

						pRgn->m_pMinDist_EI = & rEI;
					}
					else if( dist < pRgn->minDistFromRoot )
					{
						pRgn->minDistFromRoot = dist;

						pRgn->m_pMinDist_EI = & rEI;
					}

					if( dist > maxDist )
						maxDist = dist;

					maxDist++;

					dist = maxDist;

					LinkAction * pLA = ProvideLinkAction( pOldRgn, pRgn );

					m_rgnInTraceArr[ pRgn->nIndex ] = true;

					//m_linkActionMergeQues.PushPtr( dist, pLA );

					pOldRgn = pRgn;

					if( NULL == pRgn->m_pMinDist_EI )
						i = i;
				}
			}

			{
				RgnInfo * pOldRgn = pRootRgn2;

				float maxDist = 0;

				for( int i = rEI.InrR2_IOL - 1; i >= rEI.Med1_IOL; i-- )
				{
					RgnInfo * pRgn = eiAcc.GetRgnOf_IOL( i );
/*
					if( ! pRgn->IsRoot() )
					{
						pRgn->BeARoot();
						pRgn->bIsPassiveRoot = true;
					}
*/
					float dist = CalcRgnDif( pRootRgn2, pRgn );

					if( pRgn->minDistFromRoot <= 0.2 )
					{
						pRgn->minDistFromRoot = dist;

						pRgn->m_pMinDist_EI = & rEI;
					}
					else if( dist < pRgn->minDistFromRoot )
					{
						pRgn->minDistFromRoot = dist;

						pRgn->m_pMinDist_EI = & rEI;
					}

					if( dist > maxDist )
						maxDist = dist;

					maxDist++;

					dist = maxDist;

					LinkAction * pLA = ProvideLinkAction( pOldRgn, pRgn );

					m_rgnInTraceArr[ pRgn->nIndex ] = true;

					//m_linkActionMergeQues.PushPtr( dist, pLA );

					pOldRgn = pRgn;

					if( NULL == pRgn->m_pMinDist_EI )
						i = i;
				}
			}
//*/

			RgnInfo * GetRgnOf_IOL( int a_nIOL );
		}

		F32ImageRef scanImage = this->GenScanImage();
	}



	F32ImageRef RegionSegmentor31::GenScanImage()
	{
		F32ImageRef ret = this->m_src->Clone();

		ret->SetAll( 0 );

		U8ColorVal colorEdge1( 0, 210, 0 );

		U8ColorVal colorEdgeArr[ 4 ];

		{
			colorEdgeArr[0].AssignVal( 0, 180, 0 );
			colorEdgeArr[1].AssignVal( 255, 0, 0 );
			colorEdgeArr[2].AssignVal( 0, 0, 255 );
			colorEdgeArr[3].AssignVal( 0, 180, 255 );			
		}

		U8ColorVal colorRootClear( 255, 255, 255 );
		U8ColorVal colorRootInTrace( 255, 0, 200 );

		FixedVector< EdgeScan::EdgeInfo > & rEdgeInfoArr = 
			m_scanTryMgr->GetEdgeInfoArr();

		EdgeInfoAccessor eiAcc( this );

		for( int k=0; k < rEdgeInfoArr.GetSize(); k++ )
		{
			EdgeScan::EdgeInfo & rEI = rEdgeInfoArr[ k ];


			if( rEI.IsCanceled )
				continue;

			eiAcc.SetEdgeInfo( & rEdgeInfoArr[ k ] );


			{
				F32ColorVal * pix = (F32ColorVal *) ret->GetPixAt( 
					eiAcc.GetEdge_Rgn()->pos.x, eiAcc.GetEdge_Rgn()->pos.y );

				//U8ColorVal color1 = colorEdgeArr[ (int)rEI.Dir ];
				U8ColorVal color1 = colorEdge1;

				pix->val0 = color1.val0;
				pix->val1 = color1.val1;
				pix->val2 = color1.val2;
			}


			{
				RgnInfo * pRgn = eiAcc.GetR1_Rgn();

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
				RgnInfo * pRgn = eiAcc.GetR2_Rgn();

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



	F32ImageRef RegionSegmentor31::GenRootRgnImage()
	{
		F32ImageRef ret = F32Image::Create( m_src->GetSize(), 3 );

		for( int i=0; i < m_rgnInfoVect.GetSize(); i++ )
		{
			RgnInfo * pRgn = &m_rgnInfoVect[ i ];

			F32ColorVal * pix = (F32ColorVal *)ret->GetPixAt( pRgn->pos.x, pRgn->pos.y );

			if( pRgn->IsRoot() )
				pix->AssignVal( 255, 255, 255 );
			else
				pix->AssignVal( 0, 0, 0 );
		}

		return ret;
	}


	F32ImageRef RegionSegmentor31::GenRgnRootDifImage()
	{
		F32ImageRef ret = F32Image::Create( m_src->GetSize(), 3 );

		float maxVal = 0;

		for( int i=0; i < m_rgnInfoVect.GetSize(); i++ )
		{
			RgnInfo * pRgn = &m_rgnInfoVect[ i ];

			F32ColorVal * pix = (F32ColorVal *)ret->GetPixAt( pRgn->pos.x, pRgn->pos.y );

			float val = F32ColorVal::Sub(
				*pRgn->pixColors,
				*pRgn->GetRootRgn()->pixColors 
				).CalcMag();

			if( pRgn != pRgn->GetRootRgn() )
				val = val;


			if( val > maxVal )
				maxVal = val;

			pix->AssignVal( val, val, val );
		}

		return ret;
	}


	F32ImageRef RegionSegmentor31::GenMergeOrderImage()
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


	void RegionSegmentor31::PrepareSrcGradImg()
	{

		//m_srcGrad = GenMorphGradImg( m_src, 3 );
		//m_srcGrad = GenCvSmoothedImg( m_srcGrad, 3 );

//		m_srcGrad = GenMorphGradImg( m_src, 1 );
//		m_srcGrad = GenCvSmoothedImg( m_srcGrad, 5 );

		//m_srcGrad = GenMorphGradImg( m_src, 7 );
		//m_srcGrad = GenCvSmoothedImg( m_srcGrad, 11 );

		m_srcGrad = CircDiff::GenResult( m_src, 7 );
		//m_srcGrad = GenCvSmoothedImg( m_srcGrad, 5 );
		

		//m_srcGrad = GenMorphGradImg( m_src, 14 );
		//m_srcGrad = GenCvSmoothedImg( m_srcGrad, 21 );
		//m_srcGrad = GenCvSmoothedImg( m_srcGrad, 5 );

		//m_srcGrad = GenCvErodeCircleImg(m_srcGrad, 1);

		//SobelBuilder sb1( m_src, 5 );

		//m_srcGrad = sb1.


/*
		m_srcGrad = GenMorphGradImg( m_src, 3 );
		m_srcGrad = GenCvDilateCircleImg(m_srcGrad, 2);
		m_srcGrad = GenCvErodeCircleImg(m_srcGrad, 2);
		m_srcGrad = GenCvSmoothedImg( m_srcGrad, 3 );
		m_srcGrad = GenCvErodeCircleImg(m_srcGrad, 2);
*/

//		m_srcGrad = GenMorphGradImg( m_src, 2 );
	}


}