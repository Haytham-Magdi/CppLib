#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

#include <Lib\Hcv\CircleContourOfPix5.h>

namespace Hcv
{
	//using namespace Hcpl::Math;


	class CircDiff2 : FRM_Object
	{	
	public:

		class FwdBkdPoints
		{
		public:
			F32Point FwdPnt;
			F32Point BkdPnt;
		};


	public:
		CircDiff2( F32ImageRef a_srcImg, int a_radius );

		F32ImageRef GetDifMagImg()
		{
			return m_difMagImg;
		}

		F32ImageRef GetDifAngleImg()
		{
			return m_difAngleImg;
		}

		Accessor2D< FwdBkdPoints > & GetFwdBkdMatrix()
		{
			return m_fwdBwdPntArr;
		}

		F32ImageRef GenColorShowImg();

		
		
		static F32ImageRef GenResult( F32ImageRef a_src, int a_nAprSiz );

	protected:

		void Proceed();

	protected:
		CircleContourOfPix5Ref m_circCont;

		F32ImageRef m_srcImg;

		F32ImageRef m_difMagImg;
		F32ImageRef m_difAngleImg;

		Accessor2D< FwdBkdPoints > m_fwdBwdPntArr;
			//acSlide( &pntArr[0], srcSiz.width, srcSiz.height );
		

		int m_radius;
	};



	typedef Hcpl::ObjRef< CircDiff2 > CircDiff2Ref;

}