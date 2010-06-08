
#include <highgui.h>
#include <stdio.h>
#include <io.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <fstream>

#include "sift.h"
#include "util.h"
#include "match.h"
#include "stitch.h"
#include "ui.h"
#include "imgfeatures.h"
#include "kdtree.h"
#include "utils.h"
#include "xform.h"
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <stdio.h>

using namespace std;

void WriteInfoToFile(CvSeq* seq, IplImage* srcImage, char* path);
void ShowSiftImage(IplImage* srcImage, CvPoint* point, int numOfPoints);
void TestMappingMat();
void TestStitchImage();
void TestSiftFeature();
void TestCopyPixel();
void TestManulSelectMatchPoint();
void TestManulStitch();
void TestSolve();
void TestInvMap();
void TestAutoStitch();

/* the maximum number of keypoint NN candidates to check during BBF search */
#define KDTREE_BBF_MAX_NN_CHKS 200

/* threshold on squared ratio of distances between NN and 2nd NN */
#define NN_SQ_DIST_RATIO_THR 0.49

/******************************** Globals ************************************/

//char img1_file[] = ".\\beaver.png";		//怎么VC下一个点就够了的 .net下需要两个点
//char img2_file[] = ".\\beaver_xform.png";

//char img1_file[] = ".\\034.JPG";		//怎么VC下一个点就够了的 .net下需要两个点
//char img2_file[] = ".\\035.JPG";

char img1_file[MAX_PATH]="girl_b2_rot30.jpg";
char img2_file[MAX_PATH]="girl_c_rot15.jpg";
double imgzoom_scale=1.0;			//显示匹配结果图的缩放比例
IplImage* stacked;
/**************************other functions **************************************/
//----鼠标响应函数（用于根据鼠键响应缩放匹配结果的窗口大小）
void on_mouse( int event, int x, int y, int flags, void* param ) ;
//---改变匹配结果图像的大小------------------------
void resize_img();
int match_index(IplImage* img1,IplImage* img2,CvPoint* point1,CvPoint* point2);


int main(int argc, char** argv)
{
	//TestCopyPixel();
	//TestStitchImage();
	//TestMappingMat();
	//TestManulSelectMatchPoint();
	//TestManulStitch();
	//TestSolve();
	//TestInvMap();
	TestAutoStitch();


	//ShowMsgWnd("Msg Wnd", "Hello, this is the message!");
	system("pause");
	return 0;
}

void TestStitchImage()
{
	int numOfImages = 2;
	char** imageNames = (char**)calloc(numOfImages, sizeof(char*));
	IplImage** image = (IplImage**)calloc(numOfImages, sizeof(IplImage*));
	CvPoint pt1[4];
	CvPoint pt2[4];
	CvMat* mapMat = NULL;
	IplImage* dstImage = NULL;

	imageNames[0] = "dolphin_a1.jpg";
	imageNames[1] = "dolphin_a2_rot30.jpg";
	
	pt1[0].x = 196; pt2[0].x = 44;
	pt1[0].y = 144; pt2[0].y = 140;
	pt1[1].x = 162; pt2[1].x = 11;
	pt1[1].y = 176; pt2[1].y = 175;
	pt1[2].x = 154; pt2[2].x = 2;
	pt1[2].y = 152; pt2[2].y = 123;
	pt1[3].x = 245; pt2[3].x = 94;
	pt1[3].y = 177; pt2[3].y = 176;

	mapMat = GetMapMat(pt2, pt1, 4);
	double rotAngle = CV_PI/6;
	cvmSet(mapMat, 0, 0, cos(rotAngle));
	cvmSet(mapMat, 0, 1, sin(rotAngle));
	cvmSet(mapMat, 1, 0, -sin(rotAngle));
	cvmSet(mapMat, 1, 1, cos(rotAngle));
	cvmSet(mapMat, 2, 0, 128 * cos(rotAngle));
	cvmSet(mapMat, 2, 1, 128 * cos(rotAngle));
	for(int i=0; i<numOfImages; i++){
		image[i] = cvLoadImage(imageNames[i], CV_LOAD_IMAGE_COLOR);
		if(image[i] == NULL){
			printf("\nError: failed to load image: %s !", imageNames[i]);
			return;
		}
	}
	
	dstImage = StitchTwoImages(image[0], image[1], mapMat);
	cvNamedWindow("Stitch Image");
	cvShowImage("Stitch Image", dstImage);
	cvWaitKey(0);

	cvDestroyAllWindows();
	cvReleaseImage(&dstImage);
	cvReleaseMat(&mapMat);
	for(int i=0; i<numOfImages; i++){
		cvReleaseImage(&image[i]);
	}
	free(image);
	free(imageNames);
}

