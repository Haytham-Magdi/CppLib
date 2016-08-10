#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

//#define IMGDATAELM_CovMat_NOFCOLORS 7
//#define IMGDATAELM_CovMat_NOFCOLORS 3
//#define IMGDATAELM_CovMat_NOFCOLORS 22

#define IMGDATAELM_CovMat_NOFCOLORS 9

namespace Hcv
{
	//using namespace Hcpl::Math;


	class ImgDataElm_CovMat;

	class ImgDataElm_CovMat_Converter : FRM_Object
	{

	public:

		ImgDataElm_CovMat_Converter();

		void Convert( F32ColorVal & a_rSrc, ImgDataElm_CovMat * a_pDst );

		FixedVector< F32ColorVal > & GetMainColors()
		{
			return m_mainColorArr;
		}

		FixedVector< F32ColorVal > & GetDispColors()
		{
			return m_dispColorArr;
		}


	protected:

			void Init();

	protected:

			FixedVector< F32ColorVal > m_mainColorArr;
			FixedVector< F32ColorVal > m_dispColorArr;

			FixedVector< float > m_distArr;
	};

	typedef Hcpl::ObjRef< ImgDataElm_CovMat_Converter > ImgDataElm_CovMat_ConverterRef;


	class ImgDataElm_CovMat // : FRM_Object
	{
	public:

		F32ColorVal MeanColor;

		CovMat Cov_NonPure;


	public:

		void SetZeroVal()
		{
			MeanColor.AssignVal( 0, 0, 0 );

			Cov_NonPure.Reset();

			//for( int i=0; i < NofColors; i++ )
				//ColorBuff[ i ] = 0;
		}

		float CalcMagSqr()
		{
			float res = this->CalcMag();

			res = Sqr( res );

			return res;

			//static CovMat cov1;

			//Cov_NonPure.CalcPureMat( MeanColor, & cov1 );

			//float res = cov1.CalcMagSqr();

			//if( res > 0 )
			//	res = res;

			//return res;

			//float sum = 0;

			//for( int i=0; i < NofColors; i++ )
			//	sum += Sqr( ColorBuff[ i ] );

			//sum /= NofColors;

			//return sum;			
		}

		inline float CalcMag();

		void Copy(ImgDataElm_CovMat & a_rElm)
		{
			memcpy( this, & a_rElm, sizeof( ImgDataElm_CovMat ) );
		}

		void IncBy(ImgDataElm_CovMat & a_rElm)
		{
			Cov_NonPure.IncBy( a_rElm.Cov_NonPure );
			MeanColor.IncBy( a_rElm.MeanColor );

			//for( int i=0; i < NofColors; i++ )
			//	ColorBuff[ i ] += a_rElm.ColorBuff[ i ];
		}

		void DecBy(ImgDataElm_CovMat & a_rElm)
		{
			Cov_NonPure.DecBy( a_rElm.Cov_NonPure );
			MeanColor.DecBy( a_rElm.MeanColor );

			//for( int i=0; i < NofColors; i++ )
			//	ColorBuff[ i ] -= a_rElm.ColorBuff[ i ];
		}

		void CalcUnitElm(ImgDataElm_CovMat * a_pOut)
		{
			*a_pOut = *this;
			a_pOut->DividSelfBy( a_pOut->CalcMag() );
		}

		void DividSelfBy(float a_num)
		{
			Cov_NonPure.DividSelfBy( a_num );
			MeanColor.DividSelfBy( a_num );

			//for( int i=0; i < NofColors; i++ )
			//	ColorBuff[ i ] /= a_num;
		}

		static ImgDataElm_CovMat Add( 
			ImgDataElm_CovMat & a_rArg1, 
			ImgDataElm_CovMat & a_rArg2)
		{
			ImgDataElm_CovMat ret;

			ret = a_rArg1;
			ret.IncBy( a_rArg2 );

			//for( int i=0; i < NofColors; i++ )
			//	ret.ColorBuff[ i ] /= 
			//		a_rArg1.ColorBuff[ i ] + a_rArg2.ColorBuff[ i ];

			return ret;
		}

