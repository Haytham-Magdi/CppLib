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
	//using namespace Hcpl::Math;


	class GridColorPalette // : FRM_Object
	{
	public:

		class Elm // : FRM_Object
		{
		public:

			Elm();

		protected:

			//void Prepare();

		public:

			int nIndex;

			int X;
			int Y;
			int Z;

		protected:

		};


		class Share
		{
		public:

			int ElmIdx;
			int nSize;
		};


		class ColorRep // : FRM_Object
		{
		public:

			Share Shares[ 8 ];
		};

		class Core  : FRM_Object
		{
		public:

			Core();
			void PrepareColorRep( F32ColorVal & a_rColor, ColorRep * a_pOutRep );

			IndexCalc3D & GetIndexCalc()
			{
				return m_idxCalc;
			}


			FixedVector< Elm > & GetElmArr()
			{
				return m_elmArr;
			}


		protected:

			void Prepare();

		protected:

			IndexCalc3D m_idxCalc;
			int m_nSep;
			int m_nDimLen;

			FixedVector< Elm > m_elmArr;
		};


		typedef Hcpl::ObjRef< Core > CoreRef;


	public:

		static CoreRef GetCore()
		{
			if( NULL == m_core )
				m_core = new Core();

			return m_core;
		}

	protected:

		static CoreRef m_core;

	};



}