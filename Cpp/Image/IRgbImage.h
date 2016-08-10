#pragma once

#pragma inline_depth( 255 )


#include <Lib\Cpp\Common\common.h>
#include <Lib\Cpp\Image\IImage.h>

#define FRM_IRgbImage_IFC(T) public virtual Hcpl::IRgbImage<T>


namespace Hcpl
{
	
	
	template<class T>
		class IRgbImage : FRM_IObject_IFC
	{
		public:
			virtual IImage<T> * GetRImage() = 0;
			virtual IImage<T> * GetGImage() = 0;
			virtual IImage<T> * GetBImage() = 0;

			T * GetDataOrgPtr() { return m_orgPtr; }
			Int32 GetM() { return m_M; }
			Int32 GetN() { return m_N; }
			Int32 GetXBgn() { return m_nXBgn; }
			Int32 GetYBgn() { return m_nYBgn; }
			Int32 GetLineWidth() { return m_nLineWidth; }
			Int32 GetXStep() { return m_nXStep; }
			Int32 GetYStep() { return m_nYStep; }
		
		protected:
			T * m_orgPtr;
			Int32 m_M;
			Int32 m_N;
			Int32 m_nBgnX;
			Int32 m_nBgnY;
			Int32 m_nLineWidth;
			Int32 m_nXStep;
			Int32 m_nYStep;
	};
	
	
	
	typedef IRgbImage<Uint8> IUint8RgbImage;
	typedef IRgbImage<Int16> IInt16RgbImage;
	typedef IRgbImage<Double> IDoubleRgbImage;
	
}
