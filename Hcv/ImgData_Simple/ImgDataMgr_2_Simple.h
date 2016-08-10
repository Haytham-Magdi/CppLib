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
//#include <Lib\Hcv\ImgData_Simple\ImgDataElm_Simple.h>




namespace Hcv
{
	//using namespace Hcpl::Math;




	class ImgDataMgr_2_Simple_Factory : public IImgDataMgr_2_Factory
	{
	public:

		virtual IImgDataMgr_2 * CreateImgDataMgr( 
			 F32ImageRef a_srcImg, int a_nAprSiz, 
			 IImgCoreSrcRef a_dataSrc,
			 ImgRotationMgrRef a_rot );
	};

	typedef Hcpl::ObjRef< ImgDataMgr_2_Simple_Factory > ImgDataMgr_2_Simple_FactoryRef;



	class ImgDataMgr_2_Simple : public IImgDataMgr_2
	{
	public:

		ImgDataMgr_2_Simple( F32ImageRef a_srcImg, int a_nAprSiz,
			IImgCoreSrcRef a_dataSrc, ImgRotationMgrRef a_rot );

		virtual F32ImageRef GetSrcImg()
		{
			return m_srcImg;
		}

		virtual int GetAprSiz()
		{
			return m_nAprSiz;
		}
		
	protected:

		void PrepareData();


	//protected:
	public:

		F32ImageRef m_srcImg;
		int m_nAprSiz;

		F32ImageRef m_meanImg;
		F32ImageRef m_magSqrImg;
		F32ImageRef m_stanDevImg;


		ImgRotationMgrRef m_rot;
		
	};

	typedef Hcpl::ObjRef< ImgDataMgr_2_Simple > ImgDataMgr_2_SimpleRef;



}