#pragma once

#pragma inline_depth( 255 )

#include <Lib\Cpp\Common\Common.h>
#include <Lib\Cpp\Common\Object.h>
#include <Lib\Cpp\Image\IImage.h>
#include <Lib\Cpp\Image\IImageFactory.h>

#include <Lib\Cpp\Matlab\Mtlb.h>



namespace Hcpl
{
	
	template<class T>
		class MtlbImageFactory : FRM_IImageFactory_IFC(T), FRM_Object
	{
		protected:
			MtlbImageFactory() {}
			static MtlbImageFactory<T> * m_inst;


		public:

			virtual IImage<T> * CreateImage(Int32 a_M, Int32 a_N)
			{
				return NULL;
			}

			virtual IImage<T> * CloneImage(ObjRef< IImage<T> > a_src)
			{
				return NULL;
			}
				
			virtual Int32 GetType()
			{
				return 0;
			}

			static MtlbImageFactory<T> * GetInst(void)
			{
				if(NULL == m_inst)
					m_inst = new MtlbImageFactory();
				return m_inst;
			}


	};

	template<class T>
		MtlbImageFactory<T> * MtlbImageFactory<T>::m_inst = NULL;

}