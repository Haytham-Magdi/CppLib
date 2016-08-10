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
#include <Lib\Hcv\ImgDataElm_CovMat.h>


//#include <Lib\Hcv\ImgPopulation_CovMat.h>
#include <Lib\Hcv\IImgPopulation.h>

#include <Lib\Hcv\RgnSegmDiameter.h>

#include <Lib\Hcv\ImgGradMgr_2.h>





namespace Hcv
{
	//using namespace Hcpl::Math;


	class ImgPopulation_CovMat;


	class ImgDataMgr_CovMat : public IImgDataMgr
	{
	public:

		ImgDataMgr_CovMat(F32ImageRef a_srcImg, int a_nDiameter,
			bool a_bPrepareData_StandDiv = true);

		//virtual ~ImgDataMgr_CovMat()
		~ImgDataMgr_CovMat();

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
			return m_img_Mag;
		}

		bool VerifyEdge( int a_nIdx_R1, int a_nIdx_Med1, 
			int a_nIdx_Edge, int a_nIdx_Med2, int a_nIdx_R2 );

		IImgPopulationRef CreatePopulation();



		FixedVector<ImgDataElm_CovMat> & GetDataArr()
		{
			return m_dataArr;
		}

		FixedVector<ImgDataElm_CovMat> & GetDataArr_Mean()
		{
			return m_dataArr_Mean;
		}

		FixedVector<float> & GetAvgSqrMag_Arr()
		{
			return m_avgSqrMag_Arr;
		}


		friend class ImgPopulation_CovMat;

	protected:

		void ValidateDiameter( int a_nDiameter );

		void PrepareAll();

		void PrepareData_First();

		void PrepareData_StandDiv_0();

		void PrepareData_StandDiv();

		void ZeroBorder(FixedVector<ImgDataElm_CovMat> & a_dataArr,
			int a_nMargX, int a_nMargY);

		void PrepareData_First_H(int a_nAprSiz,
			FixedVector<ImgDataElm_CovMat> & a_srcArr,
			FixedVector<ImgDataElm_CovMat> & a_destArr);

		void PrepareData_First_V(int a_nAprSiz,
			FixedVector<ImgDataElm_CovMat> & a_srcArr,
			FixedVector<ImgDataElm_CovMat> & a_destArr);


		F32ImageRef GenMeanImg();


	protected:
		
		RgnSegmDiameter m_rsDiam;

		IImgPopulationRef m_pImgPop;
		FixedVector< int > m_popIdx_Arr;





		FixedVector<ImgDataElm_CovMat> m_dataArr;

		FixedVector<ImgDataElm_CovMat> m_dataArr_Mean;

		FixedVector<float> m_avgSqrMag_Arr;

		F32ImageRef m_srcImg;
		
		int m_nDiameter;

		//FixedVector<float> m_standDiv_Mag_Arr;
		F32ImageRef m_img_StandDiv_Mag;

		F32ImageRef m_img_Mag;

		bool m_bPrepareData_StandDiv;

		ImgGradMgr_2Ref m_igm_2;
	};


	typedef Hcpl::ObjRef< ImgDataMgr_CovMat > ImgDataMgr_CovMatRef;

}