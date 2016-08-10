#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

#include <Lib\Hcv\LinePath.h>
#include <Lib\Hcv\ColorFilter1DSystem.h>



namespace Hcv
{
	//using namespace Hcpl::Math;


	class ImageLineViewer : public IColorFilter1DSystemConsumer
	{


	protected:

		class RgnInfo
		{
		public:

			RgnInfo()
			{
			}


		public:

			F32ColorVal * pImgPix;
			F32ColorVal * pShowPix;
			F32Point pos;

		protected:

		};

		class IFilterOutputMgr : FRM_Object
		{
		public:
			virtual void ProcessOutput( int a_nFilterID, F32ColorVal a_outVal, int a_nBackShift ) = 0;
			virtual void ProcessOutput( int a_nFilterID, float a_outVal, int a_nBackShift ) = 0;
			virtual void ProvideSignal( Signal1DViewerRef a_sv ) = 0;
		};

		typedef Hcpl::ObjRef< IFilterOutputMgr > IFilterOutputMgrRef;

		class FilterOutputMgrBase : public IFilterOutputMgr
		{
		public:
			FilterOutputMgrBase( ImageLineViewer * a_pIlv ) : m_pIlv( a_pIlv ) {}

			virtual void ProcessOutput( int a_nFilterID, F32ColorVal a_outVal, int a_nBackShift )
			{ ThrowHcvException(); }

			virtual void ProcessOutput( int a_nFilterID, float a_outVal, int a_nBackShift )
			{ ThrowHcvException(); }

		protected:
			ImageLineViewer * m_pIlv;
		};

		class DummyOutputMgr : public FilterOutputMgrBase
		{
		public:
			DummyOutputMgr( ImageLineViewer * a_pIlv ) : FilterOutputMgrBase( a_pIlv ) {}

			virtual void ProcessOutput( int a_nFilterID, F32ColorVal a_outVal, int a_nBackShift ) { }
			virtual void ProcessOutput( int a_nFilterID, float a_outVal, int a_nBackShift ) { }
			virtual void ProvideSignal( Signal1DViewerRef a_sv ) { }

		protected:
		};

		class ColorOutputMgr : public FilterOutputMgrBase
		{
		public:
			ColorOutputMgr( ImageLineViewer * a_pIlv,
				U8ColorVal a_valColor0, U8ColorVal a_valColor1, U8ColorVal a_valColor2 ) 
				: FilterOutputMgrBase( a_pIlv ), m_sb0( NULL ), m_sb1( NULL ), m_sb2( NULL ), 
				m_valColor0(a_valColor0), m_valColor1(a_valColor1), m_valColor2(a_valColor2) {}

			virtual void ProcessOutput( int a_nFilterID, F32ColorVal a_outVal, int a_nBackShift );
			virtual void ProvideSignal( Signal1DViewerRef a_sv );

		protected:
			Signal1DBuilderRef m_sb0;
			Signal1DBuilderRef m_sb1;
			Signal1DBuilderRef m_sb2;

			U8ColorVal m_valColor0;
			U8ColorVal m_valColor1;
			U8ColorVal m_valColor2;
		};


		class AvgOutputMgr : public ColorOutputMgr
		{
		public:
			AvgOutputMgr( ImageLineViewer * a_pIlv ) : ColorOutputMgr( a_pIlv,
				u8ColorVal(200, 0, 0), u8ColorVal(0, 180, 0), u8ColorVal(0, 0, 255) ) {}

			//virtual void ProcessOutput( int a_nFilterID, F32ColorVal a_outVal, int a_nBackShift );
			//virtual void ProvideSignal( Signal1DViewerRef a_sv );
		};


		class Diff1OutputMgr : public FilterOutputMgrBase
		{
		public:
			Diff1OutputMgr( ImageLineViewer * a_pIlv ) : FilterOutputMgrBase( a_pIlv ),
				m_sb( NULL ) {}

			virtual void ProcessOutput( int a_nFilterID, F32ColorVal a_outVal, int a_nBackShift );
			virtual void ProvideSignal( Signal1DViewerRef a_sv );

		protected:
			Signal1DBuilderRef m_sb;
		};


		class SingleOutputMgr : public FilterOutputMgrBase
		{
		public:
			SingleOutputMgr( ImageLineViewer * a_pIlv, U8ColorVal a_color ) 
				: FilterOutputMgrBase( a_pIlv ), m_color(a_color), m_sb( NULL ) {}

