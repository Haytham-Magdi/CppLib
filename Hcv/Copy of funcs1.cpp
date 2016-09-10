#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\PixelTypes.h>
#include <Lib\Hcv\error.h>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <vector>

#include <Lib\Hcv\funcs1.h>


namespace Hcv
{
	using namespace Hcpl;

	void CvtRgbToHsl0(U8ImageRef a_src, U8ImageRef a_dst)
	{
		// HTODO : validate identical sizes

		U8ChannelRef bluCh = a_src->GetAt(0);
		U8ChannelRef grnCh = a_src->GetAt(1);
		U8ChannelRef redCh = a_src->GetAt(2);

		U8ChannelRef hueCh = a_dst->GetAt(0);
		U8ChannelRef satCh = a_dst->GetAt(1);
		U8ChannelRef lumCh = a_dst->GetAt(2);

		for(int y=0; y<a_src->GetHeight(); y++)
			for(int x=0; x<a_src->GetWidth(); x++)
			{
				Uint8 nRed = redCh->GetAt(x, y);
				Uint8 nGrn = grnCh->GetAt(x, y);
				Uint8 nBlu = bluCh->GetAt(x, y);

				Uint8 nMax = 0;
				if(nRed > nMax)
					nMax = nRed;
				if(nGrn > nMax)
					nMax = nGrn;
				if(nBlu > nMax)
					nMax = nBlu;

				Uint8 nMin = 255;
				if(nRed < nMin)
					nMin = nRed;
				if(nGrn < nMin)
					nMin = nGrn;
				if(nBlu < nMin)
					nMin = nBlu;


				int nHue;

				if(0 == (nMax - nMin))
					nHue = 0;
				else
				{
					if(nMax == nRed)
						nHue = 60 * (nGrn-nBlu) / (nMax-nMin);
					else if(nMax == nGrn)
						nHue = 2 * 60 + 60 * (nBlu-nRed) / (nMax-nMin);
					else	//	(nMax == nBlu)
						nHue = 4 * 60 + 60 * (nRed-nGrn) / (nMax-nMin);

					if(nHue < 0)
						nHue += 360;

					nHue = nHue * 256 / 360;
				}



				Uint8 nSat = (Uint8)( (nMax-nMin) * 255 / nMax); 
				//Uint8 nSat = (Uint8)((ULONG)(nMax-nMin)*255/nMax); 


				Uint8 nLum = (nMax + nMin) / 2;
				/*		Uint8 nLum;
				nTmp = (nMax + nMin) / 2;
				if(nTmp > 255)
				nLum = 255;
				else
				nLum = nTmp;*/


				hueCh->SetAt(x, y, nHue);
				satCh->SetAt(x, y, nSat);
				lumCh->SetAt(x, y, nLum);

			}
	}




	void CvtRgbToHsl(U8ImageRef a_src, U8ImageRef a_dst)
	{
		// HTODO : validate identical sizes

		U8ChannelRef bluCh = a_src->GetAt(0);
		U8ChannelRef grnCh = a_src->GetAt(1);
		U8ChannelRef redCh = a_src->GetAt(2);

		U8ChannelRef hueCh = a_dst->GetAt(0);
		U8ChannelRef satCh = a_dst->GetAt(1);
		U8ChannelRef lumCh = a_dst->GetAt(2);

		for(int y=0; y<a_src->GetHeight(); y++)
			for(int x=0; x<a_src->GetWidth(); x++)
			{
				Uint8 nRed = redCh->GetAt(x, y);
				Uint8 nGrn = grnCh->GetAt(x, y);
				Uint8 nBlu = bluCh->GetAt(x, y);

				Uint8 nHue;
				Uint8 nSat;
				Uint8 nVal;

				Hsv::GetHsvOfRgb(nRed, nGrn, nBlu, &nHue, &nSat, &nVal);

				hueCh->SetAt(x, y, nHue);
				satCh->SetAt(x, y, nSat);
				lumCh->SetAt(x, y, nVal );

			}
	}



	void Convolve2D( const CvMat* a, CvMat* b, const CvMat* kernel, CvPoint anchor )
	{
		int i, j, k;
		int cn, ncols, a_step;
		int ker_size = kernel->rows*kernel->cols;
		int* offset = (int*)malloc( ker_size*sizeof(offset[0]));
		float* k_data = (float*)malloc( ker_size*sizeof(k_data[0]));
		int all_same = 1;
		float first = kernel->data.fl[0];
		uchar *a_data, *b_data;

		cn = CV_MAT_CN(a->type);
		ncols = b->cols*cn;
		a_step = a->step / CV_ELEM_SIZE(a->type & ~CV_MAT_CN_MASK);

		assert( a->cols == b->cols + kernel->cols - 1 &&
			a->rows == b->rows + kernel->rows - 1 && CV_ARE_TYPES_EQ( a, b ) );
		assert( CV_MAT_TYPE(kernel->type) == CV_32FC1 );
		assert( 0 <= anchor.x && anchor.x < kernel->cols &&
			0 <= anchor.y && anchor.y < kernel->rows );

		for( i = 0, k = 0; i < kernel->rows; i++ )
			for( j = 0; j < kernel->cols; j++ )
			{
				float f = ((float*)(kernel->data.ptr + kernel->step*i))[j];
				if( f )
				{
					k_data[k] = f;
					offset[k++] = (i - anchor.y)*a_step + (j - anchor.x)*cn;
				}
				if( f != first )
					all_same = 0;
			}

			ker_size = k;
			a_data = a->data.ptr + a->step*anchor.y + CV_ELEM_SIZE(a->type)*anchor.x;
			b_data = b->data.ptr;

			for( i = 0; i < b->rows; i++, a_data += a->step, b_data += b->step )
			{
				switch( CV_MAT_DEPTH(a->type) )
				{
				case CV_8U:
					for( j = 0; j < ncols; j++ )
					{
						double s = 0;
						int val;
						for( k = 0; k < ker_size; k++ )
							s += ((uchar*)a_data)[j+offset[k]]*k_data[k];
						val = cvRound(s);
						((uchar*)b_data)[j] = CV_CAST_8U(val);
					}
					break;
				case CV_8S:
					for( j = 0; j < ncols; j++ )
					{
						double s = 0;
						int val;
						for( k = 0; k < ker_size; k++ )
							s += ((char*)a_data)[j+offset[k]]*k_data[k];
						val = cvRound(s);
						((char*)b_data)[j] = CV_CAST_8S(val);
					}
					break;
				case CV_16U:
					for( j = 0; j < ncols; j++ )
					{
						double s = 0;
						int val;
						for( k = 0; k < ker_size; k++ )
							s += ((ushort*)a_data)[j+offset[k]]*k_data[k];
						val = cvRound(s);
						((ushort*)b_data)[j] = CV_CAST_16U(val);
					}
					break;
				case CV_16S:
					for( j = 0; j < ncols; j++ )
					{
						double s = 0;
						int val;
						for( k = 0; k < ker_size; k++ )
							s += ((short*)a_data)[j+offset[k]]*k_data[k];
						val = cvRound(s);
						((short*)b_data)[j] = CV_CAST_16S(val);
					}
					break;
				case CV_32S:
					for( j = 0; j < ncols; j++ )
					{
						double s = 0;
						for( k = 0; k < ker_size; k++ )
							s += ((int*)a_data)[j+offset[k]]*k_data[k];
						((int*)b_data)[j] = cvRound(s);
					}
					break;
				case CV_32F:
					if( !all_same )
					{
						for( j = 0; j < ncols; j++ )
						{
							double s = 0;
							for( k = 0; k < ker_size; k++ )
								s += (double)((float*)a_data)[j+offset[k]]*k_data[k];
							((float*)b_data)[j] = (float)s;
						}
					}
					else
					{
						// special branch to speedup feature selection and blur tests
						for( j = 0; j < ncols; j++ )
						{
							double s = 0;
							for( k = 0; k < ker_size; k++ )
								s += (double)((float*)a_data)[j+offset[k]];
							((float*)b_data)[j] = (float)(s*first);
						}
					}
					break;
				case CV_64F:
					for( j = 0; j < ncols; j++ )
					{
						double s = 0;
						for( k = 0; k < ker_size; k++ )
							s += ((double*)a_data)[j+offset[k]]*k_data[k];
						((double*)b_data)[j] = (double)s;
					}
					break;
				default:
					assert(0);
				}
			}

			free( offset );
			free( k_data );
	}

