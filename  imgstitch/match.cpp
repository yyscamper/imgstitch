#include "match.h"
#include <assert.h>
#include "util.h"
#include "highgui.h"
#include "ui.h"
#include <time.h>

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

void RandNumbers(int* arr, int num, int max)
{
	int temp = 0;
	int i,j;
	for(i = 0; i < num; i++){
		do{
			temp = rand() % max;
			for(j=0; j < i; j++){
				if(temp == arr[j]){
					break;
				}
			}
		}while(i != j);
		arr[i] = temp;
	}
}

CvMat* Ransac(CvPoint* fromPoint, CvPoint* toPoint, int numOfPts, int* pSelIndex)
{
	printf("\nDoing Ransac to select the points....");
	//在未实现之前，暂时取前6个点求得匹配矩阵
	//return GetMapMat(fromPoint, toPoint, min(numOfPts, 10));
	int numOfPtsPerIter = 6; //每次迭代取点的个数
	assert(numOfPts >= numOfPtsPerIter);
	double pinliers = 0.9; //置信概率
	int inlierCntThreshold = cvCeil(numOfPts * pinliers);
	double distanceThreshold = 16;

	//int nIters = cvCeil(log(1 - pinliers) / log(1 - pow(pinliers, numOfPtsPerIter)));
	int nIters = 32;
	//int *pSelIndex = (int*)calloc(numOfPtsPerIter, sizeof(int));
	CvPoint* pSelFromPoints = (CvPoint*)calloc(numOfPtsPerIter, sizeof(CvPoint));
	CvPoint* pSelToPoints = (CvPoint*)calloc(numOfPtsPerIter, sizeof(CvPoint));
	CvMat* mapMat = NULL, *fromMat, *toMat, *bestMapMat;
	int inlierCnt = 0, lastMaxInlierCnt = -1;
	fromMat = cvCreateMat(1, 3, CV_32FC1);
	toMat = cvCreateMat(1, 2, CV_32FC1);
	srand((unsigned int)time(NULL));
	for(int i=0; i<nIters; i++){
		//随机选取numOfPtsPerIter个数据
		RandNumbers(pSelIndex, numOfPtsPerIter, numOfPts);
		//算出相应的单应矩阵模型参数
		for(int j=0; j < numOfPtsPerIter; j++){
			pSelFromPoints[j].x = fromPoint[pSelIndex[j]].x;
			pSelFromPoints[j].y = fromPoint[pSelIndex[j]].y;
			pSelToPoints[j].x = toPoint[pSelIndex[j]].x;
			pSelToPoints[j].y = toPoint[pSelIndex[j]].y;
		}
		mapMat = GetMapMat(pSelFromPoints, pSelToPoints, numOfPtsPerIter);
		//对其他numOfPts - numOfPtsPerIter个点计算与模型的距离，统计置信点的个数
		inlierCnt = 0;
		for(int k =0; k < numOfPts; k++){
			cvmSet(fromMat, 0, 0, fromPoint[k].x);
			cvmSet(fromMat, 0, 1, fromPoint[k].y);
			cvmSet(fromMat, 0, 2, 1);
			cvGEMM(fromMat, mapMat, 1, NULL, 0, toMat, 0);
			if( (toPoint[k].x - cvRound(cvmGet(toMat, 0, 0))) 
				* (toPoint[k].x - cvRound(cvmGet(toMat, 0, 0))) 
				+ (toPoint[k].y - cvRound(cvmGet(toMat, 0, 1))) 
				* (toPoint[k].y - cvRound(cvmGet(toMat, 0, 1)))
				< distanceThreshold ){
				inlierCnt++;
			}
		}
		//如果置信点的个数增大，则更新模型参数
		if(lastMaxInlierCnt < inlierCnt){
			lastMaxInlierCnt = inlierCnt;
			bestMapMat = cvCloneMat(mapMat);
		}
	}
	if(lastMaxInlierCnt < inlierCntThreshold){
		printf("\nRANSAC迭代结束后，内点个数较少，可能使结果不准确.(%d: %d/%d)", 
			inlierCntThreshold, lastMaxInlierCnt, numOfPts);
	}
	cvReleaseMat(&mapMat);
	//free(pSelIndex);
	free(pSelFromPoints);
	free(pSelToPoints);
	return bestMapMat;
}

CvMat* GetCascadeMapMat(CvMat** ppMapMats, int numOfMats)
{
	if(ppMapMats == NULL || numOfMats <= 0){
		return NULL;
	}
	double a1,b1,c1,d1,e1,f1,a2,b2,c2,d2,e2,f2,a3,b3,c3,d3,e3,f3;
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

int GetMatchPoints(IplImage* img1,IplImage* img2, CvPoint** ppPoint1, CvPoint** ppPoint2, bool* pSwapFlag)
{
	struct feature* feat1, * feat2, * feat;
	struct feature** nbrs;
	struct kd_node* kd_root;
	CvPoint pt1, pt2;
	double d0, d1;
	int n1, n2, k, i;
	int indexsum=0,indexleft=0,indexright = 0;
	bool swapFlag = false;
	int totalMatchPoints = 0;

	CvMemStorage* memstorage = cvCreateMemStorage(0);
	CvSeq *up_seq = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq), sizeof(CvPoint), memstorage); 
	CvSeq *down_seq = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq), sizeof(CvPoint), memstorage); 
	
	printf("Finding features in first image...\n");
	n1 = sift_features( img1, &feat1 );
	printf("Finding features in second image...\n" );
	n2 = sift_features( img2, &feat2 );

	kd_root = kdtree_build( feat2, n2 );
	for( i = 0; i < n1; i++ ){
		feat = feat1 + i;
		k = kdtree_bbf_knn( kd_root, feat, 2, &nbrs, KDTREE_BBF_MAX_NN_CHKS );
		if( k == 2 ){
			d0 = descr_dist_sq( feat, nbrs[0] );
			d1 = descr_dist_sq( feat, nbrs[1] );
			if( d0 < d1 * NN_SQ_DIST_RATIO_THR ){
				pt1 = cvPoint( cvRound( feat->x ), cvRound( feat->y ) );
				pt2 = cvPoint( cvRound( nbrs[0]->x ), cvRound( nbrs[0]->y ) );
				//---------存储单个点坐标到点序列中----------
				cvSeqPush(up_seq,&pt1); 
				cvSeqPush(down_seq,&pt2);

				if(pt1.x - pt2.x < 0){
					indexleft++;
				}else{
					indexright++;
				}
				indexsum = indexsum+pt1.x-pt2.x;
				pt2.y += img1->height;
				feat1[i].fwd_match = nbrs[0];
			}
		}
		free( nbrs );
	}
	totalMatchPoints = up_seq->total;
	printf("Found %d total matches\n", totalMatchPoints);
	//将匹配点序列转换为数组
	*ppPoint1 = (CvPoint*)calloc(totalMatchPoints, sizeof(CvPoint));
	*ppPoint2 = (CvPoint*)calloc(totalMatchPoints, sizeof(CvPoint));
	cvCvtSeqToArray(up_seq, *ppPoint1, CV_WHOLE_SEQ);
	cvCvtSeqToArray(down_seq, *ppPoint2, CV_WHOLE_SEQ);
	if (indexleft < totalMatchPoints * 0.8 && indexright > totalMatchPoints * 0.8){
		*pSwapFlag = true;
		printf("the index should be changed.\n");
	}else{
		*pSwapFlag = false;
	}
	
	//------释放内存存储器-----------
	cvReleaseMemStorage( &memstorage );
	kdtree_release( kd_root );
	free( feat1 );
	free( feat2 );
	return totalMatchPoints;
}