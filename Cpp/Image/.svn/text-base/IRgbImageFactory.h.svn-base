#pragma once

#pragma inline_depth( 255 )


#include <Lib\Cpp\Common\common.h>
#include <Lib\Cpp\Common\Object.h>
#include <Lib\Cpp\Image\IRgbImage.h>
#include <Lib\Cpp\Image\IImage.h>

#define FRM_IRgbImageFactory_IFC(T) public virtual Hcpl::IRgbImageFactory<T>



namespace Hcpl
{
	
	
	template<class T>
		class IRgbImageFactory : FRM_IObject_IFC
	{
		public:

			virtual IRgbImage<T> * RgbImageFromFile(char * a_sPath) = 0;
			virtual Int32 GetType() = 0;
	};

/*	typedef IRgbImageFactory<Uint8> IUint8RgbImageFactory;
	typedef IRgbImageFactory<Int16> IInt16RgbImageFactory;
	typedef IRgbImageFactory<Double> IDoubleRgbImageFactory;*/

}

