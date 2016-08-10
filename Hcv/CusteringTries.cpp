#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\PixelTypes.h>
#include <Lib\Hcv\error.h>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <vector>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\CusteringTries.h>


namespace Hcv
{
	using namespace Hcpl;


//typedef float dist_t;
typedef int dist_t;

	void DoClusteringStep(std::vector<CvPoint> * a_pDataPts,
		std::vector<CvPoint> * a_pResDataPts,
		std::vector<int> * a_pDataPtCnts)
	{
		std::vector<CvPoint> & dataPts = *a_pDataPts;
		const int nDataSiz = dataPts.size();

		std::vector<CvPoint> & resDataPts = *a_pResDataPts;

		std::vector<double> distWts( Sqr(nDataSiz) );

		IndexCalc2D index( nDataSiz, nDataSiz );

		double maxDist = 0;

		for(int i=0; i<nDataSiz; i++)
		{
			for(int j=i+1; j<nDataSiz; j++)
			{
				double dist = sqrt( double (Sqr( dataPts[i].x - dataPts[j].x ) +
						Sqr( dataPts[i].y - dataPts[j].y )) );

				distWts[ index.Calc(j, i) ] = dist;
					
				if(dist > maxDist)
					maxDist = dist;
			}
		}
		maxDist += 0.5;

		for(int i=0; i<nDataSiz; i++)
			distWts[ index.Calc(i, i) ] = 0;

		double t1 = 0.7;
		double t2 = 0.8;
		for(int i=0; i<nDataSiz; i++)
		{
			for(int j=i; j<nDataSiz; j++)
			{
				double val = distWts[ index.Calc(j, i) ];
				val = (maxDist - val) / maxDist;


				bool bStop = false;
				double oldVal = val;

				if(val >= t1 && val <= t2)
				{
					bStop = true;
				}

				if(val >0.7 || 1)
					//val = pow(val, 30);
					//val = pow(val, 3);
					val = val;
					//val = 1;
				else
					val = 0;

				if(bStop)
					val = val;

				distWts[ index.Calc(j, i) ] = val;
			}
		}


		for(int i=0; i<nDataSiz; i++)
		{
			for(int j=i+1; j<nDataSiz; j++)
			{
				distWts[ index.Calc(i, j) ] = 
					distWts[ index.Calc(j, i) ];
			}
		}

		std::vector<double> distWtSums( nDataSiz );

		for(int i=0; i<nDataSiz; i++)
		{
			double sum = 0;

			for(int j=0; j<nDataSiz; j++)
			{
				sum += distWts[ index.Calc(j, i) ];
			}

			distWtSums[ i ] = sum;
		}

		for(int i=0; i<nDataSiz; i++)
		{
			double sumX = 0;
			double sumY = 0;

			for(int j=0; j<nDataSiz; j++)
			{
				sumX += distWts[ index.Calc(j, i) ] * dataPts[j].x;
				sumY += distWts[ index.Calc(j, i) ] * dataPts[j].y;				
			}

			resDataPts[i].x = (int) sumX / distWtSums[ i ];
			resDataPts[i].y = (int) sumY / distWtSums[ i ];
		}
	}










	void DoClusteringStep2(std::vector<CvPoint> * a_pDataPts,
		std::vector<CvPoint> * a_pResDataPts,
		std::vector<int> * a_pDataPtCnts)
	{
		std::vector<CvPoint> & dataPts = *a_pDataPts;
		const int nDataSiz = dataPts.size();

		std::vector<CvPoint> & resDataPts = *a_pResDataPts;

		std::vector<double> distWts( Sqr(nDataSiz) );

		IndexCalc2D index( nDataSiz, nDataSiz );

		double maxDist = 0;

		for(int i=0; i<nDataSiz; i++)
		{
			for(int j=i+1; j<nDataSiz; j++)
			{
				double dist = sqrt( double (Sqr( dataPts[i].x - dataPts[j].x ) +
						Sqr( dataPts[i].y - dataPts[j].y )) );

				distWts[ index.Calc(j, i) ] = dist;
					
				if(dist > maxDist)
					maxDist = dist;
			}
		}
		maxDist += 0.5;

		for(int i=0; i<nDataSiz; i++)
			distWts[ index.Calc(i, i) ] = 0;

		double t1 = 0.7;
		double t2 = 0.8;
		for(int i=0; i<nDataSiz; i++)
		{
			for(int j=i; j<nDataSiz; j++)
			{
				double val = distWts[ index.Calc(j, i) ];
				val = (maxDist - val) / maxDist;


				bool bStop = false;
				double oldVal = val;

				if(val >= t1 && val <= t2)
				{
					bStop = true;
				}

				if(val >0.7 || 1)
					//val = pow(val, 0.30);
					//val = pow(val, 3);
					val = val;
					//val = 1;
				else
					val = 0;

				if(bStop)
					val = val;

				distWts[ index.Calc(j, i) ] = val;
			}
		}


		for(int i=0; i<nDataSiz; i++)
		{
			for(int j=i+1; j<nDataSiz; j++)
			{
				distWts[ index.Calc(i, j) ] = 
					distWts[ index.Calc(j, i) ];
			}
		}

		//const int nofNbrs = nDataSiz;
		const int nofNbrs = 100;

		std::vector<int> dataCloseNbrs( nDataSiz * nofNbrs );
		IndexCalc2D nbrIndex( nofNbrs ,nDataSiz );

		for(int i=0; i<nDataSiz; i++)
		{
			double minMaxDstWt = 10;
			int minMaxDstWtIndex;

			//std::vector<int>::pointer locDataCloseNbrs = 
			int * locDataCloseNbrs = 
				&dataCloseNbrs[ nbrIndex.Calc(0, i) ];
			//std::vector<double>::pointer locDistWts =
			double * locDistWts =
				&distWts[ index.Calc(0, i) ];

			

			for(int j=0; j<nofNbrs; j++)
			{
				locDataCloseNbrs[ j ] = j;

				double val = locDistWts[ j ];
				
				if( val < minMaxDstWt )
				{
					minMaxDstWt = val;
					minMaxDstWtIndex = j;
				}
			}

			for(int j = nofNbrs; j < nDataSiz; j++)
			{
				double val = distWts[ index.Calc(j, i) ];
				
				if( val > minMaxDstWt )
				{
					minMaxDstWt = val;
					locDataCloseNbrs[ minMaxDstWtIndex ] = j;
					
					for(int k=0; k < nofNbrs; k++)
					{
						double val2 = locDistWts[ locDataCloseNbrs[ k ] ];

						if(  val2 < minMaxDstWt )
						{
							minMaxDstWt = val2;
							minMaxDstWtIndex = k;
						}
					}

				}
			}

		}



		for(int i=0; i<nDataSiz; i++)
		{
			double sumX = 0;
			double sumY = 0;
			double sumWts = 0;

			//std::vector<int>::pointer locDataCloseNbrs = 
			int * locDataCloseNbrs = 
				&dataCloseNbrs[ nbrIndex.Calc(0, i) ];
			//std::vector<double>::pointer locDistWts =
			double * locDistWts =
				&distWts[ index.Calc(0, i) ];


			for(int j=0; j<nofNbrs; j++)
			{
				int k = locDataCloseNbrs[j];
				double wt = locDistWts[k];

				sumWts += wt;

				sumX += wt * dataPts[k].x;
				sumY += wt * dataPts[k].y;
			}

			resDataPts[i].x = (int) (sumX / sumWts);
			resDataPts[i].y = (int) (sumY / sumWts);
		}
	}







	void DoClusteringStep3(std::vector<CvPoint> * a_pDataPts,
		std::vector<CvPoint> * a_pResDataPts,
		std::vector<int> * a_pDataPtCnts)
	{
		std::vector<CvPoint> & dataPts = *a_pDataPts;
		const int nDataSiz = dataPts.size();

		std::vector<CvPoint> & resDataPts = *a_pResDataPts;

		std::vector<double> distWts( Sqr(nDataSiz) );

		IndexCalc2D index( nDataSiz, nDataSiz );

		double maxDist = 0;

		for(int i=0; i<nDataSiz; i++)
		{
			for(int j=i+1; j<nDataSiz; j++)
			{
				double dist = sqrt( double (Sqr( dataPts[i].x - dataPts[j].x ) +
						Sqr( dataPts[i].y - dataPts[j].y )) );

				distWts[ index.Calc(j, i) ] = dist;
					
				if(dist > maxDist)
					maxDist = dist;
			}
		}
		maxDist += 0.5;

		for(int i=0; i<nDataSiz; i++)
			distWts[ index.Calc(i, i) ] = 0;

		double t1 = 0.7;
		double t2 = 0.8;
		for(int i=0; i<nDataSiz; i++)
		{
			for(int j=i; j<nDataSiz; j++)
			{
				double val = distWts[ index.Calc(j, i) ];
				val = (maxDist - val) / maxDist;


				bool bStop = false;
				double oldVal = val;

				if(val >= t1 && val <= t2)
				{
					bStop = true;
				}

				if(val >0.7 || 1)
					//val = pow(val, 0.30);
					//val = pow(val, 3);
					val = val;
					//val = 1;
				else
					val = 0;

				if(bStop)
					val = val;

				distWts[ index.Calc(j, i) ] = val;
			}
		}


		for(int i=0; i<nDataSiz; i++)
		{
			for(int j=i+1; j<nDataSiz; j++)
			{
				distWts[ index.Calc(i, j) ] = 
					distWts[ index.Calc(j, i) ];
			}
		}

		//const int nofNbrs = nDataSiz;
		const int nofNbrs = 40;

		std::vector<int> dataCloseNbrs( nDataSiz * nofNbrs );
		IndexCalc2D nbrIndex( nofNbrs ,nDataSiz );

		for(int i=0; i<nDataSiz; i++)
		{
			double minMaxDstWt = 10;
			int minMaxDstWtIndex;

			//std::vector<int>::pointer locDataCloseNbrs = 
			int * locDataCloseNbrs = 
				&dataCloseNbrs[ nbrIndex.Calc(0, i) ];
			//std::vector<double>::pointer locDistWts =
			double * locDistWts =
				&distWts[ index.Calc(0, i) ];

			

			for(int j=0; j<nofNbrs; j++)
			{
				locDataCloseNbrs[ j ] = j;

				double val = locDistWts[ j ];
				
				if( val < minMaxDstWt )
				{
					minMaxDstWt = val;
					minMaxDstWtIndex = j;
				}
			}

			for(int j = nofNbrs; j < nDataSiz; j++)
			{
				double val = distWts[ index.Calc(j, i) ];
				
				if( val > minMaxDstWt )
				{
					minMaxDstWt = val;
					locDataCloseNbrs[ minMaxDstWtIndex ] = j;
					
					for(int k=0; k < nofNbrs; k++)
					{
						double val2 = locDistWts[ locDataCloseNbrs[ k ] ];

						if(  val2 < minMaxDstWt )
						{
							minMaxDstWt = val2;
							minMaxDstWtIndex = k;
						}
					}

				}
			}

		}



		for(int i=0; i<nDataSiz; i++)
		{
			//std::vector<int>::pointer locDataCloseNbrs = 
			int * locDataCloseNbrs = 
				&dataCloseNbrs[ nbrIndex.Calc(0, i) ];
			//std::vector<double>::pointer locDistWts =
			double * locDistWts =
				&distWts[ index.Calc(0, i) ];


			double wtSums[nofNbrs];
			int maxSumIndex = 0;

			for(int j=0; j<nofNbrs; j++)
			{
				wtSums[j] = 0;
				int nValJ = locDataCloseNbrs[j];

				for(int k=0; k<nofNbrs; k++)
				{
					//wtSums[j] += distWts[ index.Calc(
					//	locDataCloseNbrs[k], nValJ) ];

					wtSums[j] += ( distWts[ index.Calc(
						k, nValJ) ] ) + locDistWts[nValJ];
				}

				if(wtSums[j] > wtSums[maxSumIndex])
				{
					maxSumIndex = j;
				}

			}

			resDataPts[i].x = dataPts[ locDataCloseNbrs[maxSumIndex] ].x;
			resDataPts[i].y = dataPts[ locDataCloseNbrs[maxSumIndex] ].y;
		}
	}



	void DoClusteringStep4(std::vector<CvPoint> * a_pDataPts,
		std::vector<CvPoint> * a_pResDataPts,
		std::vector<int> * a_pDataPtCnts)
	{
		std::vector<CvPoint> & dataPts = *a_pDataPts;
		const int nDataSiz = dataPts.size();

		std::vector<CvPoint> & resDataPts = *a_pResDataPts;

		std::vector<int> distMat( Sqr(nDataSiz) );

		IndexCalc2D distIndex( nDataSiz, nDataSiz );

		int maxDist = 0;

		//const int nHugeDist = 1000000;

		for(int i=0; i<nDataSiz; i++)
		{
			for(int j=i+1; j<nDataSiz; j++)
			{
				//int dist = sqrt( (double) (Sqr( dataPts[i].x - dataPts[j].x ) +
				int dist = ( (Sqr( dataPts[i].x - dataPts[j].x ) +
						Sqr( dataPts[i].y - dataPts[j].y )) );

				distMat[ distIndex.Calc(j, i) ] = dist;
					
				if(dist > maxDist)
					maxDist = dist;
			}
		}

		for(int i=0; i<nDataSiz; i++)
			distMat[ distIndex.Calc(i, i) ] = 0;

		for(int i=0; i<nDataSiz; i++)
		{
			for(int j=i+1; j<nDataSiz; j++)
			{
				distMat[ distIndex.Calc(i, j) ] = 
					distMat[ distIndex.Calc(j, i) ];
			}
		}

		//const int nofNbrs = 10;
		const int nofNbrs = 5;
		//const int nofIndirs = 150;
		const int nofIndirs = 20;

		std::vector<int> nbrs( nDataSiz * nofNbrs );
		IndexCalc2D nbrIndex( nofNbrs, nDataSiz );

		int tmpNbrs[ nofNbrs + 1 ];
		for(int i=0; i<nDataSiz; i++)
		{
			int maxMinIndex = 0;

			int * locNbrs = 
				&nbrs[ nbrIndex.Calc(0, i) ];
			int * locDistMat =
				&distMat[ distIndex.Calc(0, i) ];

			

			for(int j=0; j < nofNbrs + 1; j++)
			{
				tmpNbrs[ j ] = j;

				if( locDistMat[ j ] > locDistMat[ maxMinIndex ] )
				{
					maxMinIndex = j;
				}
			}

			for(int j = nofNbrs + 1; j < nDataSiz; j++)
			{

				if( locDistMat[ j ] < 
					locDistMat[ tmpNbrs[ maxMinIndex ] ] )
				{
					tmpNbrs[ maxMinIndex ] = j;
					
					for(int k=0; k < nofNbrs + 1; k++)
					{

						if( locDistMat[ tmpNbrs[ k ] ] > 
							locDistMat[ tmpNbrs[ maxMinIndex ] ] )
						{
							maxMinIndex = k;
						}
					}

				}
			}

			for(int j=0; j < nofNbrs + 1; j++)
			{
				if(i == tmpNbrs[j])
				{
					tmpNbrs[j] = tmpNbrs[ nofNbrs ];
					break;
				}
			}

			for(int j=0; j < nofNbrs; j++)
			{
				int nMinIndex = j;

				for(int k= j + 1; k < nofNbrs; k++)
				{
					if(locDistMat[ tmpNbrs[k] ] <
						locDistMat[ tmpNbrs[nMinIndex] ])
						nMinIndex = k;
				}
				
				int nTmp = tmpNbrs[j];
				tmpNbrs[j] = tmpNbrs[ nMinIndex ];
				tmpNbrs[ nMinIndex ] = nTmp;
			}

			for(int j=0; j < nofNbrs; j++)
			{
				locNbrs[j] = tmpNbrs[j];
			}

		}


		std::vector<int> indirs( nDataSiz * nofIndirs );
		IndexCalc2D indirIndex( nofIndirs, nDataSiz );

		for(int i=0; i<nDataSiz; i++)
		{
			int * locNbrs = 
				&nbrs[ nbrIndex.Calc(0, i) ];
			int * locIndirs = 
				&indirs[ indirIndex.Calc(0, i) ];

			int nIndirCnt = 0;
			for(int j=0; j < nofNbrs; j++)
				locIndirs[ j ] = locNbrs[ j ];

			nIndirCnt = nofNbrs;


///
/*			
			locIndirs[0] = i;
			locIndirs[0] = locNbrs[ 0 ];

			nIndirCnt = 1;
			*/
///


			//for(int j=1; j < nIndirCnt; j++)
			for(int j=0; j < nIndirCnt; j++)
			{
				int * locNbrs2 = 
					&nbrs[ nbrIndex.Calc(0, locIndirs[ j ]) ];

				for(int k=0; k<nofNbrs; k++)
				//for(int k=0; k< (nofNbrs * 1) / 10; k++)
				{
					int nTmp = locNbrs2[ k ];

					for(int m=0; m < nIndirCnt; m++)
					{
						if( nTmp == locIndirs[ m ])
							goto CheckDone;
					}

					locIndirs[ nIndirCnt++ ] = nTmp;

					if(nIndirCnt >= nofIndirs)
						goto IndirComplete;

					locIndirs[ nIndirCnt ] = -1;
CheckDone:
					;
				}

			}
IndirComplete:
			;
		}



		for(int i=0; i<nDataSiz; i++)
		{
			int sumX = 0;
			int sumY = 0;

			int * locIndirs = 
				&indirs[ indirIndex.Calc(0, i) ];

			int nofActIndires = nofIndirs;
			for(int j=0; j<nofIndirs; j++)
			{
				int k = locIndirs[j];

				//if(-1 == k)
				if(k < 0)
				{
					nofActIndires = j;
					break;
				}

				sumX += dataPts[k].x;
				sumY += dataPts[k].y;
			}

			resDataPts[i].x = (int) (sumX / nofActIndires);
			resDataPts[i].y = (int) (sumY / nofActIndires);
		}
	}



/*
	void DoClusteringStep5(std::vector<CvPoint> * a_pDataPts,
		std::vector<CvPoint> * a_pResDataPts,
		std::vector<int> * a_pDataPtCnts)
	{
		std::vector<CvPoint> & dataPts = *a_pDataPts;
		const int nDataSiz = dataPts.size();

		std::vector<CvPoint> & resDataPts = *a_pResDataPts;

		std::vector<int> distMat( Sqr(nDataSiz) );

		IndexCalc2D distIndex( nDataSiz, nDataSiz );

		int maxDist = 0;

		for(int i=0; i<nDataSiz; i++)
		{
			for(int j=i+1; j<nDataSiz; j++)
			{
				//int dist = sqrt( (double) (Sqr( dataPts[i].x - dataPts[j].x ) +
				int dist = ( (Sqr( dataPts[i].x - dataPts[j].x ) +
						Sqr( dataPts[i].y - dataPts[j].y )) );

				distMat[ distIndex.Calc(j, i) ] = dist;
					
				if(dist > maxDist)
					maxDist = dist;
			}
		}

		for(int i=0; i<nDataSiz; i++)
			distMat[ distIndex.Calc(i, i) ] = 0;

		for(int i=0; i<nDataSiz; i++)
		{
			for(int j=i+1; j<nDataSiz; j++)
			{
				distMat[ distIndex.Calc(i, j) ] = 
					distMat[ distIndex.Calc(j, i) ];
			}
		}

		const int nofNbrs = 10;
		const int nofIndirs = 60;

		std::vector<int> nbrs( nDataSiz * nofNbrs );
		IndexCalc2D nbrIndex( nofNbrs, nDataSiz );

		int tmpNbrs[ nofNbrs + 1 ];
		for(int i=0; i<nDataSiz; i++)
		{
			int maxMinIndex = 0;

			int * locNbrs = 
				&nbrs[ nbrIndex.Calc(0, i) ];
			int * locDistMat =
				&distMat[ distIndex.Calc(0, i) ];

			

			for(int j=0; j < nofNbrs + 1; j++)
			{
				tmpNbrs[ j ] = j;

				if( locDistMat[ j ] > locDistMat[ maxMinIndex ] )
				{
					maxMinIndex = j;
				}
			}

			for(int j = nofNbrs + 1; j < nDataSiz; j++)
			{

				if( locDistMat[ j ] < 
					locDistMat[ tmpNbrs[ maxMinIndex ] ] )
				{
					tmpNbrs[ maxMinIndex ] = j;
					
					for(int k=0; k < nofNbrs + 1; k++)
					{

						if( locDistMat[ tmpNbrs[ k ] ] > 
							locDistMat[ tmpNbrs[ maxMinIndex ] ] )
						{
							maxMinIndex = k;
						}
					}

				}
			}

			for(int j=0; j < nofNbrs + 1; j++)
			{
				if(i == tmpNbrs[j])
				{
					tmpNbrs[j] = tmpNbrs[ nofNbrs ];
					break;
				}
			}

			for(int j=0; j < nofNbrs; j++)
			{
				int nMinIndex = j;

				for(int k= j + 1; k < nofNbrs; k++)
				{
					if(locDistMat[ tmpNbrs[k] ] <
						locDistMat[ tmpNbrs[nMinIndex] ])
						nMinIndex = k;
				}
				
				int nTmp = tmpNbrs[j];
				tmpNbrs[j] = tmpNbrs[ nMinIndex ];
				tmpNbrs[ nMinIndex ] = nTmp;
			}

			for(int j=0; j < nofNbrs; j++)
			{
				locNbrs[j] = tmpNbrs[j];
			}

		}


		std::vector<int> indirs( nDataSiz * nofIndirs );
		IndexCalc2D indirIndex( nofIndirs, nDataSiz );

		int nextDists[ nofIndirs ];
		int nextOrders[ nofIndirs ];

		for(int i=0; i<nDataSiz; i++)
		{
			int * locIndirs = 
				&indirs[ indirIndex.Calc(0, i) ];

			int nSelf;
			int nNbrOrder;

////////////////////////////////

			nSelf = i;
			nNbrOrder = 1;

			locIndirs[0] = nSelf;
			nextOrders[0] = nNbrOrder;
			nextDists[0] = distMat[ distIndex.Calc( 
				nbrs[ nbrIndex.Calc( nNbrOrder, nSelf) ], nSelf ) ];

			nSelf = nbrs[ nbrIndex.Calc( 0, i) ];
			nNbrOrder = 0;

			locIndirs[1] = nSelf;
			nextOrders[1] = nNbrOrder;
			nextDists[1] = distMat[ distIndex.Calc( 
				nbrs[ nbrIndex.Calc( nNbrOrder, nSelf) ], nSelf ) ];

			int nIndirCnt = 2;

////////////////////////////////

			while(nIndirCnt < nofIndirs)
			{
				int nMinDistIndex = 0;
				for(int j=0; j < nIndirCnt; j++)
				{
					if( nextDists[j] < nextDists[ nMinDistIndex ])
						nMinDistIndex = j;
				}

////////////////////

				if( nextDists[ nMinDistIndex ] == maxDist )
					break;

				nSelf = locIndirs[ nMinDistIndex ];

				const int nIndir = nbrs[ nbrIndex.Calc( 
					nextOrders[ nMinDistIndex ], nSelf ) ];

				nNbrOrder = ++nextOrders[ nMinDistIndex ];
				if(nNbrOrder >= nofNbrs)
				{
					nextDists[ nMinDistIndex ] = maxDist;
				}
				else
				{
					nextDists[ nMinDistIndex ] = distMat[ distIndex.Calc( 
						nbrs[ nbrIndex.Calc( nNbrOrder, nSelf) ], nSelf ) ];
				}

////////////////////

				for(int j=0; j < nIndirCnt; j++)
				{
					if( nIndir == locIndirs[ j ] )
						goto IndirDone;
				}

////////////////////////////////////////

			nSelf = nIndir;
			nNbrOrder = 0;

			locIndirs[nIndirCnt] = nSelf;
			nextOrders[nIndirCnt] = nNbrOrder;
			nextDists[nIndirCnt] = distMat[ distIndex.Calc( 
				nbrs[ nbrIndex.Calc( nNbrOrder, nSelf) ], nSelf ) ];

			nIndirCnt++;

////////////////////////////////////////

IndirDone:
				;
			}

			if(nIndirCnt < nofIndirs)
				locIndirs[ nIndirCnt ] = -1;
		}



		for(int i=0; i<nDataSiz; i++)
		{
			int sumX = 0;
			int sumY = 0;

			int * locIndirs = 
				&indirs[ indirIndex.Calc(0, i) ];

			//int nofActIndires = nofIndirs;
			int nofActIndires = 0;
			for(int j=0; j<nofIndirs; j++)
			//for(int j= (nofIndirs * 1) / 3; j<nofIndirs; j++)
			{
				int k = locIndirs[j];

				//if(-1 == k)
				if(k < 0)
				{
					//nofActIndires = j;
					break;
				}

				sumX += dataPts[k].x;
				sumY += dataPts[k].y;

				nofActIndires++;
			}

			int nTmpX = (int) (sumX / nofActIndires);
			int nTmpY = (int) (sumY / nofActIndires);

			int nOrgX = dataPts[i].x;
			int nOrgY = dataPts[i].y;

			int nAlfa = 1;

			resDataPts[i].x = nOrgX + (nTmpX - nOrgX) / nAlfa;
			resDataPts[i].y = nOrgY + (nTmpY - nOrgY) / nAlfa;
		}
	}


*/