		static ImgDataElm_CovMat Sub( 
			ImgDataElm_CovMat & a_rArg1, 
			ImgDataElm_CovMat & a_rArg2)
		{
			ImgDataElm_CovMat ret;

			ret = a_rArg1;
			ret.DecBy( a_rArg2 );

			//for( int i=0; i < NofColors; i++ )
			//	ret.ColorBuff[ i ] /= 
			//		a_rArg1.ColorBuff[ i ] - a_rArg2.ColorBuff[ i ];

			return ret;
		}

		static float CalcDif(ImgDataElm_CovMat & a_rElm_1, 
			ImgDataElm_CovMat & a_rElm_2)
		{
			CovMat cov1;
			
			a_rElm_1.Cov_NonPure.CalcPureMat( 
				a_rElm_1.MeanColor, & cov1 );

			ImgDataElm_CovMat elmSum = ImgDataElm_CovMat::Add(
				a_rElm_1, a_rElm_2);

			elmSum.DividSelfBy( 2 );

			//ImgDataElm_CovMat elmDif = ImgDataElm_CovMat::Sub(
				//elmSum, a_rElm_1);

			CovMat covSum;
			
			elmSum.Cov_NonPure.CalcPureMat( 
				elmSum.MeanColor, & covSum );

			CovMat covDif = CovMat::Sub( covSum, cov1 );

			//float res = covDif.CalcMagSqr();
			float res = covDif.CalcMag();

			res *= 2;

			if( res > 0.15 )
				res = res;

			return res;

			//return ImgDataElm_CovMat::Sub(
			//	a_rElm_1, a_rElm_2).CalcMag();
		}

		void SetFrom_F32ColorVal( F32ColorVal & a_color )
		{
			//if( NULL == m_converter )
				//m_converter = new ImgDataElm_CovMat_Converter();

			//m_converter->Convert( a_color, this );

			Cov_NonPure.SetByColor( a_color );
			MeanColor = a_color;

			//SetZeroVal();

			//ColorBuff[ 0 ] = a_color.val0;
			//ColorBuff[ 1 ] = a_color.val1;
			//ColorBuff[ 2 ] = a_color.val2;
		}

		F32ColorVal Gen_F32ColorVal()
		{
			return MeanColor;

			//F32ColorVal ret;

			//ret.val0 = ColorBuff[ 0 ];
			//ret.val1 = ColorBuff[ 1 ];
			//ret.val2 = ColorBuff[ 2 ];

			//return ret;
		}

		static float Calc_UnitDif(ImgDataElm_CovMat & a_rElm_1, 
			ImgDataElm_CovMat & a_rElm_2)
		{
			static ImgDataElm_CovMat ue_1;
			static ImgDataElm_CovMat ue_2;

			a_rElm_1.CalcUnitElm( & ue_1 );
			a_rElm_2.CalcUnitElm( & ue_2 );

			return ImgDataElm_CovMat::CalcDif(
				ue_1, ue_2);
		}


		inline static float Get_FarUnitDif()
		{
			return 0.37850;
		}


		//float ColorBuff[ IMGDATAELM_CovMat_NOFCOLORS ];


		static int GetNofColors() { return NofColors; }


		static FixedVector< F32ColorVal > & GetDisplayColors()			
		{
			if( NULL == m_converter )
				m_converter = new ImgDataElm_CovMat_Converter();

			return m_converter->GetDispColors();
		}

		
protected:

		static int NofColors; 

		static ImgDataElm_CovMat_ConverterRef m_converter;

		//float B;
		//float G;
		//float R;

	};


	//typedef Hcpl::ObjRef< ImgDataElm_CovMat > ImgDataElm_CovMatRef;

}