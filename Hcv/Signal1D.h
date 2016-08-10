#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>


namespace Hcv
{
	using namespace Hcpl::Math;
	using namespace std;

	class Signal1D : FRM_Object
	{
	public:

		friend class Signal1DBuilder;

		vector<float> m_data;
		int m_nBgn;		
	};

	typedef Hcpl::ObjRef< Signal1D > Signal1DRef;



	class Signal1DBuilder : FRM_Object
	{
	public:

		Signal1DBuilder(int a_capacity, int a_nBgn = 0)
		{
			Init(a_capacity, a_nBgn);

			m_bDone = false;
		}

		Signal1DBuilder()
		{
			Init(400, 0);

			m_bDone = false;
		}

		static Signal1DRef CreateConstSignal( float a_val, const int a_nSize, int a_nBgn = 0 )
		{
			Signal1DBuilder sb( a_nSize, a_nBgn );
			sb.AddConstArr( a_val, a_nSize );

			return sb.GetResult();
		}

		static Signal1DRef CreateStripSignal( float a_mag, int a_nStripFrq, 
			const int a_nSize, int a_nBgn = 0 )
		{
			Signal1DRef ret = CreateConstSignal( 0, a_nSize, a_nBgn );

			vector< float > & rData = ret->m_data;

			for( int i=0; i < rData.size(); i++ )
			{
				if( 0 != i % a_nStripFrq )
					continue;

				rData[ i ] = a_mag;
			}			

			return ret;
		}

		bool IsDone()
		{
			return m_bDone;
		}

		void AddValue( float a_value );

		void AddData( float * a_dataBuf, const int a_nSize );

		void AddConstArr( float a_val, const int a_nSize );

		Signal1DRef GetResult();



	protected:

		void Init(int a_capacity, int a_nBgn);
		void Build( );

	protected:

		Signal1DRef m_sig;
		bool m_bDone;
	};

	typedef Hcpl::ObjRef< Signal1DBuilder > Signal1DBuilderRef;




}
