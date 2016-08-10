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
	using namespace Hcpl::Math;

	typedef struct _ClusterInfo
	{
		int refID;
		int size;
	}ClusterInfo;

	class ClusteringMgr : FRM_Object
	{
	public:
		ClusteringMgr() : m_clusterInfoArr(1000), m_nHugeDist(0)
		{
			memset( (char *) &m_nHugeDist, 0x7f, sizeof(m_nHugeDist));
			m_nofIndirs = 20;
			m_nofGuids = 5;
		}

	virtual void Process( int * a_pats, IndexCalc2D * a_pPatIndex);


	int * GetPatIDs()
	{
		return &m_patIDs[0];
	}

	int GetPatClusterID(int a_index)
	{
		const int nID = m_patIDs[ a_index ];
		const int nClustInfID = m_patClusterInfoIDs[ nID ];

		return nClustInfID;
	}

	int GetNofClusters()
	{
		return m_clusterInfoArr.size();
	}

	int * GetResPats()
	{
		return &m_resPats[0];
	}


	protected:

		virtual void AssignNbrs();
		inline int CalcDist(int * feats1, int *feats2);


	protected:
		int * m_pats;
		IndexCalc2D * m_pPatIndex;

		std::vector<int> m_resPats;

		int m_nDistStep;
		int m_nNbrDist;
		const int m_nHugeDist;

		int m_nofNbrs;
		std::vector<int> m_nbrs;
		std::vector<int> m_nbrDists;

		int m_nofIndirs;
		int m_nofGuids;

		std::vector<int> m_patIDs;
		std::vector<int> m_patClusterInfoIDs;
		//int m_nofClusters;

		std::vector<ClusterInfo> m_clusterInfoArr;
	};


	typedef Hcpl::ObjRef< ClusteringMgr > ClusteringMgrRef;
}