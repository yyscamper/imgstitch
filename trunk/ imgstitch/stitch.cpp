
#include "stitch.h"
#include "util.h"
#include "match.h"
#include "highgui.h"
#include "ui.h"
inline bool Interpolate(double x, double y, IplImage* image, double* interVal)
{
	if( x<=0 || y<=0 || x >= image->width-1 || y >= image->height-1){
		return false;
	}
	double h1 = x - cvFloor(x);
	double h2 = cvCeil(x) - x;
	double v2 = y - cvFloor(y);
	double v1 = cvCeil(y) - y;

	double f12, f22, f11, f21;
	for(int c=0; c < image->nChannels; c++){
		f12 = GetPixelValF32(image, cvFloor(x), cvFloor(y), c);
		f22 = GetPixelValF32(image, cvCeil(x), cvFloor(y), c);
		f11 = GetPixelValF32(image, cvFloor(x), cvCeil(y), c);
		f21 = GetPixelValF32(image, cvCeil(x), cvCeil(y), c);
		interVal[c] = (h1*v1*f22 + h1*v2*f21 + h2*v1*f12 + h2*v2*f11)/((h1+h2)*(v1+v2));
	}
	return true;
}

IplImage* StitchTwoImages(IplImage* image1, IplImage* image2, CvMat* mapMat)
{
	IplImage *srcImage1, *srcImage2, *dstImage,*flagImage;
	CvMat *fromMat, *toMat, *invMat;
	CvPoint offset1RefDst, offset2RefImg1,  mapPoint;
	CvSize dstImageSize;

	assert(image1->depth == image2->depth && image1->nChannels == image2->nChannels);
	
	//将输入的图像转换为浮点图像
	srcImage1 = GetFloatImage(image1);
	srcImage2 = GetFloatImage(image2);
	
	//找到image2中四个顶点的映射值，以确定拼接后图像的大小
	CvPoint mapVertex[4]; //存储image2中四个顶点的映射点
	GetMapVal(cvPoint(0, 0), mapVertex, mapMat);
	GetMapVal(cvPoint(0, image2->height - 1), mapVertex + 1, mapMat);
	GetMapVal(cvPoint(image2->width - 1, 0), mapVertex + 2, mapMat);
	GetMapVal(cvPoint(image2->width - 1, image2->height - 1), mapVertex + 3, mapMat);
	int minMapX, maxMapX, minMapY, maxMapY;
	minMapX = MIN(MIN(MIN(mapVertex[0].x, mapVertex[1].x), mapVertex[2].x),mapVertex[3].x);
	maxMapX = MAX(MAX(MAX(mapVertex[0].x, mapVertex[1].x), mapVertex[2].x),mapVertex[3].x);
	minMapY = MIN(MIN(MIN(mapVertex[0].y, mapVertex[1].y), mapVertex[2].y),mapVertex[3].y);
	maxMapY = MAX(MAX(MAX(mapVertex[0].y, mapVertex[1].y), mapVertex[2].y),mapVertex[3].y);
	
	//设定两幅图像在拼接后的图像中的坐标偏移量以及接近图像的大小
	offset2RefImg1.x = minMapX; //相对于图像1，图像1的偏移量
	offset2RefImg1.y = minMapY;
	//寻找目标图像中四个角点的坐标
	int minXinDst = min(0, minMapX);
	int maxXinDst = max(srcImage1->width-1, maxMapX);
	int minYinDst = min(0, minMapY);
	int maxYinDst = max(srcImage1->height-1, maxMapY);
	dstImageSize.width = maxXinDst - minXinDst + 1; //目标图像的大小
	dstImageSize.height = maxYinDst - minYinDst + 1;
	offset1RefDst.x = 0 - minXinDst; //相对于目标图像，图像1的偏移量
	offset1RefDst.y = 0 - minYinDst;

	//分配目标图像的空间，将图像1复制到目标图像中
	dstImage = cvCreateImage(dstImageSize,srcImage1->depth, srcImage1->nChannels);
	cvSetImageROI(dstImage, cvRect(offset1RefDst.x, offset1RefDst.y, srcImage1->width, srcImage1->height));
	cvCopyImage(srcImage1, dstImage);
	cvResetImageROI(dstImage);

	//分配映射运算中变量的存储空间
	fromMat = cvCreateMat(1, 3, CV_32FC1);
	toMat = cvCreateMat(1, 2, CV_32FC1);
	invMat = GetInvMapMat(mapMat);
	flagImage = cvCreateImage(cvSize(maxMapX - minMapX, maxMapY - minMapY), IPL_DEPTH_8U, 1);
	cvSetZero(flagImage);

	//int inX, inY;
	//while(1){
	//	printf("\nPlease input the X and Y:");
	//	scanf("%d%d", &inX, &inY);
	//	cvmSet(fromMat, 0, 0, inX);
	//	cvmSet(fromMat, 0, 1, inY);
	//	cvmSet(fromMat, 0, 2, 1);
	//	cvGEMM(fromMat, invMat, 1, NULL, 0, toMat, 0);
	//	mappedPoint.x = cvRound(cvmGet(toMat, 0, 0));// + offsetPoint.x;
	//	mappedPoint.y = cvRound(cvmGet(toMat, 0, 1));// + offsetPoint.y;

	//	printf("\n(%d, %d) ------->(%d, %d)\n", inX, inY, mappedPoint.x, mappedPoint.y);
	//}
	//

	/*bool *pFlagMat = (bool*)calloc(widthOfFlagMat*heightOfFlagMat, sizeof(bool));
	assert(pFlagMat != NULL);
	for(int x = 0; x < widthOfFlagMat; x++){
		for(int y = 0; y < heightOfFlagMat; y++){
			pFlagMat[y * widthOfFlagMat + x] = false;
		}
	}*/

	CvPoint mapPtr2;
	for(int x = 0; x < srcImage2->width; x++){
		for(int y = 0; y < srcImage2->height; y++){
			//求映射后的坐标值，同函数GetMapVal
			cvmSet(fromMat, 0, 0, x);
			cvmSet(fromMat, 0, 1, y);
			cvmSet(fromMat, 0, 2, 1);
			cvGEMM(fromMat, mapMat, 1, NULL, 0, toMat, 0);
			mapPoint.x = cvRound(cvmGet(toMat, 0, 0));
			mapPoint.y = cvRound(cvmGet(toMat, 0, 1));
			mapPtr2.x = mapPoint.x - minMapX;
			mapPtr2.y = mapPoint.y - minMapY;
			
			//pFlagMat[mapPtr2.y * widthOfFlagMat + mapPtr2.x] = true;
			if(mapPtr2.x >= 0 && mapPtr2.x < flagImage->width
				&& mapPtr2.y >= 0 && mapPtr2.y < flagImage->height){
					((unsigned char*)(flagImage->imageData + mapPtr2.y
						*flagImage->widthStep))[mapPtr2.x] = 255;
			
			}

			mapPoint.x += offset1RefDst.x;
			mapPoint.y += offset1RefDst.y;
			//映射点处于Image1所在的区域时，跳过该点
			if(mapPoint.x >= offset1RefDst.x && mapPoint.x < image1->width + offset1RefDst.x 
				&& mapPoint.y >= offset1RefDst.y && mapPoint.y < offset1RefDst.y + image1->height){
				//|| mapPoint.x > dstImage->width || mapPoint.y > dstImage->height){
				continue;
			}
			CopyPixelValF32(dstImage, mapPoint.x, mapPoint.y, srcImage2, x, y);	
		}
	}
	
	double *interVal = (double*)calloc(image2->nChannels, sizeof(double));
	for(int x = 0; x < flagImage->width; x++){
		for(int y = 0; y < flagImage->height; y++){
			//绘制点处于Image1所在的区域时，跳过该点
			mapPoint.x = x +  offset2RefImg1.x;
			mapPoint.y = y +  offset2RefImg1.y;
			if(mapPoint.x >= 0 && mapPoint.x < srcImage1->width
				&& mapPoint.y >= 0 && mapPoint.y < srcImage1->height){
					continue;
			}
			mapPoint.x += offset1RefDst.x;
			mapPoint.y += offset1RefDst.y;

			if( ((unsigned char*)(flagImage->imageData + y
				*flagImage->widthStep))[x] == 0 ){
				cvmSet(fromMat, 0, 0, x + offset2RefImg1.x);
				cvmSet(fromMat, 0, 1, y + offset2RefImg1.y);
				cvmSet(fromMat, 0, 2, 1);
				cvGEMM(fromMat, invMat, 1, NULL, 0, toMat, 0);
				if(!Interpolate(cvmGet(toMat, 0, 0), cvmGet(toMat, 0, 1), srcImage2, interVal)){
					continue;
				}
				for(int c=0; c < srcImage2->nChannels; c++){
					SetPixelValF32(dstImage, mapPoint.x, mapPoint.y, c, interVal[c]);
				}	
			}
		}
	}
	cvReleaseImage(&srcImage1);
	cvReleaseImage(&srcImage2);
	cvReleaseImage(&flagImage);
	cvReleaseMat(&fromMat);
	cvReleaseMat(&toMat);
	cvReleaseMat(&invMat);
	return dstImage;
}
//
//IplImage* StitchImages(IplImage** srcImages, int numOfImages, CvMat** mapMats)
//{
//	int numOfIters = numOfImages - 1;
//	CvPoint* offsetRefDstArr = (CvPoint*)calloc(numOfIters, sizeof(CvPoint));
//	CvPoint* offsetRefFirstArr = (CvPoint*)calloc(numOfIters, sizeof(CvPoint));
//	CvRect* mapRectArr = (CvRect*)calloc(numOfIters, sizeof(CvRect));
//	CvMat** invMatArr = (CvMat**)calloc(numOfIters, sizeof(CvMat*));
//	CvMat** cascadeMatArr = (CvMat**)calloc(numOfIters, sizeof(CvMat*));
//	
//	IplImage *floatImage1, *floatImage2, *dstImage,*flagImage, *dstFlagImage;
//	CvMat *fromMat, *toMat, *invMat;
//	CvPoint offset1RefDst,  mapPoint;
//	CvSize dstImageSize;
//
//	CvPoint mapVertex[4]; //存储四个顶点的映射点
//	int minMapX, maxMapX, minMapY, maxMapY;
//	CvMat* cascadeMat = NULL;
//	int minXinDst = 0;
//	int maxXinDst = srcImages[0]->width - 1;
//	int minYinDst = 0;
//	int maxYinDst = srcImages[0]->height - 1;
//
//	for(int i=0; i<numOfImages-1; i++){
//		cascadeMatArr[i] = GetCascadeMapMat(mapMats, i+1); 
//		GetMapVal(cvPoint(0, 0), mapVertex, cascadeMatArr[i]);
//		GetMapVal(cvPoint(0, srcImages[i+1]->height - 1), mapVertex + 1, cascadeMatArr[i]);
//		GetMapVal(cvPoint(srcImages[i+1]->width - 1, 0), mapVertex + 2, cascadeMatArr[i]);
//		GetMapVal(cvPoint(srcImages[i+1]->width - 1, srcImages[i+1]->height - 1), mapVertex + 3, cascadeMatArr[i]);
//		
//		minMapX = MIN(MIN(MIN(mapVertex[0].x, mapVertex[1].x), mapVertex[2].x),mapVertex[3].x);
//		maxMapX = MAX(MAX(MAX(mapVertex[0].x, mapVertex[1].x), mapVertex[2].x),mapVertex[3].x);
//		minMapY = MIN(MIN(MIN(mapVertex[0].y, mapVertex[1].y), mapVertex[2].y),mapVertex[3].y);
//		maxMapY = MAX(MAX(MAX(mapVertex[0].y, mapVertex[1].y), mapVertex[2].y),mapVertex[3].y);
//
//		//设定两幅图像在拼接后的图像中的坐标偏移量以及接近图像的大小
//		mapRectArr[i].x = minMapX;
//		mapRectArr[i].y = minMapY;
//		mapRectArr[i].width = maxMapX - minMapX;
//		mapRectArr[i].height = maxMapY - minMapY
//		//寻找目标图像中四个角点的坐标
//		minXinDst = min(minXinDst, minMapX);
//		maxXinDst = max(maxXinDst, maxMapX);
//		minYinDst = min(minYinDst, minMapY);
//		maxYinDst = max(maxYinDst, maxMapY);
//	}
//	dstImageSize.width = maxXinDst - minXinDst + 1; //目标图像的大小
//	dstImageSize.height = maxYinDst - minYinDst + 1;
//	offset1RefDst.x = 0 - minXinDst; //相对于目标图像，图像1的偏移量
//	offset1RefDst.y = 0 - minYinDst;
//	
//	//分配目标图像的空间，将图像1复制到目标图像中
//	dstImage = cvCreateImage(dstImageSize, IPL_DEPTH_32F, 3);
//	dstFlagImage = cvCreateImage(dstImageSize, IPL_DEPTH_8U, 1);
//	cvSetZero(dstFlagImage);
//	cvSetImageROI(dstImage, cvRect(offset1RefDst.x, offset1RefDst.y, srcImages[0].width, srcImages[0].height));
//	cvCopyImage(srcImages[0], dstImage);
//	cvSetImageROI(dstFlagImage, cvGetImageROI(dstImage));
//	cvSet(dstFlagImage, CvScalar(255, 0, 0), NULL);
//	cvResetImageROI(dstImage);
//
//	//分配映射运算中变量的存储空间
//	fromMat = cvCreateMat(1, 3, CV_32FC1);
//	toMat = cvCreateMat(1, 2, CV_32FC1);
//	
//	//将输入的图像转换为浮点图像
//	IplImage* fstFloatImage = GetFloatImage(srcImages[0]);
//	IplImage* secFloatImage = NULL;
//	CvPoint mapPtr2;
//	double interVal[3];
//	for(int i=0; i<numOfIters; i++){
//		secFloatImage = GetFloatImage(srcImages[i+1]);
//		invMat = GetInvMapMat(cascadeMatArr[i]);
//		flagImage = cvCreateImage(cvSize(mapRectArr[i].width, mapRectArr[i].height), IPL_DEPTH_8U, 1);
//		cvSetZero(flagImage);
//
//		for(int x = 0; x < secFloatImage->width; x++){
//			for(int y = 0; y < secFloatImage->height; y++){
//				//求映射后的坐标值，同函数GetMapVal
//				cvmSet(fromMat, 0, 0, x);
//				cvmSet(fromMat, 0, 1, y);
//				cvmSet(fromMat, 0, 2, 1);
//				cvGEMM(fromMat, cascadeMat[i], 1, NULL, 0, toMat, 0);
//				mapPoint.x = cvRound(cvmGet(toMat, 0, 0));
//				mapPoint.y = cvRound(cvmGet(toMat, 0, 1));
//				mapPtr2.x = mapPoint.x - minMapX;
//				mapPtr2.y = mapPoint.y - minMapY;
//
//				if(mapPtr2.x >= 0 && mapPtr2.x < flagImage->width
//					&& mapPtr2.y >= 0 && mapPtr2.y < flagImage->height){
//						((unsigned char*)(flagImage->imageData + mapPtr2.y
//							*flagImage->widthStep))[mapPtr2.x] = 255;
//
//				}
//				mapPoint.x += offset1RefDst.x;
//				mapPoint.y += offset1RefDst.y;
//
//				if(((unsigned char*)(dstFlagImage->imageData + mapPoint.y
//					*dstFlagImage->widthStep))[mapPoint.x] == 0){
//					CopyPixelValF32(dstImage, mapPoint.x, mapPoint.y, floatImage2, x, y);
//					((unsigned char*)(dstFlagImage->imageData + mapPoint.y
//						*dstFlagImage->widthStep))[mapPoint.x] = 255;
//				}		
//			}
//		}
//
//		for(int x = 0; x < mapRectArr[i]->width; x++){
//			for(int y = 0; y < mapRectArr[i]->height; y++){
//				//绘制点处于Image1所在的区域时，跳过该点
//				mapPoint.x = x +  mapRectArr[i].x + offset1RefDst.x;
//				mapPoint.y = y +  mapRectArr[i].y + offset1RefDst.y;
//				if(((unsigned char*)(dstFlagImage->imageData + mapPoint.y
//					*dstFlagImage->widthStep))[mapPoint.x] != 0){
//						continue;
//				}
//				invMat = GetInvMapMat(cascadeMat[i]);
//				cvmSet(fromMat, 0, 0, x + mapRectArr[i].x);
//				cvmSet(fromMat, 0, 1, y + mapRectArr[i].y);
//				cvmSet(fromMat, 0, 2, 1);
//				cvGEMM(fromMat, invMat, 1, NULL, 0, toMat, 0);
//				if(!Interpolate(cvmGet(toMat, 0, 0), cvmGet(toMat, 0, 1), floatImage2, interVal)){
//					continue;
//				}
//				for(int c=0; c < floatImage2->nChannels; c++){
//					SetPixelValF32(dstImage, mapPoint.x, mapPoint.y, c, interVal[c]);
//				}	
//				}
//			}
//		}
//
//	}
//
//
//	
//	
//	
//	cvReleaseImage(&srcImage1);
//	cvReleaseImage(&floatImage2);
//	cvReleaseImage(&flagImage);
//	cvReleaseMat(&fromMat);
//	cvReleaseMat(&toMat);
//	cvReleaseMat(&invMat);
//	return dstImage;
//}

