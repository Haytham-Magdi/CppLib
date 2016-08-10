#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

#include <Lib\Hcv\IImgDataMgr_2.h>
#include <Lib\Hcv\IImgDataGradMgr.h>



namespace Hcv
{
	//using namespace Hcpl::Math;

	

	class IImgDataMgr_2_FactorySet : FRM_Object
	{
	public:

		virtual IImgDataMgr_2_FactoryRef GetDataMgr_Factory() = 0;

		virtual IImgDataGradMgr_FactoryRef GetGradMgr_Factory() = 0;
	};


	class ImgDataMgr_2_FactorySet_Base : public IImgDataMgr_2_FactorySet
	{
	public:

		virtual IImgDataMgr_2_FactoryRef GetDataMgr_Factory()			
		{
			return m_dataMgrFactory;
		}
		
		virtual IImgDataGradMgr_FactoryRef GetGradMgr_Factory() 
		{
			return m_gradMgrFactory;
		}

	protected:

		virtual void DummyFunc() = 0;
		
	protected:

		IImgDataMgr_2_FactoryRef m_dataMgrFactory;

		IImgDataGradMgr_FactoryRef m_gradMgrFactory;
	};


	typedef Hcpl::ObjRef< IImgDataMgr_2_FactorySet > IImgDataMgr_2_FactorySetRef;

}