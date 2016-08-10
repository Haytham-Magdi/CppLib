#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>


#include <Lib\Hcv\EdgeScan.h>

#include <Lib\Hcv\EdgeScanMgr.h>

#include <Lib\Hcv\Ptr_Filt1D2DataMgr.h>

#include <Lib\Hcv\Filt1D2DataMgr.h>

#include <Lib\Hcv\ImgGradMgr_2.h>
#include <Lib\Hcv\ImgAvgingMgr.h>


#include <Lib\Hcv\Filt1D2System.h>

#include <Lib\Hcv\RgnSegmDiameter.h>



namespace Hcv
{
	//using namespace Hcpl::Math;


	class ScanTryMgr_8 : public virtual EdgeScanMgr
	{
	public:


	protected:


	public:
		
		ScanTryMgr_8( int a_x, int a_y );

//--		ScanTryMgr_8( F32ImageRef a_src );
		ScanTryMgr_8( IImgDataMgrRef a_src, int a_nDiameter );
		

		FixedVector< EdgeScan::EdgeInfo > & GetEdgeInfoArr()
		{
			return m_edgeInfoArr;
		}

		FixedVector< EdgeScan::PixInfo > & GetPixInfoArr()
		{
			return m_pixInfoArr;
		}

		FixedVector< float > & GetMaxDif_Arr()
		{
			return m_maxDifArr;
		}



		FixedVector< EdgeScan::ScanDirMgrExtRef > & GetScanDirExtArr()
		{
			return m_scanDirExtArr;
		}


		float * GetStandDiv_Buf()
		{
			return m_standDiv_Buf;
		}

		int GetDiamCore()
		{
			return m_rsDiam.GetDiamCore();

			//return ( m_nDiameter - 2 ) / 3;
		}

		int GetDiamInr_1()
		{
			return m_rsDiam.GetDiamInr_1();

			//int nInr_1 = GetDiamCore() * 2 + 1;
			//return nInr_1;
		}
		  
		int GetDiamInr_2()
		{
			return m_rsDiam.GetDiamInr_2();

			//int nInr_2 = m_nDiameter - GetDiamInr_1() + 1;
			//return nInr_2;
		}


	protected:

		//void ValidateDiameter( int a_nDiameter );
		void ValidateDiameter( );


		virtual DataDraw * AddSigDraw( Signal1DRef a_sig, bool a_bDraw = false, 
			U8ColorVal a_color = u8ColorVal( 0, 0, 0 ), 
			int a_shiftX = 0, int a_shiftY = 0, float a_scale = 1 )
		{
			return EdgeScanMgr::AddSigDraw( a_sig, a_bDraw, a_color, 
				a_shiftX, a_shiftY, a_scale );
		}


		virtual DataDraw * AddSigDraw( ISingFilt1D2DataReaderRef a_dataRdr, bool a_bDraw = false, 
			U8ColorVal a_color = u8ColorVal( 0, 0, 0 ), 
			int a_shiftX = 0, int a_shiftY = 0, float a_scale = 1 )
		{
			int nDataSiz = a_dataRdr->GetSize();

			return EdgeScanMgr::AddSigDraw( a_dataRdr, a_bDraw, a_color, 
				a_shiftX, a_shiftY, a_scale );
		}


		virtual DataDraw * AddSigDraw( ISingFilt1D2Ref a_filt, bool a_bDraw = false, 
		//DataDraw * AddSigDraw( ISingFilt1D2Ref a_filt, bool a_bDraw = false, 
			U8ColorVal a_color = u8ColorVal( 0, 0, 0 ), 
			int a_shiftX = 0, int a_shiftY = 0, float a_scale = 1 );


		void PrepareFiltSys();

		void PrepareFiltSys_2();

		void PrepareFiltSys_Thesis();

		void PrepareFiltSys_Thesis_2();

		//void PrepareFiltSysAperSizes();
		void PrepareAperSizes();

		void ShowSignals();

		ISingFilt1D2DataReaderRef ToRdr( ISingFilt1D2DataMgrRef a_dataMgr )
		{
			return ( ISingFilt1D2DataReaderRef ) a_dataMgr;
		}

		IIntFilt1D2DataReaderRef ToRdr( IIntFilt1D2DataMgrRef a_dataMgr )
		{
			return ( IIntFilt1D2DataReaderRef ) a_dataMgr;
		}


		


