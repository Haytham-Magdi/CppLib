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
	}


	void PeakMarkerFilt1D2::Proceed()
	{
		int aa;


		ScanTryDbg::ScanTryDbgCore * pScanDbgCore = &ScanTryDbg::Core;

		bool bMySli = false;
		{
			ImgGradMgr::ScanLineItrRef sli = 
				pScanDbgCore->SliCur;

			if( NULL != sli )
			{
				if( sli->AreYou( 3, 11 ) )
	//				aa = aa;
					bMySli = true;
			}
		}


		SetupOutputMgr();

		PreparePVPoses();

		FixedVector< EdgeScan::EdgeInfo > & rEdgeInfoArr = *m_pEdgeInfoArr;

		FixedVector< EdgeScan::PixInfo > & rPixInfoArr = *m_pPixInfoArr;


		int * ioaBuf = m_ioaRdr->GetData();

		float * difBuf = m_difRdr->GetData();
		float * locBuf = m_locRdr->GetData();		
		float * dif2Buf = m_dif2Rdr->GetData();
		float * angDifBuf = m_angDifRdr->GetData();
		//float * ZCDataBuf = m_ZCDataRdr->GetData();
		float * ZCDataBuf = m_out_2_DataMgr->GetData();
		

		float * markBuf = ScanTryDbg::Core.GetMarkImg()->GetPixAt( 0, 0 );

				
		float * outBuf = m_outDataMgr->GetData();


		const int nBufSiz = m_outDataMgr->GetSize();
		
		const int nMargBef = m_outDataMgr->GetMargBef();
		const int nMargAft = m_outDataMgr->GetMargAft();

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


				//const float minAng = 40;
				//const float minAng = 30;
				const float minAng = 22;
				
				const float minUY = sin( minAng * M_PI / 180 );

				int nLastZcIdxAft = -1;

				typedef enum
				{
					NotOk = 0, MaybeOk, Ok
				}RootState;


				//for( int i = 0; i < nZciSiz; i++ )
				for( int i = 1, j = 0; i < nZciSiz - 1; i++, m_nGrandCnt++ )
				{
					//bool bMayBeOk = false;

					//if( 10 == i )
					//if( 4 == i )
					//if( 58 == i )
					//if( 27 == i )
					//if( 3 == i )
					if( 26 == i )
						i = i;

					if( bMySli && 
						1 == i )
						i = i;

					int zcIdx = zciBuf[ i ];

					if( 46 == zcIdx )
						i = i;

					//if( angDifBuf[ zcIdx ] > 20 )
					//if( angDifBuf[ zcIdx ] > 7 )
						//continue;

					const float zcVal = ZCDataBuf[ zcIdx ];

					if( zcVal < 10 )
						continue;

					j++;

					int nZcIdxBef = zciBuf[ i - 1 ];

					{
						const float zcValBef = ZCDataBuf[ nZcIdxBef ];

						if( zcValBef > - 10 )
							continue;						
					}


					int nZcIdxAft = zciBuf[ i + 1 ];

					{
						const float zcValAft = ZCDataBuf[ nZcIdxAft ];

						if( zcValAft > - 10 )
						{
							//bMayBeOk = false;
							continue;
						}
					}

					const float difVal = difBuf[ zcIdx ];

					if( difVal < 15 )
					{
						//bMayBeOk = false;
						//continue;
					}

					F32Point edgePnt( zcIdx, difVal );


					//RootState rtSateBef = RootState::MaybeOk;
					RootState rtSateBef = RootState::NotOk;

					//bool bOkBef = true;
					{
						//bool bTryAgain = true;

						int nTryCnt = 0;

						do
						{
							//rtSateBef = RootState::Ok;
							//bOkBef = true;

							if( nTryCnt > 0 )
							{
								const int nZcIdxBefN = zciBuf[ i - nTryCnt - 1 ];

								const float zcValBef = ZCDataBuf[ nZcIdxBefN ];

								if( zcValBef > - 10 ||
									nZcIdxBefN < nLastZcIdxAft )
								{
									//bOkBef = false;

									//bTryAgain = false;
									//continue;						

									//rtSateBef = RootState::MaybeOk;

									break;
								}

								nZcIdxBef = nZcIdxBefN;
							}

							const float difValBef = difBuf[ nZcIdxBef ];

							const float difRatio = ( difVal - difValBef ) / difVal;

							//if( difRatio < 0.2 )
							//if( difRatio < 0.3 )
							if( false )
							{
								//bOkBef = false;
								rtSateBef = RootState::MaybeOk;
								nTryCnt++;

								if( i - nTryCnt < 1 || nTryCnt > 5 )
									//bTryAgain = false;
									break;

								//if( difRatio < 0.07 &&
								if( difRatio < 0.15 &&
									//	Not Zero Valley
									ZCDataBuf[ nZcIdxBef ] < -40 )
								{
									rtSateBef = RootState::NotOk;
									//bMayBeOk = false;

									break;
								}
								else
								{
									//bMayBeOk = true;
								}

								continue;
							}


							F32Point edgePntBef( nZcIdxBef, difValBef );

							F32Point difPnt = F32Point::Sub( edgePnt, edgePntBef );

							const float difMag = difPnt.CalcMag();

							//if( difMag < 10 )
							//if( difMag < 7 )
							if( false )
							{
								rtSateBef = RootState::MaybeOk;

								//bOkBef = false;
								nTryCnt++;

								if( i - nTryCnt < 1 || nTryCnt > 5 )
									//bTryAgain = false;
									break;

								//bMayBeOk = true;

								continue;
							}

							F32Point difPntU = difPnt.DividBy( difMag );

							//if( fabs( difPntU.y ) < minUY )
							if( false )
							{
								//bOkBef = false;
								rtSateBef = RootState::MaybeOk;
								nTryCnt++;

								//if( i - nTryCnt < 1 || nTryCnt > 5 )
									//bTryAgain = false;
								break;

								//bMayBeOk = true;

								//continue;
							}

							rtSateBef = RootState::Ok;
						}
						//while( ! bOkBef && bTryAgain );
						while( RootState::MaybeOk == rtSateBef );						
					}

//uuuuuuuuuuuuuu

					//if( 175146 == m_nGrandCnt )
					//if( 175163 == m_nGrandCnt )						
					if( 3449 == m_nGrandCnt )						
						i = i;


					//RootState rtSateAft = RootState::MaybeOk;
					RootState rtSateAft = RootState::NotOk;

					//bool bOkAft = true;
					{
						//bool bTryAgain = true;

						int nTryCnt = 0;

						do
						{
							//rtSateAft = RootState::Ok;
							//bOkAft = true;

							if( nTryCnt > 0 )
							{
								const int nZcIdxAftN = zciBuf[ i + nTryCnt + 1 ];

								const float zcValAft = ZCDataBuf[ nZcIdxAftN ];

								if( zcValAft > - 10 ||
									nZcIdxAftN < nLastZcIdxAft )
								{
									//bOkAft = false;

									//bTryAgain = false;
									//continue;						

									//rtSateAft = RootState::MaybeOk;

									break;
								}

								nZcIdxAft = nZcIdxAftN;
							}

							const float difValAft = difBuf[ nZcIdxAft ];

							const float difRatio = ( difVal - difValAft ) / difVal;

							//if( difRatio < 0.2 )
							//if( difRatio < 0.3 )
							if( false )
							{
								//bOkAft = false;
								rtSateAft = RootState::MaybeOk;
								nTryCnt++;

								//if( i - nTryCnt < 1 || nTryCnt > 5 )
								if( i + nTryCnt + 2 > nZciSiz || nTryCnt > 5 )
									//bTryAgain = false;
									break;

								//if( difRatio < 0.07 &&
								if( difRatio < 0.15 &&
									//	Not Zero Valley
									ZCDataBuf[ nZcIdxAft ] < -40 )
								{
									rtSateAft = RootState::NotOk;
									//bMayBeOk = false;

									break;
								}
								else
								{
									//bMayBeOk = true;
								}

								continue;
							}


							F32Point edgePntAft( nZcIdxAft, difValAft );

							F32Point difPnt = F32Point::Sub( edgePnt, edgePntAft );

							const float difMag = difPnt.CalcMag();

							//if( difMag < 10 )
							//if( difMag < 7 )
							if( false )
							{
								rtSateAft = RootState::MaybeOk;

								//bOkAft = false;
								nTryCnt++;

								//if( i - nTryCnt < 1 || nTryCnt > 5 )
								if( i + nTryCnt + 2 > nZciSiz || nTryCnt > 5 )
									//bTryAgain = false;
									break;

								//bMayBeOk = true;

								continue;
							}

							F32Point difPntU = difPnt.DividBy( difMag );

							//if( fabs( difPntU.y ) < minUY )
							if( false )
							{
								//bOkAft = false;
								rtSateAft = RootState::MaybeOk;
								nTryCnt++;

								//if( i - nTryCnt < 1 || nTryCnt > 5 )
								//if( i + nTryCnt + 2 > nZciSiz || nTryCnt > 5 )
									//bTryAgain = false;
								break;

								//bMayBeOk = true;

								//continue;
							}

							rtSateAft = RootState::Ok;
						}
						//while( ! bOkAft && bTryAgain );
						while( RootState::MaybeOk == rtSateAft );						
					}


					RootState rtSateMin = ( rtSateBef < rtSateAft ) ? rtSateBef : rtSateAft;

					RootState rtSateMax = ( rtSateBef > rtSateAft ) ? rtSateBef : rtSateAft;


					if( RootState::NotOk == rtSateMin )
						continue;

					if( RootState::Ok != rtSateMax )
						continue;


					bool bBadEdge = false;
					

					const float valLoc = locBuf[ zcIdx ];


					float ratio = difVal / valLoc;

/*
					//if( ratio < 0.666 )
					if( ratio < 0.8 )
					{
						outBuf[ zcIdx ] = zcVal / 2;					
						bBadEdge = true;
					}
					else
					*/
						outBuf[ zcIdx ] = zcVal;					


					//{
						Hcpl_ASSERT( ZCDataBuf[ nZcIdxBef ] < -10 );

						const float difValBef = difBuf[ nZcIdxBef ];

						const float locValBef = locBuf[ nZcIdxBef ];

						ratio = 
							( valLoc - locValBef ) / ( difVal - difValBef );

						//if( ratio < 0.666 )
						if( false )
						{
							outBuf[ nZcIdxBef ] = - zcVal / 2;					
							bBadEdge = true;
						}
						else
						{
							if( locValBef > valLoc )
								i = i;

							outBuf[ nZcIdxBef ] = - zcVal;
						}
					//}

					//{
						Hcpl_ASSERT( ZCDataBuf[ nZcIdxAft ] < -10 );

						const float difValAft = difBuf[ nZcIdxAft ];

						const float locValAft = locBuf[ nZcIdxAft ];

						//float ratio = 
						ratio = 
							( valLoc - locValAft ) / ( difVal - difValAft );

						if( ratio < 0 )
							i = i;

						//if( ratio < 0.666 )
						if( false )
						{
							outBuf[ nZcIdxAft ] = - zcVal / 2;
							bBadEdge = true;
						}
						else
							outBuf[ nZcIdxAft ] = - zcVal;
					//}

						if( ! bBadEdge )
						{
							rEdgeInfoArr.IncSize();

							EdgeScan::EdgeInfo & rEI = rEdgeInfoArr.GetBack();

							rEI.EdgeIOL = zcIdx;
							rEI.Edge_IOA = ioaBuf[ zcIdx ];

							rEI.Root1_IOL = nZcIdxBef;
							rEI.R1_IOA = ioaBuf[ nZcIdxBef ];

							rEI.Root2_IOL = nZcIdxAft;
							rEI.R2_IOA = ioaBuf[ nZcIdxAft ];

							rEI.IsCanceled = false;
							

							if( markBuf[ rEI.R1_IOA ] > 1 )
							{
								pScanDbgCore->SliMark = 
									pScanDbgCore->SliCur;

								pScanDbgCore->nIOA = rEI.R1_IOA;
							}

							if( markBuf[ rEI.R2_IOA ] > 1 )
							{
								pScanDbgCore->SliMark = 
									pScanDbgCore->SliCur;

								pScanDbgCore->nIOA = rEI.R2_IOA;
							}

						}

					i = i;

//Continue:					
				}


			}


		}
	
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

						if( maxVal > 12 )
						//if( maxVal > 10 )
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

						if( maxVal > 12 )
						//if( maxVal > 10 )
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