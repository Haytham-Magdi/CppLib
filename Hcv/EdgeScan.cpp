#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\EdgeScan.h>

#define ARR_SIZ 3

namespace Hcv
{

	namespace EdgeScan
	{
		ScanDirMgrExt::ScanDirMgrExt( ScanDirMgr & a_rScanMgr )
		{
			PrepareSelf( a_rScanMgr );
		}

		void ScanDirMgrExt::PrepareSelf( ScanDirMgr & a_rScanMgr )
		{
			m_nDir = a_rScanMgr.GetID();

			const int nofHeads = a_rScanMgr.Get_ioaNofHeads();

			m_difArrColl.SetCapacity( a_rScanMgr.Get_ioaDataSizeTot(), nofHeads );

			for( int i=0; i < nofHeads; i++ )
			{
				m_difArrColl.AddArr( a_rScanMgr.Get_ioaSizeAt( i ) );
			}



			CvSize srcSiz = a_rScanMgr.GetCvSize();

			const int nSrcSiz1D = srcSiz.width * srcSiz.height;

			m_difGlobArr.SetSize( nSrcSiz1D );

			m_slopeGlobArr.SetSize( nSrcSiz1D );

			m_edgeInfo_GlobArr.SetSize( nSrcSiz1D );
		}

	}


}
