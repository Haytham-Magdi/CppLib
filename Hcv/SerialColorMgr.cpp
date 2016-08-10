#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\SerialColorMgr.h>

namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;


	SerialColorMgr::ISerialColorMgrCoreRef SerialColorMgr::m_pCore;



	SerialColorMgr::MainSerialColorMgr::MainSerialColorMgr()
	{
		m_currentIndex = 0;

		Init();
	}

	void SerialColorMgr::MainSerialColorMgr::Init()
	{

		m_colorVect.push_back( U8ColorVal( 0, 0, 0 ) );

		m_colorVect.push_back( U8ColorVal( 255, 0, 0 ) );

		m_colorVect.push_back( U8ColorVal( 0, 0, 255 ) );

		m_colorVect.push_back( U8ColorVal( 255, 0, 255 ) );

		m_colorVect.push_back( U8ColorVal( 0, 255, 0 ) );

		m_colorVect.push_back( U8ColorVal( 255, 255, 0 ) );

		m_colorVect.push_back( U8ColorVal( 0, 255, 255 ) );

		m_colorVect.push_back( U8ColorVal( 255, 255, 255 ) );

	}


	bool SerialColorMgr::MainSerialColorMgr::MoveNext()
	{
		m_currentIndex++;

		m_currentIndex = m_currentIndex % m_colorVect.size();

		return true;
	}

	U8ColorVal SerialColorMgr::MainSerialColorMgr::GetCurrent()
	{
		//ThrowHcvException();
		//U8ColorVal ret;

		return m_colorVect[ m_currentIndex ];
	}



}