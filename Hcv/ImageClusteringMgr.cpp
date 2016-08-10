#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\ImageClusteringMgr.h>

namespace Hcv
{
	using namespace Hcpl;
	using namespace Hcpl::Math;

	void ImageClusteringMgr::Process(
		int * a_pats,
		IndexCalc2D * a_pPatIndex
		)
	{
		HCV_ERROR();
	}


	void ImageClusteringMgr::Process( S16ImageRef a_img )
	{
		m_img = a_img;

		CreatePats();
		AssignNbrs();

		ClusteringMgr::Process(m_pats, m_pPatIndex);		
	}


	void ImageClusteringMgr::CreatePats()
	{
		CvSize imgSiz = m_img->GetSize();

		m_pPatIndex = new IndexCalc2D(
			sizeof(PixPattern) / sizeof(int),
			imgSiz.width * imgSiz.height);
		IndexCalc2D & patIndex = *m_pPatIndex;

		m_imgPats.resize( patIndex.GetTotalSize() );

		m_pats = &m_imgPats[0];

		PixPattern * pixPats = (PixPattern *) m_pats;


		S16ChannelRef ch0 = m_img->GetAt(0);
		S16ChannelRef ch1 = m_img->GetAt(1);
		S16ChannelRef ch2 = m_img->GetAt(2);

		IndexCalc2D imgIndex(imgSiz.width, imgSiz.height);
		for(int y=0; y<imgSiz.height; y++)
		{
			for(int x=0; x<imgSiz.width; x++)
			{
				PixPattern * pPat = 
					&pixPats[ imgIndex.Calc(x, y) ];

				//pPat->x = x;
				//pPat->y = y;
				pPat->b = ch0->GetAt(x, y);
				pPat->g = ch1->GetAt(x, y);
				pPat->r = ch2->GetAt(x, y);
			}
		}



	}



	inline int ImageClusteringMgr::GetNbrPixIndex(int a_orgX, int a_orgY, 
		NbrPixDir a_dir, IndexCalc2D & a_rImgIndex)
	{	
		static int dx[] = {1, 1, 0, -1};
		static int dy[] = {0, 1, 1, 1};

		int x = a_orgX + dx[ (int)a_dir ];
		int y = a_orgY + dy[ (int)a_dir ];

		return a_rImgIndex.Calc(x, y);
	}




	void ImageClusteringMgr::AssignNbrs()
	{
		IndexCalc2D patIndex(m_pPatIndex->GetSizeX(), 
			m_pPatIndex->GetSizeY());

		const int nofFeatures = patIndex.GetSizeX();

		const int nDataSiz = patIndex.GetSizeY();

		const int nDistStep = m_nDistStep = 10;		
		//const int nNbrDist = m_nNbrDist = 30 * nDistStep;
		const int nNbrDist = m_nNbrDist = 10 * nDistStep;

		const int nofNbrs = m_nofNbrs = 8;


		IndexCalc2D nbrIndex( nofNbrs, nDataSiz );


		m_nbrDists.resize( nbrIndex.GetTotalSize() );
		memset( &m_nbrDists[0], 0x7f, m_nbrDists.size() * sizeof(int));

		m_nbrs.resize( nbrIndex.GetTotalSize() );



		const int nImgWidth = m_img->GetWidth();
		const int nImgHeight = m_img->GetHeight();

		IndexCalc2D imgIndex(nImgWidth, nImgHeight);

		for(int y=0; y<nImgHeight-1; y++)
		{
			for(int x=1; x<nImgWidth-1; x++)
			{	
				//	RC, RB, CB, LB

				int i = imgIndex.Calc(x, y);

				int * featsI = &m_pats[ patIndex.Calc(0, i) ];
				int * locNbrsI = 
					&m_nbrs[ nbrIndex.Calc(0, i) ];
				int * locNbrDistsI = 
					&m_nbrDists[ nbrIndex.Calc(0, i) ];


				AssignDirDist(x, y, i, featsI, locNbrsI, locNbrDistsI, 
					RC, imgIndex, patIndex, nbrIndex);

				AssignDirDist(x, y, i, featsI, locNbrsI, locNbrDistsI, 
					RB, imgIndex, patIndex, nbrIndex);

				AssignDirDist(x, y, i, featsI, locNbrsI, locNbrDistsI, 
					CB, imgIndex, patIndex, nbrIndex);

				AssignDirDist(x, y, i, featsI, locNbrsI, locNbrDistsI, 
					LB, imgIndex, patIndex, nbrIndex);
			}
		}

		for(int y=0, x=0; y<nImgHeight-1; y++)
		{
			//	RC, RB, CB

			int i = imgIndex.Calc(x, y);

			int * featsI = &m_pats[ patIndex.Calc(0, i) ];
			int * locNbrsI = 
				&m_nbrs[ nbrIndex.Calc(0, i) ];
			int * locNbrDistsI = 
				&m_nbrDists[ nbrIndex.Calc(0, i) ];


			AssignDirDist(x, y, i, featsI, locNbrsI, locNbrDistsI, 
				RC, imgIndex, patIndex, nbrIndex);

			AssignDirDist(x, y, i, featsI, locNbrsI, locNbrDistsI, 
				RB, imgIndex, patIndex, nbrIndex);

			AssignDirDist(x, y, i, featsI, locNbrsI, locNbrDistsI, 
				CB, imgIndex, patIndex, nbrIndex);
		}

		for(int y=nImgHeight-1, x=0; x<nImgWidth-1; x++)
		{	
			// RC

			int i = imgIndex.Calc(x, y);

			int * featsI = &m_pats[ patIndex.Calc(0, i) ];
			int * locNbrsI = 
				&m_nbrs[ nbrIndex.Calc(0, i) ];
			int * locNbrDistsI = 
				&m_nbrDists[ nbrIndex.Calc(0, i) ];


			AssignDirDist(x, y, i, featsI, locNbrsI, locNbrDistsI, 
				RC, imgIndex, patIndex, nbrIndex);
		}

		for(int y=0, x=nImgWidth-1; y<nImgHeight-1; y++)
		{
			//	CB, LB

			int i = imgIndex.Calc(x, y);

			int * featsI = &m_pats[ patIndex.Calc(0, i) ];
			int * locNbrsI = 
				&m_nbrs[ nbrIndex.Calc(0, i) ];
			int * locNbrDistsI = 
				&m_nbrDists[ nbrIndex.Calc(0, i) ];

			AssignDirDist(x, y, i, featsI, locNbrsI, locNbrDistsI, 
				CB, imgIndex, patIndex, nbrIndex);

			AssignDirDist(x, y, i, featsI, locNbrsI, locNbrDistsI, 
				LB, imgIndex, patIndex, nbrIndex);
		}



	}


