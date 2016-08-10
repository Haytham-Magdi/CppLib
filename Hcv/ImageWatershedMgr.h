#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>


//#include <Lib\Hcv\IImgDataMgr.h>
//#include <Lib\Hcv\ImgDataElm_Simple.h>




namespace Hcv
{
	//using namespace Hcpl::Math;


	class RangeTester : FRM_Object
	{
	public:

		RangeTester( int a_nLen, int a_nMarg )
		{
			m_nLen = a_nLen;
			m_nMarg = a_nMarg;

			m_flagArr.SetSize( a_nLen + a_nMarg * 2 );

			for( int i=0; i < a_nMarg; i++ )
				m_flagArr[ i ] = false;

			for( int i = a_nMarg; i < a_nLen + a_nMarg; i++ )
				m_flagArr[ i ] = true;

			for( int i = a_nLen + a_nMarg; i < m_flagArr.GetSize(); i++ )
				m_flagArr[ i ] = false;
		}

		bool * GetTestBuf()
		{
			return & m_flagArr[ m_nMarg ];
		}
		

	protected:

		FixedVector< bool > m_flagArr;

		int m_nLen;
		int m_nMarg;

	};
	
	typedef Hcpl::ObjRef< RangeTester > RangeTesterRef;



	class ImageWatershed_Elm : public MultiListQueMember< ImageWatershed_Elm >			 
	{
	public:

		int Index;

		int X;
		int Y;

		int ScaledVal;

		ImageWatershed_Elm * pMaster;

		bool IsFlooded;

	};





	class ImageWatershedMgr : FRM_Object
	{
	public:

		ImageWatershedMgr(F32ImageRef a_gradImg, int a_nValScale);

		ImageWatershedMgr(float * a_gradBuf, CvSize a_imgSiz, int a_nValScale);


		void FloodToVal( float a_val );

		FixedVector< ImageWatershed_Elm > & GetDataArr()
		{
			return m_dataArr;
		}

		inline bool AreTogether( int a_nIdx_Iwe1, int a_nIdx_Iwe2 );


	protected:

		inline bool AreTogether( ImageWatershed_Elm * a_pIwe1, ImageWatershed_Elm * a_pIwe2 );

		void Init(float * a_gradBuf, CvSize a_imgSiz, int a_nValScale);

		void UpdateMasterElm(ImageWatershed_Elm * a_pIwe);

		void MergeElms( ImageWatershed_Elm * a_pIwe1, ImageWatershed_Elm * a_pIwe2 );



	protected:
		
		FixedVector< ImageWatershed_Elm > m_dataArr;

		MultiListQueMgr< ImageWatershed_Elm > m_elm_Ques;

		RangeTesterRef m_rangeTester_X;
		RangeTesterRef m_rangeTester_Y;

		CvSize m_imgSiz;
		
		float m_nValScale;
	};


	typedef Hcpl::ObjRef< ImageWatershedMgr > ImageWatershedMgrRef;

}