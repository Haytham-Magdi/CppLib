#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\RegionSegmentor41.h>

#define M_PI 3.14159265358979323846

namespace Hcv
{
	using namespace Hcpl;
	using namespace Hcpl::Math;

	//static MgrOfLinkTypes::CoreStuff m_core;
	//RegionSegmentor41::MgrOfLinkTypes::CoreStuff m_core;

	int RegionSegmentor41::RgnInfo::s_MergeOrder = 0;

	RegionSegmentor41::RegionSegmentor41(F32ImageRef a_src, S16ImageRef a_rootImg,
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

		m_linkActionMergeQues.InitSize( GetMaxDif() * 2 + 2 );
		m_linkActionEdgeQues.InitSize( GetMaxDif() * 2 + 2 );

		m_edgeInfo_Ques.InitSize( m_linkActionMergeQues.GetNofQues() );

		m_medTraceQues.Init( m_linkActionMergeQues.GetNofQues() );


		m_edgeFwdRgnVect.SetCapacity(3000);

		m_edgeBkdRgnVect.SetCapacity(3000);
		
		InitRgnInfoVect();

		//m_pDbgRgn = & m_rgnInfoVect[ 282873 ];
		//m_pDbgRgn = & m_rgnInfoVect[ 110507 ];
		//m_pDbgRgn = & m_rgnInfoVect[ 109981 ];
		//m_pDbgRgn = & m_rgnInfoVect[ 101785 ];		
		//m_pDbgRgn = & m_rgnInfoVect[ 286705 ];
		m_pDbgRgn = & m_rgnInfoVect[ 535326 ];

		//m_pDbgRgn = GetPointRgn( 301, 348 );
		

		{
			m_rgnInTraceArr.SetCapacity( m_rgnInfoVect.GetSize() );
		}

		m_fillInfoVect.SetCapacity( m_rgnInfoVect.GetSize() / 4 );

		m_nCurGrpID = -1;

		m_rgnOf_FstBadSrcArr.SetCapacity( m_rgnInfoVect.GetSize() / 4 );

		m_rgnOf_FstBadSrcArr_2.SetCapacity( m_rgnInfoVect.GetSize() / 4 );
		
		m_fillDeq.SetCapacity( m_rgnInfoVect.GetSize() / 4 );

		m_unfill_Deq.SetCapacity( m_rgnInfoVect.GetSize() / 4 );

		m_fstFromBad_EI_Arr.SetCapacity( m_rgnInfoVect.GetSize() / 4 );

		m_shortest_FromBad_EI_Arr.SetCapacity( 10000 );

		m_rgnOfBadSrcArr.SetCapacity( m_rgnInfoVect.GetSize() / 3 );

		m_rgnOf_OpenEnd_Arr.SetCapacity( m_rgnInfoVect.GetSize() / 5 );

		

		{	
			IndexCalc2D idx2D( m_src->GetSize().width,
				m_src->GetSize().height );

			//m_nTestRgnIdx = idx2D.Calc( 589, 288 );
			//m_nTestRgnIdx = idx2D.Calc( 592, 327 );
			//m_nTestRgnIdx = idx2D.Calc( 301, 348 );
			//m_nTestRgnIdx = idx2D.Calc( 308, 347 );

			m_nTestRgnIdx = idx2D.Calc( 17, 353 );




			//m_nTestRgnIdx = 237527;
			//m_nTestRgnIdx = 236823;			
			//m_nTestRgnIdx = 236118;			
			//m_nTestRgnIdx = 233301;		

			//m_nTestRgnIdx = 234005;			
			//m_nTestRgnIdx = 273544;			
		}


		InitLinks();		


		//RgnInfo * pRgn81 = GetPointRgn(81, 271);

		m_rgnConflictVect.SetCapacity( 
			10000000);
		
		m_nofConflicts = 0;


		ScanImageLines();


	}

	void RegionSegmentor41::InitRgnInfoVect(void)
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



		//m_imgDataMgr = new ImgDataMgr_Simple(
		m_imgDataMgr = new ImgDataMgr_Simple_2(
			GlobalStuff::GetLinePathImg(), m_nAprSize);


		m_standDiv_Buf = m_imgDataMgr->GetStandDiv_Mag_Image(
			)->GetPixAt(0, 0);


		ImgDataMgr_Simple_2 * pDataMgr_Exact = dynamic_cast<ImgDataMgr_Simple_2 *>
			((IImgDataMgr *)m_imgDataMgr); 

		m_dataElm_Buf = & (pDataMgr_Exact->GetDataArr())[0];

		m_dataElm_Mean_Buf = & (pDataMgr_Exact->GetDataArr_Mean())[0];

		m_avgSqrMag_Buf = & (pDataMgr_Exact->GetAvgSqrMag_Arr())[0];




