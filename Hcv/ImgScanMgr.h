#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

#include <Lib\Hcv\ImageRotationMgr_Ex.h>


namespace Hcv
{
		//using namespace Hcpl::Math;

	namespace ImgScanMgr_Ns
	{

		typedef enum
		{
			Dif_Far = 0, 
			Dif_Loc,
			Dif_Loc_Norm,
			Dif_Loc_Max,
			Dif_Far_NormNbr_1,
			Dif_Far_NormNbr_2,
			PeakVal

		}DrawSignalType;

		class SigDraw : FRM_Object
		{
		public:

			SigDraw()
			{
				IsEnabled = true;
			}
			
			SigDraw( int a_nCapacity, U8ColorVal a_color )
			{
				DataArr.SetCapacity( a_nCapacity );
				Color = a_color;

				IsEnabled = true;
			}

			U8ColorVal Color;
			FixedVector< float > DataArr;
			bool IsEnabled;
		};

		typedef Hcpl::ObjRef< SigDraw > SigDrawRef;


		class RotationMgr_Ex_Coll : FRM_Object
		{
		public:

			RotationMgr_Ex_Coll( ImageRotationMgr_ExParamsRef a_rotExParams, 
				int a_nofRots );

			ImageRotationMgr_ExRef GetRotEx_At( int a_nRotIdx )
			{
				return m_rotExArr[ a_nRotIdx ];
			}

			int GetNofRots()
			{
				return m_rotExArr.GetSize();
			}

		protected:

			ImageRotationMgr_ExParamsRef m_rotExParams;
			int m_nofRots;

			FixedVector< ImageRotationMgr_ExRef > m_rotExArr;
		};

		typedef Hcpl::ObjRef< RotationMgr_Ex_Coll > RotationMgr_Ex_CollRef;



		class ScanDir : FRM_Object
		{
		public:

			ScanDir( ImageRotationMgr_ExRef a_rotMgr,
				int nDirIdx, int a_nNormIdx, int a_nRevIdx );

			CvSize GetSrcImgSiz()
			{
				return RotationMgr->GetCoreRotMgr(
					)->GetSrcImg()->GetSize();
			}
			
			ImageRotationMgr_ExRef RotationMgr;

			int nDirIdx;

			int nNormIdx;

			int nRevIdx;

			int * OrgToRot_Map_Buf;
			int * RotToOrg_Map_Buf;

			float * GradBuf_Far;
			float * GradBuf_Loc;

			float * ConfBuf_Far;

			ImageNbrMgr::PixIndexNbr * NbrBuf_Loc;

			float * BiMixGrad_Buf_Loc;

			ImageLineItrProvider * pItrProv;

			ScanDir * pNormSD;

			float * MaxGrad_Buf_Loc;
		};

		typedef Hcpl::ObjRef< ScanDir > ScanDirRef;



		class EdgeInfo : public MultiListQueMember< EdgeInfo >			 
		{
		public:

			int GetIOA_Org_E()
			{
				return m_scanDir->RotToOrg_Map_Buf[ nIOA_Rot_E ];
			}

			int GetIOA_Org_R1()
			{
				return m_scanDir->RotToOrg_Map_Buf[ nIOA_Rot_R1 ];
			}

			int GetIOA_Org_R2()
			{
				return m_scanDir->RotToOrg_Map_Buf[ nIOA_Rot_R2 ];
			}

			void Init()
			{
				nIndex = s_nIndex++;

				IsCanceled = false;
			}

			int nIndex;

			static int s_nIndex;

			int nIOA_Rot_E;
			int nIOA_Rot_R1;
			int nIOA_Rot_R2;
			
			ScanDirRef m_scanDir;

			bool IsCanceled;
		};

/*
		class EdgeInfoAcc;
		class EdgeInfoAcc : public MultiListQueMember< EdgeInfoAcc >			 
		{
		public:

			EdgeInfoAcc()
			{
				pEI = NULL;
			}

			EdgeInfo * pEI;

		};
*/


		class ScanDir_Coll : FRM_Object
		{
		public:

			ScanDir_Coll( RotationMgr_Ex_CollRef a_rotColl );

			ScanDirRef GetScanDir_At( int a_nDirIdx )
			{
				return m_scanDirArr[ a_nDirIdx ];
			}

			int GetNofScanDirs()
			{
				return m_scanDirArr.GetSize();
			}


			F32ImageRef GetMaxGrad_Loc_Img()
			{
				return m_maxLocDif_Img;
			}

		protected:

			void PrepareMaxLocDif_Stuff();

		protected:

			FixedVector< ScanDirRef > m_scanDirArr;

			F32ImageRef m_maxLocDif_Img;
		};

		typedef Hcpl::ObjRef< ScanDir_Coll > ScanDir_CollRef;


		class MergeRateElm
		{
		public:

			int nIOA_1;
			int nIOA_2;

			float Rate;
		};

		class PixLink
		{
		public:

			int nIOA_Peer;
			float Rate;
		};

		class PixLinking
		{
		public:

			int nIOA;			
			PixLink Links[ 4 ];
		};



	}



	class ImgScanMgr : FRM_Object
	{
	public:

		ImgScanMgr( ImageRotationMgr_ExParamsRef a_rotExParams, 
			int a_nofRots, CvPoint a_p1 );
		
		FixedVector< ImgScanMgr_Ns::EdgeInfo > & GetEdgeInfoArr()
		{
			return m_edgeInfoArr;
		}

		F32ImageRef GetMaxGrad_Loc_Img()
		{
			return m_scanDir_Coll->GetMaxGrad_Loc_Img();
		}

		FixedVector< ImgScanMgr_Ns::MergeRateElm > & GetMergeRateArr()
		{
			return m_mergeRateArr;
		}
		
		FixedVector< ImgScanMgr_Ns::PixLinking > & GetPixLinkingArr()
		{
			return m_pixLinkingArr;
		}

		//void FillMergeRateArr_FromDir( int a_nDirIdx );
		void FillPixLinkingArr();

		int GetNofDirs()
		{
			return m_scanDir_Coll->GetNofScanDirs();
		}

	protected:

		void Init( ImageRotationMgr_ExParamsRef a_rotExParams, 
			int a_nofRots, CvPoint a_p1 );

		void Proceed_0();
		void Proceed_01();
		void Proceed();

		void DrawSignals();



	protected:
		
		ImageRotationMgr_ExParamsRef m_rotExParams;
		//int m_nofRots;

		ImgScanMgr_Ns::RotationMgr_Ex_CollRef m_rotEx_Coll;

		ImgScanMgr_Ns::ScanDir_CollRef m_scanDir_Coll;

		CvPoint m_p1;

		int m_nIOL_DrawFocus;

		FixedVector< ImgScanMgr_Ns::SigDrawRef > m_sigDrawArr;
		FixedVector< ImgScanMgr_Ns::EdgeInfo > m_edgeInfoArr;

		FixedVector< ImgScanMgr_Ns::MergeRateElm > m_mergeRateArr;

		FixedVector< ImgScanMgr_Ns::PixLinking > m_pixLinkingArr;

		
		
	};


	typedef Hcpl::ObjRef< ImgScanMgr > ImgScanMgrRef;
}