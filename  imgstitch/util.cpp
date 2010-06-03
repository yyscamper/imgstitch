
#include "util.h"

IplImage* PlotPoints(CvPoint* point, int numOfPoints, IplImage *image, int type)
{	
	IplImage* img = cvCreateImage(cvGetSize(image), image->depth, image->nChannels);
	img = cvCloneImage(image);
	for(int i= 0; i < numOfPoints; i++){	
		switch(type){
			case PLOT_TYPE_DOT:
				cvCircle(img, point[i], 4, CV_RGB(255, 0, 0), -1, 8, 0);
				break;
			case PLOT_TYPE_CIRCLE:
				cvCircle(img, point[i], 4, CV_RGB(255, 0, 0), 1, 8, 0);
				break;
			case PLOT_TYPE_RECT:
				cvRectangle(img, cvPoint(point[i].x - 2, point[i].y - 2), 
					cvPoint(point[i].x + 2, point[i].y + 2), 
					CV_RGB(255, 0, 0), 1, 8, 0);
				break;
			default:
				break;
		}
	}
	return img;
}

CvPoint*  ExtractPos(CvSeq* seq)
{
	CvPoint* pt = (CvPoint*)calloc(seq->total, sizeof(CvPoint));
	CvMat* pElem = NULL;
	double zoomVal = 0.0;
	for(int i=0; i < seq->total; i++){
		pElem = (CvMat*)cvGetSeqElem(seq, i);
		zoomVal =  1;//pow(2.0, cvmGet(pElem, 0, 2) - 1);
		pt[i].x = cvRound(cvmGet(pElem, 0, 0) * zoomVal);
		pt[i].y = cvRound(cvmGet(pElem, 0 ,1) * zoomVal);
	}
	return pt;
}

void PrintMat(CvMat* m)
{	
	for(int r = 0; r < m->rows; r++){
		printf("\n");
		for(int c = 0; c < m->cols; c++){
			printf("%3.4f\t", cvmGet(m, r, c));
		}
	}
	printf("\n");
}
void SetMatVal(CvMat* m, double* val)
{
	for(int r = 0; r < m->rows; r++){
		for(int c = 0; c < m->cols; c++){
			cvmSet(m, r, c, val[r * m->rows + c]);
		}
	}
}