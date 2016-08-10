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

		float Calc_UnitDif(int a_nIdx_1, int a_nIdx_2);

		CvSize GetSize() 
		{
			return m_srcImg->GetSize();
		}

		int GetDiameter()
		{
			return m_nDiameter;
		}

		F32ImageRef GetSrcImage()
		{
			return m_srcImg;
		}

		F32ImageRef GetStandDiv_Mag_Image()
		{
			return m_img_StandDiv_Mag;
		}

		F32ImageRef Get_Mag_Image()
		{
			ThrowHcvException();
			return NULL;
		}

		bool VerifyEdge( int a_nIdx_R1, int a_nIdx_Med1, 
			int a_nIdx_Edge, int a_nIdx_Med2, int a_nIdx_R2 )
		{
			ThrowHcvException();

			return false;
		}



	protected:

		void PrepareAll();

		void PrepareData_First();

		void PrepareData_StandDiv();

		void ZeroBorder(FixedVector<ImgDataElm_Simple> & a_dataArr,
			int a_nMargX, int a_nMargY);

		void PrepareData_First_H(int a_nAprSiz,
			FixedVector<ImgDataElm_Simple> & a_srcArr,
			FixedVector<ImgDataElm_Simple> & a_destArr);

		void PrepareData_First_V(int a_nAprSiz,
			FixedVector<ImgDataElm_Simple> & a_srcArr,
			FixedVector<ImgDataElm_Simple> & a_destArr);


		F32ImageRef GenMeanImg();


	protected:
		
		FixedVector<ImgDataElm_Simple> m_dataArr;

		F32ImageRef m_srcImg;
		
		int m_nDiameter;

		//FixedVector<float> m_standDiv_Mag_Arr;
		F32ImageRef m_img_StandDiv_Mag;

	};


	typedef Hcpl::ObjRef< ImgDataMgr_Simple > ImgDataMgr_SimpleRef;

}