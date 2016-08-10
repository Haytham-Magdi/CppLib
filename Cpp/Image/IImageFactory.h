#pragma once

#pragma inline_depth( 255 )


//#include <Lib\Cpp\Common\common.h>
#include <Lib\Cpp\Common\Object.h>
#include <Lib\Cpp\Image\IRgbImage.h>
#include <Lib\Cpp\Image\IImage.h>

#define FRM_IImageFactory_IFC(T) public virtual Hcpl::IImageFactory<T>



namespace Hcpl
{
	
	
	template<class T>
		class IImageFactory : FRM_IObject_IFC
	{
		public:

			virtual IImage<T> * CreateImage(Int32 a_M, Int32 a_N) = 0;
			virtual IImage<T> * CloneImage(ObjRef< IImage<T> > a_src) = 0; 
			virtual Int32 GetType() = 0;
	};
}
