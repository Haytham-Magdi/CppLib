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


	class ImgSegDataMgr;

	typedef Hcpl::ObjRef< ImgSegDataMgr > ImgSegDataMgrRef;


	class ImgSegDataMgr : FRM_Object
	{
	public:

		ImgSegDataMgr( char * a_sFilePath );


		int GetWidth()			
		{
			return m_nWidth;
		}

		int GetHeight()			
		{
			return m_nHeight;
		}

		int GetNofSegments()			
		{
			return m_nofSegments;
		}
		
		FixedVector< int > & GetPixArr()
		{
			return m_pixArr;
		}

		void ShowSegmImg( char * a_sWindowName = NULL );


		double CalcRandIndex( ImgSegDataMgrRef a_peer );
		//float CalcRandIndex( ImgSegDataMgr a_peer );


		char * GetFileName()
		{
			return m_sFileName;
		}



	protected:

		void Proceed();


	protected:

		char m_sFilePath[2000];
		char m_sFileName[1000];

		int m_nWidth;
		int m_nHeight;
		int m_nofSegments;

		FixedVector< int > m_pixArr;

	};



}

