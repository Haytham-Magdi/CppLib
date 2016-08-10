#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\ZeroCrossing_2_Filt1D2.h>



namespace Hcv
{
	using namespace Hcpl;
	using namespace Hcpl::Math;


	ZeroCrossing_2_Filt1D2::ZeroCrossing_2_Filt1D2( int a_nAprSizOrg, ISingFilt1D2DataReaderRef a_dif2Rdr,
			//ISingFilt1D2DataReaderRef a_angDifRdr,
			IIntFilt1D2DataMgrRef a_ZCIndexDataMgr, 
			ISingFilt1D2DataMgrRef a_outDataMgr, float a_dmyVal ) 
	{
		m_dif2Rdr = a_dif2Rdr;
		m_inpRdrArr.PushBack( m_dif2Rdr );

		//m_angDifRdr = a_angDifRdr;
		//m_inpRdrArr.PushBack( m_angDifRdr );


		if( NULL != a_ZCIndexDataMgr )
			m_ZCIndexDataMgr = a_ZCIndexDataMgr;
		else
		{
			m_ZCIndexDataMgr = new SimpleFilt1D2DataMgr< int >( 
				a_dif2Rdr->GetCapacity() );
		}


		//Init( a_nAprSizOrg, a_dif2Rdr, a_outDataMgr, a_dmyVal );
		Init( a_nAprSizOrg, NULL, a_outDataMgr, a_dmyVal );
	}

