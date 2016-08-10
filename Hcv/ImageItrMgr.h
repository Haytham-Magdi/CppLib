#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

#include <Lib\Hcv\ImageLineItr.h>

#include <Lib\Hcv\ImageLineItrProvider.h>



namespace Hcv
{
	//using namespace Hcpl::Math;


	class ImageItrMgr : FRM_Object
	{
	public:

		ImageItrMgr() {}

		ImageItrMgr( CvSize a_imgSiz,
			FixedVector< LineLimit > & a_lineLimit_H_Arr, 
			FixedVector< LineLimit > & a_lineLimit_V_Arr )
		{
			Init( a_imgSiz, a_lineLimit_H_Arr, a_lineLimit_V_Arr );
		}

		void Init( CvSize a_imgSiz,
			FixedVector< LineLimit > & a_lineLimit_H_Arr, 
			FixedVector< LineLimit > & a_lineLimit_V_Arr );

		CvSize GetImgSiz()
		{
			return m_imgSiz;
		}

		//void PrepareProvArr();

		ImageLineItrProvider & GetItrProvAt( int a_nProvIdx )
		{
			return m_lineItrProv_Arr[ a_nProvIdx ];
		}

	protected:

		CvSize m_imgSiz;

		FixedVector< ImageLineItrProvider > m_lineItrProv_Arr;

	};


	typedef Hcpl::ObjRef< ImageItrMgr > ImageItrMgrRef;

}