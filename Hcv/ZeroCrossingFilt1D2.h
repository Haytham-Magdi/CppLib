#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <queue>      

//#include <Lib\Hcv\Filt1D2.h>
//#include <Lib\Hcv\Filt1D2DataMgr.h>

#include <Lib\Hcv\SingFilt1D2.h>


namespace Hcv
{
	using namespace Hcpl::Math;
	using namespace std;

	class ZeroCrossingFilt1D2 : public LinearSingFilt1D2Base
	{
	public:

		ZeroCrossingFilt1D2( int a_nAprSizOrg, ISingFilt1D2DataReaderRef a_dif2Rdr,
			//ISingFilt1D2DataReaderRef a_angDifRdr,
			IIntFilt1D2DataMgrRef a_ZCIndexDataMgr = NULL, 
			ISingFilt1D2DataMgrRef a_outDataMgr = NULL, float a_dmyVal = 0 );

		virtual void Proceed_0();
		virtual void Proceed();

		IIntFilt1D2DataReaderRef GetZCPosDataRdr()
		{
			//return (IIntFilt1D2DataReaderRef) m_ZCIndexDataMgr;
			return m_ZCIndexDataMgr;
		}


	protected:

		ISingFilt1D2DataReaderRef m_dif2Rdr;
		//ISingFilt1D2DataReaderRef m_angDifRdr;

		IIntFilt1D2DataMgrRef m_ZCIndexDataMgr;
	};

	typedef Hcpl::ObjRef< ZeroCrossingFilt1D2 > ZeroCrossingFilt1D2Ref;

}
