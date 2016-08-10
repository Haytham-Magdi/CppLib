#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\PeakMarkerFilt1D2.h>



namespace Hcv
{
	using namespace Hcpl;
	using namespace Hcpl::Math;


	PeakMarkerFilt1D2::PeakMarkerFilt1D2( int a_nAprSizOrg, ISingFilt1D2DataReaderRef a_difRdr,
			ISingFilt1D2DataReaderRef a_locRdr,
			ISingFilt1D2DataReaderRef a_dif2Rdr,
			ISingFilt1D2DataReaderRef a_angDifRdr,
			ISingFilt1D2DataReaderRef a_ZCDataRdr,
			IIntFilt1D2DataReaderRef a_ZCIndexDataRdr,
			IIntFilt1D2DataReaderRef a_ioaRdr,
			FixedVector< EdgeScan::EdgeInfo > & a_edgeInfoArr,
			FixedVector< EdgeScan::PixInfo > & a_pixInfoArr,
			EdgeScan::IEdgeInfoFilt1D2DataMgrRef a_edgeInfoDataMgr,
			ISingFilt1D2DataMgrRef a_outDataMgr, float a_dmyVal ) 
	{	
		m_nGrandCnt = 0;

		m_pEdgeInfoArr = & a_edgeInfoArr;

		m_pPixInfoArr = & a_pixInfoArr;

		m_difRdr = a_difRdr;
		m_inpRdrArr.PushBack( m_difRdr );

		m_locRdr = a_locRdr;
		m_inpRdrArr.PushBack( m_locRdr );

		m_dif2Rdr = a_dif2Rdr;
		m_inpRdrArr.PushBack( m_dif2Rdr );

		m_angDifRdr = a_angDifRdr;
		m_inpRdrArr.PushBack( m_angDifRdr );

		m_ZCDataRdr = a_ZCDataRdr;
		m_inpRdrArr.PushBack( m_ZCDataRdr );

		Hcpl_ASSERT( NULL != a_ZCIndexDataRdr );
		m_ZCIndexDataRdr = a_ZCIndexDataRdr;

		Hcpl_ASSERT( NULL != a_ioaRdr );
		m_ioaRdr = a_ioaRdr;

		
		if( NULL != a_edgeInfoDataMgr )
			m_edgeInfoDataMgr = a_edgeInfoDataMgr;
		else
		{
			m_edgeInfoDataMgr = new SimpleFilt1D2DataMgr< EdgeScan::EdgeInfo >( 
				a_ZCIndexDataRdr->GetCapacity() );
		}

		m_maxBetArr.SetCapacity( m_dif2Rdr->GetCapacity() );

		m_ZCIndexArr.SetCapacity( m_dif2Rdr->GetCapacity() );

		//if( NULL != a_out_2_DataMgr )
			//m_out_2_DataMgr = a_out_2_DataMgr;
		//else
		{
			m_out_2_DataMgr = new SimpleFilt1D2DataMgr< float >( 
				m_difRdr->GetCapacity() );
		}

		m_out_3_DataMgr = new SimpleFilt1D2DataMgr< float >( 
			m_difRdr->GetCapacity() );

		m_out_4_DataMgr = new SimpleFilt1D2DataMgr< float >( 
			m_difRdr->GetCapacity() );

		m_out_5_DataMgr = new SimpleFilt1D2DataMgr< float >( 
			m_difRdr->GetCapacity() );


		//Init( a_nAprSizOrg, a_dif2Rdr, a_outDataMgr, a_dmyVal );
		//Init( a_nAprSizOrg, NULL, a_outDataMgr, a_dmyVal );
		Init( 1, NULL, a_outDataMgr, a_dmyVal );
	}


