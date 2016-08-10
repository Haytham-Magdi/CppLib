#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\Avging_ScanDirMgrExt.h>



namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;

	//using namespace Hcv::Ns_ImgGradMgr;
	using namespace Ns_ImgAvgingMgr;



		Avging_ScanDirMgrExt::Avging_ScanDirMgrExt( 
			ScanDirMgr & a_rScanMgr, int a_nofDirs )
		{
			PrepareSelf( a_rScanMgr, a_nofDirs );
		}

		void Avging_ScanDirMgrExt::PrepareSelf( 
			ScanDirMgr & a_rScanMgr, int a_nofDirs )
		{
			m_nDir = a_rScanMgr.GetID();

			m_nofDirs = a_nofDirs;

			m_nDirNorm = 
				( m_nDir + ( m_nofDirs / 2 ) ) % m_nofDirs;

			const int nofHeads = a_rScanMgr.Get_ioaNofHeads();

			m_avgArrColl.SetCapacity( a_rScanMgr.Get_ioaDataSizeTot(), nofHeads );

			for( int i=0; i < nofHeads; i++ )
			{
				m_avgArrColl.AddArr( a_rScanMgr.Get_ioaSizeAt( i ) );
			}



			CvSize srcSiz = a_rScanMgr.GetCvSize();

			const int nSrcSiz1D = srcSiz.width * srcSiz.height;

			m_avgGlobArr.SetSize( nSrcSiz1D );

			//m_slopeGlobArr.SetSize( nSrcSiz1D );

			//m_edgeInfo_GlobArr.SetSize( nSrcSiz1D );
		}



}