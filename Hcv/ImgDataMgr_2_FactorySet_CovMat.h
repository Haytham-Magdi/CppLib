#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

#include <Lib\Hcv\ImgDataMgr_2_CovMat.h>
#include <Lib\Hcv\ImgDataGradMgr_CovMat.h>



namespace Hcv
{
	//using namespace Hcpl::Math;

	


	class ImgDataMgr_2_FactorySet_CovMat : public ImgDataMgr_2_FactorySet_Base
	{
	public:

		ImgDataMgr_2_FactorySet_CovMat()
		{
			m_dataMgrFactory = new ImgDataMgr_2_CovMat_Factory();

			m_gradMgrFactory = new ImgDataGradMgr_CovMat_Factory();
		}

	protected:

		virtual void DummyFunc() {}		
	};


	typedef Hcpl::ObjRef< ImgDataMgr_2_FactorySet_CovMat > ImgDataMgr_2_FactorySet_CovMatRef;

}