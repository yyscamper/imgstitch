
#include <highgui.h>
#include <stdio.h>
#include <io.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <fstream>

#include "sift.h"
#include "util.h"

using namespace std;

void WriteInfoToFile(CvSeq* seq, IplImage* srcImage, char* path);
void ShowSiftImage(IplImage* srcImage, CvPoint* point, int numOfPoints);

int main(int argc, char** argv)
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