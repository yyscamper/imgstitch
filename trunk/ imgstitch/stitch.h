
#ifndef _IMAGE_STITCH_STITCH_H_
#define  _IMAGE_STITCH_STITCH_H_

#include "cv.h"

/**
 * 将两个矩阵按照映射光线进行拼接
 * @param mat1 输入矩阵1
 * @param mat2 输入矩阵2
 * @param mapMat 映射矩阵
 * @return 拼接后的矩阵
 */
IplImage* StitchTwoImages(IplImage* srcImage1, IplImage* srcImage2, CvMat* mapMat);

IplImage* StitchImages(IplImage** srcImages, int numOfImages, CvMat** mapMats);

#endif