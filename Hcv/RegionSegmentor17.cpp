#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\RegionSegmentor17.h>

namespace Hcv
{
	using namespace Hcpl;
	using namespace Hcpl::Math;


	RegionSegmentor17::RegionSegmentor17(F32ImageRef a_src, S16ImageRef a_rootImg,
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

		InitRgnInfoVect();
		InitLinks();		

		//RgnInfo * pRgn81 = GetPointRgn(81, 271);

		m_rgnConflictVect.resize( 
			10000000);
		
		m_nofConflicts = 0;
	}

	void RegionSegmentor17::InitRgnInfoVect(void)
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

					if(nRootImgVal > 1)
					{
						pRgnInfo->pRootRgn = pRgnInfo;
						pRgnInfo->SetActRgn( pRgnInfo );
						pRgnInfo->bIsRoot = true;

						pRgnInfo->pixColors = 
							(F32ColorVal *)m_rootValImg->GetPixAt(x, y);
					}
					else
					{
						pRgnInfo->pRootRgn = NULL;
						pRgnInfo->bIsRoot = false;

						pRgnInfo->pixColors = 
							(F32ColorVal *)m_src->GetPixAt(x, y);
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

				pRgnInfo->bIsInConflict = false;

				pRgnInfo->nMaxDistMet = 0;

				pRgnInfo->meanColors.val0 = 0;
				pRgnInfo->meanColors.val1 = 0;
				pRgnInfo->meanColors.val2 = 0;

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

	inline void RegionSegmentor17::CreateLink( RgnInfo * a_pRgn, 
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

		if(dist >1)
			dist = dist;

		//rLink.
		
/*		if( pRgn2->bIsEdge )
		{
			dist = CalcRgnDif( a_pRgn, pRgn2 );		
			//dist = 0;
			//dist = GetMaxDif();
			//dist = 5000;
		}
		else*/
		{		
			//dist = CalcRgnDif( a_pRgn, pRgn2 );		
			//dist = -1;		
		}

		// For a Diagonal Link

		// hthm
		//dist = -1;

		rLink.DistMag = rLinkR2.DistMag = dist;

		//rLink.DistMag = rLinkR2.DistMag = -1;

		if( a_pRgn->bIsRoot )
		{			
			PrepareLinkAction( a_pRgn, pRgn2, rLink, rLinkR2, 0);
		}
		else if( pRgn2->bIsRoot )
		{
			PrepareLinkAction( pRgn2, a_pRgn, rLinkR2, rLink, 0);
		}

	}




	void RegionSegmentor17::InitLinks(void)
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

	S16ImageRef RegionSegmentor17::GenDifImg(bool a_bRC, bool a_bRB,
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



	
	HistoGramRef RegionSegmentor17::GenDifHisto(bool a_bRC, bool a_bRB,
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

	
	
	
	
	void RegionSegmentor17::Segment()
	{
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
				bool bFromTmp = false; 

				pLA = m_linkActionMergeQues.PopPtrMin();

				if( NULL == pLA )
				{
					continue;
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

						float dist = CalcRgnDif( pRootRgn1, pRootRgn2 );

						if( IsConflictDist(	dist ) )
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


							interRootQues.PushPtr(dist, pLA );
							//interRootQues.PushPtr(0, pLA );

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
/*						if( pLA->pRgn2->bIsN2E && 
							(GetMaxDif() - 1) != m_nDifIndex )
							m_nDifIndex = m_nDifIndex;

						if( pLA->pRgn2->bIsE2E && 
							(GetMaxDif()) != m_nDifIndex )
							m_nDifIndex = m_nDifIndex;*/

						pLA->pRgn2->pRootRgn = pRootRgn1;

						float dist = m_linkActionMergeQues.GetLastMinIndex();
						if( dist > pLA->pRgn1->nMaxDistMet )
							pLA->pRgn2->nMaxDistMet = dist;
						
						PrepareRgnLinkActions( pLA->pRgn2, 
							m_linkActionMergeQues.GetLastMinIndex() );
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

		for(int i=0; i < m_rgnInfoVect.size(); i++)
			UpdateActRgn( &m_rgnInfoVect[i] );

		// 81, 271
		//RgnInfo * pRgn81 = GetPointRgn(81, 271);
	}

	float RegionSegmentor17::CalcRgnDif(RgnInfo * a_pRgn1, RgnInfo * a_pRgn2)
	{
		float dist = m_difScale * F32ColorVal::Sub(
			*a_pRgn1->pixColors, *a_pRgn2->pixColors).CalcMag();

		return dist;
	}

	// With Space
	float RegionSegmentor17::CalcRgnDifWS(RgnInfo * a_pRgn1, RgnInfo * a_pRgn2)
	{
		double spaceFact = 1;

		double dx = a_pRgn1->x - a_pRgn2->x;
		double dy = a_pRgn1->y - a_pRgn2->y;

		dx *= spaceFact;
		dy *= spaceFact;

		double distSpace = m_difScale * sqrt( (double) (         
			Sqr( dx ) + Sqr( dy ) ) );

		return (float)distSpace;

		

/*		if( distSpace > 15 )
			return GetMaxDif() - 1;*/

		float colorDistSqr = F32ColorVal::Sub(
			*a_pRgn1->pixColors, *a_pRgn2->pixColors).CalcMagSqr();

		float dist =  m_difScale * sqrt( (double) (
		//float dist = (int) ( (
			colorDistSqr
			+ Sqr( dx ) + Sqr( dy ) ) );
			 //);

		return dist;
	}


	void RegionSegmentor17::UpdateActRgn(RgnInfo * a_pRgn)
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

	S16ImageRef RegionSegmentor17::GenSegmentedImage(void)
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
				retCh0->SetAt( pRgn->x, pRgn->y, (short)pActRgn->meanColors.val0 );
				retCh1->SetAt( pRgn->x, pRgn->y, (short)pActRgn->meanColors.val1 );
				retCh2->SetAt( pRgn->x, pRgn->y, (short)pActRgn->meanColors.val2 );

/*				retCh0->SetAt( pRgn->x, pRgn->y, (short)pRgn->pixColors->val0 );
				retCh1->SetAt( pRgn->x, pRgn->y, (short)pRgn->pixColors->val1 );
				retCh2->SetAt( pRgn->x, pRgn->y, (short)pRgn->pixColors->val2 );*/
			}
		}

		return ret;
	}

/*
	void RegionSegmentor17::PrepareConflicts()
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

	void RegionSegmentor17::PrepareLinkAction( RgnInfo * a_pRgn, RgnInfo * a_pRgn2,
			RgnLink & a_rLink, RgnLink & a_rLink2, float a_distBef)
	{
		LinkAction * pLA = &m_linkActionVect[ m_nofLinkActions++ ];

		pLA->pRgn1 = a_pRgn;
		pLA->pRgn2 = a_pRgn2;
		pLA->pPrev = pLA;


		//float distNew = a_rLink.DistMag;
		//float distNew = a_rLink.DistMag + a_distBef;
		//float distNew = CalcRgnDif( a_pRgn->pRootRgn, a_pRgn2 );
		//float distNew = CalcRgnDifWS( a_pRgn->pRootRgn, a_pRgn2 );
		//distNew += a_distBef + 1;
		//distNew += a_distBef;

		float distNew;

/*		if( a_pRgn2->bIsEdge )
		{
			if( a_pRgn->bIsEdge )
			{
				distNew = GetMaxDif();

				if( false == a_pRgn2->bIsN2E )
					a_pRgn2->bIsE2E = true;
			}
			else
			{
				//distNew = CalcRgnDif( a_pRgn->pRootRgn, a_pRgn2 );
				//distNew = 0;
				distNew = GetMaxDif() - 1;
				//distNew = 5000;

				a_pRgn2->bIsN2E = true;
			}
		}
		else*/
		{		
			distNew = CalcRgnDif( a_pRgn->pRootRgn, a_pRgn2 );
			//distNew = a_rLink.DistMag;
			//distNew = CalcRgnDifWS( a_pRgn->pRootRgn, a_pRgn2 );

			//if( distNew >= ( GetMaxDif() - 1 ) )
			//	distNew = distNew;
		}






		/*float distNew = (int) ( 10 *
			sqrt( (double)
				( 
					Sqr( a_pRgn->pRootRgn->x - a_pRgn2->x ) +
					Sqr( a_pRgn->pRootRgn->y - a_pRgn2->y ) )
					)
					);
					*/


		//a_rLink.DistMag = a_rLink2.DistMag = distNew;
		

		//m_difQues.PushPtr( a_rLink.DistMag, pLA);
		m_linkActionMergeQues.PushPtr( distNew, pLA);

		

		a_rLink.bProcessed = a_rLink2.bProcessed = true;
	}

	bool RegionSegmentor17::HaveConflict( RgnInfo * a_pMinSizRgn, RgnInfo * a_pMaxSizRgn)
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

	void RegionSegmentor17::RemoveDuplicateConflicts( RgnInfo * a_pRgn )
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


	void RegionSegmentor17::PrepareRgnLinkActions( RgnInfo * a_pRgn, float a_distBef )
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

	S16ImageRef RegionSegmentor17::GenConflictImg(bool a_bRC, bool a_bRB,
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
						retCh0->SetAt( pRgn->x, pRgn->y, 255);
					}
				}
			}
		}

		return ret;
	}


	void RegionSegmentor17::CreateConflict( RgnInfo * a_pRgn1, RgnInfo * a_pRgn2 )
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