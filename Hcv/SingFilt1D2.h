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

	class ISingFilt1D2 : public IFilt1D2
	{
	public:

		virtual bool IsLinear() = 0;

		virtual int GetAprSize() = 0;

		virtual ISingFilt1D2DataReaderRef GetOutputReader() = 0;


	protected:

		virtual void SetupOutputMgr() = 0;

	};

	typedef Hcpl::ObjRef< ISingFilt1D2 > ISingFilt1D2Ref;




	class SingFilt1D2Base : public ISingFilt1D2
	{
	protected:

		SingFilt1D2Base() 
		{
			m_inpRdrArr.SetCapacity( 30 );
		}

		void Init( int a_nAprSizOrg, ISingFilt1D2DataReaderRef a_inpRdr,
			ISingFilt1D2DataMgrRef a_outDataMgr = NULL, float a_dmyVal = 0 );

		void ValidateInpArr();

	public:

		//SingFilt1D2Base( int a_nAprSizOrg, ISingFilt1D2DataReaderRef a_inpRdr,
			//ISingFilt1D2DataMgrRef a_outDataMgr, float a_dmyVal );

		virtual int GetAprSizeOrg()
		{
			return m_nAprSizOrg;
		}

		virtual int GetAprSize()
		{
			return m_nAprSiz;
		}

		virtual ISingFilt1D2DataReaderRef GetOutputReader()
		{
			return m_outDataMgr;
		}

		virtual void SetAprSize( int a_nAprSiz );


		virtual int GetProcCnt()
		{
			return m_nProcCnt;
		}


	protected:

		//inline void SetupOutputMgr();
		virtual void SetupOutputMgr();


	protected:

		ISingFilt1D2DataReaderRef m_inpRdr;
		FixedVector< IFilt1D2DataReaderSize * > m_inpRdrArr;
		ISingFilt1D2DataMgrRef m_outDataMgr;

		int m_nAprSizOrg;
		int m_nAprSiz;

		int m_nProcCnt;

		float m_dmyVal;
	};



	class LinearSingFilt1D2Base : public SingFilt1D2Base
	{
	protected:

		LinearSingFilt1D2Base() {}

	public:

		virtual bool IsLinear()
		{
			return true;
		}
	};


	class NonLinearSingFilt1D2Base : public SingFilt1D2Base
	{
	protected:

		NonLinearSingFilt1D2Base() {}

	public:

		virtual bool IsLinear()
		{
			return false;
		}
	};


}
