#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

#include <Lib\Hcv\Filter1D.h>


//#include <deque>      // for deque
//#include <iostream>   // for cout, endl
//#include <list>       // for list
#include <queue>      // for queue
//#include <string>     // for string


namespace Hcv
{
	using namespace Hcpl::Math;
	using namespace std;

	class ConvFilter1D : public LinearFilter1DBase
	{
	public:

		ConvFilter1D()
		{
		}

		virtual int GetLength();

		virtual IFilter1D * Clone();

		static IFilter1DRef Create( float * a_factors, int a_nAprSiz );


		friend class ConvFilter1DBuilder;


	protected:

		virtual float DoCalcOutput();
		inline float Convolve();

	protected:

		vector<float> m_factorVect;
	};

	typedef Hcpl::ObjRef< ConvFilter1D > ConvFilter1DRef;



	class ConvFilter1DBuilder : FRM_Object
	{
	public:

		ConvFilter1DBuilder()
		{
			m_filter = new ConvFilter1D();

			Init();
		}

		ConvFilter1DBuilder( ConvFilter1D * a_filter )
		{
			m_filter = a_filter;

			Init();
		}

		bool IsDone()
		{
			return m_bDone;
		}

		void AddFactor( float a_value );


		//ConvFilter1DRef GetResult();
		IFilter1DRef GetResult();



	protected:

		void Init();
		void Build( );

	protected:

		ConvFilter1DRef m_filter;
		float m_posSum;
		float m_negSum;
		bool m_bDone;
	};

	typedef Hcpl::ObjRef< ConvFilter1DBuilder > ConvFilter1DBuilderRef;




}