	void GetMaxImg(S16ImageRef a_src1, S16ImageRef a_src2, S16ImageRef a_dst)
	{
		// HTODO : validate identical sizes

		for(int nChnl = 0; nChnl<a_src1->GetNofChannels(); nChnl++)
		{
			S16ChannelRef src1Ch = a_src1->GetAt(nChnl);
			S16ChannelRef src2Ch = a_src2->GetAt(nChnl);
			S16ChannelRef dstCh = a_dst->GetAt(nChnl);

			for(int y=0; y<src1Ch->GetHeight(); y++)
			{
				for(int x=0; x<src1Ch->GetWidth(); x++)
				{
					Int16 nSrc1 = src1Ch->GetAt(x, y);
					Int16 nSrc2 = src2Ch->GetAt(x, y);

					if(nSrc1 > nSrc2)
						dstCh->SetAt(x, y, nSrc1);
					else
						dstCh->SetAt(x, y, nSrc2);
				}
			}

		}
	}

	void CalcColorSblXY(S16ImageRef a_srcX, S16ImageRef a_srcY, S16ImageRef a_dst)
	{
		// HTODO : validate identical sizes

		S16ChannelRef dstCh = a_dst->GetAt(0);

		S16ChannelRef srcXCh0 = a_srcX->GetAt(0);
		S16ChannelRef srcXCh1 = a_srcX->GetAt(1);
		S16ChannelRef srcXCh2 = a_srcX->GetAt(2);

		S16ChannelRef srcYCh0 = a_srcY->GetAt(0);
		S16ChannelRef srcYCh1 = a_srcY->GetAt(1);
		S16ChannelRef srcYCh2 = a_srcY->GetAt(2);

		for(int y=0; y<dstCh->GetHeight(); y++)
		{
			for(int x=0; x<dstCh->GetWidth(); x++)
			{
				int nSrcX0 = srcXCh0->GetAt(x, y);
				int nSrcX1 = srcXCh1->GetAt(x, y);
				int nSrcX2 = srcXCh2->GetAt(x, y);

				int nSrcY0 = srcYCh0->GetAt(x, y);
				int nSrcY1 = srcYCh1->GetAt(x, y);
				int nSrcY2 = srcYCh2->GetAt(x, y);

				double nSum = 0;

				nSum += nSrcX0 * nSrcX0 * 0.51;
				nSum += nSrcX1 * nSrcX1 * 1.44;
				nSum += nSrcX2 * nSrcX2 * 1.048;

				nSum += nSrcY0 * nSrcY0 * 0.51;
				nSum += nSrcY1 * nSrcY1 * 1.44;
				nSum += nSrcY2 * nSrcY2 * 1.048;

				nSum /= 6;

				Int16 nRes = (Int16)sqrt(nSum);
				Uint8 nMaxRes = 255;
				if(nRes >= nMaxRes)
					nRes = 255;
				else
					nRes = nRes * 255 / nMaxRes;

				dstCh->SetAt(x, y, nRes);
			}
		}



	}

	void GetColorMidean(S16ImageRef a_src, S16ImageRef a_dst)
	{
		int nAprSiz = 3;
		int nMarg = nAprSiz / 2;
		double minDif = sqrt((double)3 * 255 * 255);

		std::vector<S16ColorVal> nbrVals(nAprSiz * nAprSiz);
		const int nofNbrs = nbrVals.size();

		S16ChannelRef srcCh0 = a_src->GetAt(0);
		S16ChannelRef srcCh1 = a_src->GetAt(1);
		S16ChannelRef srcCh2 = a_src->GetAt(2);

		S16ChannelRef dstCh0 = a_dst->GetAt(0);
		S16ChannelRef dstCh1 = a_dst->GetAt(1);
		S16ChannelRef dstCh2 = a_dst->GetAt(2);

		for(int y=nMarg; y<dstCh0->GetHeight()-nMarg; y++)
		{
			for(int x=nMarg; x<dstCh0->GetWidth()-nMarg; x++)
			{
				int k = 0;

				int nSrcX = x - nMarg;
				int nSrcY = y - nMarg;

				for(int i=0; i<nAprSiz; i++, nSrcY++)
				{
					for(int j=0; j<nAprSiz; j++, nSrcX++)
					{
						S16ColorVal & cv = nbrVals[k++];

						cv.val0 = srcCh0->GetAt(nSrcX, nSrcY);
						cv.val1 = srcCh1->GetAt(nSrcX, nSrcY);
						cv.val2 = srcCh2->GetAt(nSrcX, nSrcY);
					}
				}

				/////////////////////////////////

				int nMinIndex = 0;
				int nMinIndexSum = 1000000;
				for(int i=0; i < nofNbrs; i++)
				{
					S16ColorVal & cv1 = nbrVals[i];
					int nSum = 0;

					for(int j=0; j < nofNbrs; j++)
					{
						S16ColorVal & cv2 = nbrVals[j];

						nSum += (int) sqrt( (double)
							Sqr( cv1.val0 - cv2.val0 ) +
							Sqr( cv1.val1 - cv2.val1 ) +
							Sqr( cv1.val2 - cv2.val2 ) );
					}

					if( nSum < nMinIndexSum )
					{
						nMinIndexSum = nSum;
						nMinIndex = i;
					}
				}

				S16ColorVal & cv = nbrVals[ nMinIndex ];				

				dstCh0->SetAt(x, y, cv.val0);
				dstCh1->SetAt(x, y, cv.val1);
				dstCh2->SetAt(x, y, cv.val2);


				/////////////////////////////////

				/*				int nSum0 = 0;
				int nSum1 = 0;
				int nSum2 = 0;

				for(int i=0; i<nofNbrs; i++)
				{
				S16ColorVal & cv0 = nbrVals[i];

				nSum0 += cv0.val0;
				nSum1 += cv0.val1;
				nSum2 += cv0.val2;
				}

				S16ColorVal cvm;

				cvm.val0 = nSum0 / nofNbrs;
				cvm.val1 = nSum1 / nofNbrs;
				cvm.val2 = nSum2 / nofNbrs;

				S16ColorVal cv1;

				double minDif1 = minDif;

				int nMinIdx = 0;
				for(int i=0; i < nofNbrs; i++)
				{
				cv1 = nbrVals[i];

				int nDif0 = cv1.val0 - cvm.val0;
				int nDif1 = cv1.val1 - cvm.val1;
				int nDif2 = cv1.val2 - cvm.val2;

				double dif = sqrt( (double)
				(nDif0 * nDif0) +
				(nDif1 * nDif1) +
				(nDif2 * nDif2) );

				if(dif < minDif1)
				{
				nMinIdx = i;
				minDif1 = dif;
				}
				}
				cv1 = nbrVals[nMinIdx];

				dstCh0->SetAt(x, y, cv1.val0);
				dstCh1->SetAt(x, y, cv1.val1);
				dstCh2->SetAt(x, y, cv1.val2);
				*/
			}
		}

	}

