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

#include <Lib\Hcv\Filt1D2DataMgr.h>

//#include <Lib\Cpp\Common\MultiListQueMember.h>
#include <Lib\Cpp\Common\MultiListQueMgr.h>




namespace Hcv
{

	namespace EdgeScan
	{

		//class EdgeInfo : FRM_Object
		class EdgeInfo : public MultiListQueMember< EdgeInfo >			 
		{
		public:

			EdgeInfo()
			{
				IsIntersected = false;

				HasBadSrc_Trace = false;

				RootSpaceDist = 1000000;

				Act_Min_Idx = -1;

				Act_Max_Idx = -1;

				Scaled_EdgeVal = -1;

				_DoesConflict = true;


				StandDiv_R1 = -1;
				StandDiv_R2 = -1;
				StandDiv_R1_R2 = -1;
				StandDiv_E = -1;

				nMedDoneCnt = 0;
			}

			bool HasSameActs( EdgeInfo & a_rEI )
			{
				return

					( Act_Min_Idx == a_rEI.Act_Min_Idx ) &&
						( Act_Max_Idx == a_rEI.Act_Max_Idx );
			}
			

			void Set_DoesConflict( bool a_bArg )
			{
				//if( 36098 == Index )
				//if( 3114 == Index )
				if( 31353 == Index )
					Index = Index;

				_DoesConflict = a_bArg;
			}

			bool DoesConflict()
			{
				return _DoesConflict;
			}

			int Index;

			int EdgeIOL;

			float EdgeVal;

			float Scaled_EdgeVal;

			int Root1_IOL;

			int Med1_IOL;

			int InrR1_IOL;

			int Root2_IOL;

			int Med2_IOL;

			int InrR2_IOL;

			int Act_Min_Idx;

			int Act_Max_Idx;

			int * ioa_HeadPtr;
			float * dif_HeadPtr;

			bool IsCanceled;

			bool IsIntersected;

			float RootSpaceDist;

			bool HasBadSrc_Trace;

			int Dir;

			int LineNum;

			int Nof_Bad_Merges;

			float StandDiv_R1;
			float StandDiv_R2;
			float StandDiv_R1_R2;
			float StandDiv_E;

			int nMedDoneCnt;


		protected:

			bool _DoesConflict;
		};
		
		typedef Hcpl::ObjRef< IFilt1D2DataReader< EdgeInfo > > IEdgeInfoFilt1D2DataReaderRef;

		typedef Hcpl::ObjRef< IFilt1D2DataMgr< EdgeInfo > > IEdgeInfoFilt1D2DataMgrRef;



		class EdgeInfoAcc : public MultiListQueMember< EdgeInfoAcc >			 
		{
		public:

			EdgeInfoAcc()
			{
				pEI = NULL;
			}

			EdgeInfo * pEI;

		};


		class PixInfo : FRM_Object
		{
		public:

			PixInfo()
			{
				//maxSlopeVal = - 1000000;

				//minSlopeVal = 1000000;

				maxSlopeVal = 0;

				minSlopeVal = 100;

				nIOA_OfMax_EI_R = -1;
				nIOA_OfMax_EI_Med = -1;
			}

			int Get_IOA_OfMax_EI_R()
			{
				int nRet = nIOA_OfMax_EI_R;

				if( -1 == nIOA_OfMax_EI_R )
					nRet = this->nIndex;

				return nRet;
			}

			//int Get_IOA_OfMax_EI_Med()
			//{
			//	int nRet = nIOA_OfMax_EI_Med;

			//	if( -1 == nIOA_OfMax_EI_Med )
			//		nRet = this->nIndex;

			//	return nRet;
			//}

		public:

			int nIndex;

			float maxSlopeVal;
			float minSlopeVal;

			int nIOA_OfMax_EI_R;
			int nIOA_OfMax_EI_Med;

			int nIOA_VeryDeep_R;
			int nIOA_VeryDeep_Med;

			EdgeScan::EdgeInfo * pEI_VeryDeep;
		};


		class ScanDirMgrExt : FRM_Object
		{
		public:

			//ScanDirMgrExt( ScanDirMgrRef a_scanMgr );
			ScanDirMgrExt( ScanDirMgr & a_rScanMgr );

			void PrepareSelf( ScanDirMgr & a_rScanMgr );

			float * Get_difHeadAt( int a_nIndex )
			{
				return m_difArrColl.GetHeadAt( a_nIndex );
			}

			int Get_difSizeAt( int a_nIndex )
			{
				return m_difArrColl.GetSizeAt( a_nIndex );
			}

			int Get_difDataSizeTot()
			{
				return m_difArrColl.GetDataSizeTot();
			}

			int Get_difNofHeads()
			{
				return m_difArrColl.GetNofHeads();
			}


			int GetDir()
			{
				return m_nDir;
			}
			
			int GetDirNorm(int a_nofDirs)
			{
				m_nDirNorm = 
					( m_nDir + ( a_nofDirs / 2 ) ) % a_nofDirs;

				return m_nDirNorm;
			}


			FixedVector< float > m_difGlobArr;

			FixedVector< float > m_slopeGlobArr;

			FixedVector< EdgeInfo * > m_edgeInfo_GlobArr;

			int nEI_Bgn;
			int nEI_Lim;

		protected:

			//ScanDirMgrRef m_scanMgr;

			int m_nDir;
			int m_nDirNorm;

			CommonArrCollBuff< float > m_difArrColl;

		};

		typedef Hcpl::ObjRef< ScanDirMgrExt > ScanDirMgrExtRef;


		class EdgeProcAction : public MultiListQueMember< EdgeProcAction >
		{
		public:

			//EdgeProcAction


		public:
			EdgeInfo * pEdgeInfo;
		};

	}

}