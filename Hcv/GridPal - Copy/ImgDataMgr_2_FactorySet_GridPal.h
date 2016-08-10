#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

#include <Lib\Hcv\GridPal\ImgDataMgr_2_GridPal.h>
#include <Lib\Hcv\GridPal\ImgDataGradMgr_GridPal.h>



namespace Hcv
{
	//using namespace Hcpl::Math;

	


	class ImgDataMgr_2_FactorySet_GridPal : public ImgDataMgr_2_FactorySet_Base
	{
	public:

		ImgDataMgr_2_FactorySet_GridPal()
		{
			m_dataMgrFactory = new ImgDataMgr_2_GridPal_Factory();

			m_gradMgrFactory = new ImgDataGradMgr_GridPal_Factory();
		}

	protected:

		virtual void DummyFunc() {}		
	};


	typedef Hcpl::ObjRef< ImgDataMgr_2_FactorySet_GridPal > ImgDataMgr_2_FactorySet_GridPalRef;

}