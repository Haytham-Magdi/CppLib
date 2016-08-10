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

	class RegionSegmentor17 : FRM_Object
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
			bool bProcessed;
			bool bExists;

			float DistMag;
			F32ColorVal UnitDist;

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

			F32ColorVal * pixColors;

			F32ColorVal meanColors;

			int nPixCnt;

			int x;
			int y;

			bool bIsRoot;
			bool bIsEdge;
			bool bIsInConflict;

			bool bIsN2E;
			bool bIsE2E;

			int nMaxDistMet;

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



		class LinkActionQueMgr
		{
		public:

			LinkActionQueMgr()
			{
				m_nLastMinIndex = 0;
				m_nLastMaxIndex = 0;
			}

			void InitSize(int a_nSiz)
			{
				m_multiQues.Init(a_nSiz);
			}

			int GetNofQues()
			{
				return m_multiQues.GetNofQues();
			}

			LinkAction * PopPtrMin()
			{
				LinkAction * pLA = m_multiQues.PopPtr( m_nLastMinIndex );

				while( NULL == pLA )
				{
					if(m_nLastMinIndex < m_nLastMaxIndex)
					{
						m_nLastMinIndex++;
						pLA = m_multiQues.PopPtr( m_nLastMinIndex );
					}
					else
					{
						break;
					}
				}

				return pLA;
			}

			LinkAction * PopPtrMax()
			{
				LinkAction * pLA = m_multiQues.PopPtr( m_nLastMaxIndex );

				while( NULL == pLA )
				{
					if(m_nLastMaxIndex < m_nLastMinIndex)
					{
						m_nLastMaxIndex--;
						pLA = m_multiQues.PopPtr( m_nLastMaxIndex );
					}
					else
					{
						break;
					}
				}

				return pLA;
			}   

			void PushPtr(int a_nIndex, LinkAction * a_pLA)
			{
				m_multiQues.PushPtr(a_nIndex, a_pLA);

				if(a_nIndex < m_nLastMinIndex)
					m_nLastMinIndex = a_nIndex;

				if(a_nIndex > m_nLastMaxIndex)
					m_nLastMaxIndex = a_nIndex;

			}			

			int GetLastMinIndex()
			{
				return m_nLastMinIndex;
			}

			int GetLastMaxIndex()
			{
				return m_nLastMaxIndex;
			}


		protected:

			MultiListQue< LinkAction > m_multiQues;

			int m_nLastMinIndex;
			int m_nLastMaxIndex;
		};


	public:
		RegionSegmentor17(F32ImageRef a_src, S16ImageRef a_rootImg,
			int a_nDifThreshold = 15, F32ImageRef a_rootValImg = NULL);

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

		int GetMaxDif() { return m_nMaxDif; }
		float m_difScale;

		void InitMaxDif() 
		{ 
			m_difScale = 100;
			//m_difScale = 1;

			m_nMaxDif =  (int)( m_difScale *  
				sqrt( 
			//m_nMaxDif =  ( (
				(float) ( 3 * Sqr(255)) )); 				
				//(float) ( 3 * Sqr(255)) ) * 1.4); 				
		}

		inline float CalcRgnDif(RgnInfo * a_pRgn1, RgnInfo * a_pRgn2);

		// With Space
		inline float CalcRgnDifWS(RgnInfo * a_pRgn1, RgnInfo * a_pRgn2);
		inline bool IsConflictDist(float a_dist)
		{			
			return  ( a_dist >= (m_difThreshold * m_difScale) );
		}

		inline void CreateLink( RgnInfo * a_pRgn, 
			int x, int y, RgnLinkDir a_dir);

		RgnInfo * GetPointRgn(int x, int y)
		{ 
			return &m_rgnInfoVect[m_rgnIndexCalc.Calc(x, y)]; 
		}

		inline void UpdateActRgn(RgnInfo * a_pRgn);

		inline void PrepareLinkAction(RgnInfo * a_pRgn, RgnInfo * a_pRgn2,
			RgnLink & a_rLink, RgnLink & a_rLink2, float a_distBef);

		inline bool HaveConflict( RgnInfo * a_pMinSizRgn, RgnInfo * a_pMaxSizRgn);
		inline void RemoveDuplicateConflicts( RgnInfo * a_pRgn );

		inline void PrepareRgnLinkActions( RgnInfo * a_pRgn, float a_distBef );

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
		F32ImageRef m_src;
		F32ImageRef m_orgSrc;
		S16ImageRef m_rootImg;		
		F32ImageRef m_rootValImg;

		int m_nMaxDif;
		IndexCalc2D m_rgnIndexCalc;

		std::vector<RgnInfo> m_rgnInfoVect;

		std::vector<LinkAction> m_linkActionVect;
		int m_nofLinkActions;

		std::vector<RgnConflict> m_rgnConflictVect;
		int m_nofConflicts;
		int m_nofPrimConfRgns;

		//MultiListQue< LinkAction > m_difQues;
		LinkActionQueMgr m_linkActionMergeQues;


		//MultiListQue< LinkAction > m_difQues2;
		ListQue< RgnInfo > m_RgnOfConflictQue;
		float m_difThreshold;

		int m_nVisitID;

		
	};


	typedef Hcpl::ObjRef< RegionSegmentor17 > RegionSegmentor17Ref;
}