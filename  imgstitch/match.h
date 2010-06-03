
#ifndef _IMAGE_STITCH_MATCH_H_
#define _IMAGE_STITCH_MATCH_H_
#include "cv.h"
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
int GetMappingMat(IplImage** ppSrcImg, int numOfSrcImg, IplImage** ppDstImg, int* numOfDstImg, CvMat** ppHomMatrix);

CvMat* GetMappingMat(CvPoint* pt1, CvPoint* pt2, int numOfPts);

/**
 * ʹ��RANSAC������ȡ��Ӧ����
 * @param pt1
 * @param pt2
 * @param numOfPts
 */
CvMat* Ransac(CvPoint* pt1, CvPoint* pt2, int numOfPts);

#endif