#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\MaxFilt1D2.h>



namespace Hcv
{
	using namespace Hcpl;
	using namespace Hcpl::Math;


	MaxFilt1D2::MaxFilt1D2( int a_nAprSizOrg, ISingFilt1D2DataReaderRef a_inpRdr,
			ISingFilt1D2DataMgrRef a_outDataMgr, float a_dmyVal ) 
	{
		Init( a_nAprSizOrg, a_inpRdr, a_outDataMgr, a_dmyVal );
	}

	void MaxFilt1D2::Proceed()
	{
		SetupOutputMgr();

		float * inpBuf = m_inpRdr->GetData();
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

			const int nLocMargBef = m_nAprSiz / 2;
			const int nLocMargAft = m_nAprSiz / 2;


			{
				const int nLim2 = nBufSiz - nMargAft;

				MaxFinder< float > maxFnd;

				for( int i = nMargBef; i < nLim2; i++ )
				{
					//const float valBef = inpBuf[ i - nLocMargBef ];
					//const float valAft = inpBuf[ i + nLocMargAft ];

					maxFnd.Reset();

					const int nBgnJ = i - nLocMargBef;
					const int nLimJ = i + nLocMargAft + 1;

					for( int j = nBgnJ; j < nLimJ; j++ )
					{
						maxFnd.AddValue( inpBuf[ j ] );
					}

					outBuf[ i ] = maxFnd.FindMax();
				}
			}


		}
	
	}


}