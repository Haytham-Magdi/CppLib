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


	class FiltSysDebug
	{
	public:

		class IFiltSysDebugCore : FRM_Object
		{
		};

		typedef Hcpl::ObjRef< IFiltSysDebugCore > IFiltSysDebugCoreRef;


		class MainFiltSysDebug : public IFiltSysDebugCore
		{
		public:
			MainFiltSysDebug();

			bool MainFiltSysDebug::MoveNext();
			void Init( int a_nSize );
			void RegFilterActive( int a_nFiltIdx, int a_valIdx );
			void SaveToFile( char * a_sFileName );


		public:

			int m_inpCnt;

		protected:
			void ResetLine();


		protected:
			//bool m_bIsReady;
			int m_currentIndex;
			//int m_lastSize;
			std::vector< int > m_startIdxVectArr[100];
			std::vector< bool > m_regDoneVect;


		};


		static MainFiltSysDebug * GetCore()
		{
			if( NULL == m_pCore )
				SetCore( new MainFiltSysDebug() );

			return m_pCore;			  
		}

		static void SetCore(MainFiltSysDebug * a_pCore)
		{			
			m_pCore = a_pCore;			
		}

		static bool MoveNext()
		{
			return GetCore()->MoveNext();
		}

		static void Init( int a_nSize )
		{
			GetCore()->Init( a_nSize );
		}

		static void RegFilterActive( int a_nFiltIdx, int a_valIdx )
		{
			GetCore()->RegFilterActive( a_nFiltIdx, a_valIdx );
		}

		static void SetInpCnt( int a_inpCnt )
		{
			GetCore()->m_inpCnt = a_inpCnt;
		}

		static int GetInpCnt( )
		{
			return GetCore()->m_inpCnt;
		}
		
		static void SaveToFile( char * a_sFileName )
		{
			GetCore()->SaveToFile( a_sFileName );
		}

	protected:

		//static MainFiltSysDebugRef m_pCore;
		static MainFiltSysDebug * m_pCore;
	};


}