#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

namespace Hcv
{
	class ConvKernelFriend;

	class ConvKernel : FRM_Object
	{
	private :
		ConvKernel() {}

	public :
		static ConvKernel * CreateSobel(int a_dx, int a_dy, 
			int a_aprSiz, int a_nSpace = 0);
		static ConvKernel * CreateBlur(int a_aprSiz, int a_nSpace = 0);

		S16ImageRef GenResult(S16ImageRef a_src)
		{
			S16ImageRef ret = S16Image::Create(
				a_src->GetSize(), a_src->GetNofChannels());
			Process(a_src, ret);

			return ret;
		}

		void Process(S16ImageRef a_src, S16ImageRef a_dst);
		ConvKernel * CreatePositiveInst(void);
		ConvKernel * CreateNegativeInst(void);
		ConvKernel * CreateAbsNegativeInst(void);

		friend class ConvKernelFriend;
	
	protected :
		void Prepare(int a_nSpace);
	
	protected :
		F32ImageRef m_img;
		CvPoint m_anch;

		int m_nTopMarg;
		int m_nBotomMarg;
		int m_nLeftMarg;
		int m_nRightMarg;
	};

	typedef Hcpl::ObjRef< ConvKernel > ConvKernelRef;

	class ConvKernelFriend : FRM_Object
	{
	protected :

		F32ImageRef GetConvKernelImg(ConvKernelRef a_ck)
		{
			return a_ck->m_img;
		}

		int GetConvKernelLeftMarg(ConvKernelRef a_ck)
		{
			return a_ck->m_nLeftMarg;
		}

		int GetConvKernelRightMarg(ConvKernelRef a_ck)
		{
			return a_ck->m_nRightMarg;
		}

		int GetConvKernelTopMarg(ConvKernelRef a_ck)
		{
			return a_ck->m_nTopMarg;
		}

		int GetConvKernelBotomMarg(ConvKernelRef a_ck)
		{
			return a_ck->m_nBotomMarg;
		}
		
		CvPoint GetConvKernelAnch(ConvKernelRef a_ck)
		{
			return a_ck->m_anch;
		}

		F32ChannelRef GetConvKernelChannel(ConvKernelRef a_ck)
		{
			return a_ck->m_img->GetAt(0);
		}


	};

}
