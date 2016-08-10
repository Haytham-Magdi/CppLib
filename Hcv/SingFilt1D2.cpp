#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\SingFilt1D2.h>



namespace Hcv
{
	using namespace Hcpl;
	using namespace Hcpl::Math;


	void SingFilt1D2Base::Init( int a_nAprSizOrg, ISingFilt1D2DataReaderRef a_inpRdr,
		ISingFilt1D2DataMgrRef a_outDataMgr, float a_dmyVal )
	{
		m_nProcCnt = 0;


		//Hcpl_ASSERT( NULL != a_inpRdr );
		m_inpRdr = a_inpRdr;
		
		if( NULL != a_inpRdr )
			m_inpRdrArr.PushBack( a_inpRdr );

		ValidateInpArr();

		//Hcpl_ASSERT( NULL != a_outDataMgr );

		IFilt1D2DataReaderSize * pRdrSize = m_inpRdrArr.GetBack();

		if( NULL != a_outDataMgr )
			m_outDataMgr = a_outDataMgr;
		else
		{
			m_outDataMgr = new SimpleFilt1D2DataMgr< float >( 
				pRdrSize->GetCapacity() );
		}


		m_nAprSizOrg = a_nAprSizOrg;

		m_nAprSiz = a_nAprSizOrg;

		SetAprSize( a_nAprSizOrg );

		m_dmyVal = a_dmyVal;
	}

	void SingFilt1D2Base::SetAprSize( int a_nAprSiz )
	{
		//Hcpl_ASSERT( 1 == a_nAprSiz % 2 );

		m_nAprSiz = a_nAprSiz;

		SetupOutputMgr();
	}

	void SingFilt1D2Base::SetupOutputMgr()
	{
		MaxFinder< float > maxFinderBef;
		MaxFinder< float > maxFinderAft;

		for( int i=0; i < m_inpRdrArr.GetSize(); i++ )
		{
			IFilt1D2DataReaderSize * inpRdr = m_inpRdrArr[ i ];

			maxFinderBef.AddValue( inpRdr->GetMargBef() );
			maxFinderAft.AddValue( inpRdr->GetMargAft() );
		}

		int nMargBef = maxFinderBef.FindMax() + m_nAprSiz / 2;

		int nMargAft = maxFinderAft.FindMax() + m_nAprSiz / 2;

		const int nInpSiz = m_inpRdrArr.GetBack()->GetSize();

		m_outDataMgr->SetSize( nInpSiz );
		m_outDataMgr->SetMargins( nMargBef, nMargAft );
	}


	void SingFilt1D2Base::ValidateInpArr()
	{
		const int nSizeOrg = m_inpRdrArr.GetBack()->GetSize();

		for( int i=0; i < m_inpRdrArr.GetSize(); i++ )
		{
			IFilt1D2DataReaderSize * inpRdr = m_inpRdrArr[ i ];

			if( NULL == inpRdr )
				ThrowHcvException();

			const int nSize = m_inpRdrArr[ i ]->GetSize();

			if( nSize != nSizeOrg )
				ThrowHcvException();
		}

	}

}