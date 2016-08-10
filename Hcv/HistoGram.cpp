#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\PixelTypes.h>
#include <Lib\Hcv\error.h>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

#include <Lib\Hcv\HistoGram.h>


namespace Hcv
{
	using namespace Hcpl;

	U8ImageRef HistoGram::GenDspImage(CvSize a_siz)
	{
		U8ImageRef ret;


		//ret = U8Image::Create( cvSize(256 * dx, 256 * dy), 3);
		ret = U8Image::Create( a_siz, 3);
		ret->SetAll(0);

		U8ChannelRef chnl0 = ret->GetAt(0);
		U8ChannelRef chnl1 = ret->GetAt(1);
		U8ChannelRef chnl2 = ret->GetAt(2);

		int nValDx = chnl0->GetWidth() / m_vals.size();

		int nMaxVal = 0;
		for(int i=0; i<(int)m_vals.size(); i++)
		{
			if(m_vals[i] > nMaxVal)
				nMaxVal = m_vals[i];
		}


		int nBgnX= (0.1/2.2) * chnl0->GetWidth();
		for(int i=0; i<(int)m_vals.size(); i++, nBgnX+=nValDx)
		{
			//int nValHeight = (int)((long long)m_vals[i] * chnl0->GetHeight() / m_nofPixs);

			int nTmp = (int)(long long)m_vals[i] * (chnl0->GetHeight()-20);
			int nValHeight = nTmp / nMaxVal;
			//nValHeight++;
			if(nTmp % nMaxVal > 0)
				nValHeight++;

			//if(nValHeight >= chnl0->GetHeight())
			//	nValHeight = chnl0->GetHeight() - 1;

			int nFact = (m_vals[i] > 0)? 1 : 128;
			for(int x=nBgnX; x<nBgnX+nValDx; x++)
			{
				for(int y=chnl0->GetHeight()-1-10; 
					y >= chnl0->GetHeight() - 1 - nValHeight-10; y--)
				{
					if(0 == i % 10)
					{
						chnl0->SetAt(x, y, nFact);
						chnl1->SetAt(x, y, 180 - nFact);
						chnl2->SetAt(x, y, 255 - nFact / 2);
					}
					else
					{
						chnl0->SetAt(x, y, nFact);
						chnl1->SetAt(x, y, 180 - nFact);
						chnl2->SetAt(x, y, 0);
					}
				}
			}
		}

		return ret;
	}

}