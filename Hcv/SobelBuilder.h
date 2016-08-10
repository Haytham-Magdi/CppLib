#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>



namespace Hcv
{
	using namespace Hcpl::Math;

	class SobelBuilder : FRM_Object
	{
	protected:

	public:
		SobelBuilder(F32ImageRef a_src, int a_nAprSiz);

		F32ImageRef GenImageX(int a_nOrder);
		F32ImageRef GenImageY(int a_nOrder);


	protected:
		int GetMaxOrder()
		{
			return m_nMaxOrder;
		}

		int GetAprSiz()
		{
			return m_nAprSiz;
		}

	protected:
		F32ImageRef m_src;
		const int m_nMaxOrder;
		const int m_nAprSiz;

		std::vector< F32ImageRef > m_derivesX;
		std::vector< F32ImageRef > m_derivesY;
		std::vector< F32ImageRef > m_deriveMags;
	};


	typedef Hcpl::ObjRef< SobelBuilder > SobelBuilderRef;
}