	void ImageClusteringMgr::AssignDirDist(int a_orgX, int a_orgY, int a_i, 
		int * a_featsI, int * a_locNbrsI, int * a_locNbrDistsI,
		NbrPixDir a_dir, IndexCalc2D & a_rImgIndex, 
		IndexCalc2D & a_rPatIndex, IndexCalc2D & a_rNbrIndex)
	{

		const int nDir = (int)a_dir;

		int j = GetNbrPixIndex(a_orgX, a_orgY, a_dir, a_rImgIndex);

		a_locNbrsI[ nDir ] = j;
		m_nbrs[ a_rNbrIndex.Calc(nDir + 4, j) ] = a_i;

		int * featsJ = &m_pats[ a_rPatIndex.Calc(0, j) ];

		int nDist = this->CalcDist(a_featsI, featsJ);

		a_locNbrDistsI[ nDir ] = nDist;
		m_nbrDists[ a_rNbrIndex.Calc( nDir + 4, j ) ] = nDist;
	}



	S16ImageRef ImageClusteringMgr::GenResultImage()
	{
		S16ImageRef ret;

		ret = S16Image::Create( m_img->GetSize(), 
			m_img->GetNofChannels());

		ret->SetAll(0);

		S16ChannelRef retCh0 = ret->GetAt(0);
		S16ChannelRef retCh1 = ret->GetAt(1);
		S16ChannelRef retCh2 = ret->GetAt(2);

		IndexCalc2D patIndex(m_pPatIndex->GetSizeX(), 
			m_pPatIndex->GetSizeY());

		const int nDataSiz = patIndex.GetSizeY();

		for(int i=0; i<nDataSiz; i++)
		{
			//int * featsI = &m_pats[ patIndex.Calc(0, i) ];
			int * featsI = &m_resPats[ patIndex.Calc(0, i) ];

			int x = featsI[0];
			int y = featsI[1];

			retCh0->SetAt(x, y, featsI[2]);
			retCh1->SetAt(x, y, featsI[3]);
			retCh2->SetAt(x, y, featsI[4]);
		}

		return ret;
	}



