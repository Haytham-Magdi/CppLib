#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

namespace Hcv
{
	class RegionSegmentor : FRM_Object
	{
	public:
		RegionSegmentor(S16ImageRef a_src);

		S16ImageRef GetDifRCImg(void)
		{		
			return m_difRC;
		}

		S16ImageRef GetDifRBImg(void)
		{		
			return m_difRB;
		}

		S16ImageRef GetDifCBImg(void)
		{		
			return m_difCB;
		}
		
		S16ImageRef GetDifLBImg(void)
		{		
			return m_difLB;
		}

		void CalcDiffs(void);

	protected:
			void ResetQueSizeVects(void);


	protected:
		S16ImageRef m_src;
		short m_nMaxDif;

		S16ImageRef m_difRC;
		S16ImageRef m_difRB;
		S16ImageRef m_difCB;
		S16ImageRef m_difLB;

		std::vector<int> m_queSizVectRC;
		std::vector<int> m_queSizVectRB;
		std::vector<int> m_queSizVectCB;
		std::vector<int> m_queSizVectLB;

		MultiQue<int> m_multiQueRC;
		MultiQue<int> m_multiQueRB;
		MultiQue<int> m_multiQueCB;
		MultiQue<int> m_multiQueLB;
	};


	typedef Hcpl::ObjRef< RegionSegmentor > RegionSegmentorRef;
}