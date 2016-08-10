#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

#define DIFFACT			1

namespace Hcv
{
	using namespace Hcpl::Math;

	class RegionSegmentor : FRM_Object
	{
	protected:

		typedef struct _RgnInfo
		{
			struct _RgnInfo * pActRgn;

			int x;
			int y;

			int nofPixs;

			short nLocalCh0;
			short nLocalCh1;
			short nLocalCh2;

			float nMeanCh0;
			float nMeanCh1;
			float nMeanCh2;

		}RgnInfo;

		typedef enum
		{
			RC = 0, RB, CB, LB
		}RgnLinkDir;

		typedef struct _RgnLink
		{
			RgnInfo * pRgn1;
			RgnInfo * pRgn2;
			RgnLinkDir dir;

			struct _RgnLink * pPrev;	//	for being QueList Entry
		}RgnLink;

	public:
		RegionSegmentor(S16ImageRef a_src, float a_localRatio = 0.3);

		S16ImageRef GenDifImg(bool a_bRC, bool a_bRB,
			bool a_bCB, bool a_bLB, int a_nMinDif = 0, int a_nMaxDif = -1);

		HistoGramRef GenDifHisto(bool a_bRC, bool a_bRB,
			bool a_bCB, bool a_bLB);

		void Segment(int a_nMergDif);
		S16ImageRef GenSegmentedImage(void);


	protected:
		void InitDifQues(void);
		void InitRgnInfoVect(void);

		short GetMaxDif() { return m_nMaxDif; }

		void InitMaxDif() 
		{ 
			m_nMaxDif = (short)sqrt( 
				(float) ( 3 * Sqr(255 * DIFFACT)) ) + 1; 
		}

		inline short CalcRgnDif(RgnInfo * a_pRgn1, RgnInfo * a_pRgn2);
		inline void MergRgns(RgnInfo * a_pActRgn1, RgnInfo * a_pActRgn2);

		inline void MergLink(RgnLink * a_pLink)
		{
			MergRgns(a_pLink->pRgn1->pActRgn, a_pLink->pRgn2->pActRgn);
		}

		short CalcLinkDif(RgnLink * a_pLink)
		{
			return CalcRgnDif(a_pLink->pRgn1, a_pLink->pRgn2);
		}

		inline RgnLink * CreateLink(
			RgnInfo * a_pRgn, RgnLinkDir a_dir);

		RgnInfo * GetPointRgn(int x, int y)
		{ 
			return &m_rgnInfoVect[m_rgnIndexCalc.Calc(x, y)]; 
		}

		inline void UpdateActRgn(RgnInfo * a_pRgn);

	protected:
		S16ImageRef m_src;
		short m_nMaxDif;
		IndexCalc2D m_rgnIndexCalc;

		std::vector<RgnInfo> m_rgnInfoVect;

		std::vector<RgnLink> m_rgnLinkVect;
		int m_nofLinks;

		MultiListQue< RgnLink > m_difQues;

		float m_localRatio;
		float m_meanRatio;
	};


	typedef Hcpl::ObjRef< RegionSegmentor > RegionSegmentorRef;
}