	S16ImageRef ImageClusteringMgr::GenRgnImage()
	{
		S16ImageRef ret;

		ret = S16Image::Create( m_img->GetSize(), 
			m_img->GetNofChannels());

		S16ChannelRef retCh0 = ret->GetAt(0);
		S16ChannelRef retCh1 = ret->GetAt(1);
		S16ChannelRef retCh2 = ret->GetAt(2);

		IndexCalc2D patIndex(m_pPatIndex->GetSizeX(), 
			m_pPatIndex->GetSizeY());

		const int nDataSiz = patIndex.GetSizeY();
		const int nofFeatures = patIndex.GetSizeX();

		IndexCalc2D clustSumIndex( 
			nofFeatures, m_clusterInfoArr.size() );

		std::vector<int> clustSums( clustSumIndex.GetTotalSize() );
		memset( &clustSums[0], 0, clustSums.size() * sizeof(int) );

		for(int i=0; i<nDataSiz; i++)
		{
			const int nID = m_patIDs[i];
			const int nClustInfID = m_patClusterInfoIDs[ nID ];

			int * featsSum = &clustSums[ clustSumIndex.Calc(0, nClustInfID) ];

			int * featsI = &m_pats[ patIndex.Calc(0, i) ];

			for(int f=0; f<nofFeatures; f++)
			{
				featsSum[f] += featsI[f];
			}
		}

		std::vector<int> clustMeans( clustSumIndex.GetTotalSize() );

		for(int i=0; i<m_clusterInfoArr.size(); i++)
		{
			int * featsSum = &clustSums[ clustSumIndex.Calc(0, i) ];

			int * featsMeanI = &clustMeans[ clustSumIndex.Calc(0, i) ];

			for(int f=0; f<nofFeatures; f++)
			{
				//featsMeanI[f] = featsSum[f] / m_clusterInfoArr[i].size;
				featsMeanI[f] = rand() % 256;
			}
		}

		CvSize imgSiz = m_img->GetSize();
		IndexCalc2D imgIndex(imgSiz.width, imgSiz.height);

		//for(int i=0; i<nDataSiz; i++)
		for(int y=0; y<imgSiz.height; y++)
		{
			for(int x=0; x<imgSiz.width; x++)
			{
				int i = imgIndex.Calc(x, y);

				int * featsI = &m_pats[ patIndex.Calc(0, i) ];
				//int * featsI = &m_resPats[ patIndex.Calc(0, i) ];

				const int nID = m_patIDs[i];
				const int nClustInfID = m_patClusterInfoIDs[ nID ];

				int * featsMeanI = &clustMeans[ clustSumIndex.Calc(0, nClustInfID) ];


				//int x = featsI[0];
				//int y = featsI[1];

				//retCh0->SetAt(x, y, featsMeanI[2]);
				//retCh1->SetAt(x, y, featsMeanI[3]);
				//retCh2->SetAt(x, y, featsMeanI[4]);

				retCh0->SetAt(x, y, featsMeanI[0]);
				retCh1->SetAt(x, y, featsMeanI[1]);
				retCh2->SetAt(x, y, featsMeanI[2]);
			}
		}

		return ret;
	}



	S16ImageRef ImageClusteringMgr::GenMaxRgnImage()
	{
		S16ImageRef ret;

		ret = S16Image::Create( m_img->GetSize(), 
			m_img->GetNofChannels());

		S16ChannelRef retCh0 = ret->GetAt(0);
		S16ChannelRef retCh1 = ret->GetAt(1);
		S16ChannelRef retCh2 = ret->GetAt(2);

		IndexCalc2D patIndex(m_pPatIndex->GetSizeX(), 
			m_pPatIndex->GetSizeY());

		const int nDataSiz = patIndex.GetSizeY();
		const int nofFeatures = patIndex.GetSizeX();

		IndexCalc2D clustSumIndex( 
			nofFeatures, m_clusterInfoArr.size() );

		std::vector<int> clustSums( clustSumIndex.GetTotalSize() );
		memset( &clustSums[0], 0, clustSums.size() * sizeof(int) );

		for(int i=0; i<nDataSiz; i++)
		{
			const int nID = m_patIDs[i];
			const int nClustInfID = m_patClusterInfoIDs[ nID ];

			int * featsSum = &clustSums[ clustSumIndex.Calc(0, nClustInfID) ];

			int * featsI = &m_pats[ patIndex.Calc(0, i) ];

			for(int f=0; f<nofFeatures; f++)
			{
				featsSum[f] += featsI[f];
			}
		}

		std::vector<int> clustMeans( clustSumIndex.GetTotalSize() );

		for(int i=0; i<m_clusterInfoArr.size(); i++)
		{
			int * featsSum = &clustSums[ clustSumIndex.Calc(0, i) ];

			int * featsMeanI = &clustMeans[ clustSumIndex.Calc(0, i) ];

			for(int f=0; f<nofFeatures; f++)
			{
				featsMeanI[f] += featsSum[f] / m_clusterInfoArr[i].size;
			}
		}

		int nMaxIndex = 0;
		for(int i=0; i<m_clusterInfoArr.size(); i++)
		{
			if( m_clusterInfoArr[i].size > 
				m_clusterInfoArr[nMaxIndex].size )
			{
				nMaxIndex = i;
			}
		}

		ret->SetAll(0);

		for(int i=0; i<nDataSiz; i++)
		{
			int * featsI = &m_pats[ patIndex.Calc(0, i) ];
			//int * featsI = &m_resPats[ patIndex.Calc(0, i) ];

			const int nID = m_patIDs[i];
			const int nClustInfID = m_patClusterInfoIDs[ nID ];

			if(nClustInfID != nMaxIndex)
				continue;

			int * featsMeanI = &clustMeans[ clustSumIndex.Calc(0, nClustInfID) ];


			int x = featsI[0];
			int y = featsI[1];

			//retCh0->SetAt(x, y, featsMeanI[2]);
			//retCh1->SetAt(x, y, featsMeanI[3]);
			//retCh2->SetAt(x, y, featsMeanI[4]);

			retCh0->SetAt(x, y, featsI[2]);
			retCh1->SetAt(x, y, featsI[3]);
			retCh2->SetAt(x, y, featsI[4]);
		}

		return ret;
	}



}