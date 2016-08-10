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

#include <Lib\Hcv\EdgeScan.h>


namespace Hcv
{
	using namespace Hcpl::Math;
	using namespace std;

	class PeakMarkerFilt1D2_2 : public LinearSingFilt1D2Base
	{
	public:

		PeakMarkerFilt1D2_2( int a_nAprSizOrg, 
			ISingFilt1D2DataReaderRef a_difRdr,
			ISingFilt1D2DataReaderRef a_stDevRdr,
			ISingFilt1D2DataReaderRef a_locRdr,
			ISingFilt1D2DataReaderRef a_dif2Rdr,
			ISingFilt1D2DataReaderRef a_angDifRdr,
			ISingFilt1D2DataReaderRef a_ZCDataRdr,
			IIntFilt1D2DataReaderRef a_ZCIndexDataRdr,
			IIntFilt1D2DataReaderRef a_ioaRdr,
			FixedVector< EdgeScan::EdgeInfo > & a_edgeInfoArr,
			FixedVector< EdgeScan::PixInfo > & a_pixInfoArr,
			EdgeScan::IEdgeInfoFilt1D2DataMgrRef a_edgeInfoDataMgr = NULL,
			ISingFilt1D2DataMgrRef a_outDataMgr = NULL, float a_dmyVal = 0 );

		virtual void Proceed_0();

		virtual void Proceed();

		ISingFilt1D2DataReaderRef GetOutput_2_Reader()
		{
			return m_out_2_DataMgr;
		}

		ISingFilt1D2DataReaderRef GetOutput_3_Reader()
		{
			return m_out_3_DataMgr;
		}

		ISingFilt1D2DataReaderRef GetOutput_4_Reader()
		{
			return m_out_4_DataMgr;
		}
		
		ISingFilt1D2DataReaderRef GetOutput_5_Reader()
		{
			return m_out_5_DataMgr;
		}
		

	protected:

		void PreparePVPoses_0();
		void PreparePVPoses_02();
		void PreparePVPoses();

		float CalcNearMax( float a_maxVal )
		{
			float subVal = a_maxVal - 14;

			float multVal = 0.7 * a_maxVal;

			float retVal;

			if( a_maxVal > 20 )			
				retVal = ( multVal > subVal ) ? multVal : subVal;
			else
				retVal = 5;

			return retVal;
		}

		virtual void SetupOutputMgr();

	protected:

		ISingFilt1D2DataReaderRef m_difRdr;
		ISingFilt1D2DataReaderRef m_stDevRdr;
		ISingFilt1D2DataReaderRef m_locRdr;
		ISingFilt1D2DataReaderRef m_dif2Rdr;
		ISingFilt1D2DataReaderRef m_angDifRdr;
		ISingFilt1D2DataReaderRef m_ZCDataRdr;

		IIntFilt1D2DataReaderRef m_ZCIndexDataRdr;
		IIntFilt1D2DataReaderRef m_ioaRdr;

		int m_nGrandCnt;

		int m_nDifAprSiz;

		EdgeScan::IEdgeInfoFilt1D2DataMgrRef m_edgeInfoDataMgr;

		FixedVector< float > m_maxBetArr;

		FixedVector< EdgeScan::EdgeInfo > * m_pEdgeInfoArr;

		FixedVector< EdgeScan::PixInfo > * m_pPixInfoArr;

		ISingFilt1D2DataMgrRef m_out_2_DataMgr;

		ISingFilt1D2DataMgrRef m_out_3_DataMgr;		

		ISingFilt1D2DataMgrRef m_out_4_DataMgr;		

		ISingFilt1D2DataMgrRef m_out_5_DataMgr;		

		FixedVector< int > m_ZCIndexArr;
	};

	typedef Hcpl::ObjRef< PeakMarkerFilt1D2_2 > PeakMarkerFilt1D2_2Ref;

}