		//void PrepareEnv( F32ImageRef a_src );
		void PrepareEnv( IImgDataMgrRef a_src, int a_nDif_Diameter );
		 

		void FillLineData( Ns_ImgGradMgr::ScanLineItrRef a_sli );

		//void UseScanOutput( );

		void DspLineOnImg( int a_x, int a_y,
			Ns_ImgGradMgr::ScanLineItrRef a_sli );


		//void Find_Edges_Roots();
		void Find_Edges_Roots( int a_x = -1, int a_y = -1 );


		EdgeScan::EdgeInfo * GetEI_Of_Pix( int a_x, int a_y,
			int a_nDir );

		EdgeScan::EdgeInfo * GetEI_Of_Pix( int a_nPI,
			int a_nDir );

		void ScanPntDir( int a_x, int a_y,
			Ns_ImgGradMgr::ScanLineItrRef a_sli = NULL );

		void ScanSli( int a_x, int a_y,
			Ns_ImgGradMgr::ScanLineItrRef a_sli );

		void Show_Edges_Roots( );

		void PostProcessEI_Arr_2();

		void PrepareDeepRoots();

		F32ImageRef GenMaxSlopeImg();

		F32ImageRef GenMinSlopeImg();

		F32ImageRef GenFinDifImg( int a_nDir );

		F32ImageRef GenSlopeImg( int a_nDir );

		void PostProcScanLine( Ns_ImgGradMgr::ScanLineItrRef a_sli,
			int a_nofEIs_BefLine );

	protected:

		//F32ImageRef m_src;
		IImgDataMgrRef m_src;

		int m_nDiameter;

		
		RgnSegmDiameter m_rsDiam;
		

		int m_nMaxScanSize;

		ImgGradMgr_2Ref m_imgGradMgr;
		//ImgAvging MgrRef m_imgGradMgr;
		ImgGradMgr_2Ref m_igm_2;

		ScanDirMgrCollRef m_sdmColl;

		int m_nScanDir;

		Hcpl::Int32 * m_dirBuf;

		F32ColorVal m_colorScan;
		F32ColorVal m_colorLoc;

		ISingFilt1D2DataMgrRef m_scanData;
		ISingFilt1D2DataMgrRef m_scanData_2;
		ISingFilt1D2DataMgrRef m_stDevLocData;
		ISingFilt1D2DataMgrRef m_locData;
		ISingFilt1D2DataMgrRef m_angDifData;
		ISingFilt1D2DataMgrRef m_confData;
		ISingFilt1D2DataMgrRef m_mag_Data;		
		ISingFilt1D2DataMgrRef m_standDivData;
		ISingFilt1D2DataMgrRef m_standDiv_R_Data;
		ISingFilt1D2DataMgrRef m_standDiv_R12_Data;
		
		


		ISingFilt1D2DataMgrRef m_finDifData;

		Ptr_SingFilt1D2DataMgrRef m_finDif_ptrDataMgr;


		IIntFilt1D2DataMgrRef m_ioaData;
		
		Ptr_IntFilt1D2DataMgrRef m_ioa_ptrDataMgr;

		Signal1DRef m_sigPosDelta;

		Filt1D2SysEmptyRef m_filtSys;

		FixedVector< EdgeScan::EdgeInfo > m_edgeInfoArr;

		//FixedVector< DataDraw > m_sigDrawVect;

		FixedVector< EdgeScan::PixInfo > m_pixInfoArr;

		FixedVector< EdgeScan::ScanDirMgrExtRef > m_scanDirExtArr;

		MultiListQueMgr< EdgeScan::EdgeProcAction > m_edgeProcAction_Ques;

		FixedVector< EdgeScan::EdgeProcAction > m_edgePA_Arr;

		FixedVector< int > m_someIOA_Arr;

		float * m_standDiv_Buf;

		float m_difScale;

		FixedVector< float > m_maxDifArr;

		FixedVector< EdgeScan::EdgeInfo * > m_maxEI_Arr;

		FixedVector< bool > m_isMaxDirArr;

		FixedVector< float > m_slopeOfMaxDifArr;
		//FixedVector< float > m_slopeOfMaxDifArr;
	};


	typedef Hcpl::ObjRef< ScanTryMgr_8 > ScanTryMgr_8Ref;

}