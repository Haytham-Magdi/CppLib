#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <Lib\Hcv\funcs1.h>
#include <vector>
#include <Lib\Hcv\Image.h>
//#include <Lib\Cpp\Common\commonLib.h>

#include <Lib\Hcv\MemAccOperations.h>
#include <Lib\Hcv\ImageAccessorOperations.h>


namespace Hcv
{
	namespace ImageAccessorOperations
	{
		using namespace Hcpl;
		using namespace Hcv::MemAccOperations;


		////-------------------------------------------------------------------

		//F32ImageAccessor1C_Ref FillImage_Stripes(
		//	F32ImageAccessor3C_Ref a_imgAcc,
		//	F32ColorVal & a_val1, F32ColorVal & a_val2, int a_stripWidth)
		
		F32ImageAccessor3C_Ref GenFillImage_Stripes(
			F32ImageAccessor3C_Ref a_imgAcc,
			F32ColorVal & a_val1, F32ColorVal & a_val2, int a_stripWidth)
		{
			//F32ImageAccessor1C_Ref ret = NULL;

			//F32ImageAccessor1C_Ref ret =
			F32ImageAccessor3C_Ref ret =
				//new F32ImageAccessor1C(a_imgAcc->GetSrcImg()->CloneNew_WithChannels(1));
				//new F32ImageAccessor3C(a_imgAcc->GetSrcImg()->CloneNew());
				new F32ImageAccessor3C(a_imgAcc->GetSrcImg()->Clone());
			//new F32ImageAccessor1C(a_imgAcc->GetSrcImg()->CloneNew());

			//new F32ImageAccessor1C(a_imgAcc->GetSrcImg()->Clone());

			auto aut1 = ret->GetMemAccessor();

			FillImage_Stripes<F32ColorVal>(aut1, a_val1, a_val2, a_stripWidth);


			//FillImage_Stripes(a_imgAcc, a_val1, a_val2, a_stripWidth);
			////FillImage_Stripes(ret->GetMemAccessor(), a_val1, a_val2, a_stripWidth);

			//CalcMagSqrImage(a_imgAcc->GetMemAccessor(), ret->GetMemAccessor());

			return ret;
		}




		//F32ImageAccessor1C_Ref FillImage_Stripes(
		//	F32ImageAccessor3C_Ref a_imgAcc,
		//	F32ColorVal & a_val1, F32ColorVal & a_val2, int a_stripWidth)
		//{
		//	F32ImageAccessor1C_Ref ret = NULL;
		//		//F32ImageAccessor1C_Ref ret = F32ImageAccessor1C(
		//		//F32Image::Create(a_imgAcc->GetSrcImg()->GetSize(), 1));

		//	//F32ImageAccessor3C_Ref ret = a_imgAcc->CloneNew();
		//	//ret->SetSrcImg(a_imgAcc->GetSrcImg()->CloneNew());
		//	//ret->Lock();

		//	//FillImage_Stripes(a_imgAcc, a_val1, a_val2, a_stripWidth);

		//	return ret;
		//}

		////F32ImageAccessor3C_Ref imgAcc1 = new F32ImageAccessor3C(src);

		////F32ImageAccessor3C_Ref imgAcc0 = new F32ImageAccessor3C(src->CloneNew());
		////CopyImage(imgAcc0->GetMemAccessor(), imgAcc1->GetMemAccessor());


		////-------------------------------------------------------------------



	};
}