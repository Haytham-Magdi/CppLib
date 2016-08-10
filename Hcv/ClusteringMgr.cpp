#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\ClusteringMgr.h>

namespace Hcv
{
	using namespace Hcpl;
	using namespace Hcpl::Math;


	void ClusteringMgr::Process(
		int * a_pats,
		IndexCalc2D * a_pPatIndex
		)
	{
		m_pats = a_pats;

		m_pPatIndex = a_pPatIndex;

		IndexCalc2D patIndex(m_pPatIndex->GetSizeX(), 
			m_pPatIndex->GetSizeY());

		const int nofFeatures = patIndex.GetSizeX();

		const int nDataSiz = patIndex.GetSizeY();

		m_resPats.resize( patIndex.GetTotalSize() );
		int * resPats = &m_resPats[0];

		AssignNbrs();

		const int nDistStep = m_nDistStep;		
		const int nNbrDist = m_nNbrDist;

		const int nofNbrs = m_nofNbrs;

		const int nofIndirs = m_nofIndirs;
		const int nofGuids = m_nofGuids;


		IndexCalc2D nbrIndex( nofNbrs, nDataSiz );



		//	Sorting Nbrs
		for(int i=0; i<nDataSiz; i++)
		{
			int * locNbrsI = 
				&m_nbrs[ nbrIndex.Calc(0, i) ];
			int * locNbrDistsI = 
				&m_nbrDists[ nbrIndex.Calc(0, i) ];



			for(int j=0; j < nofNbrs; j++)
			{
				//if(m_nHugeDist == locNbrDistsI[ j ])
				if(m_nHugeDist == locNbrDistsI[ j ])
					i = i;

				int nMinIndex = j;

				for(int k= j + 1; k < nofNbrs; k++)
				{
					if(locNbrDistsI[ k ] < locNbrDistsI[ nMinIndex ])
						nMinIndex = k;
				}
				
				int nTmp = locNbrDistsI[j];
				locNbrDistsI[j] = locNbrDistsI[ nMinIndex ];
				locNbrDistsI[ nMinIndex ] = nTmp;

				nTmp = locNbrsI[j];
				locNbrsI[j] = locNbrsI[ nMinIndex ];
				locNbrsI[ nMinIndex ] = nTmp;
			}
		}


///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

		std::vector<int> indirs( nDataSiz * nofIndirs );
		IndexCalc2D indirIndex( nofIndirs, nDataSiz );

		std::vector<int> indirCnts( nDataSiz );
		
		std::vector<int> nextDists( nofIndirs );
		std::vector<int> nextOrders( nofIndirs );

		for(int i=0; i<nDataSiz; i++)
		{
			int & rIndirCntI = indirCnts[i] = 0;

			//memset(nextDists, 0, sizeof(int) * nofIndirs);
			//memset(nextOrders, 0, sizeof(int) * nofIndirs);

			int * locIndirs = 
				&indirs[ indirIndex.Calc(0, i) ];

			int nSelf;
			int nNbrOrder;

////////////////////////////////

			nSelf = i;
			nNbrOrder = 0;

			locIndirs[rIndirCntI] = nSelf;
			nextOrders[rIndirCntI] = nNbrOrder;
			nextDists[rIndirCntI] = m_nbrDists[ nbrIndex.Calc( 
				nNbrOrder, nSelf) ];

			rIndirCntI++;

			//indirCnts[i] = rIndirCntI;

////////////////////////////////

			int nChosenIndex = 0;

			int nCycleIndex = 0;
			int nCycleSiz = nofGuids;
			int nCycleID = 0;
			int nofContinues = 0;
			int nBgn = 0;
			int nMinCycleDist = m_nHugeDist;

			double nMaxAllowedDist = 0;

			bool bPrepareDist = true;

			while(rIndirCntI < nofIndirs)
			{
				//if(rIndirCntI < nofGuids)
				if(true == bPrepareDist)
				{

					if(rIndirCntI >= nofGuids)
					{
						nMaxAllowedDist *= 2;
						//nMaxAllowedDist *= 1;
						//nMaxAllowedDist *= 1.5;
						//nMaxAllowedDist = m_nHugeDist;

						bPrepareDist = false;
						continue;
					}


					int nMinDistIndex = 0;
					for(int j=0; j < rIndirCntI; j++)
					{
						if( nextDists[j] < nextDists[ nMinDistIndex ])
							nMinDistIndex = j;
					}

					if(nextDists[ nMinDistIndex ] > nMaxAllowedDist)
						nMaxAllowedDist = nextDists[ nMinDistIndex ];
					
					if(nMaxAllowedDist >= m_nHugeDist)
					{
						goto DoneI;
					}

					nChosenIndex = nMinDistIndex;
				}
				else
				{
					//if(1231 == i)
					if(268 == i)
						i = i;

					nChosenIndex = nCycleIndex;

					if(nChosenIndex >= nCycleSiz)
					//if(nCycleIndex >= nCycleSiz)
					//if(nCycleIndex >= rIndirCntI)
					{
						if(nofContinues >= nCycleSiz - nBgn)
						{
//							nMaxAllowedDist *= 1.5;
							//nMaxAllowedDist += nDistStep;
							//nMaxAllowedDist = nNbrDist;
							nMaxAllowedDist = m_nHugeDist;

							if( nMaxAllowedDist > nNbrDist )
							//if( nMaxAllowedDist > nNbrDist / 5 )
								break;
							//i = i;
						}

						nCycleIndex = nBgn;
						nChosenIndex = nBgn;
						nofContinues = 0;
						nCycleSiz = rIndirCntI;
						nCycleID++;
						nMinCycleDist = m_nHugeDist;
					}
					nCycleIndex++;

					if(nextDists[ nChosenIndex ] < nMinCycleDist)
					{
						nMinCycleDist = nextDists[ nChosenIndex ];
					}

					if(nextDists[ nChosenIndex ] > nMaxAllowedDist)
					{
						nofContinues++;
						continue;					
					}
				}

////////////////////

				if(268 == i)
					i = i;

				nSelf = locIndirs[ nChosenIndex ];

				const int nIndir = m_nbrs[ nbrIndex.Calc( 
					nextOrders[ nChosenIndex ], nSelf ) ];

				nNbrOrder = ++nextOrders[ nChosenIndex ];
				if(nNbrOrder >= nofNbrs)
				{
					nextDists[ nChosenIndex ] = m_nHugeDist;

					// Swapping with bgn

					int nTmp;

					nTmp = locIndirs[ nBgn ];
					locIndirs[ nBgn ] = locIndirs[ nChosenIndex ];
					locIndirs[ nChosenIndex ] = nTmp;

					nTmp = nextDists[ nBgn ];
					nextDists[ nBgn ] = nextDists[ nChosenIndex ];
					nextDists[ nChosenIndex ] = nTmp;

					nTmp = nextOrders[ nBgn ];
					nextOrders[ nBgn ] = nextOrders[ nChosenIndex ];
					nextOrders[ nChosenIndex ] = nTmp;

					nBgn++;

					if(1231 == i)
						i = i;

					if( nBgn >= rIndirCntI )
					{
						break;
					}
				}
				else
				{
					int nNewDist = m_nbrDists[ nbrIndex.Calc( 
						nNbrOrder, locIndirs[ nChosenIndex ] ) ]; 

					if( nNewDist > nextDists[nChosenIndex] )
						nextDists[nChosenIndex] = nNewDist;
				}

////////////////////

				for(int j=0; j < rIndirCntI; j++)
				{
					if( nIndir == locIndirs[ j ] )
						goto IndirDone;
				}

////////////////////////////////////////

				nSelf = nIndir;
				nNbrOrder = 0;

				locIndirs[rIndirCntI] = nSelf;
				nextOrders[rIndirCntI] = nNbrOrder;
				nextDists[rIndirCntI] = m_nbrDists[ nbrIndex.Calc( 
					nNbrOrder, nSelf) ];

				rIndirCntI++;
				//indirCnts[i] = rIndirCntI;

////////////////////////////////////////

IndirDone:
				;
			}

			if(1231 == i)
				i = i;

DoneI:
			;
		}
		


//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////


		std::vector<int> featSums( nofFeatures );

		for(int i=0; i<nDataSiz; i++)
		{
			if(268 == i)
				i = i;

			memset( &featSums[0], 0, featSums.size() * sizeof(int) );

			int * resFeatsI = &resPats[ patIndex.Calc(0, i) ];

			int * locIndirs = 
				&indirs[ indirIndex.Calc(0, i) ];
			
			const int nIndirCntI = indirCnts[i];

			for(int j=0; j<nIndirCntI; j++)				
			{
				int k = locIndirs[j];

				//if(-1 == k)
				if(k < 0)
				{
					//nofActIndires = j;
					break;
				}

				int * featsK = &m_pats[ patIndex.Calc(0, k) ];

				for(int f=0; f < nofFeatures; f++)
				{
					featSums[f] += featsK[f];
				}

			}

			for(int f=0; f < nofFeatures; f++)
			{
				resFeatsI[f] = featSums[f] / nIndirCntI;
			}

			//int nTmpX = (int) (sumX / nIndirCntI);
			//int nTmpY = (int) (sumY / nIndirCntI);

			//resPatArr[i].Features[0] = nTmpX;
			//resPatArr[i].Features[1] = nTmpY;
		}


		std::vector<int> friends( nDataSiz * nofIndirs );
		IndexCalc2D friendIndex( nofIndirs, nDataSiz );
		std::vector<int> friendCnts( nDataSiz );

		m_patIDs.resize( nDataSiz );
		m_patClusterInfoIDs.resize( nDataSiz );

		memset( &m_patClusterInfoIDs[0], 0xff, m_patClusterInfoIDs.size() * sizeof(int));


		for(int i=0; i<nDataSiz; i++)
		{
			int * featsI = &m_pats[ patIndex.Calc(0, i) ];
			int * resFeatsI = &resPats[ patIndex.Calc(0, i) ];

			m_patIDs[i] = i;

			const int nofIndirsI = indirCnts[i];
			int * locIndires = 
				&indirs[ indirIndex.Calc(0, i) ];

			int * locFrniends = 
				&friends[ friendIndex.Calc(0, i) ];

			int & nFrniendCntI = friendCnts[i];
			nFrniendCntI = 0;

			for(int j=0; j<nofIndirsI; j++)
			{
				const int ind = locIndires[j];
				int * featsInd = &m_pats[ patIndex.Calc(0, ind) ];

				int nOldDist;
				{
					int sqrSum = 0;
					for(int f=0; f < nofFeatures; f++)
					{
						int nAbsDifF =  abs( featsI[f] - featsInd[f] ) * nDistStep;
						sqrSum += Sqr( nAbsDifF );
					}
					nOldDist = sqrt( (double) sqrSum );
				}

				int * resFeatsInd = &resPats[ patIndex.Calc(0, ind) ];

				int nNewDist;
				{
					int sqrSum = 0;
					for(int f=0; f < nofFeatures; f++)
					{
						int nAbsDifF =  abs( resFeatsI[f] - resFeatsInd[f] ) * nDistStep;
						sqrSum += Sqr( nAbsDifF );
					}
					nNewDist = sqrt( (double) sqrSum );
				}

				if( nNewDist > nOldDist * 1.3 )
				//if( nNewDist > nOldDist * 0.8 )
					continue;

				locFrniends[ nFrniendCntI++ ] = ind;
			}
		}


		std::vector<int> tmpArr( nDataSiz );

		for(int i=0; i<nDataSiz; i++)
		{
			const int nofFriendsI = friendCnts[i];
			int * locFriends = 
				&friends[ friendIndex.Calc(0, i) ];

			int nCnt = 0;

			int k, nID;
			int nMin = nDataSiz;

			nID = m_patIDs[i];

			while( true )
			{
				k = nID;
				tmpArr[ nCnt++ ] = k;

				if( k < nMin )
					nMin = k;				

				nID = m_patIDs[k];

				if(k == nID)
					break;
			}

			for(int j=0; j<nofFriendsI; j++)
			{
				const int nFrnd = locFriends[ j ];

				if( i < 27 && 27 == nFrnd)
					i = i;

				nID = m_patIDs[ nFrnd ];

				while( true )
				{
					k = nID;
					tmpArr[ nCnt++ ] = k;

					if( k < nMin )
						nMin = k;				

					nID = m_patIDs[k];

					if(k == nID)
						break;
				}
			}

			for(int j=0; j<nCnt; j++)
			{
				m_patIDs[ tmpArr[ j ] ] = nMin;
			}
		}


		m_clusterInfoArr.resize(0);
		

		for(int i=0; i<nDataSiz; i++)
		{
			if( 12 == i )
				i = i;

			int nCnt = 0;

			int k, nID;
			int nMin = nDataSiz;

			tmpArr[ nCnt++ ] = i;

			nID = m_patIDs[i];

			while( true )
			{
				k = nID;
				tmpArr[ nCnt++ ] = k;

				if( k < nMin )
					nMin = k;				

				nID = m_patIDs[k];

				if(k == nID)
					break;
			}

			for(int j=0; j<nCnt; j++)
				m_patIDs[ tmpArr[ j ] ] = nMin;

			if(i == m_patIDs[i])
			{
				ClusterInfo ci;

				ci.refID = i;
				ci.size = 0;

				m_clusterInfoArr.push_back( ci );

				m_patClusterInfoIDs[ i ] = m_clusterInfoArr.size() - 1;
			}
		}

		const int nofClusters = m_clusterInfoArr.size();

		//memset( &clusterInfoArr[0], 0, clusterInfoArr.size() * sizeof(ClusterInfo) );


		//std::vector<int> clusterCnts( nofClusters );
		//memset( &clusterCnts[0], 0, clusterCnts.size() * sizeof(int) );

		{
		std::vector<int> clusterIDs( nofClusters );

		int nMaxID = 0;
		clusterIDs[ nMaxID ] = 0;

		for(int i=0; i<nDataSiz; i++)
		{
			int & rPtID = m_patIDs[i];

			if(rPtID > clusterIDs[ nMaxID ])
			{
				nMaxID++;
				clusterIDs[ nMaxID ] = rPtID;
				//rPtID = nMaxID;
				m_clusterInfoArr[ nMaxID ].size++;
			}
			else
			{
				//for(int j=0; j<=nMaxID; j++)
				{
					//int j = m_patIDs[ rPtID ];
					int j = m_patClusterInfoIDs[ rPtID ];
					//if(rPtID == clusterIDs[ j ])
					{
						//rPtID = j;
						m_clusterInfoArr[ j ].size++;
						//break;
					}
				}
			}

		}

		}




	}

	
	inline int ClusteringMgr::CalcDist(int * feats1, int * feats2)
	{
		const int nofFeatures = (*m_pPatIndex).GetSizeX();

		int sqrSum = 0;
		for(int f=0; f < nofFeatures; f++)
		{
			int nAbsDifF =  abs( feats1[f] - feats2[f] ) * m_nDistStep;
			if(nAbsDifF > m_nNbrDist)
				return m_nHugeDist;

			sqrSum += Sqr( nAbsDifF );
		}

		int dist = sqrt( (double) sqrSum );
		if(dist > m_nNbrDist)
			return m_nHugeDist;

		return dist;
	}


