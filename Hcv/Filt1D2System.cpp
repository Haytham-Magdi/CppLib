#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\Filt1D2System.h>



namespace Hcv
{
	using namespace Hcpl;
	using namespace Hcpl::Math;


	void Filt1D2SystemBase::Init( int a_nDataCapacity )
	{
		m_nDataCapacity = a_nDataCapacity;

		m_filtVect.SetCapacity( 300 );
	}

	void Filt1D2SystemBase::Proceed()
	{
		for( int i=0; i < m_filtVect.GetSize(); i++ )
			m_filtVect[ i ]->Proceed();
	}

}