	void PeakMarkerFilt1D2::SetupOutputMgr()
	{
		SingFilt1D2Base::SetupOutputMgr();

		const int nInpSiz = m_inpRdrArr.GetBack()->GetSize();

		m_out_2_DataMgr->SetSize( nInpSiz );
		m_out_2_DataMgr->SetMargins(
			//nMargBef, nMargAft );
			m_outDataMgr->GetMargBef(), m_outDataMgr->GetMargAft() );

		m_out_3_DataMgr->SetSize( nInpSiz );
		m_out_3_DataMgr->SetMargins(
			//nMargBef, nMargAft );
			m_outDataMgr->GetMargBef(), m_outDataMgr->GetMargAft() );

		m_out_4_DataMgr->SetSize( nInpSiz );
		m_out_4_DataMgr->SetMargins(
			//nMargBef, nMargAft );
			m_outDataMgr->GetMargBef(), m_outDataMgr->GetMargAft() );

		m_out_5_DataMgr->SetSize( nInpSiz );
		m_out_5_DataMgr->SetMargins(
			//nMargBef, nMargAft );
			m_outDataMgr->GetMargBef(), m_outDataMgr->GetMargAft() );

		
	}


	void PeakMarkerFilt1D2::Proceed()
	{
		int aa;


		ScanTryDbg::ScanTryDbgCore * pScanDbgCore = &ScanTryDbg::Core;

		bool bMySli = false;
		{
			Ns_ImgGradMgr::ScanLineItrRef sli = 
				pScanDbgCore->SliCur;

			if( NULL != sli )
			{
				if( sli->AreYou( 3, 11 ) )
	//				aa = aa;
					bMySli = true;
			}
		}


		FixedVector< EdgeScan::EdgeInfo > & rEdgeInfoArr = *m_pEdgeInfoArr;

		FixedVector< EdgeScan::PixInfo > & rPixInfoArr = *m_pPixInfoArr;

		int nEI_Bgn = rEdgeInfoArr.GetSize();

		SetupOutputMgr();

		PreparePVPoses();

		int * ioaBuf = m_ioaRdr->GetData();

		float * difBuf = m_difRdr->GetData();
		float * locBuf = m_locRdr->GetData();		
		float * dif2Buf = m_dif2Rdr->GetData();
		float * angDifBuf = m_angDifRdr->GetData();
		//float * ZCDataBuf = m_ZCDataRdr->GetData();
		float * ZCDataBuf = m_out_2_DataMgr->GetData();

		float * out_3_Buf = m_out_3_DataMgr->GetData();
		
		float * out_4_Buf = m_out_4_DataMgr->GetData();

		for( int k=0; k < m_out_4_DataMgr->GetSize(); k++ )
			out_4_Buf[ k ] = 0;
		

		float * out_5_Buf = m_out_5_DataMgr->GetData();

		for( int k=0; k < m_out_5_DataMgr->GetSize(); k++ )
			out_5_Buf[ k ] = 0;
		

		float * markBuf = ScanTryDbg::Core.GetMarkImg()->GetPixAt( 0, 0 );

				
		float * outBuf = m_outDataMgr->GetData();


		const int nBufSiz = m_outDataMgr->GetSize();
		
		const int nMargBef = m_outDataMgr->GetMargBef();
		const int nMargAft = m_outDataMgr->GetMargAft();

		//int nEI_Idx = 0;

		if( nMargBef + nMargAft >= nBufSiz )
		{
			for( int i=0; i < nBufSiz; i++ )
				outBuf[ i ] = m_dmyVal;
		}
		else
		{
			for( int i=0; i < nMargBef; i++ )
				outBuf[ i ] = m_dmyVal;

			for( int i = nBufSiz - nMargAft; i < nBufSiz; i++ )
				outBuf[ i ] = m_dmyVal;

			//const int nLocMargBef = m_nAprSiz / 2;
			//const int nLocMargAft = m_nAprSiz / 2;

			{
				const int nLim2 = nBufSiz - nMargAft;

				for( int i = nMargBef; i < nLim2; i++ )
				{
					outBuf[ i ] = 0;
				}

				//int * zciBuf = m_ZCIndexDataRdr->GetData();
				//const int nZciSiz = m_ZCIndexDataRdr->GetSize();

				int * zciBuf = & m_ZCIndexArr[ 0 ];
				const int nZciSiz = m_ZCIndexArr.GetSize();





				for( int i = 1, j = 0; i < nZciSiz - 1; i++, m_nGrandCnt++ )
				{

					if( 26 == i )
						i = i;

					if( bMySli && 
						1 == i )
						i = i;

					int zcIdx = zciBuf[ i ];

					const float zcVal = ZCDataBuf[ zcIdx ];

					if( zcVal < 10 )
						continue;

					j++;

					int nZcIdxBef = zciBuf[ i - 1 ];

					int nZcIdxAft = zciBuf[ i + 1 ];


					const float difVal = difBuf[ zcIdx ];

					const float difValBef = difBuf[ nZcIdxBef ];

					const float difValAft = difBuf[ nZcIdxAft ];



					out_4_Buf[ zcIdx ] = 120;
					{
						float net_Peak_Bef_Val = difVal - difValBef;

						int nEnd = nZcIdxBef;
						if( 1 == i )
							nEnd = 0;

						bool bDone60 = false;
						bool bDone35 = false;
						bool bDone15 = false;

						for( int k = zcIdx; k >= nEnd; k-- )
						{
							float netValK = difBuf[ k ] - difValBef;

							float relVal = 100 * netValK / net_Peak_Bef_Val;
							out_3_Buf[ k ] = relVal;

							if( ! bDone60 )
							{
								if( relVal <= 60 )
								{
									//out_4_Buf[ k ] = 100;
									out_4_Buf[ k ] = relVal + 20;
									 
									bDone60 = true;
								}
							}

							if( ! bDone35 )
							{
								if( relVal <= 35 )
								{
									//out_4_Buf[ k ] = 100;
									out_4_Buf[ k ] = relVal + 20;
									 
									bDone35 = true;
								}
							}

							if( ! bDone15 )
							{
								if( relVal <= 15 )
								{
									//out_4_Buf[ k ] = 100;
									out_4_Buf[ k ] = relVal + 20;
									 
									bDone15 = true;
								}
							}

						}
					}


					{
						float net_Peak_Aft_Val = difVal - difValAft;

						int nEnd = nZcIdxAft;
						if( nZciSiz - 2 == i )
							nEnd = m_difRdr->GetSize() - 1;

						bool bDone60 = false;
						bool bDone35 = false;
						bool bDone15 = false;

						for( int k = zcIdx; k <= nEnd; k++ )
						{
							float netValK = difBuf[ k ] - difValAft;

							float relVal = 100 * netValK / net_Peak_Aft_Val;
							out_3_Buf[ k ] = relVal;

							if( ! bDone60 )
							{
								if( relVal <= 60 )
								{
									//out_4_Buf[ k ] = 100;
									out_4_Buf[ k ] = relVal + 20;
									 
									bDone60 = true;
								}
							}

							if( ! bDone35 )
							{
								if( relVal <= 35 )
								{
									//out_4_Buf[ k ] = 100;
									out_4_Buf[ k ] = relVal + 20;
									 
									bDone35 = true;
								}
							}

							if( ! bDone15 )
							{
								if( relVal <= 15 )
								{
									//out_4_Buf[ k ] = 100;
									out_4_Buf[ k ] = relVal + 20;
									 
									bDone15 = true;
								}
							}

						}




					}



					if( 3449 == m_nGrandCnt )						
						i = i;


					outBuf[ zcIdx ] = zcVal;					

					outBuf[ nZcIdxBef ] = - zcVal;

					outBuf[ nZcIdxAft ] = - zcVal;


					//if( ! bBadEdge )
					{
						rEdgeInfoArr.IncSize();

						EdgeScan::EdgeInfo & rEI = rEdgeInfoArr.GetBack();

						//rEI.Index = nEI_Idx++;
						rEI.Index = pScanDbgCore->nEI_CntGlob++;
					

						rEI.EdgeIOL = zcIdx;
						//rEI.Edge_IOA = ioaBuf[ zcIdx ];

						rEI.EdgeVal = difVal;

						rEI.Root1_IOL = nZcIdxBef;
						//rEI.R1_IOA = ioaBuf[ nZcIdxBef ];

						int nR1_IOA = ioaBuf[ nZcIdxBef ];;

						rEI.Root2_IOL = nZcIdxAft;
						//rEI.R2_IOA = ioaBuf[ nZcIdxAft ];

						int nR2_IOA = ioaBuf[ nZcIdxAft ];;

						rEI.ioa_HeadPtr = ioaBuf;

						rEI.dif_HeadPtr = difBuf;

						rEI.IsCanceled = false;

						rEI.Nof_Bad_Merges = 0;

						if( NULL != pScanDbgCore->SliCur )
						{
							rEI.Dir = pScanDbgCore->SliCur->GetDirIndex();

							rEI.LineNum = pScanDbgCore->SliCur->GetLineNum();
						}

									

						
/*
						if( markBuf[ nR1_IOA ] > 1 )
						{
							pScanDbgCore->SliMark = 
								pScanDbgCore->SliCur;

							//pScanDbgCore->nIOA = rEI.R1_IOA;
							pScanDbgCore->nIOA = nR1_IOA;
						}

						if( markBuf[ nR2_IOA ] > 1 )
						{
							pScanDbgCore->SliMark = 
								pScanDbgCore->SliCur;

							//pScanDbgCore->nIOA = rEI.R2_IOA;
							pScanDbgCore->nIOA = nR2_IOA;
						}
*/
//uuuuuuuuuuuuu
/*

						difVal;

						const float difValBef = difBuf[ nZcIdxBef ];
	
						const float difValAft = difBuf[ nZcIdxAft ];


						{
							const float difNetBef = difVal - difValBef;

							bool bNotFound = true;

							for( int i = rEI.EdgeIOL - 1; i >= rEI.Root1_IOL; i-- )
							{
								const float valNetI = difBuf[ i ] - difValBef;

								float valSlope = valNetI / difNetBef;

								valSlope *= 100;

								int nIOA_I = ioaBuf[ i ];

								EdgeScan::PixInfo & rPI = rPixInfoArr[ nIOA_I ];

								if( valSlope > rPI.maxSlopeVal )
									rPI.maxSlopeVal = valSlope;

								if( valSlope < rPI.minSlopeVal )
									rPI.minSlopeVal = valSlope;

								if( bNotFound )
								{
									if( valSlope < 15 )
									{
										rEI.Root1_IOL = i;
										rEI.R1_IOA = nIOA_I;

										bNotFound = false;
									}
								}
							}

						}


						{
							const float difNetAft = difVal - difValAft;

							bool bNotFound = true;

							for( int i = rEI.EdgeIOL + 1; i <= rEI.Root2_IOL; i++ )
							{
								const float valNetI = difBuf[ i ] - difValAft;

								float valSlope = valNetI / difNetAft;

								valSlope *= 100;

								int nIOA_I = ioaBuf[ i ];

								EdgeScan::PixInfo & rPI = rPixInfoArr[ nIOA_I ];

								if( valSlope > rPI.maxSlopeVal )
									rPI.maxSlopeVal = valSlope;

								if( valSlope < rPI.minSlopeVal )
									rPI.minSlopeVal = valSlope;

								if( bNotFound )
								{
									if( valSlope < 15 )
									{
										rEI.Root2_IOL = i;
										rEI.R2_IOA = nIOA_I;

										bNotFound = false;
									}
								}
							}
						}

*/






					}













					i = i;

				}


			}


		}


		//for( int i = 0; i < nBufSiz; i++ )
		//{
		//	out_5_Buf[ i ] = 0;
		//}


		//int nEI_Lim = rEdgeInfoArr.GetSize();


		//for( int k = nEI_Bgn; k < nEI_Lim; k++ )
		//{
		//	EdgeScan::EdgeInfo & rEI = rEdgeInfoArr[ k ];


		//}


		//if( nEI_Lim > nEI_Bgn )
		//	nEI_Bgn = nEI_Bgn;


	
	}




	
	void PeakMarkerFilt1D2::PreparePVPoses()
	{
		m_ZCIndexArr.ResetSize();

		int * ioaBuf = m_ioaRdr->GetData();

		float * difBuf = m_difRdr->GetData();
		float * locBuf = m_locRdr->GetData();		
		float * dif2Buf = m_dif2Rdr->GetData();
		float * angDifBuf = m_angDifRdr->GetData();
		float * ZCDataBuf = m_ZCDataRdr->GetData();

		float * markBuf = ScanTryDbg::Core.GetMarkImg()->GetPixAt( 0, 0 );

				
		float * out_2_Buf = m_out_2_DataMgr->GetData();


		const int nBufSiz = m_outDataMgr->GetSize();
		
		const int nMargBef = m_outDataMgr->GetMargBef();
		const int nMargAft = m_outDataMgr->GetMargAft();

		if( nMargBef + nMargAft >= nBufSiz )
		{
			for( int i=0; i < nBufSiz; i++ )
				out_2_Buf[ i ] = m_dmyVal;
		}
		else
		{
			for( int i=0; i < nMargBef; i++ )
				out_2_Buf[ i ] = m_dmyVal;

			for( int i = nBufSiz - nMargAft; i < nBufSiz; i++ )
				out_2_Buf[ i ] = m_dmyVal;

			{
				const int nLim2 = nBufSiz - nMargAft;

				for( int i = nMargBef; i < nLim2; i++ )
				{
					out_2_Buf[ i ] = 0;
				}

				int nEnd = nMargBef;

				float endVal = difBuf[ nEnd ];

				int nMaxIdx = nMargBef;

				float maxVal = difBuf[ nMaxIdx ];

				float maxVal_Near = CalcNearMax( maxVal );				

				int nMinIdx = nMargBef;

				bool bGoodMax = false;


				//const float peakThr = 10;
				const float peakThr = 12;
				//const float peakThr = 17;
				//const float peakThr = 20;

				for( int i = nMargBef + 1; i < nLim2; i++ )
				{
					nEnd = i;

					endVal = difBuf[ nEnd ];

					if( endVal < difBuf[ nMinIdx ] )
					{
						nMinIdx = nEnd;
					}


					if( nMaxIdx < nMinIdx )
					{
						nMaxIdx = nMinIdx;
						maxVal = difBuf[ nMaxIdx ];

						
						if( maxVal > peakThr )
						{
							bGoodMax = true;
							maxVal_Near = CalcNearMax( maxVal );
						}
						else
							bGoodMax = false;
					}


					if( endVal > maxVal )
					{
						nMaxIdx = nEnd;
						maxVal = difBuf[ nMaxIdx ];						

						if( maxVal > peakThr )
						{
							bGoodMax = true;
							maxVal_Near = CalcNearMax( maxVal );
						}
						else
							bGoodMax = false;
					}

					//if( 128 == i )
					if( 108 == i )
						i = i;

					if( bGoodMax &&
						endVal < maxVal_Near )
					{
						bool bGoodBgn = false;
						int nBgn;

						//for( int j = nMinIdx; j < nMaxIdx; j++ )
						for( nBgn = nMinIdx; nBgn < nMaxIdx; nBgn++ )
						{
							float valJ = difBuf[ nBgn ];

							if( valJ >= maxVal_Near )
							{
								bGoodBgn = true;
								//nBgn = j;

								break;
							}
						}

						//if( bGoodBgn )
						{
							//for( int j = nBgn; j <= nEnd; j++ )
							{
							//	out_2_Buf[ j ] = 50;
							}
						}


						m_ZCIndexArr.PushBack( nMinIdx );
						out_2_Buf[ nMinIdx ] = - 50;

						m_ZCIndexArr.PushBack( nMaxIdx );
						out_2_Buf[ nMaxIdx ] = 50;

						nMinIdx = nEnd;
						bGoodMax = false;
					}

				}

				m_ZCIndexArr.PushBack( nMinIdx );
				out_2_Buf[ nMinIdx ] = - 50;
			}

		}



	}





}