//void TestSiftFeature()
//{
//	IplImage *srcimage = 0;
//	char pathtempin[] = "chair.jpg";
//	char outFilePath[] = "air.txt";
//
//	srcimage = cvLoadImage(pathtempin, 0);
//	if (srcimage == NULL){
//		printf("\nError: failed to load image: %s !", pathtempin);
//		exit(-1);
//	}
//
//	//创建descriptor存储,求SFIT特征点
//	CvMemStorage* descriptorstorage = cvCreateMemStorage(0);
//	CvSeq* decriptorseq = cvCreateSeq(0,sizeof(CvSeq),sizeof(CvMat),descriptorstorage);
//	SIFT(decriptorseq, srcimage);
//
//	WriteInfoToFile(decriptorseq, srcimage, outFilePath);
//
//	CvPoint* pPointArr = ExtractPos(decriptorseq);
//	int numOfPoints = decriptorseq->total;
//
//	ShowSiftImage(srcimage, pPointArr, numOfPoints);
//
//	cvReleaseImage(&srcimage);
//	cvReleaseMemStorage(&descriptorstorage);
//	free(pPointArr);
//}
//

void ShowSiftImage(IplImage* srcImage, CvPoint* point, int numOfPoints)
{
	IplImage* siftImage = PlotPoints(point, numOfPoints, srcImage, PLOT_TYPE_DOT);
	cvNamedWindow("SIFT Image");
	cvShowImage("SIFT Image", siftImage);
	cvWaitKey(0);
	cvDestroyAllWindows();
	cvReleaseImage(&siftImage);
}

void TestMappingMat()
{
	CvPoint pt1[4];
	CvPoint pt2[4];
	/*pt1[0].x = 1; pt2[0].x = 4;
	pt1[0].y = 2; pt2[0].y = 5;
	pt1[1].x = 3; pt2[1].x = 6;
	pt1[1].y = 4; pt2[1].y = 7;
	pt1[2].x = 5; pt2[2].x = 8;
	pt1[2].y = 6; pt2[2].y = 9;
	pt1[3].x = 7; pt2[3].x = 10;
	pt1[3].y = 8; pt2[3].y = 11;*/

	/*pt1[0].x = 1; pt2[0].x = 4;
	pt1[0].y = 1; pt2[0].y = 4;
	pt1[1].x = 2; pt2[1].x = 5;
	pt1[1].y = 1; pt2[1].y = 4;
	pt1[2].x = 2; pt2[2].x = 5;
	pt1[2].y = 5; pt2[2].y = 8;
	pt1[3].x = 1; pt2[3].x = 4;
	pt1[3].y = 3; pt2[3].y = 6;*/

	//dolphin_1, dolphin_2
	pt1[0].x = 196; pt2[0].x = 44;
	pt1[0].y = 144; pt2[0].y = 140;
	pt1[1].x = 162; pt2[1].x = 11;
	pt1[1].y = 176; pt2[1].y = 175;
	pt1[2].x = 154; pt2[2].x = 2;
	pt1[2].y = 152; pt2[2].y = 123;
	pt1[3].x = 245; pt2[3].x = 94;
	pt1[3].y = 177; pt2[3].y = 176;

	CvMat* mapMat = GetMapMat(pt2, pt1, 4);
	PrintMat(mapMat);
}

