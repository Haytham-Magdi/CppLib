#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\PixelTypes.h>
#include <Lib\Hcv\error.h>
#include <Lib\Hcv\Channel.h>

//#include "D:\HthmWork_D\OpenCV-2.3.1-win-superpack\opencv\build\include\opencv2\legacy\compat.hpp"




namespace Hcv
{
	template<class T>
	class Image : FRM_Object
	{
	public:
		Image(IplImage * a_src)
		{
			Hcv::InitLib();

			Init(a_src);
		}

		CHANNEL_REF(T) GetAt(int a_nChannel)
		{
			return m_channels[a_nChannel];
		}

		IplImage * GetIplImagePtr(void)
		{
			Hcpl_ASSERT( NULL != (IplImage *)m_orgImg );

			return (IplImage *)m_orgImg;
		}

		int GetNofChannels(void)
		{
			return m_orgImg->nChannels;
		}

		int GetWidth(void)
		{
			return m_channels[0]->GetWidth();
		}

		int GetHeight(void)
		{
			return m_channels[0]->GetHeight();
		}

		CvSize GetSize()
		{
			return cvGetSize (this->GetIplImagePtr());
		}

		int GetSize1D()
		{
			CvSize siz = GetSize();

			return siz.width * siz.height;
		}

		int GetSize1D_InBytes()
		{
			return GetSize1D() * sizeof(T);
		}

		CvMat* GetMatPtr()
		{
			return cvGetMat(this->GetIplImagePtr(), &m_mat);
		}

		static Image * Create(CvSize a_siz, int a_nChannels)
		{
			return new Image(cvCreateImage(a_siz, 
				HCV_DEPTH_ID(T), a_nChannels)); 
		}

		void SetAll(T a_val)
		{
			for(unsigned int i=0; i<m_channels.size(); i++)
				m_channels[i]->SetAll(a_val);
		}

		T * GetPixAt(int a_nX, int a_nY)
		{
			return &m_pixs[a_nX * m_nofChannels + a_nY * m_nofLineUnits];
		}

		T * GetDataPtr()
		{
			return GetPixAt(0, 0);
		}

		Image< T > * CloneNew()
		{
			Image<T> * pRet = Image<T>::Create(this->GetSize(), this->GetNofChannels());

			return pRet;
		}

		Image< T > * Clone()
		{
			CvSize siz = this->GetSize();

			Image<T> * pRet = this->CloneNew();
			this->CopyTo(pRet);

			//for(int y=0; y < siz.height; y++)
			//{
			//	for(int x=0; x < siz.width; x++)
			//	{
			//		T * srcPix = this->GetPixAt(x, y);
			//		T * dstPix = pRet->GetPixAt(x, y);

			//		for(int i=0; i < m_nofChannels; i++)
			//		{
			//			dstPix[i] = srcPix[i];
			//		}
			//	}
			//}

			return pRet;
		}

		void CopyTo(Image<T> * destImg)
		{
			Hcpl_ASSERT(AreEqualCvSizes( GetSize(), destImg->GetSize()));
			Hcpl_ASSERT(GetNofChannels() == destImg->GetNofChannels());
			Hcpl_ASSERT(GetSize1D_InBytes() == destImg->GetSize1D_InBytes());

			memcpy(destImg->GetDataPtr(), this->GetDataPtr(), this->GetSize1D_InBytes());

			throw "Not Implemented.";
		}

	protected:
		void Init(IplImage * a_src)
		{
			m_orgImg = a_src;

			int nofChns = m_nofChannels = a_src->nChannels;

			m_channels.resize(0);

			for(int i=0; i<nofChns; i++)
				m_channels.push_back(new Channel<T>(m_orgImg, i));

			{
				//m_nWidth = a_src->width;
				//m_nHeight = a_src->height;				

				m_nofLineUnits = a_src->widthStep / sizeof(T);

				//HCV_CALL( cvGetImageRawData ((IplImage *)a_src, (Hcpl::Uint8 **) &m_pixs, 
				HCV_CALL( cvGetRawData ((IplImage *)a_src, (Hcpl::Uint8 **) &m_pixs, 
					NULL, NULL));			

			//m_pixs = &srcData[m_nBgnX * m_nStepX + a_nChnl +
			//	m_nBgnY * m_nLineLen];

			}

		}



	protected:
		IplImageRef m_orgImg;
		ChannelRefColl<T> m_channels;
		CvMat m_mat;

		int m_nofChannels;

		// nofChannels * nofPixs
		int m_nofLineUnits;

		T * m_pixs;
	};

#define IMAGE_REF(T) Hcpl::ObjRef< Hcv::Image< T > >

	typedef Hcv::Image< Hcpl::Uint8 > U8Image;
	typedef Hcpl::ObjRef< U8Image > U8ImageRef;

	typedef Hcv::Image< Hcpl::Int16 > S16Image;
	typedef Hcpl::ObjRef< S16Image > S16ImageRef;

	typedef Hcv::Image< Hcpl::Int32 > S32Image;
	typedef Hcpl::ObjRef< S32Image > S32ImageRef;

	typedef Hcv::Image< Hcpl::Float > F32Image;
	typedef Hcpl::ObjRef< F32Image > F32ImageRef;


/*	template<class T>
	class ImageRef : Hcpl::ObjRef< Image< T > >
	{
	};*/

}
