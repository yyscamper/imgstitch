#include "match.h"
#include <assert.h>
#include "util.h"

CvMat* GetMapMat(CvPoint* fromPoints, CvPoint* toPoints, int numOfPts)
{
	//方法一, 参考：http://www.irahul.com/workzone/stitch/index.php
	//assert(numOfPts == 4);
	CvMat* P = cvCreateMat(numOfPts, 3, CV_32FC1);
	CvMat* b = cvCreateMat(numOfPts, 2, CV_32FC1);
	CvMat* a = cvCreateMat(3, 2, CV_32FC1);

	//填充P矩阵
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

CvMat* GetInvMapMat(CvMat* mapMat)
{
	/*CvMat* invMap = cvCreateMat(2, 3, CV_32FC1);
	cvInvert(mapMat, invMap, CV_QR);
	return invMap;*/

	double a, b, c, d, e, f;
	a = cvmGet(mapMat, 0, 0);
	b = cvmGet(mapMat, 0, 1);
	c = cvmGet(mapMat, 1, 0);
	d = cvmGet(mapMat, 1, 1);
	e = cvmGet(mapMat, 2, 0);
	f = cvmGet(mapMat, 2, 1);
	if(abs(a*d - b*c) < 1E-10){ //等于零
		return NULL;
	}

	CvMat* invMap = cvCreateMat(3, 2, CV_32FC1);
	cvmSet(invMap, 0, 0, d);
	cvmSet(invMap, 0, 1, -b);
	cvmSet(invMap, 1, 0, -c);
	cvmSet(invMap, 1, 1, a);
	cvmSet(invMap, 2, 0, c*f - d*e);
	cvmSet(invMap, 2, 1, b*e - a*f);
	cvConvertScale(invMap, invMap, 1.0 / (a*d - b*c));
	return invMap;
}

void GetMapVal(CvPoint srcPoint, CvPoint* pDstPoint, CvMat* mapMat)
{
	CvMat* fromMat = cvCreateMat(1, 3, CV_32FC1);
	CvMat* toMat = cvCreateMat(1, 2, CV_32FC1);
	cvmSet(fromMat, 0, 0, srcPoint.x);
	cvmSet(fromMat, 0, 1, srcPoint.y);
	cvmSet(fromMat, 0, 2, 1);
	cvGEMM(fromMat, mapMat, 1, NULL, 0, toMat, 0);
	pDstPoint->x = cvRound(cvmGet(toMat, 0, 0));
	pDstPoint->y = cvRound(cvmGet(toMat, 0, 1));
	cvReleaseMat(&fromMat);
	cvReleaseMat(&toMat);
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

CvMat* GetCascadeMapMat(CvMat** ppMapMats, int numOfMats)
{
	if(ppMapMats == NULL || numOfMats <= 0){
		return NULL;
	}
	int a1,b1,c1,d1,e1,f1,a2,b2,c2,d2,e2,f2,a3,b3,c3,d3,e3,f3;
	CvMat* dstMat = cvCloneMat(ppMapMats[numOfMats - 1]);
	if(numOfMats == 1){
		return dstMat;
	}
	a2 = cvmGet(dstMat, 0, 0);
	b2 = cvmGet(dstMat, 1, 0);
	c2 = cvmGet(dstMat, 0, 1);
	d2 = cvmGet(dstMat, 1, 1);
	e2 = cvmGet(dstMat, 2, 0);
	f2 = cvmGet(dstMat, 2, 1);

	for(int i = numOfMats - 2; i >= 0; i--){
		a1 = cvmGet(ppMapMats[i], 0, 0);
		b1 = cvmGet(ppMapMats[i], 1, 0);
		c1 = cvmGet(ppMapMats[i], 0, 1);
		d1 = cvmGet(ppMapMats[i], 1, 1);
		e1 = cvmGet(ppMapMats[i], 2, 0);
		f1 = cvmGet(ppMapMats[i], 2, 1);

		a3 = a1*a2 + b1*c2;
		b3 = a1*b2 + b1*d2;
		c3 = c1*a2 + d1*c2;
		d3 = c1*b2 + d1*d2;
		e3 = a1*e2 + b1*f2 + e1;
		f3 = c1*e2 + d1*f2 + f1;

		a2 = a3;
		b2 = b3;
		c2 = c3;
		d2 = d3;
		e2 = e3;
		f2 = f3;
	}
	cvmSet(dstMat, 0, 0, a2);
	cvmSet(dstMat, 0, 1, c2);
	cvmSet(dstMat, 1, 0, b2);
	cvmSet(dstMat, 1, 1, d2);
	cvmSet(dstMat, 2, 0, e2);
	cvmSet(dstMat, 2, 1, f2);
	return dstMat;
}