//void WriteInfoToFile(CvSeq* seq, IplImage* srcImage, char* path)
//{
//	double temp;
//	int i,j;
//	int counter = seq->total;
//	ofstream outFile(path); 
//
//	outFile<<srcImage->width/2<<" ";
//	outFile<<srcImage->height/2<<" ";
//	outFile<<"\n";
//	CvMat* mat = NULL;
//	for(i=0;i<counter;i++)
//	{
//		mat = (CvMat*)cvGetSeqElem(seq, i);
//		for(j=0;j<SIFT_DESCRIPTOR_DIM * SIFT_ARRAY_DIM * SIFT_ARRAY_DIM+3;j++)
//		{
//			temp = cvmGet(mat,0,j);
//			outFile<<temp<<" ";
//		}
//		outFile<<"\n";
//	}
//	outFile.close();
//}

void TestCopyPixel()
{
	char* imageName = "dolphin_1.jpg";
	IplImage* image = cvLoadImage(imageName, CV_LOAD_IMAGE_COLOR);
	if(image == NULL){
		printf("\nError: failed to load image: %s !", imageName);
		return;
	}

	IplImage* srcImage = cvCreateImage(cvGetSize(image), IPL_DEPTH_32F ,image->nChannels);
	cvConvertScale(image, srcImage, 1/255.0);
	IplImage* cpyImage = cvCreateImage(cvGetSize(srcImage), srcImage->depth, srcImage->nChannels);
	assert(cpyImage != NULL);
	for( int x=0; x<srcImage->width/2; x++){
		for(int y=0; y<srcImage->height/4; y++){
			CopyPixelValF32(cpyImage, x, y, srcImage, x, y);
		}
	}

	cvNamedWindow(imageName);
	cvShowImage(imageName, cpyImage);
	cvWaitKey(0);

	cvDestroyAllWindows();
	cvReleaseImage(&cpyImage);
	cvReleaseImage(&srcImage);
}

void TestManulSelectMatchPoint()
{
	char* imageName1 = "dolphin_a1.jpg";
	char* imageName2 = "dolphin_a2_rot30.jpg";
	IplImage* image1 = cvLoadImage(imageName1, CV_LOAD_IMAGE_COLOR);
	if(image1 == NULL){
		printf("\nError: failed to load image: %s !", imageName1);
		return;
	}
	IplImage* image2 = cvLoadImage(imageName2, CV_LOAD_IMAGE_COLOR);
	if(image2 == NULL){
		printf("\nError: failed to load image: %s !", imageName2);
		return;
	}
	ManualSelectMatchPoints(image1, image2);
	cvWaitKey(0);
	cvDestroyAllWindows();
}

void TestManulStitch()
{
	//char* imageName1 = "girl_b1.jpg";
	//char* imageName2 = "girl_b2_rot30.jpg";

	//char* imageName1 = "girl_2.jpg";
	//char* imageName2 = "girl_5_rot45.jpg";

	//char* imageName1 = "ee_1.jpg";
	//char* imageName2 = "ee_2.jpg";

	char* imageName1 = "girl_b1.jpg";
	char* imageName2 = "girl_b2_rot30_s1.67333.jpg";

	IplImage* image1 = cvLoadImage(imageName1, CV_LOAD_IMAGE_COLOR);
	if(image1 == NULL){
		printf("\nError: failed to load image: %s !", imageName1);
		return;
	}
	IplImage* image2 = cvLoadImage(imageName2, CV_LOAD_IMAGE_COLOR);
	if(image2 == NULL){
		printf("\nError: failed to load image: %s !", imageName2);
		return;
	}
	
	ManulStitch(image1, image2);
}

void TestSolve()
{
	float data1[] = {
	1, 2, 0,
	4, 5, 6,
	1, 1, 0,
	1, 1, 1
	};

	float data2[] = {
		1, 1,
		2, 0,
		3, 0,
		3, 2
	};

	CvMat mat1, mat2;
	cvInitMatHeader(&mat1, 4, 3, CV_32FC1, data1, CV_AUTOSTEP);
	PrintMat(&mat1);
	cvInitMatHeader(&mat2, 4, 2, CV_32FC1, data2, CV_AUTOSTEP);
	PrintMat(&mat2);
	CvMat* pDstMat = cvCreateMat(3, 2, CV_32FC1);
	cvSolve(&mat1, &mat2, pDstMat, CV_QR);
	PrintMat(pDstMat);
}

