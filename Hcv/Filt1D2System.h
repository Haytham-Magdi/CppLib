#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <queue>      

#include <Lib\Hcv\Filt1D2.h>
#include <Lib\Hcv\Filt1D2DataMgr.h>


namespace Hcv
{
	using namespace Hcpl::Math;
	using namespace std;



	class Filt1D2SystemBase : FRM_Object
	{
	protected:

		Filt1D2SystemBase() {}

		void Init( int a_nDataCapacity );

	public:

		void Proceed();

		FixedVector< IFilt1D2Ref > & GetFiltArr()
		{
			return m_filtVect;
		}

	protected:


	protected:

		int m_nDataCapacity;

		FixedVector< IFilt1D2Ref > m_filtVect;

	};

	//typedef Hcpl::ObjRef< IFilt1D2System > IFilt1D2SystemRef;



	class Filt1D2SysEmpty : public Filt1D2SystemBase
	{
	public:

		Filt1D2SysEmpty( int a_nDataCapacity )
		{
			Init( a_nDataCapacity );
		}

	};

	typedef Hcpl::ObjRef< Filt1D2SysEmpty > Filt1D2SysEmptyRef;
}
