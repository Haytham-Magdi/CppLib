#pragma once

#pragma inline_depth( 255 )


#include <Lib\Cpp\Common\common.h>

#define FRM_IImage_IFC(T) public virtual Hcpl::IImage<T>



namespace Hcpl
{

	
	template<class T>
		class IImage : FRM_IObject_IFC
	{
		public:
			T * GetDataOrgPtr() { return m_orgPtr; }
			Int32 GetM() { return m_M; }
			Int32 GetN() { return m_N; }
			Int32 GetXBgn() { return m_nXBgn; }
			Int32 GetYBgn() { return m_nYBgn; }
			Int32 GetLineWidth() { return m_nLineWidth; }
			Int32 GetXStep() { return m_nXStep; }
			Int32 GetYStep() { return m_nYStep; }
			Int32 GetType() { return m_nType; }
		
		protected:
			T * m_orgPtr;
			Int32 m_M;
			Int32 m_N;
			Int32 m_nBgnX;
			Int32 m_nBgnY;
			Int32 m_nLineWidth;
			Int32 m_nXStep;
			Int32 m_nYStep;
			Int32 m_nType;
	};
	

	typedef IImage<Uint8> IUint8Image;
	typedef IImage<Int16> IInt16Image;
	typedef IImage<Double> IDoubleImage;
}