IplImage* AutoStitchTwoImages(IplImage* srcImage1, IplImage* srcImage2)
{
	CvPoint *pPoint1, *pPoint2;
	bool swapFlag = false;
	IplImage *pTempImage;
	CvPoint *pTempPoint;
	CvMat* mapMat = NULL; 
	int numOfPoints = 0;

	numOfPoints = GetMatchPoints(srcImage1, srcImage2, &pPoint1, &pPoint2, &swapFlag);
	swapFlag = false;
	if(swapFlag){
		//swap the points
		pTempPoint = pPoint1;
		pPoint1 = pPoint2;
		pPoint2 = pTempPoint;
		//swap the images
		pTempImage = srcImage1;
		srcImage1 = srcImage2;
		srcImage2 = pTempImage;
	}

	int selIndex[6];
	mapMat = Ransac(pPoint2, pPoint1, numOfPoints, selIndex);	
	printf("\n映射矩阵为:");
	PrintMat(mapMat);
	CvPoint pt1[6], pt2[6];
	for(int i=0; i<6; i++){
		pt1[i].x = pPoint1[i].x;
		pt1[i].y = pPoint1[i].y;
		pt2[i].x = pPoint2[i].x;
		pt2[i].y = pPoint2[i].y;
	}

	IplImage* pMatchImage = PlotMatchImage(srcImage1, srcImage2, pPoint1, pPoint2, numOfPoints);
	cvNamedWindow("Match Image");
	cvShowImage("Match Image", pMatchImage);
	//cvWaitKey(0);
	//cvDestroyWindow("Match Image");
	//cvReleaseImage(&pMatchImage);

	IplImage* pSelMatchImage = PlotMatchImage(srcImage1, srcImage2, pt1, pt2, 6);
	cvNamedWindow("Selected Match Image");
	cvShowImage("Selected Match Image", pSelMatchImage);
	//cvWaitKey(0);
	//cvDestroyWindow("Match Image");
	//cvReleaseImage(&pSelMatchImage);
	return StitchTwoImages(srcImage1, srcImage2, mapMat);
}