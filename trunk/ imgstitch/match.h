
#ifndef _IMAGE_STITCH_MATCH_H_
#define _IMAGE_STITCH_MATCH_H_
#include "cv.h"

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
 * @param point 输入的点
 * @param mapMat 映射矩阵，应该为3x2的数组
 * @return 映射后得到的像点
 */
CvPoint GetMapVal(CvPoint point, CvMat* mapMat);

/**
 * 批量获取映射值
 * 此函数相当于多次调用GetMapVal，但是速度更快
 * @param pSrcPoints 输入的点，为一个数组
 * @param numOfPoints 输入点数组的元素个数
 * @param pDstPoints 保存映射后得到的像点的数组名，注意外部应该为其分配好内存空间
 * @param mapMat 映射矩阵，应该为3x2的数组
 */
void GetMapValInBatch(CvPoint* pSrcPoints, int numOfPoints, CvPoint* pDstPoints, CvMat* mapMat);

/**
 * 使用RANSAC方法获取映射矩阵
 * @param fromPoint 输入的匹配点1，相当于映射函数的定义域
 * @param toPoint 输入的匹配点2， 相当于映射函数的值域
 * @param numOfPts 匹配点的个数
 * @return 映射矩阵，为3x2的浮点数矩阵
 * @notice 注意fromPoint与toPoint的位置不要弄反了
 */
CvMat* Ransac(CvPoint* fromPoints, CvPoint* toPoints, int numOfPts);

//-------注意： 以下的两个函数暂时不用-------------//

/**
 *  匹配点结构体
 *  该结构体定义了两幅图像中匹配点的坐标信息。数组pt1、pt2分别为两幅图像的点，pt1中各个点与pt2中对应位置的点匹配，
 *  num描述了数组pt1和pt2的元素个数，pt1与pt2的个数应该相等。
 */
typedef struct{
	CvPoint* pt1;
	CvPoint* pt2;
	int num;
}MatchPoint_t;

/**
 *  获取图片的匹配信息
 *  根据输入的若干图像的SIFT特征寻找图像片的连接关系，返回根据连接关系排序后的图像数组以及两连接图像的匹配点信息
 *  @param ppSrcImg 输入的图像数组
 *  @param numOfSrcImg 输入的图像数组的大小
 *  @param ppDstImg 输出的图像数组，是将输入图像数组的元素根据SIFT特征匹配关系排序后的结果
 *  @param numOfDstImg 输出的图像数组的大小。由于输入的图像数组中某些图片不能与其他任何图片匹配，
 *         这些图像需要予以剔除，因此导致输出图像数组的大小与输入不同。
 *  @param pMatchPoint 输出图像数组中相邻图片的匹配点信息，参见MatchPoint_t结构的定义
 *  @return 如果输出的图像数组中第一幅与最后一幅存在连接关系，则返回1，
 *              同时pMatchPoint数组中的最后一项存储了第一幅与最后一幅的匹配点信息。
 *		    否则返回0.
 */
int GetMatchInfo(IplImage** ppSrcImg, int numOfSrcImg, IplImage** ppDstImg, int* numOfDstImg, MatchPoint_t* pMatchPoint);

/**
 *  获取匹配图像两两之间的映射矩阵
 *  根据输入的若干图像的SIFT特征寻找图像片的连接关系，返回根据连接关系排序后的图像数组以及两连接图像的匹配点信息
 *  @param ppSrcImg 输入的图像数组
 *  @param numOfSrcImg 输入的图像数组的大小
 *  @param ppDstImg 输出的图像数组，是将输入图像数组的元素根据SIFT特征匹配关系排序后的结果
 *  @param numOfDstImg 输出的图像数组的大小。由于输入的图像数组中某些图片不能与其他任何图片匹配，
 *         这些图像需要予以剔除，因此导致输出图像数组的大小与输入不同。
 *  @param ppHomMatrix 输出图像数组中相邻图片的匹配点信息，参见MatchPoint_t结构的定义
 *  @return 如果输出的图像数组中第一幅与最后一幅存在连接关系，则返回1，
 *              同时pMatchPoint数组中的最后一项存储了第一幅与最后一幅的匹配点信息。
 *		    否则返回0.
 */
int GetMapMat(IplImage** ppSrcImg, int numOfSrcImg, IplImage** ppDstImg, int* numOfDstImg, CvMat** ppHomMatrix);



#endif