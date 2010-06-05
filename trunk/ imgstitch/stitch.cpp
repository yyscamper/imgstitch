
#include "stitch.h"
#include "util.h"

IplImage* StitchImages(IplImage* image1, IplImage* image2, CvMat* mapMat)
{
	assert(image1->depth == image2->depth && image1->nChannels == image2->nChannels);
	IplImage* srcImage1 = GetFloatImage(image1);
	IplImage* srcImage2 = GetFloatImage(image2);
	IplImage* dstImage = cvCreateImage(cvSize(srcImage1->width + srcImage2->width, srcImage1->height + srcImage2->height),
		srcImage1->depth, srcImage1->nChannels);
	CvPoint offsetPoint, mappedPoint;
	offsetPoint.x = 0;
	offsetPoint.y = (dstImage->height - srcImage1->height)/2;
	cvSetImageROI(dstImage, cvRect(offsetPoint.x, offsetPoint.y, srcImage1->width, srcImage1->height));
	cvCopyImage(srcImage1, dstImage);
	cvResetImageROI(dstImage);
	
	CvMat* coorMat1 = cvCreateMat(2, 1, CV_32FC1);
	CvMat* coorMat2 = cvCreateMat(2, 1, CV_32FC1);
	CvMat* mapSiftMat = cvCreateMat(2, 1, CV_32FC1);
	CvMat* mapScaleRotMat = cvCreateMat(2, 2, CV_32FC1);
	cvmSet(mapScaleRotMat, 0, 0, cvmGet(mapMat, 0, 0));
	cvmSet(mapScaleRotMat, 0, 1, cvmGet(mapMat, 0, 1));
	cvmSet(mapScaleRotMat, 1, 0, cvmGet(mapMat, 1, 0));
	cvmSet(mapScaleRotMat, 1, 1, cvmGet(mapMat, 1, 1));
	cvmSet(mapSiftMat, 0, 0, cvmGet(mapMat, 2, 0));
	cvmSet(mapSiftMat, 1, 0, cvmGet(mapMat, 2, 1));

	for(int x = 0; x < srcImage2->width; x++){
		for(int y = 0; y < srcImage2->height; y++){
			cvmSet(coorMat1, 0, 0, x);
			cvmSet(coorMat1, 1, 0, y);
			cvGEMM(mapScaleRotMat, coorMat1, 1, mapSiftMat, 1, coorMat2, 0);
			//printf("\n(%d, %d) ----> (%d, %d)", x, y, cvRound(cvmGet(coorMat2, 0, 0)), cvRound(cvmGet(coorMat2, 1, 0)));
			mappedPoint.x = cvRound(cvmGet(coorMat2, 0, 0)) + offsetPoint.x;
			mappedPoint.y = cvRound(cvmGet(coorMat2, 1, 0)) + offsetPoint.y;
			if(mappedPoint.x >= 0 && mappedPoint.x < image1->width //绘制点处于Image1所在的区域时，跳过该点
				&& mappedPoint.y >= offsetPoint.y && mappedPoint.y < offsetPoint.y + image1->height
				|| mappedPoint.x > dstImage->width || mappedPoint.y > dstImage->height){
				continue;
			}
			//printf("\n(%d, %d) ----> (%d, %d)", x, y, mappedPoint.x, mappedPoint.y);
			CopyPixelVal(dstImage, mappedPoint.x, mappedPoint.y, srcImage2, x, y);
		}
	}

	cvReleaseImage(&srcImage1);
	cvReleaseImage(&srcImage2);
	cvReleaseMat(&coorMat1);
	cvReleaseMat(&coorMat2);
	cvReleaseMat(&mapSiftMat);
	cvReleaseMat(&mapScaleRotMat);
	return dstImage;
}