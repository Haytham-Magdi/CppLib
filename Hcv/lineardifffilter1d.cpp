#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\LinearDiffFilter1D.h>

namespace Hcv
{
	using namespace Hcpl;
	using namespace Hcpl::Math;



	float LinearDiffFilter1D::DoCalcOutput()
	{
		float outVal = m_valQue.GetBack() - m_valQue.GetFront();

		outVal *= m_outFact;

		return outVal;
	}

	IFilter1D * LinearDiffFilter1D::Clone()
	{
		LinearDiffFilter1D * pFlt1 = new LinearDiffFilter1D( this->m_outFact );

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


	IFilter1DRef LinearDiffFilter1D::Create( int a_nAprSiz, float a_outFact )
	{
		ConvFilter1DBuilderRef fb1;

		{
			LinearDiffFilter1D * flt1 = new LinearDiffFilter1D( a_outFact );

			fb1 = new ConvFilter1DBuilder( flt1 );
		}

		for( int i=0; i < a_nAprSiz; i++ )
			fb1->AddFactor( 1 );

		return fb1->GetResult();
	}


}