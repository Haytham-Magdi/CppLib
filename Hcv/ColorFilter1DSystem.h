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

#include <Lib\Hcv\ColorFilter1D.h>


namespace Hcv
{
	using namespace Hcpl::Math;
	using namespace std;

	class IColorFilter1DSystemConsumer : FRM_Object
	{
	public:

		virtual void ReceiveFilterOutput( int a_nFilterID, F32ColorVal a_outVal, int a_nBackShift ) = 0;
		virtual void ReceiveFilterOutput( int a_nFilterID, float a_outVal, int a_nBackShift ) = 0;
		virtual void NotifyAllOutputDone( ) = 0;
	};

	typedef Hcpl::ObjRef< IColorFilter1DSystemConsumer > IColorFilter1DSystemConsumerRef;

/*
	class IColorFilter1DSystem : FRM_Object
	{
	public:

		virtual int AddFilter( IColorFilter1DRef a_filt ) = 0;
		virtual void InputVal(F32ColorVal a_val);

	};


	typedef Hcpl::ObjRef< IColorFilter1DSystem > IColorFilter1DSystemRef;
*/


	class ColorFilter1DSystemBuilder;

	//class ColorFilter1DSystem : public IColorFilter1DSystem
	class ColorFilter1DSystem : FRM_Object
	{
	protected:


        typedef enum { VT_Single = 0, VT_Color } ValType;


		class IInputSrc : FRM_Object
		{
		public:

			virtual F32ColorVal GetColorVal() = 0;
			virtual float GetSingleVal() = 0;
			virtual bool HasInput() = 0;
			virtual int GetBackAccmShift() = 0;
			virtual bool ProvidesColorVal() = 0;
			virtual bool ProvidesSingleVal() = 0;

		};

		typedef Hcpl::ObjRef< IInputSrc > IInputSrcRef;


		class OrgInputSrc : public IInputSrc
		{
		public:

			virtual F32ColorVal GetColorVal() { return m_colorVal; }
			virtual float GetSingleVal() { return m_colorVal.CalcMag(); }
			virtual bool HasInput() { return true; }
			virtual int GetBackAccmShift() { return 0; }
			virtual bool ProvidesColorVal() { return true; }
			virtual bool ProvidesSingleVal() { return true; }

			void SetInputVal( F32ColorVal a_colorVal )
			{
				m_colorVal = a_colorVal;
			}

		protected:
			F32ColorVal m_colorVal;
		};


		class IFilterMgr : public IInputSrc
		{
		public:
			virtual bool Process() = 0;
			virtual void ResetInput() = 0;
			
		};

		typedef Hcpl::ObjRef< IFilterMgr > IFilterMgrRef;


		class FilterMgrBase : public IFilterMgr
		{
		public:
			FilterMgrBase( int a_id ) { m_id = a_id; }

			virtual int GetBackAccmShift() { return m_backAcmShift; }

			virtual F32ColorVal GetColorVal() { ThrowHcvException(); F32ColorVal dmy; return dmy; }
			virtual float GetSingleVal() { ThrowHcvException(); return -999; }

			virtual bool ProvidesColorVal() { return false; }
			virtual bool ProvidesSingleVal() { return false; }

		protected:
			int GetID() { return m_id; }

		protected:
			int m_backAcmShift;

		private:
			int m_id;
		};


		class ColorFilterMgr : public FilterMgrBase 
		{
		public:

			ColorFilterMgr( IColorFilter1DRef a_colorFilter, int a_id, IInputSrc * a_inputSrc, 
				IColorFilter1DSystemConsumer * a_consumer );

			virtual F32ColorVal GetColorVal() { return m_colorFilter->GetLastOutput(); }

			virtual bool ProvidesColorVal() { return true; }
			virtual bool HasInput() { return m_colorFilter->HasOutput(); }

			// Tmp Only
			//virtual bool ProvidesSingleVal() { return true; }
			//virtual float GetSingleVal() { return m_colorFilter->GetLastOutput().CalcMag(); }

			virtual bool Process();
			virtual void ResetInput() 
			{ 
				m_colorFilter->ResetInput();
			}

