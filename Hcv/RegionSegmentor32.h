#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

#include <Lib\Hcv\ConflictScanner2.h>
#include <Lib\Hcv\BadRootScanner.h>

#include <Lib\Hcv\SlideMgr.h>

#include <Lib\Hcv\ScanTryMgr2.h>



namespace Hcv
{
	using namespace Hcpl::Math;

/*
	class IRegionSegmentor32 : FRM_Object
	{	
	public:
		virtual S16ImageRef GenSegmentedImage(bool a_bShowMeanColor = true) = 0;
		virtual void Segment() = 0;
		virtual S16ImageRef GetLastOutputImg() = 0;
		virtual void ShowEdgeOfConflict() = 0;
	};

	typedef Hcpl::ObjRef< IRegionSegmentor32 > IRegionSegmentor32Ref;
*/


	class RegionSegmentor32 : public IRegionSegmentor20
	{
//////////////////////////////////////////////////////////////

		typedef enum
		{
			RC = 0, 
			RB, CB, LB,
			LC,		
			LT, CT, RT
		}RgnLinkDir;


	public:
		class LinkTypeMgr;

		class MgrOfLinkTypes
		{ 

		public:
			class CoreStuff
			{
			public:
				CoreStuff() : m_mapIndexCalc(8, 8), m_dxyIndexCalc(3, 3)
				{
					InitLinkTypeMgrs();
					InitMaps();
				}

				float GetDist(int a_i, int a_j)
				{
					return m_distMap[ 
						//m_mapIndexCalc.Calc(a_i, a_j) ];
						m_mapIndexCalc.Calc(a_j, a_i) ];
				}

				int GetNbr(int a_i, int a_j)
				{
					return m_nbrMap[ 
						//m_mapIndexCalc.Calc(a_i, a_j) ];
						m_mapIndexCalc.Calc(a_j, a_i) ];
				}

				int GetInverseLinkIndex(int a_index)
				{
					return (&m_linkTypeMgrVect[a_index])->GetInverseLinkTypeMgr()->GetIndex();
				}

				int GetLinkIndex(int a_dx, int a_dy)
				{
					int index = m_dxyIndexMap[ m_dxyIndexCalc.Calc( a_dx + 1, a_dy + 1) ];

					return index;
				}
				


			protected:
				void InitLinkTypeMgrs();				
				void InitMaps();

			protected:
				FixedVector<LinkTypeMgr> m_linkTypeMgrVect;
				float m_distMap[64];
				int m_nbrMap[64];
				IndexCalc2D m_mapIndexCalc;

				int m_dxyIndexMap[9];
				IndexCalc2D m_dxyIndexCalc;

			};


		public:
			static CoreStuff * GetCore()
			{
				static CoreStuff core;

				return &core;
			}


		protected:
			//static CoreStuff m_core;


		};


		class LinkTypeMgr
		{
			friend class MgrOfLinkTypes;

		public:

			F32Point GetDirXY()
			{
				return m_dirXY;
			}

			F32Point GetUnitDirXY()
			{
				return m_unitDirXY;
			}

			int GetIndex()
			{
				return (int)m_dir;
			}

			RgnLinkDir GetLinkDir()
			{
				return m_dir;
			}

			LinkTypeMgr * GetInverseLinkTypeMgr()
			{
				return m_pInverseLinkTypeMgr;
			}

		protected:
			void Init(LinkTypeMgr * a_pLTM0, int a_i, F32Point & a_dirXY)
			{
				m_dirXY = a_dirXY;
				m_unitDirXY = a_dirXY.GetUnitVect();
				m_dir = (RgnLinkDir)a_i;

				RgnLinkDir inverseDir;

				if( a_i < 4 )
				{
					inverseDir = (RgnLinkDir)( a_i + 4 );
				}
				else
				{
					inverseDir = (RgnLinkDir)( a_i - 4 );
				}

				m_pInverseLinkTypeMgr = &a_pLTM0[ (int)inverseDir ]; 
			}
		
		
		protected:
			F32Point m_dirXY;
			F32Point m_unitDirXY;
			RgnLinkDir m_dir;
			//RgnLinkDir m_inverseDir;
			LinkTypeMgr * m_pInverseLinkTypeMgr;
			//float * m_dists
		};













//////////////////////////////////////////////////////////////

