#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\ImageNbrMgr.h>

namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;


	ImageNbrMgr::ImageNbrMgr( ImageItrMgrRef a_srcItr, int a_nAprSiz )
	{
		m_srcItr = a_srcItr;
		m_nAprSiz = a_nAprSiz;

		Prepare();
	}

	FixedVector< ImageNbrMgr::PixIndexNbr > & 
		ImageNbrMgr::GetNbr_Arr_ByNormIdx( int a_nNormIdx )
	{	
		if( 0 == a_nNormIdx )
		{
			return m_nbr_H_Arr;
		}
		else if( 1 == a_nNormIdx )
		{
			return m_nbr_V_Arr;
		}
		else
		{
			ThrowHcvException();
			return m_nbr_H_Arr;
		}
	}


	void ImageNbrMgr::Prepare()
	{
		//	H 
		PrepareNbrArr( m_nbr_H_Arr, m_srcItr->GetItrProvAt( 0 ) );

		//	V 
		PrepareNbrArr( m_nbr_V_Arr, m_srcItr->GetItrProvAt( 1 ) );
	}

	void ImageNbrMgr::PrepareNbrArr( FixedVector< PixIndexNbr > & a_nbrArr,
		ImageLineItrProvider & a_prov )
	{
		CvSize imgSiz = m_srcItr->GetImgSiz();
		int nImgSiz1D = imgSiz.width * imgSiz.height;

		int nMarg = m_nAprSiz / 2;

		{
			FixedVector< PixIndexNbr > & rNbrArr = a_nbrArr;

			rNbrArr.SetSize( nImgSiz1D );

			ImageLineItrProvider & rProv = a_prov;
				

			FixedVector< ImageLineItr > & rLineItrArr = 
				rProv.GetLineItrArr();
			
			for( int i=0; i < rLineItrArr.GetSize(); i++ )
			{
				ImageLineItr & rLineItr = rLineItrArr[ i ];

				NumberIterator itr_BgnEnd = rLineItr.GenBgnToEndItr();


				if( itr_BgnEnd.GetLimStep() < m_nAprSiz )
				{

					{
						NumberIterator itr = rLineItr.GenItr( 
							rLineItr.GetFarBgn(), rLineItr.GetFarEnd() );

						for( int j = itr.CalcCurStep(); 
							j < itr.GetLimStep(); j++, itr.MoveNext() )
						{
							PixIndexNbr & rPin = rNbrArr[ itr.GetCurVal() ];

							rPin.nIdxBef = -1;
							rPin.nIdxAft = -1;
						}			
					}

					//if( itr_BgnEnd.GetLimStep() > nMarg )
					{

						{
							NumberIterator itr = rLineItr.GenItr( 
								rLineItr.GetBgn(), rLineItr.GetAftEnd( - nMarg ) );

							for( int j = itr.CalcCurStep(); 
								j < itr.GetLimStep(); j++, itr.MoveNext() )
							{
								PixIndexNbr & rPin = rNbrArr[ itr.GetCurVal() ];

								rPin.nIdxBef = -1;

								rPin.nIdxAft = itr.GetCurVal() +								
									nMarg * itr.GetStepVal();
							}			
						}

						{
							NumberIterator itr = rLineItr.GenItr( 
								rLineItr.GetAftBgn( nMarg ), rLineItr.GetEnd() );

							for( int j = itr.CalcCurStep(); 
								j < itr.GetLimStep(); j++, itr.MoveNext() )
							{
								PixIndexNbr & rPin = rNbrArr[ itr.GetCurVal() ];

								rPin.nIdxBef = itr.GetCurVal() -
									nMarg * itr.GetStepVal();

								rPin.nIdxAft = -1;
							}			
						}

					}

					//	End: if( itr_BgnEnd.GetLimStep() < m_nAprSiz )
				}
				else
				{
					{
						NumberIterator itr = rLineItr.GenItr( 
							rLineItr.GetFarBgn(), rLineItr.GetAftBgn( -1 ) );

						for( int j = itr.CalcCurStep(); 
							j < itr.GetLimStep(); j++, itr.MoveNext() )
						{
							PixIndexNbr & rPin = rNbrArr[ itr.GetCurVal() ];

							rPin.nIdxBef = -1;
							rPin.nIdxAft = -1;
						}			
					}

					{
						NumberIterator itr = rLineItr.GenItr( 
							rLineItr.GetBgn(), rLineItr.GetAftBgn( nMarg - 1 ) );

						for( int j = itr.CalcCurStep(); 
							j < itr.GetLimStep(); j++, itr.MoveNext() )
						{
							PixIndexNbr & rPin = rNbrArr[ itr.GetCurVal() ];

							rPin.nIdxBef = -1;

							rPin.nIdxAft = itr.GetCurVal() +								
								nMarg * itr.GetStepVal();
						}			
					}

////////////////////////////////////
//--------------------------------

					{
						NumberIterator itr = rLineItr.GenItr( 
							rLineItr.GetAftBgn( nMarg ), 
							rLineItr.GetAftEnd( - nMarg ) );

						for( int j = itr.CalcCurStep(); 
							j < itr.GetLimStep(); j++, itr.MoveNext() )
						{
							PixIndexNbr & rPin = rNbrArr[ itr.GetCurVal() ];

							rPin.nIdxBef = itr.GetCurVal() -
								nMarg * itr.GetStepVal();

							rPin.nIdxAft = itr.GetCurVal() +
								nMarg * itr.GetStepVal();
						}			
					}

//--------------------------------
////////////////////////////////////

					{
						NumberIterator itr = rLineItr.GenItr( 
							rLineItr.GetAftEnd( - ( nMarg - 1 ) ), rLineItr.GetEnd() );

						for( int j = itr.CalcCurStep(); 
							j < itr.GetLimStep(); j++, itr.MoveNext() )
						{
							PixIndexNbr & rPin = rNbrArr[ itr.GetCurVal() ];

							rPin.nIdxBef = itr.GetCurVal() -
								nMarg * itr.GetStepVal();

							rPin.nIdxAft = -1;
						}			
					}

					{
						NumberIterator itr = rLineItr.GenItr( 
							rLineItr.GetAftEnd( 1 ), rLineItr.GetFarEnd() );

						for( int j = itr.CalcCurStep(); 
							j < itr.GetLimStep(); j++, itr.MoveNext() )
						{
							PixIndexNbr & rPin = rNbrArr[ itr.GetCurVal() ];

							rPin.nIdxBef = -1;
							rPin.nIdxAft = -1;
						}			
					}
				}	//	else




			}


		}


	}


}