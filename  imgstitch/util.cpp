
#include "util.h"

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
	printf("\n");
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

float GetPixelVal(IplImage* image, int x, int y, int c)
{
	return ((float*)(image->imageData + x * image->widthStep))[y * image->nChannels + c];
}

void SetPixelVal(IplImage* image, int x, int y, int c, float val)
{
	((float*)(image->imageData + x * image->widthStep))[y * image->nChannels + c] = val;
}

void CopyPixelVal(IplImage* dstImage, int dstX, int dstY, IplImage* srcImage, int srcX, int srcY)
{
	assert(srcImage->nChannels == dstImage->nChannels && srcImage->depth == IPL_DEPTH_32F && dstImage->depth == IPL_DEPTH_32F);
	for(int c = 0; c < srcImage->nChannels; c++){
		//printf("\nBefore Copy: %f", ((float*)(dstImage->imageData + dstY * dstImage->widthStep))[dstX * dstImage->nChannels + c] );
		((float*)(dstImage->imageData + dstY * dstImage->widthStep))[dstX * dstImage->nChannels + c] 
			= ((float*)(srcImage->imageData + srcY * srcImage->widthStep))[srcX * srcImage->nChannels + c];
		//printf("\nAfter Copy:%f", ((float*)(dstImage->imageData + dstY * dstImage->widthStep))[dstX * dstImage->nChannels + c]);
	}
}

IplImage* GetFloatImage(IplImage* srcImage)
{
	IplImage* dstImage = cvCreateImage(cvGetSize(srcImage), IPL_DEPTH_32F ,srcImage->nChannels);
	if(srcImage->depth == IPL_DEPTH_8U){
		cvConvertScale(srcImage, dstImage, 1/255.0);
	}else if(srcImage->depth == IPL_DEPTH_32F){
		cvCopyImage(srcImage, dstImage);
	}else{
		printf("\nIn GetFloatImage(), the depth of input image is not IPL_DEPTH_8U!");
		return NULL;
	}
	return dstImage;	
}

bool IsPointInRect(CvRect* rect, int x, int y)
{
	if(rect == NULL){
		return false;
	}
	return (x >= rect->x && x < rect->x + rect->width && y >= rect->y && y < rect->y + rect->height);
}