	protected:

		struct _RgnInfo;
		struct _RgnLink;


	public:


		typedef struct _TraceUnit
		{
/*			struct _TraceUnit(
				struct _RgnInfo * pRgn,

				struct _TraceUnit * pFwdTU,
				struct _RgnLink * pFwdLink,

				struct _TraceUnit * pBkdTU,							
				struct _RgnLink * pBkdLink
					)
			{
				pFwdTU = NULL;
				pBkdTU = NULL;

				pFwdLink = NULL;
				pBkdLink = NULL;						
			}*/


/*			struct _TraceUnit()
			{
				pFwdTU = NULL;
				pBkdTU = NULL;

				pFwdLink = NULL;
				pBkdLink = NULL;						
			}*/

			struct _RgnInfo * pRgn;

			struct _TraceUnit * pFwdTU;
			struct _TraceUnit * pBkdTU;

			struct _RgnLink * pFwdLink;
			struct _RgnLink * pBkdLink;

			char Type;
			int Index;
		}TraceUnit;



		typedef struct _RgnConflict
		{
			struct _RgnInfo * pPeerRgn;

			struct _RgnInfo * pOrgEdge;
			struct _RgnInfo * pOrgR1;
			struct _RgnInfo * pOrgR2;


			struct _RgnConflict * pNext;	//	for being APtrList Entry
			struct _RgnConflict * pPrev;	//	for being APtrList Entry
		}RgnConflict;




		typedef struct _RgnLink
		{
			struct _RgnLink()
			{
				bInTrace = false;
			}

			struct _RgnInfo * pPeerRgn;
			RgnLinkDir dir;
			bool bProcessed;
			bool bExists;
			bool bInTrace;

			float DistMag;
			F32ColorVal UnitDist;

			struct _RgnLink * pPrev;	//	for being ListQue Entry
		}RgnLink;

		class RgnInfoListElm;

/*
			class RgnInfoListElm
			{
			public:
				struct _RgnInfo * pRgn;
				RgnInfoListElm * pNext;
			};
*/

