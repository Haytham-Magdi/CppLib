#pragma once

#pragma inline_depth( 255 )


#include <Lib\Cpp\Common\common.h>
#include <Lib\Cpp\Common\Object.h>
#include <Lib\Cpp\Image\IRgbImage.h>
#include <Lib\Cpp\Image\IImage.h>

#include <Lib\Cpp\Matlab\Mtlb.h>

#include <engine.h>

namespace Hcpl
{
	
	template<class T>	
		class MtlbRgbImage : FRM_IRgbImage_IFC(T), FRM_Object
	{
		public:
			
			MtlbRgbImage(mxArray * a_pMxArr, char * a_sVarName)
			{
				m_pMxArr = a_pMxArr;
				strcpy(m_sVarName, a_sVarName);
				
				int a;
				
				a = mxGetNumberOfDimensions(m_pMxArr);
				
				m_orgPtr = (T*) mxGetData(m_pMxArr);

				mxClassID clsID = mxGetClassID(m_pMxArr);
				
				int * dims;
				
				dims = (int *) mxGetDimensions(m_pMxArr);

				for(int i=0; i<dims[0]; i++)
					for(int j=0; j<dims[1]; j++)
						m_orgPtr[j + i * dims[0]] = 255;

					engPutVariable(Mtlb::GetEngine(),
						m_sVarName, m_pMxArr);

				
				a = 0;
			};
			
			virtual ~MtlbRgbImage()
			{
				mxDestroyArray(m_pMxArr);
				Mtlb::DeleteVarName(m_sVarName);
			}
			
			virtual IImage<T> * GetRImage()
			{
				return NULL;
			}
			
			virtual IImage<T> * GetGImage()
			{
				return NULL;
			}
			
			virtual IImage<T> * GetBImage()
			{
				return NULL;
			}
			
		protected:
			
			mxArray * m_pMxArr;
			char m_sVarName[40];
	};
}