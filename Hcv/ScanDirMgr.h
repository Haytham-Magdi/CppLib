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
	private:
		ScanDirMgr( ScanDirMgr & a_src ) : m_id( 0 ) { }

	public:

		class PixLineInfo
		{
		public:

			PixLineInfo()
			{
				nLineNum = -1;
				nIOL = -1;
			}

			int nLineNum;
			int nIOL;
		};


		typedef enum
		{
			RC = 0, 
			RB, 
			CB, 
			LB
			
			//,
			//LC,		
			//LT, CT, RT

		}DirType;




	public:

		ScanDirMgr(  ) : m_id( s_id++ ) {}
		//ScanDirMgr( float a_angDig, CvSize a_siz )
		ScanDirMgr( float a_angDig, CvSize a_siz ) : m_id( s_id++ ) 
		{
			Init( a_angDig, a_siz );
		}

		void Init( float a_angDig, CvSize a_siz );

		int CalcAprSizAct( int a_nAprSizOrg );

		int GetID()
		{
			return m_id;
		}

		FixedVector< F32Point > & GetShiftArr()
		{
			return m_shiftArr;
		}

		FixedVector< int > & GetShiftIndexArr()
		{
			return m_shiftIndexArr;
		}


		
/*
		FixedVector< F32Point > & GetBgnPntArr()
		{
			return m_bgnPntArr;
		}
*/

		FixedVector< int > & GetBgnIndexArr()
		{
			return m_bgnIndexArr;
		}

		FixedVector< int > & GetLimArr()
		{
			return m_limArr;
		}

		FixedVector< int > & GetShiftStartArr()
		{
			return m_shiftStartArr;
		}

		int * Get_ioaHeadAt( int a_nIndex )
		{
			return m_ioaArrColl.GetHeadAt( a_nIndex );
		}

		int Get_ioaSizeAt( int a_nIndex )
		{
			return m_ioaArrColl.GetSizeAt( a_nIndex );
		}

		int Get_ioaDataSizeTot()
		{
			return m_ioaArrColl.GetDataSizeTot();
		}

		int Get_ioaNofHeads()
		{
			return m_ioaArrColl.GetNofHeads();
		}


		FixedVector< PixLineInfo > & GetPixLineInfoArr();


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


		void PrepareShiftStuff();

		void PrepareShiftStuff_RC();
		void PrepareShiftStuff_RB();
		void PrepareShiftStuff_CB();
		void PrepareShiftStuff_LB();

		void Prepare_Grand_IOA_Arr();

	protected:

		FixedVector< F32Point > m_shiftArr;
		FixedVector< int > m_shiftIndexArr;

		CvSize m_srcSiz;

		F32Point m_mainDirPnt;

		float m_angDig;
		float m_angRad;

		F32Point m_difPnt;

		//FixedVector< F32Point > m_bgnPntArr;
		FixedVector< int > m_bgnIndexArr;

		FixedVector< int > m_shiftStartArr;

		FixedVector< int > m_limArr;

		FixedVector< PixLineInfo > m_pixLineInfoArr;

		DirType m_dirType;

		//FixedVector< int > m_grand_IOA_Arr;

		//FixedVector< int * > m_ioaHeadArr;

		//FixedVector< int > m_ioaSizeArr;

		CommonArrCollBuff< int > m_ioaArrColl;

		const int m_id;

	public:

		static int s_id;


	};


	typedef Hcpl::ObjRef< ScanDirMgr > ScanDirMgrRef;

}