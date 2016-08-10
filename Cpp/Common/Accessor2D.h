#pragma once

#pragma inline_depth( 255 )

#include <stdlib.h>


#include <Lib\Cpp\Common\Debug.h>
#include <Lib\Cpp\Common\indexcalc.h>




namespace Hcpl
{
	template<class T>
	class Accessor2D : FRM_Object
	{
	public :

		Accessor2D( int a_nSizX, int a_nSizY ) : m_nSizX(a_nSizX), 
			m_nSizY(a_nSizY), m_inexCalc(a_nSizX, a_nSizY)
		{
			Init( NULL, a_nSizX, a_nSizY );
		}

		Accessor2D(T * a_data, int a_nSizX, int a_nSizY, int a_nSizTot = -1) : m_nSizX(a_nSizX), 
			m_nSizY(a_nSizY), m_inexCalc(a_nSizX, a_nSizY)
		{
			Init( a_data, a_nSizX, a_nSizY, a_nSizTot );
		}

		int GetSizeX()
		{
			return m_nSizX;
		}

		int GetSizeY()
		{
			return m_nSizY;
		}

		int GetTotalSize()
		{
			return m_nSizTot;
		}

/*
		T & operator[][](int a_posX, int a_posY)
		{
			return m_data[a_pos];
		}
*/

		T & GetAt(int a_x, int a_y)
		{
			return m_data[ m_inexCalc.Calc(a_x, a_y) ];
		}

		T * GetPtrAt(int a_x, int a_y)
		{
			return &m_data[ m_inexCalc.Calc(a_x, a_y) ];
		}

		int GetIndex1D(int a_x, int a_y)
		{
			return m_inexCalc.Calc(a_x, a_y);
		}

	protected:

		void Init(T * a_data, int a_nSizX, int a_nSizY, int a_nSizTot = -1)
		{
			//m_data = a_data;


			const int nSizTot = a_nSizX * a_nSizY;

			if( -1 != a_nSizTot )
				Debug::Assert( nSizTot == a_nSizTot );

			if( NULL == a_data )
			{
				m_spareVect.SetSize( nSizTot );
				m_data = &m_spareVect[ 0 ];
			}
			else
			{
				m_data = a_data;
			}

			m_nSizTot = nSizTot;

			//m_nSizX = a_nSizX;
			//m_nSizY = a_nSizY;
			//m_nSizTot = m_nSizX * m_nSizY;
		}


	protected:

		T * m_data;
		const int m_nSizX;
		const int m_nSizY;
		int m_nSizTot;

		FixedVector< T > m_spareVect;

		IndexCalc2D m_inexCalc;
	};

}