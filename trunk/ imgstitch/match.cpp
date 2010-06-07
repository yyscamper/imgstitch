#include "match.h"
#include <assert.h>
#include "util.h"

CvMat* GetMapMat(CvPoint* fromPoints, CvPoint* toPoints, int numOfPts)
{
	//����һ, �ο���http://www.irahul.com/workzone/stitch/index.php
	//assert(numOfPts == 4);
	CvMat* P = cvCreateMat(numOfPts, 3, CV_32FC1);
	CvMat* b = cvCreateMat(numOfPts, 2, CV_32FC1);
	CvMat* a = cvCreateMat(3, 2, CV_32FC1);

	//���P����
	for(int r = 0; r < numOfPts; r++){
		cvmSet(P, r, 0, fromPoints[r].x);
		cvmSet(P, r, 1, fromPoints[r].y);
		cvmSet(P, r, 2, 1);
		cvmSet(b, r, 0, toPoints[r].x);
		cvmSet(b, r, 1, toPoints[r].y);
	}
	cvSolve(P, b, a, CV_QR);
	cvReleaseMat(&P);
	cvReleaseMat(&b);
	return a;
}

CvPoint GetMapVal(CvPoint point, CvMat* mapMat)
{
	CvMat* fromMat = cvCreateMat(1, 3, CV_32FC1);
	CvMat* toMat = cvCreateMat(1, 2, CV_32FC1);
	cvmSet(fromMat, 0, 0, point.x);
	cvmSet(fromMat, 0, 1, point.y);
	cvmSet(fromMat, 0, 2, 1);
	cvGEMM(fromMat, mapMat, 1, NULL, 0, toMat, 0);
	CvPoint dstPoint;
	dstPoint.x = cvRound(cvmGet(toMat, 0, 0));
	dstPoint.y = cvRound(cvmGet(toMat, 0, 1));
	cvReleaseMat(&fromMat);
	cvReleaseMat(&toMat);
	return dstPoint;
}

void GetMapValInBatch(CvPoint* pSrcPoints, int numOfPoints, CvPoint* pDstPoints, CvMat* mapMat)
{
	CvMat* fromMat = cvCreateMat(1, 3, CV_32FC1);
	CvMat* toMat = cvCreateMat(1, 2, CV_32FC1);
	for(int i=0; i<numOfPoints; i++){
		cvmSet(fromMat, 0, 0, pSrcPoints[i].x);
		cvmSet(fromMat, 0, 1, pSrcPoints[i].y);
		cvmSet(fromMat, 0, 2, 1);
		cvGEMM(fromMat, mapMat, 1, NULL, 0, toMat, 0);
		pDstPoints[i].x = cvRound(cvmGet(toMat, 0, 0));
		pDstPoints[i].y = cvRound(cvmGet(toMat, 0, 1));
	}
	cvReleaseMat(&fromMat);
	cvReleaseMat(&toMat);
}

CvMat* Ransac(CvPoint* fromPoint, CvPoint* toPoint, int numOfPts)
{
	int numOfPtsPerIter = 4; //ÿ�ε���ȡ��ĸ���
	double pinliers = 0.8; //���Ÿ���
	int nIters = cvCeil(log(1 - pinliers) / log(1 - pow(pinliers, numOfPtsPerIter)));
	for(int i=0; i<nIters; i++){
		//���ѡȡnumOfPtsPerIter������

		//�����Ӧ�ĵ�Ӧ����ģ�Ͳ���

		//������numOfPts - numOfPtsPerIter���������ģ�͵ľ��룬ͳ�����ŵ�ĸ���

		//������ŵ�ĸ������������ģ�Ͳ���

	}
	CvMat* a = cvCreateMat(3, 2, CV_32FC1);
	return a;
}