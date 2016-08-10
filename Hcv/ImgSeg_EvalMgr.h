#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

#include <Lib\Hcv\ScanDirMgr.h>
#include <Lib\Hcv\ScanDirMgrColl.h>
#include <Lib\Hcv\Signal1D.h>

#include <Lib\Hcv\IImgDataMgr.h>
#include <Lib\Hcv\ImgGradMgr.h>



namespace Hcv
{
	//using namespace Hcpl::Math;


	class ImgSeg_EvalMgr : FRM_Object
	{
	public:

		class HumanImg_ExpectedVal
		{
		public:

			int nImgNum;
			double expectedVal;
		};


	public:

		ImgSeg_EvalMgr();


		void Proceed();

		static void Save_ExpectedIndex_Text();

		//static double Calc_ExpectedIndex();
		static void Calc_ExpectedIndex();
		static void Calc_ExpectedIndex_2();

		void Set_AppSegMgr( ImgSegDataMgrRef a_arg )
		{
			m_appSegMgr = a_arg;
		}

		void Add_HumanSegMgr( ImgSegDataMgrRef a_arg )
		{
			m_humanSegMgr_Arr.PushBack( a_arg );
		}

		//void Reset( ImgSegDataMgrRef a_arg );
		void Reset();

		void Set_HumanSegMgr_Coll( CString a_collPath );

		double GetAvgIndexVal()
		{
			return m_avgIndexVal;
		}

		double GetMaxIndexVal()
		{
			return m_maxIndexVal;
		}


	protected:


	protected:
		

		FixedVector< ImgSegDataMgrRef > m_humanSegMgr_Arr;
		ImgSegDataMgrRef m_appSegMgr;

		double m_avgIndexVal;
		double m_maxIndexVal;
	};


	typedef Hcpl::ObjRef< ImgSeg_EvalMgr > ImgSeg_EvalMgrRef;

}

