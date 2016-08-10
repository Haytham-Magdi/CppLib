#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

namespace Hcv
{
	using namespace Hcpl::Math;

	class RegionSegmentor12 : FRM_Object
	{
	protected:

		struct _RgnInfo;


		typedef struct _RgnConflict
		{
			struct _RgnInfo * pPeerRgn;

			struct _RgnConflict * pNext;	//	for being APtrList Entry
			struct _RgnConflict * pPrev;	//	for being APtrList Entry
		}RgnConflict;

		typedef struct _RgnLink
		{
			struct _RgnInfo * pPeerRgn;
			//RgnLinkDir dir;
			int nDist;
			bool bProcessed;
			bool bExists;

			struct _RgnLink * pPrev;	//	for being ListQue Entry
		}RgnLink;

		typedef struct _RgnInfo
		{
		protected:
			struct _RgnInfo * pActRgn;

		public:
			struct _RgnInfo * GetActRgn()
			{
				return pRootRgn->pActRgn;
			}

			void SetActRgn(struct _RgnInfo * a_pActRgn)
			{
				pRootRgn->pActRgn = a_pActRgn;
			}

			struct _RgnInfo * pRootRgn;

			short nValCh0;
			short nValCh1;
			short nValCh2;

			int nMeanValCh0;
			int nMeanValCh1;
			int nMeanValCh2;

			int nPixCnt;

			int x;
			int y;

			bool bIsRoot;

			RgnLink links[8];
			APtrList< RgnConflict > conflictList;

			bool bHasConflicts;

			int nLastVisitID;

			struct _RgnInfo * pPrev;	//	for being ListQue Entry
		}RgnInfo;

		typedef enum
		{
			RC = 0, RB, CB, LB
		}RgnLinkDir;

		typedef struct _LinkAction
		{
			RgnInfo * pRgn1;
			RgnInfo * pRgn2;

			struct _LinkAction * pPrev;	//	for being ListQue Entry
		}LinkAction;

	public:
		RegionSegmentor12(S16ImageRef a_src, S16ImageRef a_rootImg,
			int a_nDifThreshold = 15);

		S16ImageRef GenDifImg(bool a_bRC, bool a_bRB,
			bool a_bCB, bool a_bLB);

		S16ImageRef GenConflictImg(bool a_bRC, bool a_bRB,
			bool a_bCB, bool a_bLB);

		HistoGramRef GenDifHisto(bool a_bRC, bool a_bRB,
			bool a_bCB, bool a_bLB);

		void Segment();
		S16ImageRef GenSegmentedImage(void);


	protected:
		void InitLinks(void);
		void InitRgnInfoVect(void);
		void PrepareConflicts();
		void PrepareRgnConflicts( RgnInfo * a_pRgn );

		short GetMaxDif() { return m_nMaxDif; }

		void InitMaxDif() 
		{ 
			m_nMaxDif = (short)sqrt( 
				(float) ( 3 * Sqr(255)) ); 
		}

		inline int CalcRgnDif(RgnInfo * a_pRgn1, RgnInfo * a_pRgn2);
		inline bool IsConflictDist(int a_nDist)
		{
			return (a_nDist >= m_nDifThreshold);
		}

		inline void CreateLink( RgnInfo * a_pRgn, 
			int x, int y, RgnLinkDir a_dir);

		RgnInfo * GetPointRgn(int x, int y)
		{ 
			return &m_rgnInfoVect[m_rgnIndexCalc.Calc(x, y)]; 
		}

		inline void UpdateActRgn(RgnInfo * a_pRgn);

		inline void PrepareLinkAction(RgnInfo * a_pRgn, RgnInfo * a_pRgn2,
			RgnLink & a_rLink, RgnLink & a_rLink2);

		inline bool HaveConflict( RgnInfo * a_pMinSizRgn, RgnInfo * a_pMaxSizRgn);
		inline void RemoveDuplicateConflicts( RgnInfo * a_pRgn );

		inline void PrepareRgnLinkActions( RgnInfo * a_pRgn );

		inline void CreateConflict( RgnInfo * a_pRgn1, RgnInfo * a_pRgn2 );

		bool PointInImage( int x, int y )
		{
			if( 
				x < 0 ||
				x > m_rgnIndexCalc.GetSizeX() - 1 ||
				y < 0 ||
				y > m_rgnIndexCalc.GetSizeY() - 1 
				)
				return false;
			else
				return true;
		}

		



	protected:
		S16ImageRef m_src;
		S16ImageRef m_rootImg;		
		short m_nMaxDif;
		IndexCalc2D m_rgnIndexCalc;

		std::vector<RgnInfo> m_rgnInfoVect;

		std::vector<LinkAction> m_linkActionVect;
		int m_nofLinkActions;

		std::vector<RgnConflict> m_rgnConflictVect;
		int m_nofConflicts;
		int m_nofPrimConfRgns;

		MultiListQue< LinkAction > m_difQues;
		//MultiListQue< LinkAction > m_difQues2;
		ListQue< RgnInfo > m_RgnOfConflictQue;
		int m_nDifThreshold;

		int m_nDifIndex;
		int m_nVisitID;

		
	};


	typedef Hcpl::ObjRef< RegionSegmentor12 > RegionSegmentor12Ref;
}