	void ClusteringMgr::AssignNbrs()
	{
		IndexCalc2D patIndex(m_pPatIndex->GetSizeX(), 
			m_pPatIndex->GetSizeY());

		const int nofFeatures = patIndex.GetSizeX();

		const int nDataSiz = patIndex.GetSizeY();

		const int nDistStep = m_nDistStep = 10;		
		const int nNbrDist = m_nNbrDist = 30 * nDistStep;

		const int nofNbrs = m_nofNbrs = 5;


		IndexCalc2D nbrIndex( nofNbrs, nDataSiz );


		m_nbrDists.resize( nbrIndex.GetTotalSize() );
		memset( &m_nbrDists[0], 0x7f, m_nbrDists.size() * sizeof(int));

		m_nbrs.resize( nbrIndex.GetTotalSize() );

		std::vector<int> maxMinNbrIndices( nDataSiz );
		memset( &maxMinNbrIndices[0], 0, maxMinNbrIndices.size() * sizeof(int));

		

		for(int i=0; i<nDataSiz; i++)
		{
			int & nMaxMinIndexI = maxMinNbrIndices[ i ];
			int * locNbrsI = 
				&m_nbrs[ nbrIndex.Calc(0, i) ];
			int * locNbrDistsI = 
				&m_nbrDists[ nbrIndex.Calc(0, i) ];


			int * featsI = &m_pats[ patIndex.Calc(0, i) ];

			for(int j=i+1; j<nDataSiz; j++)
			{
				int * featsJ = &m_pats[ patIndex.Calc(0, j) ];

				int dist = CalcDist(featsI, featsJ);
				if(dist > nNbrDist)
					goto DoneJ;


				if(dist < locNbrDistsI[ nMaxMinIndexI ])
				{
					locNbrsI[ nMaxMinIndexI ] = j;
					locNbrDistsI[ nMaxMinIndexI ] = dist;

					for(int k=0; k < nofNbrs; k++)
					{
						if( locNbrDistsI[ k ] > 
							locNbrDistsI[ nMaxMinIndexI ] )
						{
							nMaxMinIndexI = k;
						}
					}
				}


				int & nMaxMinIndexJ = maxMinNbrIndices[ j ];
				int * locNbrsJ = 
					&m_nbrs[ nbrIndex.Calc(0, j) ];
				int * locNbrDistsJ = 
					&m_nbrDists[ nbrIndex.Calc(0, j) ];

				if(dist < locNbrDistsJ[ nMaxMinIndexJ ])
				{
					locNbrsJ[ nMaxMinIndexJ ] = i;
					locNbrDistsJ[ nMaxMinIndexJ ] = dist;

					for(int k=0; k < nofNbrs; k++)
					{
						if( locNbrDistsJ[ k ] > 
							locNbrDistsJ[ nMaxMinIndexJ ] )
						{
							nMaxMinIndexJ = k;
						}
					}
				}

DoneJ:
				;
			}
		}



	}

}