		typedef struct _RgnInfo
		{
		protected:
			struct _RgnInfo * pActRgn;

			bool bIsRoot;
			//bool bInTrace;
			struct _RgnInfo * pRootRgn;

		public:


			struct _RgnInfo()
			{
				this->BeNotRoot();
				bShowLocalColor = false;
				//bShowLocalColor = true;
				bInTrace = false;

				maxTrace_BWR = 0;

				bInTraceAttachDone = false;

				bIsValleyRoot = false;
				
				nTraceProcID = -1;

				bIsPassiveRoot = false;

				pSrcRgn = NULL;

			}

			struct _RgnInfo * GetRootRgn()
			{
				return pRootRgn;
			}

			void SetRootRgn(struct _RgnInfo * a_pRootRgn)
			{
				//if( NULL != pRootRgn )
					//pRootRgn = pRootRgn;

				int a = 0;

				//if( 257 == this->pos.x && 4 == this->pos.y )
				if( 581 == this->pos.x && 345 == this->pos.y )
					a = 1;

				if(NULL != this->pRootRgn)
				{
					//ThrowHcvException();
					a = 1;
				}

				nMergeOrder = s_MergeOrder++;

				this->pRootRgn = a_pRootRgn;
			}

			struct _RgnInfo * GetActRgn()
			{
				Hcpl_ASSERT( NULL != pRootRgn );

				return pRootRgn->pActRgn;
			}

			void SetActRgn(struct _RgnInfo * a_pActRgn)
			{
				pRootRgn->pActRgn = a_pActRgn;

				if(pRootRgn != a_pActRgn)
					pRootRgn = pRootRgn;
			}

			bool IsRoot()
			{
				return this->bIsRoot;
			}


			void BeARoot()
			{
				int a = 0;

				if( 581 == this->pos.x && 345 == this->pos.y )
					a = 1;

				/*if( pos.x == 249 &&
					pos.y == 11 )
					pos.x = pos.x;*/

				if(NULL != this->pRootRgn)
					a = 1;

				//nMergeOrder = 0;
				nMergeOrder = s_MergeOrder++;

				this->pRootRgn = this;
				this->bIsRoot = true;
				this->SetActRgn( this );
			}

			void BeNotRoot()
			{
				this->pRootRgn = NULL;
				this->bIsRoot = false;
			}

			bool IsInMidTrace()
			{
				return ( -1 != maxAccXYDistFromRoot );
			}

			int nIndex;

			bool bShowLocalColor;

			bool bInTrace;

			// Max Trace Bad Width Ratio
			float maxTrace_BWR;

			//RgnLinkDir localGradDir;

			bool bIsValleyRoot;

			bool bInTraceAttachDone;

			int nTraceProcID;


			float distMagFromRoot;

			F32ColorVal * pixColors;

			F32ColorVal meanColors;
			F32ColorVal showColor;

			int nPixCnt;

			//int x;
			//int y;

			F32Point pos;

			//bool bIsInConflict;

			int nMaxDistMet;

			RgnLink links[8];
			APtrList< RgnConflict > conflictList;

			bool bHasConflicts;

			float accXYDistFromRoot;

			float maxAccXYDistFromRoot;

			float minDistFromRoot;

			bool bFromBadSrc;

			struct _RgnInfo * m_pDeepSrcBadRgn;

			bool bFromBadSrc_Org;			

			bool bFromSureBadSrc;

			bool bFstFromBadSrc;

			bool bInNativeTrace;

			EdgeScan::EdgeInfo * m_pMinDist_EI;

			RgnInfoListElm * pTraceRootElm;

			struct _RgnInfo * pTraceFwdRoot;
			struct _RgnInfo * pTraceBkdRoot;

			struct _RgnInfo * pSrcRgn;

			int nTraceID;

			int nMergeOrder;

			bool bIsPassiveRoot;

			static int s_MergeOrder;

			int nLastVisitID;

			struct _RgnInfo * pPrev;	//	for being ListQue Entry
		}RgnInfo;


		class RgnInfoListElm
		{
		public:
			RgnInfo * pRgn;
			RgnInfoListElm * pNext;
		};

		class EdgeInfoAccessor
		{
		public:

			EdgeInfoAccessor( RegionSegmentor32 * a_pSgm )
			{
				m_pSgm = a_pSgm;

				m_rgn_Buf = &(m_pSgm->m_rgnInfoVect)[ 0 ];
			}

			void SetEdgeInfo( EdgeScan::EdgeInfo * a_pEI )
			{
				m_pEI = a_pEI;

				m_ioaBuf = m_pEI->ioa_HeadPtr;
			}

			RgnInfo * GetRgnOf_IOL( int a_nIOL )
			{
				int nIOA = m_ioaBuf[ a_nIOL ];

				return & m_rgn_Buf[ nIOA ];
			}

			RgnInfo * GetR1_Rgn()
			{
				int nIOA = m_ioaBuf[ m_pEI->InrR1_IOL ];

				return & m_rgn_Buf[ nIOA ];
			}

			RgnInfo * GetR2_Rgn()
			{
				int nIOA = m_ioaBuf[ m_pEI->InrR2_IOL ];

				return & m_rgn_Buf[ nIOA ];
			}

