
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
 * ���������ƥ��㣬��ȡӳ�����
 * ������õ��Ǵ�fromPointӳ�䵽toPoint��ӳ�����
 * @param fromPoint ӳ���Դ�㣬�൱��ӳ�亯���Ķ�����
 * @param toPoint   ӳ�����㣬 �൱��ӳ�亯����ֵ��
 * @param numOfPts  ƥ������
 * @return ӳ�����Ϊ3x2�ĸ���������
 * @notice ע��fromPoint��toPoint��λ�ò�ҪŪ����
 */
CvMat* GetMapMat(CvPoint* fromPoints, CvPoint* toPoints, int numOfPts);

/**
 * ����ӳ���ϵ����ȡӳ��ֵ
 * ӳ���ϵͨ��ӳ���������ʾ���˹����൱�ڶ����˺����壬���������������ֵ
 * @param srcPoint ����ĵ�
 * @param pDstPoint ӳ���ĵ��ָ��
 * @param mapMat ӳ�����Ӧ��Ϊ3x2������
 * @return ӳ���õ������
 */
void GetMapVal(CvPoint srcPoint, CvPoint* pDstPoint, CvMat* mapMat);

/**
 * ������ȡӳ��ֵ
 * �˺����൱�ڶ�ε���GetMapVal�������ٶȸ���
 * @param pSrcPoints ����ĵ㣬Ϊһ������
 * @param numOfPoints ����������Ԫ�ظ���
 * @param pDstPoints ����ӳ���õ���������������ע���ⲿӦ��Ϊ�������ڴ�ռ�
 * @param mapMat ӳ�����Ӧ��Ϊ3x2������
 */
void GetMapValInBatch(CvPoint* pSrcPoints, int numOfPoints, CvPoint* pDstPoints, CvMat* mapMat);

CvMat* GetInvMapMat(CvMat* mapMat);
CvMat* GetInvMapVal(CvPoint srcPoint, CvPoint* pDstPoint, CvMat* invMapMat);

CvMat* GetCascadeMapMat(CvMat** ppMatMats, int numOfMats);
/**
 * ʹ��RANSAC������ȡӳ�����
 * @param fromPoint �����ƥ���1���൱��ӳ�亯���Ķ�����
 * @param toPoint �����ƥ���2�� �൱��ӳ�亯����ֵ��
 * @param numOfPts ƥ���ĸ���
 * @return ӳ�����Ϊ3x2�ĸ���������
 * @notice ע��fromPoint��toPoint��λ�ò�ҪŪ����
 */
CvMat* Ransac(CvPoint* fromPoints, CvPoint* toPoints, int numOfPts, int* pSelIndex);

int GetMatchPoints(IplImage* img1,IplImage* img2, CvPoint** ppPoint1, CvPoint** ppPoint2, bool* pSwapFlag);

#endif