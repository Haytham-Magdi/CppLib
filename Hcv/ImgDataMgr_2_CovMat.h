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
#include <Lib\Hcv\ImgDataElm_CovMat.h>




namespace Hcv
{
	//using namespace Hcpl::Math;


	class ImgDataMgr_2_CovMat_Factory : public IImgDataMgr_2_Factory
	{
	public:

		virtual IImgDataMgr_2 * CreateImgDataMgr( 
			 F32ImageRef a_srcImg, int a_nAprSiz, 
			 IImgCoreSrcRef a_dataSrc,
			 ImgRotationMgrRef a_rot );
	};

	typedef Hcpl::ObjRef< ImgDataMgr_2_CovMat_Factory > ImgDataMgr_2_CovMat_FactoryRef;



	class ImgDataMgr_2_CovMat : public IImgDataMgr_2
	{
	public:

		ImgDataMgr_2_CovMat( F32ImageRef a_srcImg, int a_nAprSiz );

		virtual F32ImageRef GetSrcImg()
		{
			return m_srcImg;
		}

		virtual int GetAprSiz()
		{
			return m_nAprSiz;
		}

		FixedVector< ImgDataElm_CovMat > & GetDataArr()
		{
			return m_dataArr;
		}


		static void ZeroBorder(
			FixedVector<ImgDataElm_CovMat> & a_dataArr,
			CvSize a_imgSiz,
			int a_nMargX, int a_nMargY);

		static void PrepareData_First_H(
			int a_nAprSiz, CvSize a_imgSiz,
			FixedVector< ImgDataElm_CovMat > & a_srcArr,
			FixedVector< ImgDataElm_CovMat > & a_destArr);

		static void PrepareData_First_V(
			int a_nAprSiz, CvSize a_imgSiz,
			FixedVector< ImgDataElm_CovMat > & a_srcArr,
			FixedVector< ImgDataElm_CovMat > & a_destArr);


	protected:

		void PrepareData();


	protected:

		F32ImageRef m_srcImg;
		int m_nAprSiz;

		FixedVector<ImgDataElm_CovMat> m_dataArr;

	};

	typedef Hcpl::ObjRef< ImgDataMgr_2_CovMat > ImgDataMgr_2_CovMatRef;



}