void TestInvMap()
{
	char* imageName = "girl_b1.jpg";
	IplImage* tempImage = cvLoadImage(imageName, CV_LOAD_IMAGE_COLOR);
	if(tempImage == NULL){
		printf("\nError: failed to load image: %s !", imageName);
		return;
	}
	IplImage* image = GetFloatImage(tempImage);

	float mapMatData[] = {
		0.866, 0.5,
		-0.5, 0.866,
		100, 200
	};
	CvMat mapMat;
	cvInitMatHeader(&mapMat, 3, 2, CV_32FC1, mapMatData, CV_AUTOSTEP);

	IplImage* dstImage = cvCreateImage(cvSize(image->width * 3, image->height * 3), IPL_DEPTH_32F, image->nChannels);
	CvPoint offsetPoint, mappedPoint;
	offsetPoint.x = image->width;
	offsetPoint.y = image->height;

	CvMat* fromMat = cvCreateMat(1, 3, CV_32FC1);
	CvMat* toMat = cvCreateMat(1, 2, CV_32FC1);
	CvMat* invMat = GetInvMapMat(&mapMat);

	PrintMat(&mapMat);printf("\n");
	PrintMat(invMat);

	for(int x = 0; x < image->width; x++){
		for(int y = 0; y < image->height; y++){
			cvmSet(fromMat, 0, 0, x);
			cvmSet(fromMat, 0, 1, y);
			cvmSet(fromMat, 0, 2, 1);
			cvGEMM(fromMat, &mapMat, 1, NULL, 0, toMat, 0);
			//cvGEMM(fromMat, invMat, 1, NULL, 0, toMat, 0);
			mappedPoint.x = cvRound(cvmGet(toMat, 0, 0));
			mappedPoint.y = cvRound(cvmGet(toMat, 0, 1));
			mappedPoint.x += offsetPoint.x;
			mappedPoint.y += offsetPoint.y;
			CopyPixelValF32(dstImage, mappedPoint.x, mappedPoint.y, image, x, y);
		}
	}

	cvNamedWindow("TestImage");
	cvShowImage("TestImage", dstImage);
	cvWaitKey(0);
	cvDestroyAllWindows();
	cvReleaseImage(&tempImage);
	cvReleaseImage(&image);
}

void TestAutoStitch()
{
	CvPoint* point1 = NULL;
	CvPoint* point2 = NULL;
	IplImage* img1, * img2;
	//	char img1_file[MAX_PATH]="whitebear.jpg";
	//	char img2_file[MAX_PATH]="whitebear3.jpg";

	img1 = cvLoadImage( img1_file, 1 );
	if( ! img1 )
		fatal_error( "unable to load image from %s", img1_file );
	img2 = cvLoadImage( img2_file, 1 );
	if( ! img2 )
		fatal_error( "unable to load image from %s", img2_file );

	point1 = (CvPoint*)calloc( 500, sizeof(struct CvPoint) );
	point2 = (CvPoint*)calloc(500, sizeof(struct CvPoint));
	match_index(img1,img2,point1,point2);
	printf("point1[1]=%d\n",point1[1].x);


	//	resize_img();



	cvWaitKey( 0 );

	free(point1);
	free(point2);

	return;
}


