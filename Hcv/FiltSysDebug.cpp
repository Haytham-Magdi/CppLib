
#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\FiltSysDebug.h>

//#include <fstream.h>
//#include <iostream.h>
//#include <io.h>


namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;


	//FiltSysDebug::IFiltSysDebugCoreRef FiltSysDebug::m_pCore;
	FiltSysDebug::MainFiltSysDebug * FiltSysDebug::m_pCore;

	



	FiltSysDebug::MainFiltSysDebug::MainFiltSysDebug()
	{
		m_currentIndex = 0;

		//Init( a_nSize );
	}

	void FiltSysDebug::MainFiltSysDebug::Init( int a_nSize )
	{
		m_regDoneVect.resize( a_nSize );
		ResetLine();

		//m_lastSize = m_currentIndex;
		m_currentIndex = 0;


		for( int i=0; i < 100; i++ )
		{
			std::vector< int > & rStartIdxVect = m_startIdxVectArr[i];

			rStartIdxVect.resize( a_nSize );

			for( int j=0; j < rStartIdxVect.size(); j++ )
			{
				rStartIdxVect[j] = -9999;
			}
		}
		
	}

	void FiltSysDebug::MainFiltSysDebug::ResetLine()
	{
		m_inpCnt = 0;

		for( int i=0; i < m_regDoneVect.size(); i++ )
		{
			m_regDoneVect[i] = false;
		}
	}

	void FiltSysDebug::MainFiltSysDebug::RegFilterActive( int a_nFiltIdx, int a_valIdx )
	{
		std::vector< int > & rStartIdxVect = m_startIdxVectArr[ m_currentIndex ];

		if( false == m_regDoneVect[ a_nFiltIdx ] )
		{
			rStartIdxVect[ a_nFiltIdx ] = a_valIdx;

			m_regDoneVect[ a_nFiltIdx ] = true;
		}
		else
			return;

		
		std::vector< int > * pOldStartIdxVect = NULL;
		if( m_currentIndex > 0 )
		{
			pOldStartIdxVect = &m_startIdxVectArr[ m_currentIndex - 1 ];

			const int oldVal = (*pOldStartIdxVect)[ a_nFiltIdx ];

			if( oldVal != a_valIdx )
				a_valIdx = a_valIdx;
			else
				a_valIdx = a_valIdx;
		}

	}

	bool FiltSysDebug::MainFiltSysDebug::MoveNext()
	{
		m_currentIndex++;

		this->ResetLine();

		if( m_currentIndex >= 100 )
			return false;

		return true;
	}


	void FiltSysDebug::MainFiltSysDebug::SaveToFile( char * a_sFileName )
	{
		char text1[7000] = "";
		int nTxtLen = 0;

		for( int i=0; i < m_currentIndex; i++ )
		//for( int i=0; i < m_lastSize; i++ )
		{
			std::vector< int > & rStartIdxVect = m_startIdxVectArr[ i ];

			if( 0 == i )
			{
				nTxtLen += sprintf( &text1[ nTxtLen ], "\n\r" );

				for( int j=0; j < rStartIdxVect.size(); j++ )
				{
					nTxtLen += sprintf( &text1[ nTxtLen ], "%d\t", j );
				}
			}
			
			nTxtLen += sprintf( &text1[ nTxtLen ], "\n\r" );


			for( int j=0; j < rStartIdxVect.size(); j++ )
			{
				nTxtLen += sprintf( &text1[ nTxtLen ], "%d\t", rStartIdxVect[j] );
			}


		}

		
/*
		fstream file_op("c:\\CoderSource_file.txt",ios::out);

		file_op<<"Test Write to file";
		file_op.close();
		*/
	}



}