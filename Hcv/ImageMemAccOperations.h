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




namespace Hcv
{
	namespace ImageMemAccOperations
	{
		using namespace Hcpl;
		using namespace Hcv::MemAccOperations;


		////-------------------------------------------------------------------

		//F32Image_Float1C_MemAccessor_2D_Ref FillImage_Stripes(
		//	F32Image3C_F32ColorVal_MemAccessor_2D_Ref a_imgAcc,
		//	F32ColorVal & a_val1, F32ColorVal & a_val2, int a_stripWidth)
		
		F32Image3C_F32ColorVal_MemAccessor_2D_Ref GenFillImage_Stripes(
			F32Image3C_F32ColorVal_MemAccessor_2D_Ref a_imgAcc,
			F32ColorVal & a_val1, F32ColorVal & a_val2, int a_stripWidth)
		{
			//F32Image_Float1C_MemAccessor_2D_Ref ret = NULL;

			//F32Image_Float1C_MemAccessor_2D_Ref ret =
			F32Image3C_F32ColorVal_MemAccessor_2D_Ref ret =
				//new F32Image_Float1C_MemAccessor_2D(a_imgAcc->GetSrcImg()->CloneNew_WithChannels(1));
				//new F32Image3C_F32ColorVal_MemAccessor_2D(a_imgAcc->GetSrcImg()->CloneNew());
				new F32Image3C_F32ColorVal_MemAccessor_2D(a_imgAcc->GetSrcImg()->Clone());
			//new F32Image_Float1C_MemAccessor_2D(a_imgAcc->GetSrcImg()->CloneNew());

			//new F32Image_Float1C_MemAccessor_2D(a_imgAcc->GetSrcImg()->Clone());

			auto aut1 = ret->GetMemAccessor();

			FillImage_Stripes<F32ColorVal>(aut1, a_val1, a_val2, a_stripWidth);


			//FillImage_Stripes(a_imgAcc, a_val1, a_val2, a_stripWidth);
			////FillImage_Stripes(ret->GetMemAccessor(), a_val1, a_val2, a_stripWidth);

			//CalcMagSqrImage(a_imgAcc->GetMemAccessor(), ret->GetMemAccessor());

			return ret;
		}




		//F32Image_Float1C_MemAccessor_2D_Ref FillImage_Stripes(
		//	F32Image3C_F32ColorVal_MemAccessor_2D_Ref a_imgAcc,
		//	F32ColorVal & a_val1, F32ColorVal & a_val2, int a_stripWidth)
		//{
		//	F32Image_Float1C_MemAccessor_2D_Ref ret = NULL;
		//		//F32Image_Float1C_MemAccessor_2D_Ref ret = F32Image_Float1C_MemAccessor_2D(
		//		//F32Image::Create(a_imgAcc->GetSrcImg()->GetSize(), 1));

		//	//F32Image3C_F32ColorVal_MemAccessor_2D_Ref ret = a_imgAcc->CloneNew();
		//	//ret->SetSrcImg(a_imgAcc->GetSrcImg()->CloneNew());
		//	//ret->Lock();

		//	//FillImage_Stripes(a_imgAcc, a_val1, a_val2, a_stripWidth);

		//	return ret;
		//}

		////F32Image3C_F32ColorVal_MemAccessor_2D_Ref imgAcc1 = new F32Image3C_F32ColorVal_MemAccessor_2D(src);

		////F32Image3C_F32ColorVal_MemAccessor_2D_Ref imgAcc0 = new F32Image3C_F32ColorVal_MemAccessor_2D(src->CloneNew());
		////CopyImage(imgAcc0->GetMemAccessor(), imgAcc1->GetMemAccessor());


		////-------------------------------------------------------------------



	};
}