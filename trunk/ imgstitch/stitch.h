
#ifndef _IMAGE_STITCH_STITCH_H_
#define  _IMAGE_STITCH_STITCH_H_

#include "cv.h"

/**
 * ������������ӳ����߽���ƴ��
 * @param mat1 �������1
 * @param mat2 �������2
 * @param mapMat ӳ�����
 * @return ƴ�Ӻ�ľ���
 */
IplImage* StitchImages(IplImage* srcImage1, IplImage* srcImage2, CvMat* mapMat);

#endif