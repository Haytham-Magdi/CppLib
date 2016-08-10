#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

#include <Lib\Hcv\IImgPopulation.h>

#include <Lib\Hcv\ImgDataMgr_C7.h>



namespace Hcv
{
	//using namespace Hcpl::Math;


	class ImgPopulation_C7 : public IImgPopulation
	{
	private:

		ImgPopulation_C7( );

	public:

		ImgPopulation_C7( ImgDataMgr_C7Ref a_dataMgr );


		void AddElm(int a_nIdx);

		void SubElm(int a_nIdx);

		void AddRange(int * a_idxBuf, int a_nofElms);

		void AddElm_Thick(int a_nIdx);

		void SubElm_Thick(int a_nIdx);

		void AddRange_Thick(int * a_idxBuf, int a_nofElms);

		int GetNofElms() { return m_nPopSize; }

		void IncBy( IImgPopulation * a_pop );

		void Copy( IImgPopulation * a_pop );


		void Reset();

		float CalcVariance();

		float CalcStandDiv();

	protected:

		inline void DoAddElm(int a_nIdx);

		inline void DoAddElm_Thick(int a_nIdx);		

		inline float DoCalcVariance();

	protected:

		ImgDataMgr_C7Ref m_dataMgr;

		ImgDataElm_C7 * m_data_Buf;

		ImgDataElm_C7 * m_data_Avg_Buf;

		float * m_magSqr_Avg_Buf;

		int m_nDataSize;

		ImgDataElm_C7 m_elm_MeanSum;

		int m_nPopSize;

		float m_sum_magSqr;


	};


	typedef Hcpl::ObjRef< ImgPopulation_C7 > ImgPopulation_C7Ref;

}