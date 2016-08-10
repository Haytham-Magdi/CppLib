#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <queue>      

#include <Lib\Hcv\Filt1D2.h>
#include <Lib\Hcv\Filt1D2DataMgr.h>


namespace Hcv
{
	using namespace Hcpl::Math;
	using namespace std;



	class Filt1D2SysProto : public Filt1D2SystemBase
	{
	protected:


	public:
		Filt1D2SysProto( int a_nDataCapacity );

		ISingFilt1D2DataMgrRef GetInpMgr()
		{
			return m_inpMgr;
		}

		ISingFilt1D2Ref GetAvgFilt()
		{
			return m_avgFilt;
		}

	protected:


	protected:

		ISingFilt1D2DataMgrRef m_inpMgr;

		ISingFilt1D2Ref m_avgFilt;

	};

	typedef Hcpl::ObjRef< Filt1D2SysProto > Filt1D2SysProtoRef;

}
