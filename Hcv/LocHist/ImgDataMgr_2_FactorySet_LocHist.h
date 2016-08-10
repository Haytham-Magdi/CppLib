#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

#include <Lib\Hcv\LocHist\ImgDataMgr_2_LocHist.h>
#include <Lib\Hcv\LocHist\ImgDataGradMgr_LocHist.h>



namespace Hcv
{
	//using namespace Hcpl::Math;

	


	class ImgDataMgr_2_FactorySet_LocHist : public ImgDataMgr_2_FactorySet_Base
	{
	public:

		ImgDataMgr_2_FactorySet_LocHist()
		{
			m_dataMgrFactory = new ImgDataMgr_2_LocHist_Factory();

			m_gradMgrFactory = new ImgDataGradMgr_LocHist_Factory();
		}

	protected:

		virtual void DummyFunc() {}		
	};


	typedef Hcpl::ObjRef< ImgDataMgr_2_FactorySet_LocHist > ImgDataMgr_2_FactorySet_LocHistRef;

}