	void DoClusteringStep6(std::vector<CvPoint> * a_pDataPts,
		std::vector<CvPoint> * a_pResDataPts,
		std::vector<int> * a_pDataPtCnts)
	{
		std::vector<CvPoint> & dataPts = *a_pDataPts;
		const int nDataSiz = dataPts.size();

		std::vector<CvPoint> & resDataPts = *a_pResDataPts;

		std::vector<int> distMat( Sqr(nDataSiz) );

		IndexCalc2D distIndex( nDataSiz, nDataSiz );

		int maxDist = 0;

		for(int i=0; i<nDataSiz; i++)
		{
			for(int j=i+1; j<nDataSiz; j++)
			{
				//int dist = sqrt( (double) (Sqr( dataPts[i].x - dataPts[j].x ) +
				int dist = ( (Sqr( dataPts[i].x - dataPts[j].x ) +
						Sqr( dataPts[i].y - dataPts[j].y )) );

				distMat[ distIndex.Calc(j, i) ] = dist;
					
				if(dist > maxDist)
					maxDist = dist;
			}
		}

		for(int i=0; i<nDataSiz; i++)
			distMat[ distIndex.Calc(i, i) ] = 0;

		for(int i=0; i<nDataSiz; i++)
		{
			for(int j=i+1; j<nDataSiz; j++)
			{
				distMat[ distIndex.Calc(i, j) ] = 
					distMat[ distIndex.Calc(j, i) ];
			}
		}

		const int nofNbrs = 10;
		const int nofIndirs = 30;
		const int nIndirGrowth = 7;

		std::vector<int> nbrs( nDataSiz * nofNbrs );
		IndexCalc2D nbrIndex( nofNbrs, nDataSiz );

		int tmpNbrs[ nofNbrs + 1 ];
		for(int i=0; i<nDataSiz; i++)
		{
			int maxMinIndex = 0;

			int * locNbrs = 
				&nbrs[ nbrIndex.Calc(0, i) ];
			int * locDistMat =
				&distMat[ distIndex.Calc(0, i) ];

			

			for(int j=0; j < nofNbrs + 1; j++)
			{
				tmpNbrs[ j ] = j;

				if( locDistMat[ j ] > locDistMat[ maxMinIndex ] )
				{
					maxMinIndex = j;
				}
			}

			for(int j = nofNbrs + 1; j < nDataSiz; j++)
			{

				if( locDistMat[ j ] < 
					locDistMat[ tmpNbrs[ maxMinIndex ] ] )
				{
					tmpNbrs[ maxMinIndex ] = j;
					
					for(int k=0; k < nofNbrs + 1; k++)
					{

						if( locDistMat[ tmpNbrs[ k ] ] > 
							locDistMat[ tmpNbrs[ maxMinIndex ] ] )
						{
							maxMinIndex = k;
						}
					}

				}
			}

			for(int j=0; j < nofNbrs + 1; j++)
			{
				if(i == tmpNbrs[j])
				{
					tmpNbrs[j] = tmpNbrs[ nofNbrs ];
					break;
				}
			}

			for(int j=0; j < nofNbrs; j++)
			{
				int nMinIndex = j;

				for(int k= j + 1; k < nofNbrs; k++)
				{
					if(locDistMat[ tmpNbrs[k] ] <
						locDistMat[ tmpNbrs[nMinIndex] ])
						nMinIndex = k;
				}
				
				int nTmp = tmpNbrs[j];
				tmpNbrs[j] = tmpNbrs[ nMinIndex ];
				tmpNbrs[ nMinIndex ] = nTmp;
			}

			for(int j=0; j < nofNbrs; j++)
			{
				locNbrs[j] = tmpNbrs[j];
			}

		}


		std::vector<int> indirs( nDataSiz * nofIndirs );
		IndexCalc2D indirIndex( nofIndirs, nDataSiz );

		int nextDists[ nofIndirs ];
		int nextOrders[ nofIndirs ];

		for(int i=0; i<nDataSiz; i++)
		{
			int * locIndirs = 
				&indirs[ indirIndex.Calc(0, i) ];

			int nSelf;
			int nNbrOrder;

////////////////////////////////

			nSelf = i;
			nNbrOrder = 1;

			locIndirs[0] = nSelf;
			nextOrders[0] = nNbrOrder;
			nextDists[0] = distMat[ distIndex.Calc( 
				nbrs[ nbrIndex.Calc( nNbrOrder, nSelf) ], nSelf ) ];

			nSelf = nbrs[ nbrIndex.Calc( 0, i) ];
			nNbrOrder = 0;

			locIndirs[1] = nSelf;
			nextOrders[1] = nNbrOrder;
			nextDists[1] = distMat[ distIndex.Calc( 
				nbrs[ nbrIndex.Calc( nNbrOrder, nSelf) ], nSelf ) ];

			int nIndirCnt = 2;

////////////////////////////////

			int cycleTraces[nofIndirs];
			memset(cycleTraces, 0, nofIndirs * sizeof(int));

			int nCycleID = 1;
			//int nCycleSiz = (nIndirCnt / 2);
			int nCycleSiz = 1;
			int nCycleCnt = 0;

			while(nIndirCnt < nofIndirs)
			{
				int nMinDistIndex = 0;
				for(int j=0; j < nIndirCnt; j++)
				{
					if(nCycleID == cycleTraces[j])
						continue;

					if( nextDists[j] < nextDists[ nMinDistIndex ])
						nMinDistIndex = j;
				}

////////////////////

				if( nextDists[ nMinDistIndex ] == maxDist )
					break;

				nSelf = locIndirs[ nMinDistIndex ];

				const int nIndir = nbrs[ nbrIndex.Calc( 
					nextOrders[ nMinDistIndex ], nSelf ) ];

				nNbrOrder = ++nextOrders[ nMinDistIndex ];
				if(nNbrOrder >= nofNbrs)
				{
					nextDists[ nMinDistIndex ] = maxDist;
				}
				else
				{
					int nNewDist = distMat[ distIndex.Calc( 
						nbrs[ nbrIndex.Calc( nNbrOrder, nSelf) ], nSelf ) ];

					if(nNewDist > nextDists[ nMinDistIndex ])
						nextDists[ nMinDistIndex ] = nNewDist;
				}

				cycleTraces[ nMinDistIndex ] = nCycleID;
				nCycleCnt++;

				if( nCycleCnt >= nCycleSiz)
				{
					nCycleID++;
					nCycleSiz = (nIndirCnt < nIndirGrowth) ?
					//nCycleSiz = nIndirCnt * 0.5;
						//1 : nIndirCnt * 10 / 10;
						//(nIndirCnt / 2) : nIndirGrowth;
						1 : 1;
					nCycleCnt = 0;
				}

////////////////////

				for(int j=0; j < nIndirCnt; j++)
				{
					if( nIndir == locIndirs[ j ] )
						goto IndirDone;
				}

////////////////////////////////////////

				nSelf = nIndir;
				nNbrOrder = 0;

				locIndirs[nIndirCnt] = nSelf;
				nextOrders[nIndirCnt] = nNbrOrder;
				nextDists[nIndirCnt] = distMat[ distIndex.Calc( 
					nbrs[ nbrIndex.Calc( nNbrOrder, nSelf) ], nSelf ) ];

				nIndirCnt++;

////////////////////////////////////////

IndirDone:
				;
			}

			if(nIndirCnt < nofIndirs)
				locIndirs[ nIndirCnt ] = -1;
		}



		const int nMaxMove = 7;
		for(int i=0; i<nDataSiz; i++)
		{
			int sumX = 0;
			int sumY = 0;

			int * locIndirs = 
				&indirs[ indirIndex.Calc(0, i) ];

			//int nofActIndires = nofIndirs;
			int nofActIndires = 0;
			for(int j=0; j<nofIndirs; j++)				
			//for(int j= (nofIndirs * 1) / 3; j<nofIndirs; j++)
			//for(int j = nofIndirs - nIndirGrowth; j<nofIndirs; j++)
			{
				int k = locIndirs[j];

				//if(-1 == k)
				if(k < 0)
				{
					//nofActIndires = j;
					break;
				}

				sumX += dataPts[k].x;
				sumY += dataPts[k].y;

				nofActIndires++;
			}

			int nTmpX = (int) (sumX / nofActIndires);
			int nTmpY = (int) (sumY / nofActIndires);

			int nOrgX = dataPts[i].x;
			int nOrgY = dataPts[i].y;

			double diff = sqrt( (double)
				( Sqr( nOrgX - nTmpX ) + Sqr( nOrgY - nTmpY ) ) );

			//double alfa = 0.1;
			double alfa = nMaxMove / diff;
			if(alfa > 1)
				alfa = 1;
//				alfa = alfa;

				alfa = 1;

			resDataPts[i].x = (int)(nOrgX + (nTmpX - nOrgX) * alfa);
			resDataPts[i].y = (int)(nOrgY + (nTmpY - nOrgY) * alfa);
		}
	}