	void ZeroCrossing_2_Filt1D2::Proceed_0()
	{
		SetupOutputMgr();

		float * dif2Buf = m_dif2Rdr->GetData();
		//float * angDifBuf = m_angDifRdr->GetData();
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

			int * zciBuf = m_ZCIndexDataMgr->GetData();
			int nZciCnt = 0;

			const int nLocMargBef = m_nAprSiz / 2;
			const int nLocMargAft = m_nAprSiz / 2;

			{
				const int nLim2 = nBufSiz - nMargAft;

				typedef enum
				{
					Up,
					Down,
					Zero,
					None
				}ValType;


				ValType vtCur = ValType::None;
				ValType vtOld = ValType::None;


				AvgFinder< float > avgFnd;

				{
					const int nZCI = nMargBef;
					
					zciBuf[ nZciCnt++ ] = nZCI;
						
					outBuf[ nZCI ] = - 25;
				}


				for( int i = nMargBef + 1, nOldI = -1; i < nLim2; i++ )
				{
					vtOld = vtCur;

					outBuf[ i ] = 0;

					//if( nLim2 / 2 == i )
					if( 46 == i )
						i = i;

					const float valDif2 = dif2Buf[ i ];
					//const float valAngDif = angDifBuf[ i ];

					const float valDif2Bef = dif2Buf[ i - nLocMargBef ];
					const float valDif2Aft = dif2Buf[ i + nLocMargAft ];


					//if( 0 == valDif2 && 0 == valAngDif )

					//if( dif2Buf[ i - 1 ] >= 0 && 
						//dif2Buf[ i + 1 ] <= 0 && 

					//if( dif2Buf[ i - 1 ] > 0 && 
						//dif2Buf[ i + 1 ] < 0 && 

					//if( dif2Buf[ i - nLocMargBef ] > 0 && 
						//dif2Buf[ i + nLocMargAft ] < 0 && 

					if( valDif2Bef * valDif2Aft < 0 )
					{
						//if( fabs( valDif2Bef ) < nLocMargBef * 2 ||
							//fabs( valDif2Aft ) < nLocMargAft * 2
						if( fabs( valDif2Bef ) < 1 ||
							fabs( valDif2Aft ) < 1
							)
						{
							//outBuf[ i ] = 0;
							//continue;
						}


						//if( nOldI != i - 1 )
						//if( i - nOldI > 4 )
							//zciBuf[ nZciCnt++ ] = i;

						nOldI = i;

						if( valDif2Bef > valDif2Aft )
						{
							vtCur = ValType::Up;
							//valRes = 50;
						}
						else
						{
							vtCur = ValType::Down;
							//valRes = - 50;
						}


						{
							//if( valAngDif >= 25 )
								//valRes /= 2;
						}
					}
					//else if( 0 == valDif2 )
					else if( fabs( valDif2 ) < 0.3 )
					{
						if( ValType::Up == vtOld || ValType::Down == vtOld )
							vtCur = vtOld;
						else
							vtCur = ValType::Zero;
					}
					else
					{
						vtCur = ValType::None;
						//valRes = 0;
					}

					if( ValType::Up == vtOld &&
						ValType::Up != vtCur )
					{
						const int nZCI = (int)( avgFnd.FindAvg() + 0.5555 );
						
						zciBuf[ nZciCnt++ ] = nZCI;
							
						outBuf[ nZCI ] = 50;

						avgFnd.Reset();
					}
					else if( ValType::Down == vtOld &&
						ValType::Down != vtCur ) 						
					{
						const int nZCI = (int)( avgFnd.FindAvg() + 0.5555 );
						
						zciBuf[ nZciCnt++ ] = nZCI;
							
						outBuf[ nZCI ] = -50;

						avgFnd.Reset();
					}

					if( ValType::Zero == vtOld && ValType::Zero != vtCur )
					{
						outBuf[ i - 1 ] = - 25;
						zciBuf[ nZciCnt++ ] = i - 1;
					}
					else if( ValType::Zero != vtOld && ValType::Zero == vtCur )
					{
						outBuf[ i ] = - 25;
						zciBuf[ nZciCnt++ ] = i;
					}


					if( ValType::Up == vtCur ||
						ValType::Down == vtCur )
					{
						avgFnd.AddValue( i );
					}
					else if( ValType::Zero == vtCur )
					{
						//outBuf[ i ] = - 25;
					}
					else if( ValType::None == vtCur )
					{
						outBuf[ i ] = 0;
					}
					else
					{
						ThrowHcvException();
					}

					//float valRes = 0;
					//outBuf[ i ] = valRes;
				}

				{
					const int nZCI = nLim2 - 1;
					
					zciBuf[ nZciCnt++ ] = nZCI;
						
					outBuf[ nZCI ] = - 25;
				}

			}


			m_ZCIndexDataMgr->SetSize( nZciCnt );
		}
	
	}


	void ZeroCrossing_2_Filt1D2::Proceed()
	{
		SetupOutputMgr();

		float * dif2Buf = m_dif2Rdr->GetData();
		//float * angDifBuf = m_angDifRdr->GetData();
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

			int * zciBuf = m_ZCIndexDataMgr->GetData();
			int nZciCnt = 0;

			const int nLocMargBef = m_nAprSiz / 2;
			const int nLocMargAft = m_nAprSiz / 2;

			{
				const int nLim2 = nBufSiz - nMargAft;

				typedef enum
				{
					Up,
					Down,
					Zero,
					None
				}ValType;


				ValType vtCur = ValType::None;
				ValType vtOld = ValType::None;


				AvgFinder< float > avgFnd;

				{
					const int nZCI = nMargBef;
					
					zciBuf[ nZciCnt++ ] = nZCI;
						
					outBuf[ nZCI ] = - 25;
				}


				for( int i = nMargBef + 1, nOldI = -1; i < nLim2; i++ )
				{
					vtOld = vtCur;

					outBuf[ i ] = 0;

					//if( nLim2 / 2 == i )
					if( 46 == i )
						i = i;

					const float valDif2 = dif2Buf[ i ];
					//const float valAngDif = angDifBuf[ i ];

					const float valDif2Bef = dif2Buf[ i - nLocMargBef ];
					const float valDif2Aft = dif2Buf[ i + nLocMargAft ];


					//if( 0 == valDif2 && 0 == valAngDif )

					//if( dif2Buf[ i - 1 ] >= 0 && 
						//dif2Buf[ i + 1 ] <= 0 && 

					//if( dif2Buf[ i - 1 ] > 0 && 
						//dif2Buf[ i + 1 ] < 0 && 

					//if( dif2Buf[ i - nLocMargBef ] > 0 && 
						//dif2Buf[ i + nLocMargAft ] < 0 && 

					if( valDif2Bef * valDif2Aft < 0 )
					{
						nOldI = i;

						if( valDif2Bef > valDif2Aft )
						{
							vtCur = ValType::Up;

							{
								outBuf[ i ] = 50;
								zciBuf[ nZciCnt++ ] = i;
							}
						}
						else
						{
							vtCur = ValType::Down;

							{
								outBuf[ i ] = -50;
								zciBuf[ nZciCnt++ ] = i;
							}
						}
					}
					//else if( fabs( valDif2 ) < 0.3 )
					else if( fabs( valDif2 ) < 1.25 )
					{
						//if( ValType::Up == vtOld || ValType::Down == vtOld )
							//vtCur = vtOld;
						//else
							vtCur = ValType::Zero;

						{
							outBuf[ i ] = -25;
							zciBuf[ nZciCnt++ ] = i;
						}
					}
					else
					{
						vtCur = ValType::None;
						//valRes = 0;
					}

/*
					if( ValType::Up == vtOld &&
						ValType::Up != vtCur )
					{
						const int nZCI = (int)( avgFnd.FindAvg() + 0.5555 );
						
						zciBuf[ nZciCnt++ ] = nZCI;
							
						outBuf[ nZCI ] = 50;

						avgFnd.Reset();
					}
					else if( ValType::Down == vtOld &&
						ValType::Down != vtCur ) 						
					{
						const int nZCI = (int)( avgFnd.FindAvg() + 0.5555 );
						
						zciBuf[ nZciCnt++ ] = nZCI;
							
						outBuf[ nZCI ] = -50;

						avgFnd.Reset();
					}

					if( ValType::Zero == vtOld && ValType::Zero != vtCur )
					{
						outBuf[ i - 1 ] = - 25;
						zciBuf[ nZciCnt++ ] = i - 1;
					}
					else if( ValType::Zero != vtOld && ValType::Zero == vtCur )
					{
						outBuf[ i ] = - 25;
						zciBuf[ nZciCnt++ ] = i;
					}


					if( ValType::Up == vtCur ||
						ValType::Down == vtCur )
					{
						avgFnd.AddValue( i );
					}
					else if( ValType::Zero == vtCur )
					{
						//outBuf[ i ] = - 25;
					}
					else if( ValType::None == vtCur )
					{
						outBuf[ i ] = 0;
					}
					else
					{
						ThrowHcvException();
					}
*/

					//float valRes = 0;
					//outBuf[ i ] = valRes;
				}

				{
					const int nZCI = nLim2 - 1;
					
					zciBuf[ nZciCnt++ ] = nZCI;
						
					outBuf[ nZCI ] = - 25;
				}

			}


			m_ZCIndexDataMgr->SetSize( nZciCnt );
		}
	
	}





}