#pragma once


#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\PixelTypes.h>
#include <Lib\Hcv\error.h>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>


namespace Hcv
{
	using namespace Hcpl;

	class HistoGram : FRM_Object
	{
	public :
		HistoGram(int a_nSiz = 256)
		{
			m_vals.resize(a_nSiz);
			m_vals.assign(m_vals.size(), 0);			
			m_nofPixs = 0;
		}

		void Reset()
		{
			m_vals.assign(m_vals.size(), 0);
			m_nofPixs = 0;
		}

		Int32 GetAt(int a_nIdx)
		{
			return m_vals[a_nIdx];
		}

		std::vector<Int32> * GetVectPtr()
		{
			return &m_vals;
		}

		U8ImageRef GenDspImage(void)
		{
			return GenDspImage( cvSize(m_vals.size() * 2.2, 320) );
		}

		U8ImageRef GenDspImage(CvSize a_siz);

		void RefreshNofPixs()
		{
			m_nofPixs = 0;
			for(int i=0; i<(int)m_vals.size(); i++)
				m_nofPixs += m_vals[i];
		}

		int GetNofPixs()
		{
			return m_nofPixs;
		}

	protected :
		std::vector<Int32> m_vals;
		int m_nofPixs;
	};

	typedef Hcpl::ObjRef< HistoGram > HistoGramRef;

	template<class T>
	void CalcHisto(CHANNEL_REF(T) a_chnl, T a_dmy, HistoGramRef ao_hist)
	{
		ao_hist->Reset();

		Int32 * histData =   &(*(ao_hist->GetVectPtr()))[0];

		int x, y;
		for(y=0; y<a_chnl->GetHeight(); y++)
		{
			for(x=0; x<a_chnl->GetWidth(); x++)
			{
				histData[
					a_chnl->GetAt(x, y)]++;
			}
		}

		ao_hist->RefreshNofPixs();
	}

	
}