	S16ImageRef GenColorMideanImg(S16ImageRef a_src)
	{
		S16ImageRef ret = S16Image::Create(
			a_src->GetSize(), a_src->GetNofChannels());

		GetColorMidean(a_src, ret);
		return ret;
	}

	S16ImageRef GenWeightedColorMideanImg( 
		S16ImageRef a_src, S16ImageRef a_srcGrad, int a_nAprSiz )
	{
		S16ImageRef ret = S16Image::Create(
			a_src->GetSize(), a_src->GetNofChannels());

		//		GetColorMidean(a_src, ret);

		//void GetColorMidean(S16ImageRef a_src, S16ImageRef a_dst)
		//{

		S16ImageRef a_dst = ret;

		int nAprSiz = a_nAprSiz;
		int nMarg = nAprSiz / 2;
		double minDif = sqrt((double)3 * 255 * 255);

		std::vector<S16ColorVal> nbrVals(nAprSiz * nAprSiz);
		const int nofNbrs = nbrVals.size();

		std::vector<int> nbrValWts( nofNbrs );

		S16ChannelRef srcCh0 = a_src->GetAt(0);
		S16ChannelRef srcCh1 = a_src->GetAt(1);
		S16ChannelRef srcCh2 = a_src->GetAt(2);

		S16ChannelRef gradCh0 = a_srcGrad->GetAt(0);

		S16ChannelRef dstCh0 = a_dst->GetAt(0);
		S16ChannelRef dstCh1 = a_dst->GetAt(1);
		S16ChannelRef dstCh2 = a_dst->GetAt(2);

		for(int y=nMarg; y<dstCh0->GetHeight()-nMarg; y++)
		{
			for(int x=nMarg; x<dstCh0->GetWidth()-nMarg; x++)
			{
				int k = 0;

				int nSrcX = x - nMarg;
				int nSrcY = y - nMarg;

				int orgK;
				for(int i=0; i<nAprSiz; i++, nSrcY++)
				{
					for(int j=0; j<nAprSiz; j++, nSrcX++)
					{
						if( nSrcX == x && nSrcY == y )
							orgK = k;

						nbrValWts[ k ] = gradCh0->GetAt(nSrcX, nSrcY);

						if( nbrValWts[ k ] > 15 )
							nbrValWts[ k ] = 100;
						else
							nbrValWts[ k ] = 0;

						nbrValWts[ k ]++; 


						S16ColorVal & cv = nbrVals[k++];

						cv.val0 = srcCh0->GetAt(nSrcX, nSrcY);
						cv.val1 = srcCh1->GetAt(nSrcX, nSrcY);
						cv.val2 = srcCh2->GetAt(nSrcX, nSrcY);
					}
				}

				/////////////////////////////////

				int nMinIndex = 0;
				int nMinIndexSum = 1000000;
				for(int i=0; i < nofNbrs; i++)
				{
					S16ColorVal & cv1 = nbrVals[i];
					int nSum = 0;

					for(int j=0; j < nofNbrs; j++)
					{
						S16ColorVal & cv2 = nbrVals[j];

						int nFact = 1;

						if( j == orgK )
							nFact = 6;


						nSum += nFact * (int) sqrt( (double)
							Sqr( cv1.val0 - cv2.val0 ) +
							Sqr( cv1.val1 - cv2.val1 ) +
							Sqr( cv1.val2 - cv2.val2 ) );
					}

					nSum *= nbrValWts[ i ];

					if( nSum < nMinIndexSum )
					{
						nMinIndexSum = nSum;
						nMinIndex = i;
					}
				}

				S16ColorVal & cv = nbrVals[ nMinIndex ];				

				dstCh0->SetAt(x, y, cv.val0);
				dstCh1->SetAt(x, y, cv.val1);
				dstCh2->SetAt(x, y, cv.val2);
			}
		}



		return ret;
	}

	U8ImageRef GenColorDsp(U8ImageRef a_src)
	{
		Uint8 colors0 [256];
		memset(colors0, 0, 256);

		Uint8 colors1 [256];
		memset(colors1, 0, 256);

		Uint8 colors2 [256];
		memset(colors2, 0, 256);

		for(int i=0; i<128; i++)
		{
			colors1[i] = 255;
			colors2[i] = i * 2;
		}

		for(int i=128, j=0; i<256; i++, j++)
		{
			colors1[i] = 255 - j * 2;
			colors2[i] = 255;
		}

		for(int i=0; i<255; i++)
		{
			colors0[i] = (Uint8)(colors0[i] * (0.25 + 0.75 * i / 255));
			colors1[i] = (Uint8)(colors1[i] * (0.25 + 0.75 * i / 255));
			colors2[i] = (Uint8)(colors2[i] * (0.25 + 0.75 * i / 255));

			//colors2[i] = colors2[i] * i / 255;
		}

		U8ImageRef ret = U8Image::Create( a_src->GetSize(), 3);

		U8ChannelRef srcCh = a_src->GetAt(0);

		U8ChannelRef retCh0 = ret->GetAt(0);
		U8ChannelRef retCh1 = ret->GetAt(1);
		U8ChannelRef retCh2 = ret->GetAt(2);

		for(int y=0; y<srcCh->GetHeight(); y++)
		{
			for(int x=0; x<srcCh->GetWidth(); x++)
			{
				retCh0->SetAt(x, y,
					colors0[ srcCh->GetAt(x, y) ] );

				retCh1->SetAt(x, y,
					colors1[ srcCh->GetAt(x, y) ] );

				retCh2->SetAt(x, y,
					colors2[ srcCh->GetAt(x, y) ] );
			}
		}

		return ret;
	}



