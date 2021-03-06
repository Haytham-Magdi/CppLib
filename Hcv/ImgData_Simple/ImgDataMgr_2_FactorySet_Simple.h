#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

#include <Lib\Hcv\ImgData_Simple\ImgDataMgr_2_Simple.h>
#include <Lib\Hcv\ImgData_Simple\ImgDataGradMgr_Simple.h>



namespace Hcv
{
	//using namespace Hcpl::Math;

	


	class ImgDataMgr_2_FactorySet_Simple : public ImgDataMgr_2_FactorySet_Base
	{
	public:

		ImgDataMgr_2_FactorySet_Simple()
		{
			m_dataMgrFactory = new ImgDataMgr_2_Simple_Factory();

			m_gradMgrFactory = new ImgDataGradMgr_Simple_Factory();
		}

	protected:

		virtual void DummyFunc() {}		
	};


	typedef Hcpl::ObjRef< ImgDataMgr_2_FactorySet_Simple > ImgDataMgr_2_FactorySet_SimpleRef;

}