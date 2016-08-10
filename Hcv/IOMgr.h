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


	class IOMgr
	{
	public:

		class IIOMgrCore
		{
		public:

			virtual void ReadString( char * a_sKey, char * a_buff ) = 0;
			virtual bool ValueExists( char * a_sKey ) = 0;
		};


		static IIOMgrCore * GetCore()
		{
			//return NULL;
			return m_pCore;
			
		}

		static void SetCore(IIOMgrCore * a_pCore)
		{			
			m_pCore = a_pCore;			
		}

		static void ReadString( char * a_sKey, char * a_buff )
		{
			GetCore()->ReadString( a_sKey, a_buff );
		}

		static bool ValueExists( char * a_sKey )
		{
			return GetCore()->ValueExists( a_sKey );
		}

					


		static float ReadFloat( char * a_sKey )
		{
			m_tmpBuff[0] = 0;
			IOMgr::ReadString(a_sKey, m_tmpBuff);
			
			return atof(m_tmpBuff);
		}

		static int ReadInt( char * a_sKey )
		{
			m_tmpBuff[0] = 0;
			IOMgr::ReadString(a_sKey, m_tmpBuff);
			
			return atoi(m_tmpBuff);
		}


	protected:

		static IIOMgrCore * m_pCore;

		static char m_tmpBuff[100];

	};


}