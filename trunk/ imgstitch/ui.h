#ifndef _IMAGE_STITCH_UI_H_
#define _IMAGE_STITCH_UI_H_

#include "cv.h"
#include "highgui.h"
#include "util.h"

//�����ɵ���Ƶ�ͼ����
IplImage* PlotPoints(CvPoint* point, int numOfPoints, IplImage *image, int type);
#define PLOT_TYPE_DOT		0
#define PLOT_TYPE_CIRCLE    1
#define PLOT_TYPE_RECT		2

//��Ϣ������ʾ
void ShowMsgWnd(char* title, char* msg);

//�˹�ѡ������ͼ���ƥ���
void ManualSelectMatchPoints(IplImage* image1, IplImage* image2);

//��ȡƥ�����Ϣ
int GetSelectedMatchPoints(CvPoint** point1, CvPoint** point2);

//ͨ���˹�ѡ�����ͼ��ƴ��
void ManulStitch(IplImage* image1, IplImage* image2);

void PlotMatchLine(IplImage* image, CvPoint* pt1, int num1, CvPoint* pt2, int num2);

IplImage* PlotMatchImage(IplImage* image1, IplImage* image2, CvPoint* pt1, CvPoint* pt2, int numOfPoints);
#endif