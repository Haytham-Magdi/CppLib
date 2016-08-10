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


	class CBIS_Berkeley_Executer : FRM_Object
	{
	public:

		class ImgMth_SegVal
		{
			public:

				int nImgNum;
				double expected_Val;

				double indexVal_Avg;
				double indexVal_Max;

				double npr_Avg;
				double npr_Max;
		};




	public:

		CBIS_Berkeley_Executer();


		void Proceed();

		void Set_MethodDirPath( char * a_methodDirPath );

	protected:


	protected:

		FixedVector< char > m_methodDirPath_BufArr;

	};


	typedef Hcpl::ObjRef< CBIS_Berkeley_Executer > CBIS_Berkeley_ExecuterRef;

}

