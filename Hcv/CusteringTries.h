#pragma once


namespace Hcv
{
	using namespace Hcpl;

	void DoClusteringStep(std::vector<CvPoint> * a_pDataPts,
		std::vector<CvPoint> * a_pResDataPts,
		std::vector<int> * a_pDataPtCnts = NULL);

	void DoClusteringStep2(std::vector<CvPoint> * a_pDataPts,
		std::vector<CvPoint> * a_pResDataPts,
		std::vector<int> * a_pDataPtCnts = NULL);

	void DoClusteringStep3(std::vector<CvPoint> * a_pDataPts,
		std::vector<CvPoint> * a_pResDataPts,
		std::vector<int> * a_pDataPtCnts = NULL);

	void DoClusteringStep4(std::vector<CvPoint> * a_pDataPts,
		std::vector<CvPoint> * a_pResDataPts,
		std::vector<int> * a_pDataPtCnts = NULL);

//	void DoClusteringStep5(std::vector<CvPoint> * a_pDataPts,
//		std::vector<CvPoint> * a_pResDataPts,
//		std::vector<int> * a_pDataPtCnts = NULL);

	void DoClusteringStep6(std::vector<CvPoint> * a_pDataPts,
		std::vector<CvPoint> * a_pResDataPts,
		std::vector<int> * a_pDataPtCnts = NULL);

	void DoClusteringStep7(std::vector<CvPoint> * a_pDataPts,
		std::vector<CvPoint> * a_pResDataPts,
		std::vector<int> * a_pDataPtCnts = NULL);

	void DoClusteringStep8(std::vector<CvPoint> * a_pDataPts,
		std::vector<CvPoint> * a_pResDataPts,
		std::vector<int> * a_pDataPtCnts = NULL);

	void DoClusteringStep9(std::vector<CvPoint> * a_pDataPts,
		std::vector<CvPoint> * a_pResDataPts,
		std::vector<int> * a_pDataPtCnts = NULL);

	void DoClusteringStep10(std::vector<CvPoint> * a_pDataPts,
		std::vector<CvPoint> * a_pResDataPts, 
		S16ImageRef a_orgImg, S16ImageRef a_resImg,
		std::vector<int> * a_pDataPtCnts = NULL);

	void DoClusteringStep11(std::vector<CvPoint> * a_pDataPts,
		std::vector<CvPoint> * a_pResDataPts, 
		S16ImageRef a_orgImg, S16ImageRef a_resImg,
		std::vector<int> * a_pDataPtCnts = NULL);

	void DoClusteringStep12(std::vector<CvPoint> * a_pDataPts,
		std::vector<CvPoint> * a_pResDataPts, 
		S16ImageRef a_orgImg, S16ImageRef a_resImg,
		std::vector<int> * a_pDataPtCnts = NULL);


	template<class T, int nofFeatures>
	class Pattern
	{
	public:

		T Features[ nofFeatures ];
	};




	void DoClusteringStep13(std::vector<CvPoint> * a_pDataPts,
		std::vector<CvPoint> * a_pResDataPts, 
		S16ImageRef a_orgImg, S16ImageRef a_resImg,
		std::vector< Pattern< int, 2 > > * a_pPatArr,
		std::vector<int> * a_pDataPtCnts = NULL);

	
	void DoClusteringStep14(std::vector<CvPoint> * a_pDataPts,
		std::vector<CvPoint> * a_pResDataPts, 
		S16ImageRef a_orgImg, S16ImageRef a_resImg,
		std::vector< Pattern< int, 2 > > * a_pPatArr,
		std::vector<int> * a_pDataPtCnts = NULL);

	
	void DoClusteringStep15(std::vector<CvPoint> * a_pDataPts,
		std::vector<CvPoint> * a_pResDataPts, 
		S16ImageRef a_orgImg, S16ImageRef a_resImg,
		std::vector< Pattern< int, 2 > > * a_pPatArr,
		std::vector<int> * a_pDataPtCnts = NULL);

	void DoClusteringStep16(std::vector<CvPoint> * a_pDataPts,
		std::vector<CvPoint> * a_pResDataPts, 
		S16ImageRef a_orgImg, S16ImageRef a_resImg,
		std::vector< Pattern< int, 2 > > * a_pPatArr,
		std::vector<int> * a_pDataPtCnts = NULL);


}