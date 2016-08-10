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

//#include <Lib\Hcv\Filt1D2.h>
//#include <Lib\Hcv\Filt1D2DataMgr.h>

#include <Lib\Hcv\SingFilt1D2.h>


namespace Hcv
{
	using namespace Hcpl::Math;
	using namespace std;

	class DifFilt1D2 : public LinearSingFilt1D2Base
	{
	public:

		DifFilt1D2( int a_nAprSizOrg, ISingFilt1D2DataReaderRef a_inpRdr,
			ISingFilt1D2DataMgrRef a_outDataMgr = NULL, float a_dmyVal = 0 );

		virtual void Proceed();


	protected:

	};

	typedef Hcpl::ObjRef< DifFilt1D2 > DifFilt1D2Ref;

}
