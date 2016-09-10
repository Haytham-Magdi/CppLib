#pragma once


namespace Hcv
{
	using namespace Hcpl;

	typedef struct _U8ColorVal
	{
		Uint8 val0;
		Uint8 val1;
		Uint8 val2;
	}U8ColorVal;

	typedef struct _S16ColorVal
	{
		Int16 val0;
		Int16 val1;
		Int16 val2;
	}S16ColorVal;
	
	typedef struct _S32ColorVal
	{
		Int32 val0;
		Int32 val1;
		Int32 val2;

		static struct _S32ColorVal Add( 
			struct _S32ColorVal & a_rArg1, 
			struct _S32ColorVal & a_rArg2)
		{
			struct _S32ColorVal ret;

			ret.val0 = a_rArg1.val0 + a_rArg2.val0;
			ret.val1 = a_rArg1.val1 + a_rArg2.val1;
			ret.val2 = a_rArg1.val2 + a_rArg2.val2;

			return ret;
		}

		struct _S32ColorVal MultBy( int a_num )
		{
			struct _S32ColorVal ret;

			ret.val0 = this->val0 * a_num;
			ret.val1 = this->val1 * a_num;
			ret.val2 = this->val2 * a_num;

			return ret;
		}

		struct _S32ColorVal DividBy( int a_num )
		{
			struct _S32ColorVal ret;

			ret.val0 = this->val0 / a_num;
			ret.val1 = this->val1 / a_num;
			ret.val2 = this->val2 / a_num;

			return ret;
		}

	}S32ColorVal;

	template<class T>
	void FillBorder(CHANNEL_REF(T) a_chnl, T a_val, int a_nThickness)		
	{
		FillBorder(a_chnl, a_val,
			a_nThickness, a_nThickness, a_nThickness, a_nThickness);
	}

	template<class T>
	void FillBorder(CHANNEL_REF(T) a_chnl, T a_val,
		int a_nThickLeft, int a_nThickRight, int a_nThickTop, int a_nThickBotom)
	{
		// HTODO : Validate size, and get ROI

		int x, y;

		for(y=0; y<a_nThickTop; y++)
			for(x=0; x<a_chnl->GetWidth(); x++)
				a_chnl->SetAt(x, y, a_val);

		for(y=a_chnl->GetHeight()-1-a_nThickBotom; y<a_chnl->GetHeight(); y++)
			for(x=0; x<a_chnl->GetWidth(); x++)
				a_chnl->SetAt(x, y, a_val);

		for(x=0; x<a_nThickLeft; x++)
			for(y=0; y<a_chnl->GetHeight(); y++)
				a_chnl->SetAt(x, y, a_val);

		for(x=a_chnl->GetWidth()-1-a_nThickRight; x<a_chnl->GetWidth(); x++)
			for(y=0; y<a_chnl->GetHeight(); y++)
				a_chnl->SetAt(x, y, a_val);		
	}

	void CvtRgbToHsl0(U8ImageRef a_src, U8ImageRef a_dst);
	void CvtRgbToHsl(U8ImageRef a_src, U8ImageRef a_dst);
	void Convolve2D( const CvMat* a, CvMat* b, const CvMat* kernel, CvPoint anchor );
	void GetMaxImg(S16ImageRef a_src1, S16ImageRef a_src2, S16ImageRef a_dst);
	void CalcColorSblXY(S16ImageRef a_srcX, S16ImageRef a_srcY, S16ImageRef a_dst);
	void GetColorMidean(S16ImageRef a_src, S16ImageRef a_dst);
	S16ImageRef GenColorMideanImg( S16ImageRef a_src);
	S16ImageRef GenWeightedColorMideanImg( 
		S16ImageRef a_src, S16ImageRef a_srcGrad, int a_nAprSiz );
	U8ImageRef GenColorDsp(U8ImageRef a_src);
	U8ImageRef GenScaledImage(U8ImageRef a_src);
	S16ImageRef GenDownSampledImage(S16ImageRef a_src, int a_nStep);
	S16ImageRef GenDownSampledImage(S16ImageRef a_src, int a_nStepX, int a_nStepY);
	S16ImageRef GenUpSampledImage(S16ImageRef a_src, int a_nStep);
	S16ImageRef GenUpSampledImage(S16ImageRef a_src, int a_nStepX, int a_nStepY);
	S16ImageRef GenColorSobelGradImage(S16ImageRef a_src, int a_nAprSiz);
	S16ImageRef GenS16FromU8Image(U8ImageRef a_src);
	U8ImageRef GenU8FromS16Image(S16ImageRef a_src);
	S16ImageRef GenScaledS16FromS32Image(S32ImageRef a_src);
	void ShowImage(IplImage * a_iplImagePtr, const char * a_sWndName);
	S16ImageRef GenMinimaImage(S16ImageRef a_src);
	S16ImageRef GenMaximaImage(S16ImageRef a_src);
	S32ImageRef GenNeighbourhoodImage(S16ImageRef a_src);
	U8ImageRef GenNegativeImage(U8ImageRef a_src);
	S16ImageRef GenPyrDownImg(S16ImageRef a_src);

	S16ImageRef GenHDensityImg(S16ImageRef a_hdifImg);

	std::vector<CvPoint> GenDataPoints(S16ImageRef a_src, const int nDataVal = 0);

	S16ImageRef GenDataImage(std::vector<CvPoint> a_data, CvSize a_imgSiz,
		int nDataVal = 0, const int nBkgVal = 255);

	S16ImageRef GenThresholdImg(S16ImageRef a_src, int a_nVal);

	void DrawPoints(CvPoint * a_data, int a_nofPoints, 
		S16ImageRef a_img, CvScalar a_color);

	void DrawPoints(std::vector<CvPoint> a_data, S16ImageRef a_img, CvScalar a_color);

	class ClusteringMgr;
	void DrawPoints(S16ImageRef a_img, CvPoint * a_points, 
		//int a_nofPoints, int * a_ids, U8ColorVal * a_colors);
		int a_nofPoints, ClusteringMgr & a_rCm, U8ColorVal * a_colors);

	

	void DrawPoints(S16ImageRef a_img, std::vector<CvPoint> * a_pPointArr,
		std::vector<int> * a_pPointIDs, std::vector<U8ColorVal> * a_pColorArr);

	U8ColorVal u8ColorVal(Uint8 a_val0, Uint8 a_val1, Uint8 a_val2);

	S16ImageRef GenFastAvgHImg(S16ImageRef a_src, int a_nAprSiz);
	S16ImageRef GenFastAvgVImg(S16ImageRef a_src, int a_nAprSiz);

	S16ImageRef GenFastAvgImg(S16ImageRef a_src, int a_nAprSiz);
	S16ImageRef GenDifMagImg(S16ImageRef a_src);
	S16ImageRef GenFlattenedImg(S16ImageRef a_src, S16ImageRef a_srcGrad, int a_nAprSiz);
	S16ImageRef GenTriChGrayImg( S16ImageRef a_src );

	void ShowImage(U8ImageRef dsp, const char * a_sWndName);
	void ShowImage(S16ImageRef a_src, const char * a_sWndName);
	void SaveImage(S16ImageRef a_src, char * a_sFilePath);

	S16ImageRef GenMorphGradImg(S16ImageRef a_src, int a_nAprSiz);
}