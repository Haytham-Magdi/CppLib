#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\AbsFilt1D2.h>



namespace Hcv
{
	using namespace Hcpl;
	using namespace Hcpl::Math;


	AbsFilt1D2::AbsFilt1D2( ISingFilt1D2DataReaderRef a_inpRdr,
			ISingFilt1D2DataMgrRef a_outDataMgr, float a_dmyVal ) 
	{
		Init( 1, a_inpRdr, a_outDataMgr, a_dmyVal );
	}

	void AbsFilt1D2::Proceed()
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

			{
				const int nLim2 = nBufSiz - nMargAft;

				for( int i = nMargBef; i < nLim2; i++ )
				{
					const float val = inpBuf[ i ];

					if( val >= 0 )
						outBuf[ i ] = val;
					else
						outBuf[ i ] = - val;
				}
			}


		}
	
	}


}