
#ifndef _IMAGE_STITCH_UTIL_H_
#define _IMAGE_STITCH_UTIL_H_

#include <cv.h>

#define PLOT_TYPE_DOT		0
#define PLOT_TYPE_CIRCLE    1
#define PLOT_TYPE_RECT		2

IplImage* PlotPoints(CvPoint* point, int numOfPoints, IplImage *image, int type);
CvPoint*  ExtractPos(CvSeq* pDescSeq);
#endif