#pragma once



//#ifdef Hcpl_DEBUG
#define Hcpl_ASSERT(C)  Hcpl::Debug::Assert((C))
//#else
//	#define Hcpl_ASSERT(C)  
//#endif





namespace Hcpl
{
	class Debug
	{
	public:
		static void Assert(int a_bCond)
		{
			if( ! a_bCond)
			{
				Break();

				//_CrtDbgBreak
				throw "Debug Assert Exp.";
			}
		}


		static void Break(void)
		{

		}

	};



}