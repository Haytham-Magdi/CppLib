#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
//#include <Lib\Cpp\stl\vector>
#include <vector>



namespace Hcv
{
	template<class T>
	class Channel : FRM_Object
	{
	public:
		Channel(IplImageRef a_src, int a_nChnl,
			int a_nBgnX, int a_nBgnY, int a_nLimX, int a_nLimY)
		{
			Init(a_src, a_nChnl,
				a_nBgnX, a_nBgnY, a_nLimX, a_nLimY);
		}

		Channel(IplImageRef a_src, int a_nChnl)
		{
			int nBgnX = 0;
			int nBgnY = 0;
			int nLimX = a_src->width;
			int nLimY = a_src->height;

			Init(a_src, a_nChnl,
				nBgnX, nBgnY, nLimX, nLimY);
		}

		int GetWidth()
		{
			return m_nWidth;
		}

		int GetHeight()
		{
			return m_nHeight;
		}

		T GetAt(int a_nX, int a_nY)
		{
			return m_pixs[a_nX * m_nStepX + a_nY * m_nLineLen];
		}

		void SetAt(int a_nX, int a_nY, T a_val)
		{
			m_pixs[a_nX * m_nStepX + a_nY * m_nLineLen] = a_val;
		}

		void IncAt(int a_nX, int a_nY, T a_val)
		{
			m_pixs[a_nX * m_nStepX + a_nY * m_nLineLen] += a_val;
		}

		void DecAt(int a_nX, int a_nY, T a_val)
		{
			m_pixs[a_nX * m_nStepX + a_nY * m_nLineLen] -= a_val;
		}

		void SetAll(T a_val)
		{
			for(int y=0; y<this->GetHeight(); y++)
			{
				for(int x=0; x<this->GetWidth(); x++)
				{		
					this->SetAt(x, y, a_val);
				}
			}
		}

	protected :

		void Init(IplImageRef a_src, int a_nChnl,
			int a_nBgnX, int a_nBgnY, int a_nLimX, int a_nLimY)
		{
			Hcpl_ASSERT(HCV_DEPTH_ID(T) == a_src->depth);

			m_orgImg = a_src;

			m_nBgnX = a_nBgnX;
			m_nBgnY = a_nBgnY;
			m_nLimX = a_nLimX;
			m_nLimY = a_nLimY;

			m_nWidth = a_nLimX - m_nBgnX;
			m_nHeight = a_nLimY - m_nBgnY;

			m_nStepX = a_src->nChannels;
			m_nLineLen = a_src->widthStep / sizeof(T);

			T * srcData;
			//HCV_CALL( cvGetImageRawData ((IplImage *)a_src, (Hcpl::Uint8 **) &srcData, 
			HCV_CALL( cvGetRawData ((IplImage *)a_src, (Hcpl::Uint8 **) &srcData, 
				NULL, NULL));			

			m_pixs = &srcData[m_nBgnX * m_nStepX + a_nChnl +
				m_nBgnY * m_nLineLen];

//Error:
			return;
		}


	protected :
		int m_nChnl;
		IplImageRef m_orgImg;
		CvMatRef m_orgMat;

		int m_nBgnX;
		int m_nBgnY;
		int m_nLimX;
		int m_nLimY;

		int m_nStepX;
		int m_nLineLen;
		int m_nWidth;
		int m_nHeight;
		T * m_pixs;
	};


#define CHANNEL_REF(T)		Hcpl::ObjRef< Channel< T > >

	typedef Channel< Hcpl::Uint8 > U8Channel;
	typedef Hcpl::ObjRef< U8Channel > U8ChannelRef;

	typedef Channel< Hcpl::Int16 > S16Channel;
	typedef Hcpl::ObjRef< S16Channel > S16ChannelRef;

	typedef Channel< Hcpl::Int32 > S32Channel;
	typedef Hcpl::ObjRef< S32Channel > S32ChannelRef;

	typedef Channel< Hcpl::Float > F32Channel;
	typedef Hcpl::ObjRef< F32Channel > F32ChannelRef;

	template<class T>
	class ChannelRefColl : public std::vector< Hcpl::ObjRef< Channel< T > > >
	{
	};
}