
#ifndef _IMAGE_STITCH_MATCH_H_
#define _IMAGE_STITCH_MATCH_H_
#include "cv.h"

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
 * @param point ����ĵ�
 * @param mapMat ӳ�����Ӧ��Ϊ3x2������
 * @return ӳ���õ������
 */
CvPoint GetMapVal(CvPoint point, CvMat* mapMat);

/**
 * ������ȡӳ��ֵ
 * �˺����൱�ڶ�ε���GetMapVal�������ٶȸ���
 * @param pSrcPoints ����ĵ㣬Ϊһ������
 * @param numOfPoints ����������Ԫ�ظ���
 * @param pDstPoints ����ӳ���õ���������������ע���ⲿӦ��Ϊ�������ڴ�ռ�
 * @param mapMat ӳ�����Ӧ��Ϊ3x2������
 */
void GetMapValInBatch(CvPoint* pSrcPoints, int numOfPoints, CvPoint* pDstPoints, CvMat* mapMat);

/**
 * ʹ��RANSAC������ȡӳ�����
 * @param fromPoint �����ƥ���1���൱��ӳ�亯���Ķ�����
 * @param toPoint �����ƥ���2�� �൱��ӳ�亯����ֵ��
 * @param numOfPts ƥ���ĸ���
 * @return ӳ�����Ϊ3x2�ĸ���������
 * @notice ע��fromPoint��toPoint��λ�ò�ҪŪ����
 */
CvMat* Ransac(CvPoint* fromPoints, CvPoint* toPoints, int numOfPts);

//-------ע�⣺ ���µ�����������ʱ����-------------//

/**
 *  ƥ���ṹ��
 *  �ýṹ�嶨��������ͼ����ƥ����������Ϣ������pt1��pt2�ֱ�Ϊ����ͼ��ĵ㣬pt1�и�������pt2�ж�Ӧλ�õĵ�ƥ�䣬
 *  num����������pt1��pt2��Ԫ�ظ�����pt1��pt2�ĸ���Ӧ����ȡ�
 */
typedef struct{
	CvPoint* pt1;
	CvPoint* pt2;
	int num;
}MatchPoint_t;

/**
 *  ��ȡͼƬ��ƥ����Ϣ
 *  �������������ͼ���SIFT����Ѱ��ͼ��Ƭ�����ӹ�ϵ�����ظ������ӹ�ϵ������ͼ�������Լ�������ͼ���ƥ�����Ϣ
 *  @param ppSrcImg �����ͼ������
 *  @param numOfSrcImg �����ͼ������Ĵ�С
 *  @param ppDstImg �����ͼ�����飬�ǽ�����ͼ�������Ԫ�ظ���SIFT����ƥ���ϵ�����Ľ��
 *  @param numOfDstImg �����ͼ������Ĵ�С�����������ͼ��������ĳЩͼƬ�����������κ�ͼƬƥ�䣬
 *         ��Щͼ����Ҫ�����޳�����˵������ͼ������Ĵ�С�����벻ͬ��
 *  @param pMatchPoint ���ͼ������������ͼƬ��ƥ�����Ϣ���μ�MatchPoint_t�ṹ�Ķ���
 *  @return ��������ͼ�������е�һ�������һ���������ӹ�ϵ���򷵻�1��
 *              ͬʱpMatchPoint�����е����һ��洢�˵�һ�������һ����ƥ�����Ϣ��
 *		    ���򷵻�0.
 */
int GetMatchInfo(IplImage** ppSrcImg, int numOfSrcImg, IplImage** ppDstImg, int* numOfDstImg, MatchPoint_t* pMatchPoint);

/**
 *  ��ȡƥ��ͼ������֮���ӳ�����
 *  �������������ͼ���SIFT����Ѱ��ͼ��Ƭ�����ӹ�ϵ�����ظ������ӹ�ϵ������ͼ�������Լ�������ͼ���ƥ�����Ϣ
 *  @param ppSrcImg �����ͼ������
 *  @param numOfSrcImg �����ͼ������Ĵ�С
 *  @param ppDstImg �����ͼ�����飬�ǽ�����ͼ�������Ԫ�ظ���SIFT����ƥ���ϵ�����Ľ��
 *  @param numOfDstImg �����ͼ������Ĵ�С�����������ͼ��������ĳЩͼƬ�����������κ�ͼƬƥ�䣬
 *         ��Щͼ����Ҫ�����޳�����˵������ͼ������Ĵ�С�����벻ͬ��
 *  @param ppHomMatrix ���ͼ������������ͼƬ��ƥ�����Ϣ���μ�MatchPoint_t�ṹ�Ķ���
 *  @return ��������ͼ�������е�һ�������һ���������ӹ�ϵ���򷵻�1��
 *              ͬʱpMatchPoint�����е����һ��洢�˵�һ�������һ����ƥ�����Ϣ��
 *		    ���򷵻�0.
 */
int GetMapMat(IplImage** ppSrcImg, int numOfSrcImg, IplImage** ppDstImg, int* numOfDstImg, CvMat** ppHomMatrix);



#endif