			RgnInfo * GetMed1_Rgn()
			{
				int nIOA = m_ioaBuf[ m_pEI->Med1_IOL ];

				return & m_rgn_Buf[ nIOA ];
			}

			RgnInfo * GetMed2_Rgn()
			{
				int nIOA = m_ioaBuf[ m_pEI->Med2_IOL ];

				return & m_rgn_Buf[ nIOA ];
			}

			RgnInfo * GetEdge_Rgn()
			{
				int nIOA = m_ioaBuf[ m_pEI->EdgeIOL ];

				return & m_rgn_Buf[ nIOA ];
			}

		protected:

			RegionSegmentor32 * m_pSgm;

			
			RgnInfo * m_rgn_Buf;


			EdgeScan::EdgeInfo * m_pEI;

			int * m_ioaBuf;
		};

		friend class EdgeInfoAccessor;


		typedef struct _LinkAction : public MultiListQueMember< struct _LinkAction >
		{
			struct _LinkAction()
			{
				bShowTrace = false;
				bActionCanceled = false;
				bIntersected = false;
				nIndex = 0;
			}


			RgnInfo * pRgn1;
			RgnInfo * pRgn2;

			RgnLink * pLink1;
			RgnLink * pLink2;

			bool bShowTrace;
			bool bActionCanceled;
			bool bIntersected;

			int nIndex;


			//struct _LinkAction * pPrev;	//	for being ListQue Entry
		}LinkAction;





	public:
		RegionSegmentor32(F32ImageRef a_src, S16ImageRef a_rootImg,
			int a_nDifThreshold = 15, F32ImageRef a_rootValImg = NULL);

		S16ImageRef GenDifImg(bool a_bRC, bool a_bRB,
			bool a_bCB, bool a_bLB);

		S16ImageRef GenConflictImg(bool a_bRC, bool a_bRB,
			bool a_bCB, bool a_bLB);

		HistoGramRef GenDifHisto(bool a_bRC, bool a_bRB,
			bool a_bCB, bool a_bLB);

		virtual void Segment();
		virtual S16ImageRef GenSegmentedImage(bool a_bShowMeanColor = true);
		
		virtual F32ImageRef GenSegmentedImage2(bool a_bShowMeanColor = true);

		virtual S16ImageRef GetLastOutputImg()
		{
			return m_outImg;
		}

		virtual void ShowEdgeOfConflict();
		virtual void ShowValleyPath(int a_x, int a_y);
		virtual void ShowSrcPath(int a_x, int a_y);
		

	protected:

		float GetRgnGradVal( RgnInfo * a_pRgn )
		{
			return *m_srcGrad->GetPixAt( a_pRgn->pos.x, a_pRgn->pos.y );
		}

		void ScanImageLines();

		void InitLinks(void);
		void ProcessEdgeQues();
		void TraceLinkActionRoots( LinkAction * a_pLA );
			//RgnInfo ** a_ppRoot1, RgnInfo ** a_ppRoot2 );

		void RegionSegmentor32::AddFwdTraceUnit(RgnInfo * a_pRgn, int a_nFwdIndex, int a_nBkdIndex);
		void RegionSegmentor32::AddBkdTraceUnit(RgnInfo * a_pRgn, int a_nFwdIndex, int a_nBkdIndex);

		int GetInverseLinkIndex(int a_index)
		{
			return MgrOfLinkTypes::GetCore()->GetInverseLinkIndex(a_index);
		}

		void PrepareTraceListPtrs();




