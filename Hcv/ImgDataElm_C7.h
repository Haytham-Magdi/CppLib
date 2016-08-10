#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>


#include <Lib\Hcv\ImgDataElm_C07.h>



//#define IMGDATAELM_C7_NOFCOLORS 7

//#define IMGDATAELM_C7_NOFCOLORS 3
//#define IMGDATAELM_C7_NOFCOLORS 22

namespace Hcv
{
	//using namespace Hcpl::Math;

/*
	class ImgDataElm_C7;

	class ImgDataElm_C7_Converter : FRM_Object
	{
		public:

			ImgDataElm_C7_Converter();

			void Convert( F32ColorVal & a_rSrc, ImgDataElm_C7 * a_pDst );

	protected:

			void Init();

	protected:

			FixedVector< F32ColorVal > m_mainColorArr;

			FixedVector< float > m_distArr;
	};

	//typedef Hcpl::ObjRef< ImgDataElm_C7::ImgDataElm_C7_Converter > ImgDataElm_C7_ConverterRef;
	typedef Hcpl::ObjRef< ImgDataElm_C7_Converter > ImgDataElm_C7_ConverterRef;
*/

	class ImgDataElm_C7  : public ImgDataElm_C07
	{
	};

/*
	class ImgDataElm_C7 // : FRM_Object
	{
	public:



	public:

		//F32ColorVal Color;

		void SetZeroVal()
		{
			for( int i=0; i < NofColors; i++ )
				ColorBuff[ i ] = 0;
		}

		float CalcMagSqr()
		{
			float sum = 0;

			for( int i=0; i < NofColors; i++ )
				sum += Sqr( ColorBuff[ i ] );

			//sum /= NofColors;

			return sum;			
		}

		inline float CalcMag();

		void Copy(ImgDataElm_C7 & a_rElm)
		{
			memcpy( this, & a_rElm, sizeof( ImgDataElm_C7 ) );
		}

		void IncBy(ImgDataElm_C7 & a_rElm)
		{
			for( int i=0; i < NofColors; i++ )
				ColorBuff[ i ] += a_rElm.ColorBuff[ i ];
		}

		void DecBy(ImgDataElm_C7 & a_rElm)
		{
			for( int i=0; i < NofColors; i++ )
				ColorBuff[ i ] -= a_rElm.ColorBuff[ i ];
		}

		void CalcUnitElm(ImgDataElm_C7 * a_pOut)
		{
			*a_pOut = *this;
			a_pOut->DividSelfBy( a_pOut->CalcMag() );
		}

		void DividSelfBy(float a_num)
		{
			for( int i=0; i < NofColors; i++ )
				ColorBuff[ i ] /= a_num;
		}

		static ImgDataElm_C7 Add( 
			ImgDataElm_C7 & a_rArg1, 
			ImgDataElm_C7 & a_rArg2)
		{
			ImgDataElm_C7 ret;

			for( int i=0; i < NofColors; i++ )
				ret.ColorBuff[ i ] /= 
					a_rArg1.ColorBuff[ i ] + a_rArg2.ColorBuff[ i ];

			return ret;
		}

		static ImgDataElm_C7 Sub( 
			ImgDataElm_C7 & a_rArg1, 
			ImgDataElm_C7 & a_rArg2)
		{
			ImgDataElm_C7 ret;

			for( int i=0; i < NofColors; i++ )
				ret.ColorBuff[ i ] /= 
					a_rArg1.ColorBuff[ i ] - a_rArg2.ColorBuff[ i ];

			return ret;
		}

		static float CalcDif(ImgDataElm_C7 & a_rElm_1, 
			ImgDataElm_C7 & a_rElm_2)
		{
			return ImgDataElm_C7::Sub(
				a_rElm_1, a_rElm_2).CalcMag();
		}

		void SetFrom_F32ColorVal( F32ColorVal & a_color )
		{
			if( NULL == m_converter )
				m_converter = new ImgDataElm_C7_Converter();

			m_converter->Convert( a_color, this );

			//SetZeroVal();

			//ColorBuff[ 0 ] = a_color.val0;
			//ColorBuff[ 1 ] = a_color.val1;
			//ColorBuff[ 2 ] = a_color.val2;
		}

		F32ColorVal Gen_F32ColorVal()
		{
			F32ColorVal ret;

			ret.val0 = ColorBuff[ 0 ];
			ret.val1 = ColorBuff[ 1 ];
			ret.val2 = ColorBuff[ 2 ];

			return ret;
		}

		static float Calc_UnitDif(ImgDataElm_C7 & a_rElm_1, 
			ImgDataElm_C7 & a_rElm_2)
		{
			static ImgDataElm_C7 ue_1;
			static ImgDataElm_C7 ue_2;

			a_rElm_1.CalcUnitElm( & ue_1 );
			a_rElm_2.CalcUnitElm( & ue_2 );

			return ImgDataElm_C7::CalcDif(
				ue_1, ue_2);
		}


		inline static float Get_FarUnitDif()
		{
			return 0.37850;
		}


		float ColorBuff[ IMGDATAELM_C7_NOFCOLORS ];


		static int GetNofColors() { return NofColors; }

		
protected:

		static int NofColors; 

		static ImgDataElm_C7_ConverterRef m_converter;

		//float B;
		//float G;
		//float R;

	};

*/
	//typedef Hcpl::ObjRef< ImgDataElm_C7 > ImgDataElm_C7Ref;

}