#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\Filt1D2SysProto.h>



namespace Hcv
{
	using namespace Hcpl;
	using namespace Hcpl::Math;


	Filt1D2SysProto::Filt1D2SysProto( int a_nDataCapacity )
	{
		Init( a_nDataCapacity );

		m_inpMgr = 
			new SimpleFilt1D2DataMgr< float >( a_nDataCapacity );

		m_avgFilt = new AvgFilt1D2( 
			7, (ISingFilt1D2DataReaderRef) m_inpMgr );

		m_filtVect.PushBack( (IFilt1D2Ref) m_avgFilt );
	}

}