	U8ImageRef GenScaledImage(U8ImageRef a_src)
	{
		U8ImageRef ret;
		ret = U8Image::Create( a_src->GetSize(), 1);

		double nMin, nMax;

		HCV_CALL( cvMinMaxLoc( a_src->GetIplImagePtr(), &nMin, &nMax) );

		HCV_CALL( cvConvertScale( a_src->GetIplImagePtr(), 
			ret->GetIplImagePtr(), 
			255 / (nMax - nMin), 
			- nMin) );

		return ret;
	}

	S16ImageRef GenDownSampledImage(S16ImageRef a_src, int a_nStep)
	{
		return GenDownSampledImage(a_src, a_nStep, a_nStep);
	}

	S16ImageRef GenDownSampledImage(S16ImageRef a_src, int a_nStepX, int a_nStepY)
	{
		CvSize retSiz = cvSize(a_src->GetWidth() / a_nStepX,
			a_src->GetHeight() / a_nStepY);

		S16ImageRef ret;
		ret = S16Image::Create( retSiz, a_src->GetNofChannels());


		for(int nCh=0; nCh<ret->GetNofChannels(); nCh++)
		{
			S16ChannelRef srcCh = a_src->GetAt(nCh);
			S16ChannelRef retCh = ret->GetAt(nCh);


			for(int y=0, nSrcY=0; y<retCh->GetHeight(); y++, nSrcY+=a_nStepY)
			{
				for(int x=0, nSrcX=0; x<retCh->GetWidth(); x++, nSrcX+=a_nStepX)
				{
					retCh->SetAt(x, y,
						srcCh->GetAt(nSrcX, nSrcY));
				}
			}

		}

		return ret;
	}

	S16ImageRef GenUpSampledImage(S16ImageRef a_src, int a_nStep)
	{
		return GenUpSampledImage(a_src, a_nStep, a_nStep);
	}

	S16ImageRef GenUpSampledImage(S16ImageRef a_src, int a_nStepX, int a_nStepY)
	{
		CvSize retSiz = cvSize(a_src->GetWidth() * a_nStepX,
			a_src->GetHeight() * a_nStepY);

		S16ImageRef ret;
		ret = S16Image::Create( retSiz, a_src->GetNofChannels());


		for(int nCh=0; nCh<ret->GetNofChannels(); nCh++)
		{
			S16ChannelRef srcCh = a_src->GetAt(nCh);
			S16ChannelRef retCh = ret->GetAt(nCh);


			for(int y=0; y<retCh->GetHeight(); y++)
			{
				for(int x=0; x<retCh->GetWidth(); x++)
				{
					retCh->SetAt(x, y,
						srcCh->GetAt(x / a_nStepX, y / a_nStepY));
				}
			}

		}

		return ret;
	}

	S16ImageRef GenColorSobelGradImage(S16ImageRef a_src, int a_nAprSiz)
	{
		S16ImageRef ret;

		// X

		ConvKernelRef sblKer = ConvKernel::CreateSobel(1, 0, a_nAprSiz);

		S16ImageRef resX = S16Image::Create(
			a_src->GetSize(), a_src->GetNofChannels());

		sblKer->Process(a_src, resX);

		// Y

		sblKer = ConvKernel::CreateSobel(0, 1, a_nAprSiz);

		S16ImageRef resY = S16Image::Create(
			a_src->GetSize(), a_src->GetNofChannels());

		sblKer->Process(a_src, resY);

		//	XY

		ret = S16Image::Create(
			a_src->GetSize(), 1);

		CalcColorSblXY(resX, resY, ret);


		return ret;
	}

	S16ImageRef GenS16FromU8Image(U8ImageRef a_src)
	{
		S16ImageRef ret;

		ret = S16Image::Create(
			cvGetSize(a_src->GetIplImagePtr()), a_src->GetNofChannels());

		HCV_CALL( cvConvertScale( a_src->GetIplImagePtr(), 
			ret->GetIplImagePtr()) );

		return ret;
	}

	U8ImageRef GenU8FromS16Image(S16ImageRef a_src)
	{
		U8ImageRef ret;

		ret = U8Image::Create(
			cvGetSize(a_src->GetIplImagePtr()), a_src->GetNofChannels());

		if(1 == a_src->GetNofChannels())
		{
			double dMin, dMax;

			HCV_CALL( cvMinMaxLoc( a_src->GetIplImagePtr(), &dMin, &dMax) );

			if(dMin < 0 || dMax > 255)
			{
				HCV_CALL( cvConvertScale( a_src->GetIplImagePtr(), 
					ret->GetIplImagePtr(), (255/ (dMax - dMin) ), -dMin) );
			}
			else
			{
				HCV_CALL( cvConvertScale( a_src->GetIplImagePtr(), 
					ret->GetIplImagePtr()) );
			}
		}
		else
		{
			HCV_CALL( cvConvertScale( a_src->GetIplImagePtr(), 
				ret->GetIplImagePtr()) );
		}


		return ret;
	}


	S16ImageRef GenScaledS16FromS32Image(S32ImageRef a_src)
	{
		S16ImageRef ret;

		ret = S16Image::Create(
			cvGetSize(a_src->GetIplImagePtr()), a_src->GetNofChannels());

		double dMin, dMax;

		HCV_CALL( cvMinMaxLoc( a_src->GetIplImagePtr(), &dMin, &dMax) );

		if(dMin < 0 || dMax > 255)
		{
			HCV_CALL( cvConvertScale( a_src->GetIplImagePtr(), 
				ret->GetIplImagePtr(), (255/ (dMax - dMin) ), -dMin) );
		}
		else
		{
			HCV_CALL( cvConvertScale( a_src->GetIplImagePtr(), 
				ret->GetIplImagePtr()) );
		}

		return ret;
	}

	void ShowImage(IplImage * a_iplImagePtr, const char * a_sWndName)
	{
		HCV_CALL( cvNamedWindow( a_sWndName, 1 ) );
		HCV_CALL( cvShowImage( a_sWndName, a_iplImagePtr) );
	}


