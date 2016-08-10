#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\ImgPopulation_C7.h>

namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;


	ImgPopulation_C7::ImgPopulation_C7( ImgDataMgr_C7Ref a_dataMgr ) 
	{ 
		m_dataMgr = a_dataMgr;

		m_data_Buf = & a_dataMgr->m_dataArr[ 0 ];

		m_data_Avg_Buf = & a_dataMgr->m_dataArr_Mean[ 0 ];

		m_magSqr_Avg_Buf = & a_dataMgr->m_avgSqrMag_Arr[ 0 ];;


		m_nDataSize = a_dataMgr->m_dataArr.GetSize();

		Reset();

	}



	void ImgPopulation_C7::AddElm(int a_nIdx)
	{
		DoAddElm(a_nIdx);
	}

	void ImgPopulation_C7::SubElm(int a_nIdx)
	{
		ImgDataElm_C7 & rElm = m_data_Buf[ a_nIdx ];
		
		m_elm_MeanSum.DecBy( rElm );

		m_sum_magSqr -= rElm.CalcMagSqr();

		m_nPopSize--;
	}

	void ImgPopulation_C7::AddRange(int * a_idxBuf, int a_nofElms)
	{
		for(int i=0; i < a_nofElms; i++)
			DoAddElm( a_idxBuf[ i ] );
	}


	void ImgPopulation_C7::DoAddElm(int a_nIdx)
	{
		ImgDataElm_C7 & rElm = m_data_Buf[ a_nIdx ];
		
		m_elm_MeanSum.IncBy( rElm );

		m_sum_magSqr += rElm.CalcMagSqr();

		m_nPopSize++;
	}

//--------------



	void ImgPopulation_C7::AddElm_Thick(int a_nIdx)
	{
		DoAddElm_Thick(a_nIdx);
	}

	void ImgPopulation_C7::SubElm_Thick(int a_nIdx)
	{
		ImgDataElm_C7 & rElm = m_data_Avg_Buf[ a_nIdx ];

		m_elm_MeanSum.DecBy( rElm );

		m_sum_magSqr -= m_magSqr_Avg_Buf[ a_nIdx ];

		m_nPopSize--;
	}

	void ImgPopulation_C7::AddRange_Thick(int * a_idxBuf, int a_nofElms)
	{
		for(int i=0; i < a_nofElms; i++)
		{
			//if( i > a_nofElms * 0.35 && i < a_nofElms * 0.65 )
				//continue;

			DoAddElm_Thick( a_idxBuf[ i ] );
		}
	}


	void ImgPopulation_C7::DoAddElm_Thick(int a_nIdx)
	{
		//ImgDataElm_C7 & rElm = m_data_Buf[ a_nIdx ];
		ImgDataElm_C7 & rElm = m_data_Avg_Buf[ a_nIdx ];

		m_elm_MeanSum.IncBy( rElm );

		//m_sum_magSqr += rElm.CalcMagSqr();
		m_sum_magSqr += m_magSqr_Avg_Buf[ a_nIdx ];

		m_nPopSize++;
	}



//--------------------


	void ImgPopulation_C7::IncBy( IImgPopulation * a_pop )
	{
		ImgPopulation_C7 * pop1 = 
			dynamic_cast< ImgPopulation_C7 * > (a_pop);

		m_elm_MeanSum.IncBy( pop1->m_elm_MeanSum );

		m_nPopSize += pop1->m_nPopSize;

		m_sum_magSqr += pop1->m_sum_magSqr;
	}


	void ImgPopulation_C7::Copy( IImgPopulation * a_pop )
	{
		ImgPopulation_C7 * pop1 = 
			dynamic_cast< ImgPopulation_C7 * > (a_pop);

		m_elm_MeanSum.Copy( pop1->m_elm_MeanSum );

		m_nPopSize = pop1->m_nPopSize;

		m_sum_magSqr = pop1->m_sum_magSqr;
	}


	void ImgPopulation_C7::Reset()
	{
		m_elm_MeanSum.SetZeroVal();

		m_nPopSize = 0;

		m_sum_magSqr = 0;
	}


	float ImgPopulation_C7::CalcVariance()
	{
		return DoCalcVariance();
	}


	float ImgPopulation_C7::CalcStandDiv()
	{
		return sqrt( (float) DoCalcVariance() );
	}


	float ImgPopulation_C7::DoCalcVariance()
	{
		ImgDataElm_C7 meanElm;

		meanElm.Copy( m_elm_MeanSum );

		meanElm.DividSelfBy( m_nPopSize );

		float mean_MagSqr = meanElm.CalcMagSqr();

		float ret = m_sum_magSqr / m_nPopSize - mean_MagSqr;

		if( ret < 0 )
			ret = 0;

		return ret;
	}


}