			virtual void ProcessOutput( int a_nFilterID, float a_outVal, int a_nBackShift );
			virtual void ProvideSignal( Signal1DViewerRef a_sv );

		protected:
			U8ColorVal m_color;
			Signal1DBuilderRef m_sb;
		};


		class Diff1MarkerOutputMgr : public SingleOutputMgr
		{
		public:
			Diff1MarkerOutputMgr( ImageLineViewer * a_pIlv, U8ColorVal a_color ) 
				: SingleOutputMgr( a_pIlv, a_color ) {}

			virtual void ProcessOutput( int a_nFilterID, float a_outVal, int a_nBackShift );
		};

		class Diff2MarkerOutputMgr : public SingleOutputMgr
		{
		public:
			Diff2MarkerOutputMgr( ImageLineViewer * a_pIlv, U8ColorVal a_color ) 
				: SingleOutputMgr( a_pIlv, a_color ) {}

			virtual void ProcessOutput( int a_nFilterID, float a_outVal, int a_nBackShift );
		};

		class Diff1AvgOutputMgr : public SingleOutputMgr
		{
		public:
			Diff1AvgOutputMgr( ImageLineViewer * a_pIlv ) 
				: SingleOutputMgr( a_pIlv, u8ColorVal(0, 128, 255) ) {}
		};


		class Diff2OutputMgr : public FilterOutputMgrBase
		{
		public:
			Diff2OutputMgr( ImageLineViewer * a_pIlv ) : FilterOutputMgrBase( a_pIlv ),
				m_sb( NULL ) {}

			virtual void ProcessOutput( int a_nFilterID, F32ColorVal a_outVal, int a_nBackShift );
			virtual void ProvideSignal( Signal1DViewerRef a_sv );

		protected:

			Signal1DBuilderRef m_sb;
		};



	public:

		ImageLineViewer( F32ImageRef a_srcImg, LinePathRef a_lp );


		virtual void ReceiveFilterOutput( int a_nFilterID, F32ColorVal a_outVal, int a_nBackShift );
		virtual void ReceiveFilterOutput( int a_nFilterID, float a_outVal, int a_nBackShift );

		virtual void NotifyAllOutputDone( );


		F32ImageRef GetResultImg();

		U8ImageRef GetSignalDspImg();




	protected:

		void DoPrepare();

		int AddFilter( ColorFilter1DSystemBuilder * pFiltSysBld, 
			IColorFilter1DRef a_filt, IFilterOutputMgrRef a_outMgr, int a_nSrcID = -1 );

		int AddFilter( ColorFilter1DSystemBuilder * pFiltSysBld, 
			IFilter1DRef a_filt, IFilterOutputMgrRef a_outMgr, int a_nSrcID = -1 );

		int AddColorToSingleConverter( ColorFilter1DSystemBuilder * pFiltSysBld, 
			IFilterOutputMgrRef a_outMgr, int a_nSrcID = -1 );


		//int AddSingleFilter(

		void DoProcess();

		void InitRgnInfoVect();

		RgnInfo * GetPointRgn(int x, int y)
		{ 
			return &m_rgnInfoVect[m_rgnIndexCalc.Calc(x, y)]; 
		}

		RgnInfo * GetBackInputRgn( int a_nBackShift )
		{ 
			return m_inputRgnPtrVect[ m_inputRgnPtrVect.GetSize() - 1 - a_nBackShift ]; 
		}


		friend class IFilterOutputMgr;


	protected:

		//ColorFilter1DSystemBuilder m_filtSysBld;

		const int m_nAprSize;

		IndexCalc2D m_rgnIndexCalc;

		F32ImageRef m_srcImg;
		
		LinePathRef m_lp;

		F32ImageRef m_resImg;

		U8ImageRef m_sigDspImg;

		FixedVector<RgnInfo> m_rgnInfoVect;

		FixedVector< RgnInfo * > m_inputRgnPtrVect;

		//FixedVector< IFilterOutputMgr * > m_filterOutputMgrVect;
		FixedVector< IFilterOutputMgrRef > m_filterOutputMgrVect;

		ColorFilter1DSystemRef m_filtSys;
	};


	typedef Hcpl::ObjRef< ImageLineViewer > ImageLineViewerRef;

}