	S16ImageRef GenMinimaImage(S16ImageRef a_src)
	{
		S16ImageRef ret;

		ret = S16Image::Create(
			cvGetSize(a_src->GetIplImagePtr()), 1);

		int nAprSiz = 3;
		int nMarg = nAprSiz / 2;

		FillBorder(ret->GetAt(0), (Int16)0, nMarg);

		S16ChannelRef srcCh = a_src->GetAt(0);
		S16ChannelRef retCh = ret->GetAt(0);


		for(int y=nMarg; y<retCh->GetHeight()-nMarg; y++)
		{
			for(int x=nMarg; x<retCh->GetWidth()-nMarg; x++)
			{		
				int nVal = srcCh->GetAt(x, y);
				int nRank = nAprSiz * nAprSiz;
				int nMax = nRank - 1;
				bool bNotLvl = false;

				for(int nSrcY=y-nMarg, nMaskY=0; nMaskY<nAprSiz; nSrcY++, nMaskY++)
				{
					for(int nSrcX=x-nMarg, nMaskX=0; nMaskX<nAprSiz; nSrcX++, nMaskX++)
					{
						int nNbrVal = srcCh->GetAt(nSrcX, nSrcY);

						if(nVal <= nNbrVal)
							nRank--;
						if(nVal != nNbrVal)
							bNotLvl = true;
					}
				}

				/*				if(0 == nVal)
				retCh->SetAt(x, y, 0);
				else
				retCh->SetAt(x, y, 255);*/

				//						if(0 == nVal)
				//					retCh->SetAt(x, y, (Int16)(nMax / 1.3));

				//				if(703 == x && 334 == y)
				//					x = x;

				if(0 == nVal && bNotLvl)
					retCh->SetAt(x, y, 0);
				else if(0 == nRank && bNotLvl)
					retCh->SetAt(x, y, nMax / 4);
				else if(0 == nRank)
					retCh->SetAt(x, y, nMax / 2);
				else
					retCh->SetAt(x, y, nMax);

				/*				if(0 == nVal && bNotLvl)
				retCh->SetAt(x, y, 0);
				else if(0 == nVal)
				retCh->SetAt(x, y, nMax / 2);
				else
				retCh->SetAt(x, y, nMax);*/

				/*				if(bNotLvl)
				retCh->SetAt(x, y, nRank);
				else
				retCh->SetAt(x, y, nMax * 2);*/


				/*				if(0 == nRank)
				retCh->SetAt(x, y, 0);
				else
				retCh->SetAt(x, y, 255);*/
			}
		}

		return ret;
	}



	S16ImageRef GenMaximaImage(S16ImageRef a_src)
	{
		S16ImageRef ret;

		ret = S16Image::Create(
			cvGetSize(a_src->GetIplImagePtr()), 1);

		int nAprSiz = 3;
		int nMarg = nAprSiz / 2;

		FillBorder(ret->GetAt(0), (Int16)0, nMarg);

		S16ChannelRef srcCh = a_src->GetAt(0);
		S16ChannelRef retCh = ret->GetAt(0);


		for(int y=nMarg; y<retCh->GetHeight()-nMarg; y++)
		{
			for(int x=nMarg; x<retCh->GetWidth()-nMarg; x++)
			{		
				int nVal = srcCh->GetAt(x, y);
				int nRank = nAprSiz * nAprSiz;
				int nMax = nRank - 1;
				bool bNotLvl = false;

				for(int nSrcY=y-nMarg, nMaskY=0; nMaskY<nAprSiz; nSrcY++, nMaskY++)
				{
					for(int nSrcX=x-nMarg, nMaskX=0; nMaskX<nAprSiz; nSrcX++, nMaskX++)
					{
						int nNbrVal = srcCh->GetAt(nSrcX, nSrcY);

						if(nVal >= nNbrVal)
							nRank--;
						if(nVal != nNbrVal)
							bNotLvl = true;
					}
				}

				/*				if(0 == nVal)
				retCh->SetAt(x, y, 0);
				else
				retCh->SetAt(x, y, 255);*/

				if(bNotLvl)
					retCh->SetAt(x, y, nRank);
				else
					retCh->SetAt(x, y, nMax);


				/*				if(0 == nRank)
				retCh->SetAt(x, y, 0);
				else
				retCh->SetAt(x, y, 255);*/
			}
		}

		return ret;
	}



	U8ImageRef GenNegativeImage(U8ImageRef a_src)
	{
		U8ImageRef ret;

		ret = U8Image::Create(
			a_src->GetSize(), a_src->GetNofChannels());

		for(int nCh=0; nCh<ret->GetNofChannels(); nCh++)
		{
			U8ChannelRef srcCh = a_src->GetAt(nCh);
			U8ChannelRef retCh = ret->GetAt(nCh);


			for(int y=0; y<retCh->GetHeight(); y++)
			{
				for(int x=0; x<retCh->GetWidth(); x++)
				{
					retCh->SetAt(x, y,
						255 - srcCh->GetAt(x, y));
				}
			}

		}

		return ret;
	}


	S32ImageRef GenNeighbourhoodImage(S16ImageRef a_src)
	{
		S32ImageRef ret;

		ret = S32Image::Create(
			cvSize(256, 256), 1);


		S16ChannelRef srcCh = a_src->GetAt(0);
		S32ChannelRef retCh = ret->GetAt(0);

		retCh->SetAll(0);

		for(int y=0; y<srcCh->GetHeight()-1; y++)
		{
			for(int x=1; x<srcCh->GetWidth()-1; x++)
			{		
				int nVal = srcCh->GetAt(x, y);

				retCh->IncAt(nVal, srcCh->GetAt(x + 1, y), 1);
				retCh->IncAt(nVal, srcCh->GetAt(x + 1, y + 1), 1);
				retCh->IncAt(nVal, srcCh->GetAt(x, y + 1), 1);
				retCh->IncAt(nVal, srcCh->GetAt(x - 1, y + 1), 1);
			}
		}

		for(int y=0, x=0; y<srcCh->GetHeight()-1; y++)
		{
			int nVal = srcCh->GetAt(x, y);

			retCh->IncAt(nVal, srcCh->GetAt(x + 1, y), 1);
			retCh->IncAt(nVal, srcCh->GetAt(x + 1, y + 1), 1);
			retCh->IncAt(nVal, srcCh->GetAt(x, y + 1), 1);
		}

		for(int y=srcCh->GetHeight()-1, x=0; x<srcCh->GetWidth()-1; x++)
		{		
			int nVal = srcCh->GetAt(x, y);
			retCh->IncAt(nVal, srcCh->GetAt(x + 1, y), 1);
		}

		for(int y=0, x=srcCh->GetWidth()-1; y<srcCh->GetHeight()-1; y++)
		{
			int nVal = srcCh->GetAt(x, y);

			retCh->IncAt(nVal, srcCh->GetAt(x, y + 1), 1);
			retCh->IncAt(nVal, srcCh->GetAt(x - 1, y + 1), 1);
		}

		///////////////////

		for(int y=0; y<retCh->GetHeight(); y++)
		{
			for(int x=y+1; x<retCh->GetWidth(); x++)
			{
				int nVal = (retCh->GetAt(x, y) +
					retCh->GetAt(y, x)) / 2;
				retCh->SetAt(x, y, nVal);
				retCh->SetAt(y, x, nVal);
			}
		}

		return ret;
	}

	S16ImageRef GenPyrDownImg(S16ImageRef a_src)
	{
		S16ImageRef ret;

		CvSize retSiz = cvGetSize(a_src->GetIplImagePtr());
		retSiz.width /= 2; retSiz.width++;
		retSiz.height /= 2; retSiz.height++;

		ret = S16Image::Create(retSiz, a_src->GetNofChannels());
		ret->SetAll(0);

		cvPyrDown (a_src->GetIplImagePtr(), ret->GetIplImagePtr());

		for(int i=0; i<ret->GetNofChannels(); i++)
		{
			FillBorder(ret->GetAt(i), (Int16)0, 1);
		}

		return ret;
	}

