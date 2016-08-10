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
	//using namespace Hcpl::Math;


	class SerialColorMgr
	{
	public:

		class ISerialColorMgrCore : public IIterator< U8ColorVal > 
		{
		};

		typedef Hcpl::ObjRef< ISerialColorMgrCore > ISerialColorMgrCoreRef;


		class MainSerialColorMgr : public ISerialColorMgrCore
		{
		public:
			MainSerialColorMgr();

			virtual bool MoveNext();
			virtual U8ColorVal GetCurrent();

		protected:
			void Init();

		protected:
			//bool m_bIsReady;
			int m_currentIndex;
			std::vector< U8ColorVal > m_colorVect;


		};


		static ISerialColorMgrCore * GetCore()
		{
			if( NULL == m_pCore )
				SetCore( new MainSerialColorMgr() );

			return m_pCore;			
		}

		static void SetCore(ISerialColorMgrCore * a_pCore)
		{			
			m_pCore = a_pCore;			
		}

		static bool MoveNext()
		{
			return GetCore()->MoveNext();
		}
			
		static U8ColorVal GetCurrent()
		{
			return GetCore()->GetCurrent();
		}


	protected:

		static ISerialColorMgrCoreRef m_pCore;
	};


}