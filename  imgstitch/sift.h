#ifndef _IMAGE_STITCH_SIFT_H_
#define _IMAGE_STITCH_SIFT_H_

#include <cv.h>

#define SIFT_INIT_SIGMA 0.5
#define SIFT_SIGMA 1.6
#define SIFT_OCTAVE_NUM 4
#define SIFT_PEROCTAVE 2                                        //每阶的层数
#define SIFT_DVALUE_THR 0.01									//DVALUE的门限值
#define SIFT_RVALUE_THR 12.1									//RVALUE的门限值
#define SIFT_MAX_INTERP_STEPS 5
#define SIFT_BORDER 10
#define SIFT_ARRAY_DIM 4
#define SIFT_DESCRIPTOR_DIM 8
#define SIFT_DES_NORM_THR 0.2

/**
 *  提取SIFT特征向量
 *  @param image 输入的图像
 *  @param descriptorseq SIFT所有特征点的特征向量，序列中每个元素为131维的向量（CvMat结构），
 *         向量中131个元素依次为：特征点横坐标、特征点纵坐标、特征点所在的层、128维特征向量
 */
void SIFT(CvSeq* decriptorseq, IplImage *image);

#endif