	std::vector<CvPoint> GenDataPoints(S16ImageRef a_src, const int nDataVal)
	{
		std::vector<CvPoint> dataPts(1000);
		dataPts.resize(0);

		S16ChannelRef ch0 = a_src->GetAt(0);

		for(int y=0; y<ch0->GetHeight(); y++)
		{
			for(int x=0; x<ch0->GetWidth(); x++)
			{
				if(nDataVal == ch0->GetAt(x, y))
					dataPts.push_back(cvPoint(x, y));
			}
		}

		return dataPts;
	}

	S16ImageRef GenDataImage(std::vector<CvPoint> a_data, CvSize a_imgSiz,
		int nDataVal, const int nBkgVal)
	{
		S16ImageRef ret;

		ret = S16Image::Create(a_imgSiz, 1);
		ret->SetAll(nBkgVal);

		S16ChannelRef ch0 = ret->GetAt(0);

		for(int i=0; i<a_data.size(); i++)
		{
			ch0->SetAt(a_data[i].x, a_data[i].y, nDataVal);
		}

		return ret;
	}

	S16ImageRef GenHDensityImg(S16ImageRef a_hdifImg)
	{
		S16ImageRef ret;

		const int nIntvX = 23;

		CvSize retSiz = cvGetSize(a_hdifImg->GetIplImagePtr());
		retSiz.width /= nIntvX; retSiz.width--;

		ret = S16Image::Create(retSiz, 1);
		//ret->SetAll(0);

		S16ChannelRef srcCh0 = a_hdifImg->GetAt(0);
		S16ChannelRef retCh0 = ret->GetAt(0);

		const int nScanSiz = nIntvX * 2;

		for(int y=0; y<retCh0->GetHeight(); y++)
		{
			for(int x=0; x<retCh0->GetWidth(); x++)
			{
				int srcX = x * nIntvX;

				int nSum = 0;

				for(int i=0; i<nScanSiz; i++, srcX++)
					nSum += srcCh0->GetAt(srcX, y);

				retCh0->SetAt(x, y, nSum);
			}
		}

		return ret;
	}




	S16ImageRef GenThresholdImg(S16ImageRef a_src, int a_nVal)
	{
		S16ImageRef ret;

		//const int nIntvX = 23;

		CvSize retSiz = cvGetSize(a_src->GetIplImagePtr());

		ret = S16Image::Create(retSiz, a_src->GetNofChannels());

		S16ChannelRef srcCh0 = a_src->GetAt(0);
		//		S16ChannelRef srcCh1 = a_src->GetAt(1);
		//		S16ChannelRef srcCh2 = a_src->GetAt(2);

		S16ChannelRef retCh0 = ret->GetAt(0);
		//		S16ChannelRef retCh1 = ret->GetAt(1);
		//		S16ChannelRef retCh2 = ret->GetAt(2);

		for(int y=0; y<retCh0->GetHeight(); y++)
		{
			for(int x=0; x<retCh0->GetWidth(); x++)
			{
				int nVal;

				nVal = srcCh0->GetAt(x, y);
				if(nVal >= a_nVal)
					retCh0->SetAt(x, y, 255);
				else
					retCh0->SetAt(x, y, 0);

				/*				nVal = srcCh1->GetAt(x, y);
				if(nVal >= a_nVal)
				retCh1->SetAt(x, y, 255);
				else
				retCh1->SetAt(x, y, 0);

				nVal = srcCh2->GetAt(x, y);
				if(nVal >= a_nVal)
				retCh2->SetAt(x, y, 255);
				else
				retCh2->SetAt(x, y, 0);*/
			}
		}

		return ret;
	}




	void DrawPoints(std::vector<CvPoint> a_data, S16ImageRef a_img, CvScalar a_color)
	{
		DrawPoints(&a_data[0], a_data.size(), a_img, a_color);
	}


	void DrawPoints(CvPoint * a_data, int a_nofPoints, 
		S16ImageRef a_img, CvScalar a_color)
	{
		const int nBlue = a_color.val[0];
		const int nGreen = a_color.val[1];
		const int nRed = a_color.val[2];

		S16ChannelRef chB = a_img->GetAt(0);
		S16ChannelRef chG = a_img->GetAt(1);
		S16ChannelRef chR = a_img->GetAt(2);

		for(int i=0; i<a_nofPoints; i++)
		{
			chB->SetAt(a_data[i].x, a_data[i].y, nBlue);
			chG->SetAt(a_data[i].x, a_data[i].y, nGreen);
			chR->SetAt(a_data[i].x, a_data[i].y, nRed);
		}
	}








	void DrawPoints(S16ImageRef a_img, std::vector<CvPoint> * a_pPointArr,
		std::vector<int> * a_pPointIDs, std::vector<U8ColorVal> * a_pColorArr)
	{
		std::vector<CvPoint> & pointArr = *a_pPointArr;
		std::vector<int> & pointIDs = *a_pPointIDs;
		std::vector<U8ColorVal> & colorArr = *a_pColorArr;

		HCV_ERROR();


		//DrawPoints(a_img, &pointArr[0], 
		//	pointArr.size(), &pointIDs[0], &colorArr[0]);
	}


	void DrawPoints(S16ImageRef a_img, CvPoint * a_points, 
		//int a_nofPoints, int * a_ids, U8ColorVal * a_colors)
		int a_nofPoints, ClusteringMgr & a_rCm, U8ColorVal * a_colors)
	{
		S16ChannelRef ch0 = a_img->GetAt(0);
		S16ChannelRef ch1 = a_img->GetAt(1);
		S16ChannelRef ch2 = a_img->GetAt(2);

		for(int i=0; i<a_nofPoints; i++)
		{
			U8ColorVal color = 
				a_colors[ a_rCm.GetPatClusterID(i) ];

			ch0->SetAt(a_points[i].x, a_points[i].y, color.val0);
			ch1->SetAt(a_points[i].x, a_points[i].y, color.val1);
			ch2->SetAt(a_points[i].x, a_points[i].y, color.val2);
		}
	}

	U8ColorVal u8ColorVal(Uint8 a_val0, Uint8 a_val1, Uint8 a_val2)
	{
		U8ColorVal ret;

		ret.val0 = a_val0;
		ret.val1 = a_val1;
		ret.val2 = a_val2;

		return ret;
	}

	S16ImageRef GenFastAvgHImg(S16ImageRef a_src, int a_nAprSiz)
	{
		S16ImageRef ret;

		const int nMarg = a_nAprSiz / 2;

		CvSize srcSiz = a_src->GetSize();
		CvSize retSiz = cvSize( srcSiz.width - 2 * nMarg, srcSiz.height);

		ret = S16Image::Create(retSiz, a_src->GetNofChannels());

		for(int nCh=0; nCh<a_src->GetNofChannels(); nCh++)
		{
			S16ChannelRef srcCh = a_src->GetAt(nCh);
			S16ChannelRef retCh = ret->GetAt(nCh);

			for(int y=0; y<retCh->GetHeight(); y++)
			{
				int nSum = 0;

				for(int nSrcX=0; nSrcX < a_nAprSiz; nSrcX++)
				{
					nSum += srcCh->GetAt( nSrcX, y);
				}
				retCh->SetAt( 0, y, nSum / a_nAprSiz );

				int nSrcXSub = 0;
				int nSrcXAdd = a_nAprSiz;

				for(int x=1; x < retCh->GetWidth(); x++, nSrcXSub++, nSrcXAdd++)
				{
					nSum -= srcCh->GetAt( nSrcXSub, y);
					nSum += srcCh->GetAt( nSrcXAdd, y);

					retCh->SetAt( x, y, nSum / a_nAprSiz );
				}
			}
		}

		return ret;
	}





