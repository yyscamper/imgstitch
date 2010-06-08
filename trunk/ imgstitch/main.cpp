
#include <highgui.h>
#include <stdio.h>
#include <io.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <fstream>

#include "sift.h"
#include "util.h"
#include "match.h"
#include "stitch.h"
#include "ui.h"

using namespace std;

void WriteInfoToFile(CvSeq* seq, IplImage* srcImage, char* path);
void ShowSiftImage(IplImage* srcImage, CvPoint* point, int numOfPoints);
void TestMappingMat();
void TestStitchImage();
void TestSiftFeature();
void TestCopyPixel();
void TestManulSelectMatchPoint();
void TestManulStitch();
void TestSolve();
void TestInvMap();

int main(int argc, char** argv)
{
	//TestCopyPixel();
	//TestStitchImage();
	//TestMappingMat();
	//TestManulSelectMatchPoint();
	TestManulStitch();
	//TestSolve();
	//TestInvMap();


	//ShowMsgWnd("Msg Wnd", "Hello, this is the message!");
	system("pause");
	return 0;
}

void TestStitchImage()
{
	int numOfImages = 2;
	char** imageNames = (char**)calloc(numOfImages, sizeof(char*));
	IplImage** image = (IplImage**)calloc(numOfImages, sizeof(IplImage*));
	CvPoint pt1[4];
	CvPoint pt2[4];
	CvMat* mapMat = NULL;
	IplImage* dstImage = NULL;

	imageNames[0] = "dolphin_a1.jpg";
	imageNames[1] = "dolphin_a2_rot30.jpg";
	
	pt1[0].x = 196; pt2[0].x = 44;
	pt1[0].y = 144; pt2[0].y = 140;
	pt1[1].x = 162; pt2[1].x = 11;
	pt1[1].y = 176; pt2[1].y = 175;
	pt1[2].x = 154; pt2[2].x = 2;
	pt1[2].y = 152; pt2[2].y = 123;
	pt1[3].x = 245; pt2[3].x = 94;
	pt1[3].y = 177; pt2[3].y = 176;

	mapMat = GetMapMat(pt2, pt1, 4);
	double rotAngle = CV_PI/6;
	cvmSet(mapMat, 0, 0, cos(rotAngle));
	cvmSet(mapMat, 0, 1, sin(rotAngle));
	cvmSet(mapMat, 1, 0, -sin(rotAngle));
	cvmSet(mapMat, 1, 1, cos(rotAngle));
	cvmSet(mapMat, 2, 0, 128 * cos(rotAngle));
	cvmSet(mapMat, 2, 1, 128 * cos(rotAngle));
	for(int i=0; i<numOfImages; i++){
		image[i] = cvLoadImage(imageNames[i], CV_LOAD_IMAGE_COLOR);
		if(image[i] == NULL){
			printf("\nError: failed to load image: %s !", imageNames[i]);
			return;
		}
	}
	
	dstImage = StitchTwoImages(image[0], image[1], mapMat);
	cvNamedWindow("Stitch Image");
	cvShowImage("Stitch Image", dstImage);
	cvWaitKey(0);

	cvDestroyAllWindows();
	cvReleaseImage(&dstImage);
	cvReleaseMat(&mapMat);
	for(int i=0; i<numOfImages; i++){
		cvReleaseImage(&image[i]);
	}
	free(image);
	free(imageNames);
}

void TestSiftFeature()
{
	IplImage *srcimage = 0;
	char pathtempin[] = "chair.jpg";
	char outFilePath[] = "air.txt";

	srcimage = cvLoadImage(pathtempin, 0);
	if (srcimage == NULL){
		printf("\nError: failed to load image: %s !", pathtempin);
		exit(-1);
	}

	//创建descriptor存储,求SFIT特征点
	CvMemStorage* descriptorstorage = cvCreateMemStorage(0);
	CvSeq* decriptorseq = cvCreateSeq(0,sizeof(CvSeq),sizeof(CvMat),descriptorstorage);
	SIFT(decriptorseq, srcimage);

	WriteInfoToFile(decriptorseq, srcimage, outFilePath);

	CvPoint* pPointArr = ExtractPos(decriptorseq);
	int numOfPoints = decriptorseq->total;

	ShowSiftImage(srcimage, pPointArr, numOfPoints);

	cvReleaseImage(&srcimage);
	cvReleaseMemStorage(&descriptorstorage);
	free(pPointArr);
}


