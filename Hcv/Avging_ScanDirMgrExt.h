#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

#include <Lib\Hcv\ScanDirMgr.h>
#include <Lib\Hcv\ScanDirMgrColl.h>
#include <Lib\Hcv\Signal1D.h>

#include <Lib\Hcv\IImgDataMgr.h>
#include <Lib\Hcv\ImgGradMgr.h>




namespace Hcv
{
	//using namespace Hcpl::Math;

	namespace Ns_ImgAvgingMgr
	{

		class AvgingScan_QueElm : public MultiListQueMember< AvgingScan_QueElm >
		{
		public:
			
			int nIdx_Bef;
			int nIdx_Cur;
			int nIdx_Aft;

			int nDir;

			int ScaledVal;
		};


		class Avging_ScanDirMgrExt : FRM_Object
		{
		public:

			//Avging_ScanDirMgrExt( ScanDirMgrRef a_scanMgr );
			Avging_ScanDirMgrExt( ScanDirMgr & a_rScanMgr, int a_nofDirs );

			void PrepareSelf( ScanDirMgr & a_rScanMgr, int a_nofDirs );

			float * Get_avgHeadAt( int a_nIndex )
			{
				return m_avgArrColl.GetHeadAt( a_nIndex );
			}

			int Get_avgSizeAt( int a_nIndex )
			{
				return m_avgArrColl.GetSizeAt( a_nIndex );
			}

			int Get_avgDataSizeTot()
			{
				return m_avgArrColl.GetDataSizeTot();
			}

			int Get_avgNofHeads()
			{
				return m_avgArrColl.GetNofHeads();
			}


			int GetDir()
			{
				return m_nDir;
			}
			
			int GetDirNorm()
			{
				return m_nDirNorm;
			}


			FixedVector< AvgingScan_QueElm > m_avgGlobArr;

			//FixedVector< float > m_slopeGlobArr;

			//FixedVector< EdgeInfo * > m_edgeInfo_GlobArr;

			int nEI_Bgn;
			int nEI_Lim;

		protected:

			//ScanDirMgrRef m_scanMgr;

			int m_nDir;
			int m_nDirNorm;

			int m_nofDirs;

			CommonArrCollBuff< float > m_avgArrColl;

		};


		typedef Hcpl::ObjRef< Avging_ScanDirMgrExt > Avging_ScanDirMgrExtRef;


	}


}