		void ProcessLinkActionTrace( LinkAction * a_pLA );
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
				//(float) ( 3 * Sqr(255)) )); 				
				(float) ( 3 * Sqr(500)) )); 				
				//(float) ( 3 * Sqr(255)) ) * 1.4); 				

			m_nMaxDif += m_nAprSize;
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
			RgnLink & a_rLink, RgnLink & a_rLink2, float a_distBef, bool a_bSetProcessed = true);

		//inline LinkAction * ProvideLinkAction(RgnInfo * a_pRgn, RgnLinkDir a_dir, bool a_bSetProcessed = true);
		inline LinkAction * ProvideLinkAction( RgnInfo * a_pRgn, RgnLinkDir a_dir );

		inline LinkAction * ProvideLinkAction( RgnInfo * a_pRgn, RgnInfo * a_pRgn2 );

		inline LinkAction * CloneLinkAction(LinkAction * a_pLA );

		inline bool HaveConflict( RgnInfo * a_pMinSizRgn, RgnInfo * a_pMaxSizRgn);

		inline RgnConflict * GetExistingConflict( RgnInfo * a_pMinSizRgn, RgnInfo * a_pMaxSizRgn);

		inline void RemoveDuplicateConflicts( RgnInfo * a_pRgn );

		inline void PrepareRgnLinkActions( RgnInfo * a_pRgn, float a_distBef );

		inline void CreateConflict( RgnInfo * a_pRgn1, RgnInfo * a_pRgn2, RgnInfo * pOrgEdge = NULL );

		void CreateConf_WithTrace( RgnInfo * a_pRgn1, EdgeScan::EdgeInfo * a_pEI );

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


		F32ImageRef GenScanImage();

		F32ImageRef GenRootRgnImage();

		F32ImageRef GenRgnRootDifImage();

		F32ImageRef GenMergeOrderImage();

		F32ImageRef Gen_FalseMergeImage();

		F32ImageRef Gen_FalseMergeImage_2();

		void MarkRgnsFromBadSrc();

		void ResetBadSrcWork();

		F32ImageRef GenBadSrcImage();

		void ProcessLineScanResult( RgnLinkDir a_dir );

		void PrepareSrcGradImg();



	protected:

		SlideMgrRef m_slideMgr;

		F32ImageRef m_src;
		F32ImageRef m_srcGrad;
		F32ImageRef m_orgSrc;
		F32ImageRef m_rootRgnImg;
		F32ImageRef m_slideRootDifImg;

		LinkAction * pLADbg; 

		int m_nMaxDif;
		IndexCalc2D m_rgnIndexCalc;

		FixedVector<RgnInfo> m_rgnInfoVect;
		FixedVector<RgnInfo *> m_medTraceRgnPtrVect;

		FixedVector< RgnInfoListElm > m_traceRootAllocVect;

		FixedVector<TraceUnit> m_edgeFwdRgnVect;
		FixedVector<TraceUnit> m_edgeBkdRgnVect;


		FixedVector<LinkAction> m_linkActionVect;
		int m_nofLinkActions;

		FixedVector<RgnConflict> m_rgnConflictVect;
		int m_nofConflicts;
		int m_nofPrimConfRgns;

		//MultiListQue< LinkAction > m_difQues;
		MultiListQueMgr< LinkAction > m_linkActionMergeQues;
		MultiListQueMgr< LinkAction > m_linkActionEdgeQues;


		//MultiListQue< LinkAction > m_difQues2;
		ListQue< RgnInfo > m_RgnOfConflictQue;
		float m_difThreshold;

		int m_nVisitID;

		S16ImageRef m_outImg;

		RgnInfo * m_pAnyRoot;

		RgnInfo * m_pDbgRgn;

		MultiListQue< LinkAction > m_medTraceQues;
		
		//F32ImageRef m_scanImg;

		ScanTryMgr2Ref m_scanTryMgr; 

		FixedVector< RgnInfo * > m_rgnInTraceArr;
		FixedVector< RgnInfo * > m_rgnOfBadSrcArr;

		FixedVector< RgnInfo * > m_rgnOf_FstBadSrcArr;
		

		int m_nTestRgnIdx;

		
		const int m_nAprSize;

		
		
	};


	typedef Hcpl::ObjRef< RegionSegmentor32 > RegionSegmentor32Ref;
}