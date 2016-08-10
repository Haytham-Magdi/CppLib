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

#include <Lib\Hcv\ImgGradMgr.h>

#include <Lib\Hcv\Filt1D2System.h>



namespace Hcv
{
	//using namespace Hcpl::Math;


	class ScanTryMgr2 : public virtual EdgeScanMgr
	{
	public:


	protected:


	public:
		
		ScanTryMgr2( int a_x, int a_y );

		ScanTryMgr2( F32ImageRef a_src );

		FixedVector< EdgeScan::EdgeInfo > & GetEdgeInfoArr()
		{
			return m_edgeInfoArr;
		}


	protected:


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


		


		void PrepareEnv( F32ImageRef a_src );

		void FillLineData( ImgGradMgr::ScanLineItrRef a_sli );

		//void UseScanOutput( );

		void DspLineOnImg( int a_x, int a_y,
			ImgGradMgr::ScanLineItrRef a_sli );


		void Find_Edges_Roots();

		void ScanPntDir( int a_x, int a_y,
			ImgGradMgr::ScanLineItrRef a_sli = NULL );

		void ScanSli( int a_x, int a_y,
			ImgGradMgr::ScanLineItrRef a_sli );

		void Show_Edges_Roots( );

		void PostProcessEI_Arr();

		void PostProcessEI_Arr_2();

		F32ImageRef GenMaxSlopeImg();

		F32ImageRef GenMinSlopeImg();

		F32ImageRef GenFinDifImg( int a_nDir );

		F32ImageRef GenSlopeImg( int a_nDir );

		void PostProcScanLine( ImgGradMgr::ScanLineItrRef a_sli,
			int a_nofEIs_BefLine );

	protected:

		F32ImageRef m_src;

		int m_nMaxScanSize;

		ImgGradMgrRef m_imgGradMgr;

		ScanDirMgrCollRef m_sdmColl;

		int m_nScanDir;

		Hcpl::Int32 * m_dirBuf;

		F32ColorVal m_colorScan;
		F32ColorVal m_colorLoc;

		ISingFilt1D2DataMgrRef m_scanData;
		ISingFilt1D2DataMgrRef m_locData;
		ISingFilt1D2DataMgrRef m_angDifData;


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

		float m_difScale;

		FixedVector< float > m_maxDifArr;

		FixedVector< bool > m_isMaxDirArr;

		FixedVector< float > m_slopeOfMaxDifArr;
		//FixedVector< float > m_slopeOfMaxDifArr;
	};


	typedef Hcpl::ObjRef< ScanTryMgr2 > ScanTryMgr2Ref;

}