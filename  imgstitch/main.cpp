
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

using namespace std;

void WriteInfoToFile(CvSeq* seq, IplImage* srcImage, char* path);
void ShowSiftImage(IplImage* srcImage, CvPoint* point, int numOfPoints);
void TestMappingMat();

int main(int argc, char** argv)
{
	TestMappingMat();
	system("pause");
	return 0;

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
	return 0;
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
	pt1[0].x = 1; pt2[0].x = 4;
	pt1[0].y = 2; pt2[0].y = 5;
	pt1[1].x = 3; pt2[1].x = 6;
	pt1[1].y = 4; pt2[1].y = 7;
	pt1[2].x = 5; pt2[2].x = 8;
	pt1[2].y = 6; pt2[2].y = 9;
	pt1[3].x = 7; pt2[3].x = 10;
	pt1[3].y = 8; pt2[3].y = 11;

	CvMat* mapMat = GetMappingMat(pt1, pt2, 4);
	PrintMat(mapMat);
}