#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

#include <Lib\Hcv\IImgDataMgr_2.h>
//#include <Lib\Hcv\GridPal\ImgDataElm_GridPal.h>

#include <Lib\Hcv\GridColorPalette.h>




namespace Hcv
{
	//using namespace Hcpl::Math;


	class ImgDataSrc_GridPal : public IImgCoreSrc
	{
	public:

/*
		class PixEnt : public MultiListQueMember< PixEnt >
		{
		public:

			PixEnt()
			{
				nIndex = -10;
				nClustIndex = -10;
				bFlag_1 = false;
			}

		public:

			int nIndex;
			int nClustIndex;
			bool bFlag_1;
		};

		class Cluster
		{
		public:

			Cluster()
			{
				MeanColorSum.AssignVal( 0, 0, 0 );
				NofPixes = 0;
				SqrMagSum = 0;
				StanDev = -10;
			}

			void AddColor( F32ColorVal & rColor )
			{
				MeanColorSum.IncBy( rColor );
				SqrMagSum += rColor.CalcMagSqr();
				NofPixes++;
			}

			F32ColorVal GetMeanColor()
			{
				return MeanColorSum.DividBy( NofPixes );
			}

			void PrepareStanDev()
			{
				//StanDev = SqrMagSum - MeanColorSum.CalcMagSqr();
				//StanDev /= NofPixes;

				StanDev = SqrMagSum / NofPixes;
				StanDev -= MeanColorSum.DividBy( NofPixes ).CalcMagSqr();

				if( StanDev < 0 )
					StanDev = 0;

				StanDev = sqrt( (float) StanDev );
			}

			F32ColorVal MeanColorSum;
			float SqrMagSum;
			int NofPixes;
			F32ColorVal OrgColor;

			float StanDev;
		};
*/
/*
		class ClustShare
		{
		public:

			ClustShare()
			{
				nofPixes = 0;
			}

		public:

			int nClustIdx;
			int nofPixes;
		};
*/


		//class ShareEx : public ClustShare
		class ShareEx : public GridColorPalette::Share			
		{
		public:

			ShareEx()
			{
				pPrev = NULL;
				pNext = NULL;
			}

		public:

			ShareEx * pPrev;
			ShareEx * pNext;
		};


		class ClustHisto
		{
		public:

			//void Init( FixedVector< Cluster > & a_rClusterArr );
			void Init();

			void IncAt( int a_nIdx, int a_nIncVal );
			void DecAt( int a_nIdx, int a_nDecVal );

			void IncBy( GridColorPalette::ColorRep & a_rRep );
			void DecBy( GridColorPalette::ColorRep & a_rRep );

			void Reset();

		protected:

			void AddShareToList( ShareEx & a_rShare );
			void RemoveShareFromList( ShareEx & a_rShare );

		public:

			ShareEx * m_pFstShare;
			int m_nListSize;

			int m_nMaxListSiz;

			FixedVector< ShareEx > m_clustShareArr;
			//FixedVector< Cluster > * m_pClusterArr;
		};



	public:

		ImgDataSrc_GridPal( F32ImageRef a_srcImg );

	protected:

		void Proceed();

		int GetMaxStanDev_Scaled()
		{
			int ret = 600 * m_nDifScale;
			return ret;
		}

		F32ImageRef GenResultImg();
		

	public:

		ClustHisto m_clustHisto;

		int m_nDifScale;

		F32ImageRef m_srcImg;
		F32ImageRef m_meanImg;
		F32ImageRef m_sqrMagMeanImg;
		F32ImageRef m_stanDevImg;

		//FixedVector< Cluster > m_clusterArr;

		//FixedVector< PixEnt > m_pixEntArr;

		//MultiListQueMgr< PixEnt > m_pixEntQues;



	};



	class ImgDataMgr_2_GridPal_Factory : public IImgDataMgr_2_Factory
	{
	public:

		virtual IImgDataMgr_2 * CreateImgDataMgr( 
			 F32ImageRef a_srcImg, int a_nAprSiz, 
			 IImgCoreSrcRef a_dataSrc,
			 ImgRotationMgrRef a_rot );
	};

	typedef Hcpl::ObjRef< ImgDataMgr_2_GridPal_Factory > ImgDataMgr_2_GridPal_FactoryRef;



	class ImgDataMgr_2_GridPal : public IImgDataMgr_2
	{
	public:

		ImgDataMgr_2_GridPal( F32ImageRef a_srcImg, int a_nAprSiz,
			IImgCoreSrcRef a_dataSrc, ImgRotationMgrRef a_rot );

		virtual F32ImageRef GetSrcImg()
		{
			return m_srcImg;
		}

		virtual int GetAprSiz()
		{
			return m_nAprSiz;
		}
		
		ImgDataSrc_GridPal * GetDataSrc()
		{
			return m_pDataSrc_LH;
		}

		void PrepareExData();
		void CleanExData();

	protected:

		void PrepareData();

		F32ImageRef GenResImg();

		void PreparePixEntArr();
		

		void AddToArrColl( 
			Hcpl::CommonArrCollBuff< GridColorPalette::Share > & rPixShareArrColl );

		void PrepareArrColl_V();
		void PrepareArrColl_Tot();

		void IncHistoFrom_V( int a_nVI );
		void DecHistoFrom_V( int a_nVI );


	//protected:
	public:

		F32ImageRef m_srcImg;
		int m_nAprSiz;

		F32ImageRef m_meanImg;
		F32ImageRef m_magSqrImg;
		F32ImageRef m_stanDevImg;


		ImgRotationMgrRef m_rot;
		
		IImgCoreSrcRef m_dataSrc;
		ImgDataSrc_GridPal * m_pDataSrc_LH;

		ImgDataSrc_GridPal::ClustHisto * m_pClustHisto;

		//FixedVector< ImgDataSrc_GridPal::PixEnt > m_pixEntArr;

		Hcpl::CommonArrCollBuff< GridColorPalette::Share > m_pixShareArrColl_V;
		Hcpl::CommonArrCollBuff< GridColorPalette::Share > m_pixShareArrColl_Tot;

	};

	typedef Hcpl::ObjRef< ImgDataMgr_2_GridPal > ImgDataMgr_2_GridPalRef;



}