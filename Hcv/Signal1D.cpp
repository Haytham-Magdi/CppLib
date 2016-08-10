#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\Signal1D.h>

namespace Hcv
{
	using namespace Hcpl;
	using namespace Hcpl::Math;


	void Signal1DBuilder::Init(int a_capacity, int a_nBgn)
	{
		m_sig = new Signal1D();

		m_sig->m_data.resize( a_capacity );
		m_sig->m_data.resize( 0 );

		m_sig->m_nBgn = a_nBgn;
	}

	void Signal1DBuilder::AddValue( float a_value )
	{
		if( true == m_bDone )
		{
			Hcpl::Debug::Assert( false );
			return;
		}

		m_sig->m_data.push_back( a_value );
	}

	void Signal1DBuilder::AddData( float * a_dataBuf, const int a_nSize )
	{
		for( int i=0; i < a_nSize; i++ )
			AddValue( a_dataBuf[ i ] );
	}

	void Signal1DBuilder::AddConstArr( float a_val, const int a_nSize )
	{
		for( int i=0; i < a_nSize; i++ )
			AddValue( a_val );
	}

	void Signal1DBuilder::Build( )
	{
		m_bDone = true;
	}

	Signal1DRef Signal1DBuilder::GetResult()
	{
		Build();

		if( false == m_bDone )
		{
			Hcpl::Debug::Assert( false );
			return NULL;
		}

		return m_sig;
	}

}