

int Hcpl::IObject::s_id = 0;

int Hcpl::ObjectFriend::s_id = 0;

//int Hcpl::ObjectFriend::s_nTestBuf_Cnt = 0;
//int Hcpl::ObjectFriend::s_nTestBuf_Siz = 1000000;
int Hcpl::ObjectFriend::s_nTestBuf_Siz;


//int * Hcpl::ObjectFriend::m_testBuf = new int[ 1000000 ];
//int * Hcpl::ObjectFriend::m_testBuf = 
//	new int[ Hcpl::ObjectFriend::s_nTestBuf_Siz ];

int * Hcpl::ObjectFriend::m_testBuf;
int * Hcpl::ObjectFriend::m_testBuf_2;


#include <Lib\Cpp\Common\TypeIDGen.cpp>
#include <Lib\Cpp\Common\FilePathMgr.cpp>
#include <Lib\Cpp\Common\NumberIterator.cpp>



