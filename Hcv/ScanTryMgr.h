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



namespace Hcv
{
	//using namespace Hcpl::Math;


	class ScanTryMgr : public virtual EdgeScanMgr
	{
	public:


	protected:


	public:
		
		ScanTryMgr( int a_x, int a_y );

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
			return EdgeScanMgr::AddSigDraw( a_dataRdr, a_bDraw, a_color, 
				a_shiftX, a_shiftY, a_scale );
		}


		virtual DataDraw * AddSigDraw( ISingFilt1D2Ref a_filt, bool a_bDraw = false, 
		//DataDraw * AddSigDraw( ISingFilt1D2Ref a_filt, bool a_bDraw = false, 
			U8ColorVal a_color = u8ColorVal( 0, 0, 0 ), 
			int a_shiftX = 0, int a_shiftY = 0, float a_scale = 1 );


		void PrepareFiltSys_0();
		void PrepareFiltSys();

		//void PrepareFiltSysAperSizes();
		void PrepareAperSizes();

		void ShowResult();


	protected:

		F32ColorVal m_colorScan;
		F32ColorVal m_colorLoc;

		Signal1DRef m_sigScan;
		Signal1DRef m_sigLoc;
		Signal1DRef m_sigAngDif;
		Signal1DRef m_sigPosDelta;

		Filt1D2SysEmptyRef m_filtSys;

		//FixedVector< DataDraw > m_sigDrawVect;
	};


	typedef Hcpl::ObjRef< ScanTryMgr > ScanTryMgrRef;

}