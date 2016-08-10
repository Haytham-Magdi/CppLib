#pragma once


#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

#include <Lib\Hcv\EdgeScan.h>


namespace Hcv
{

	class ScanTryDbg_2
	{
	private:
		ScanTryDbg_2()
		{

		}

		~ScanTryDbg_2()
		{

		}

	public:

		class ScanTryDbg_2Core
		{
		public:

			ScanTryDbg_2Core()
			{
				Reset();
			}

			void Reset()
			{
				SliCur = NULL;
				SliMark = NULL;

				RootPnt = F32Point( 0, 0 );

				nIOA = 0;

				nEI_CntGlob = 0;

				markDistToCent = 1000000;


				//nGrandCnt = -1;
			}

			void CreateMarkImg( CvSize a_imgSiz );

			F32ImageRef GetMarkImg( )
			{
				return m_markImg;
			}

			void MarkPoint( int a_x, int a_y, int a_nRadius );


		public:

			F32Point RootPnt;

			F32Point markCentPnt;
			F32Point markClosePnt;

			float markDistToCent;

			int nIOA;
			int nIOA_XY;

			//int nIOL;

			int nEI_CntGlob;

			int nEI_Index;

			//int nGrandCnt;

			ImgGradMgr_2::ScanLineItrRef SliCur;
			ImgGradMgr_2::ScanLineItrRef SliMark;

		protected:
			
			F32ImageRef m_markImg;
		};

		static ScanTryDbg_2Core Core;
	};

}