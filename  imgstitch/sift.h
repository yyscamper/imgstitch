#ifndef _IMAGE_STITCH_SIFT_H_
#define _IMAGE_STITCH_SIFT_H_

#include <cv.h>

#define SIFT_INIT_SIGMA 0.5
#define SIFT_SIGMA 1.6
#define SIFT_OCTAVE_NUM 4
#define SIFT_PEROCTAVE 2                                        //ÿ�׵Ĳ���
#define SIFT_DVALUE_THR 0.01									//DVALUE������ֵ
#define SIFT_RVALUE_THR 12.1									//RVALUE������ֵ
#define SIFT_MAX_INTERP_STEPS 5
#define SIFT_BORDER 10
#define SIFT_ARRAY_DIM 4
#define SIFT_DESCRIPTOR_DIM 8
#define SIFT_DES_NORM_THR 0.2

/**
 *  ��ȡSIFT��������
 *  @param image �����ͼ��
 *  @param descriptorseq SIFT���������������������������ÿ��Ԫ��Ϊ131ά��������CvMat�ṹ����
 *         ������131��Ԫ������Ϊ������������ꡢ�����������ꡢ���������ڵĲ㡢128ά��������
 */
void SIFT(CvSeq* decriptorseq, IplImage *image);

#endif


