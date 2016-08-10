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



namespace Hcv
{
	//using namespace Hcpl::Math;

	class ImgGradMgr;
	class ImgGradMgr_2;
	class ImgAvgingMgr;
	

	namespace Ns_ImgGradMgr
	{
		class ScanLineItr : FRM_Object
		{
		private:
			ScanLineItr( ) {}

		public:
			//ScanLineItr( F32Point a_pnt, int a_nDir );

			~ScanLineItr( )
			{

			}

			int GetCurIOL()
			{
				return m_nCurI;
			}

			int GetCurPI()
			{
				return m_nCPI;
			}

			F32Point GetCurPnt()
			{
				return F32Point::CreateFromIndex( m_nCPI, m_srcSiz );
			}

			int GetDirIndex()
			{
				return m_nDir;
			}

			int GetLineNum()
			{
				return m_nLineNum;
			}			

			bool MoveNext();
			bool MovePrev();

			void GotoFirst();
			void GotoLast();

			bool AreYou( int a_nDir, int a_nBPI )
			{
				return ( a_nDir == m_nDir &&
					//a_nBPI == m_nBPI );
					false );
			}

			friend class ImgGradMgr;
			friend class ImgGradMgr_2;
			friend class ImgAvgingMgr;

		protected:

		protected:

			int m_nDir;

			int m_nLineNum;

			CvSize m_srcSiz;

			//int m_nBPI;

			//int m_nSI;
		//int m_nLimI;
			//int m_nEI;

			int m_nCurI;

			int m_nCPI;

			//FixedVector< int > * m_pShiftArr;

			int * m_pIOA_Head;
			int m_nIOA_Size;
		};

		typedef Hcpl::ObjRef< ScanLineItr > ScanLineItrRef;

	}
	
}


namespace Hcv
{


	using namespace Ns_ImgGradMgr;


	class ImgGradMgr : FRM_Object
	{


	public:



		class FwdBkdPoints
		{
		public:
			F32Point FwdPnt;
			F32Point BkdPnt;
		};


		ScanLineItrRef GenScanLineItr( F32Point a_pnt, int a_nDir );

		ScanLineItrRef GenScanLineItr( int a_nPI, int a_nDir );

		void PrepareScanLineItr( ScanLineItrRef a_sli, int a_nPI, int a_nDir );

		ScanLineItrRef GenScanLineItr_FromLine(
			int a_nLineNum, int a_nDir, int a_nCurI = 0 );



		ImgGradMgr( F32ImageRef a_src, const int a_nAprSiz, const int a_nofDirs );

		FixedVector< float > & GetGradDataArr()
		{
			return m_gradDataArr;
		}

		FixedVector< bool > & GetIsPeakDataArr();

		int GetNofDirs()
		{
			return m_nofDirs;
		}

		int GetAprSiz()
		{
			return m_nAprSiz;
		}

		ScanDirMgrCollRef GetScanDirMgrColl()
		{
			return m_sdmColl;
		}

		F32ImageRef GetGradImgOfDir( const int a_nDir );

		F32ImageRef GetGradAngleImg();

		S32ImageRef GetGradDirImg();

		F32ImageRef GetGradMagImg();

		F32ImageRef GenIsPeakImg();

		Accessor2D< FwdBkdPoints > & GetFwdBkdMatrix();

		Signal1DRef GenLineSignal( F32Point a_pnt, int a_nDir );


	protected:

		void Proceed();

		void PrepareTotGrad();

		static F32Point GetPntOfIndex( int a_nIndex, CvSize a_siz );

		void PrepareDirPeaks( int a_nDir, int a_nAprSiz, int a_nStepCnt );

	protected:

		F32ImageRef m_src;

		FixedVector< float > m_gradDataArr;
		FixedVector< bool > m_isPeakDataArr;
		int m_nofDirs;
		int m_nAprSiz;

		F32ImageRef m_gradMagImg;
		F32ImageRef m_gradAngleImg;

		S32ImageRef m_gradDirImg;

		ScanDirMgrCollRef m_sdmColl;

		Accessor2D< FwdBkdPoints > m_fwdBwdPntArr;
		bool m_bFwdBkdDone;

	};


	typedef Hcpl::ObjRef< ImgGradMgr > ImgGradMgrRef;

}