	void DoClusteringStep7(std::vector<CvPoint> * a_pDataPts,
		std::vector<CvPoint> * a_pResDataPts,
		std::vector<int> * a_pDataPtCnts)
	{
		std::vector<CvPoint> & dataPts = *a_pDataPts;
		const int nDataSiz = dataPts.size();

		std::vector<CvPoint> & resDataPts = *a_pResDataPts;

		std::vector<int> distMat( Sqr(nDataSiz) );

		IndexCalc2D distIndex( nDataSiz, nDataSiz );

		int maxDist = 0;

		for(int i=0; i<nDataSiz; i++)
		{
			for(int j=i+1; j<nDataSiz; j++)
			{
				//int dist = sqrt( (double) (Sqr( dataPts[i].x - dataPts[j].x ) +
				int dist = ( (Sqr( dataPts[i].x - dataPts[j].x ) +
						Sqr( dataPts[i].y - dataPts[j].y )) );

				distMat[ distIndex.Calc(j, i) ] = dist;
					
				if(dist > maxDist)
					maxDist = dist;
			}
		}

		for(int i=0; i<nDataSiz; i++)
			distMat[ distIndex.Calc(i, i) ] = 0;

		for(int i=0; i<nDataSiz; i++)
		{
			for(int j=i+1; j<nDataSiz; j++)
			{
				distMat[ distIndex.Calc(i, j) ] = 
					distMat[ distIndex.Calc(j, i) ];
			}
		}

		const int nofNbrs = 10;
		const int nofIndirs = 30;

		std::vector<int> nbrs( nDataSiz * nofNbrs );
		IndexCalc2D nbrIndex( nofNbrs, nDataSiz );

		int tmpNbrs[ nofNbrs + 1 ];
		for(int i=0; i<nDataSiz; i++)
		{
			int maxMinIndex = 0;

			int * locNbrs = 
				&nbrs[ nbrIndex.Calc(0, i) ];
			int * locDistMat =
				&distMat[ distIndex.Calc(0, i) ];

			

			for(int j=0; j < nofNbrs + 1; j++)
			{
				tmpNbrs[ j ] = j;

				if( locDistMat[ j ] > locDistMat[ maxMinIndex ] )
				{
					maxMinIndex = j;
				}
			}

			for(int j = nofNbrs + 1; j < nDataSiz; j++)
			{

				if( locDistMat[ j ] < 
					locDistMat[ tmpNbrs[ maxMinIndex ] ] )
				{
					tmpNbrs[ maxMinIndex ] = j;
					
					for(int k=0; k < nofNbrs + 1; k++)
					{

						if( locDistMat[ tmpNbrs[ k ] ] > 
							locDistMat[ tmpNbrs[ maxMinIndex ] ] )
						{
							maxMinIndex = k;
						}
					}

				}
			}

			for(int j=0; j < nofNbrs + 1; j++)
			{
				if(i == tmpNbrs[j])
				{
					tmpNbrs[j] = tmpNbrs[ nofNbrs ];
					break;
				}
			}

			for(int j=0; j < nofNbrs; j++)
			{
				int nMinIndex = j;

				for(int k= j + 1; k < nofNbrs; k++)
				{
					if(locDistMat[ tmpNbrs[k] ] <
						locDistMat[ tmpNbrs[nMinIndex] ])
						nMinIndex = k;
				}
				
				int nTmp = tmpNbrs[j];
				tmpNbrs[j] = tmpNbrs[ nMinIndex ];
				tmpNbrs[ nMinIndex ] = nTmp;
			}

			for(int j=0; j < nofNbrs; j++)
			{
				locNbrs[j] = tmpNbrs[j];
			}

		}


		std::vector<int> indirs( nDataSiz * nofIndirs );
		IndexCalc2D indirIndex( nofIndirs, nDataSiz );

		const int nStepLim = nofIndirs * 3;

		int steps[ nStepLim ];
		int nextDists[ nStepLim ];
		int nextOrders[ nStepLim ];
		int stepCnts[ nStepLim ];
		int stepParents[ nStepLim ];

		for(int i=0; i<nDataSiz; i++)
		{
			//int * locIndirs = 
			//	&indirs[ indirIndex.Calc(0, i) ];

			int nSelf;
			int nNbrOrder;

////////////////////////////////

			nSelf = i;
			nNbrOrder = 1;

			//locIndirs[0] = nSelf;
			nextOrders[0] = nNbrOrder;
			nextDists[0] = distMat[ distIndex.Calc( 
				nbrs[ nbrIndex.Calc( nNbrOrder, nSelf) ], nSelf ) ];

			nSelf = nbrs[ nbrIndex.Calc( 0, i) ];
			nNbrOrder = 0;

			//locIndirs[1] = nSelf;
			nextOrders[1] = nNbrOrder;
			nextDists[1] = distMat[ distIndex.Calc( 
				nbrs[ nbrIndex.Calc( nNbrOrder, nSelf) ], nSelf ) ];

			int nIndirCnt = 2;

////////////////////////////////

			while(nIndirCnt < nofIndirs)
			{
				int nMinDistIndex = 0;
				for(int j=0; j < nIndirCnt; j++)
				{
					if( nextDists[j] < nextDists[ nMinDistIndex ])
						nMinDistIndex = j;
				}

////////////////////

				if( nextDists[ nMinDistIndex ] == maxDist )
					break;

				//nSelf = locIndirs[ nMinDistIndex ];

				const int nIndir = nbrs[ nbrIndex.Calc( 
					nextOrders[ nMinDistIndex ], nSelf ) ];

				nNbrOrder = ++nextOrders[ nMinDistIndex ];
				if(nNbrOrder >= nofNbrs)
				{
					nextDists[ nMinDistIndex ] = maxDist;
				}
				else
				{
					int nNewDist = distMat[ distIndex.Calc( 
						nbrs[ nbrIndex.Calc( nNbrOrder, nSelf) ], nSelf ) ];

					//if(nNewDist > nextDists[ nMinDistIndex ])
						nextDists[ nMinDistIndex ] = nNewDist;
				}

////////////////////

				for(int j=0; j < nIndirCnt; j++)
				{
					//if( nIndir == locIndirs[ j ] )
						goto IndirDone;
				}

////////////////////////////////////////

				nSelf = nIndir;
				nNbrOrder = 0;

				//locIndirs[nIndirCnt] = nSelf;
				nextOrders[nIndirCnt] = nNbrOrder;
				nextDists[nIndirCnt] = distMat[ distIndex.Calc( 
					nbrs[ nbrIndex.Calc( nNbrOrder, nSelf) ], nSelf ) ];

				nIndirCnt++;

////////////////////////////////////////

IndirDone:
				;
			}

//			if(nIndirCnt < nofIndirs)
//				locIndirs[ nIndirCnt ] = -1;
		}



		const int nMaxMove = 7;
		for(int i=0; i<nDataSiz; i++)
		{
			int sumX = 0;
			int sumY = 0;

			int * locIndirs = 
				&indirs[ indirIndex.Calc(0, i) ];

			//int nofActIndires = nofIndirs;
			int nofActIndires = 0;
			for(int j=0; j<nofIndirs; j++)				
			//for(int j= (nofIndirs * 1) / 3; j<nofIndirs; j++)
			//for(int j = nofIndirs - nIndirGrowth; j<nofIndirs; j++)
			{
				int k = locIndirs[j];

				//if(-1 == k)
				if(k < 0)
				{
					//nofActIndires = j;
					break;
				}

				sumX += dataPts[k].x;
				sumY += dataPts[k].y;

				nofActIndires++;
			}

			int nTmpX = (int) (sumX / nofActIndires);
			int nTmpY = (int) (sumY / nofActIndires);

			int nOrgX = dataPts[i].x;
			int nOrgY = dataPts[i].y;

			double diff = sqrt( (double)
				( Sqr( nOrgX - nTmpX ) + Sqr( nOrgY - nTmpY ) ) );

			//double alfa = 0.1;
			double alfa = nMaxMove / diff;
			if(alfa > 1)
				alfa = 1;
//				alfa = alfa;

				alfa = 1;

			resDataPts[i].x = (int)(nOrgX + (nTmpX - nOrgX) * alfa);
			resDataPts[i].y = (int)(nOrgY + (nTmpY - nOrgY) * alfa);
		}
	}


/*	void DoClusteringStep8o(std::vector<CvPoint> * a_pDataPts,
		std::vector<CvPoint> * a_pResDataPts,
		std::vector<int> * a_pDataPtCnts)
	{
		std::vector<CvPoint> & dataPts = *a_pDataPts;
		const int nDataSiz = dataPts.size();

		std::vector<CvPoint> & resDataPts = *a_pResDataPts;

		std::vector<int> distMat( Sqr(nDataSiz) );

		IndexCalc2D distIndex( nDataSiz, nDataSiz );

		int maxDist = 0;

		for(int i=0; i<nDataSiz; i++)
		{
			for(int j=i+1; j<nDataSiz; j++)
			{
				int dist = sqrt( (double) (Sqr( dataPts[i].x - dataPts[j].x ) +
				//int dist = ( (Sqr( dataPts[i].x - dataPts[j].x ) +
						Sqr( dataPts[i].y - dataPts[j].y )) );

				distMat[ distIndex.Calc(j, i) ] = dist;
					
				if(dist > maxDist)
					maxDist = dist;
			}
		}

		for(int i=0; i<nDataSiz; i++)
			distMat[ distIndex.Calc(i, i) ] = 0;

		for(int i=0; i<nDataSiz; i++)
		{
			for(int j=i+1; j<nDataSiz; j++)
			{
				distMat[ distIndex.Calc(i, j) ] = 
					distMat[ distIndex.Calc(j, i) ];
			}
		}

		const int nofNbrs = 10;
		const int nofIndirs = 30;
		const int nGuidSiz = 5;

		std::vector<int> nbrs( nDataSiz * nofNbrs );
		IndexCalc2D nbrIndex( nofNbrs, nDataSiz );

		int tmpNbrs[ nofNbrs + 1 ];
		for(int i=0; i<nDataSiz; i++)
		{
			int maxMinIndex = 0;

			int * locNbrs = 
				&nbrs[ nbrIndex.Calc(0, i) ];
			int * locDistMat =
				&distMat[ distIndex.Calc(0, i) ];

			

			for(int j=0; j < nofNbrs + 1; j++)
			{
				tmpNbrs[ j ] = j;

				if( locDistMat[ j ] > locDistMat[ maxMinIndex ] )
				{
					maxMinIndex = j;
				}
			}

			for(int j = nofNbrs + 1; j < nDataSiz; j++)
			{

				if( locDistMat[ j ] < 
					locDistMat[ tmpNbrs[ maxMinIndex ] ] )
				{
					tmpNbrs[ maxMinIndex ] = j;
					
					for(int k=0; k < nofNbrs + 1; k++)
					{

						if( locDistMat[ tmpNbrs[ k ] ] > 
							locDistMat[ tmpNbrs[ maxMinIndex ] ] )
						{
							maxMinIndex = k;
						}
					}

				}
			}

			for(int j=0; j < nofNbrs + 1; j++)
			{
				if(i == tmpNbrs[j])
				{
					tmpNbrs[j] = tmpNbrs[ nofNbrs ];
					break;
				}
			}

			for(int j=0; j < nofNbrs; j++)
			{
				int nMinIndex = j;

				for(int k= j + 1; k < nofNbrs; k++)
				{
					if(locDistMat[ tmpNbrs[k] ] <
						locDistMat[ tmpNbrs[nMinIndex] ])
						nMinIndex = k;
				}
				
				int nTmp = tmpNbrs[j];
				tmpNbrs[j] = tmpNbrs[ nMinIndex ];
				tmpNbrs[ nMinIndex ] = nTmp;
			}

			for(int j=0; j < nofNbrs; j++)
			{
				locNbrs[j] = tmpNbrs[j];
			}

		}


		std::vector<int> indirs( nDataSiz * nofIndirs );
		IndexCalc2D indirIndex( nofIndirs, nDataSiz );

		int nextDists[ nofIndirs ];
		int nextOrders[ nofIndirs ];

		for(int i=0; i<nDataSiz; i++)
		{
			int * locIndirs = 
				&indirs[ indirIndex.Calc(0, i) ];

			int nSelf;
			int nNbrOrder;

////////////////////////////////

			nSelf = i;
			nNbrOrder = 1;

			locIndirs[0] = nSelf;
			nextOrders[0] = nNbrOrder;
			nextDists[0] = distMat[ distIndex.Calc( 
				nbrs[ nbrIndex.Calc( nNbrOrder, nSelf) ], nSelf ) ];

			nSelf = nbrs[ nbrIndex.Calc( 0, i) ];
			nNbrOrder = 0;

			locIndirs[1] = nSelf;
			nextOrders[1] = nNbrOrder;
			nextDists[1] = distMat[ distIndex.Calc( 
				nbrs[ nbrIndex.Calc( nNbrOrder, nSelf) ], nSelf ) ];

			int nIndirCnt = 2;

////////////////////////////////

			int cycleTraces[nofIndirs];
			memset(cycleTraces, 0, nofIndirs * sizeof(int));

			int nCycleID = 1;
			//int nCycleSiz = (nIndirCnt / 2);
			int nCycleSiz = 1;
			int nCycleCnt = 0;

			while(nIndirCnt < nofIndirs)
			{
				int nMinDistIndex = 0;
				for(int j=0; j < nIndirCnt; j++)
				{
					if(nCycleID == cycleTraces[j])
						continue;

					if( nextDists[j] < nextDists[ nMinDistIndex ])
						nMinDistIndex = j;
				}

////////////////////

				if( nextDists[ nMinDistIndex ] == maxDist )
					break;

				nSelf = locIndirs[ nMinDistIndex ];

				const int nIndir = nbrs[ nbrIndex.Calc( 
					nextOrders[ nMinDistIndex ], nSelf ) ];

				nNbrOrder = ++nextOrders[ nMinDistIndex ];
				if(nNbrOrder >= nofNbrs)
				{
					nextDists[ nMinDistIndex ] = maxDist;
				}
				else
				{
					int nNewDist = distMat[ distIndex.Calc( 
						nbrs[ nbrIndex.Calc( nNbrOrder, nSelf) ], nSelf ) ];

					if(nNewDist > nextDists[ nMinDistIndex ])
						nextDists[ nMinDistIndex ] = nNewDist;
				}

				cycleTraces[ nMinDistIndex ] = nCycleID;
				nCycleCnt++;

				if( nCycleCnt >= nCycleSiz)
				{
					nCycleID++;
					nCycleSiz = (nIndirCnt < nGuidSiz) ?
						1 : nIndirCnt;
					nCycleCnt = 0;
				}

////////////////////

				for(int j=0; j < nIndirCnt; j++)
				{
					if( nIndir == locIndirs[ j ] )
						goto IndirDone;
				}

////////////////////////////////////////

				nSelf = nIndir;
				nNbrOrder = 0;

				locIndirs[nIndirCnt] = nSelf;
				nextOrders[nIndirCnt] = nNbrOrder;
				nextDists[nIndirCnt] = distMat[ distIndex.Calc( 
					nbrs[ nbrIndex.Calc( nNbrOrder, nSelf) ], nSelf ) ];

				nIndirCnt++;

////////////////////////////////////////

IndirDone:
				;
			}

			if(nIndirCnt < nofIndirs)
				locIndirs[ nIndirCnt ] = -1;
		}



		const int nMaxMove = 7;
		for(int i=0; i<nDataSiz; i++)
		{
			int sumX = 0;
			int sumY = 0;

			int * locIndirs = 
				&indirs[ indirIndex.Calc(0, i) ];

			//int nofActIndires = nofIndirs;
			int nofActIndires = 0;
			for(int j=0; j<nofIndirs; j++)				
			//for(int j= (nofIndirs * 1) / 3; j<nofIndirs; j++)
			//for(int j = nofIndirs - nIndirGrowth; j<nofIndirs; j++)
			{
				int k = locIndirs[j];

				//if(-1 == k)
				if(k < 0)
				{
					//nofActIndires = j;
					break;
				}

				sumX += dataPts[k].x;
				sumY += dataPts[k].y;

				nofActIndires++;
			}

			int nTmpX = (int) (sumX / nofActIndires);
			int nTmpY = (int) (sumY / nofActIndires);

			int nOrgX = dataPts[i].x;
			int nOrgY = dataPts[i].y;

			double diff = sqrt( (double)
				( Sqr( nOrgX - nTmpX ) + Sqr( nOrgY - nTmpY ) ) );

			//double alfa = 0.1;
			double alfa = nMaxMove / diff;
			if(alfa > 1)
				alfa = 1;
//				alfa = alfa;

				alfa = 1;

			resDataPts[i].x = (int)(nOrgX + (nTmpX - nOrgX) * alfa);
			resDataPts[i].y = (int)(nOrgY + (nTmpY - nOrgY) * alfa);
		}
	}

*/


