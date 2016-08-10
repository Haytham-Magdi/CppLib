#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\AvgFilt1D2.h>



namespace Hcv
{
	using namespace Hcpl;
	using namespace Hcpl::Math;


	AvgFilt1D2::AvgFilt1D2( int a_nAprSizOrg, ISingFilt1D2DataReaderRef a_inpRdr,
			ISingFilt1D2DataMgrRef a_outDataMgr, float a_dmyVal ) 
	{
		Init( a_nAprSizOrg, a_inpRdr, a_outDataMgr, a_dmyVal );
	}

	void AvgFilt1D2::Proceed()
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

			float sum = 0;

			{
				const int nBgn = nMargBef - nLocMargBef;
				const int nLim1 = nBgn + m_nAprSiz;

				for( int i = nBgn; i < nLim1; i++ )
					sum += inpBuf[ i ];

				outBuf[ nMargBef ] = sum / m_nAprSiz;

				const int nLim2 = nBufSiz - nMargAft;

				for( int i = nMargBef + 1; i < nLim2; i++ )
				{
					const float valSub = inpBuf[ i - nLocMargBef - 1 ];
					const float valAdd = inpBuf[ i + nLocMargAft ];

					sum -= valSub;
					sum += valAdd;

					float val = sum / m_nAprSiz;

					outBuf[ i ] = val;
				}
			}


		}
	
	}


}