		protected:

			IColorFilter1DRef m_colorFilter;
			IInputSrc * m_inputSrc;
			IColorFilter1DSystemConsumer * m_consumer;
		};



		class ColorToSingleConverter : public FilterMgrBase 
		{
		public:

			ColorToSingleConverter( int a_id, IInputSrc * a_inputSrc, 
				IColorFilter1DSystemConsumer * a_consumer );

			virtual float GetSingleVal() { return m_lastVal; }

			virtual bool ProvidesSingleVal() { return true; }
			virtual bool HasInput() { return m_inputSrc->HasInput(); }

			virtual bool Process();
			virtual void ResetInput() {  }

		protected:

			float m_lastVal;
			IInputSrc * m_inputSrc;
			IColorFilter1DSystemConsumer * m_consumer;
		};


		class ColorToSingleFstValConverter : public ColorToSingleConverter 
		{
		public:

			ColorToSingleFstValConverter( int a_id, IInputSrc * a_inputSrc, 
				IColorFilter1DSystemConsumer * a_consumer ) : 
			ColorToSingleConverter( a_id, a_inputSrc, a_consumer ) { }

			virtual bool Process();
		};



		class SingleFilterMgr : public FilterMgrBase 
		{
		public:

			SingleFilterMgr( IFilter1DRef a_singleFilter, int a_id, IInputSrc * a_inputSrc, 
				IColorFilter1DSystemConsumer * a_consumer );

			virtual float GetSingleVal() { return m_singleFilter->GetLastOutput(); }

			virtual bool ProvidesSingleVal() { return true; }
			virtual bool HasInput() { return m_singleFilter->HasOutput(); }

			virtual bool Process();
			virtual void ResetInput() { m_singleFilter->ResetInput(); }

		protected:

			IFilter1DRef m_singleFilter;
			IInputSrc * m_inputSrc;
			IColorFilter1DSystemConsumer * m_consumer;
		};



	public:

		void InputVal(F32ColorVal a_val);

		void ResetInput();

		int GetInputCnt()
		{
			return m_nInputCnt;
		}

		int GetNofFilters()
		{
			return m_filterMgrVect.GetSize();
		}

		int GetMaxBackShift()
		{
			return m_maxBackShift;
		}

		friend class ColorFilter1DSystemBuilder;

	protected:

		ColorFilter1DSystem( IColorFilter1DSystemConsumer * a_consumer )
		{
			m_consumer = a_consumer;
			m_nInputCnt = 0;

			m_filterMgrVect.SetCapacity( 100 );

			//F32ColorVal m_colorInput;

			m_maxBackShift;
		}

	protected:

		IColorFilter1DSystemConsumer * m_consumer;
		int m_nInputCnt;

		F32ColorVal m_inputVal;

		int m_maxBackShift;		

		OrgInputSrc m_orgInputSrc;

		FixedVector< IFilterMgrRef > m_filterMgrVect;
	};

	typedef Hcpl::ObjRef< ColorFilter1DSystem > ColorFilter1DSystemRef;



	class ColorFilter1DSystemBuilder : FRM_Object
	{

	public:

		ColorFilter1DSystemBuilder( IColorFilter1DSystemConsumer * a_consumer )
		{
			m_filtSys = new ColorFilter1DSystem( a_consumer );

			Init();
		}

		bool IsDone()
		{
			return m_bDone;
		}

		int AddFilter( IColorFilter1DRef a_filt, int a_nSrcFilterID = -1 );

		int AddFilter( IFilter1DRef a_filt, int a_nSrcFilterID = -1 );

		int AddColorToSingleConverter( int a_nSrcFilterID = -1 );

		ColorFilter1DSystemRef GetResult();


		int GetFilterCnt()
		{
			return m_filtSys->GetNofFilters();
		}


	protected:

		void Init();
		void Build( );

	protected:

		ColorFilter1DSystemRef m_filtSys;

		bool m_bDone;
	};

	typedef Hcpl::ObjRef< ColorFilter1DSystemBuilder > ColorFilter1DSystemBuilderRef;




}
