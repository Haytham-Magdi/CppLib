#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\Filter1D.h>

namespace Hcv
{
	using namespace Hcpl;
	using namespace Hcpl::Math;


	void Filter1DBase::InputVal(float a_val)
	{
		// By hthm tmp
		//if( m_valQue.GetSize() >= GetLength() )
		//	return;

		m_valQue.PushBack( a_val );

		if( m_valQue.GetSize() > GetLength() )
		{
			m_valQue.PopFront();
		}
	}


	int Filter1DBase::GetShift()
	{
		return GetLength() / 2;
	}


	void Filter1DBase::ResetInput()
	{
		m_valQue.ResetSize();
	}

	bool Filter1DBase::HasOutput()
	{
		return ( m_valQue.GetSize() == GetLength() );
	}


}