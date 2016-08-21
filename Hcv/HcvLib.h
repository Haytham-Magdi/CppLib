#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\killers.h>
#include <Lib\Hcv\error.h>
#include <Lib\Hcv\PixelTypes.h>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>
#include <Lib\Hcv\ConvKernel.h>
#include <Lib\Hcv\Hsv.h>
#include <Lib\Hcv\HistoGram.h>
#include <Lib\Hcv\InitLib.h>
#include <Lib\Hcv\RegionSegmentor.h>
#include <Lib\Hcv\RegionSegmentor2.h>
#include <Lib\Hcv\RegionSegmentor3.h>
#include <Lib\Hcv\RegionSegmentor4.h>
#include <Lib\Hcv\RegionSegmentor5.h>
#include <Lib\Hcv\RegionSegmentor6.h>
#include <Lib\Hcv\RegionSegmentor7.h>
#include <Lib\Hcv\RegionSegmentor8.h>
#include <Lib\Hcv\RegionSegmentor9.h>
#include <Lib\Hcv\RegionSegmentor10.h>
#include <Lib\Hcv\RegionSegmentor11.h>
#include <Lib\Hcv\RegionSegmentor12.h>
#include <Lib\Hcv\RegionSegmentor13.h>
#include <Lib\Hcv\RegionSegmentor14.h>
#include <Lib\Hcv\RegionSegmentor15.h>
#include <Lib\Hcv\RegionSegmentor16.h>
#include <Lib\Hcv\RegionSegmentor17.h>
#include <Lib\Hcv\RegionSegmentor18.h>
#include <Lib\Hcv\RegionSegmentor19.h>
#include <Lib\Hcv\RegionSegmentor20.h>
#include <Lib\Hcv\RegionSegmentor21.h>
#include <Lib\Hcv\RegionSegmentor22.h>
#include <Lib\Hcv\RegionSegmentor23.h>
#include <Lib\Hcv\RegionSegmentor24.h>
#include <Lib\Hcv\RegionSegmentor25.h>
#include <Lib\Hcv\RegionSegmentor26.h>
#include <Lib\Hcv\RegionSegmentor27.h>
#include <Lib\Hcv\RegionSegmentor28.h>
#include <Lib\Hcv\RegionSegmentor29.h>
#include <Lib\Hcv\RegionSegmentor30.h>
#include <Lib\Hcv\RegionSegmentor31.h>
#include <Lib\Hcv\RegionSegmentor32.h>
#include <Lib\Hcv\RegionSegmentor33.h>
#include <Lib\Hcv\RegionSegmentor34.h>
#include <Lib\Hcv\RegionSegmentor35.h>
#include <Lib\Hcv\RegionSegmentor36.h>
#include <Lib\Hcv\RegionSegmentor37.h>
#include <Lib\Hcv\RegionSegmentor38.h>
#include <Lib\Hcv\RegionSegmentor39.h>
#include <Lib\Hcv\RegionSegmentor40.h>
#include <Lib\Hcv\RegionSegmentor41.h>
#include <Lib\Hcv\RegionSegmentor42.h>
#include <Lib\Hcv\RegionSegmentor43.h>
#include <Lib\Hcv\RegionSegmentor44.h>
#include <Lib\Hcv\RegionSegmentor45.h>
#include <Lib\Hcv\RegionSegmentor46.h>
#include <Lib\Hcv\RegionSegmentor47.h>
#include <Lib\Hcv\RegionSegmentor48.h>
#include <Lib\Hcv\RegionSegmentor49.h>
#include <Lib\Hcv\RegionSegmentor50.h>
#include <Lib\Hcv\RegionSegmentor51.h>
#include <Lib\Hcv\AdaptiveConv.h>
#include <Lib\Hcv\AdaptiveConv2.h>
#include <Lib\Hcv\CusteringTries.h>
#include <Lib\Hcv\ClusteringMgr.h>
#include <Lib\Hcv\ImageClusteringMgr.h>
#include <Lib\Hcv\SignalOneDim.h>
#include <Lib\Hcv\SobelBuilder.h>
#include <Lib\Hcv\PNEdgeBuilder.h>
#include <Lib\Hcv\IOMgr.h>
#include <Lib\Hcv\LinePath.h>
#include <Lib\Hcv\LinePathInt.h>
#include <Lib\Hcv\Signal1D.h>
#include <Lib\Hcv\Signal1DViewer.h>
#include <Lib\Hcv\Filter1D.h>
#include <Lib\Hcv\ConvFilter1D.h>
//#include <Lib\Hcv\LinearColorFilter1D.h>
#include <Lib\Hcv\LinearAvgFilter1D.h>
#include <Lib\Hcv\LinearDiffFilter1D.h>
#include <Lib\Hcv\ImageLineViewer.h>
#include <Lib\Hcv\ColorFilter1D.h>
#include <Lib\Hcv\CompositeColorFilter1D.h>
#include <Lib\Hcv\ColorFilter1DSystem.h>
#include <Lib\Hcv\AbsFilter1D.h>
#include <Lib\Hcv\ScaleFixFilter1D.h>
#include <Lib\Hcv\PeakCleanerFilter1D.h>
#include <Lib\Hcv\PeakMarkerFilter1D.h>
#include <Lib\Hcv\ValleyMaximizerFilter1D.h>
#include <Lib\Hcv\ImageLineViewer2.h>
#include <Lib\Hcv\ImageLineViewer3.h>
#include <Lib\Hcv\SerialColorMgr.h>
#include <Lib\Hcv\ConflictScanner.h>
#include <Lib\Hcv\ConflictScanner2.h>
#include <Lib\Hcv\FiltSysDebug.h>
#include <Lib\Hcv\ValleyMarkerFilter1D.h>
#include <Lib\Hcv\DiagonalLinePath.h>
#include <Lib\Hcv\NegDiagonalLinePath.h>
#include <Lib\Hcv\MaxFilter1D.h>
#include <Lib\Hcv\BadRootScanner.h>
#include <Lib\Hcv\CircleContPath.h>
#include <Lib\Hcv\CircleContourOfPix.h>
#include <Lib\Hcv\CircleContourOfPix2.h>
#include <Lib\Hcv\CircleContourOfPix3.h>
#include <Lib\Hcv\CircleContourOfPix4.h>
#include <Lib\Hcv\CircleContourOfPix5.h>
#include <Lib\Hcv\MultiColorSignal1DViewer.h>
#include <Lib\Hcv\CircDiff.h>
#include <Lib\Hcv\CircDiff2.h>
#include <Lib\Hcv\SlideMgr.h>
#include <Lib\Hcv\LineStepper.h>
#include <Lib\Hcv\LineStepperByAngle.h>
#include <Lib\Hcv\ScanDirMgr.h>
#include <Lib\Hcv\ImgDataMgr_Simple.h>
#include <Lib\Hcv\ImgDataMgr_Simple_2.h>
#include <Lib\Hcv\ImgDataMgr_C7.h>
#include <Lib\Hcv\ImgDataMgr_CovMat.h>
#include <Lib\Hcv\IImgDataMgr.h>
#include <Lib\Hcv\ImgDataElm_Simple.h>
#include <Lib\Hcv\ScanDirMgrColl.h>
#include <Lib\Hcv\SubGradMgr.h>
#include <Lib\Hcv\ImgGradMgr.h>
#include <Lib\Hcv\ImgGradMgr_2.h>
#include <Lib\Hcv\ImgAvgingMgr.h>
#include <Lib\Hcv\Avging_ScanDirMgrExt.h>
#include <Lib\Hcv\DeltaSignal.h>
#include <Lib\Hcv\Filt1D2.h>
#include <Lib\Hcv\SingFilt1D2.h>
#include <Lib\Hcv\Filt1D2DataMgr.h>
#include <Lib\Hcv\AvgFilt1D2.h>
#include <Lib\Hcv\Filt1D2System.h>
#include <Lib\Hcv\Filt1D2SysProto.h>
#include <Lib\Hcv\ScanTryMgr.h>
#include <Lib\Hcv\DifFilt1D2.h>
#include <Lib\Hcv\PeakMarkerFilt1D2.h>
#include <Lib\Hcv\AbsFilt1D2.h>
#include <Lib\Hcv\ZeroCrossingFilt1D2.h>
#include <Lib\Hcv\MaxFilt1D2.h>
#include <Lib\Hcv\EdgeScanMgr.h>
#include <Lib\Hcv\EdgeScan.h>
#include <Lib\Hcv\ScanTryMgr2.h>
#include <Lib\Hcv\ScanTryMgr_3.h>
#include <Lib\Hcv\ScanTryMgr_4.h>
#include <Lib\Hcv\ScanTryMgr_5.h>
#include <Lib\Hcv\ScanTryMgr_6.h>
#include <Lib\Hcv\ScanTryMgr_7.h>
#include <Lib\Hcv\ScanTryMgr_8.h>
#include <Lib\Hcv\ScanTryMgr_9.h>
#include <Lib\Hcv\ScanTryDbg.h>
#include <Lib\Hcv\ZeroCrossing_2_Filt1D2.h>
#include <Lib\Hcv\Ptr_Filt1D2DataMgr.h>
#include <Lib\Hcv\ImageWatershedMgr.h>
#include <Lib\Hcv\IImgPopulation.h>
#include <Lib\Hcv\ImgPopulation_Simple.h>
#include <Lib\Hcv\ImgPopulation_C7.h>
#include <Lib\Hcv\ImgPopulation_CovMat.h>
#include <Lib\Hcv\ImgSegDataMgr.h>
#include <Lib\Hcv\ImgSeg_EvalMgr.h>
#include <Lib\Hcv\ImgSeg_Method_EvalMgr.h>
#include <Lib\Hcv\ImgSeg_GrandMethod_EvalMgr.h>
#include <Lib\Hcv\CBIS_Berkeley_Executer.h>
#include <Lib\Hcv\PeakMarkerFilt1D2_2.h>
#include <Lib\Hcv\RgnSegmDiameter.h>
#include <Lib\Hcv\ColorTryMgr.h>
#include <Lib\Hcv\ImgDataElm_C7.h>
#include <Lib\Hcv\ImgDataElm_C07.h>
#include <Lib\Hcv\ImgDataElm_CovMat.h>
#include <Lib\Hcv\ImgRotationMgr.h>
#include <Lib\Hcv\ImageLineItr.h>
#include <Lib\Hcv\ImageLineItrProvider.h>
#include <Lib\Hcv\ImageItrMgr.h>
#include <Lib\Hcv\ImageRotationMgr_Ex.h>
#include <Lib\Hcv\IImgDataMgr_2.h>
#include <Lib\Hcv\ImgDataMgr_2_CovMat.h>
#include <Lib\Hcv\ImageNbrMgr.h>
#include <Lib\Hcv\IImgDataGradMgr.h>
#include <Lib\Hcv\ImgDataGradMgr_CovMat.h>
#include <Lib\Hcv\IImgDataMgr_2_FactorySet.h>
#include <Lib\Hcv\ImgDataMgr_2_FactorySet_CovMat.h>
//#include <Lib\Hcv\ImageRotationMgr_Friend.h>
#include <Lib\Hcv\ImgBiMixGrad.h>
#include <Lib\Hcv\ImgScanMgr.h>
#include <Lib\Hcv\ImgClusteringTry.h>
#include <Lib\Hcv\ImageAccessor.h>
#include <Lib\Hcv\ImageOperations.h>
#include <Lib\Hcv\ImageAccessorOperations.h>
#include <Lib\Hcv\F32ColorVal_Operations.h>
#include <Lib\Hcv\Element_Operations.h>


#include <Lib\Hcv\LocHist\ImgDataGradMgr_LocHist.h>
#include <Lib\Hcv\LocHist\ImgDataMgr_2_FactorySet_LocHist.h>
#include <Lib\Hcv\LocHist\ImgDataMgr_2_LocHist.h>

#include <Lib\Hcv\GridColorPalette.h>

#include <Lib\Hcv\GridPal\ImgDataGradMgr_GridPal.h>
#include <Lib\Hcv\GridPal\ImgDataMgr_2_FactorySet_GridPal.h>
#include <Lib\Hcv\GridPal\ImgDataMgr_2_GridPal.h>

#include <Lib\Hcv\ImgData_Simple\ImgDataGradMgr_Simple.h>
#include <Lib\Hcv\ImgData_Simple\ImgDataMgr_2_FactorySet_Simple.h>
#include <Lib\Hcv\ImgData_Simple\ImgDataMgr_2_Simple.h>

#include <Lib\Hcv\Saica\ImgAngleDirMgr.h>
#include <Lib\Hcv\Saica\AngleDirMgrColl.h>
#include <Lib\Hcv\Saica\RotationMgrColl.h>
#include <Lib\Hcv\Saica\SaicaProcess1.h>










