#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>


#include <Lib\Hcv\IImgDataMgr.h>
#include <Lib\Hcv\ImgDataElm_Simple.h>




namespace Hcv
{
	//using namespace Hcpl::Math;


	class ImgDataMgr_Simple : public IImgDataMgr
	{
	public:

		ImgDataMgr_Simple(F32ImageRef a_srcImg, int a_nDiameter);

		float CalcDif(int a_nIdx_1, int a_nIdx_2);

		CvSize GetSize() 
		{
			return m_srcImg->GetSize();
		}

		int GetDiameter()
		{
			return m_nDiameter;
		}


	protected:

		void PrepareAll();

		void PrepareData_First();


	protected:

		FixedVector<ImgDataElm_Simple> m_dataArr;

		F32ImageRef m_srcImg;
		
		int m_nDiameter;


	};


	typedef Hcpl::ObjRef< ImgDataMgr_Simple > ImgDataMgr_SimpleRef;

}