		for(int y=0; y < srcSiz.height; y++)
		{
			for(int x=0; x < srcSiz.width; x++)
			{	
				RgnInfo * pRgnInfo = GetPointRgn(x, y);

				pRgnInfo->nIndex = m_rgnIndexCalc.Calc(x, y);

				pRgnInfo->pPrev = pRgnInfo;

				
				pRgnInfo->pDataElm = & m_dataElm_Buf[ pRgnInfo->nIndex ];

				pRgnInfo->dataElm_Mean.Copy( 
					m_dataElm_Mean_Buf[ pRgnInfo->nIndex ] );

				pRgnInfo->magSqr = m_avgSqrMag_Buf[ pRgnInfo->nIndex ];



				pRgnInfo->pixColors = 
					(F32ColorVal *)m_src->GetPixAt(x, y);

				pRgnInfo->showColor = *pRgnInfo->pixColors;


				//pRgnInfo->bIsInConflict = false;

				pRgnInfo->accXYDistFromRoot = 0;
				pRgnInfo->maxAccXYDistFromRoot = -1;

				pRgnInfo->minDistFromRoot = 0;

				pRgnInfo->bFstFromBadSrc = false;

				pRgnInfo->bBadFillDone = false;

				pRgnInfo->bBad_UnfillDone = false;

				pRgnInfo->bInIntersection = false;

				pRgnInfo->bFromBadSrc = false;

				pRgnInfo->m_pDeepSrcBadRgn = NULL;

				pRgnInfo->bFromBadSrc_Org = false;

				pRgnInfo->bFrom_MaybeBadSrc = false;				

				pRgnInfo->bMaybe_FstFromBadSrc = false;				

				pRgnInfo->bInNativeTrace = false;				

				pRgnInfo->bFromSureBadSrc = false;

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

	inline void RegionSegmentor41::CreateLink( RgnInfo * a_pRgn, 
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
		//F32ColorVal colorDist1 = F32ColorVal::Sub(
			//*pRgn2->pixColors, *a_pRgn->pixColors);

		F32ColorVal colorDist1 = m_imgDataMgr->CalcDif(
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




	void RegionSegmentor41::InitLinks(void)
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

	S16ImageRef RegionSegmentor41::GenDifImg(bool a_bRC, bool a_bRB,
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


		//F32Point testPnt( 417, 534 );
		F32Point testPnt( 419, 532 );


		int nTestCnt = 0;

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
						//if( F32Point::Sub( testPnt, pRgn->pos ).CalcMag() < 2 ) 
						if( F32Point::Sub( testPnt, pRgn->pos ).CalcMag() < 3 ) 
						{
							RgnInfo * pActRgn1 = pRgn->GetActRgn();
							RgnInfo * pActRgn2 = links[j].pPeerRgn->GetActRgn();

							if( pActRgn1->nPixCnt < 20 ||
								pActRgn2->nPixCnt < 20 )
							{
								i = i;
							}

							nTestCnt++;

							i = i;
						}

						retCh0->SetAt( pRgn->pos.x, pRgn->pos.y, 255);
					}
				}
			}
		}

		return ret;
	}



	
	HistoGramRef RegionSegmentor41::GenDifHisto(bool a_bRC, bool a_bRB,
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

	
	
	
	
	void RegionSegmentor41::Segment()
	{
		//m_pDbgRgn = & m_rgnInfoVect[ 110507 ];

		RgnInfo * pTestRgn = & m_rgnInfoVect[ m_nTestRgnIdx ];

		const int nLastLA_Index = GlobalStuff::m_nLA_Index;

		ListQue< LinkAction > tmpQue;

		//MultiListQue< LinkAction > m_difQues;
		
		MultiListQue< LinkAction > interRootQues;
		
		interRootQues.Init( m_linkActionMergeQues.GetNofQues() );

		//ListQue< LinkAction > interRootQue;


		const int nLim_e = 1;
		//const int nLim_e = 2;
		//const int nLim_e = 3;
		//const int nLim_e = 4;
		//const int nLim_e = 5;
		//const int nLim_e = 6;
		//const int nLim_e = 7;
		//const int nLim_e = 15;
		//const int nLim_e = 20;


		EdgeInfoAccessor eiAcc( this );


		for(int e=0; e < nLim_e; e++)
		{
			e = e;
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


					if( 2110125 == pLA->nIndex )
						i = i;

					if( 169560 == pLA->pRgn1->nIndex )
						i = i;

					if( 169560 == pLA->pRgn2->nIndex )
						i = i;


					//pLA->pRgn1->bIsPassiveRoot = false;
					//pLA->pRgn2->bIsPassiveRoot = false;


					GlobalStuff::m_nLA_Index = pLA->nIndex;

					//if( 3590427 == pLA->nIndex )
					if( 284395 == pLA->nIndex )
						pLA = pLA;


					float distCur = m_linkActionMergeQues.GetLastMinIndex();

					float distAct = (int)CalcRgnDif( pLA->pRgn1, pLA->pRgn2 );

					if( distAct > distCur )
					{
						m_linkActionMergeQues.PushPtr( distAct, pLA );

						continue;
					}

					//{
					//	int nLastLA_MinIdx = 
					//		m_linkActionMergeQues.GetLastMinIndex();

					//	while(
					//		true)
					//		//m_edgeInfo_Ques.Get_CurMinIndex() < 
					//		//nLastLA_MinIdx )
					//		//nLastLA_MinIdx * 0.7 )
					//		//nLastLA_MinIdx * 0.1 )
					//		//nLastLA_MinIdx * 3 )
					//	{
					//		EdgeScan::EdgeInfo * pEI = 
					//			m_edgeInfo_Ques.PopPtrMin();

					//		if( NULL == pEI )
					//			break;

					//		EdgeScan::EdgeInfo & rEI = *pEI;

					//		if( rEI.IsCanceled )
					//			continue;

					//		if( 19902 == rEI.Index )
					//			rEI.Index = rEI.Index;

					//		eiAcc.SetEdgeInfo( & rEI );

					//		RgnInfo * pRootRgn1 = eiAcc.GetR1_Rgn();
					//		RgnInfo * pRootRgn2 = eiAcc.GetR2_Rgn();

					//		UpdateActRgn( pRootRgn1 );
					//		RgnInfo * pRgnAct1 = pRootRgn1->GetActRgn();

					//		UpdateActRgn( pRootRgn2 );
					//		RgnInfo * pRgnAct2 = pRootRgn2->GetActRgn();

					//		//if( pRgnAct1 == pRgnAct2 )
					//			//continue;

					//		CreateConflict( pRootRgn1, pRootRgn2, eiAcc.GetEdge_Rgn(), & rEI  );
					//	}



					//}



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
					//if( 0 == i )
					//if( 0 == i || e >= 1 )
					//if( 0 == i || e >= 1 )
					{
						if( NULL != pRootRgn1 && NULL != pRootRgn2 )
						{
							if( 0 == i )
							{

								if( pRootRgn1 == pRootRgn2 )
									continue;

								bMergeRoots = true;

								//float distCur = m_linkActionMergeQues.GetLastMinIndex();


								//-- interRootQues.PushPtr(
								//-- 	m_linkActionMergeQues.GetLastMinIndex(), pLA );



								//float dist = CalcRgnDif( pRootRgn1, pRootRgn2 );
							}

						}
						else		//	NULL == pRootRgn2
						{
							//if( NULL != pLA->pRgn1->pTraceRootElm )
								//continue;

							if( 1 == i )
								i = i;

							UpdateActRgn( pRootRgn1 );
							RgnInfo * pActRgn1 = pRootRgn1->GetActRgn();

							bool bAddRgn = true;


							if( bAddRgn )
							{	
								//*pLA->pRgn2->pixColors = *pLA->pRgn1->pixColors;

								if( m_maxDif_Buf[ pLA->pRgn2->nIndex ] <=
									m_maxDif_Buf[ pRootRgn1->nIndex ] )
								{
									pLA->pRgn2->BeARoot();
									pLA->pRgn2->SetActRgn( pRootRgn1->GetActRgn() );
								}
								else
								{
									pLA->pRgn2->SetRootRgn( pRootRgn1 );
								}

								pLA->pRgn2->pSrcRgn = pLA->pRgn1;

								pActRgn1->IncPopulationBy( pLA->pRgn2 );

								PrepareRgnLinkActions( pLA->pRgn2, 0 );
							}

							continue;
						}

					}

					//else	//	i > 0
//--					if( 1 == i )
					//if( bMergeRoots )
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

							if( 3590427 == pLA->nIndex )
								pLA = pLA;

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
								//*pRootRgn2->pixColors = *pRootRgn1->pixColors;
								//pRootRgn2->bIsPassiveRoot = false;

								//PrepareRgnLinkActions( pRootRgn2, 
									//m_linkActionMergeQues.GetLastMinIndex() );
							}

							if( 0 != pMinSizRgn->conflictList.GetSize() )
								RemoveDuplicateConflicts( pMaxSizRgn );

							{
								RgnInfo * pMasterRgn, * pSlaveRgn;

								if( pRootRgn2->bIsPassiveRoot ||
									pActRgn1 < pActRgn2 )	
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

								pMasterRgn->IncPopulationBy( pSlaveRgn );
							}

							if( pRootRgn2->bIsPassiveRoot )
							{
								pRootRgn2->SetRootRgn( pRootRgn1 );

								pRootRgn2->bIsPassiveRoot = false;
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

			if( 1 == e )
			{
				for(int i=0; i < m_rgnInfoVect.GetSize(); i++)
				{
					RgnInfo * pRgn2 = &m_rgnInfoVect[i];
					
					RgnInfo * pActRgn2 = pRgn2->GetActRgn();
					
					//if( 1 == pActRgn2->nPixCnt )
					if( pActRgn2->nPixCnt < 20 )
						i = i;
				}
			}


			//if( e > 0 && e < nLim_e - 1 )
			//if( e > 0 )
				//ResetBadSrcWork();


			//if( 0 == e )
//if( e < nLim_e - 1 )
				//MarkRgnsFromBadSrc();
 
			//e = 5;
			//if( 0 == e )
			if( e < nLim_e - 1 )
			{

				for( int i=0; i < m_rgnOfBadSrcArr.GetSize(); i++ )			
				{
					RgnInfo * pRgn = m_rgnOfBadSrcArr[ i ];			

					if( 169560 == pRgn->nIndex || 168855 == pRgn->nIndex )
						i = i;

					//if( ! pRgn->bFromBadSrc )
					if( ! pRgn->bFromBadSrc && ! pRgn->bFlag1 )
					{
						if( pRgn->bFstFromBadSrc )
							i = i;

						continue;
					}
					
					//Hcpl_ASSERT( pRgn->bFromBadSrc );

					//if( pRgn->bFstFromBadSrc )
						//continue;
					
					Hcpl_ASSERT( ! pRgn->IsRoot() );

					{
						RgnInfo * pActRgn = pRgn->GetActRgn();
						pActRgn->nPixCnt--;
					}

					if( 353667 == pRgn->nIndex )
						i = i;

					pRgn->SetRootRgn( NULL );
					pRgn->pSrcRgn = NULL;
				

					for( int j=0; j < 8; j++ )
					{
						RgnLink & rLink = pRgn->links[ j ];

						if( ! rLink.bExists )
							continue;

						RgnInfo * pRgn2 = rLink.pPeerRgn;

						if( 169560 == pRgn2->nIndex	|| 168855 == pRgn2->nIndex )
							j = j;

						//if( pRgn2->bFromBadSrc )
						if( pRgn2->bFromBadSrc || pRgn2->bFlag1 )
							continue;

						//if( 169560 == pRgn2->nIndex	&& 168855 == pRgn->nIndex )
							//j = j;

						//pRgn2->bFstFromBadSrc

						Hcpl_ASSERT( NULL != pRgn2->GetRootRgn() );


						RgnLink & rLink2 = pRgn2->links[ ( j + 4 ) % 8 ];

						Hcpl_ASSERT( rLink2.pPeerRgn == pRgn );

						PrepareLinkAction( pRgn2, pRgn, rLink2, rLink, 0 );
					}
				}

			}





		}

		//ShowImage( GenMergeOrderImage(), "MergeOrderImage");	

		{
			F32ImageRef sgmImg2 = GenSegmentedImage2();
			ShowImage( sgmImg2, "GenSegmentedImage_2");	
			SaveImage( sgmImg2, "sgmImg2.jpg" );
		}

		ShowImage( 
			//GenColorDsp( 
			( 
			GenU8FromF32Image( GenMergeOrderImage() )), 			
			"MergeOrderImage");	

		ShowImage( GenRootRgnImage(), "RootRgnImage" );

		ShowImage( GenTraceDamImage(), "TraceDamImage" );

		

		//ShowImage( Gen_FalseMergeImage(), "FalseMerge" );
		ShowImage( Gen_FalseMergeImage_2(), "FalseMerge" );

		//ShowImage( GenBadSrcImage(), "BadSrcImage" );

		

		{
			m_rootRgnImg = GenRgnRootDifImage();
			F32ImageRef img1 = m_rootRgnImg;

			//ShowImage( img1, "GenRgnRootDifImage" );

			//GlobalStuff::SetLinePathImg( img1 );
			//GlobalStuff::ShowLinePathImg();
		}

	}

	float RegionSegmentor41::CalcRgnDif(RgnInfo * a_pRgn1, RgnInfo * a_pRgn2)
	{
		//float dist = m_difScale * m_imgDataMgr->CalcDif(
			//a_pRgn1->nIndex, a_pRgn2->nIndex);
		
		UpdateActRgn(a_pRgn1);

		UpdateActRgn(a_pRgn2);

		RgnInfo * pRgnAct_1 = a_pRgn1->GetActRgn_2();
		RgnInfo * pRgnAct_2 = a_pRgn2->GetActRgn_2();


		ImgDataElm_Simple elm_Mean_1;

		elm_Mean_1.Copy( pRgnAct_1->dataElm_Mean );
		elm_Mean_1.DividSelfBy( pRgnAct_1->nPixCnt );


		ImgDataElm_Simple elm_Mean_2;

		elm_Mean_2.Copy( pRgnAct_2->dataElm_Mean );
		elm_Mean_2.DividSelfBy( pRgnAct_2->nPixCnt );


		elm_Mean_1.IncBy( elm_Mean_2 );
		elm_Mean_1.DividSelfBy( 2 );
			
		float magSqr = ( pRgnAct_1->magSqr / pRgnAct_1->nPixCnt +
			pRgnAct_2->magSqr / pRgnAct_2->nPixCnt ) / 2;

		float dist = magSqr - elm_Mean_1.CalcMagSqr();
		
		if( dist < 0 )
			dist = 0;

		dist = sqrt( dist );

		if( dist > 1000 )
			dist = dist;

		dist *= m_difScale;

		dist += m_nAprSize;

		//dist = m_difScale * ImgDataElm_Simple::CalcDif(
		//	*a_pRgn1->pDataElm, *a_pRgn2->pDataElm);
		//	//*pRgnAct_1->pDataElm, *pRgnAct_2->pDataElm);


		return dist;
	}

	void RegionSegmentor41::UpdateActRgn(RgnInfo * a_pRgn)
	{
		if( NULL == a_pRgn->GetRootRgn() )
			return;
		
		if( a_pRgn->GetRootRgn() == a_pRgn->GetRootRgn()->GetActRgn() )
			return;

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

	S16ImageRef RegionSegmentor41::GenSegmentedImage( bool a_bShowMeanColor )
	{
		S16ImageRef ret = S16Image::Create(m_src->GetSize(), 3);
		//S16ImageRef ret = GenS16FromF32Image(
			//GenSegmentedImage2());

		//return ret;

		for(int i=0; i < m_rgnInfoVect.GetSize(); i++)
		{
			RgnInfo * pRgn = &m_rgnInfoVect[i];
			RgnInfo * pActRgn = pRgn->GetActRgn();

			//pActRgn->IncPopulationBy( 

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




	F32ImageRef RegionSegmentor41::GenSegmentedImage2( bool a_bShowMeanColor )
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



	void RegionSegmentor41::ShowValleyPath(int a_x, int a_y)
	{
		Hcv::S16ImageRef img0 = this->GenSegmentedImage(false);
		//Hcv::F32ImageRef img1 = this->GenSegmentedImage2(false);

		//img1 = img1->Clone();
		Hcv::F32ImageRef img1 = GenF32FromS16Image( img0 );


		RgnInfo * pRgn1 = GetPointRgn( a_x, a_y );


		ShowImage(img1, "RgnSgmImg");

	}


	void RegionSegmentor41::ShowSrcPath(int a_x, int a_y)
	{
		//a_x = 266;
		//a_y = 426;

		//a_x = 588;
		//a_y = 341;


		Hcv::S16ImageRef img0 = this->GenSegmentedImage(false);
		//Hcv::F32ImageRef img1 = this->GenSegmentedImage2(false);

		//img1 = img1->Clone();
		Hcv::F32ImageRef img1 = GenF32FromS16Image( img0 );


		RgnInfo * pRgn1 = GetPointRgn( a_x, a_y );

		Signal1DBuilderRef sb0 = new Signal1DBuilder( 1300 );
		Signal1DBuilderRef sb1 = new Signal1DBuilder( 1300 );
		Signal1DBuilderRef sb2 = new Signal1DBuilder( 1300 );


		RgnInfo * pRgn1_Old = NULL;

		int nSrcCnt = 0;
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

			{
				float minRootDist = pRgn1->minDistFromRoot / m_difScale;

				sb2->AddValue( minRootDist );
			}

			pColor->AssignVal( 0, 255, 0 );

			pRgn1_Old = pRgn1;
			pRgn1 = pRgn1->pSrcRgn;

			nSrcCnt++;

			if( nSrcCnt >= 1500 )
				break;

		}while( NULL != pRgn1 );

		{
			//F32ColorVal * pColor = (F32ColorVal *)img1->GetPixAt(
				//a_x, a_y );

			//pColor->AssignVal( 0, 0, 255 );

			RgnInfo * pRgn1_1 = GetPointRgn( a_x, a_y );

			DrawCircle( img1, F32Point( a_x, a_y ), u8ColorVal( 0, 0, 255 ) );

			DrawCircle( img1, pRgn1_Old->pos, u8ColorVal( 0, 140, 255 ) );

			UpdateActRgn( pRgn1_1 );
			UpdateActRgn( pRgn1_Old );

			Hcpl_ASSERT( pRgn1_1->GetActRgn() == pRgn1_Old->GetActRgn() );



			if( NULL != pRgn1_1->m_pDeepSrcBadRgn )
			{
				DrawCircle( img1, pRgn1_1->m_pDeepSrcBadRgn->pos, 
				u8ColorVal( 0, 255, 255 ) );
			}

		}


		ShowImage(img1, "RgnSgmImg");

		{
			Signal1DViewer sv1;

			sv1.AddSignal( sb0->GetResult(), u8ColorVal( 255, 0, 0 ) );
			sv1.AddSignal( sb1->GetResult(), u8ColorVal( 0, 255, 0 ) );
			sv1.AddSignal( sb2->GetResult(), u8ColorVal( 0, 0, 255 ) );

			ShowImage( sv1.GenDisplayImage(), "Src Path" );
		}

	}


	void RegionSegmentor41::ShowEdgeOfConflict()
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
	void RegionSegmentor41::PrepareConflicts()
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

	//RegionSegmentor41::LinkAction * RegionSegmentor41::CloneLinkAction(LinkAction * a_pLA )
	//{
	//	LinkAction * pLA = &m_linkActionVect[ m_nofLinkActions++ ];

	//	*pLA = *a_pLA;

	//	return pLA;
	//}

	RegionSegmentor41::LinkAction * RegionSegmentor41::ProvideLinkAction( 
		//RegionSegmentor41::RgnInfo * a_pRgn, RegionSegmentor41::RgnInfo * a_pRgn2 )
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


	RegionSegmentor41::LinkAction * RegionSegmentor41::ProvideLinkAction(RgnInfo * a_pRgn, RgnLinkDir a_dir )
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



	void RegionSegmentor41::PrepareLinkAction( RgnInfo * a_pRgn, RgnInfo * a_pRgn2,
			RgnLink & a_rLink, RgnLink & a_rLink2, float a_distBef, bool a_bSetProcessed )
	{
		Hcpl_ASSERT( NULL != a_pRgn->GetRootRgn() );

		//if( 169560 == a_pRgn->nIndex || 169560 == a_pRgn2->nIndex )
			//a_bSetProcessed = a_bSetProcessed;

		if( 169560 == a_pRgn->nIndex && 168855 == a_pRgn2->nIndex )
			a_bSetProcessed = a_bSetProcessed;


		if( 169560 == a_pRgn2->nIndex && 168855 == a_pRgn->nIndex )
			a_bSetProcessed = a_bSetProcessed;


		//if( 169560 == pRgn2->nIndex	|| 168855 == pRgn2->nIndex )
			//j = j;


/*
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
*/

		LinkAction * pLA = &m_linkActionVect[ m_nofLinkActions++ ];

		pLA->pRgn1 = a_pRgn;
		pLA->pRgn2 = a_pRgn2;
		pLA->pLink1 = &a_rLink;
		pLA->pLink2 = &a_rLink2;


		float distNew;

		{		
			float distRoot;
			//float distNear;			
			{
				RgnInfo * pRgnTmp;
				/*if( NULL != a_pRgn2->GetRootRgn() )
					pRgnTmp = a_pRgn2->GetRootRgn();
				else*/
					pRgnTmp = a_pRgn2;

				distRoot = CalcRgnDif( a_pRgn->GetRootRgn(), pRgnTmp );

				//distNear = CalcRgnDif( a_pRgn, pRgnTmp );
			}

			float distNear = CalcRgnDif( a_pRgn, a_pRgn2 );

			//distNew = distRoot;
			//distNew = ( distRoot + distNear ) / 2;
			//distNew = a_rLink.DistMag;

			float gradVal = *m_srcGrad->GetPixAt(
				a_pRgn2->pos.x, a_pRgn2->pos.y );


			distNew = distRoot;
			//distNew = gradVal * m_difScale;


			//distNew = m_maxDif_Buf[ a_pRgn->nIndex ] * m_difScale;

			//distNew = distNear;


			//distNew = 
				//m_dir_difBuf_Arr [ (int)a_rLink.dir ] [ a_pRgn->nIndex ] * m_difScale;

			

		}


		//a_rLink.DistMag = a_rLink2.DistMag = distNew;
		
		//if( a_bSetProcessed )
		a_rLink.bProcessed = a_rLink2.bProcessed = true;


		//if( pLA->pRgn1->bFstFromBadSrc && pLA->pRgn2->bFromBadSrc )

		//if( pLA->pRgn1->bFstFromBadSrc || pLA->pRgn2->bFstFromBadSrc )
		//if( pLA->pRgn1->bInTraceDam_Pos || pLA->pRgn2->bInTraceDam_Pos )
		//if( NULL == pLA->pRgn1->pShortestEI &&
		//if( 0 == pLA->pRgn1->nTraceRank &&
		//	
		//	//pLA->pRgn1->pShortestEI  != pLA->pRgn2->pShortestEI )
		//	//NULL != pLA->pRgn2->pShortestEI )
		//	0 != pLA->pRgn2->nTraceRank )

		if( pLA->pRgn1->nTraceRank < pLA->pRgn2->nTraceRank )
		{			
			//distNew = GetMaxDif() - 1;
			distNew = distNew;
		}

		m_linkActionMergeQues.PushPtr( distNew, pLA);
		
	}

	bool RegionSegmentor41::HaveConflict( RgnInfo * a_pMinSizRgn, RgnInfo * a_pMaxSizRgn)
	{
		RegionSegmentor41::RgnConflict * pConflict = GetExistingConflict( a_pMinSizRgn, a_pMaxSizRgn);

		if( NULL != pConflict )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	RegionSegmentor41::RgnConflict * RegionSegmentor41::GetExistingConflict( RgnInfo * a_pMinSizRgn, RgnInfo * a_pMaxSizRgn)
	{
		//bool bRet = false;

		APtrList< RgnConflict > & minConfList = 
			a_pMinSizRgn->conflictList;

		RegionSegmentor41::RgnConflict * pConflict = minConfList.Last();

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

	void RegionSegmentor41::RemoveDuplicateConflicts( RgnInfo * a_pRgn )
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


	void RegionSegmentor41::PrepareRgnLinkActions( RgnInfo * a_pRgn, float a_distBef )
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

			//if( pRgn2->bInTrace )
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

			//if( pRgn2->bInTrace )
				//continue;

			PrepareLinkAction( a_pRgn, pRgn2, rLink, rLink2, a_distBef );
		}

	}

	S16ImageRef RegionSegmentor41::GenConflictImg(bool a_bRC, bool a_bRB,
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



	void RegionSegmentor41::CreateConflict( RgnInfo * a_pRgn1, RgnInfo * a_pRgn2, RgnInfo * a_pOrgEdge, EdgeScan::EdgeInfo * a_pEI )
	{
		if( NULL == a_pOrgEdge )
			a_pOrgEdge = a_pOrgEdge;

		RgnConflict * pRc = &m_rgnConflictVect[ m_nofConflicts++ ];
		pRc->pNext = pRc;
		pRc->pPrev = pRc;

		pRc->pEI = a_pEI;
		pRc->pOrgEdge = a_pOrgEdge;
		pRc->pOrgR1 = a_pRgn1;
		pRc->pOrgR2 = a_pRgn2;

		pRc->pPeerRgn = a_pRgn2;
		a_pRgn1->conflictList.PushLast( pRc );

		RgnConflict * pRc2 = &m_rgnConflictVect[ m_nofConflicts++ ];
		pRc2->pNext = pRc2;
		pRc2->pPrev = pRc2;

		pRc2->pEI = a_pEI;
		pRc2->pOrgEdge = a_pOrgEdge;
		pRc2->pOrgR1 = a_pRgn1;
		pRc2->pOrgR2 = a_pRgn2;

		pRc2->pPeerRgn = a_pRgn1;
		a_pRgn2->conflictList.PushLast( pRc2 );
	}


//////////////////////////////////////////////////////////////


	void RegionSegmentor41::MgrOfLinkTypes::CoreStuff::InitLinkTypeMgrs()
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

	void RegionSegmentor41::MgrOfLinkTypes::CoreStuff::InitMaps()
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


	void RegionSegmentor41::ScanImageLines()
	{
		CvSize siz1 = m_src->GetSize();

		//m_conflictScanner = new ConflictScanner2( m_src );

		//const int nHalfAprSize = m_conflictScanner->GetAprSize() / 2;

		//m_conflictScanner = NULL;



		//GlobalStuff::SetLinePathImg( scanImage );


/////////////////////////


		m_scanTryMgr = new ScanTryMgr_4( m_imgDataMgr );
		//m_scanTryMgr = new ScanTryMgr_4( m_src );
		
		//m_scanTryMgr = new ScanTryMgr2( m_src );

		m_maxDif_Buf = & (m_scanTryMgr->GetMaxDif_Arr())[0];

		{
			FixedVector< EdgeScan::ScanDirMgrExtRef > & rScanMgrExt_Arr =
				m_scanTryMgr->GetScanDirExtArr();

			for( int i = 0; i < 4; i++ )
			{
				m_dir_difBuf_Arr[ i ] = m_dir_difBuf_Arr[ i + 4 ] =
					&(rScanMgrExt_Arr[ i * 2 ]->m_difGlobArr)[ 0 ];
			}
		}



		static int dxArr[] = {1, 1, 0, -1};
		static int dyArr[] = {0, 1, 1, 1};

		int nCurTraceProcID = 0;

		FixedVector< EdgeScan::EdgeInfo > & rEdgeInfoArr = 
			m_scanTryMgr->GetEdgeInfoArr();

		EdgeInfoAccessor eiAcc( this );

		CvSize srcSize = m_src->GetSize();




		for( int k=0; k < rEdgeInfoArr.GetSize(); k++ )
		{
			EdgeScan::EdgeInfo & rEI = rEdgeInfoArr[ k ];

			if( rEI.IsCanceled )
				continue;

			eiAcc.SetEdgeInfo( & rEI );


			{
				//float * difBuf = rEI.dif_HeadPtr;

				//const float difVal = difBuf[ rEI.EdgeIOL ] ;

				//if( difVal < 20 )
				//if( rEI.Scaled_EdgeVal < 17 )
				if( ! rEI.DoesConflict() )					
				{
					//rEI.IsCanceled = true;
					
					//continue;
				}

			}


			RgnInfo * pRgnEdge = eiAcc.GetEdge_Rgn();

			RgnInfo * pRootRgn1 = eiAcc.GetR1_Rgn();
			RgnInfo * pRootRgn2 = eiAcc.GetR2_Rgn();


			{
				
				float dif_Edge = m_maxDif_Buf[ pRgnEdge->nIndex ];

				float dif_R1 = m_maxDif_Buf[ pRootRgn1->nIndex ];
				float dif_R2 = m_maxDif_Buf[ pRootRgn2->nIndex ];
				
				if( dif_Edge < dif_R1 )
				{
					dif_Edge = dif_Edge;

					rEI.IsCanceled = true;
					continue;
				}

				if( dif_Edge < dif_R2 )
				{
					dif_Edge = dif_Edge;

					rEI.IsCanceled = true;
					continue;
				}

				float dif_R_Max = 0;

				if( dif_R1 > dif_R_Max )
					dif_R_Max = dif_R1;

				if( dif_R2 > dif_R_Max )
					dif_R_Max = dif_R2;

				dif_R_Max *= m_difScale;
				
				m_edgeInfo_Ques.PushPtr( dif_R_Max, & rEI );
			}
		}



		{
			ImageWatershedMgrRef iwm1 = new ImageWatershedMgr(
				m_maxDif_Buf, m_src->GetSize(), (int)m_difScale );


			while(true)
			{
				EdgeScan::EdgeInfo * pEI = m_edgeInfo_Ques.PopPtrMin();

				if( NULL == pEI )
					break;

				EdgeScan::EdgeInfo & rEI = *pEI;

				if( rEI.IsCanceled )
					continue;

				int nLast_EI_Idx = m_edgeInfo_Ques.GetLastMinIndex();

				float floodVal = (float)nLast_EI_Idx / m_difScale;

				//floodVal *= 0.8;

				//if( 19902 == rEI.Index )
				if( 6442 == rEI.Index )
					rEI.Index = rEI.Index;

				eiAcc.SetEdgeInfo( & rEI );

				RgnInfo * pRootRgn1 = eiAcc.GetR1_Rgn();
				RgnInfo * pRootRgn2 = eiAcc.GetR2_Rgn();

				//iwm1->FloodToVal( floodVal );

				//if( iwm1->AreTogether(
				//	pRootRgn1->nIndex, pRootRgn2->nIndex ) )
				//{
				//	rEI.IsCanceled = true;
				//	continue;
				//}



				if( ! pRootRgn1->IsRoot() )
				{
					pRootRgn1->BeARoot();		

					//if( pRootRgn1->bInTrace )
					//{
					//	pRootRgn1->bIsPassiveRoot = true;
					//}
					//else
					{
						PrepareRgnLinkActions( pRootRgn1, 0 );
					}
				}


				if( ! pRootRgn2->IsRoot() )
				{
					pRootRgn2->BeARoot();		

					//if( pRootRgn2->bInTrace )
					//{
					//	pRootRgn2->bIsPassiveRoot = true;
					//}
					//else
					{
						PrepareRgnLinkActions( pRootRgn2, 0 );
					}
				}



				//if( pRgnAct1 == pRgnAct2 )
					//continue;

				CreateConflict( pRootRgn1, pRootRgn2, eiAcc.GetEdge_Rgn(), & rEI  );
			}			

		}







////////-------------------------------------------





		const float max_SpaceDist = sqrt(
			(float)Sqr(srcSize.width) + Sqr(srcSize.height));

		MultiListQueMgr< EdgeScan::EdgeInfo > ei_Space_Ques;
		
		ei_Space_Ques.InitSize( (max_SpaceDist + 100) * 10 );



		for( int k=0; k < rEdgeInfoArr.GetSize(); k++ )
		{
			EdgeScan::EdgeInfo & rEI = rEdgeInfoArr[ k ];

			//rEI.InrR1_IOL = rEI.Med1_IOL;
			//rEI.InrR2_IOL = rEI.Med2_IOL;

			if( rEI.IsCanceled )
				continue;

			eiAcc.SetEdgeInfo( & rEI );

			RgnInfo * pRootRgn1 = eiAcc.GetR1_Rgn();
			RgnInfo * pRootRgn2 = eiAcc.GetR2_Rgn();

			rEI.RootSpaceDist = F32Point::Sub(
				pRootRgn1->pos, pRootRgn2->pos ).CalcMag();

			ei_Space_Ques.PushPtr( rEI.RootSpaceDist * 10, & rEI);


/*
			for( int i = rEI.Med1_IOL + 1; i < rEI.Med2_IOL; i++ )
			{
				RgnInfo * pRgn = eiAcc.GetRgnOf_IOL( i );


				if( ! pRgn->bInTrace )							
					m_rgnInTraceArr.PushBack( pRgn );

				pRgn->bInTrace = true;


				if( NULL == pRgn->pShortestEI )
				{
					pRgn->pShortestEI = & rEI;
				}
				//else if( rEI.RootSpaceDist < pRgn->minRootSpaceDist )
				else if( rEI.RootSpaceDist < pRgn->pShortestEI->RootSpaceDist )
				{
					//pRgn->minRootSpaceDist = rEI.RootSpaceDist;
					pRgn->pShortestEI = & rEI;
				}

			}
*/

		}






		{
			const int nMax_TraceRank = 10000000;

			FixedVector<RgnInfo *> traceRgn_Arr(3000);

			IndexCalc2D idx_2D( m_src->GetSize().width,
				m_src->GetSize().height );


			F32Point testPnt( 17, 353 );

			while( true )
			{
				EdgeScan::EdgeInfo * pEI = ei_Space_Ques.PopPtrMin();

				if( NULL == pEI )
				{
					break;
				}

				EdgeScan::EdgeInfo & rEI = *pEI;

				if( rEI.IsCanceled )
					continue;

				eiAcc.SetEdgeInfo( & rEI );

				traceRgn_Arr.ResetSize();

				RgnInfo * pRootRgn1 = eiAcc.GetR1_Rgn();
				RgnInfo * pRootRgn2 = eiAcc.GetR2_Rgn();





				bool bGood = true;

				if( 8867 == rEI.Index )
					bGood = bGood;

				RgnInfo * pRgnOld = NULL;

				int nMin_RankNotZero = 0;

				//for( int i = rEI.Med1_IOL; i <= rEI.Med2_IOL; i++ )
				for( int i = rEI.InrR1_IOL; i <= rEI.InrR2_IOL; i++ )
				{
					RgnInfo * pRgn = eiAcc.GetRgnOf_IOL( i );

					if( 5 == rEI.Dir &&
						F32Point::Sub( testPnt, pRgn->pos ).CalcMag() < 2.5 ) 
						//m_nTestRgnIdx == pRgn->nIndex )
						i = i;

				
					RgnInfo * pRgn2 = pRgn;

					if(NULL != pRgnOld)
					{
						//pRgnEx = NULL;

						F32Point difPnt = F32Point::Sub( 
							pRgn->pos, pRgnOld->pos );

						Hcpl_ASSERT( fabs( difPnt.x ) <= 1 && fabs( difPnt.y ) <= 1 );


						if(fabs( difPnt.x ) > 0.2 && fabs( difPnt.y ) > 0.2)
						{
							F32Point posEx(
								pRgnOld->pos.x, pRgn->pos.y);

							RgnInfo * pRgnEx = & m_rgnInfoVect[ idx_2D.Calc(posEx.x, posEx.y) ];

							pRgn2 = pRgnEx;
						}

					}


					while(true)
					{

						//	Intersection !
						//if( pRgn2->bInTraceDam_Pos
						if( 0 != pRgn2->nTraceRank
												
							
							//&&

							//rEI.RootSpaceDist > 1.3 * pRgn->pShortestEI->RootSpaceDist
							)							
						{
							//bGood = false;					
							//goto Mark_Good_Trace;

							if( pRgn2->nTraceRank > 0 )
							{
								if( 0 == nMin_RankNotZero )
								{
									nMin_RankNotZero = pRgn2->nTraceRank;
								}
								else if( pRgn2->nTraceRank < nMin_RankNotZero )
								{
									nMin_RankNotZero = pRgn2->nTraceRank;
								}
								}

						}

						traceRgn_Arr.PushBack(pRgn2);

						if(pRgn2 == pRgn)
						{
							break;
						}
						else
						{
							//pRgnEx = NULL;
							pRgn2 = pRgn;
						}

					}


					pRgnOld = pRgn;
				}


	Mark_Good_Trace:

				if( ! bGood )
					continue;

				int nGood_TraceRank = nMax_TraceRank;;

				if( nMin_RankNotZero > 0 )
					nGood_TraceRank = nMin_RankNotZero - 1;



				for(int i=0; i < traceRgn_Arr.GetSize(); i++)
				{
					RgnInfo * pRgn = traceRgn_Arr[ i ];

					//pRgn->bInTraceDam_Pos = true;

					if( pRgn->nTraceRank > 0 )
						continue;

					pRgn->nTraceRank = nGood_TraceRank;

					//if( NULL == pRgn->pShortestEI )
						//pRgn->pShortestEI = & rEI;
				}



			}	//	while
		}





/////////--------------------------------------------





		for( int k=0; k < rEdgeInfoArr.GetSize(); k++ )
		{
			EdgeScan::EdgeInfo & rEI = rEdgeInfoArr[ k ];

			if( rEI.IsCanceled )
				continue;

			eiAcc.SetEdgeInfo( & rEI );


			RgnInfo * pRgnEdge = eiAcc.GetEdge_Rgn();

			RgnInfo * pRootRgn1 = eiAcc.GetR1_Rgn();
			RgnInfo * pRootRgn2 = eiAcc.GetR2_Rgn();


			if( ! pRootRgn1->IsRoot() )
			{
				pRootRgn1->BeARoot();		

				//if( pRootRgn1->bInTrace )
				//{
				//	pRootRgn1->bIsPassiveRoot = true;
				//}
				//else
				{
					PrepareRgnLinkActions( pRootRgn1, 0 );
				}
			}


			if( ! pRootRgn2->IsRoot() )
			{
				pRootRgn2->BeARoot();		

				//if( pRootRgn2->bInTrace )
				//{
				//	pRootRgn2->bIsPassiveRoot = true;
				//}
				//else
				{
					PrepareRgnLinkActions( pRootRgn2, 0 );
				}
			}

			//CreateConflict( pRootRgn1, pRootRgn2, eiAcc.GetEdge_Rgn(), & rEI  );
		}



		{
			FixedVector< EdgeScan::PixInfo > & rPixInfoArr = 
				m_scanTryMgr->GetPixInfoArr();

			for( int i=0; i < rPixInfoArr.GetSize(); i++ )
			{
				EdgeScan::PixInfo & rPI = rPixInfoArr[ i ];

				//if( rPI.maxSlopeVal > 15 )

				RgnInfo * pRgn = & m_rgnInfoVect[ i ];

				if( pRgn->IsRoot() )
					continue;

				if( rPI.maxSlopeVal > 35 )
				{
					pRgn->bIsPassiveRoot = true;
					continue;
				}

				pRgn->BeARoot();		

				//if( pRgn->bInTrace )
				//{
				//	pRgn->bIsPassiveRoot = true;
				//}
				//else
				{
					PrepareRgnLinkActions( pRgn, 0 );
				}
			}

		}


		


		RgnInfo * pRgn_R1_Test = & m_rgnInfoVect[ 251299 ];

		F32ImageRef scanImage = this->GenScanImage();
	}



	F32ImageRef RegionSegmentor41::GenScanImage()
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




	F32ImageRef RegionSegmentor41::GenBadSrcImage()
	{
		F32ImageRef ret = F32Image::Create( m_src->GetSize(), 3 );

		ret->SetAll( 0 );

		for( int i=0; i < m_rgnInfoVect.GetSize(); i++ )
		{
			RgnInfo * pRgn = &m_rgnInfoVect[ i ];

			//F32ColorVal * pix = (F32ColorVal *)ret->GetPixAt( pRgn->pos.x, pRgn->pos.y );

			if( pRgn->bFromBadSrc )
				//pix->AssignVal( 255, 255, 255 );
				DrawCircle( ret, pRgn->pos, 
					//u8ColorVal( 255, 255, 255 ), 2 );
					u8ColorVal( 180, 180, 180 ), 2 );
		}

		for( int i=0; i < m_rgnInfoVect.GetSize(); i++ )
		{
			RgnInfo * pRgn = &m_rgnInfoVect[ i ];

			F32ColorVal * pix = (F32ColorVal *)ret->GetPixAt( pRgn->pos.x, pRgn->pos.y );

			if( pRgn->bFstFromBadSrc )
				//pix->AssignVal( 0, 255, 0 );
				DrawCircle( ret, pRgn->pos, 
					u8ColorVal( 0, 255, 0 ), 2 );
		}

		return ret;
	}

	F32ImageRef RegionSegmentor41::GenRootRgnImage()
	{
		F32ImageRef ret = F32Image::Create( m_src->GetSize(), 3 );

		for( int i=0; i < m_rgnInfoVect.GetSize(); i++ )
		{
			RgnInfo * pRgn = &m_rgnInfoVect[ i ];

			F32ColorVal * pix = (F32ColorVal *)ret->GetPixAt( pRgn->pos.x, pRgn->pos.y );

			if( pRgn->IsRoot() )
			{
				//if( 284516 == pRgn->nIndex )

				pix->AssignVal( 255, 255, 255 );
			}
			else
			{
				pix->AssignVal( 0, 0, 0 );
			}
		}

/*
		//if( 282873 == pRgn->nIndex )
		{
			{
				RgnInfo * pEdgeRgn = &m_rgnInfoVect[ 230998 ];			

				DrawCircle( ret, pEdgeRgn->pos, u8ColorVal( 255, 155, 0 ), 5 );
			}

			{
				RgnInfo * pMed1_Rgn = &m_rgnInfoVect[ 204649 ];			

				DrawCircle( ret, pMed1_Rgn->pos, u8ColorVal( 255, 0, 155 ), 5 );
			}

			{
				RgnInfo * pMed2_Rgn = &m_rgnInfoVect[ 340512 ];			

				DrawCircle( ret, pMed2_Rgn->pos, u8ColorVal( 255, 0, 155 ), 5 );
			}

			RgnInfo * pRgn = &m_rgnInfoVect[ 282873 ];

			DrawCircle( ret, pRgn->pos, u8ColorVal( 0, 255, 0 ), 5 );
			//pix->AssignVal( 0, 255, 0 );


			RgnInfoListElm * pRootElm = pRgn->pTraceRootElm;

			if( NULL != pRootElm )
			{
				UpdateActRgn( pRgn );
				RgnInfo * pActRgn_0 = pRgn->GetActRgn();

				bool bMergeOK = false;

				do
				{
					UpdateActRgn( pRootElm->pRgn );
					RgnInfo * pActRgnR = pRootElm->pRgn->GetActRgn();

					if(	pActRgnR == pActRgn_0 )
					{
						bMergeOK = true;
						break;
					}

					DrawCircle( ret, pRootElm->pRgn->pos, 
						u8ColorVal( 255, 0, 255 ), 5 );

					pRootElm = pRootElm->pNext;
				}while( NULL != pRootElm );

			}

		}
*/



		return ret;
	}



	

	F32ImageRef RegionSegmentor41::GenTraceDamImage()
	{
		F32ImageRef ret = F32Image::Create( m_src->GetSize(), 3 );

		for( int i=0; i < m_rgnInfoVect.GetSize(); i++ )
		{
			RgnInfo * pRgn = &m_rgnInfoVect[ i ];

			F32ColorVal * pix = (F32ColorVal *)ret->GetPixAt( 
				pRgn->pos.x, pRgn->pos.y );

			//if( pRgn->bInTraceDam_Pos )
			if( 0 != pRgn->nTraceRank )				
			{
				//if( 284516 == pRgn->nIndex )

				pix->AssignVal( 255, 255, 255 );
			}
			else
			{
				pix->AssignVal( 0, 0, 0 );
			}
		}

/*
		//if( 282873 == pRgn->nIndex )
		{
			{
				RgnInfo * pEdgeRgn = &m_rgnInfoVect[ 230998 ];			

				DrawCircle( ret, pEdgeRgn->pos, u8ColorVal( 255, 155, 0 ), 5 );
			}

			{
				RgnInfo * pMed1_Rgn = &m_rgnInfoVect[ 204649 ];			

				DrawCircle( ret, pMed1_Rgn->pos, u8ColorVal( 255, 0, 155 ), 5 );
			}

			{
				RgnInfo * pMed2_Rgn = &m_rgnInfoVect[ 340512 ];			

				DrawCircle( ret, pMed2_Rgn->pos, u8ColorVal( 255, 0, 155 ), 5 );
			}

			RgnInfo * pRgn = &m_rgnInfoVect[ 282873 ];

			DrawCircle( ret, pRgn->pos, u8ColorVal( 0, 255, 0 ), 5 );
			//pix->AssignVal( 0, 255, 0 );


			RgnInfoListElm * pRootElm = pRgn->pTraceRootElm;

			if( NULL != pRootElm )
			{
				UpdateActRgn( pRgn );
				RgnInfo * pActRgn_0 = pRgn->GetActRgn();

				bool bMergeOK = false;

				do
				{
					UpdateActRgn( pRootElm->pRgn );
					RgnInfo * pActRgnR = pRootElm->pRgn->GetActRgn();

					if(	pActRgnR == pActRgn_0 )
					{
						bMergeOK = true;
						break;
					}

					DrawCircle( ret, pRootElm->pRgn->pos, 
						u8ColorVal( 255, 0, 255 ), 5 );

					pRootElm = pRootElm->pNext;
				}while( NULL != pRootElm );

			}

		}
*/



		return ret;
	}


	F32ImageRef RegionSegmentor41::GenRgnRootDifImage()
	{
		F32ImageRef ret = F32Image::Create( m_src->GetSize(), 3 );

		float maxVal = 0;

		for( int i=0; i < m_rgnInfoVect.GetSize(); i++ )
		{
			RgnInfo * pRgn = &m_rgnInfoVect[ i ];

			F32ColorVal * pix = (F32ColorVal *)ret->GetPixAt( pRgn->pos.x, pRgn->pos.y );

			float val = m_imgDataMgr->CalcDif(
				pRgn->nIndex,
				pRgn->GetRootRgn()->nIndex);


			if( pRgn != pRgn->GetRootRgn() )
				val = val;


			if( val > maxVal )
				maxVal = val;

			pix->AssignVal( val, val, val );
		}

		return ret;
	}

	
	void RegionSegmentor41::ResetBadSrcWork()
	{
		Hcpl_ASSERT( false );
		
		for( int i=0; i < m_rgnOfBadSrcArr.GetSize(); i++ )
		{
			RgnInfo * pRgn = m_rgnOfBadSrcArr[ i ];

			pRgn->bFromBadSrc = false;
			pRgn->bFromBadSrc_Org = false;

			pRgn->bFrom_MaybeBadSrc = false;			

			pRgn->bFromSureBadSrc = false;

			pRgn->m_pDeepSrcBadRgn = NULL;
		}
		m_rgnOfBadSrcArr.ResetSize();
	}


	void RegionSegmentor41::MarkRgnsFromBadSrc()
	{
		RgnInfo * pTestRgn = & m_rgnInfoVect[ m_nTestRgnIdx ];
		

		FixedVector< EdgeScan::EdgeInfo > & rEdgeInfoArr = 
			m_scanTryMgr->GetEdgeInfoArr();

		EdgeInfoAccessor eiAcc( this );



		for( int k=0; k < rEdgeInfoArr.GetSize(); k++ )
		{
			EdgeScan::EdgeInfo & rEI = rEdgeInfoArr[ k ];

			if( rEI.IsCanceled )
				continue;

			eiAcc.SetEdgeInfo( & rEI );

			RgnInfo * pRgn_R1 = eiAcc.GetR1_Rgn();
			RgnInfo * pRgn_R2 = eiAcc.GetR2_Rgn();

			int nIdx1 = pRgn_R1->GetActRgn()->nIndex;
			int nIdx2 = pRgn_R2->GetActRgn()->nIndex;

			if( nIdx1 < nIdx2 )
			{
				rEI.Act_Min_Idx = nIdx1;
				rEI.Act_Max_Idx = nIdx2;
			}
			else
			{
				rEI.Act_Min_Idx = nIdx2;
				rEI.Act_Max_Idx = nIdx1;
			}
		}


		for( int k=0; k < rEdgeInfoArr.GetSize(); k++ )
		{
			EdgeScan::EdgeInfo & rEI = rEdgeInfoArr[ k ];

			if( rEI.IsCanceled )
				continue;


			eiAcc.SetEdgeInfo( & rEI );


			rEI.HasBadSrc_Trace = true;

			for( int i = rEI.Med1_IOL + 1; i < rEI.Med2_IOL; i++ )
			{
				RgnInfo * pRgn = eiAcc.GetRgnOf_IOL( i );

				if( pRgn->pShortestEI->HasSameActs( rEI ) )
					continue;

				//if( ( rEI.RootSpaceDist / pRgn->minRootSpaceDist ) >= 1.3 )
				//if( ( rEI.RootSpaceDist / pRgn->pShortestEI->RootSpaceDist ) >= 1.3 )
				
				if( rEI.RootSpaceDist > pRgn->pShortestEI->RootSpaceDist )
				{
					rEI.HasBadSrc_Trace = false;
					break;
				}
			}

		}









		for( int k=0; k < rEdgeInfoArr.GetSize(); k++ )
		{
			EdgeScan::EdgeInfo & rEI = rEdgeInfoArr[ k ];


			if( rEI.IsCanceled )
				continue;

			if( ! rEI.HasBadSrc_Trace )
				continue;


			eiAcc.SetEdgeInfo( & rEI );

			RgnInfo * pRootRgn1 = eiAcc.GetR1_Rgn();
			RgnInfo * pRootRgn2 = eiAcc.GetR2_Rgn();

			for( int i = rEI.Med1_IOL + 1; i < rEI.Med2_IOL; i++ )
			{
				RgnInfo * pRgn = eiAcc.GetRgnOf_IOL( i );

				{
					m_traceRootAllocVect.IncSize();

					RgnInfoListElm * pElm = &m_traceRootAllocVect.GetBack();

					pElm->pRgn = pRootRgn1;

					pElm->pNext = pRgn->pTraceRootElm;
					pRgn->pTraceRootElm = pElm;
				}

				{
					m_traceRootAllocVect.IncSize();

					RgnInfoListElm * pElm = &m_traceRootAllocVect.GetBack();

					pElm->pRgn = pRootRgn2;

					pElm->pNext = pRgn->pTraceRootElm;
					pRgn->pTraceRootElm = pElm;
				}

			}









		}




		//m_rgnOfBadSrcArr.SetCapacity( m_rgnInTraceArr.GetSize() );

		//F32Point testPnt( 278, 338 );
		//F32Point testPnt( 518, 134 );
		//F32Point testPnt( 86, 261 );

		//F32Point testPnt( 555, 135 );

		//F32Point testPnt( 563, 123 );
		//F32Point testPnt( 554, 120 );
		F32Point testPnt( 289, 781 );



		for( int i=0; i < m_rgnInTraceArr.GetSize(); i++ )			
		{
			RgnInfo * pRgn = m_rgnInTraceArr[ i ];			

			//if( pRgn->bInNativeTrace )
				//continue;

			//if( F32Point::Sub( testPnt, pRgn->pos ).CalcMag() < 3 ) 
			if( F32Point::Sub( testPnt, pRgn->pos ).CalcMag() < 0.5 ) 
				i = i;


			RgnInfoListElm * pRootElm = pRgn->pTraceRootElm;

			Hcpl_ASSERT( ! pRgn->IsRoot() || pRgn->bIsPassiveRoot );

			if( NULL == pRootElm )
				continue;

			//UpdateActRgn( pRgn->GetRootRgn() );
			UpdateActRgn( pRgn );
			RgnInfo * pActRgn_0 = pRgn->GetActRgn();


			RgnInfo * rootActArr[ 2 ];
			int nActCnt = 0;


			bool bMergeOK = false;

			do
			{
				UpdateActRgn( pRootElm->pRgn );
				RgnInfo * pActRgnR = pRootElm->pRgn->GetActRgn();


				if( pRgn->bInIntersection )
				{

				}
				else if( 0 == nActCnt )
				{
					rootActArr[ nActCnt++ ] = pActRgnR;
				}
				//else if( 1 == nActCnt )
				else
				{
					int k=0;

					for( ; k < nActCnt; k++ )
					{
						if( pActRgnR == rootActArr[ k ] )
							break;
					}

					if( k == nActCnt )								
					{
						if( nActCnt < 2 )
							rootActArr[ nActCnt++ ] = pActRgnR;
						else
						{
							//bOK = false;
							pRgn->bInIntersection = true;
						}
					}
				}




				if(	pActRgnR == pActRgn_0 )
				{
					bMergeOK = true;
					//break;
				}

				pRootElm = pRootElm->pNext;
			}while( NULL != pRootElm );


			if( bMergeOK )
			{
				pRgn->bInNativeTrace = true;
			}
			else
			{
				Hcpl_ASSERT( ! pRgn->bFstFromBadSrc );

				pRgn->bFrom_MaybeBadSrc = true;
			}

			pRgn->bFrom_MaybeBadSrc = true;
		}


////////////////////////////////////////////////////////////////////


		//FixedVector< EdgeScan::EdgeInfo > & rEdgeInfoArr = 
			//m_scanTryMgr->GetEdgeInfoArr();

		//EdgeInfoAccessor eiAcc( this );

		for( int k=0; k < rEdgeInfoArr.GetSize(); k++ )
		{
			EdgeScan::EdgeInfo & rEI = rEdgeInfoArr[ k ];

			if( rEI.IsCanceled )
				continue;



			eiAcc.SetEdgeInfo( & rEI );

			RgnInfo * pActRgn_R1 = eiAcc.GetR1_Rgn()->GetActRgn();

			RgnInfo * pActRgn_R2 = eiAcc.GetR2_Rgn()->GetActRgn();

			{
				RgnInfo * pActRgn_Med1 = eiAcc.GetMed1_Rgn()->GetActRgn();

				if( pActRgn_Med1 != pActRgn_R1 )
				{
					rEI.HasBadSrc_Trace = false;
					continue;
				}
			}
			{
				RgnInfo * pActRgn_Med2 = eiAcc.GetMed2_Rgn()->GetActRgn();

				if( pActRgn_Med2 != pActRgn_R2 )
				{
					rEI.HasBadSrc_Trace = false;
					continue;
				}
			}


			typedef enum
			{
				Native_1, Bad, Native_2
			}BadTrace_State;

			BadTrace_State state = BadTrace_State::Native_1;

			//rEI.HasBadSrc_Trace = true;

			//for( int i = rEI.InrR1_IOL + 1; i < rEI.InrR2_IOL; i++ )
			for( int i = rEI.Med1_IOL + 1; i < rEI.Med2_IOL; i++ )
			{
				RgnInfo * pRgn = eiAcc.GetRgnOf_IOL( i );


				if( pRgn->bInIntersection )
					rEI.IsIntersected = true;

				if( rEI.IsIntersected )
				{
					rEI.HasBadSrc_Trace = false;
					break;
				}


				UpdateActRgn( pRgn );
				RgnInfo * pActRgn_I = pRgn->GetActRgn();

				if( BadTrace_State::Native_1 == state )
				{
					if( pActRgn_I == pActRgn_R1 )
					{

					}
					else if( pActRgn_I == pActRgn_R2 )
					{
						rEI.HasBadSrc_Trace = false;
					}
					else if( ! pRgn->bFrom_MaybeBadSrc )
					{
						rEI.HasBadSrc_Trace = false;
					}
					else
					{
						state = BadTrace_State::Bad;
					}
				}
				else if( BadTrace_State::Bad == state )
				{
					if( pActRgn_I == pActRgn_R1 )
					{
						rEI.HasBadSrc_Trace = false;
					}
					else if( pActRgn_I == pActRgn_R2 )
					{
						state = BadTrace_State::Native_2;
					}
					else if( ! pRgn->bFrom_MaybeBadSrc )
					{
						rEI.HasBadSrc_Trace = false;
					}
				}
				else if( BadTrace_State::Native_2 == state )
				{
					if( pActRgn_I != pActRgn_R2 )
					{
						rEI.HasBadSrc_Trace = false;
					}
				}

			}

			//rEI.HasBadSrc_Trace = true;


		}



		for( int k=0; k < rEdgeInfoArr.GetSize(); k++ )
		{
			EdgeScan::EdgeInfo & rEI = rEdgeInfoArr[ k ];

			if( rEI.IsCanceled )
				continue;

			if( ! rEI.HasBadSrc_Trace )
				continue;

			eiAcc.SetEdgeInfo( & rEI );


			RgnInfo * pActRgn_R1 = eiAcc.GetR1_Rgn()->GetActRgn();

			RgnInfo * pActRgn_R2 = eiAcc.GetR2_Rgn()->GetActRgn();


			//for( int i = rEI.InrR1_IOL + 1; i < rEI.InrR2_IOL; i++ )
			for( int i = rEI.Med1_IOL + 1; i < rEI.Med2_IOL; i++ )
			{
				RgnInfo * pRgn = eiAcc.GetRgnOf_IOL( i );

				Hcpl_ASSERT( ! pRgn->IsRoot() || pRgn->bIsPassiveRoot );

				//if( pRgn->IsRoot() )
					//continue;

				//Hcpl_ASSERT( ! pRgn->IsRoot() );

				//if( ! pRgn->bFrom_MaybeBadSrc )
					//continue;

				UpdateActRgn( pRgn );
				RgnInfo * pActRgn_I = pRgn->GetActRgn();

				if( pActRgn_I == pActRgn_R1 ||
					 pActRgn_I == pActRgn_R2 )
				{
					continue;
				}

				if( m_nTestRgnIdx == pRgn->nIndex )
					i = i;


				//if( ! pRgn->bFrom_MaybeBadSrc )
					//continue;


				if( pRgn->bFromBadSrc )
					continue;

				//Hcpl_ASSERT( ! pRgn->IsRoot() );
				if( pRgn->IsRoot() )
					continue;

				pRgn->bFromBadSrc = true;
				pRgn->bFromBadSrc_Org = true;

				m_rgnOfBadSrcArr.PushBack( pRgn );
			}

		}




///////////////////////////////////////////////////////////////////////



		m_difScale = m_difScale;

///*

		//	2nd for
		for( int i=0; i < m_rgnOfBadSrcArr.GetSize(); i++ )			
		{
			RgnInfo * pRgn = m_rgnOfBadSrcArr[ i ];			

			if( ! pRgn->bFromBadSrc )
				continue;
			
			if( pRgn->bFstFromBadSrc )
			{
				//Hcpl_ASSERT( false );
				continue;
			}

			UpdateActRgn( pRgn );

			for( int j=0; j < 8; j++ )
			//for( int j=0; j < 8; j += 2 )
			{
				RgnLink & rLink = pRgn->links[ j ];

				if( ! rLink.bExists )
					continue;

				RgnInfo * pRgn2 = rLink.pPeerRgn;

				if( pRgn2->bFromBadSrc )
					continue;

				if( pRgn2->bFstFromBadSrc )
				{
					j = j;

					//Hcpl_ASSERT( false );
					continue;
				}


				UpdateActRgn( pRgn2 );

				//F32Point testPnt( 311, 372 );
				//F32Point testPnt( 294, 362 );
				F32Point testPnt( 86, 261 );


				if( pRgn2->GetActRgn() != pRgn->GetActRgn() )
					continue;

				//if( pRgn2->GetActRgn() == pRgn->GetActRgn() )
				{
					if( 282873 == pRgn2->nIndex )
						i = i;

					//if( NULL != pRgn2->m_pDeepSrcBadRgn )
						//continue;

					bool bOK = true;
					{
						RgnInfoListElm * pRootElm = pRgn2->pTraceRootElm;

						//Hcpl_ASSERT( NULL != pRootElm );

						RgnInfo * rootActArr[ 2 ];
						int nActCnt = 0;

						//UpdateActRgn( pRgn->GetRootRgn() );
						//UpdateActRgn( pRgn );
						//RgnInfo * pActRgn_0 = pRgn->GetActRgn();

						//bool bMergeOK = false;

						//do
						//while( NULL != pRootElm )
						while( false )
						{
							UpdateActRgn( pRootElm->pRgn );
							RgnInfo * pActRgnR = pRootElm->pRgn->GetActRgn();

							if( 0 == nActCnt )
							{
								rootActArr[ nActCnt++ ] = pActRgnR;
							}
							//else if( 1 == nActCnt )
							else
							{
								int k=0;

								for( ; k < nActCnt; k++ )
								{
									if( pActRgnR == rootActArr[ k ] )
										break;
								}

								if( k == nActCnt )								
								{
									if( nActCnt < 2 )
										rootActArr[ nActCnt++ ] = pActRgnR;
									else
									{
										bOK = false;
										break;
									}
								}
							}

							pRootElm = pRootElm->pNext;
						//}while( NULL != pRootElm );
						}


					}

					bOK = true;

					if( bOK )
					{
						pRgn->bFstFromBadSrc = true;
						//pRgn2->bFstFromBadSrc = true;

						m_rgnOf_FstBadSrcArr.PushBack( pRgn );
						//m_rgnOf_FstBadSrcArr.PushBack( pRgn2 );
					}


					

					if( F32Point::Sub( testPnt, pRgn->pos ).CalcMag() < 3 ) 
						j = j;

					if( F32Point::Sub( testPnt, pRgn2->pos ).CalcMag() < 3 ) 
						j = j;


					
				}
			}
		}	//	end 2nd for


/////////////////////////////////////////////


		//testPnt = F32Point( 383, 525 );
		//testPnt = F32Point( 91, 252 );

		



		m_fillDeq.ResetSize();

///*

		m_nCurGrpID = -1;

		for( int i=0; i < m_rgnOf_FstBadSrcArr.GetSize(); i++ )			
		{
			RgnInfo * pRgn = m_rgnOf_FstBadSrcArr[ i ];			

			if( ! pRgn->bFstFromBadSrc )
				continue;

			if( -1 != pRgn->nFstBadGrpID )
				continue;

			m_nCurGrpID++;

			pRgn->nFstBadGrpID = m_nCurGrpID;


			m_fillDeq.ResetSize();

			m_fillDeq.PushBack( pRgn );

			m_rgnOf_FstBadSrcArr_2.PushBack( pRgn );



			while( m_fillDeq.GetSize() > 0 )
			{
				RgnInfo * pRgn1_1 = m_fillDeq.GetFront();
				m_fillDeq.PopFront();

				//Hcpl_ASSERT( pRgn1_1->bFstFromBadSrc );

				//Hcpl_ASSERT( -1 == pRgn1_1->nFstBadGrpID );

				{
					//pRgn1_1->nFstBadGrpID = m_nCurGrpID;
					
					//pRgn1_1->pFillInfo = pFillInfo;

					//pRgn1_1->bBadFillDone = true;

					//curFillRgnArr.PushBack( pRgn1_1 );

				}


				for( int j=0; j < 8; j++ )
				//for( int j=0; j < 8; j += 2 )
				{
					RgnLink & rLink = pRgn1_1->links[ j ];

					if( ! rLink.bExists )
						continue;

					RgnInfo * pRgn2 = rLink.pPeerRgn;

					//if( pRgn2->GetActRgn() != pLastActRgn )
						//continue;

					if( ! pRgn2->bFstFromBadSrc )
						continue;

					if( -1 != pRgn2->nFstBadGrpID )
					{
						//Hcpl_ASSERT( m_nCurGrpID == pRgn2->nFstBadGrpID );

						continue;
					}

					//Hcpl_ASSERT( pRgn2->GetActRgn() == pRgn1_1->GetActRgn() );


					// Try 12Aug11
					//if( pRgn2->GetActRgn() != pRgn1_1->GetActRgn() )
						//continue;


					pRgn2->nFstBadGrpID = m_nCurGrpID;

					m_fillDeq.PushBack( pRgn2 );

					m_rgnOf_FstBadSrcArr_2.PushBack( pRgn2 );
				}

			}




		}



		FixedVector< RgnInfo * > curFillRgnArr( 50000 );

		m_fstFromBad_EI_Arr.ResetSize();


///*
		int nCur_Fill_Cycle = 0;

		int nCur_Unfill_Cycle = 0;

		// Fillings
		//for( int i=0; i < m_rgnOf_FstBadSrcArr.GetSize(); i++ )			
		for( int i=0; i < m_rgnOf_FstBadSrcArr_2.GetSize(); i++ )			
		{
			//RgnInfo * pRgn = m_rgnOf_FstBadSrcArr[ i ];			
			RgnInfo * pRgn = m_rgnOf_FstBadSrcArr_2[ i ];			

			if( ! pRgn->bFstFromBadSrc )
				continue;

			if( pRgn->bBadFillDone )
				continue;

			//Hcpl_ASSERT( ! pRgn->bFromBadSrc );
			
			curFillRgnArr.ResetSize();

			
			m_fillInfoVect.IncSize();

			FillInfo * pFillInfo = & m_fillInfoVect.GetBack();

			UpdateActRgn( pRgn );

			RgnInfo * pLastActRgn = pRgn->GetActRgn();

			EdgeScan::EdgeInfo * pFstShort_EI = pRgn->pShortestEI;

			const int nCurBadGrpID = pRgn->nFstBadGrpID;

			m_fillDeq.ResetSize();

			m_unfill_Deq.ResetSize();

			nCur_Fill_Cycle++;
			
			pRgn->nFill_Cycle = nCur_Fill_Cycle;

			m_fillDeq.PushBack( pRgn );


			for( ; i < m_rgnOf_FstBadSrcArr_2.GetSize(); i++ )
			{
				RgnInfo * pRgn3 = m_rgnOf_FstBadSrcArr_2[ i ];			

				if( pRgn3->bBadFillDone )
					continue;

				Hcpl_ASSERT( pRgn3->bFstFromBadSrc );

				Hcpl_ASSERT( ! pRgn3->bBadFillDone );

				if( nCurBadGrpID != pRgn3->nFstBadGrpID )
				{
					i--;
					break;
				}

				pRgn3->nFill_Cycle = nCur_Fill_Cycle;


				{
					pRgn3->pFillInfo = pFillInfo;

					pRgn3->bBadFillDone = true;

					curFillRgnArr.PushBack( pRgn3 );
				}

				m_fillDeq.PushBack( pRgn3 );
			}


			bool bRootMet = false;

			while( m_fillDeq.GetSize() > 0 )
			{

				while( m_fillDeq.GetSize() > 0 )
				{
					RgnInfo * pRgn1_1 = m_fillDeq.GetFront();
					m_fillDeq.PopFront();

					if( pRgn1_1->bBad_UnfillDone )
						continue;

					EdgeScan::EdgeInfo * pFS_EI_1_1 = pRgn1_1->pShortestEI;

					if( NULL == pFS_EI_1_1 )
						i = i;
					 

					//for( int j=0; j < 8; j++ )
					for( int j=0; j < 8; j += 2 )
					{
						RgnLink & rLink = pRgn1_1->links[ j ];

						if( ! rLink.bExists )
							continue;

						RgnInfo * pRgn2 = rLink.pPeerRgn;


						if( pRgn2->GetActRgn() != pRgn1_1->GetActRgn() )
						{
							//if( ! pRgn2->bFromBadSrc && pRgn1_1->bFstFromBadSrc )
								//pRgn2->bFlag2 = true;

							//if( ! pRgn2->bFromBadSrc && ! pRgn2->bFlag1 )

							if( ! pRgn2->bFromBadSrc && 
								//pFS_EI_1_1->Act_Max_Idx != pRgn1_1->GetActRgn()->nIndex &&
								//pFS_EI_1_1->Act_Min_Idx != pRgn1_1->GetActRgn()->nIndex )

								pFS_EI_1_1->Act_Max_Idx != pRgn2->GetActRgn()->nIndex &&
								pFS_EI_1_1->Act_Min_Idx != pRgn2->GetActRgn()->nIndex )
							{
								m_rgnOf_OpenEnd_Arr.PushBack( pRgn1_1 );
								m_rgnOf_OpenEnd_Arr.PushBack( pRgn2 );

								pRgn2->bFlag2 = true;
							}

							continue;
						}

						if( pRgn2->bBad_UnfillDone )
						//if( false )
						{
							if( 3 == nCurBadGrpID )
								j = j;

							bRootMet = true;

							Hcpl_ASSERT( pRgn2->bBadFillDone );

							if( ! pRgn1_1->bFstFromBadSrc &&
								4 == nCurBadGrpID )
								j = j;
								//pRgn1_1->bFlag1 = true;

							//pRgn2 = pRgn1_1;

							//pRgn2->bBadFillDone = true;
							
							//pRgn2->bBad_UnfillDone = true;

							//Hcpl_ASSERT( m_unfill_Deq.GetSize() == 0 );
							
							m_unfill_Deq.PushBack( pRgn2 );

							//break;

							continue;
						}


						if( pRgn2->bBadFillDone )
							continue;


						//if( pRgn2->bBad_UnfillDone )
							//continue;


						if( m_nTestRgnIdx == pRgn2->nIndex )
							j = j;

						if( pRgn2->bFromBadSrc && ! pRgn2->bFstFromBadSrc )
							continue;

	//uuuuuu

						if( pRgn2->bFstFromBadSrc )
						{
							if( 3 == nCurBadGrpID )
								j = j;

							if( 5 == nCur_Fill_Cycle )
								j = j;

							if( ! pFstShort_EI->HasSameActs( *pRgn2->pShortestEI ) )
							{
								if( 3 == nCurBadGrpID )
								//if( 4 == nCurBadGrpID )
									j = j;

								if( 3 == pRgn2->nFstBadGrpID )
									j = j;

								bRootMet = true;


								pRgn2 = pRgn1_1;

								Hcpl_ASSERT( pRgn2->bBadFillDone );

								//pRgn2->bBadFillDone = false;
								//pRgn2->bBadFillDone = true;
								pRgn2->bBad_UnfillDone = true;

								if( 353667 == pRgn2->nIndex )
									j = j;

								Hcpl_ASSERT( ! pRgn2->IsRoot() );

								m_rgnOfBadSrcArr.PushBack( pRgn2 );

								pRgn2->bFlag1 = true;

								//pRgn2->bMaybe_FstFromBadSrc = true;

								//Hcpl_ASSERT( m_unfill_Deq.GetSize() == 0 );
								m_unfill_Deq.PushBack( pRgn2 );

								//break;
								goto Unfill_Work;
							}
						}

	///////

						if( pRgn2->IsRoot() )
						{
							if( 3 == nCurBadGrpID )
								j = j;

							bRootMet = true;

							
							//pRgn2->bBadFillDone = false;
							pRgn2->bBadFillDone = true;
							pRgn2->bBad_UnfillDone = true;

							//Hcpl_ASSERT( m_unfill_Deq.GetSize() == 0 );
							m_unfill_Deq.PushBack( pRgn2 );

							//break;
							goto Unfill_Work;
						}


						if( pRgn2->bBadFillDone )
							continue;

						if( pRgn2->bFstFromBadSrc &&
							( nCurBadGrpID != pRgn2->nFstBadGrpID ) )
							continue;

						pRgn2->nFill_Cycle = nCur_Fill_Cycle;

						{
							pRgn2->pFillInfo = pFillInfo;

							pRgn2->bBadFillDone = true;

							curFillRgnArr.PushBack( pRgn2 );
						}

						if( NULL == pRgn2->pShortestEI )
							pRgn2->pShortestEI = pFS_EI_1_1;


						m_fillDeq.PushBack( pRgn2 );
					}


				}


Unfill_Work:

				nCur_Unfill_Cycle++;

				if( bRootMet )
				{

					while( m_unfill_Deq.GetSize() > 0 )
					{
						RgnInfo * pRgn1_1 = m_unfill_Deq.GetFront();
						m_unfill_Deq.PopFront();

						if( 137996 == pRgn1_1->GetActRgn()->nIndex &&
							4 == nCurBadGrpID )
						{
							//m_unfill_Deq.ResetSize();
							//break;

							i = i;
						}

//						if( 6825 == nCur_Unfill_Cycle )
						//if( 6826 == nCur_Unfill_Cycle )
						//if( 6827 == nCur_Unfill_Cycle )
//							pRgn1_1->bFlag2 = true;

						if( F32Point::Sub( testPnt, pRgn1_1->pos ).CalcMag() < 0.5 ) 
							i = i;

						Hcpl_ASSERT( pRgn1_1->bBad_UnfillDone );

						//if( pRgn1_1->bBadFillDone )
						//if( ! pRgn1_1->bBad_UnfillDone )

						//if( ! pRgn1_1->bBadFillDone )
						//	continue;

						{
							pRgn1_1->pFillInfo = NULL;
						}


						//for( int j=0; j < 8; j++ )
						for( int j=0; j < 8; j += 2 )
						{
							RgnLink & rLink = pRgn1_1->links[ j ];

							if( ! rLink.bExists )
								continue;

							RgnInfo * pRgn2 = rLink.pPeerRgn;

							if( m_nTestRgnIdx == pRgn2->nIndex )
								j = j;

							//if( ! pRgn2->bBadFillDone )
							if( pRgn2->bBad_UnfillDone )
								continue;

							if( ! pRgn2->bBadFillDone )
								continue;

							// Try 17Aug11
							if( pRgn2->GetActRgn() != pRgn1_1->GetActRgn() )
								continue;

							if( pRgn2->bFstFromBadSrc )
							{
								pRgn2->bMaybe_FstFromBadSrc = true;
								continue;
							}

							//Hcpl_ASSERT( ! pRgn2->bFromBadSrc );
							
							if( pRgn2->bFromBadSrc )
								//continue;
								j = j;

							//pRgn2->bBadFillDone = false;
							pRgn2->bBad_UnfillDone = true;

							// Try
							if( ! pRgn2->IsRoot() )
							{
								if( ! pRgn2->bFlag1 )
								{
									//pRgn2->bFromBadSrc = true;
									m_rgnOfBadSrcArr.PushBack( pRgn2 );
								}
							}
							else
							{
								j = j;
							}

							if( 353667 == pRgn2->nIndex )
								j = j;

							pRgn2->bFlag1 = true;

							pRgn2->bFromBadSrc = false;

							m_unfill_Deq.PushBack( pRgn2 );
						}

					}
					//	End unfill

				}

				//m_fillDeq.ResetSize();
			}





			//if( bRootMet )
			{


				for( int j=0; j < curFillRgnArr.GetSize(); j++ )
				{
					RgnInfo * pRgn3 = curFillRgnArr[ j ];

					//if( ! pRgn3->bBadFillDone )
					if( pRgn3->bBad_UnfillDone )
						continue;

					//pRgn3->bFromBadSrc = true;

					//pRgn3->bBadFillDone = false;

					if( ! pRgn3->bFstFromBadSrc )
					{
						//pRgn3->bFstFromBadSrc = true;
						//pRgn3->bFlag1 = true;

						//pRgn3->bBadFillDone = false;
						//pRgn3->bBad_UnfillDone = true;

						// Try
						if( ! pRgn3->IsRoot() )
						{
							pRgn3->bFromBadSrc = true;
							m_rgnOfBadSrcArr.PushBack( pRgn3 );
						}
						else
						{
							j = j;
						}

						continue;
					}
					else
					{

						if( ! pRgn3->bMaybe_FstFromBadSrc )
						{
							pRgn3->bFstFromBadSrc = false;
							continue;
						}


					}

				//--//

				}

				//continue;
			}
			//else
			//{
			//	for( int j=0; j < curFillRgnArr.GetSize(); j++ )
			//	{
			//		RgnInfo * pRgn3 = curFillRgnArr[ j ];

			//		if( F32Point::Sub( testPnt, pRgn3->pos ).CalcMag() < 0.5 ) 
			//			j = j;

			//		pRgn3->bBadFillDone = false;

			//		pRgn3->bFromBadSrc = true;
			//		pRgn3->bFstFromBadSrc = false;

			//		m_rgnOfBadSrcArr.PushBack( pRgn3 );
			//	}
			//}

		}	//	End Fillings


		for( int j=0; j < m_rgnOf_OpenEnd_Arr.GetSize(); j += 2 )
		{
			RgnInfo * pRgn3_0 = m_rgnOf_OpenEnd_Arr[ j ];
			RgnInfo * pRgn3 = m_rgnOf_OpenEnd_Arr[ j + 1 ];

			if( ! pRgn3->bFlag2 )
				continue;

			

			if( pRgn3->bFlag1 ||
				pRgn3_0->bFlag1 ||
				pRgn3->bFromBadSrc )
			{
				pRgn3->bFlag2 = false;

				m_rgnOf_OpenEnd_Arr[ j ] = m_rgnOf_OpenEnd_Arr.GetAtBack( 1 );
				m_rgnOf_OpenEnd_Arr[ j + 1 ] = m_rgnOf_OpenEnd_Arr.GetAtBack( 0 );

				if( 0 == m_rgnOf_OpenEnd_Arr.GetSize() )
					j = j;

				m_rgnOf_OpenEnd_Arr.IncSize( - 2 );
				j -= 2;
			}
		}


		for( int i=0; i < m_rgnOf_FstBadSrcArr_2.GetSize(); i++ )
		{
			//RgnInfo * pRgn3 = curFillRgnArr[ j ];


			//if( ! pRgn3->bFstFromBadSrc )
				//continue;



			curFillRgnArr.ResetSize();

			const int nCurBadGrpID = m_rgnOf_FstBadSrcArr_2[ i ]->nFstBadGrpID;

			for( ; i < m_rgnOf_FstBadSrcArr_2.GetSize(); i++ )
			{
				RgnInfo * pRgn3 = m_rgnOf_FstBadSrcArr_2[ i ];			

				if( nCurBadGrpID != pRgn3->nFstBadGrpID )
				{
					i--;
					break;
				}

				if( ! pRgn3->bFstFromBadSrc )
					continue;

				curFillRgnArr.PushBack( pRgn3 );
			}


			if( curFillRgnArr.GetSize() == 0 )
				continue;




			Hcpl_ASSERT( curFillRgnArr.GetSize() > 0 );

			EdgeScan::EdgeInfo * pShortest_EI = curFillRgnArr[ 0 ]->pShortestEI;

			const int nFstBadGrpID_Org = curFillRgnArr[ 0 ]->nFstBadGrpID;

			if( 2 == nFstBadGrpID_Org )
				i = i;

			//m_shortest_FromBad_EI_Arr.ResetSize();
			//
			//m_shortest_FromBad_EI_Arr.PushBack( curFillRgnArr[ 0 ]->pShortestEI );

			//EdgeInfoAccessor eiAcc_New( this );
			//EdgeInfoAccessor eiAcc_M( this );

			RgnInfo * pRgn_Shortest_EI = curFillRgnArr[ 0 ];



			for( int k=0; k < curFillRgnArr.GetSize(); k++ )
			{
				RgnInfo * pRgn4 = curFillRgnArr[ k ];






				Hcpl_ASSERT( pRgn4->bFromBadSrc );


				Hcpl_ASSERT( pRgn4->bMaybe_FstFromBadSrc );



				Hcpl_ASSERT( nFstBadGrpID_Org == pRgn4->nFstBadGrpID );

				if( F32Point::Sub( testPnt, pRgn4->pos ).CalcMag() < 5 ) 
					i = i;


				{
					//EdgeScan::EdgeInfo * pNew_EI = pRgn4->pShortestEI;
					//eiAcc_New.SetEdgeInfo( pNew_EI );
					//
					//EdgeScan::EdgeInfo * pCur_EI = NULL;

					//int m = 0;
					//for( ; m < m_shortest_FromBad_EI_Arr.GetSize(); m++ )
					//{
					//	pCur_EI = m_shortest_FromBad_EI_Arr[ m ];
					//	
					//	eiAcc_M.SetEdgeInfo( pCur_EI );

					//	if( eiAcc_M.GetR1_Rgn()->GetActRgn() ==
					//			eiAcc_New.GetR1_Rgn()->GetActRgn() &&

					//		eiAcc_M.GetR2_Rgn()->GetActRgn() ==
					//			eiAcc_New.GetR2_Rgn()->GetActRgn() )
					//	{
					//		break;
					//	}

					//}

					//if( m < m_shortest_FromBad_EI_Arr.GetSize() )
					//{

					//	if( pNew_EI->RootSpaceDist < pCur_EI->RootSpaceDist )
					//	{
					//		m_shortest_FromBad_EI_Arr[ m ] = pNew_EI;
					//	}

					//}
					//else
					//{
					//	m_shortest_FromBad_EI_Arr.PushBack( pNew_EI );
					//}



					//Hcpl_ASSERT( pRgn4->pShortestEI->HasBadSrc_Trace );

					if( pRgn4->pShortestEI->HasBadSrc_Trace &&
					//if( 

						pRgn4->pShortestEI->RootSpaceDist <
						pShortest_EI->RootSpaceDist )
					{
						pShortest_EI = pRgn4->pShortestEI;

						pRgn_Shortest_EI = pRgn4;
					}

				}

				pRgn4->bFstFromBadSrc = false;
			}

			{
				//int m = 0;
				//for( ; m < m_shortest_FromBad_EI_Arr.GetSize(); m++ )
				//{
				//	m_fstFromBad_EI_Arr.PushBack( 
				//		m_shortest_FromBad_EI_Arr[ m ] );
				//}

				if( 8632 == pShortest_EI->Index )
				//if( 7008 == pShortest_EI->Index )
					i = i;

				if( 2 == nFstBadGrpID_Org )
					i = i;

				m_fstFromBad_EI_Arr.PushBack( pShortest_EI );
			}



		}



///*
		//m_rgnOf_FstBadSrcArr.ResetSize();

		int nDoneCnt = 0;

		curFillRgnArr.ResetSize();

		for( int i=0; i < m_fstFromBad_EI_Arr.GetSize(); i++ )
		{
			//break;

			EdgeScan::EdgeInfo & rEI = * m_fstFromBad_EI_Arr[ i ];

			//Hcpl_ASSERT( rEI.HasBadSrc_Trace );

			EdgeInfoAccessor eiAcc( this );

			eiAcc.SetEdgeInfo( & rEI );

			bool bDone = false;

			bool bFstMet = false;

			RgnInfo * pRgnOld = NULL;

			{
				RgnInfo * pRgn = eiAcc.GetRgnOf_IOL( rEI.Med1_IOL );
				pRgn->bFlag2 = true;
			}

			{
				RgnInfo * pRgn = eiAcc.GetRgnOf_IOL( rEI.Med2_IOL );
				//pRgn->bFlag2 = true;
			}

			if( 13369 == rEI.Index )
				i = i;

			for( int j = rEI.InrR1_IOL + 1; j < rEI.InrR2_IOL; j++ )
			//for( int j = rEI.Med1_IOL + 1; j < rEI.Med2_IOL; j++ )
			{
				RgnInfo * pRgn = eiAcc.GetRgnOf_IOL( j );

				if( NULL == pRgnOld )
					pRgnOld = pRgn;

				//if( F32Point::Sub( testPnt, pRgn->pos ).CalcMag() < 5 ) 
				if( F32Point::Sub( testPnt, pRgn->pos ).CalcMag() < 3 ) 
					j = j;


				//Hcpl_ASSERT( pRgn->bFromBadSrc );


				//if( ! ( pRgn->bFromBadSrc || pRgn->bFlag1 ) )
					//pRgn->bFlag2 = true;


				//( &m_rgnInfoVect[ 296509 ] )->bFlag2 = true;
				//( &m_rgnInfoVect[ 301824 ] )->bFlag2 = true;

				//if( pRgn->bFromBadSrc )
				if( pRgn->bFromBadSrc || pRgn->bFlag1 || 
					pRgnOld->bFromBadSrc || pRgnOld->bFlag1 )
				{
					//if( ! pRgn->bFromBadSrc )
					//{
					//	pRgn->bFromBadSrc = true;						
					//
					//	m_rgnOfBadSrcArr.PushBack( pRgn );
					//}

					if( 109981 == pRgn->nIndex )
						j = j;

					//if( ! pRgn->bFstFromBadSrc )
					{
						pRgn->bFstFromBadSrc = true;
						
						pRgnOld->bFstFromBadSrc = true;						

						//if( ! pRgn->bFromBadSrc )
						//{
						//	if( ! pRgn->IsRoot() )
						//	{
						//		pRgn->bFromBadSrc = true;
						//		m_rgnOfBadSrcArr.PushBack( pRgn );
						//	}
						//}
						//
						//if( ! pRgnOld->bFromBadSrc )
						//{
						//	if( ! pRgnOld->IsRoot() )
						//	{
						//		pRgnOld->bFromBadSrc = true;
						//		m_rgnOfBadSrcArr.PushBack( pRgnOld );
						//	}
						//}

						//m_rgnOf_FstBadSrcArr.PushBack( pRgn );
						curFillRgnArr.PushBack( pRgn );
					}

					if( NULL != pRgnOld )
					{
						F32Point difPnt = F32Point::Sub( pRgn->pos, pRgnOld->pos );

						if( fabs( difPnt.x ) >= 2 ||
							fabs( difPnt.y ) >= 2 )
						{
							j = j;
						}


						//if( 0 != difPnt.x && 0 != difPnt.y )
						{
							RgnInfo * pRgnMed = GetPointRgn(
								pRgnOld->pos.x, pRgn->pos.y ); 
							

							pRgnMed->bFstFromBadSrc = true;
						}

						{
							//RgnInfo * pRgnMed2 = GetPointRgn(
							//	pRgn->pos.x, pRgnOld->pos.y ); 
							//

							//pRgnMed2->bFstFromBadSrc = true;
						}

					}

					//pRgnOld = pRgn;

					bDone = true;

					//bFstMet = true;
				}
				pRgnOld = pRgn;

			}

			if( bDone )
				nDoneCnt++;

		}

		nDoneCnt = nDoneCnt;



		for( int j=0; j < m_rgnOf_OpenEnd_Arr.GetSize(); j += 2 )
		{
			RgnInfo * pRgn3_0 = m_rgnOf_OpenEnd_Arr[ j ];
			RgnInfo * pRgn3 = m_rgnOf_OpenEnd_Arr[ j + 1 ];

			//pRgn3_0->bFstFromBadSrc = true;
			
			//pRgn3->bFstFromBadSrc = true;
			pRgn3->bFlag2 = false;
		}


//*/

/*
		RgnInfo * pRgn_169560 = & m_rgnInfoVect[ 169560 ];

		//RgnInfo * pRgn_168855 = & m_rgnInfoVect[ 168855 ];

		



///*
		
		//for( int i=0; i < m_rgnOf_FstBadSrcArr.GetSize(); i++ )			
		for( int i=0; i < curFillRgnArr.GetSize(); i++ )			
		{
			//RgnInfo * pRgn = m_rgnOf_FstBadSrcArr[ i ];			
			RgnInfo * pRgn = curFillRgnArr[ i ];
			

			if( ! pRgn->bFstFromBadSrc )
				continue;

			//{
			//	pRgn->bFstFromBadSrc = true;
			//	continue;
			//}


			//if( F32Point::Sub( testPnt, pRgn->pos ).CalcMag() < 3 ) 
			if( F32Point::Sub( testPnt, pRgn->pos ).CalcMag() < 6 ) 
				i = i;


			for( int j=0; j < 8; j++ )
			//for( int j=0; j < 8; j += 2 )
			{
				RgnLink & rLink = pRgn->links[ j ];

				if( ! rLink.bExists )
					continue;

				RgnInfo * pRgn2 = rLink.pPeerRgn;

				//if( pRgn2->GetActRgn() != pLastActRgn )
					//continue;

				if( pRgn2->bFstFromBadSrc )
					continue;

				if( ! pRgn2->bFromBadSrc && ! pRgn2->bFlag1 )
					continue;

				//if( pRgn2->GetActRgn() != pRgn->GetActRgn() )
					//continue;
	
				pRgn2->bFstFromBadSrc = true;
				//pRgn2->bFlag2 = true;
			}



		}

//*/



//*/
//*/









	}
	

	F32ImageRef RegionSegmentor41::Gen_FalseMergeImage_2()
	{
		F32ImageRef ret = this->m_src->Clone();

		EdgeInfoAccessor eiAcc( this );

		F32Point testPnt( 278, 338 );



		FixedVector< U8ColorVal > fstColorArr( 300 );

		//for( int i=0; i < 10; i++ )
		for( int i = 111; i < 10; i++ )
		{
			//fstColorArr.PushBack( u8ColorVal( 0, 255, 0 ) );

			fstColorArr.PushBack( u8ColorVal( 0, 255, 0 ) );
			fstColorArr.PushBack( u8ColorVal( 0, 0, 255 ) );
			fstColorArr.PushBack( u8ColorVal( 255, 0, 0 ) );
			fstColorArr.PushBack( u8ColorVal( 0, 255, 255 ) );

			fstColorArr.PushBack( u8ColorVal( 255, 0, 255 ) );

			fstColorArr.PushBack( u8ColorVal( 255, 255, 0 ) );

			//fstColorArr.PushBack( u8ColorVal( 255, 255, 255 ) );
		}


		CvSize srcSiz = ret->GetSize();

		F32Point minPnt( srcSiz.width, srcSiz.height );
		F32Point maxPnt( 0, 0 );


		for( int i=0; i < m_rgnInfoVect.GetSize(); i++ )
		{
			RgnInfo * pRgn = & m_rgnInfoVect[ i ];


			if( F32Point::Sub( testPnt, pRgn->pos ).CalcMag() < 0.5 ) 
				i = i;


			F32ColorVal * pix = (F32ColorVal *) ret->GetPixAt( pRgn->pos.x, pRgn->pos.y );

			
			if( pRgn->bFlag2 )
			{
				//pix->AssignVal( 0, 70, 70 );
				DrawCircle( ret, pRgn->pos, u8ColorVal( 255, 0, 255 ), 2 );
			}
			else if( pRgn->bFlag1 )
			{
				//pix->AssignVal( 255, 0, 0 );
				//pix->AssignVal( 155, 0, 0 );
				pix->AssignVal( 70, 0, 0 );
				//pix->AssignVal( 0, 0, 0 );
				//pix->AssignVal( 0, 255, 0 );
			}
			//else if( pRgn->bFstFromBadSrc )
			
			if( pRgn->bFstFromBadSrc )
			{
				if( pRgn->pos.x < minPnt.x )
					minPnt.x = pRgn->pos.x;

				if( pRgn->pos.x > maxPnt.x )
					maxPnt.x = pRgn->pos.x;

				if( pRgn->pos.y < minPnt.y )
					minPnt.y = pRgn->pos.y;

				if( pRgn->pos.y > maxPnt.y )
					maxPnt.y = pRgn->pos.y;

				pix->AssignVal( 0, 255, 0 );
				//DrawCircle( ret, pRgn->pos, u8ColorVal( 255, 255, 255 ), 1 );

			//if( pRgn->bFstFromBadSrc && pRgn->IsRoot() )
				//DrawCircle( ret, pRgn->pos, u8ColorVal( 0, 255, 255 ), 4 );
			//else
				//DrawCircle( ret, pRgn->pos, u8ColorVal( 0, 255, 0 ), 2 );

				//Hcpl_ASSERT( -1 != pRgn->nFstBadGrpID );

				//if( -1 == pRgn->nFstBadGrpID || pRgn->nFstBadGrpID >= fstColorArr.GetSize() )
				//	//pix->AssignVal( 255, 255, 255 );
				//	pix->AssignVal( 0, 255, 0 );
				//else
				//	//pix->AssignVal( fstColorArr[ pRgn->nFstBadGrpID ].ToF32() );
				//	pix->CopyU8( fstColorArr[ pRgn->nFstBadGrpID ] );

				//DrawCircle( ret, pRgn->pos, fstColorArr[ pRgn->nFstBadGrpID ], 2 );

				//DrawCircle( ret, pRgn->pos, u8ColorVal( 0, 255, 0 ), 1 );
				
			}
			else if( pRgn->bFromBadSrc )
			{
				if( pRgn->bFromBadSrc_Org )
				{
					//pix->AssignVal( 180, 180, 180 );
					//pix->AssignVal( 130, 130, 130 );
					//pix->AssignVal( 70, 70, 70 );
					pix->AssignVal( 40, 40, 40 );
				}
				else
				{
					//pix->AssignVal( 255, 30, 255 );
					//pix->AssignVal( 155, 30, 155 );
					pix->AssignVal( 70, 20, 70 );
				}
			}
			//else if( pRgn->bFlag1 )			
			//{
			//	//pix->AssignVal( 255, 0, 0 );
			//	//pix->AssignVal( 155, 0, 0 );
			//	pix->AssignVal( 70, 0, 0 );
			//}
			else
				pix->AssignVal( 0, 0, 0 );

			if( pRgn->bBad_UnfillDone && pRgn->IsRoot() )
				DrawCircle( ret, pRgn->pos, u8ColorVal( 0, 255, 255 ), 2 );

			{
				
				//RgnInfo * pRgn2 = & m_rgnInfoVect[ 88605 ];
				RgnInfo * pRgn2 = & m_rgnInfoVect[ 273544 ];
				//DrawCircle( ret, pRgn2->pos, u8ColorVal( 255, 255, 0 ), 3 );
			}

			{
				//DrawCircle( ret, F32Point( 301, 348 ), u8ColorVal( 255, 0, 255 ), 3 );
			}

		}

		minPnt.x -= 20;
		minPnt.y -= 20;

		maxPnt.x += 20;
		maxPnt.y += 20;

		//ret = GenClipedImg( ret, minPnt, maxPnt );

		//ret = GenUpSampledImage( ret, 2 );

		{
			//F32ImageRef ret2 = GenUpSampledImage( ret, 4 );

				//SaveImage( ret2, "E:\\ret2.jpg" );
			//SaveImage( ret2, "ret2.jpg" );
		}

		return ret;
	}

	F32ImageRef RegionSegmentor41::Gen_FalseMergeImage()
	{
		F32ImageRef ret = this->m_src->Clone();

		EdgeInfoAccessor eiAcc( this );

		for( int i=0; i < m_rgnInfoVect.GetSize(); i++ )
		{
			RgnInfo * pRgn = &m_rgnInfoVect[ i ];

			//if( 295 == pRgn->pos.x && 176 == pRgn->pos.y )
			//if( 227 == pRgn->pos.x && 323 == pRgn->pos.y )
			if( 260 == pRgn->pos.x && 272 == pRgn->pos.y )
				i = i;

			F32ColorVal * pix = (F32ColorVal *) ret->GetPixAt( pRgn->pos.x, pRgn->pos.y );

			pix->AssignVal( 0, 0, 0 );

			EdgeScan::EdgeInfo * pMinEI = pRgn->m_pMinDist_EI;

			if( NULL == pMinEI )
				continue;

			eiAcc.SetEdgeInfo( pMinEI );

			//RgnInfo * pRgnR1 = eiAcc.GetR1_Rgn();
			RgnInfo * pRgnR1 = eiAcc.GetMed1_Rgn();

			//if( pRgnR1->nMergeOrder < pRgn->nMergeOrder * 0.5 )
			//if( pRgnR1->nMergeOrder * 0.5 > pRgn->nMergeOrder )
			//if( pRgnR1->nMergeOrder * 0.8 > pRgn->nMergeOrder )
			if( pRgnR1->nMergeOrder * 0.9 > pRgn->nMergeOrder )
			//if( pRgnR1->nMergeOrder > pRgn->nMergeOrder )
				pix->AssignVal( 0, 255, 0 );



			//RgnInfo * pRgnR2 = eiAcc.GetR2_Rgn();
			RgnInfo * pRgnR2 = eiAcc.GetMed2_Rgn();

			//if( pRgnR2->nMergeOrder < pRgn->nMergeOrder * 0.5 )
			//if( pRgnR2->nMergeOrder * 0.5 > pRgn->nMergeOrder )
			//if( pRgnR2->nMergeOrder * 0.8 > pRgn->nMergeOrder )
			if( pRgnR2->nMergeOrder * 0.9 > pRgn->nMergeOrder )
			//if( pRgnR2->nMergeOrder > pRgn->nMergeOrder )
				pix->AssignVal( 0, 255, 0 );
		}


		return ret;
	}


	F32ImageRef RegionSegmentor41::GenMergeOrderImage()
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


	void RegionSegmentor41::PrepareSrcGradImg()
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