void ShowSiftImage(IplImage* srcImage, CvPoint* point, int numOfPoints)
{
	IplImage* siftImage = PlotPoints(point, numOfPoints, srcImage, PLOT_TYPE_DOT);
	cvNamedWindow("SIFT Image");
	cvShowImage("SIFT Image", siftImage);
	cvWaitKey(0);
	cvDestroyAllWindows();
	cvReleaseImage(&siftImage);
}

void TestMappingMat()
{
	CvPoint pt1[4];
	CvPoint pt2[4];
	/*pt1[0].x = 1; pt2[0].x = 4;
	pt1[0].y = 2; pt2[0].y = 5;
	pt1[1].x = 3; pt2[1].x = 6;
	pt1[1].y = 4; pt2[1].y = 7;
	pt1[2].x = 5; pt2[2].x = 8;
	pt1[2].y = 6; pt2[2].y = 9;
	pt1[3].x = 7; pt2[3].x = 10;
	pt1[3].y = 8; pt2[3].y = 11;*/

	/*pt1[0].x = 1; pt2[0].x = 4;
	pt1[0].y = 1; pt2[0].y = 4;
	pt1[1].x = 2; pt2[1].x = 5;
	pt1[1].y = 1; pt2[1].y = 4;
	pt1[2].x = 2; pt2[2].x = 5;
	pt1[2].y = 5; pt2[2].y = 8;
	pt1[3].x = 1; pt2[3].x = 4;
	pt1[3].y = 3; pt2[3].y = 6;*/

	//dolphin_1, dolphin_2
	pt1[0].x = 196; pt2[0].x = 44;
	pt1[0].y = 144; pt2[0].y = 140;
	pt1[1].x = 162; pt2[1].x = 11;
	pt1[1].y = 176; pt2[1].y = 175;
	pt1[2].x = 154; pt2[2].x = 2;
	pt1[2].y = 152; pt2[2].y = 123;
	pt1[3].x = 245; pt2[3].x = 94;
	pt1[3].y = 177; pt2[3].y = 176;

	CvMat* mapMat = GetMapMat(pt2, pt1, 4);
	PrintMat(mapMat);
}

void WriteInfoToFile(CvSeq* seq, IplImage* srcImage, char* path)
{
	double temp;
	int i,j;
	int counter = seq->total;
	ofstream outFile(path); 

	outFile<<srcImage->width/2<<" ";
	outFile<<srcImage->height/2<<" ";
	outFile<<"\n";
	CvMat* mat = NULL;
	for(i=0;i<counter;i++)
	{
		mat = (CvMat*)cvGetSeqElem(seq, i);
		for(j=0;j<SIFT_DESCRIPTOR_DIM * SIFT_ARRAY_DIM * SIFT_ARRAY_DIM+3;j++)
		{
			temp = cvmGet(mat,0,j);
			outFile<<temp<<" ";
		}
		outFile<<"\n";
	}
	outFile.close();
}

void TestCopyPixel()
{
	char* imageName = "dolphin_1.jpg";
	IplImage* image = cvLoadImage(imageName, CV_LOAD_IMAGE_COLOR);
	if(image == NULL){
		printf("\nError: failed to load image: %s !", imageName);
		return;
	}

	IplImage* srcImage = cvCreateImage(cvGetSize(image), IPL_DEPTH_32F ,image->nChannels);
	cvConvertScale(image, srcImage, 1/255.0);
	IplImage* cpyImage = cvCreateImage(cvGetSize(srcImage), srcImage->depth, srcImage->nChannels);
	assert(cpyImage != NULL);
	for( int x=0; x<srcImage->width/2; x++){
		for(int y=0; y<srcImage->height/4; y++){
			CopyPixelValF32(cpyImage, x, y, srcImage, x, y);
		}
	}

	cvNamedWindow(imageName);
	cvShowImage(imageName, cpyImage);
	cvWaitKey(0);

	cvDestroyAllWindows();
	cvReleaseImage(&cpyImage);
	cvReleaseImage(&srcImage);
}

