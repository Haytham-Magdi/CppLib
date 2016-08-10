
#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

#include <Lib\Hcv\EdgeScan.h>

#include <Lib\Hcv\ScanTryDbg.h>


namespace Hcv
{

	ScanTryDbg::ScanTryDbgCore ScanTryDbg::Core;


	void ScanTryDbg::ScanTryDbgCore::CreateMarkImg( CvSize a_imgSiz )
	{
		m_markImg = F32Image::Create( a_imgSiz, 1 );
		
		m_markImg->SetAll( 0 );		
	}


	void ScanTryDbg::ScanTryDbgCore::MarkPoint( int a_x, int a_y, int a_nRadius )
	{
		DrawCircle( m_markImg, F32Point( a_x, a_y ),
			u8ColorVal( 255, 255, 255 ), a_nRadius );
	}


}