void on_mouse( int event, int x, int y, int flags, void* param ) 
{
	if( (event==CV_EVENT_LBUTTONUP) &&  (flags==CV_EVENT_FLAG_CTRLKEY) ) 
	{
				
	}
	
	
	
	if( (event==CV_EVENT_LBUTTONUP) &&  (flags==CV_EVENT_FLAG_ALTKEY) ) 
		//ALT和鼠标左键开始按下的时候放大匹配结果图
	{
		
		
		//---------特征点匹配函数--------------	
		if(imgzoom_scale<1.5)
		{
			imgzoom_scale=1.1*imgzoom_scale;
		}
		
		else imgzoom_scale=1.0;
		//----放大匹配结果图-----
		resize_img();

	}
	
	if( (event==CV_EVENT_RBUTTONUP) &&  (flags==CV_EVENT_FLAG_ALTKEY) ) 
		
		//ALT和鼠标右键开始按下的时候缩小匹配结果图
	{
		
		
		//---------特征点匹配函数--------------	
		if(imgzoom_scale>0.0)
		{
			imgzoom_scale=0.9*imgzoom_scale;
		}
		
		else imgzoom_scale=0.5;

		//----缩小匹配结果图-----
		resize_img();

		//printf("%f\n",imgzoom_scale);
	}
}

//--------------改变匹配结果图像的大小------------------------

void resize_img()
{
	IplImage* resize_stacked;

	resize_stacked=cvCreateImage(cvSize(  (int)(stacked->width*imgzoom_scale),  (int)(stacked->height*imgzoom_scale)  ),
				stacked->depth,
				stacked->nChannels);
	//----缩放匹配结果图的大小------

	cvResize(stacked, resize_stacked, CV_INTER_AREA );
	
	printf("\n同时ALT键和鼠标右键按下将缩小匹配结果图\n\n");

	printf("同时ALT键和鼠标左键按下将放大匹配结果图\n");


	cvNamedWindow( "Matches", 1 );

	
	//---------鼠键响应函数--------------
	
	cvSetMouseCallback("Matches", on_mouse, 0 );
	

	cvShowImage( "Matches", resize_stacked);

	cvWaitKey( 0 );


}