void TestManulSelectMatchPoint()
{
	char* imageName1 = "dolphin_a1.jpg";
	char* imageName2 = "dolphin_a2_rot30.jpg";
	IplImage* image1 = cvLoadImage(imageName1, CV_LOAD_IMAGE_COLOR);
	if(image1 == NULL){
		printf("\nError: failed to load image: %s !", imageName1);
		return;
	}
	IplImage* image2 = cvLoadImage(imageName2, CV_LOAD_IMAGE_COLOR);
	if(image2 == NULL){
		printf("\nError: failed to load image: %s !", imageName2);
		return;
	}
	ManualSelectMatchPoints(image1, image2);
	cvWaitKey(0);
	cvDestroyAllWindows();
}

void TestManulStitch()
{
	//char* imageName1 = "girl_b1.jpg";
	//char* imageName2 = "girl_b2_rot30.jpg";

	//char* imageName1 = "girl_2.jpg";
	//char* imageName2 = "girl_5_rot45.jpg";

	//char* imageName1 = "ee_1.jpg";
	//char* imageName2 = "ee_2.jpg";

	char* imageName1 = "girl_b1.jpg";
	char* imageName2 = "girl_b2_rot30_s1.67333.jpg";

	IplImage* image1 = cvLoadImage(imageName1, CV_LOAD_IMAGE_COLOR);
	if(image1 == NULL){
		printf("\nError: failed to load image: %s !", imageName1);
		return;
	}
	IplImage* image2 = cvLoadImage(imageName2, CV_LOAD_IMAGE_COLOR);
	if(image2 == NULL){
		printf("\nError: failed to load image: %s !", imageName2);
		return;
	}
	
	ManulStitch(image1, image2);
}

void TestSolve()
{
	float data1[] = {
	1, 2, 0,
	4, 5, 6,
	1, 1, 0,
	1, 1, 1
	};

	float data2[] = {
		1, 1,
		2, 0,
		3, 0,
		3, 2
	};

	CvMat mat1, mat2;
	cvInitMatHeader(&mat1, 4, 3, CV_32FC1, data1, CV_AUTOSTEP);
	PrintMat(&mat1);
	cvInitMatHeader(&mat2, 4, 2, CV_32FC1, data2, CV_AUTOSTEP);
	PrintMat(&mat2);
	CvMat* pDstMat = cvCreateMat(3, 2, CV_32FC1);
	cvSolve(&mat1, &mat2, pDstMat, CV_QR);
	PrintMat(pDstMat);
}

void TestInvMap()
{
	char* imageName = "girl_b1.jpg";
	IplImage* tempImage = cvLoadImage(imageName, CV_LOAD_IMAGE_COLOR);
	if(tempImage == NULL){
		printf("\nError: failed to load image: %s !", imageName);
		return;
	}
	IplImage* image = GetFloatImage(tempImage);

	float mapMatData[] = {
		0.866, 0.5,
		-0.5, 0.866,
		100, 200
	};
	CvMat mapMat;
	cvInitMatHeader(&mapMat, 3, 2, CV_32FC1, mapMatData, CV_AUTOSTEP);

	IplImage* dstImage = cvCreateImage(cvSize(image->width * 3, image->height * 3), IPL_DEPTH_32F, image->nChannels);
	CvPoint offsetPoint, mappedPoint;
	offsetPoint.x = image->width;
	offsetPoint.y = image->height;

	CvMat* fromMat = cvCreateMat(1, 3, CV_32FC1);
	CvMat* toMat = cvCreateMat(1, 2, CV_32FC1);
	CvMat* invMat = GetInvMapMat(&mapMat);

	PrintMat(&mapMat);printf("\n");
	PrintMat(invMat);

	for(int x = 0; x < image->width; x++){
		for(int y = 0; y < image->height; y++){
			cvmSet(fromMat, 0, 0, x);
			cvmSet(fromMat, 0, 1, y);
			cvmSet(fromMat, 0, 2, 1);
			cvGEMM(fromMat, &mapMat, 1, NULL, 0, toMat, 0);
			//cvGEMM(fromMat, invMat, 1, NULL, 0, toMat, 0);
			mappedPoint.x = cvRound(cvmGet(toMat, 0, 0));
			mappedPoint.y = cvRound(cvmGet(toMat, 0, 1));
			mappedPoint.x += offsetPoint.x;
			mappedPoint.y += offsetPoint.y;
			CopyPixelValF32(dstImage, mappedPoint.x, mappedPoint.y, image, x, y);
		}
	}

	cvNamedWindow("TestImage");
	cvShowImage("TestImage", dstImage);
	cvWaitKey(0);
	cvDestroyAllWindows();
	cvReleaseImage(&tempImage);
	cvReleaseImage(&image);
}

