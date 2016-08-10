#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\LinearColorFilter1D.h>

namespace Hcv
{
	using namespace Hcpl;
	using namespace Hcpl::Math;


	void LinearColorFilter1D::InputVal(F32ColorVal & a_val)
	{
		m_flt0->InputVal( a_val.val0 );
		m_flt1->InputVal( a_val.val1 );
		m_flt2->InputVal( a_val.val2 );
	}

	F32ColorVal LinearColorFilter1D::CalcOutput()
	{
		F32ColorVal outVal;

		outVal.val0 = m_flt0->CalcOutput();
		outVal.val1 = m_flt1->CalcOutput();
		outVal.val2 = m_flt2->CalcOutput();

		return outVal;
	}

	F32ColorVal LinearColorFilter1D::GetLastOutput()
	{
		F32ColorVal outVal;

		outVal.val0 = m_flt0->GetLastOutput();
		outVal.val1 = m_flt1->GetLastOutput();
		outVal.val2 = m_flt2->GetLastOutput();

		return outVal;
	}

	bool LinearColorFilter1D::HasOutput()
	{
		return m_flt0->HasOutput();
	}

	int LinearColorFilter1D::GetShift()
	{
		return m_flt0->GetShift();
	}

	int LinearColorFilter1D::GetLength()
	{
		return m_flt0->GetLength();
	}

}