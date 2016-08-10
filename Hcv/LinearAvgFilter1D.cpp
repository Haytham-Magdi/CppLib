#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\LinearAvgFilter1D.h>

namespace Hcv
{
	using namespace Hcpl;
	using namespace Hcpl::Math;


	void LinearAvgFilter1D::InputVal(float a_val)
	{
		// By hthm tmp
		//if( m_valQue.GetSize() >= GetLength() )
		//	return;

		m_valQue.PushBack( a_val );

		if( m_inpSum < 0 )
			m_inpSum = m_inpSum;
		
		m_inpSum += a_val;

		if( m_valQue.GetSize() > m_factorVect.size() )
		{
			float inpSumOld = m_inpSum;

			m_inpSum -= m_valQue.GetFront();
				
			if( m_inpSum < 0 && inpSumOld >= 0 )
				m_inpSum = m_inpSum;
		
			m_valQue.PopFront();
		}
	}


	float LinearAvgFilter1D::DoCalcOutput()
	{
		float outVal = m_inpSum / m_valQue.GetSize();

		if( outVal < 0 )
			outVal = outVal;

		return outVal;
	}

	IFilter1D * LinearAvgFilter1D::Clone()
	{
		LinearAvgFilter1D * pFlt1 = new LinearAvgFilter1D();

		pFlt1->m_valQue.ResetSize();

		{
			vector< float > & srcVect = this->m_factorVect;
			vector< float > & dstVect = pFlt1->m_factorVect;

			dstVect.resize( srcVect.size() );

			for( int i=0; i < dstVect.size(); i++ )
				dstVect[ i ] = srcVect[ i ];
		}

		return pFlt1;
	}


	IFilter1DRef LinearAvgFilter1D::Create( int a_nAprSiz )
	{
		ConvFilter1DBuilderRef fb1;

		{
			LinearAvgFilter1D * flt1 = new LinearAvgFilter1D();

			fb1 = new ConvFilter1DBuilder( flt1 );
		}

		for( int i=0; i < a_nAprSiz; i++ )
			fb1->AddFactor( 1 );

		return fb1->GetResult();
	}


}