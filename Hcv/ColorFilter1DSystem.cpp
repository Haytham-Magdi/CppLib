#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\ColorFilter1DSystem.h>

namespace Hcv
{
	using namespace Hcpl;
	using namespace Hcpl::Math;

	
	void ColorFilter1DSystem::InputVal(F32ColorVal a_val)
	{		
		m_nInputCnt++;		

		//FiltSysDebug::SetInpCnt( m_nInputCnt );

		m_orgInputSrc.SetInputVal( a_val );

		bool bAllOutputDone = true;

		for( int i=0; i < m_filterMgrVect.GetSize(); i++ )
		{
			if( false == m_filterMgrVect[i]->Process() )
				bAllOutputDone = false;
		}

		if( bAllOutputDone )
			m_consumer->NotifyAllOutputDone();  
	}

	void ColorFilter1DSystem::ResetInput()
	{
		m_nInputCnt = 0;

		for(int i=0; i < m_filterMgrVect.GetSize(); i++ )
		{
			m_filterMgrVect[ i ]->ResetInput();
		}
	}

///////////////////////////////////////////////////////////////////////////////////////////////


	void ColorFilter1DSystemBuilder::Init()
	{
		m_bDone = false;
	}


	int ColorFilter1DSystemBuilder::AddFilter( IColorFilter1DRef a_filt, int a_nSrcFilterID )
	{
		if( true == m_bDone )
		{
			ThrowHcvException();
			return -999;
		}

		int nSrcFiltID = a_nSrcFilterID;

		if( -1 == nSrcFiltID )
		{
			if( m_filtSys->m_filterMgrVect.GetSize() > 0 )
			{
				nSrcFiltID = m_filtSys->m_filterMgrVect.GetSize() - 1;
			}
			else
			{
				nSrcFiltID = -10;
			}
		}

		ColorFilter1DSystem::IInputSrc * inputSrc = NULL;

		if( -10 != nSrcFiltID )
		{
			inputSrc = m_filtSys->m_filterMgrVect[ nSrcFiltID ];
		}
		else
		{
			inputSrc = &m_filtSys->m_orgInputSrc;
		}

		int id = m_filtSys->m_filterMgrVect.GetSize();

		ColorFilter1DSystem::IFilterMgrRef filterMgr = new ColorFilter1DSystem::ColorFilterMgr( 
				a_filt, id, inputSrc, m_filtSys->m_consumer );

		m_filtSys->m_filterMgrVect.PushBack( filterMgr );

		if( filterMgr->GetBackAccmShift() > m_filtSys->m_maxBackShift )
			m_filtSys->m_maxBackShift = filterMgr->GetBackAccmShift();

		return id;
	}


	int ColorFilter1DSystemBuilder::AddFilter( IFilter1DRef a_filt, int a_nSrcFilterID )
	{
		if( true == m_bDone )
		{
			ThrowHcvException();
			return -999;
		}

		int nSrcFiltID = a_nSrcFilterID;

		if( -1 == nSrcFiltID )
		{
			if( m_filtSys->m_filterMgrVect.GetSize() > 0 )
			{
				nSrcFiltID = m_filtSys->m_filterMgrVect.GetSize() - 1;
			}
			else
			{
				nSrcFiltID = -10;
			}
		}

		ColorFilter1DSystem::IInputSrc * inputSrc = NULL;

		if( -10 != nSrcFiltID )
		{
			inputSrc = m_filtSys->m_filterMgrVect[ nSrcFiltID ];
		}
		else
		{
			inputSrc = &m_filtSys->m_orgInputSrc;
		}

		int id = m_filtSys->m_filterMgrVect.GetSize();

		ColorFilter1DSystem::IFilterMgrRef filterMgr = new ColorFilter1DSystem::SingleFilterMgr( 
				a_filt, id, inputSrc, m_filtSys->m_consumer );

		m_filtSys->m_filterMgrVect.PushBack( filterMgr );

		if( filterMgr->GetBackAccmShift() > m_filtSys->m_maxBackShift )
			m_filtSys->m_maxBackShift = filterMgr->GetBackAccmShift();

		return id;
	}


	int ColorFilter1DSystemBuilder::AddColorToSingleConverter( int a_nSrcFilterID )
	{
		if( true == m_bDone )
		{
			ThrowHcvException();
			return -999;
		}

		int nSrcFiltID = a_nSrcFilterID;

		if( -1 == nSrcFiltID )
		{
			if( m_filtSys->m_filterMgrVect.GetSize() > 0 )
			{
				nSrcFiltID = m_filtSys->m_filterMgrVect.GetSize() - 1;
			}
			else
			{
				nSrcFiltID = -10;
			}
		}

		ColorFilter1DSystem::IInputSrc * inputSrc = NULL;

		if( -10 != nSrcFiltID )
		{
			inputSrc = m_filtSys->m_filterMgrVect[ nSrcFiltID ];
		}
		else
		{
			inputSrc = &m_filtSys->m_orgInputSrc;
		}


		int id = m_filtSys->m_filterMgrVect.GetSize();

		ColorFilter1DSystem::IFilterMgrRef filterMgr = new ColorFilter1DSystem::ColorToSingleConverter( 
				id, inputSrc, m_filtSys->m_consumer );

		m_filtSys->m_filterMgrVect.PushBack( filterMgr );

		if( filterMgr->GetBackAccmShift() > m_filtSys->m_maxBackShift )
			m_filtSys->m_maxBackShift = filterMgr->GetBackAccmShift();

		return id;
	}