int match_index(IplImage* img1,IplImage* img2,CvPoint* point1,CvPoint* point2)
{
	//	IplImage* img1, * img2;
		IplImage* resize_stacked;
	struct feature* feat1, * feat2, * feat;
	struct feature** nbrs;
	struct kd_node* kd_root;
	CvPoint pt1, pt2;
	double d0, d1;
	int n1, n2, k, i, m = 0;
	int indexsum=0,indexleft=0,indexright = 0;

//	char img1_file[MAX_PATH]="whitebear.jpg";
//	char img2_file[MAX_PATH]="whitebear3.jpg";
	
	//------------内存存储器------------
	CvMemStorage* memstorage = cvCreateMemStorage(0);

	//-----------up_seq存储img1匹配出的特征点的坐标-----------

	CvSeq *up_seq = cvCreateSeq(	CV_SEQ_ELTYPE_POINT,
					sizeof(CvSeq),
					sizeof(CvPoint),
					memstorage); 

	//-----------down_seq存储img2匹配出的特征点的坐标-----------

	CvSeq *down_seq = cvCreateSeq(	CV_SEQ_ELTYPE_POINT,
					sizeof(CvSeq),
					sizeof(CvPoint),
					memstorage); 
	
/*
	img1 = cvLoadImage( img1_file, 1 );
	if( ! img1 )
		fatal_error( "unable to load image from %s", img1_file );
	img2 = cvLoadImage( img2_file, 1 );
	if( ! img2 )
		fatal_error( "unable to load image from %s", img2_file );
		*/

	stacked = stack_imgs( img1, img2 );


	fprintf( stderr, "Finding features in %s...\n", img1_file );
	n1 = sift_features( img1, &feat1 );

	
	fprintf( stderr, "Finding features in %s...\n", img2_file );
	n2 = sift_features( img2, &feat2 );


	//----------显示img1的SIFT特征---------
	if(1)
	{
		draw_features( img1, feat1, n1 );
		cvNamedWindow( "img1_feature", 1 );
		cvShowImage( "img1_feature", img1 );
	}


	//----------显示img2的SIFT特征---------
	if(1)
	{
		draw_features( img2, feat2, n2 );
		cvNamedWindow( "img2_feature", 1 );
		cvShowImage( "img2_feature", img2 );
	}

	kd_root = kdtree_build( feat2, n2 );



	for( i = 0; i < n1; i++ )
	{
		feat = feat1 + i;
		k = kdtree_bbf_knn( kd_root, feat, 2, &nbrs, KDTREE_BBF_MAX_NN_CHKS );
	//	printf("%d",k);
		if( k == 2 )
		{
			d0 = descr_dist_sq( feat, nbrs[0] );
			d1 = descr_dist_sq( feat, nbrs[1] );
			if( d0 < d1 * NN_SQ_DIST_RATIO_THR )
			{
				pt1 = cvPoint( cvRound( feat->x ), cvRound( feat->y ) );
				pt2 = cvPoint( cvRound( nbrs[0]->x ), cvRound( nbrs[0]->y ) );

				//---------存储单个点坐标到点序列中----------
				cvSeqPush(up_seq,&pt1); 
				cvSeqPush(down_seq,&pt2);

				if(pt1.x-pt2.x < 0)
					indexleft++;
				else
					indexright++;
				indexsum = indexsum+pt1.x-pt2.x;
				cvLine(stacked,pt1,pt1,CV_RGB(0,255,255),1,8,0);
				pt2.y += img1->height;
				cvLine( stacked, pt1, pt2, CV_RGB(255,0,255), 1, 8, 0 );
				m++;
				feat1[i].fwd_match = nbrs[0];
				point1[m] = pt1;
				point2[m] = pt2;

				
			}
		}
		free( nbrs );
	}



	fprintf( stderr, "Found %d total matches\n", m );



	//---------存储匹配点的坐标到文件-----------------

	cvSave("upimg_matched_points.txt", up_seq, NULL, "matched_points_up", cvAttrList(NULL,NULL));
	cvSave("downimg_matched_points.txt", down_seq, NULL, "matched_points_down", cvAttrList(NULL,NULL));

	fprintf( stderr, "%d 个匹配的点的坐标已分别保存到文件:\n upimg_matched_points.xml和downimg_matched_points.xml\n", m );	

	printf("indexsum=%d\n indexleft = %d\n indexright = %d\n",indexsum,indexleft,indexright);
	if (indexleft < m * 0.8 && indexright > m * 0.8) printf("the index should be changed.\n");


			resize_stacked=cvCreateImage(cvSize(  (int)(stacked->width*imgzoom_scale),  (int)(stacked->height*imgzoom_scale)  ),
				stacked->depth,
				stacked->nChannels);
	//----缩放匹配结果图的大小------

	cvResize(stacked, resize_stacked, CV_INTER_AREA );


	cvNamedWindow( "Matches", 1 );
		cvShowImage( "Matches", resize_stacked);

//	resize_img();

//	cvWaitKey( 0 );


	/* 
	UNCOMMENT BELOW TO SEE HOW RANSAC FUNCTION WORKS

	Note that this line above:

	feat1[i].fwd_match = nbrs[0];

	is important for the RANSAC function to work.
	*/
	
	{
		CvMat* H;
		H = ransac_xform( feat1, n1, FEATURE_FWD_MATCH, lsq_homog, 4, 0.01,
			homog_xfer_err, 3.0, NULL, NULL );
		if( H )
		{
			IplImage* xformed;
			CvSize newsize;
			newsize.height = cvGetSize( img2 ).height*2;
			newsize.width = cvGetSize( img2 ).width*2;
			xformed = cvCreateImage(newsize, IPL_DEPTH_8U, 3 );

			cvWarpPerspective( img1, xformed, H, 
				CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS,
				cvScalarAll( 0 ) );
			cvNamedWindow( "Xformed", 1 );
			cvShowImage( "Xformed", xformed );
//			cvWaitKey( 0 );
			cvReleaseImage( &xformed );
			cvReleaseMat( &H );
		}
	}



	//------释放内存存储器-----------
	cvReleaseMemStorage( &memstorage );
	cvReleaseImage( &stacked );
	cvReleaseImage( &img1 );
	cvReleaseImage( &img2 );
	kdtree_release( kd_root );
	free( feat1 );
	free( feat2 );

	return 0;
}
