#include "match.h"
#include <assert.h>
#include "util.h"

CvMat* GetMappingMat(CvPoint* pt1, CvPoint* pt2, int numOfPts)
{
	//����һ, �ο���http://www.irahul.com/workzone/stitch/index.php
	//assert(numOfPts == 4);
	CvMat* P = cvCreateMat(numOfPts, 3, CV_32FC1);
	CvMat* b = cvCreateMat(numOfPts, 2, CV_32FC1);
	CvMat* a = cvCreateMat(3, 2, CV_32FC1);

	//���P����
	for(int r = 0; r < numOfPts; r++){
		cvmSet(P, r, 0, pt1[r].x);
		cvmSet(P, r, 1, pt1[r].y);
		cvmSet(P, r, 2, 1);
		cvmSet(b, r, 0, pt2[r].x);
		cvmSet(b, r, 1, pt2[r].y);
	}
	cvSolve(P, b, a, CV_QR);
	cvReleaseMat(&P);
	cvReleaseMat(&b);
	return a;
}

CvMat* Ransac(CvPoint* pt1, CvPoint* pt2, int numOfPts)
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