	void DoClusteringStep8(std::vector<CvPoint> * a_pDataPts,
		std::vector<CvPoint> * a_pResDataPts,
		std::vector<int> * a_pDataPtCnts)
	{
		std::vector<CvPoint> & dataPts = *a_pDataPts;
		const int nDataSiz = dataPts.size();

		std::vector<CvPoint> & resDataPts = *a_pResDataPts;

		std::vector<int> distMat( Sqr(nDataSiz) );

		IndexCalc2D distIndex( nDataSiz, nDataSiz );

		int maxDist = 0;
		const int nHugeDist = 1000000;
		

		for(int i=0; i<nDataSiz; i++)
		{
			for(int j=i+1; j<nDataSiz; j++)
			{
				int dist = sqrt( (double) (Sqr( dataPts[i].x - dataPts[j].x ) +
				//int dist = ( (Sqr( dataPts[i].x - dataPts[j].x ) +
						Sqr( dataPts[i].y - dataPts[j].y )) );

				distMat[ distIndex.Calc(j, i) ] = dist;
					
				if(dist > maxDist)
					maxDist = dist;
			}
		}

		for(int i=0; i<nDataSiz; i++)
			distMat[ distIndex.Calc(i, i) ] = 0;

		for(int i=0; i<nDataSiz; i++)
		{
			for(int j=i+1; j<nDataSiz; j++)
			{
				distMat[ distIndex.Calc(i, j) ] = 
					distMat[ distIndex.Calc(j, i) ];
			}
		}

		const int nofNbrs = 5;
		//const int nofNbrs = 10;

		std::vector<int> nbrs( nDataSiz * nofNbrs );
		IndexCalc2D nbrIndex( nofNbrs, nDataSiz );

		int tmpNbrs[ nofNbrs + 1 ];
		for(int i=0; i<nDataSiz; i++)
		{
			int maxMinIndex = 0;

			int * locNbrs = 
				&nbrs[ nbrIndex.Calc(0, i) ];
			int * locDistMat =
				&distMat[ distIndex.Calc(0, i) ];

			

			for(int j=0; j < nofNbrs + 1; j++)
			{
				tmpNbrs[ j ] = j;

				if( locDistMat[ j ] > locDistMat[ maxMinIndex ] )
				{
					maxMinIndex = j;
				}
			}

			for(int j = nofNbrs + 1; j < nDataSiz; j++)
			{

				if( locDistMat[ j ] < 
					locDistMat[ tmpNbrs[ maxMinIndex ] ] )
				{
					tmpNbrs[ maxMinIndex ] = j;
					
					for(int k=0; k < nofNbrs + 1; k++)
					{

						if( locDistMat[ tmpNbrs[ k ] ] > 
							locDistMat[ tmpNbrs[ maxMinIndex ] ] )
						{
							maxMinIndex = k;
						}
					}

				}
			}

			for(int j=0; j < nofNbrs + 1; j++)
			{
				if(i == tmpNbrs[j])
				{
					tmpNbrs[j] = tmpNbrs[ nofNbrs ];
					break;
				}
			}

			for(int j=0; j < nofNbrs; j++)
			{
				int nMinIndex = j;

				for(int k= j + 1; k < nofNbrs; k++)
				{
					if(locDistMat[ tmpNbrs[k] ] <
						locDistMat[ tmpNbrs[nMinIndex] ])
						nMinIndex = k;
				}
				
				int nTmp = tmpNbrs[j];
				tmpNbrs[j] = tmpNbrs[ nMinIndex ];
				tmpNbrs[ nMinIndex ] = nTmp;
			}

			for(int j=0; j < nofNbrs; j++)
			{
				locNbrs[j] = tmpNbrs[j];
			}

		}


///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

		const int nofIndirs = 20;
		const int nofGuids = 5;

		std::vector<int> indirs( nDataSiz * nofIndirs );
		IndexCalc2D indirIndex( nofIndirs, nDataSiz );

		std::vector<int> indirCnts( nDataSiz );
		
		int nextDists[ nofIndirs ];
		int nextOrders[ nofIndirs ];

		for(int i=0; i<nDataSiz; i++)
		{
			indirCnts[i] = 0;

			memset(nextDists, 0, sizeof(int) * nofIndirs);
			memset(nextOrders, 0, sizeof(int) * nofIndirs);

			int * locIndirs = 
				&indirs[ indirIndex.Calc(0, i) ];

			int nSelf;
			int nNbrOrder;

////////////////////////////////

			nSelf = i;
			nNbrOrder = 1;

			locIndirs[0] = nSelf;
			nextOrders[0] = nNbrOrder;
			nextDists[0] = distMat[ distIndex.Calc( 
				nbrs[ nbrIndex.Calc( nNbrOrder, nSelf) ], nSelf ) ];

			nSelf = nbrs[ nbrIndex.Calc( 0, i) ];
			nNbrOrder = 0;

/*			locIndirs[1] = nSelf;
			nextOrders[1] = nNbrOrder;
			nextDists[1] = distMat[ distIndex.Calc( 
				nbrs[ nbrIndex.Calc( nNbrOrder, nSelf) ], nSelf ) ];*/

			int nIndirCnt = 1;
			indirCnts[i] = nIndirCnt;

////////////////////////////////

			int nChosenIndex = 0;

			int nCycleIndex = 0;
			int nCycleSiz = nofGuids;
			int nCycleID = 0;
			int nofContinues = 0;
			int nMinCycleDist = maxDist;

			double nMaxAllowedDist = 0;

			while(nIndirCnt < nofIndirs)
			{
				if(nIndirCnt < nofGuids)
				{
					int nMinDistIndex = 0;
					for(int j=0; j < nIndirCnt; j++)
					{
						if( nextDists[j] < nextDists[ nMinDistIndex ])
							nMinDistIndex = j;
					}

					if(nextDists[ nMinDistIndex ] > nMaxAllowedDist)
						nMaxAllowedDist = nextDists[ nMinDistIndex ];
					
					nChosenIndex = nMinDistIndex;

					if(nIndirCnt == nofGuids - 1)
					{
						nMaxAllowedDist *= 2;
						//nMaxAllowedDist = nHugeDist;
					}
				}
				else
				{
					if(1231 == i)
						i = i;

					nChosenIndex = nCycleIndex;

					if(nChosenIndex >= nCycleSiz)
					//if(nCycleIndex >= nCycleSiz)
					//if(nCycleIndex >= nIndirCnt)
					{
						if(nofContinues >= nCycleSiz)
						{
//							nMaxAllowedDist *= 1.5;
							nMaxAllowedDist ++;
						}

						nCycleIndex = 0;
						nChosenIndex = 0;
						nofContinues = 0;
						nCycleSiz = nIndirCnt;
						nCycleID++;
						nMinCycleDist = nHugeDist;
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

				if( nextDists[ nChosenIndex ] >= maxDist )
					break;

				nSelf = locIndirs[ nChosenIndex ];

				const int nIndir = nbrs[ nbrIndex.Calc( 
					nextOrders[ nChosenIndex ], nSelf ) ];

				nNbrOrder = ++nextOrders[ nChosenIndex ];
				if(nNbrOrder >= nofNbrs)
				{
					nextDists[ nChosenIndex ] = maxDist;
				}
				else
				{
					nextDists[ nChosenIndex ] = distMat[ distIndex.Calc( 
						nbrs[ nbrIndex.Calc( nNbrOrder, nSelf) ], nSelf ) ];
				}

////////////////////

				for(int j=0; j < nIndirCnt; j++)
				{
					if( nIndir == locIndirs[ j ] )
						goto IndirDone;
				}

////////////////////////////////////////

				nSelf = nIndir;
				nNbrOrder = 0;

				locIndirs[nIndirCnt] = nSelf;
				nextOrders[nIndirCnt] = nNbrOrder;
				nextDists[nIndirCnt] = distMat[ distIndex.Calc( 
					nbrs[ nbrIndex.Calc( nNbrOrder, nSelf) ], nSelf ) ];

				nIndirCnt++;
				indirCnts[i] = nIndirCnt;

////////////////////////////////////////

IndirDone:
				;
			}

			if(nIndirCnt < nofIndirs)
				locIndirs[ nIndirCnt ] = -1;
		}
		


//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

		for(int i=0; i<nDataSiz; i++)
		{
			int sumX = 0;
			int sumY = 0;

			int * locIndirs = 
				&indirs[ indirIndex.Calc(0, i) ];
			
			//int nofActIndires = nofIndirs;
			int nofActIndires = 0;

//			for(int j=0; j<nofIndirs; j++)
			for(int j=0; j<indirCnts[i]; j++)				
			//for(int j= (nofIndirs * 1) / 3; j<nofIndirs; j++)
			{
				int k = locIndirs[j];

				//if(-1 == k)
				if(k < 0)
				{
					//nofActIndires = j;
					break;
				}

				sumX += dataPts[k].x;
				sumY += dataPts[k].y;

				nofActIndires++;
			}

			int nTmpX = (int) (sumX / indirCnts[i]);
			int nTmpY = (int) (sumY / indirCnts[i]);

			int nOrgX = dataPts[i].x;
			int nOrgY = dataPts[i].y;

/*			if(indirCnts[i] < nofIndirs)
			{
				nTmpX = nOrgX;
				nTmpY = nOrgY;
			}*/

			int nAlfa = 1;

			resDataPts[i].x = nOrgX + (nTmpX - nOrgX) / nAlfa;
			resDataPts[i].y = nOrgY + (nTmpY - nOrgY) / nAlfa;
		}
	}




	void DoClusteringStep9(std::vector<CvPoint> * a_pDataPts,
		std::vector<CvPoint> * a_pResDataPts,
		std::vector<int> * a_pDataPtCnts)
	{
		std::vector<CvPoint> & dataPts = *a_pDataPts;
		const int nDataSiz = dataPts.size();

		std::vector<CvPoint> & resDataPts = *a_pResDataPts;

		std::vector<int> distMat( Sqr(nDataSiz) );

		IndexCalc2D distIndex( nDataSiz, nDataSiz );

		int maxDist = 0;

		const int nHugeDist = 1000000;
		//const int nNbrDist2 = Sqr( 20 );
		const int nNbrDist2 = nHugeDist;

		for(int i=0; i<nDataSiz; i++)
		{
			for(int j=i+1; j<nDataSiz; j++)
			{
				int nDifX2 = Sqr( dataPts[i].x - dataPts[j].x );
				if(nDifX2 > nNbrDist2)
				{
					distMat[ distIndex.Calc(j, i) ] = nHugeDist;
					continue;
				}

				int nDifY2 = Sqr( dataPts[i].y - dataPts[j].y );
				if(nDifY2 > nNbrDist2)
				{
					distMat[ distIndex.Calc(j, i) ] = nHugeDist;
					continue;
				}



				//int dist = sqrt( (double) (Sqr( dataPts[i].x - dataPts[j].x ) +
				int dist = ( (nDifX2 + nDifY2) );

				distMat[ distIndex.Calc(j, i) ] = dist;
					
				if(dist > maxDist)
					maxDist = dist;
			}
		}

		for(int i=0; i<nDataSiz; i++)
			distMat[ distIndex.Calc(i, i) ] = 0;

		for(int i=0; i<nDataSiz; i++)
		{
			for(int j=i+1; j<nDataSiz; j++)
			{
				distMat[ distIndex.Calc(i, j) ] = 
					distMat[ distIndex.Calc(j, i) ];
			}
		}

		//const int nofNbrs = 10;
		const int nofNbrs = 5;
		//const int nofIndirs = 150;
		const int nofIndirs = 20;

		std::vector<int> nbrs( nDataSiz * nofNbrs );
		IndexCalc2D nbrIndex( nofNbrs, nDataSiz );

		int tmpNbrs[ nofNbrs + 1 ];
		for(int i=0; i<nDataSiz; i++)
		{
			int maxMinIndex = 0;

			int * locNbrs = 
				&nbrs[ nbrIndex.Calc(0, i) ];
			int * locDistMat =
				&distMat[ distIndex.Calc(0, i) ];

			

			for(int j=0; j < nofNbrs + 1; j++)
			{
				tmpNbrs[ j ] = j;

				if( locDistMat[ j ] > locDistMat[ maxMinIndex ] )
				{
					maxMinIndex = j;
				}
			}

			for(int j = nofNbrs + 1; j < nDataSiz; j++)
			{
			//	if(nHugeDist == locDistMat[ j ])
			//		continue;

				if( locDistMat[ j ] < 
					locDistMat[ tmpNbrs[ maxMinIndex ] ] )
				{
					tmpNbrs[ maxMinIndex ] = j;
					
					for(int k=0; k < nofNbrs + 1; k++)
					{

						if( locDistMat[ tmpNbrs[ k ] ] > 
							locDistMat[ tmpNbrs[ maxMinIndex ] ] )
						{
							maxMinIndex = k;
						}
					}

				}
			}

			for(int j=0; j < nofNbrs + 1; j++)
			{
				if(i == tmpNbrs[j])
				{
					tmpNbrs[j] = tmpNbrs[ nofNbrs ];
					break;
				}
			}

			for(int j=0; j < nofNbrs; j++)
			{
				int nMinIndex = j;

				for(int k= j + 1; k < nofNbrs; k++)
				{
					if(locDistMat[ tmpNbrs[k] ] <
						locDistMat[ tmpNbrs[nMinIndex] ])
						nMinIndex = k;
				}
				
				int nTmp = tmpNbrs[j];
				tmpNbrs[j] = tmpNbrs[ nMinIndex ];
				tmpNbrs[ nMinIndex ] = nTmp;
			}

			for(int j=0; j < nofNbrs; j++)
			{
				locNbrs[j] = tmpNbrs[j];
			}

		}


		std::vector<int> indirs( nDataSiz * nofIndirs );
		IndexCalc2D indirIndex( nofIndirs, nDataSiz );

		for(int i=0; i<nDataSiz; i++)
		{
			int * locNbrs = 
				&nbrs[ nbrIndex.Calc(0, i) ];
			int * locIndirs = 
				&indirs[ indirIndex.Calc(0, i) ];

			int nIndirCnt = 0;
			for(int j=0; j < nofNbrs; j++)
				locIndirs[ j ] = locNbrs[ j ];

			nIndirCnt = nofNbrs;


///
/*			
			locIndirs[0] = i;
			locIndirs[0] = locNbrs[ 0 ];

			nIndirCnt = 1;
			*/
///


			//for(int j=1; j < nIndirCnt; j++)
			for(int j=0; j < nIndirCnt; j++)
			{
				int * locNbrs2 = 
					&nbrs[ nbrIndex.Calc(0, locIndirs[ j ]) ];

				for(int k=0; k<nofNbrs; k++)
				//for(int k=0; k< (nofNbrs * 1) / 10; k++)
				{
					int nTmp = locNbrs2[ k ];

					for(int m=0; m < nIndirCnt; m++)
					{
						if( nTmp == locIndirs[ m ])
							goto CheckDone;
					}

					locIndirs[ nIndirCnt++ ] = nTmp;

					if(nIndirCnt >= nofIndirs)
						goto IndirComplete;

					locIndirs[ nIndirCnt ] = -1;
CheckDone:
					;
				}

			}
IndirComplete:
			;
		}



		for(int i=0; i<nDataSiz; i++)
		{
			int sumX = 0;
			int sumY = 0;

			int * locIndirs = 
				&indirs[ indirIndex.Calc(0, i) ];

			int nofActIndires = nofIndirs;
			for(int j=0; j<nofIndirs; j++)
			{
				int k = locIndirs[j];

				//if(-1 == k)
				if(k < 0)
				{
					nofActIndires = j;
					break;
				}

				sumX += dataPts[k].x;
				sumY += dataPts[k].y;
			}

			resDataPts[i].x = (int) (sumX / nofActIndires);
			resDataPts[i].y = (int) (sumY / nofActIndires);
		}
	}




	void DoClusteringStep10(std::vector<CvPoint> * a_pDataPts,
		std::vector<CvPoint> * a_pResDataPts, 
		S16ImageRef a_orgImg, S16ImageRef a_resImg,
		std::vector<int> * a_pDataPtCnts)
	{
		std::vector<CvPoint> & dataPts = *a_pDataPts;
		const int nDataSiz = dataPts.size();

		std::vector<CvPoint> & resDataPts = *a_pResDataPts;

		//int * pi = new int[400000000];
		//delete [] pi;

		std::vector<int> distMat( Sqr(nDataSiz) );

		IndexCalc2D distIndex( nDataSiz, nDataSiz );


		int maxDist = 0;
		const int nHugeDist = 1000000;
		const int nNbrDist = 30;
		
		int nMinX = nHugeDist;
		int nMaxX = 0;

		int nMinY = nHugeDist;
		int nMaxY = 0;

/*		for(int i=0; i<nDataSiz; i++)
		{
			int x = dataPts[i].x;

			if(x < nMinX)
				nMinX = x;
			if(x > nMaxX)
				nMaxX = x;

			int y = dataPts[i].y;

			if(y < nMinY)
				nMinY = y;
			if(y > nMaxY)
				nMaxY = y;
		}*/


		for(int i=0; i<nDataSiz; i++)
		{
			for(int j=i+1; j<nDataSiz; j++)
			{
				int nAbsDifX =  abs( dataPts[i].x - dataPts[j].x );
				if(nAbsDifX > nNbrDist)
				{
					distMat[ distIndex.Calc(j, i) ] = nHugeDist;
					distMat[ distIndex.Calc(i, j) ] = nHugeDist;
					continue;
				}

				int nAbsDifY =  abs( dataPts[i].y - dataPts[j].y );
				if(nAbsDifY > nNbrDist)
				{
					distMat[ distIndex.Calc(j, i) ] = nHugeDist;
					distMat[ distIndex.Calc(i, j) ] = nHugeDist;
					continue;
				}


				int dist = sqrt( (double) (Sqr( nAbsDifX ) +
						Sqr( nAbsDifY )) );

				distMat[ distIndex.Calc(j, i) ] = dist;
				distMat[ distIndex.Calc(i, j) ] = dist;
					
				if(dist > maxDist)
					maxDist = dist;
			}
		}

		for(int i=0; i<nDataSiz; i++)
			distMat[ distIndex.Calc(i, i) ] = 0;

/*		for(int i=0; i<nDataSiz; i++)
		{
			for(int j=i+1; j<nDataSiz; j++)
			{
				distMat[ distIndex.Calc(i, j) ] = 
					distMat[ distIndex.Calc(j, i) ];
			}
		}*/

		const int nofNbrs = 5;
		//const int nofNbrs = 10;

		std::vector<int> nbrs( nDataSiz * nofNbrs );
		IndexCalc2D nbrIndex( nofNbrs, nDataSiz );

		int tmpNbrs[ nofNbrs + 1 ];
		for(int i=0; i<nDataSiz; i++)
		{
			int maxMinIndex = 0;

			int * locNbrs = 
				&nbrs[ nbrIndex.Calc(0, i) ];
			int * locDistMat =
				&distMat[ distIndex.Calc(0, i) ];

			

			for(int j=0; j < nofNbrs + 1; j++)
			{
				tmpNbrs[ j ] = j;

				if( locDistMat[ j ] > locDistMat[ maxMinIndex ] )
				{
					maxMinIndex = j;
				}
			}

			for(int j = nofNbrs + 1; j < nDataSiz; j++)
			{

				if( locDistMat[ j ] < 
					locDistMat[ tmpNbrs[ maxMinIndex ] ] )
				{
					tmpNbrs[ maxMinIndex ] = j;
					
					for(int k=0; k < nofNbrs + 1; k++)
					{

						if( locDistMat[ tmpNbrs[ k ] ] > 
							locDistMat[ tmpNbrs[ maxMinIndex ] ] )
						{
							maxMinIndex = k;
						}
					}

				}
			}

			for(int j=0; j < nofNbrs + 1; j++)
			{
				if(i == tmpNbrs[j])
				{
					tmpNbrs[j] = tmpNbrs[ nofNbrs ];
					break;
				}
			}

			for(int j=0; j < nofNbrs; j++)
			{
				int nMinIndex = j;

				for(int k= j + 1; k < nofNbrs; k++)
				{
					if(locDistMat[ tmpNbrs[k] ] <
						locDistMat[ tmpNbrs[nMinIndex] ])
						nMinIndex = k;
				}
				
				int nTmp = tmpNbrs[j];
				tmpNbrs[j] = tmpNbrs[ nMinIndex ];
				tmpNbrs[ nMinIndex ] = nTmp;
			}

			for(int j=0; j < nofNbrs; j++)
			{
				locNbrs[j] = tmpNbrs[j];
			}

		}


///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

		//const int nofIndirs = 20;
		const int nofIndirs = 40;
		const int nofGuids = 5;

		std::vector<int> indirs( nDataSiz * nofIndirs );
		IndexCalc2D indirIndex( nofIndirs, nDataSiz );

		std::vector<int> indirCnts( nDataSiz );
		
		int nextDists[ nofIndirs ];
		int nextOrders[ nofIndirs ];

		for(int i=0; i<nDataSiz; i++)
		{
			indirCnts[i] = 0;

			memset(nextDists, 0, sizeof(int) * nofIndirs);
			memset(nextOrders, 0, sizeof(int) * nofIndirs);

			int * locIndirs = 
				&indirs[ indirIndex.Calc(0, i) ];

			int nSelf;
			int nNbrOrder;

////////////////////////////////

			nSelf = i;
			nNbrOrder = 1;

			locIndirs[0] = nSelf;
			nextOrders[0] = nNbrOrder;
			nextDists[0] = distMat[ distIndex.Calc( 
				nbrs[ nbrIndex.Calc( nNbrOrder, nSelf) ], nSelf ) ];

			nSelf = nbrs[ nbrIndex.Calc( 0, i) ];
			nNbrOrder = 0;

			int nIndirCnt = 1;
			indirCnts[i] = nIndirCnt;

////////////////////////////////

			int nChosenIndex = 0;

			int nCycleIndex = 0;
			int nCycleSiz = nofGuids;
			int nCycleID = 0;
			int nofContinues = 0;
			int nBgn = 0;
			int nMinCycleDist = nHugeDist;

			double nMaxAllowedDist = 0;

			while(nIndirCnt < nofIndirs)
			{
				if(nIndirCnt < nofGuids)
				{
					int nMinDistIndex = 0;
					for(int j=0; j < nIndirCnt; j++)
					{
						if( nextDists[j] < nextDists[ nMinDistIndex ])
							nMinDistIndex = j;
					}

					if(nextDists[ nMinDistIndex ] > nMaxAllowedDist)
						nMaxAllowedDist = nextDists[ nMinDistIndex ];
					
					nChosenIndex = nMinDistIndex;

					if(nIndirCnt == nofGuids - 1)
					{
						nMaxAllowedDist *= 2;
						//nMaxAllowedDist *= 1.5;
						//nMaxAllowedDist = nHugeDist;
					}
				}
				else
				{
					if(1231 == i)
						i = i;

					nChosenIndex = nCycleIndex;

					if(nChosenIndex >= nCycleSiz)
					//if(nCycleIndex >= nCycleSiz)
					//if(nCycleIndex >= nIndirCnt)
					{
						if(nofContinues >= nCycleSiz - nBgn)
						{
//							nMaxAllowedDist *= 1.5;
							nMaxAllowedDist ++;
						}

						nCycleIndex = nBgn;
						nChosenIndex = nBgn;
						nofContinues = 0;
						nCycleSiz = nIndirCnt;
						nCycleID++;
						nMinCycleDist = nHugeDist;
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

				if( nextDists[ nChosenIndex ] >= nHugeDist )
					break;

				nSelf = locIndirs[ nChosenIndex ];

				const int nIndir = nbrs[ nbrIndex.Calc( 
					nextOrders[ nChosenIndex ], nSelf ) ];

				nNbrOrder = ++nextOrders[ nChosenIndex ];
				if(nNbrOrder >= nofNbrs)
				{
					nextDists[ nChosenIndex ] = nHugeDist;

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

					if( nBgn >= nIndirCnt )
					{
						break;
					}
				}
				else
				{
					nextDists[ nChosenIndex ] = distMat[ distIndex.Calc( 
						nbrs[ nbrIndex.Calc( nNbrOrder, nSelf) ], nSelf ) ];
				}

////////////////////

				for(int j=0; j < nIndirCnt; j++)
				{
					if( nIndir == locIndirs[ j ] )
						goto IndirDone;
				}

////////////////////////////////////////

				nSelf = nIndir;
				nNbrOrder = 0;

				locIndirs[nIndirCnt] = nSelf;
				nextOrders[nIndirCnt] = nNbrOrder;
				nextDists[nIndirCnt] = distMat[ distIndex.Calc( 
					nbrs[ nbrIndex.Calc( nNbrOrder, nSelf) ], nSelf ) ];

				nIndirCnt++;
				indirCnts[i] = nIndirCnt;

////////////////////////////////////////

IndirDone:
				;
			}

			if(1231 == i)
				i = i;

			if(nIndirCnt < nofIndirs)
				locIndirs[ nIndirCnt ] = -1;
		}
		


//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////


		for(int i=0; i<nDataSiz; i++)
		{
			int sumX = 0;
			int sumY = 0;

			int * locIndirs = 
				&indirs[ indirIndex.Calc(0, i) ];
			
			//int nofActIndires = nofIndirs;
			int nofActIndires = 0;

//			for(int j=0; j<nofIndirs; j++)
			for(int j=0; j<indirCnts[i]; j++)				
			//for(int j= (nofIndirs * 1) / 3; j<nofIndirs; j++)
			{
				int k = locIndirs[j];

				//if(-1 == k)
				if(k < 0)
				{
					//nofActIndires = j;
					break;
				}

				sumX += dataPts[k].x;
				sumY += dataPts[k].y;

				nofActIndires++;
			}

			int nTmpX = (int) (sumX / indirCnts[i]);
			int nTmpY = (int) (sumY / indirCnts[i]);

			int nOrgX = dataPts[i].x;
			int nOrgY = dataPts[i].y;

/*			if(indirCnts[i] < nofIndirs)
			{
				nTmpX = nOrgX;
				nTmpY = nOrgY;
			}*/

			int nAlfa = 1;

			resDataPts[i].x = nOrgX + (nTmpX - nOrgX) / nAlfa;
			resDataPts[i].y = nOrgY + (nTmpY - nOrgY) / nAlfa;
		}



		CvScalar pntColor = CV_RGB(255, 0, 255);
		CvScalar indirColor = CV_RGB(0, 255, 0);
		CvScalar spColor = CV_RGB(255, 130, 0);


		int nShowPoint = 55;

		//const int spX = 349;
//		const int spX = 352;
//		const int spY = 186;

		const int spX = 303;
		const int spY = 93;

		for(int i=0; i<nDataSiz; i++)
		{
			//if( spX == dataPts[i].x && spY == dataPts[i].y )
			if( spX == resDataPts[i].x && spY == resDataPts[i].y )
			{
				nShowPoint = i;
				break;
			}
		}


		const int nofSPIndirs = indirCnts[ nShowPoint ];

		int * spLocIndirs = &indirs[ indirIndex.Calc(0, nShowPoint) ];

		std::vector<CvPoint> spIndirPts( indirCnts[ nofSPIndirs ] );

		DrawPoints(dataPts, a_orgImg, pntColor);

		for(int i=0; i < nofSPIndirs; i++)
			spIndirPts[i] = dataPts[ spLocIndirs[ i ] ]; 

		DrawPoints(spIndirPts, a_orgImg, indirColor);

		spIndirPts.resize(0);
		spIndirPts.push_back( dataPts[ nShowPoint ] );

		DrawPoints(spIndirPts, a_orgImg, spColor);

		///////////

		DrawPoints(resDataPts, a_resImg, pntColor);

		spIndirPts.resize( nofSPIndirs );

		for(int i=0; i < nofSPIndirs; i++)
			spIndirPts[i] = resDataPts[ spLocIndirs[ i ] ]; 

		DrawPoints(spIndirPts, a_resImg, indirColor);

		spIndirPts.resize(0);
		spIndirPts.push_back( resDataPts[ nShowPoint ] );

		DrawPoints(spIndirPts, a_resImg, spColor);

	}






	void DoClusteringStep11(std::vector<CvPoint> * a_pDataPts,
		std::vector<CvPoint> * a_pResDataPts, 
		S16ImageRef a_orgImg, S16ImageRef a_resImg,
		std::vector<int> * a_pDataPtCnts)
	{
		std::vector<CvPoint> & dataPts = *a_pDataPts;
		const int nDataSiz = dataPts.size();

		std::vector<CvPoint> & resDataPts = *a_pResDataPts;

		//std::vector<dist_t> distMat( Sqr(nDataSiz) );
		std::vector<int> distMat( Sqr(nDataSiz) );

		IndexCalc2D distIndex( nDataSiz, nDataSiz );


		dist_t maxDist = 0;
		const dist_t nHugeDist = 1000000;
		const dist_t nNbrDist = 30;
		
		int nMinX = nHugeDist;
		int nMaxX = 0;

		int nMinY = nHugeDist;
		int nMaxY = 0;

/*		for(int i=0; i<nDataSiz; i++)
		{
			int x = dataPts[i].x;

			if(x < nMinX)
				nMinX = x;
			if(x > nMaxX)
				nMaxX = x;

			int y = dataPts[i].y;

			if(y < nMinY)
				nMinY = y;
			if(y > nMaxY)
				nMaxY = y;
		}*/


		for(int i=0; i<nDataSiz; i++)
		{
			for(int j=i+1; j<nDataSiz; j++)
			{
				int nAbsDifX =  abs( dataPts[i].x - dataPts[j].x );
				if(nAbsDifX > nNbrDist)
				{
					distMat[ distIndex.Calc(j, i) ] = nHugeDist;
					distMat[ distIndex.Calc(i, j) ] = nHugeDist;
					continue;
				}

				int nAbsDifY =  abs( dataPts[i].y - dataPts[j].y );
				if(nAbsDifY > nNbrDist)
				{
					distMat[ distIndex.Calc(j, i) ] = nHugeDist;
					distMat[ distIndex.Calc(i, j) ] = nHugeDist;
					continue;
				}


				dist_t dist = sqrt( (double) (Sqr( nAbsDifX ) +
						Sqr( nAbsDifY )) );

				distMat[ distIndex.Calc(j, i) ] = dist;
				distMat[ distIndex.Calc(i, j) ] = dist;
					
				if(dist > maxDist)
					maxDist = dist;
			}
		}

		for(int i=0; i<nDataSiz; i++)
			distMat[ distIndex.Calc(i, i) ] = 0;

/*		for(int i=0; i<nDataSiz; i++)
		{
			for(int j=i+1; j<nDataSiz; j++)
			{
				distMat[ distIndex.Calc(i, j) ] = 
					distMat[ distIndex.Calc(j, i) ];
			}
		}*/

		const int nofNbrs = 5;
		//const int nofNbrs = 10;

		std::vector<int> nbrs( nDataSiz * nofNbrs );
		IndexCalc2D nbrIndex( nofNbrs, nDataSiz );

		int tmpNbrs[ nofNbrs + 1 ];
		for(int i=0; i<nDataSiz; i++)
		{
			int maxMinIndex = 0;

			int * locNbrs = 
				&nbrs[ nbrIndex.Calc(0, i) ];
			dist_t * locDistMat =
				&distMat[ distIndex.Calc(0, i) ];

			

			for(int j=0; j < nofNbrs + 1; j++)
			{
				tmpNbrs[ j ] = j;

				if( locDistMat[ j ] > locDistMat[ maxMinIndex ] )
				{
					maxMinIndex = j;
				}
			}

			for(int j = nofNbrs + 1; j < nDataSiz; j++)
			{

				if( locDistMat[ j ] < 
					locDistMat[ tmpNbrs[ maxMinIndex ] ] )
				{
					tmpNbrs[ maxMinIndex ] = j;
					
					for(int k=0; k < nofNbrs + 1; k++)
					{

						if( locDistMat[ tmpNbrs[ k ] ] > 
							locDistMat[ tmpNbrs[ maxMinIndex ] ] )
						{
							maxMinIndex = k;
						}
					}

				}
			}

			for(int j=0; j < nofNbrs + 1; j++)
			{
				if(i == tmpNbrs[j])
				{
					tmpNbrs[j] = tmpNbrs[ nofNbrs ];
					break;
				}
			}

			for(int j=0; j < nofNbrs; j++)
			{
				int nMinIndex = j;

				for(int k= j + 1; k < nofNbrs; k++)
				{
					if(locDistMat[ tmpNbrs[k] ] <
						locDistMat[ tmpNbrs[nMinIndex] ])
						nMinIndex = k;
				}
				
				int nTmp = tmpNbrs[j];
				tmpNbrs[j] = tmpNbrs[ nMinIndex ];
				tmpNbrs[ nMinIndex ] = nTmp;
			}

			for(int j=0; j < nofNbrs; j++)
			{
				locNbrs[j] = tmpNbrs[j];
			}

		}


///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

		//const int nofIndirs = 20;
		const int nofIndirs = 40;
		const int nofGuids = 5;

		std::vector<int> indirs( nDataSiz * nofIndirs );
		IndexCalc2D indirIndex( nofIndirs, nDataSiz );

		std::vector<int> indirCnts( nDataSiz );
		
		dist_t nextDists[ nofIndirs ];
		int nextOrders[ nofIndirs ];

		for(int i=0; i<nDataSiz; i++)
		{
			indirCnts[i] = 0;

			//memset(nextDists, 0, sizeof(dist_t) * nofIndirs);
			//memset(nextOrders, 0, sizeof(dist_t) * nofIndirs);

			int * locIndirs = 
				&indirs[ indirIndex.Calc(0, i) ];

			int nSelf;
			int nNbrOrder;

////////////////////////////////

			nSelf = i;
			nNbrOrder = 1;

			locIndirs[0] = nSelf;
			nextOrders[0] = nNbrOrder;
			nextDists[0] = distMat[ distIndex.Calc( 
				nbrs[ nbrIndex.Calc( nNbrOrder, nSelf) ], nSelf ) ];

			nSelf = nbrs[ nbrIndex.Calc( 0, i) ];
			nNbrOrder = 0;

			int nIndirCnt = 1;
			indirCnts[i] = nIndirCnt;

////////////////////////////////

			int nChosenIndex = 0;

			int nCycleIndex = 0;
			int nCycleSiz = nofGuids;
			int nCycleID = 0;
			int nofContinues = 0;
			int nBgn = 0;
			dist_t nMinCycleDist = nHugeDist;

			dist_t nMaxAllowedDist = 0;

			while(nIndirCnt < nofIndirs)
			{
				if(nIndirCnt < nofGuids)
				{
					int nMinDistIndex = 0;
					for(int j=0; j < nIndirCnt; j++)
					{
						if( nextDists[j] < nextDists[ nMinDistIndex ])
							nMinDistIndex = j;
					}

					if(nextDists[ nMinDistIndex ] > nMaxAllowedDist)
						nMaxAllowedDist = nextDists[ nMinDistIndex ];
					
					nChosenIndex = nMinDistIndex;

					if(nIndirCnt == nofGuids - 1)
					{
						nMaxAllowedDist *= 2;
						//nMaxAllowedDist *= 1.5;
						//nMaxAllowedDist = nHugeDist;
					}
				}
				else
				{
					if(1231 == i)
						i = i;

					nChosenIndex = nCycleIndex;

					if(nChosenIndex >= nCycleSiz)
					//if(nCycleIndex >= nCycleSiz)
					//if(nCycleIndex >= nIndirCnt)
					{
						if(nofContinues >= nCycleSiz - nBgn)
						{
//							nMaxAllowedDist *= 1.5;
							nMaxAllowedDist ++;
						}

						nCycleIndex = nBgn;
						nChosenIndex = nBgn;
						nofContinues = 0;
						nCycleSiz = nIndirCnt;
						nCycleID++;
						nMinCycleDist = nHugeDist;
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

				if( nextDists[ nChosenIndex ] >= nHugeDist )
					break;

				nSelf = locIndirs[ nChosenIndex ];

				const int nIndir = nbrs[ nbrIndex.Calc( 
					nextOrders[ nChosenIndex ], nSelf ) ];

				nNbrOrder = ++nextOrders[ nChosenIndex ];
				if(nNbrOrder >= nofNbrs)
				{
					nextDists[ nChosenIndex ] = nHugeDist;

					// Swapping with bgn

					dist_t nTmp;

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

					if( nBgn >= nIndirCnt )
					{
						break;
					}
				}
				else
				{
					nextDists[ nChosenIndex ] = distMat[ distIndex.Calc( 
						nbrs[ nbrIndex.Calc( nNbrOrder, nSelf) ], nSelf ) ];
				}

////////////////////

				for(int j=0; j < nIndirCnt; j++)
				{
					if( nIndir == locIndirs[ j ] )
						goto IndirDone;
				}

////////////////////////////////////////

				nSelf = nIndir;
				nNbrOrder = 0;

				locIndirs[nIndirCnt] = nSelf;
				nextOrders[nIndirCnt] = nNbrOrder;
				nextDists[nIndirCnt] = distMat[ distIndex.Calc( 
					nbrs[ nbrIndex.Calc( nNbrOrder, nSelf) ], nSelf ) ];

				nIndirCnt++;
				indirCnts[i] = nIndirCnt;

////////////////////////////////////////

IndirDone:
				;
			}

			if(1231 == i)
				i = i;

			if(nIndirCnt < nofIndirs)
				locIndirs[ nIndirCnt ] = -1;
		}
		


//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////


		for(int i=0; i<nDataSiz; i++)
		{
			int sumX = 0;
			int sumY = 0;

			int * locIndirs = 
				&indirs[ indirIndex.Calc(0, i) ];
			
			//int nofActIndires = nofIndirs;
			int nofActIndires = 0;

//			for(int j=0; j<nofIndirs; j++)
			for(int j=0; j<indirCnts[i]; j++)				
			//for(int j= (nofIndirs * 1) / 3; j<nofIndirs; j++)
			{
				int k = locIndirs[j];

				//if(-1 == k)
				if(k < 0)
				{
					//nofActIndires = j;
					break;
				}

				sumX += dataPts[k].x;
				sumY += dataPts[k].y;

				nofActIndires++;
			}

			int nTmpX = (int) (sumX / indirCnts[i]);
			int nTmpY = (int) (sumY / indirCnts[i]);

			int nOrgX = dataPts[i].x;
			int nOrgY = dataPts[i].y;

/*			if(indirCnts[i] < nofIndirs)
			{
				nTmpX = nOrgX;
				nTmpY = nOrgY;
			}*/

			int nAlfa = 1;

			resDataPts[i].x = nOrgX + (nTmpX - nOrgX) / nAlfa;
			resDataPts[i].y = nOrgY + (nTmpY - nOrgY) / nAlfa;
		}



		CvScalar pntColor = CV_RGB(255, 0, 255);
		CvScalar indirColor = CV_RGB(0, 255, 0);
		CvScalar spColor = CV_RGB(255, 130, 0);


		int nShowPoint = 55;

		//const int spX = 349;
//		const int spX = 352;
//		const int spY = 186;

		const int spX = 303;
		const int spY = 93;

		for(int i=0; i<nDataSiz; i++)
		{
			//if( spX == dataPts[i].x && spY == dataPts[i].y )
			if( spX == resDataPts[i].x && spY == resDataPts[i].y )
			{
				nShowPoint = i;
				break;
			}
		}


		const int nofSPIndirs = indirCnts[ nShowPoint ];

		int * spLocIndirs = &indirs[ indirIndex.Calc(0, nShowPoint) ];

		std::vector<CvPoint> spIndirPts( indirCnts[ nofSPIndirs ] );

		DrawPoints(dataPts, a_orgImg, pntColor);

		for(int i=0; i < nofSPIndirs; i++)
			spIndirPts[i] = dataPts[ spLocIndirs[ i ] ]; 

		DrawPoints(spIndirPts, a_orgImg, indirColor);

		spIndirPts.resize(0);
		spIndirPts.push_back( dataPts[ nShowPoint ] );

		DrawPoints(spIndirPts, a_orgImg, spColor);

		///////////

		DrawPoints(resDataPts, a_resImg, pntColor);

		spIndirPts.resize( nofSPIndirs );

		for(int i=0; i < nofSPIndirs; i++)
			spIndirPts[i] = resDataPts[ spLocIndirs[ i ] ]; 

		DrawPoints(spIndirPts, a_resImg, indirColor);

		spIndirPts.resize(0);
		spIndirPts.push_back( resDataPts[ nShowPoint ] );

		DrawPoints(spIndirPts, a_resImg, spColor);

	}



	void DoClusteringStep12(std::vector<CvPoint> * a_pDataPts,
		std::vector<CvPoint> * a_pResDataPts, 
		S16ImageRef a_orgImg, S16ImageRef a_resImg,
		std::vector<int> * a_pDataPtCnts)
	{
		std::vector<CvPoint> & dataPts = *a_pDataPts;
		const int nDataSiz = dataPts.size();

		std::vector<CvPoint> & resDataPts = *a_pResDataPts;


		const int nDistStep = 100;
		const int nHugeDist = 0x7f7f7f7f;
		const int nNbrDist = 30 * nDistStep;
		
/*		int nMinX = nHugeDist;
		int nMaxX = 0;

		int nMinY = nHugeDist;
		int nMaxY = 0;

		for(int i=0; i<nDataSiz; i++)
		{
			int x = dataPts[i].x;

			if(x < nMinX)
				nMinX = x;
			if(x > nMaxX)
				nMaxX = x;

			int y = dataPts[i].y;

			if(y < nMinY)
				nMinY = y;
			if(y > nMaxY)
				nMaxY = y;
		}*/



		const int nofNbrs = 5;
		//const int nofNbrs = 10;

		std::vector<int> nbrDists( nDataSiz * nofNbrs );
		memset( &nbrDists[0], 0x7f, nbrDists.size() * sizeof(int));

		IndexCalc2D nbrDistIndex( nofNbrs, nDataSiz );

		std::vector<int> nbrs( nDataSiz * nofNbrs );
		IndexCalc2D nbrIndex( nofNbrs, nDataSiz );

		std::vector<int> maxMinNbrIndices( nDataSiz );
		memset( &maxMinNbrIndices[0], 0, maxMinNbrIndices.size() * sizeof(int));

		

		for(int i=0; i<nDataSiz; i++)
		{
			int & nMaxMinIndexI = maxMinNbrIndices[ i ];
			int * locNbrsI = 
				&nbrs[ nbrIndex.Calc(0, i) ];
			int * locNbrDistsI = 
				&nbrDists[ nbrIndex.Calc(0, i) ];

			for(int j=i+1; j<nDataSiz; j++)
			{
				int nAbsDifX =  abs( dataPts[i].x - dataPts[j].x ) * nDistStep;
				if(nAbsDifX > nNbrDist)
					continue;

				int nAbsDifY =  abs( dataPts[i].y - dataPts[j].y ) * nDistStep;
				if(nAbsDifY > nNbrDist)
					continue;


				int dist = sqrt( (double) (Sqr( nAbsDifX ) +
						Sqr( nAbsDifY )) );
				if(dist > nNbrDist)
					continue;

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
					&nbrs[ nbrIndex.Calc(0, j) ];
				int * locNbrDistsJ = 
					&nbrDists[ nbrIndex.Calc(0, j) ];

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

			}
		}


		for(int i=0; i<nDataSiz; i++)
		{
			int & nMaxMinIndexI = maxMinNbrIndices[ i ];
			int * locNbrsI = 
				&nbrs[ nbrIndex.Calc(0, i) ];
			int * locNbrDistsI = 
				&nbrDists[ nbrIndex.Calc(0, i) ];



			for(int j=0; j < nofNbrs; j++)
			{
				//if(nHugeDist == locNbrDistsI[ j ])
				if(nHugeDist == locNbrDistsI[ j ])
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

		const int nofIndirs = 20;
		//const int nofIndirs = 40;
		const int nofGuids = 5;

		std::vector<int> indirs( nDataSiz * nofIndirs );
		IndexCalc2D indirIndex( nofIndirs, nDataSiz );

		std::vector<int> indirCnts( nDataSiz );
		
		int nextDists[ nofIndirs ];
		int nextOrders[ nofIndirs ];

		for(int i=0; i<nDataSiz; i++)
		{
			indirCnts[i] = 0;
			int nIndirCnt = 0;

			//memset(nextDists, 0, sizeof(int) * nofIndirs);
			//memset(nextOrders, 0, sizeof(int) * nofIndirs);

			int * locIndirs = 
				&indirs[ indirIndex.Calc(0, i) ];

			int nSelf;
			int nNbrOrder;

////////////////////////////////

			nSelf = i;
			nNbrOrder = 0;

			locIndirs[nIndirCnt] = nSelf;
			nextOrders[nIndirCnt] = nNbrOrder;
			nextDists[nIndirCnt] = nbrDists[ nbrDistIndex.Calc( 
				nNbrOrder, nSelf) ];

			nIndirCnt++;

			indirCnts[i] = nIndirCnt;

////////////////////////////////

			int nChosenIndex = 0;

			int nCycleIndex = 0;
			int nCycleSiz = nofGuids;
			int nCycleID = 0;
			int nofContinues = 0;
			int nBgn = 0;
			int nMinCycleDist = nHugeDist;

			double nMaxAllowedDist = 0;

			while(nIndirCnt < nofIndirs)
			{
				if(nIndirCnt < nofGuids)
				{
					int nMinDistIndex = 0;
					for(int j=0; j < nIndirCnt; j++)
					{
						if( nextDists[j] < nextDists[ nMinDistIndex ])
							nMinDistIndex = j;
					}

					if(nextDists[ nMinDistIndex ] > nMaxAllowedDist)
						nMaxAllowedDist = nextDists[ nMinDistIndex ];
					
					nChosenIndex = nMinDistIndex;

					if(nIndirCnt == nofGuids - 1)
					{
						nMaxAllowedDist *= 2;
						//nMaxAllowedDist *= 1.5;
						//nMaxAllowedDist = nHugeDist;
					}
				}
				else
				{
					if(1231 == i)
						i = i;

					nChosenIndex = nCycleIndex;

					if(nChosenIndex >= nCycleSiz)
					//if(nCycleIndex >= nCycleSiz)
					//if(nCycleIndex >= nIndirCnt)
					{
						if(nofContinues >= nCycleSiz - nBgn)
						{
//							nMaxAllowedDist *= 1.5;
							nMaxAllowedDist += nDistStep;

							if( nMaxAllowedDist > nNbrDist )
								break;
							//i = i;
						}

						nCycleIndex = nBgn;
						nChosenIndex = nBgn;
						nofContinues = 0;
						nCycleSiz = nIndirCnt;
						nCycleID++;
						nMinCycleDist = nHugeDist;
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

				if(99 == i && nIndirCnt > 35)
					i = i;

				//if( nextDists[ nChosenIndex ] >= nHugeDist )
				//if( nextDists[ nChosenIndex ] >= nNbrDist )
				//	break;

				nSelf = locIndirs[ nChosenIndex ];

				const int nIndir = nbrs[ nbrIndex.Calc( 
					nextOrders[ nChosenIndex ], nSelf ) ];

				nNbrOrder = ++nextOrders[ nChosenIndex ];
				if(nNbrOrder >= nofNbrs)
				{
					nextDists[ nChosenIndex ] = nHugeDist;

					// Swapping with bgn

/*					int nTmp;

					nTmp = locIndirs[ nBgn ];
					locIndirs[ nBgn ] = locIndirs[ nChosenIndex ];
					locIndirs[ nChosenIndex ] = nTmp;

					nTmp = nextDists[ nBgn ];
					nextDists[ nBgn ] = nextDists[ nChosenIndex ];
					nextDists[ nChosenIndex ] = nTmp;

					nTmp = nextOrders[ nBgn ];
					nextOrders[ nBgn ] = nextOrders[ nChosenIndex ];
					nextOrders[ nChosenIndex ] = nTmp;

					nBgn++;*/

					if(1231 == i)
						i = i;

					if( nBgn >= nIndirCnt )
					{
						break;
					}
				}
				else
				{
					nextDists[nChosenIndex] = nbrDists[ nbrDistIndex.Calc( 
						nNbrOrder, locIndirs[ nChosenIndex ] ) ]; 
				}

////////////////////

				for(int j=0; j < nIndirCnt; j++)
				{
					if( nIndir == locIndirs[ j ] )
						goto IndirDone;
				}

////////////////////////////////////////

				if(36 == nIndir && 99 == i)
					i = i;

				nSelf = nIndir;
				nNbrOrder = 0;

				locIndirs[nIndirCnt] = nSelf;
				nextOrders[nIndirCnt] = nNbrOrder;
				nextDists[nIndirCnt] = nbrDists[ nbrDistIndex.Calc( 
					nNbrOrder, nSelf) ];

				nIndirCnt++;
				indirCnts[i] = nIndirCnt;

////////////////////////////////////////

IndirDone:
				;
			}

			if(1231 == i)
				i = i;

			if(nIndirCnt < nofIndirs)
				locIndirs[ nIndirCnt ] = -1;
		}
		


//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////


		for(int i=0; i<nDataSiz; i++)
		{
			int sumX = 0;
			int sumY = 0;

			int * locIndirs = 
				&indirs[ indirIndex.Calc(0, i) ];
			
			//int nofActIndires = nofIndirs;
			int nofActIndires = 0;

//			for(int j=0; j<nofIndirs; j++)
			for(int j=0; j<indirCnts[i]; j++)				
			//for(int j= (nofIndirs * 1) / 3; j<nofIndirs; j++)
			{
				int k = locIndirs[j];

				//if(-1 == k)
				if(k < 0)
				{
					//nofActIndires = j;
					break;
				}

				sumX += dataPts[k].x;
				sumY += dataPts[k].y;

				nofActIndires++;
			}

			int nTmpX = (int) (sumX / indirCnts[i]);
			int nTmpY = (int) (sumY / indirCnts[i]);

			int nOrgX = dataPts[i].x;
			int nOrgY = dataPts[i].y;

/*			if(indirCnts[i] < nofIndirs)
			{
				nTmpX = nOrgX;
				nTmpY = nOrgY;
			}*/

			int nAlfa = 1;

			resDataPts[i].x = nOrgX + (nTmpX - nOrgX) / nAlfa;
			resDataPts[i].y = nOrgY + (nTmpY - nOrgY) / nAlfa;
		}



		CvScalar pntColor = CV_RGB(255, 0, 255);
		CvScalar indirColor = CV_RGB(0, 255, 0);
		CvScalar spColor = CV_RGB(255, 130, 0);


		int nShowPoint = 55;

		//const int spX = 349;
//		const int spX = 352;
//		const int spY = 186;

		const int spX = 303;
		const int spY = 93;

		for(int i=0; i<nDataSiz; i++)
		{
			//if( spX == dataPts[i].x && spY == dataPts[i].y )
			if( spX == resDataPts[i].x && spY == resDataPts[i].y )
			{
				nShowPoint = i;
				break;
			}
		}


		const int nofSPIndirs = indirCnts[ nShowPoint ];

		int * spLocIndirs = &indirs[ indirIndex.Calc(0, nShowPoint) ];

		std::vector<CvPoint> spIndirPts( indirCnts[ nofSPIndirs ] );

		DrawPoints(dataPts, a_orgImg, pntColor);

		for(int i=0; i < nofSPIndirs; i++)
			spIndirPts[i] = dataPts[ spLocIndirs[ i ] ]; 

		DrawPoints(spIndirPts, a_orgImg, indirColor);

		spIndirPts.resize(0);
		spIndirPts.push_back( dataPts[ nShowPoint ] );

		DrawPoints(spIndirPts, a_orgImg, spColor);

		///////////

		DrawPoints(resDataPts, a_resImg, pntColor);

		spIndirPts.resize( nofSPIndirs );

		for(int i=0; i < nofSPIndirs; i++)
			spIndirPts[i] = resDataPts[ spLocIndirs[ i ] ]; 

		DrawPoints(spIndirPts, a_resImg, indirColor);

		spIndirPts.resize(0);
		spIndirPts.push_back( resDataPts[ nShowPoint ] );

		DrawPoints(spIndirPts, a_resImg, spColor);

	}

	


	void DoClusteringStep13(std::vector<CvPoint> * a_pDataPts,
		std::vector<CvPoint> * a_pResDataPts, 
		S16ImageRef a_orgImg, S16ImageRef a_resImg,
		std::vector< Pattern< int, 2 > > * a_pPatArr,
		std::vector<int> * a_pDataPtCnts)
	{
		Pattern< int, 2 > & p1 = (*a_pPatArr)[0];
		Pattern< int, 2 > & p2 = (*a_pPatArr)[1];

		std::vector<CvPoint> & dataPts = *a_pDataPts;
		const int nDataSiz = dataPts.size();

		std::vector<CvPoint> & resDataPts = *a_pResDataPts;


		const int nDistStep = 100;
		const int nHugeDist = 0x7f7f7f7f;
		const int nNbrDist = 30 * nDistStep;
		
/*		int nMinX = nHugeDist;
		int nMaxX = 0;

		int nMinY = nHugeDist;
		int nMaxY = 0;

		for(int i=0; i<nDataSiz; i++)
		{
			int x = dataPts[i].x;

			if(x < nMinX)
				nMinX = x;
			if(x > nMaxX)
				nMaxX = x;

			int y = dataPts[i].y;

			if(y < nMinY)
				nMinY = y;
			if(y > nMaxY)
				nMaxY = y;
		}*/



		const int nofNbrs = 5;
		//const int nofNbrs = 10;

		std::vector<int> nbrDists( nDataSiz * nofNbrs );
		memset( &nbrDists[0], 0x7f, nbrDists.size() * sizeof(int));

		IndexCalc2D nbrDistIndex( nofNbrs, nDataSiz );

		std::vector<int> nbrs( nDataSiz * nofNbrs );
		IndexCalc2D nbrIndex( nofNbrs, nDataSiz );

		std::vector<int> maxMinNbrIndices( nDataSiz );
		memset( &maxMinNbrIndices[0], 0, maxMinNbrIndices.size() * sizeof(int));

		

		for(int i=0; i<nDataSiz; i++)
		{
			int & nMaxMinIndexI = maxMinNbrIndices[ i ];
			int * locNbrsI = 
				&nbrs[ nbrIndex.Calc(0, i) ];
			int * locNbrDistsI = 
				&nbrDists[ nbrIndex.Calc(0, i) ];

			for(int j=i+1; j<nDataSiz; j++)
			{
				int nAbsDifX =  abs( dataPts[i].x - dataPts[j].x ) * nDistStep;
				if(nAbsDifX > nNbrDist)
					continue;

				int nAbsDifY =  abs( dataPts[i].y - dataPts[j].y ) * nDistStep;
				if(nAbsDifY > nNbrDist)
					continue;


				int dist = sqrt( (double) (Sqr( nAbsDifX ) +
						Sqr( nAbsDifY )) );
				if(dist > nNbrDist)
					continue;

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
					&nbrs[ nbrIndex.Calc(0, j) ];
				int * locNbrDistsJ = 
					&nbrDists[ nbrIndex.Calc(0, j) ];

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

			}
		}


		for(int i=0; i<nDataSiz; i++)
		{
			int & nMaxMinIndexI = maxMinNbrIndices[ i ];
			int * locNbrsI = 
				&nbrs[ nbrIndex.Calc(0, i) ];
			int * locNbrDistsI = 
				&nbrDists[ nbrIndex.Calc(0, i) ];



			for(int j=0; j < nofNbrs; j++)
			{
				//if(nHugeDist == locNbrDistsI[ j ])
				if(nHugeDist == locNbrDistsI[ j ])
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

		const int nofIndirs = 20;
		//const int nofIndirs = 40;
		const int nofGuids = 5;

		std::vector<int> indirs( nDataSiz * nofIndirs );
		IndexCalc2D indirIndex( nofIndirs, nDataSiz );

		std::vector<int> indirCnts( nDataSiz );
		
		int nextDists[ nofIndirs ];
		int nextOrders[ nofIndirs ];

		for(int i=0; i<nDataSiz; i++)
		{
			indirCnts[i] = 0;
			int nIndirCnt = 0;

			//memset(nextDists, 0, sizeof(int) * nofIndirs);
			//memset(nextOrders, 0, sizeof(int) * nofIndirs);

			int * locIndirs = 
				&indirs[ indirIndex.Calc(0, i) ];

			int nSelf;
			int nNbrOrder;

////////////////////////////////

			nSelf = i;
			nNbrOrder = 0;

			locIndirs[nIndirCnt] = nSelf;
			nextOrders[nIndirCnt] = nNbrOrder;
			nextDists[nIndirCnt] = nbrDists[ nbrDistIndex.Calc( 
				nNbrOrder, nSelf) ];

			nIndirCnt++;

			indirCnts[i] = nIndirCnt;

////////////////////////////////

			int nChosenIndex = 0;

			int nCycleIndex = 0;
			int nCycleSiz = nofGuids;
			int nCycleID = 0;
			int nofContinues = 0;
			int nBgn = 0;
			int nMinCycleDist = nHugeDist;

			double nMaxAllowedDist = 0;

			bool bPrepareDist = true;

			while(nIndirCnt < nofIndirs)
			{
				//if(nIndirCnt < nofGuids)
				if(true == bPrepareDist)
				{

					if(nIndirCnt >= nofGuids)
					{
						nMaxAllowedDist *= 2;
						//nMaxAllowedDist *= 1.5;
						//nMaxAllowedDist = nHugeDist;

						bPrepareDist = false;
						continue;
					}


					int nMinDistIndex = 0;
					for(int j=0; j < nIndirCnt; j++)
					{
						if( nextDists[j] < nextDists[ nMinDistIndex ])
							nMinDistIndex = j;
					}

					if(nextDists[ nMinDistIndex ] > nMaxAllowedDist)
						nMaxAllowedDist = nextDists[ nMinDistIndex ];
					
					nChosenIndex = nMinDistIndex;
				}
				else
				{
					if(1231 == i)
						i = i;

					nChosenIndex = nCycleIndex;

					if(nChosenIndex >= nCycleSiz)
					//if(nCycleIndex >= nCycleSiz)
					//if(nCycleIndex >= nIndirCnt)
					{
						if(nofContinues >= nCycleSiz - nBgn)
						{
//							nMaxAllowedDist *= 1.5;
							nMaxAllowedDist += nDistStep;

							if( nMaxAllowedDist > nNbrDist )
								break;
							//i = i;
						}

						nCycleIndex = nBgn;
						nChosenIndex = nBgn;
						nofContinues = 0;
						nCycleSiz = nIndirCnt;
						nCycleID++;
						nMinCycleDist = nHugeDist;
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

				nSelf = locIndirs[ nChosenIndex ];

				const int nIndir = nbrs[ nbrIndex.Calc( 
					nextOrders[ nChosenIndex ], nSelf ) ];

				nNbrOrder = ++nextOrders[ nChosenIndex ];
				if(nNbrOrder >= nofNbrs)
				{
					nextDists[ nChosenIndex ] = nHugeDist;

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

					if( nBgn >= nIndirCnt )
					{
						break;
					}
				}
				else
				{
					nextDists[nChosenIndex] = nbrDists[ nbrDistIndex.Calc( 
						nNbrOrder, locIndirs[ nChosenIndex ] ) ]; 
				}

////////////////////

				for(int j=0; j < nIndirCnt; j++)
				{
					if( nIndir == locIndirs[ j ] )
						goto IndirDone;
				}

////////////////////////////////////////

				nSelf = nIndir;
				nNbrOrder = 0;

				locIndirs[nIndirCnt] = nSelf;
				nextOrders[nIndirCnt] = nNbrOrder;
				nextDists[nIndirCnt] = nbrDists[ nbrDistIndex.Calc( 
					nNbrOrder, nSelf) ];

				nIndirCnt++;
				indirCnts[i] = nIndirCnt;

////////////////////////////////////////

IndirDone:
				;
			}

			if(1231 == i)
				i = i;
		}
		


//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////


		for(int i=0; i<nDataSiz; i++)
		{
			int sumX = 0;
			int sumY = 0;

			int * locIndirs = 
				&indirs[ indirIndex.Calc(0, i) ];
			
			//int nofActIndires = nofIndirs;
			int nofActIndires = 0;

//			for(int j=0; j<nofIndirs; j++)
			for(int j=0; j<indirCnts[i]; j++)				
			//for(int j= (nofIndirs * 1) / 3; j<nofIndirs; j++)
			{
				int k = locIndirs[j];

				//if(-1 == k)
				if(k < 0)
				{
					//nofActIndires = j;
					break;
				}

				sumX += dataPts[k].x;
				sumY += dataPts[k].y;

				nofActIndires++;
			}

			int nTmpX = (int) (sumX / indirCnts[i]);
			int nTmpY = (int) (sumY / indirCnts[i]);

			int nOrgX = dataPts[i].x;
			int nOrgY = dataPts[i].y;

/*			if(indirCnts[i] < nofIndirs)
			{
				nTmpX = nOrgX;
				nTmpY = nOrgY;
			}*/

			int nAlfa = 1;

			resDataPts[i].x = nOrgX + (nTmpX - nOrgX) / nAlfa;
			resDataPts[i].y = nOrgY + (nTmpY - nOrgY) / nAlfa;
		}


		std::vector<int> friends( nDataSiz * nofIndirs );
		IndexCalc2D friendIndex( nofIndirs, nDataSiz );
		std::vector<int> friendCnts( nDataSiz );
		std::vector<int> ids( nDataSiz );


		for(int i=0; i<nDataSiz; i++)
		{
			ids[i] = i;

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

				int nOldDist;
				{
					int nAbsDifX =  abs( dataPts[i].x - dataPts[ind].x ) * nDistStep;
					int nAbsDifY =  abs( dataPts[i].y - dataPts[ind].y ) * nDistStep;

					nOldDist = sqrt( (double) (Sqr( nAbsDifX ) +
						Sqr( nAbsDifY )) );
				}

				int nNewDist;
				{
					int nAbsDifX =  abs( resDataPts[i].x - resDataPts[ind].x ) * nDistStep;
					int nAbsDifY =  abs( resDataPts[i].y - resDataPts[ind].y ) * nDistStep;

//					int nAbsDifX =  abs( resDataPts[i].x - dataPts[ind].x ) * nDistStep;
//					int nAbsDifY =  abs( resDataPts[i].y - dataPts[ind].y ) * nDistStep;

					nNewDist = sqrt( (double) (Sqr( nAbsDifX ) +
						Sqr( nAbsDifY )) );
				}

				//if(27 == i)
				if(4646 == i)					
					i = i;

				if( nNewDist > nOldDist * 1.3 )
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

			nID = ids[i];

			while( true )
			{
				k = nID;
				tmpArr[ nCnt++ ] = k;

				if( k < nMin )
					nMin = k;				

				nID = ids[k];

				if(k == nID)
					break;
			}

			for(int j=0; j<nofFriendsI; j++)
			{
				const int nFrnd = locFriends[ j ];

				if( i < 27 && 27 == nFrnd)
					i = i;

				nID = ids[ nFrnd ];

				while( true )
				{
					k = nID;
					tmpArr[ nCnt++ ] = k;

					if( k < nMin )
						nMin = k;				

					nID = ids[k];

					if(k == nID)
						break;
				}
			}

			if(26 == i)
				i = i;

			for(int j=0; j<nCnt; j++)
			{
				ids[ tmpArr[ j ] ] = nMin;
			}
		}

		int nofClusters = 0;


		for(int i=0; i<nDataSiz; i++)
		{
			if( 12 == i )
				i = i;

			int nCnt = 0;

			int k, nID;
			int nMin = nDataSiz;

			tmpArr[ nCnt++ ] = i;

			nID = ids[i];

			while( true )
			{
				k = nID;
				tmpArr[ nCnt++ ] = k;

				if( k < nMin )
					nMin = k;				

				nID = ids[k];

				if(k == nID)
					break;
			}

			for(int j=0; j<nCnt; j++)
				ids[ tmpArr[ j ] ] = nMin;

			if(i == ids[i])
				nofClusters++;
		}



		std::vector<int> clusterCnts( nofClusters );
		memset( &clusterCnts[0], 0, clusterCnts.size() * sizeof(int) );

		{
		std::vector<int> clusterIDs( nofClusters );

		int nMaxID = 0;
		clusterIDs[ nMaxID ] = 0;

		for(int i=0; i<nDataSiz; i++)
		{
			int & rPtID = ids[i];

			if(rPtID > clusterIDs[ nMaxID ])
			{
				nMaxID++;
				clusterIDs[ nMaxID ] = rPtID;
				rPtID = nMaxID;
				clusterCnts[ nMaxID ]++;
			}
			else
			{
				//for(int j=0; j<=nMaxID; j++)
				{
					int j = ids[ rPtID ];
					//if(rPtID == clusterIDs[ j ])
					{
						rPtID = j;
						clusterCnts[ j ]++;
						//break;
					}
				}
			}

		}

		}

		std::vector<U8ColorVal> colorArr( nofClusters + 100 );
		memset( &colorArr[0], 0, colorArr.size() * sizeof(U8ColorVal));

		colorArr[0] = u8ColorVal(0, 0, 255);
		colorArr[1] = u8ColorVal(0, 255, 0);
		colorArr[2] = u8ColorVal(255, 0, 0);
		colorArr[3] = u8ColorVal(255, 0, 255);
		colorArr[4] = u8ColorVal(0, 255, 255);
		colorArr[5] = u8ColorVal(255, 255, 0);

		DrawPoints(a_resImg, &dataPts, &ids, &colorArr);



/*
		CvScalar pntColor = CV_RGB(255, 255, 0);
		//CvScalar indirColor = CV_RGB(0, 255, 0);
		CvScalar indirColor = CV_RGB(255, 255, 0);
		CvScalar friendColor = CV_RGB(0, 0, 255);
		CvScalar spColor = CV_RGB(255, 130, 0);


		int nShowPoint = 55;

		const int spX = 349;
//		const int spX = 352;
		const int spY = 186;

//		const int spX = 303;
//		const int spY = 93;

		for(int i=0; i<nDataSiz; i++)
		{
			if( spX == dataPts[i].x && spY == dataPts[i].y )
			//if( spX == resDataPts[i].x && spY == resDataPts[i].y )
			{
				nShowPoint = i;
				break;
			}
		}


		DrawPoints(dataPts, a_orgImg, pntColor);








		const int nofSPIndirs = indirCnts[ nShowPoint ];

		int * spLocIndirs = &indirs[ indirIndex.Calc(0, nShowPoint) ];

		std::vector<CvPoint> spIndirPts( nofSPIndirs );

		for(int i=0; i < nofSPIndirs; i++)
			spIndirPts[i] = dataPts[ spLocIndirs[ i ] ]; 

		DrawPoints(spIndirPts, a_orgImg, indirColor);




		const int nofSPFriends = friendCnts[ nShowPoint ];

		int * spLocFriends = &friends[ friendIndex.Calc(0, nShowPoint) ];

		std::vector<CvPoint> spFriendPts( nofSPFriends);

		for(int i=0; i < nofSPFriends; i++)
			spFriendPts[i] = dataPts[ spLocFriends[ i ] ]; 

		DrawPoints(spFriendPts, a_orgImg, friendColor);



		spIndirPts.resize(0);
		spIndirPts.push_back( dataPts[ nShowPoint ] );

		DrawPoints(spIndirPts, a_orgImg, spColor);

		///////////

		DrawPoints(resDataPts, a_resImg, pntColor);




		spIndirPts.resize( nofSPIndirs );

		for(int i=0; i < nofSPIndirs; i++)
			spIndirPts[i] = resDataPts[ spLocIndirs[ i ] ]; 

		DrawPoints(spIndirPts, a_resImg, indirColor);



		spFriendPts.resize( nofSPFriends );

		for(int i=0; i < nofSPFriends; i++)
			spFriendPts[i] = resDataPts[ spLocFriends[ i ] ]; 

		DrawPoints(spFriendPts, a_resImg, friendColor);



		spIndirPts.resize(0);
		spIndirPts.push_back( resDataPts[ nShowPoint ] );

		DrawPoints(spIndirPts, a_resImg, spColor);
*/
	}

	

	void DoClusteringStep14(std::vector<CvPoint> * a_pDataPts,
		std::vector<CvPoint> * a_pResDataPts, 
		S16ImageRef a_orgImg, S16ImageRef a_resImg,
		std::vector< Pattern< int, 2 > > * a_pPatArr,
		std::vector<int> * a_pDataPtCnts)
	{
		const int nofFeatures = 2;

		Pattern< int, nofFeatures > & p1 = (*a_pPatArr)[0];
		Pattern< int, nofFeatures > & p2 = (*a_pPatArr)[1];

		//std::vector< Pattern< int, nofFeatures > > & patArr = *a_pPatArr;

		std::vector<CvPoint> & dataPts = *a_pDataPts;

		IndexCalc2D a_patIndex( nofFeatures, dataPts.size() );

		const int nDataSiz = a_patIndex.GetSizeY();

		std::vector<CvPoint> & resDataPts = *a_pResDataPts;
		//std::vector< Pattern< int, nofFeatures > > & resPatArr = 
			//*( (std::vector< Pattern< int, nofFeatures > > *)  a_pResDataPts);

		int * pats = (int *) &dataPts[0];

		int * resPats = (int *) &resDataPts[0];


		//std::vector<int> nbrs( nDataSiz * nofNbrs );
		//IndexCalc2D nbrIndex( nofNbrs, nDataSiz );

		const int nDistStep = 10;
		const int nHugeDist = 0x7f7f7f7f;
		const int nNbrDist = 30 * nDistStep;
		
		int nMinX = nHugeDist;
		int nMaxX = 0;

		int nMinY = nHugeDist;
		int nMaxY = 0;


		const int nofNbrs = 5;
		//const int nofNbrs = 10;
		//const int nofNbrs = 50;

		std::vector<int> nbrDists( nDataSiz * nofNbrs );
		memset( &nbrDists[0], 0x7f, nbrDists.size() * sizeof(int));

		IndexCalc2D nbrDistIndex( nofNbrs, nDataSiz );

		std::vector<int> nbrs( nDataSiz * nofNbrs );
		IndexCalc2D nbrIndex( nofNbrs, nDataSiz );

		std::vector<int> maxMinNbrIndices( nDataSiz );
		memset( &maxMinNbrIndices[0], 0, maxMinNbrIndices.size() * sizeof(int));

		

		for(int i=0; i<nDataSiz; i++)
		{
			int & nMaxMinIndexI = maxMinNbrIndices[ i ];
			int * locNbrsI = 
				&nbrs[ nbrIndex.Calc(0, i) ];
			int * locNbrDistsI = 
				&nbrDists[ nbrIndex.Calc(0, i) ];

			int * featsI = &pats[ a_patIndex.Calc(0, i) ];

			for(int j=i+1; j<nDataSiz; j++)
			{
				int * featsJ = &pats[ a_patIndex.Calc(0, j) ];

				int sqrSum = 0;
				for(int f=0; f < nofFeatures; f++)
				{
					int nAbsDifF =  abs( featsI[f] - featsJ[f] ) * nDistStep;
					if(nAbsDifF > nNbrDist)
						goto DoneJ;

					sqrSum += Sqr( nAbsDifF );
				}

				int dist = sqrt( (double) sqrSum );
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
					&nbrs[ nbrIndex.Calc(0, j) ];
				int * locNbrDistsJ = 
					&nbrDists[ nbrIndex.Calc(0, j) ];

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


		for(int i=0; i<nDataSiz; i++)
		{
			int & nMaxMinIndexI = maxMinNbrIndices[ i ];
			int * locNbrsI = 
				&nbrs[ nbrIndex.Calc(0, i) ];
			int * locNbrDistsI = 
				&nbrDists[ nbrIndex.Calc(0, i) ];



			for(int j=0; j < nofNbrs; j++)
			{
				//if(nHugeDist == locNbrDistsI[ j ])
				if(nHugeDist == locNbrDistsI[ j ])
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

		const int nofIndirs = 20;
		//const int nofIndirs = 40;
		const int nofGuids = 5;

		std::vector<int> indirs( nDataSiz * nofIndirs );
		IndexCalc2D indirIndex( nofIndirs, nDataSiz );

		std::vector<int> indirCnts( nDataSiz );
		
		int nextDists[ nofIndirs ];
		int nextOrders[ nofIndirs ];

		for(int i=0; i<nDataSiz; i++)
		{
			indirCnts[i] = 0;
			int nIndirCnt = 0;

			//memset(nextDists, 0, sizeof(int) * nofIndirs);
			//memset(nextOrders, 0, sizeof(int) * nofIndirs);

			int * locIndirs = 
				&indirs[ indirIndex.Calc(0, i) ];

			int nSelf;
			int nNbrOrder;

////////////////////////////////

			nSelf = i;
			nNbrOrder = 0;

			locIndirs[nIndirCnt] = nSelf;
			nextOrders[nIndirCnt] = nNbrOrder;
			nextDists[nIndirCnt] = nbrDists[ nbrDistIndex.Calc( 
				nNbrOrder, nSelf) ];

			nIndirCnt++;

			indirCnts[i] = nIndirCnt;

////////////////////////////////

			int nChosenIndex = 0;

			int nCycleIndex = 0;
			int nCycleSiz = nofGuids;
			int nCycleID = 0;
			int nofContinues = 0;
			int nBgn = 0;
			int nMinCycleDist = nHugeDist;

			double nMaxAllowedDist = 0;

			bool bPrepareDist = true;

			while(nIndirCnt < nofIndirs)
			{
				//if(nIndirCnt < nofGuids)
				if(true == bPrepareDist)
				{

					if(nIndirCnt >= nofGuids)
					{
						nMaxAllowedDist *= 2;
						//nMaxAllowedDist *= 1.5;
						//nMaxAllowedDist = nHugeDist;

						bPrepareDist = false;
						continue;
					}


					int nMinDistIndex = 0;
					for(int j=0; j < nIndirCnt; j++)
					{
						if( nextDists[j] < nextDists[ nMinDistIndex ])
							nMinDistIndex = j;
					}

					if(nextDists[ nMinDistIndex ] > nMaxAllowedDist)
						nMaxAllowedDist = nextDists[ nMinDistIndex ];
					
					nChosenIndex = nMinDistIndex;
				}
				else
				{
					if(1231 == i)
						i = i;

					nChosenIndex = nCycleIndex;

					if(nChosenIndex >= nCycleSiz)
					//if(nCycleIndex >= nCycleSiz)
					//if(nCycleIndex >= nIndirCnt)
					{
						if(nofContinues >= nCycleSiz - nBgn)
						{
//							nMaxAllowedDist *= 1.5;
							nMaxAllowedDist += nDistStep;

							if( nMaxAllowedDist > nNbrDist )
								break;
							//i = i;
						}

						nCycleIndex = nBgn;
						nChosenIndex = nBgn;
						nofContinues = 0;
						nCycleSiz = nIndirCnt;
						nCycleID++;
						nMinCycleDist = nHugeDist;
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

				nSelf = locIndirs[ nChosenIndex ];

				const int nIndir = nbrs[ nbrIndex.Calc( 
					nextOrders[ nChosenIndex ], nSelf ) ];

				nNbrOrder = ++nextOrders[ nChosenIndex ];
				if(nNbrOrder >= nofNbrs)
				{
					nextDists[ nChosenIndex ] = nHugeDist;

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

					if( nBgn >= nIndirCnt )
					{
						break;
					}
				}
				else
				{
					nextDists[nChosenIndex] = nbrDists[ nbrDistIndex.Calc( 
						nNbrOrder, locIndirs[ nChosenIndex ] ) ]; 
				}

////////////////////

				for(int j=0; j < nIndirCnt; j++)
				{
					if( nIndir == locIndirs[ j ] )
						goto IndirDone;
				}

////////////////////////////////////////

				nSelf = nIndir;
				nNbrOrder = 0;

				locIndirs[nIndirCnt] = nSelf;
				nextOrders[nIndirCnt] = nNbrOrder;
				nextDists[nIndirCnt] = nbrDists[ nbrDistIndex.Calc( 
					nNbrOrder, nSelf) ];

				nIndirCnt++;
				indirCnts[i] = nIndirCnt;

////////////////////////////////////////

IndirDone:
				;
			}

			if(1231 == i)
				i = i;
		}
		


//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////


		std::vector<int> featSums( nofFeatures );

		for(int i=0; i<nDataSiz; i++)
		{
			memset( &featSums[0], 0, featSums.size() * sizeof(int) );

			int * resFeatsI = &resPats[ a_patIndex.Calc(0, i) ];

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

				int * featsK = &pats[ a_patIndex.Calc(0, k) ];

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
		std::vector<int> ids( nDataSiz );


		for(int i=0; i<nDataSiz; i++)
		{
			int * featsI = &pats[ a_patIndex.Calc(0, i) ];
			int * resFeatsI = &resPats[ a_patIndex.Calc(0, i) ];

			ids[i] = i;

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
				int * featsInd = &pats[ a_patIndex.Calc(0, ind) ];

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

				int * resFeatsInd = &resPats[ a_patIndex.Calc(0, ind) ];

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

			nID = ids[i];

			while( true )
			{
				k = nID;
				tmpArr[ nCnt++ ] = k;

				if( k < nMin )
					nMin = k;				

				nID = ids[k];

				if(k == nID)
					break;
			}

			for(int j=0; j<nofFriendsI; j++)
			{
				const int nFrnd = locFriends[ j ];

				if( i < 27 && 27 == nFrnd)
					i = i;

				nID = ids[ nFrnd ];

				while( true )
				{
					k = nID;
					tmpArr[ nCnt++ ] = k;

					if( k < nMin )
						nMin = k;				

					nID = ids[k];

					if(k == nID)
						break;
				}
			}

			if(26 == i)
				i = i;

			for(int j=0; j<nCnt; j++)
			{
				ids[ tmpArr[ j ] ] = nMin;
			}
		}

		int nofClusters = 0;


		for(int i=0; i<nDataSiz; i++)
		{
			if( 12 == i )
				i = i;

			int nCnt = 0;

			int k, nID;
			int nMin = nDataSiz;

			tmpArr[ nCnt++ ] = i;

			nID = ids[i];

			while( true )
			{
				k = nID;
				tmpArr[ nCnt++ ] = k;

				if( k < nMin )
					nMin = k;				

				nID = ids[k];

				if(k == nID)
					break;
			}

			for(int j=0; j<nCnt; j++)
				ids[ tmpArr[ j ] ] = nMin;

			if(i == ids[i])
				nofClusters++;
		}



		std::vector<int> clusterCnts( nofClusters );
		memset( &clusterCnts[0], 0, clusterCnts.size() * sizeof(int) );

		{
		std::vector<int> clusterIDs( nofClusters );

		int nMaxID = 0;
		clusterIDs[ nMaxID ] = 0;

		for(int i=0; i<nDataSiz; i++)
		{
			int & rPtID = ids[i];

			if(rPtID > clusterIDs[ nMaxID ])
			{
				nMaxID++;
				clusterIDs[ nMaxID ] = rPtID;
				rPtID = nMaxID;
				clusterCnts[ nMaxID ]++;
			}
			else
			{
				//for(int j=0; j<=nMaxID; j++)
				{
					int j = ids[ rPtID ];
					//if(rPtID == clusterIDs[ j ])
					{
						rPtID = j;
						clusterCnts[ j ]++;
						//break;
					}
				}
			}

		}

		}

		std::vector<U8ColorVal> colorArr( nofClusters + 100 );
		memset( &colorArr[0], 0, colorArr.size() * sizeof(U8ColorVal));

		colorArr[0] = u8ColorVal(0, 0, 255);
		colorArr[1] = u8ColorVal(0, 255, 0);
		colorArr[2] = u8ColorVal(255, 0, 0);
		colorArr[3] = u8ColorVal(255, 0, 255);
		colorArr[4] = u8ColorVal(0, 255, 255);
		colorArr[5] = u8ColorVal(255, 255, 0);
		colorArr[6] = u8ColorVal(128, 128, 128);

		DrawPoints(a_resImg, &dataPts, &ids, &colorArr);



/*
		CvScalar pntColor = CV_RGB(255, 255, 0);
		//CvScalar indirColor = CV_RGB(0, 255, 0);
		CvScalar indirColor = CV_RGB(255, 255, 0);
		CvScalar friendColor = CV_RGB(0, 0, 255);
		CvScalar spColor = CV_RGB(255, 130, 0);


		int nShowPoint = 55;

		const int spX = 349;
//		const int spX = 352;
		const int spY = 186;

//		const int spX = 303;
//		const int spY = 93;

		for(int i=0; i<nDataSiz; i++)
		{
			if( spX == dataPts[i].x && spY == dataPts[i].y )
			//if( spX == resDataPts[i].x && spY == resDataPts[i].y )
			{
				nShowPoint = i;
				break;
			}
		}


		DrawPoints(dataPts, a_orgImg, pntColor);








		const int nofSPIndirs = indirCnts[ nShowPoint ];

		int * spLocIndirs = &indirs[ indirIndex.Calc(0, nShowPoint) ];

		std::vector<CvPoint> spIndirPts( nofSPIndirs );

		for(int i=0; i < nofSPIndirs; i++)
			spIndirPts[i] = dataPts[ spLocIndirs[ i ] ]; 

		DrawPoints(spIndirPts, a_orgImg, indirColor);




		const int nofSPFriends = friendCnts[ nShowPoint ];

		int * spLocFriends = &friends[ friendIndex.Calc(0, nShowPoint) ];

		std::vector<CvPoint> spFriendPts( nofSPFriends);

		for(int i=0; i < nofSPFriends; i++)
			spFriendPts[i] = dataPts[ spLocFriends[ i ] ]; 

		DrawPoints(spFriendPts, a_orgImg, friendColor);



		spIndirPts.resize(0);
		spIndirPts.push_back( dataPts[ nShowPoint ] );

		DrawPoints(spIndirPts, a_orgImg, spColor);

		///////////

		DrawPoints(resDataPts, a_resImg, pntColor);




		spIndirPts.resize( nofSPIndirs );

		for(int i=0; i < nofSPIndirs; i++)
			spIndirPts[i] = resDataPts[ spLocIndirs[ i ] ]; 

		DrawPoints(spIndirPts, a_resImg, indirColor);



		spFriendPts.resize( nofSPFriends );

		for(int i=0; i < nofSPFriends; i++)
			spFriendPts[i] = resDataPts[ spLocFriends[ i ] ]; 

		DrawPoints(spFriendPts, a_resImg, friendColor);



		spIndirPts.resize(0);
		spIndirPts.push_back( resDataPts[ nShowPoint ] );

		DrawPoints(spIndirPts, a_resImg, spColor);
*/
	}

	




	void DoClusteringStep15(std::vector<CvPoint> * a_pDataPts,
		std::vector<CvPoint> * a_pResDataPts, 
		S16ImageRef a_orgImg, S16ImageRef a_resImg,
		std::vector< Pattern< int, 2 > > * a_pPatArr,
		std::vector<int> * a_pDataPtCnts)
	{
		const int nofFeatures = 2;

		Pattern< int, nofFeatures > & p1 = (*a_pPatArr)[0];
		Pattern< int, nofFeatures > & p2 = (*a_pPatArr)[1];

		//std::vector< Pattern< int, nofFeatures > > & patArr = *a_pPatArr;

		std::vector<CvPoint> & dataPts = *a_pDataPts;

		IndexCalc2D a_patIndex( nofFeatures, dataPts.size() );

		const int nDataSiz = a_patIndex.GetSizeY();

		std::vector<CvPoint> & resDataPts = *a_pResDataPts;
		//std::vector< Pattern< int, nofFeatures > > & resPatArr = 
			//*( (std::vector< Pattern< int, nofFeatures > > *)  a_pResDataPts);

		int * pats = (int *) &dataPts[0];

		int * resPats = (int *) &resDataPts[0];


		//std::vector<int> nbrs( nDataSiz * nofNbrs );
		//IndexCalc2D nbrIndex( nofNbrs, nDataSiz );

		const int nDistStep = 10;
		const int nHugeDist = 0x7f7f7f7f;
		const int nNbrDist = 30 * nDistStep;
		
		int nMinX = nHugeDist;
		int nMaxX = 0;

		int nMinY = nHugeDist;
		int nMaxY = 0;


		const int nofNbrs = 5;
		//const int nofNbrs = 10;
		//const int nofNbrs = 50;

		std::vector<int> nbrDists( nDataSiz * nofNbrs );
		memset( &nbrDists[0], 0x7f, nbrDists.size() * sizeof(int));

		IndexCalc2D nbrDistIndex( nofNbrs, nDataSiz );

		std::vector<int> nbrs( nDataSiz * nofNbrs );
		IndexCalc2D nbrIndex( nofNbrs, nDataSiz );

		std::vector<int> maxMinNbrIndices( nDataSiz );
		memset( &maxMinNbrIndices[0], 0, maxMinNbrIndices.size() * sizeof(int));

		

		for(int i=0; i<nDataSiz; i++)
		{
			int & nMaxMinIndexI = maxMinNbrIndices[ i ];
			int * locNbrsI = 
				&nbrs[ nbrIndex.Calc(0, i) ];
			int * locNbrDistsI = 
				&nbrDists[ nbrIndex.Calc(0, i) ];

			int * featsI = &pats[ a_patIndex.Calc(0, i) ];

			for(int j=i+1; j<nDataSiz; j++)
			{
				int * featsJ = &pats[ a_patIndex.Calc(0, j) ];

				int sqrSum = 0;
				for(int f=0; f < nofFeatures; f++)
				{
					int nAbsDifF =  abs( featsI[f] - featsJ[f] ) * nDistStep;
					if(nAbsDifF > nNbrDist)
						goto DoneJ;

					sqrSum += Sqr( nAbsDifF );
				}

				int dist = sqrt( (double) sqrSum );
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
					&nbrs[ nbrIndex.Calc(0, j) ];
				int * locNbrDistsJ = 
					&nbrDists[ nbrIndex.Calc(0, j) ];

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


		for(int i=0; i<nDataSiz; i++)
		{
			int & nMaxMinIndexI = maxMinNbrIndices[ i ];
			int * locNbrsI = 
				&nbrs[ nbrIndex.Calc(0, i) ];
			int * locNbrDistsI = 
				&nbrDists[ nbrIndex.Calc(0, i) ];



			for(int j=0; j < nofNbrs; j++)
			{
				//if(nHugeDist == locNbrDistsI[ j ])
				if(nHugeDist == locNbrDistsI[ j ])
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

		const int nofIndirs = 20;
		//const int nofIndirs = 40;
		const int nofGuids = 5;

		std::vector<int> indirs( nDataSiz * nofIndirs );
		IndexCalc2D indirIndex( nofIndirs, nDataSiz );

		std::vector<int> indirCnts( nDataSiz );
		
		int nextDists[ nofIndirs ];
		int nextOrders[ nofIndirs ];

		for(int i=0; i<nDataSiz; i++)
		{
			indirCnts[i] = 0;
			int nIndirCnt = 0;

			//memset(nextDists, 0, sizeof(int) * nofIndirs);
			//memset(nextOrders, 0, sizeof(int) * nofIndirs);

			int * locIndirs = 
				&indirs[ indirIndex.Calc(0, i) ];

			int nSelf;
			int nNbrOrder;

////////////////////////////////

			nSelf = i;
			nNbrOrder = 0;

			locIndirs[nIndirCnt] = nSelf;
			nextOrders[nIndirCnt] = nNbrOrder;
			nextDists[nIndirCnt] = nbrDists[ nbrDistIndex.Calc( 
				nNbrOrder, nSelf) ];

			nIndirCnt++;

			indirCnts[i] = nIndirCnt;

////////////////////////////////

			int nChosenIndex = 0;

			int nCycleIndex = 0;
			int nCycleSiz = nofGuids;
			int nCycleID = 0;
			int nofContinues = 0;
			int nBgn = 0;
			int nMinCycleDist = nHugeDist;

			double nMaxAllowedDist = 0;

			bool bPrepareDist = true;

			while(nIndirCnt < nofIndirs)
			{
				//if(nIndirCnt < nofGuids)
				if(true == bPrepareDist)
				{

					if(nIndirCnt >= nofGuids)
					{
						nMaxAllowedDist *= 2;
						//nMaxAllowedDist *= 1.5;
						//nMaxAllowedDist = nHugeDist;

						bPrepareDist = false;
						continue;
					}


					int nMinDistIndex = 0;
					for(int j=0; j < nIndirCnt; j++)
					{
						if( nextDists[j] < nextDists[ nMinDistIndex ])
							nMinDistIndex = j;
					}

					if(nextDists[ nMinDistIndex ] > nMaxAllowedDist)
						nMaxAllowedDist = nextDists[ nMinDistIndex ];
					
					nChosenIndex = nMinDistIndex;
				}
				else
				{
					if(1231 == i)
						i = i;

					nChosenIndex = nCycleIndex;

					if(nChosenIndex >= nCycleSiz)
					//if(nCycleIndex >= nCycleSiz)
					//if(nCycleIndex >= nIndirCnt)
					{
						if(nofContinues >= nCycleSiz - nBgn)
						{
//							nMaxAllowedDist *= 1.5;
							nMaxAllowedDist += nDistStep;

							if( nMaxAllowedDist > nNbrDist )
								break;
							//i = i;
						}

						nCycleIndex = nBgn;
						nChosenIndex = nBgn;
						nofContinues = 0;
						nCycleSiz = nIndirCnt;
						nCycleID++;
						nMinCycleDist = nHugeDist;
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

				nSelf = locIndirs[ nChosenIndex ];

				const int nIndir = nbrs[ nbrIndex.Calc( 
					nextOrders[ nChosenIndex ], nSelf ) ];

				nNbrOrder = ++nextOrders[ nChosenIndex ];
				if(nNbrOrder >= nofNbrs)
				{
					nextDists[ nChosenIndex ] = nHugeDist;

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

					if( nBgn >= nIndirCnt )
					{
						break;
					}
				}
				else
				{
					nextDists[nChosenIndex] = nbrDists[ nbrDistIndex.Calc( 
						nNbrOrder, locIndirs[ nChosenIndex ] ) ]; 
				}

////////////////////

				for(int j=0; j < nIndirCnt; j++)
				{
					if( nIndir == locIndirs[ j ] )
						goto IndirDone;
				}

////////////////////////////////////////

				nSelf = nIndir;
				nNbrOrder = 0;

				locIndirs[nIndirCnt] = nSelf;
				nextOrders[nIndirCnt] = nNbrOrder;
				nextDists[nIndirCnt] = nbrDists[ nbrDistIndex.Calc( 
					nNbrOrder, nSelf) ];

				nIndirCnt++;
				indirCnts[i] = nIndirCnt;

////////////////////////////////////////

IndirDone:
				;
			}

			if(1231 == i)
				i = i;
		}
		


//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////


		std::vector<int> featSums( nofFeatures );

		for(int i=0; i<nDataSiz; i++)
		{
			memset( &featSums[0], 0, featSums.size() * sizeof(int) );

			int * resFeatsI = &resPats[ a_patIndex.Calc(0, i) ];

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

				int * featsK = &pats[ a_patIndex.Calc(0, k) ];

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
		std::vector<int> ids( nDataSiz );


		for(int i=0; i<nDataSiz; i++)
		{
			int * featsI = &pats[ a_patIndex.Calc(0, i) ];
			int * resFeatsI = &resPats[ a_patIndex.Calc(0, i) ];

			ids[i] = i;

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
				int * featsInd = &pats[ a_patIndex.Calc(0, ind) ];

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

				int * resFeatsInd = &resPats[ a_patIndex.Calc(0, ind) ];

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

			nID = ids[i];

			while( true )
			{
				k = nID;
				tmpArr[ nCnt++ ] = k;

				if( k < nMin )
					nMin = k;				

				nID = ids[k];

				if(k == nID)
					break;
			}

			for(int j=0; j<nofFriendsI; j++)
			{
				const int nFrnd = locFriends[ j ];

				if( i < 27 && 27 == nFrnd)
					i = i;

				nID = ids[ nFrnd ];

				while( true )
				{
					k = nID;
					tmpArr[ nCnt++ ] = k;

					if( k < nMin )
						nMin = k;				

					nID = ids[k];

					if(k == nID)
						break;
				}
			}

			if(26 == i)
				i = i;

			for(int j=0; j<nCnt; j++)
			{
				ids[ tmpArr[ j ] ] = nMin;
			}
		}

		int nofClusters = 0;


		for(int i=0; i<nDataSiz; i++)
		{
			if( 12 == i )
				i = i;

			int nCnt = 0;

			int k, nID;
			int nMin = nDataSiz;

			tmpArr[ nCnt++ ] = i;

			nID = ids[i];

			while( true )
			{
				k = nID;
				tmpArr[ nCnt++ ] = k;

				if( k < nMin )
					nMin = k;				

				nID = ids[k];

				if(k == nID)
					break;
			}

			for(int j=0; j<nCnt; j++)
				ids[ tmpArr[ j ] ] = nMin;

			if(i == ids[i])
				nofClusters++;
		}



		std::vector<int> clusterCnts( nofClusters );
		memset( &clusterCnts[0], 0, clusterCnts.size() * sizeof(int) );

		{
		std::vector<int> clusterIDs( nofClusters );

		int nMaxID = 0;
		clusterIDs[ nMaxID ] = 0;

		for(int i=0; i<nDataSiz; i++)
		{
			int & rPtID = ids[i];

			if(rPtID > clusterIDs[ nMaxID ])
			{
				nMaxID++;
				clusterIDs[ nMaxID ] = rPtID;
				rPtID = nMaxID;
				clusterCnts[ nMaxID ]++;
			}
			else
			{
				//for(int j=0; j<=nMaxID; j++)
				{
					int j = ids[ rPtID ];
					//if(rPtID == clusterIDs[ j ])
					{
						rPtID = j;
						clusterCnts[ j ]++;
						//break;
					}
				}
			}

		}

		}

		std::vector<U8ColorVal> colorArr( nofClusters + 100 );
		memset( &colorArr[0], 0, colorArr.size() * sizeof(U8ColorVal));

		colorArr[0] = u8ColorVal(0, 0, 255);
		colorArr[1] = u8ColorVal(0, 255, 0);
		colorArr[2] = u8ColorVal(255, 0, 0);
		colorArr[3] = u8ColorVal(255, 0, 255);
		colorArr[4] = u8ColorVal(0, 255, 255);
		colorArr[5] = u8ColorVal(255, 255, 0);
		colorArr[6] = u8ColorVal(128, 128, 128);

		DrawPoints(a_resImg, &dataPts, &ids, &colorArr);



/*
		CvScalar pntColor = CV_RGB(255, 255, 0);
		//CvScalar indirColor = CV_RGB(0, 255, 0);
		CvScalar indirColor = CV_RGB(255, 255, 0);
		CvScalar friendColor = CV_RGB(0, 0, 255);
		CvScalar spColor = CV_RGB(255, 130, 0);


		int nShowPoint = 55;

		const int spX = 349;
//		const int spX = 352;
		const int spY = 186;

//		const int spX = 303;
//		const int spY = 93;

		for(int i=0; i<nDataSiz; i++)
		{
			if( spX == dataPts[i].x && spY == dataPts[i].y )
			//if( spX == resDataPts[i].x && spY == resDataPts[i].y )
			{
				nShowPoint = i;
				break;
			}
		}


		DrawPoints(dataPts, a_orgImg, pntColor);








		const int nofSPIndirs = indirCnts[ nShowPoint ];

		int * spLocIndirs = &indirs[ indirIndex.Calc(0, nShowPoint) ];

		std::vector<CvPoint> spIndirPts( nofSPIndirs );

		for(int i=0; i < nofSPIndirs; i++)
			spIndirPts[i] = dataPts[ spLocIndirs[ i ] ]; 

		DrawPoints(spIndirPts, a_orgImg, indirColor);




		const int nofSPFriends = friendCnts[ nShowPoint ];

		int * spLocFriends = &friends[ friendIndex.Calc(0, nShowPoint) ];

		std::vector<CvPoint> spFriendPts( nofSPFriends);

		for(int i=0; i < nofSPFriends; i++)
			spFriendPts[i] = dataPts[ spLocFriends[ i ] ]; 

		DrawPoints(spFriendPts, a_orgImg, friendColor);



		spIndirPts.resize(0);
		spIndirPts.push_back( dataPts[ nShowPoint ] );

		DrawPoints(spIndirPts, a_orgImg, spColor);

		///////////

		DrawPoints(resDataPts, a_resImg, pntColor);




		spIndirPts.resize( nofSPIndirs );

		for(int i=0; i < nofSPIndirs; i++)
			spIndirPts[i] = resDataPts[ spLocIndirs[ i ] ]; 

		DrawPoints(spIndirPts, a_resImg, indirColor);



		spFriendPts.resize( nofSPFriends );

		for(int i=0; i < nofSPFriends; i++)
			spFriendPts[i] = resDataPts[ spLocFriends[ i ] ]; 

		DrawPoints(spFriendPts, a_resImg, friendColor);



		spIndirPts.resize(0);
		spIndirPts.push_back( resDataPts[ nShowPoint ] );

		DrawPoints(spIndirPts, a_resImg, spColor);
*/
	}

	

	void DoClusteringStep16(std::vector<CvPoint> * a_pDataPts,
		std::vector<CvPoint> * a_pResDataPts, 
		S16ImageRef a_orgImg, S16ImageRef a_resImg,
		std::vector< Pattern< int, 2 > > * a_pPatArr,
		std::vector<int> * a_pDataPtCnts)
	{
		ClusteringMgr cm;
		//ImageClusteringMgr cm;

		int * pats = (int *) &( *a_pDataPts )[0];

		IndexCalc2D patIndex( 2, ( *a_pDataPts ).size() );

		cm.Process(
			pats, 
			&patIndex
			//a_pResDataPts, 
			//a_orgImg, 
			//a_resImg
			);


		std::vector<U8ColorVal> colorArr( cm.GetNofClusters() + 100 );
		memset( &colorArr[0], 0, colorArr.size() * sizeof(U8ColorVal));

		colorArr[0] = u8ColorVal(0, 0, 255);
		colorArr[1] = u8ColorVal(0, 255, 0);
		colorArr[2] = u8ColorVal(255, 0, 0);
		colorArr[3] = u8ColorVal(255, 0, 255);
		colorArr[4] = u8ColorVal(0, 255, 255);
		colorArr[5] = u8ColorVal(255, 255, 0);
		colorArr[6] = u8ColorVal(128, 128, 128);

		//DrawPoints(a_resImg, a_pDataPts, cm.GetPatIDs(), &colorArr);

		DrawPoints(a_resImg, (CvPoint *) cm.GetResPats(), 
			patIndex.GetSizeY(), cm, &colorArr[0]);

/*
		CvScalar pntColor = CV_RGB(255, 255, 0);
		//CvScalar indirColor = CV_RGB(0, 255, 0);
		CvScalar indirColor = CV_RGB(255, 255, 0);
		CvScalar friendColor = CV_RGB(0, 0, 255);
		CvScalar spColor = CV_RGB(255, 130, 0);


		int nShowPoint = 55;

		const int spX = 349;
//		const int spX = 352;
		const int spY = 186;

//		const int spX = 303;
//		const int spY = 93;

		for(int i=0; i<nDataSiz; i++)
		{
			if( spX == dataPts[i].x && spY == dataPts[i].y )
			//if( spX == resDataPts[i].x && spY == resDataPts[i].y )
			{
				nShowPoint = i;
				break;
			}
		}


		DrawPoints(dataPts, a_orgImg, pntColor);








		const int nofSPIndirs = indirCnts[ nShowPoint ];

		int * spLocIndirs = &indirs[ indirIndex.Calc(0, nShowPoint) ];

		std::vector<CvPoint> spIndirPts( nofSPIndirs );

		for(int i=0; i < nofSPIndirs; i++)
			spIndirPts[i] = dataPts[ spLocIndirs[ i ] ]; 

		DrawPoints(spIndirPts, a_orgImg, indirColor);




		const int nofSPFriends = friendCnts[ nShowPoint ];

		int * spLocFriends = &friends[ friendIndex.Calc(0, nShowPoint) ];

		std::vector<CvPoint> spFriendPts( nofSPFriends);

		for(int i=0; i < nofSPFriends; i++)
			spFriendPts[i] = dataPts[ spLocFriends[ i ] ]; 

		DrawPoints(spFriendPts, a_orgImg, friendColor);



		spIndirPts.resize(0);
		spIndirPts.push_back( dataPts[ nShowPoint ] );

		DrawPoints(spIndirPts, a_orgImg, spColor);

		///////////

		DrawPoints(resDataPts, a_resImg, pntColor);




		spIndirPts.resize( nofSPIndirs );

		for(int i=0; i < nofSPIndirs; i++)
			spIndirPts[i] = resDataPts[ spLocIndirs[ i ] ]; 

		DrawPoints(spIndirPts, a_resImg, indirColor);



		spFriendPts.resize( nofSPFriends );

		for(int i=0; i < nofSPFriends; i++)
			spFriendPts[i] = resDataPts[ spLocFriends[ i ] ]; 

		DrawPoints(spFriendPts, a_resImg, friendColor);



		spIndirPts.resize(0);
		spIndirPts.push_back( resDataPts[ nShowPoint ] );

		DrawPoints(spIndirPts, a_resImg, spColor);
*/


	}





}