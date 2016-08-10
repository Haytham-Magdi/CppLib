#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

#include <Lib\Hcv\ClusteringMgr.h>

namespace Hcv
{
	using namespace Hcpl::Math;

	typedef struct _PixPattern
	{
		//int x;
		//int y;
		int b;
		int g;
		int r;
	}PixPattern;

	class ImageClusteringMgr : public ClusteringMgr
	{
	public:

		typedef enum
		{
			RC = 0, RB, CB, LB
		}NbrPixDir;

		// override
		void Process( int * a_pats, IndexCalc2D * a_pPatIndex);

		virtual void Process( S16ImageRef a_img );

		S16ImageRef GenResultImage();
		S16ImageRef GenRgnImage();
		S16ImageRef GenMaxRgnImage();

	protected:

		void CreatePats();

		// override
		void AssignNbrs();

		inline void AssignDirDist(int a_orgX, int a_orgY, int a_i, 
			int * a_featsI, int * a_locNbrsI, int * a_locNbrDistsI,
			NbrPixDir a_dir, IndexCalc2D & a_rImgIndex, 
			IndexCalc2D & a_rPatIndex, IndexCalc2D & a_rNbrIndex);

		inline int GetNbrPixIndex(int a_orgX, int a_orgY, 
			NbrPixDir a_dir, IndexCalc2D & a_rImgIndex);

	protected:
		S16ImageRef m_img;
		std::vector<int> m_imgPats;

	};

	typedef Hcpl::ObjRef< ImageClusteringMgr > ImageClusteringMgrRef;
}