	S16ImageRef GenFastAvgVImg(S16ImageRef a_src, int a_nAprSiz)
	{
		S16ImageRef ret;

		const int nMarg = a_nAprSiz / 2;

		CvSize srcSiz = a_src->GetSize();
		CvSize retSiz = cvSize( srcSiz.width, srcSiz.height - 2 * nMarg);

		ret = S16Image::Create(retSiz, a_src->GetNofChannels());

		for(int nCh=0; nCh<a_src->GetNofChannels(); nCh++)
		{
			S16ChannelRef srcCh = a_src->GetAt(nCh);
			S16ChannelRef retCh = ret->GetAt(nCh);

			for(int x=0; x<retCh->GetWidth(); x++)
			{
				int nSum = 0;

				for(int nSrcY=0; nSrcY < a_nAprSiz; nSrcY++)
				{
					nSum += srcCh->GetAt( x, nSrcY);
				}
				retCh->SetAt( x, 0, nSum / a_nAprSiz );

				int nSrcYSub = 0;
				int nSrcYAdd = a_nAprSiz;

				for(int y=1; y < retCh->GetHeight(); y++, nSrcYSub++, nSrcYAdd++)
				{
					nSum -= srcCh->GetAt( x, nSrcYSub);
					nSum += srcCh->GetAt( x, nSrcYAdd);

					retCh->SetAt( x, y, nSum / a_nAprSiz );
				}
			}
		}

		return ret;
	}

	S16ImageRef GenFastAvgImg(S16ImageRef a_src, int a_nAprSiz)
	{
		S16ImageRef ret = a_src;

		ret = GenFastAvgHImg( ret, a_nAprSiz );
		ret = GenFastAvgVImg( ret, a_nAprSiz );

		return ret;
	}

	S16ImageRef GenDifMagImg(S16ImageRef a_src)
	{
		S16ImageRef ret = S16Image::Create(
			a_src->GetSize(), 1);

		const int nMaxDif = (int) sqrt( (double) 3 * Sqr( 255 ) );

		S16ChannelRef srcCh0 = a_src->GetAt(0);
		S16ChannelRef srcCh1 = a_src->GetAt(1);
		S16ChannelRef srcCh2 = a_src->GetAt(2);

		S16ChannelRef retCh0 = ret->GetAt(0);


		for(int y=0; y<retCh0->GetHeight(); y++)
		{
			for(int x=0; x<retCh0->GetWidth(); x++)
			{
				short nVal = (short) sqrt( (double)
					Sqr( srcCh0->GetAt( x, y ) ) +
					Sqr( srcCh1->GetAt( x, y ) ) +
					Sqr( srcCh2->GetAt( x, y ) ) );

				nVal =  (nVal * 255) / nMaxDif;


				retCh0->SetAt( x, y, nVal );
			}
		}

		return ret;
	}

	S16ImageRef GenFlattenedImg(S16ImageRef a_src, S16ImageRef a_srcGrad, int a_nAprSiz)
	{
		S32ImageRef srcByGradImg = S32Image::Create( 
			a_src->GetSize(), a_src->GetNofChannels());

		S16ChannelRef gradCh = a_srcGrad->GetAt(0);

		const int nSrcWidth = a_src->GetSize().width;
		const int nSrcHeight = a_src->GetSize().height;


		//	Preparing Weight Img.
		for(int ch=0; ch < a_srcGrad->GetNofChannels(); ch++)
		{
			S16ChannelRef srcGradCh = a_srcGrad->GetAt( ch );

			for(int y=0; y < srcGradCh->GetHeight(); y++)
			{
				for(int x=0; x < srcGradCh->GetWidth(); x++)
				{
					int nVal = srcGradCh->GetAt( x, y );

					//nVal = nVal * nVal * nVal * nVal;
					//nVal = nVal * nVal;
					nVal = nVal * nVal;

					//nVal = 255 - nVal;

//					if( nVal < 230 )
//						nVal /= 1000;

					if( 0 == nVal )
						nVal = 1;

					srcGradCh->SetAt(x, y, nVal );
				}
			}
		}


		//	Multipling a_src by grad.
		{
			for(int ch=0; ch < a_src->GetNofChannels(); ch++)
			{
				S16ChannelRef srcCh = a_src->GetAt( ch );
				S32ChannelRef srcByGradCh = srcByGradImg->GetAt( ch );

				for(int y=0; y < nSrcHeight; y++)
				{
					for(int x=0; x < nSrcWidth; x++)
					{
						srcByGradCh->SetAt(x, y, 
							srcCh->GetAt( x, y ) * gradCh->GetAt( x, y ) );
					}
				}
			}
		}
		//	End of Multipling.

		////////////////////////////////////////////////////////

		S16ImageRef horzImg;
		{
			S16ImageRef ret;

			const int nMarg = a_nAprSiz / 2;

			CvSize srcSiz = a_src->GetSize();
			CvSize retSiz = cvSize( srcSiz.width - 2 * nMarg, srcSiz.height);

			ret = S16Image::Create(retSiz, a_src->GetNofChannels());
			ret->SetAll(0);

			for(int nCh=0; nCh<a_src->GetNofChannels(); nCh++)
			{
				S16ChannelRef retCh = ret->GetAt(nCh);
				S32ChannelRef srcByGradCh = srcByGradImg->GetAt( nCh );

				for(int y=0; y<retCh->GetHeight(); y++)
				{
					int nSumSbg = 0;
					int nSumGrad = 0;

					for(int nSrcX=0; nSrcX < a_nAprSiz; nSrcX++)
					{
						nSumSbg += srcByGradCh->GetAt( nSrcX, y);
						nSumGrad += gradCh->GetAt( nSrcX, y);

					}
					retCh->SetAt( 0, y, nSumSbg / nSumGrad );

					int nSrcXSub = 0;
					int nSrcXAdd = a_nAprSiz;

					for(int x=1; x < retCh->GetWidth(); x++, nSrcXSub++, nSrcXAdd++)
					{
						nSumSbg -= srcByGradCh->GetAt( nSrcXSub, y);
						nSumSbg += srcByGradCh->GetAt( nSrcXAdd, y);

						nSumGrad -= gradCh->GetAt( nSrcXSub, y);
						nSumGrad += gradCh->GetAt( nSrcXAdd, y);

						retCh->SetAt( x, y, nSumSbg / nSumGrad );
					}
				}
			}

			horzImg = ret;

			//	Multipling Horz by grad.
			{
				for(int ch=0; ch < horzImg->GetNofChannels(); ch++)
				{
					S16ChannelRef srcCh = horzImg->GetAt( ch );
					S32ChannelRef srcByGradCh = srcByGradImg->GetAt( ch );

					for(int y=0; y < nSrcHeight; y++)
					{
						for(int x=0; x < nSrcWidth; x++)
						{
							srcByGradCh->SetAt(x, y, 
								srcCh->GetAt( x, y ) * gradCh->GetAt( x + nMarg, y ) );
						}
					}
				}
			}
			//	End of Multipling Horz.

		}
		//	End of Horz




		////////////////////////////////////////////////////////////


		//return srcByGradImg;

		S16ImageRef vertImg;
		{
			S16ImageRef ret;

			const int nMarg = a_nAprSiz / 2;

			CvSize srcSiz = a_src->GetSize();
			CvSize retSiz = cvSize( srcSiz.width, srcSiz.height - 2 * nMarg);

			ret = S16Image::Create(retSiz, a_src->GetNofChannels());
			ret->SetAll(0);

			for(int nCh=0; nCh<a_src->GetNofChannels(); nCh++)
			{
				S16ChannelRef retCh = ret->GetAt(nCh);
				S32ChannelRef srcByGradCh = srcByGradImg->GetAt( nCh );

				for(int x=0; x<retCh->GetWidth(); x++)
				{
					int nSumSbg = 0;
					int nSumGrad = 0;

					for(int nSrcY=0; nSrcY < a_nAprSiz; nSrcY++)
					{
						nSumSbg += srcByGradCh->GetAt( x, nSrcY);
						nSumGrad += gradCh->GetAt( x, nSrcY);

					}
					retCh->SetAt( x, 0, nSumSbg / nSumGrad );

					int nSrcYSub = 0;
					int nSrcYAdd = a_nAprSiz;

					for(int y=1; y < retCh->GetHeight(); y++, nSrcYSub++, nSrcYAdd++)
					{
						nSumSbg -= srcByGradCh->GetAt( x, nSrcYSub);
						nSumSbg += srcByGradCh->GetAt( x, nSrcYAdd);

						nSumGrad -= gradCh->GetAt( x, nSrcYSub);
						nSumGrad += gradCh->GetAt( x, nSrcYAdd);

						retCh->SetAt( x, y, nSumSbg / nSumGrad );
					}
				}
			}

			vertImg = ret;
		}
		//	End of Vert


		return vertImg;
	}


