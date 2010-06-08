
#ifndef _IMAGE_STITCH_MATCH_H_
#define _IMAGE_STITCH_MATCH_H_

#include "cv.h"
#include "sift.h"
#include "kdtree.h"
#include "imgfeatures.h"

/* the maximum number of keypoint NN candidates to check during BBF search */
#define KDTREE_BBF_MAX_NN_CHKS 200

/* threshold on squared ratio of distances between NN and 2nd NN */
#define NN_SQ_DIST_RATIO_THR 0.49


/**
 * 根据输入的匹配点，获取映射矩阵
 * 函数获得的是从fromPoint映射到toPoint的映射矩阵
 * @param fromPoint 映射的源点，相当于映射函数的定义域
 * @param toPoint   映射的像点， 相当于映射函数的值域
 * @param numOfPts  匹配点个数
 * @return 映射矩阵，为3x2的浮点数矩阵
 * @notice 注意fromPoint与toPoint的位置不要弄反了
 */
CvMat* GetMapMat(CvPoint* fromPoints, CvPoint* toPoints, int numOfPts);

/**
 * 根据映射关系，获取映射值
 * 映射关系通过映射矩阵来表示，此功能相当于定义了函数体，根据输入参数求函数值
 * @param srcPoint 输入的点
 * @param pDstPoint 映射后的点的指针
 * @param mapMat 映射矩阵，应该为3x2的数组
 * @return 映射后得到的像点
 */
void GetMapVal(CvPoint srcPoint, CvPoint* pDstPoint, CvMat* mapMat);

/**
 * 批量获取映射值
 * 此函数相当于多次调用GetMapVal，但是速度更快
 * @param pSrcPoints 输入的点，为一个数组
 * @param numOfPoints 输入点数组的元素个数
 * @param pDstPoints 保存映射后得到的像点的数组名，注意外部应该为其分配好内存空间
 * @param mapMat 映射矩阵，应该为3x2的数组
 */
void GetMapValInBatch(CvPoint* pSrcPoints, int numOfPoints, CvPoint* pDstPoints, CvMat* mapMat);

CvMat* GetInvMapMat(CvMat* mapMat);
CvMat* GetInvMapVal(CvPoint srcPoint, CvPoint* pDstPoint, CvMat* invMapMat);

CvMat* GetCascadeMapMat(CvMat** ppMatMats, int numOfMats);
/**
 * 使用RANSAC方法获取映射矩阵
 * @param fromPoint 输入的匹配点1，相当于映射函数的定义域
 * @param toPoint 输入的匹配点2， 相当于映射函数的值域
 * @param numOfPts 匹配点的个数
 * @return 映射矩阵，为3x2的浮点数矩阵
 * @notice 注意fromPoint与toPoint的位置不要弄反了
 */
CvMat* Ransac(CvPoint* fromPoints, CvPoint* toPoints, int numOfPts, int* pSelIndex);

int GetMatchPoints(IplImage* img1,IplImage* img2, CvPoint** ppPoint1, CvPoint** ppPoint2, bool* pSwapFlag);

#endif