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

#include <Lib\Hcv\ScanTryMgr_4.h>

#include <Lib\Hcv\ImgDataElm_Simple.h>


namespace Hcv
{
	using namespace Hcpl::Math;

/*
	class IRegionSegmentor43 : FRM_Object
	{	
	public:
		virtual S16ImageRef GenSegmentedImage(bool a_bShowMeanColor = true) = 0;
		virtual void Segment() = 0;
		virtual S16ImageRef GetLastOutputImg() = 0;
		virtual void ShowEdgeOfConflict() = 0;
	};

	typedef Hcpl::ObjRef< IRegionSegmentor43 > IRegionSegmentor43Ref;
*/


	class RegionSegmentor43 : public IRegionSegmentor20
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

		class FillInfo
		{ 
		public:

			FillInfo()
			{
				bRootMet = false;
			}

			bool bRootMet;
		};


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

		class RgnInfo;
		class RgnLink;


	public:


		typedef struct _TraceUnit
		{
/*			struct _TraceUnit(
				RgnInfo * pRgn,

				struct _TraceUnit * pFwdTU,
				class RgnLink * pFwdLink,

				struct _TraceUnit * pBkdTU,							
				class RgnLink * pBkdLink
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

			RgnInfo * pRgn;

			struct _TraceUnit * pFwdTU;
			struct _TraceUnit * pBkdTU;

			class RgnLink * pFwdLink;
			class RgnLink * pBkdLink;

			char Type;
			int Index;
		}TraceUnit;



		typedef struct _RgnConflict
		{
			RgnInfo * pPeerRgn;

			RgnInfo * pOrgEdge;
			RgnInfo * pOrgR1;
			RgnInfo * pOrgR2;

			EdgeScan::EdgeInfo * pEI;

			struct _RgnConflict * pNext;	//	for being APtrList Entry
			struct _RgnConflict * pPrev;	//	for being APtrList Entry
		}RgnConflict;



		class LinkAction;


		class RgnLink
		{
		public:

			RgnLink()
			{
				bInTrace = false;
				//pInvLink = NULL;

				pCurLA = NULL;
			}

			//RgnInfo * pPeerRgn;

			RgnInfo * pRgn1;
			RgnInfo * pRgn2;

			RgnLink * GetInvLinkPtr()
			{
				return & pRgn2->links[ ( (int)dir + 4 ) % 8 ];
			}

			RgnLinkDir dir;
			bool bProcessed;
			bool bExists;
			bool bInTrace;

			float DistMag;
			F32ColorVal UnitDist;

			LinkAction * pCurLA;

			RgnLink * pPrev;	//	for being ListQue Entry
		};

		class RgnInfoListElm;

/*
			class RgnInfoListElm
			{
			public:
				RgnInfo * pRgn;
				RgnInfoListElm * pNext;
			};
*/

		class RgnInfo
		{
		protected:
			RgnInfo * pActRgn;

			bool bIsRoot;
			//bool bInTrace;
			RgnInfo * pRootRgn;

		public:


			RgnInfo()
			{
				this->BeNotRoot();
				bShowLocalColor = false;
				//bShowLocalColor = true;
				bInTrace = false;

				//bInTraceDam_Pos = false;
				//bInTraceDam_Neg = false;

				nTraceRank = 0;

				bFlag1 = false;

				bFlag2 = false;

				maxTrace_BWR = 0;

				bInTraceAttachDone = false;

				//minRootSpaceDist = 1000000;

				pShortestEI = NULL;

				bIsValleyRoot = false;
				
				nTraceProcID = -1;

				bIsPassiveRoot = false;

				pSrcRgn = NULL;

				pFillInfo = NULL;

				nFstBadGrpID = -1;

				nFill_Cycle = -1;
			}

			RgnInfo * GetRootRgn()
			{
				return pRootRgn;
			}

			void SetRootRgn(RgnInfo * a_pRootRgn)
			{
				//if( NULL != pRootRgn )
					//pRootRgn = pRootRgn;

				int a = 0;

				if( 101896 == this->nIndex 
					|| 101896 == a_pRootRgn->nIndex )
					a = 1;

				if( 122171 == this->nIndex )
					a = 1;

				if( 122171 == a_pRootRgn->nIndex )
					a = 1;


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

			RgnInfo * GetActRgn_2()
			{
				//Hcpl_ASSERT( NULL != pRootRgn );

				if( NULL == pRootRgn )
					return this;

				return pRootRgn->pActRgn;
			}

			RgnInfo * GetActRgn()
			{
				Hcpl_ASSERT( NULL != pRootRgn );

				return pRootRgn->pActRgn;
			}

			void SetActRgn(RgnInfo * a_pActRgn)
			{
				int a = 0;

				if( 101896 == this->nIndex 
					|| 101896 == a_pActRgn->nIndex )
					a = 1;

				pRootRgn->pActRgn = a_pActRgn;

				if(pRootRgn != a_pActRgn)
					pRootRgn = pRootRgn;
			}

			bool IsRoot()
			{
				//return this->bIsRoot;
				return this == pRootRgn;
			}


			void BeARoot()
			{
				int a = 0;

				if( 122171 == this->nIndex )
					a = 1;


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
				int a = 0;

				if( 101896 == this->nIndex )
					a = 1;

				this->pRootRgn = NULL;
				this->bIsRoot = false;
			}

			bool IsInMidTrace()
			{
				return ( -1 != maxAccXYDistFromRoot );
			}

			void IncPopulationBy( RgnInfo * a_pRgn2 )
			{
				//this->meanColors.IncBy( *a_pRgn2->pixColors );
				this->meanColors.IncBy( a_pRgn2->meanColors );
				this->nPixCnt += a_pRgn2->nPixCnt;

				this->dataElm_Mean.IncBy( a_pRgn2->dataElm_Mean );

				this->magSqr += a_pRgn2->magSqr;
			}


			ListQue< EdgeScan::EdgeInfoAcc > m_listQue_eiAcc;

			int nIndex;

			bool bShowLocalColor;

			bool bInTrace;

			//bool bInTraceDam_Pos;
			//bool bInTraceDam_Neg;

			int nTraceRank;
	

			//float minRootSpaceDist;

			EdgeScan::EdgeInfo * pShortestEI;

			// Max Trace Bad Width Ratio
			float maxTrace_BWR;

			//RgnLinkDir localGradDir;

			bool bIsValleyRoot;

			bool bInTraceAttachDone;

			int nTraceProcID;


			float distMagFromRoot;


			ImgDataElm_Simple * pDataElm;

			ImgDataElm_Simple dataElm_Mean;

			float magSqr;

			//m_avgSqrMag_Buf = & (pDataMgr_Exact->GetAvgSqrMag_Arr())[0];


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

			RgnInfo * m_pDeepSrcBadRgn;

			bool bFlag1;			

			bool bFlag2;			

			bool bFromBadSrc_Org;			

			bool bFrom_MaybeBadSrc;			

			bool bMaybe_FstFromBadSrc;			

			bool bFromSureBadSrc;

			bool bFstFromBadSrc;

			FillInfo * pFillInfo;

			int nFstBadGrpID;

			bool bInNativeTrace;

			bool bBadFillDone;
			
			bool bBad_UnfillDone;

			int nFill_Cycle;

			EdgeScan::EdgeInfo * m_pMinDist_EI;

			RgnInfoListElm * pTraceRootElm;

			bool bInIntersection;

			RgnInfo * pTraceFwdRoot;
			RgnInfo * pTraceBkdRoot;

			RgnInfo * pSrcRgn;

			int nTraceID;

			int nMergeOrder;

			bool bIsPassiveRoot;

			static int s_MergeOrder;

			int nLastVisitID;

			RgnInfo * pPrev;	//	for being ListQue Entry
		};


		class RgnInfoListElm
		{
		public:
			RgnInfo * pRgn;
			RgnInfoListElm * pNext;
		};

		class EdgeInfoAccessor
		{
		public:

			EdgeInfoAccessor( RegionSegmentor43 * a_pSgm )
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

			RegionSegmentor43 * m_pSgm;

			
			RgnInfo * m_rgn_Buf;


			EdgeScan::EdgeInfo * m_pEI;

			int * m_ioaBuf;
		};

		friend class EdgeInfoAccessor;


		class LinkAction : public MultiListQueMember< LinkAction >
		{
		public:

			LinkAction()
			{
				bShowTrace = false;
				bActionCanceled = false;
				bIntersected = false;
				nIndex = 0;
				bIsActive = false;
			}


			//RgnInfo * pRgn1;
			//RgnInfo * pRgn2;

			RgnLink * pLink1;
			//RgnLink * pLink2;

			bool bIsActive;

			bool bShowTrace;
			bool bActionCanceled;
			bool bIntersected;

			int nScaledDist;

			int nIndex;

		};





	public:
		RegionSegmentor43(F32ImageRef a_src, S16ImageRef a_rootImg,
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

		void Manage_EI_Activation( RgnInfo * a_pRgn_1, RgnInfo * a_pRgn_2 );

		float GetRgnGradVal( RgnInfo * a_pRgn )
		{
			return *m_srcGrad->GetPixAt( a_pRgn->pos.x, a_pRgn->pos.y );
		}

		void ScanImageLines();

		void InitLinks(void);
		void ProcessEdgeQues();
		void TraceLinkActionRoots( LinkAction * a_pLA );
			//RgnInfo ** a_ppRoot1, RgnInfo ** a_ppRoot2 );

		void RegionSegmentor43::AddFwdTraceUnit(RgnInfo * a_pRgn, int a_nFwdIndex, int a_nBkdIndex);
		void RegionSegmentor43::AddBkdTraceUnit(RgnInfo * a_pRgn, int a_nFwdIndex, int a_nBkdIndex);

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

		inline void PrepareLinkAction( RgnLink & a_rLink, float a_distBef, 
			bool a_bSetProcessed = true);

		//inline LinkAction * ProvideLinkAction(RgnInfo * a_pRgn, RgnLinkDir a_dir, bool a_bSetProcessed = true);
		inline LinkAction * ProvideLinkAction( RgnInfo * a_pRgn, RgnLinkDir a_dir );

		inline LinkAction * ProvideLinkAction( RgnInfo * a_pRgn, RgnInfo * a_pRgn2 );

		//inline LinkAction * CloneLinkAction(LinkAction * a_pLA );

		inline bool HaveConflict( RgnInfo * a_pMinSizRgn, RgnInfo * a_pMaxSizRgn);

		inline void Push_LA_To_MergeQues( int a_nIndex, LinkAction * pLA );

		inline LinkAction * PopMinPtr_From_MergeQues();

		inline LinkAction * PopMaxPtr_From_MergeQues();

		inline RgnConflict * GetExistingConflict( RgnInfo * a_pMinSizRgn, RgnInfo * a_pMaxSizRgn);

		inline void RemoveDuplicateConflicts( RgnInfo * a_pRgn );

		inline void PrepareRgnLinkActions( RgnInfo * a_pRgn, float a_distBef );

		inline void CreateConflict( RgnInfo * a_pRgn1, RgnInfo * a_pRgn2, RgnInfo * pOrgEdge = NULL, EdgeScan::EdgeInfo * a_pEI = NULL );

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

		F32ImageRef GenTraceDamImage();

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

		RgnLink * GetLinkBetweenRgns( int a_nIdx_1, int a_nIdx_2 );



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

		
		FixedVector< FillInfo > m_fillInfoVect;

		FixedVector<RgnInfo> m_rgnInfoVect;
		FixedVector<RgnInfo *> m_medTraceRgnPtrVect;

		FixedVector< RgnInfoListElm > m_traceRootAllocVect;

		FixedVector<TraceUnit> m_edgeFwdRgnVect;
		FixedVector<TraceUnit> m_edgeBkdRgnVect;


		//FixedVector<LinkAction> m_linkActionVect;

		PtrPrvider< LinkAction > m_linkAction_PtrPrvider;
		PtrPrvider< EdgeScan::EdgeInfoAcc > m_eiAcc_PtrPrvider;

		//int m_nofLinkActions;

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


		MultiListQueMgr< EdgeScan::EdgeInfo > m_edgeInfo_Ques;

		
		//F32ImageRef m_scanImg;

		ScanTryMgr_4Ref m_scanTryMgr; 
		//ScanTryMgr2Ref m_scanTryMgr; 

		float * m_dir_difBuf_Arr[8];

		float * m_maxDif_Buf;

		EdgeScan::PixInfo * m_scanPix_Buf;


		ImgAvgingMgrRef m_imgAvgingMgr;

		IImgDataMgrRef m_imgDataMgr;

		float * m_standDiv_Buf;

		ImgDataElm_Simple * m_dataElm_Buf;

		ImgDataElm_Simple * m_dataElm_Mean_Buf;

		float * m_avgSqrMag_Buf;


		FixedVector< RgnInfo * > m_rgnInTraceArr;
		FixedVector< RgnInfo * > m_rgnOfBadSrcArr;

		FixedVector< RgnInfo * > m_rgnOf_FstBadSrcArr;

		int m_nCurGrpID;

		FixedVector< RgnInfo * > m_rgnOf_FstBadSrcArr_2;

		FixedVector< RgnInfo * > m_rgnOf_OpenEnd_Arr;

		FixedDeque< RgnInfo * > m_fillDeq;

		FixedDeque< RgnInfo * > m_unfill_Deq;

		FixedVector< EdgeScan::EdgeInfo * > m_fstFromBad_EI_Arr;

		FixedVector< EdgeScan::EdgeInfo * > m_shortest_FromBad_EI_Arr;

		int m_nTestRgnIdx;

		
		const int m_nAprSize;

		
		
	};


	typedef Hcpl::ObjRef< RegionSegmentor43 > RegionSegmentor43Ref;
}