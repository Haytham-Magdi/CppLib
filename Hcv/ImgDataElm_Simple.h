#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

namespace Hcv
{
	//using namespace Hcpl::Math;


	class ImgDataElm_Simple // : FRM_Object
	{
	public:

		F32ColorVal Color;

		void SetZeroVal()
		{
			Color.AssignVal(0, 0, 0);
		}

		float CalcMagSqr()
		{
			return Color.CalcMagSqr();
		}

		inline float CalcMag();

		void Copy(ImgDataElm_Simple & a_rElm)
		{
			Color = a_rElm.Color;
		}

		void IncBy(ImgDataElm_Simple & a_rElm)
		{
			Color.IncBy(a_rElm.Color);
		}

		void DecBy(ImgDataElm_Simple & a_rElm)
		{
			Color.DecBy(a_rElm.Color);
		}

		void CalcUnitElm(ImgDataElm_Simple * a_pOut)
		{
			*a_pOut = *this;
			a_pOut->DividSelfBy( a_pOut->CalcMag() );
		}

		void DividSelfBy(float a_num)
		{
			Color.DividSelfBy(a_num);
		}

		static float CalcDif(ImgDataElm_Simple & a_rElm_1, 
			ImgDataElm_Simple & a_rElm_2)
		{
			return F32ColorVal::Sub(
				a_rElm_1.Color, a_rElm_2.Color).CalcMag();


			//float dif_0 = a_rElm_1.Color.val0 - a_rElm_2.Color.val0;
			//float dif_1 = a_rElm_1.Color.val1 - a_rElm_2.Color.val1;
			//float dif_2 = a_rElm_1.Color.val2 - a_rElm_2.Color.val2;

			//MaxFinder< float > maxFnd;

			//maxFnd.AddValue( fabs( dif_0 ) );
			//maxFnd.AddValue( fabs( dif_1 ) );
			//maxFnd.AddValue( fabs( dif_2 ) );

			//return maxFnd.FindMax();
		}

		static float Calc_UnitDif(ImgDataElm_Simple & a_rElm_1, 
			ImgDataElm_Simple & a_rElm_2)
		{
			static ImgDataElm_Simple ue_1;
			static ImgDataElm_Simple ue_2;

			a_rElm_1.CalcUnitElm( & ue_1 );
			a_rElm_2.CalcUnitElm( & ue_2 );

			return ImgDataElm_Simple::CalcDif(
				ue_1, ue_2);
		}


		inline static float Get_FarUnitDif()
		{
			//F32ColorVal c1;
			////c1.AssignVal( 0, 128, 255 );
			//c1.AssignVal( 128, 128, 128 );
			////c1.AssignVal( 255, 255, 255 );

			////c1.DividSelfBy( 2 );

			//F32ColorVal uc1;
			//uc1 = c1;
			//uc1.DividSelfBy( c1.CalcMag() );



			//F32ColorVal c2;
			//c2.AssignVal( 0, 0, 255 );

			//F32ColorVal uc2;
			//uc2 = c2;
			//uc2.DividSelfBy( c2.CalcMag() );



			//F32ColorVal c3;
			////c3 = F32ColorVal::Add( c1, c2 );
			//c3 = F32ColorVal::Add( uc1, uc2 );
			//c3.DividSelfBy( 2 );

			//F32ColorVal uc3;
			//uc3 = c3;
			//uc3.DividSelfBy( c3.CalcMag() );


			//float u_Dif = F32ColorVal::Sub( uc1, uc2 ).CalcMag();

			//float u_Dif_3 = F32ColorVal::Sub( uc1, uc3 ).CalcMag();


			//return 4.7313;

			return 0.37850;
		}

		//float B;
		//float G;
		//float R;

	};


	typedef Hcpl::ObjRef< ImgDataElm_Simple > ImgDataElm_SimpleRef;

}