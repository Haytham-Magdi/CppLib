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


	class ScanDirMgr : FRM_Object
	{
	public:

		class PixLineInfo
		{
		public:

			PixLineInfo()
			{
				nLineNum = -1;
				nLOL = -1;
			}

			int nLineNum;
			int nLOL;
		};



	public:

		ScanDirMgr(  ) {}
		//ScanDirMgr( float a_angDig, CvSize a_siz )
		ScanDirMgr( float a_angDig, CvSize a_siz )
		{
			Init( a_angDig, a_siz );
		}

		void Init( float a_angDig, CvSize a_siz );

		FixedVector< F32Point > & GetShiftArr()
		{
			return m_shiftArr;
		}

		FixedVector< int > & GetShiftIndexArr()
		{
			return m_shiftIndexArr;
		}


		

		FixedVector< F32Point > & GetBgnPntArr()
		{
			return m_bgnPntArr;
		}

		FixedVector< int > & GetBgnIndexArr()
		{
			return m_bgnIndexArr;
		}

		FixedVector< int > & GetLimArr()
		{
			return m_limArr;
		}

		FixedVector< PixLineInfo > & GetPixLineInfoArr();


/*
		S32ImageRef GetNextMapImg()
		{
			return m_nextMapImg;
		}

		S32ImageRef GetPrevMapImg()
		{
			return m_prevMapImg;
		}
*/

		CvSize GetCvSize()
		{
			return m_srcSiz;
		}

		F32Point GetMainDirPnt()
		{
			return m_mainDirPnt;
		}

		float GetAngleByDigree()
		{
			return m_angDig;
		}

		float GetAngleByRad()
		{
			return m_angRad;
		}

		F32Point GetDifPnt()
		{
			return m_difPnt;
		}


	protected:

		void PrepareMaps();

		void PrepareBgnPntArr();

		void PrepareBgnPntArr_1();
		void PrepareBgnPntArr_2();
		void PrepareBgnPntArr_3();
		void PrepareBgnPntArr_4();


	protected:

		FixedVector< F32Point > m_shiftArr;
		FixedVector< int > m_shiftIndexArr;

		CvSize m_srcSiz;

		F32Point m_mainDirPnt;

		float m_angDig;
		float m_angRad;

		F32Point m_difPnt;

		FixedVector< F32Point > m_bgnPntArr;
		FixedVector< int > m_bgnIndexArr;

		FixedVector< int > m_limArr;

		FixedVector< PixLineInfo > m_pixLineInfoArr;

		//S32ImageRef m_nextMapImg;
		//S32ImageRef m_prevMapImg;

	};


	typedef Hcpl::ObjRef< ScanDirMgr > ScanDirMgrRef;

}