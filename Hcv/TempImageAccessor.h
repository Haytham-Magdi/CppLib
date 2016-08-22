#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <Lib\Hcv\funcs1.h>
#include <vector>
#include <Lib\Hcv\Image.h>



namespace Hcv
{
	using namespace Hcpl;

	template<class T, int const V_NofChannels>
	class TempImageAccessor;


#define TempImageAccessor_REF(T) ObjRef< TempImageAccessor< T >>

	template<class class T>
	class TempImageAccessor : FRM_Object
	{
	public:

		TempImageAccessor(MemAccessor_2D_REF(T) a_memAccessor)
		{
			int nSize_X = a_memAccessor->GetOffsetCalc()->GetOffsetCalc_X()->GetMaxNofSteps();
			int nSize_Y = a_memAccessor->GetOffsetCalc()->GetOffsetCalc_Y()->GetMaxNofSteps();
			int nSize_1D = nSize_X * nSize_Y;

			m_allocVect.SetSize(nSize_1D);

			OffsetCalc_2D_Ref offsetCalc = new OffsetCalc_2D(1, nSize_X, nSize_Y);

			m_memAccessor = new MemAccessor_2D(m_allocVect.GetHeadPtr(), offsetCalc);
			m_memAccessor->Lock();
		}

		MemAccessor_2D_REF(T) GetMemAccessor()
		{
			return m_memAccessor;
		}

	protected:

		TempImageAccessor()
		{

		}

	protected:

		MemAccessor_2D_REF(T) m_memAccessor;
		FixedVector<T> m_allocVect;
	};

}