	S16ImageRef GenTriChGrayImg( S16ImageRef a_src )
	{
		S16ImageRef ret;

		ret = S16Image::Create(a_src->GetSize(), a_src->GetNofChannels());

		S16ChannelRef srcCh0 = a_src->GetAt(0);
		S16ChannelRef srcCh1 = a_src->GetAt(1);
		S16ChannelRef srcCh2 = a_src->GetAt(2);

		S16ChannelRef retCh0 = ret->GetAt(0);
		S16ChannelRef retCh1 = ret->GetAt(1);
		S16ChannelRef retCh2 = ret->GetAt(2);

		for(int y=0; y < srcCh0->GetHeight(); y++)
		{
			for(int x=0; x < srcCh0->GetWidth(); x++)
			{
				int nVal = (
					srcCh0->GetAt( x, y ) * 0.51 +
					srcCh1->GetAt( x, y ) * 1.44 +
					srcCh2->GetAt( x, y ) * 1.048 
					) / 3;

				retCh0->SetAt( x, y, (short) nVal );
				retCh1->SetAt( x, y, (short) nVal );
				retCh2->SetAt( x, y, (short) nVal );
			}
		}

		return ret;

	}



	
	SignalOneDimRef GenHorzLineFromImage(S16ImageRef a_src)
	{
		return GenHorzLineFromImage( a_src, a_src->GetHeight() / 2 );
	}


	SignalOneDimRef GenHorzLineFromImage(S16ImageRef a_src, int a_y, int a_nChannel)
	{
		SignalOneDimRef retSign = new SignalOneDim();

		retSign->m_nBgn = 0;

		std::vector<int> & rData = retSign->m_data;
		rData.resize( a_src->GetWidth() );

		S16ChannelRef srcCh = a_src->GetAt( a_nChannel );

		for(int x=0; x < srcCh->GetWidth(); x++)
			rData[ x ] = srcCh->GetAt( x, a_y );
			

		return retSign;
	}


	S16ImageRef GenMorphGradImg(S16ImageRef a_src, int a_nAprSiz)
	{
		S16ImageRef res = a_src;
		const int nAprSiz = a_nAprSiz;


		U8ImageRef resU8 = GenU8FromS16Image(res);

		U8ImageRef resMax =  U8Image::Create( resU8->GetSize(),
			resU8->GetNofChannels() );

		U8ImageRef resMin =  U8Image::Create( resU8->GetSize(),
			resU8->GetNofChannels() );

		//int j = ( nAprSiz / 2 ) + 1;
		//int j = ( nAprSiz / 2 );
		int j = nAprSiz;

		for(int i=0; i<1; i++)
		{
		   cvDilate( resU8->GetIplImagePtr(), 
			   resMax->GetIplImagePtr(), 0, j );

		   cvErode( resU8->GetIplImagePtr(), 
			   resMin->GetIplImagePtr(), 0, j );

		}

		U8ImageRef resDif = U8Image::Create( resU8->GetSize(),
			resU8->GetNofChannels() );

		HCV_CALL(
			cvSub ( 
			resMax->GetIplImagePtr(),
			resMin->GetIplImagePtr(),
			resDif->GetIplImagePtr())
		);

		res = GenS16FromU8Image( resDif );

		if( 3 == res->GetNofChannels() )
			res = GenDifMagImg( res );

		return res;
	}


	void ShowImage(U8ImageRef dsp, const char * a_sWndName)
	{
		ShowImage(dsp->GetIplImagePtr(), a_sWndName);
	}

	void ShowImage(S16ImageRef a_src, const char * a_sWndName)
	{
		U8ImageRef dsp = GenU8FromS16Image(a_src);
		ShowImage(dsp->GetIplImagePtr(), a_sWndName);
	}

	void SaveImage(S16ImageRef a_src, char * a_sFilePath)
	{
		U8ImageRef dsp = GenU8FromS16Image(a_src);

		HCV_CALL( cvvSaveImage(a_sFilePath, 
			dsp->GetIplImagePtr()) );
	}

	S16ImageRef GenImageFromHorzSignal(SignalOneDimRef a_sig, int a_nHeight)
	{
		S16ImageRef ret = S16Image::Create( 
			cvSize(a_sig->m_data.size(), a_nHeight), 3);

		S16ChannelRef retCh0 = ret->GetAt(0);
		S16ChannelRef retCh1 = ret->GetAt(1);
		S16ChannelRef retCh2 = ret->GetAt(2);

		for(int x=0; x < a_sig->m_data.size(); x++)
		{
			short nVal = a_sig->m_data[x];

			for(int y=0; y < a_nHeight; y++)
			{
				retCh0->SetAt(x, y, nVal);
				retCh1->SetAt(x, y, nVal);
				retCh2->SetAt(x, y, nVal);
			}
		}

		return ret;
	}

}