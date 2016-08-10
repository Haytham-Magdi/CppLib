#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\ConvFilter1D.h>

namespace Hcv
{
	using namespace Hcpl;
	using namespace Hcpl::Math;


	float ConvFilter1D::DoCalcOutput()
	{
		return Convolve();
	}

	float ConvFilter1D::Convolve()
	{
		float outVal = 0;

		for( int i=0; i < m_valQue.GetSize(); i++)
		{
			outVal += m_valQue[i] * m_factorVect[i];
		}

		return outVal;
	}

	int ConvFilter1D::GetLength()
	{
		return m_factorVect.size();
	}

	IFilter1D * ConvFilter1D::Clone()
	{
		ConvFilter1D * pFlt1 = new ConvFilter1D();

		//pFlt1->m_valQue.empty();

		{
			vector< float > & srcVect = this->m_factorVect;
			vector< float > & dstVect = pFlt1->m_factorVect;

			dstVect.resize( srcVect.size() );

			for( int i=0; i < dstVect.size(); i++ )
				dstVect[ i ] = srcVect[ i ];
		}

		pFlt1->ResetInput();

		return pFlt1;
	}

	IFilter1DRef ConvFilter1D::Create( float * a_factors, int a_nAprSiz )
	{
		ConvFilter1DBuilderRef fb1 = new ConvFilter1DBuilder();

		for( int i=0; i < a_nAprSiz; i++ )
			fb1->AddFactor( a_factors[ i ] );

		IFilter1DRef flt1 = fb1->GetResult();

		return flt1;
	}



///////////////////////////////////////////////////////////////////////////////////////


	void ConvFilter1DBuilder::Init()
	{
		m_posSum = 0;
		m_negSum = 0;

		m_filter->m_factorVect.resize( 15 );
		m_filter->m_factorVect.resize( 0 );

		m_bDone = false;
	}

	void ConvFilter1DBuilder::AddFactor( float a_value )
	{
		if( true == m_bDone )
		{
			Hcpl::Debug::Assert( false );
			return;
		}

		m_filter->m_factorVect.push_back( a_value );

		if( a_value > 0 )
		{
			m_posSum += a_value;
		}
		else
		{
			m_negSum -= a_value;
		}

	}

	void ConvFilter1DBuilder::Build( )
	{
		float divVal = 0;

		if( m_posSum > m_negSum )
		{
			divVal = m_posSum;
		}
		else
		{
			divVal = m_negSum;
		}

		if( divVal > 0 )
		{
			vector< float > & factVect = m_filter->m_factorVect;

			for( int i=0; i < factVect.size(); i++ )
				factVect[ i ] /= divVal;
		}

		m_bDone = true;
	}

	//ConvFilter1DRef ConvFilter1DBuilder::GetResult()
	IFilter1DRef ConvFilter1DBuilder::GetResult()
	{
		if( false == m_bDone )
		{
			Build();

			//Hcpl::Debug::Assert( false );
			//return NULL;
		}

		return ( IFilter1DRef ) m_filter;
	}

}