	void ColorFilter1DSystemBuilder::Build( )
	{
		m_bDone = true;
	}

	ColorFilter1DSystemRef ColorFilter1DSystemBuilder::GetResult()
	{
		if( false == m_bDone )
		{
			Build();
		}

		//return ( ColorFilter1DSystemRef ) m_filtSys;

		//FiltSysDebug::Init( m_filtSys->m_filterMgrVect.GetSize() );

		return m_filtSys;
	}


////////////////////////////////////////////////////////////


	ColorFilter1DSystem::ColorFilterMgr::ColorFilterMgr( IColorFilter1DRef a_colorFilter, int a_id, 
		IInputSrc * a_inputSrc, IColorFilter1DSystemConsumer * a_consumer ) : FilterMgrBase( a_id )
	{
		if( false == a_inputSrc->ProvidesColorVal() )
			ThrowHcvException();

		m_inputSrc = a_inputSrc;

		m_colorFilter = a_colorFilter;
		m_consumer = a_consumer;
		
		m_backAcmShift = a_colorFilter->GetBackShift() +
			a_inputSrc->GetBackAccmShift();						
	}


	ColorFilter1DSystem::ColorToSingleConverter::ColorToSingleConverter( int a_id, 
		IInputSrc * a_inputSrc, IColorFilter1DSystemConsumer * a_consumer ) : FilterMgrBase( a_id )
	{
		if( false == a_inputSrc->ProvidesColorVal() )
			ThrowHcvException();

		m_inputSrc = a_inputSrc;

		m_consumer = a_consumer;
		
		m_backAcmShift = a_inputSrc->GetBackAccmShift();						
	}


	ColorFilter1DSystem::SingleFilterMgr::SingleFilterMgr( IFilter1DRef a_singleFilter, int a_id,		
		IInputSrc * a_inputSrc, IColorFilter1DSystemConsumer * a_consumer ) : FilterMgrBase( a_id )
	{
		if( false == a_inputSrc->ProvidesSingleVal() )
			ThrowHcvException();

		m_inputSrc = a_inputSrc;

		m_singleFilter = a_singleFilter;
		m_consumer = a_consumer;
		
		m_backAcmShift = a_singleFilter->GetBackShift() +
			a_inputSrc->GetBackAccmShift();
	}

	bool ColorFilter1DSystem::ColorFilterMgr::Process()
	{

		if( false == m_inputSrc->HasInput() )
			return false;

		//FiltSysDebug::RegFilterActive( this->GetID(), 
			//FiltSysDebug::GetInpCnt( ) );

		m_colorFilter->InputVal( m_inputSrc->GetColorVal() );		

		// by hthm tmp
		//return false;

		if( m_colorFilter->HasOutput() )
		{
			m_consumer->ReceiveFilterOutput( GetID(), 
				m_colorFilter->CalcOutput(), m_backAcmShift );

			return true;
		}
		else
		{
			return false;
		}
	}


	bool ColorFilter1DSystem::ColorToSingleFstValConverter::Process()
	{
		if( false == m_inputSrc->HasInput() )
			return false;

		m_lastVal = m_inputSrc->GetColorVal().val0;

		m_consumer->ReceiveFilterOutput( GetID(), 
			m_lastVal, m_backAcmShift );

		return true;
	}


	bool ColorFilter1DSystem::ColorToSingleConverter::Process()
	{
		// by hthm tmp
		//return false;

		if( false == m_inputSrc->HasInput() )
			return false;

		//FiltSysDebug::RegFilterActive( this->GetID(), 
			//FiltSysDebug::GetInpCnt( ) );

		m_lastVal = m_inputSrc->GetColorVal().CalcMag();

		m_consumer->ReceiveFilterOutput( GetID(), 
			m_lastVal, m_backAcmShift );

		return true;
	}


	bool ColorFilter1DSystem::SingleFilterMgr::Process()
	{
		int a = 0;

		//if( GetID() == 5 )
		//	a = 0;

		if( false == m_inputSrc->HasInput() )
			return false;

		//FiltSysDebug::RegFilterActive( this->GetID(), 
			//FiltSysDebug::GetInpCnt( ) );

		m_singleFilter->InputVal( m_inputSrc->GetSingleVal() );		

		//if( GetID() == 5 )
		//	a = 0;

		// by hthm tmp
		//return false;

		if( m_singleFilter->HasOutput() )
		{
			m_consumer->ReceiveFilterOutput( GetID(), 
				m_singleFilter->CalcOutput(), m_backAcmShift );

			return true;
		}
		else
		{
			return false;
		}
	}



}