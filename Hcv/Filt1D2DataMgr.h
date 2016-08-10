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


namespace Hcv
{
	using namespace Hcpl::Math;
	using namespace std;


	class IFilt1D2DataReaderSize : FRM_Object
	{
	public:

		virtual int GetSize() = 0;
		virtual int GetCapacity() = 0;
		virtual int GetMargBef() = 0;
		virtual int GetMargAft() = 0;

	protected:

	};


	template<class T>
	class IFilt1D2DataReader : public virtual IFilt1D2DataReaderSize
	{
	public:

		virtual T * GetData() = 0;

	protected:

	};


	typedef Hcpl::ObjRef< IFilt1D2DataReader< float > > ISingFilt1D2DataReaderRef;
	
	typedef Hcpl::ObjRef< IFilt1D2DataReader< int > > IIntFilt1D2DataReaderRef;
	
	typedef Hcpl::ObjRef< IFilt1D2DataReader< F32ColorVal > > IColorFilt1D2DataReaderRef;


	template<class T>
	class IFilt1D2DataMgr : public virtual IFilt1D2DataReader< T >
	{
	public:

		virtual void SetSize( int a_nSize ) = 0;

		virtual void SetMargins( const int a_nMargBef, const int a_nMargAft ) = 0;

		virtual void AssignData( T * a_dataBuf, int a_nSize ) = 0;


		//Hcpl::ObjRef< IFilt1D2DataReader< T > > ToRdr()
		//{
		//	return ( Hcpl::ObjRef< IFilt1D2DataReader< T > > ) 
		//}


	protected:

	};

	//typedef Hcpl::ObjRef< IFilt1D2DataMgr< T > > IFilt1D2DataMgrRef< T >;

	//template<class T>
	//typedef Hcpl::ObjRef< IFilt1D2DataMgr< T > > IFilt1D2DataMgrRef< T >;

	typedef Hcpl::ObjRef< IFilt1D2DataMgr< float > > ISingFilt1D2DataMgrRef;

	typedef Hcpl::ObjRef< IFilt1D2DataMgr< int > > IIntFilt1D2DataMgrRef;

	typedef Hcpl::ObjRef< IFilt1D2DataMgr< F32ColorVal > > IColorFilt1D2DataMgrRef;


	template<class T>
	class SimpleFilt1D2DataMgr : public IFilt1D2DataMgr< T >
	{
	public:

		SimpleFilt1D2DataMgr( int a_nCapacity )
		{
			m_vect.SetCapacity( a_nCapacity );
			m_vect.SetSize( a_nCapacity );

			m_nMargBef = 0;
			m_nMargAft = 0;
		}

		virtual T * GetData()
		{
			return & m_vect[ 0 ];
		}

		virtual int GetSize()
		{
			return m_vect.GetSize();
		}

		virtual int GetCapacity()
		{
			return m_vect.GetCapacity();
		}

		virtual void SetSize( int a_nSize )
		{
			return m_vect.SetSize( a_nSize );
		}

		virtual int GetMargBef()
		{
			return m_nMargBef;
		}

		virtual int GetMargAft()
		{
			return m_nMargAft;
		}

		virtual void SetMargins( const int a_nMargBef, const int a_nMargAft )
		{
			m_nMargBef = a_nMargBef;
			m_nMargAft = a_nMargAft;
		}

		virtual void AssignData( T * a_dataBuf, int a_nSize )
		{
			m_vect.SetSize( a_nSize );

			for( int i=0; i < a_nSize; i++ )
				m_vect[ i ] = a_dataBuf[ i ];
		}

	protected:

		FixedVector< T > m_vect;

		int m_nMargBef;
		int m_nMargAft;
	};


}
