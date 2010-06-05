#include "match.h"
#include <assert.h>
#include "util.h"

CvMat* GetMappingMat(CvPoint* pt1, CvPoint* pt2, int numOfPts)
{
	//方法一, 参考：http://www.irahul.com/workzone/stitch/index.php
	//assert(numOfPts == 4);
	CvMat* P = cvCreateMat(numOfPts, 3, CV_32FC1);
	CvMat* b = cvCreateMat(numOfPts, 2, CV_32FC1);
	CvMat* a = cvCreateMat(3, 2, CV_32FC1);

	//填充P矩阵
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
	int numOfPtsPerIter = 4; //每次迭代取点的个数
	double pinliers = 0.8; //置信概率
	int nIters = cvCeil(log(1 - pinliers) / log(1 - pow(pinliers, numOfPtsPerIter)));
	for(int i=0; i<nIters; i++){
		//随机选取numOfPtsPerIter个数据

		//算出相应的单应矩阵模型参数

		//对其他numOfPts - numOfPtsPerIter个点计算与模型的距离，统计置信点的个数

		//如果置信点的个数增大，则更新模型参数

	}
	CvMat* a = cvCreateMat(3, 2, CV_32FC1);
	return a;
}