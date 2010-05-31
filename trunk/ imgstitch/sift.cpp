
#include <highgui.h>
#include <stdio.h>
#include <io.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <fstream>

#include "sift.h"

using namespace std;

#define PI 3.14159
#define Scale_K 1.414
#define interp_hist_peak( l, c, r ) ( 0.5 * ((l)-(r)) / ((l) - 2.0*(c) + (r)) )

typedef struct Octave{
    int level;
    IplImage* scales[SIFT_PEROCTAVE+3];
}Octave;

typedef struct Feat{
	int x;
	int y;
	int level;
}Feat;

//找到特征点的候选点
void FindCandidatePoint(CvMat** DoGMat,CvSeq* pointseq);
//得到一幅图像（x,y）邻域8点，存入points
void Get8points(CvMat* mat,double* points,int h,int w);
//得到一个octave中mediumlevel层的（h,w）邻域26点，存入points
void Get26points(CvMat** DoGMat,int mediumlevel,double* points,int h,int w);
//确定数组points的最后一点是否是极值点
bool IsMaxorMin(double* points,int num);
//计算图像image的(x,y)的梯度
double CalculateMagnitude(IplImage *image,int x,int y);
//计算图像image的(x,y)的角度
double CalculateTheta(IplImage *image,int x,int y);
//求gaussian-weighted的权重
double GaussianWeighed(double deltax,double deltay,double sigma);
//加入直方图
void AddtoOritationHistogram(CvMat* oritationHis,int dimention,double theta,double magnitude,double gaussian);
//生成算子
void FormDescriptor(IplImage* image,int level,int x,int y,double sigma,double scale_k,CvSeq* decriptorseq,int oc);
//生成图像的一个算子
CvMat* DescriptorinImage(IplImage* image,int arraysize,int x,int y,int level);
//找到主方向，返回bin number
int FindDominatOritation(CvMat* his);
//将梯度加权加入相邻descriptor
void AddtoHistbyweight(CvMat* hist,double xbin,double ybin,double obin,double mag,int arraysize);
//生成包含坐标的descriptor
CvMat* Mattodes(CvMat* mat,int x,int y,int level,int dim);
//对方向直方图进行平滑
void SmoothHist(CvMat* hist,int n);
//归一化descriptor
void NormalizeDescriptor(CvMat* descriptor,int dim);
//打开图片，并生成初始image
IplImage* InitImage(double sigma, IplImage* image);

//创建octave
Octave CreateOctave(int level);
//以image为最底层生成oc，初始sigma，每一级k为scale_k
Octave FormOctave(Octave oc,IplImage *image,int level,double sigma,double scale_k);
//由srcoc生成指定level的DOG,level层-(level-1)层，不用减1
CvMat* DOG(Octave srcoc,int level,int rows,int cols);
//释放oc
void ReleaseOctave(Octave oc);
//确定最终极值点
void FindDetailExtremum(CvMat** DoGMat,CvSeq* pointseq,CvSeq* featseq);

void SIFT(CvSeq* decriptorseq, IplImage *image)
{
	IplImage* srcimage = InitImage(SIFT_SIGMA, image);
	IplImage* firstimage = cvCloneImage(srcimage);

	Octave firstoctave = CreateOctave(SIFT_PEROCTAVE+3);
	double scale_k = pow(2,1.0/(SIFT_PEROCTAVE));

	int s;
	double sigma_oc;
	for(s=0;s<SIFT_OCTAVE_NUM;s++)
	{
		sigma_oc = SIFT_SIGMA*pow(2.0,s);
		firstoctave = FormOctave(firstoctave,firstimage,SIFT_PEROCTAVE+3,sigma_oc,scale_k);

		//求DOG矩阵
		CvMat* DoGMat[SIFT_PEROCTAVE+2];
		int i;
		for(i=1;i<SIFT_PEROCTAVE+3;i++)
		{
			DoGMat[i-1] = DOG(firstoctave,i,firstoctave.scales[0]->height,firstoctave.scales[0]->width);
		}

		CvMemStorage* pointstorage = cvCreateMemStorage(0);
		CvSeq* pointseq = cvCreateSeq(0,sizeof(CvSeq),sizeof(Feat),pointstorage);
		CvMemStorage* featstorage = cvCreateMemStorage(0);
		CvSeq* featseq = cvCreateSeq(0,sizeof(CvSeq),sizeof(Feat),featstorage);

		FindCandidatePoint(DoGMat,pointseq);

		int couter1 = pointseq->total;
		int	couter2 = featseq->total;

		FindDetailExtremum(DoGMat,pointseq,featseq);

		int	couter3 = featseq->total;

		Feat* point = (Feat*)malloc(sizeof(Feat));

		for(i=0;i<couter3;i++)
		{
			cvSeqPopFront(featseq,point);
			//point = (Feat*)cvGetSeqElem(featseq,i);
			FormDescriptor(firstoctave.scales[point->level],point->level,point->x,point->y,1.6,scale_k,decriptorseq,s);
		}
		free(point);
		cvClearSeq(featseq);

		firstimage = cvCreateImage(cvSize(firstoctave.scales[SIFT_PEROCTAVE]->width/2,firstoctave.scales[SIFT_PEROCTAVE]->height/2),firstoctave.scales[SIFT_PEROCTAVE]->depth,firstoctave.scales[SIFT_PEROCTAVE]->nChannels);
		cvResize(firstoctave.scales[SIFT_PEROCTAVE],firstimage);

		cvReleaseMemStorage(&pointstorage);
		cvReleaseMemStorage(&featstorage);	
		for(i=0;i<SIFT_PEROCTAVE+2;i++)
		{
			cvReleaseMat(&(DoGMat[i]));
		}
	}

	cvReleaseImage(&srcimage);
	cvReleaseImage(&firstimage);     //释放空间
	ReleaseOctave(firstoctave);
}

IplImage* InitImage(double sigma, IplImage* srcimage)
{
	IplImage *destimage = 0, *firstimage = 0;
	CvSize s_destimage;
	double sig_diff;
	
	//得到两倍大小图像
	s_destimage.height = 2*srcimage->height;
	s_destimage.width = 2*srcimage->width;
	destimage = cvCreateImage(s_destimage,srcimage->depth,srcimage->nChannels);   
	cvResize(srcimage,destimage);
	sig_diff = sqrt( sigma * sigma - SIFT_INIT_SIGMA * SIFT_INIT_SIGMA * 4 );

	//对destimage进行高斯滤波，生成firstimage
	firstimage = cvCloneImage(destimage);    
	cvSmooth( destimage, firstimage, CV_GAUSSIAN, 0, 0, sig_diff);

	cvReleaseImage(&destimage);
	return firstimage;
}

void FindCandidatePoint(CvMat** DoGMat,CvSeq* pointseq)
{
	int kinoc=0,m=0,n=0;

	for(kinoc=1;kinoc<SIFT_PEROCTAVE+1;kinoc++)
	{																			//边缘点未检测极值点
		for(m=SIFT_BORDER;m<DoGMat[kinoc]->rows-SIFT_BORDER;m++)
		{
			for(n=SIFT_BORDER;n<DoGMat[kinoc]->cols-SIFT_BORDER;n++)
			{
				double points[9] = {0};
				double points2[27] = {0};
				Get8points(DoGMat[kinoc],points,m,n);
				if(IsMaxorMin(points,9))
				{
					Get26points(DoGMat,kinoc,points2,m,n);
					if(IsMaxorMin(points2,27))
					{
						Feat f;
						f.x = n;
						f.y = m;
						f.level = kinoc;
						cvSeqPush(pointseq,&f);
					}
				}	
			}
		}
	}
	return;
}

void Get8points(CvMat* mat,double* points,int h,int w)
{
	int i,j;
	for(i=-1;i<2;i++)
	{
		for(j=-1;j<2;j++)
		{
			points[i*3+j+4] = cvmGet(mat,h+i,w+j);
		}
	}

	double temp = points[8];
	points[8] = points[4];
	points[4] = temp;
	return;
}

void Get26points(CvMat** DoGMat,int mediumlevel,double* points,int h,int w)
{
	int i,j,k;
	for(k=-1;k<2;k++)
	{
		for(i=-1;i<2;i++)
		{
			for(j=-1;j<2;j++)
			{
				points[9*k+i*3+j+13] = cvmGet(DoGMat[mediumlevel+k],h+i,w+j);
			}
		}
	}
	double temp = points[26];
	points[26] = points[13];
	points[13] = temp;
	return;
	
}

bool IsMaxorMin(double* points,int num)
{
	int i=0,j=0;
	double med = points[num-1];
	while(med > points[i] && i<num-1) i++;
	while(med < points[j] && j<num-1) j++;
	if(i == (num-1) || j == (num-1)) return true;
	else return false;
}



double CalculateMagnitude(IplImage *image,int x,int y)
{
	double temp1=0,temp2=0;
	if(x>=1 && y>=1 && x<(image->width-1) && y<(image->height-1))
	{
	temp1 = ((uchar*)(image->imageData+image->widthStep*y))[x+1] - ((uchar*)(image->imageData+image->widthStep*y))[x-1];
	temp1 = pow(temp1,2);
	temp2 = ((uchar*)(image->imageData+image->widthStep*(y+1)))[x] - ((uchar*)(image->imageData+image->widthStep*(y-1)))[x];
	temp2 = pow(temp2,2);
	}
	return sqrt(temp1+temp2);
}

double CalculateTheta(IplImage *image,int x,int y)
{
	double temp1,temp2,theta=0;
	if(x>=1 && y>=1 && x<(image->width-1) && y<(image->height-1))
	{
	temp1 = ((uchar*)(image->imageData+image->widthStep*y))[x+1] - ((uchar*)(image->imageData+image->widthStep*y))[x-1];
	temp2 = ((uchar*)(image->imageData+image->widthStep*(y+1)))[x] - ((uchar*)(image->imageData+image->widthStep*(y-1)))[x];
	theta = cvFastArctan((float)temp2,(float)temp1);
	}
	return theta;
}


void AddtoOritationHistogram(CvMat* oritationHis,int dimention,double theta,double magnitude,double gaussian)
{
	int binnumber;
	double temp;
	binnumber = cvFloor(theta*dimention/360);
	temp = cvmGet(oritationHis,0,binnumber);
	if(gaussian < 0)
	{
		temp += magnitude;
		cvmSet(oritationHis,0,binnumber,temp);
	}
	else
	{
		temp += magnitude*gaussian;
		cvmSet(oritationHis,0,binnumber,temp);
	}
	return;
}

void SmoothHist(CvMat* hist,int n)
{
	double prev, tmp1, tmp2, value;
	double h0 = cvmGet(hist,0,0);
	int i;

	prev = cvmGet(hist,0,n-1);
	for( i = 0; i < n; i++ )
	{
		tmp1 = cvmGet(hist,0,i);
		if(i == n-1 ) tmp2 = h0;
		else tmp2 = cvmGet(hist,0,i+1);
		value = 0.25 * prev + 0.5 * tmp1 + 
			0.25 * tmp2;
		cvmSet(hist,0,i,value);
		prev = tmp1;
	}

	return;
}

int FindDominatOritation(CvMat* his)
{
	int i,num=0;
	double temp = cvmGet(his,0,0);;
	for(i=1;i<his->cols;i++)
	{
		if(temp < cvmGet(his,0,i))
		{
			temp = cvmGet(his,0,i);
			num = i;
		}
	}
	return num;
}

double GaussianWeighed(double deltax,double deltay,double sigma)
{
	double a = pow(double(deltax),2)+pow(double(deltay),2);
	double b = pow(sigma,2);
	a = -1*a/2/b;
	double gaussian = exp(a);
	//double gaussian = a/2/CV_PI/b;
	return gaussian;
}


void AddtoHistbyweight(CvMat* hist,double xbin,double ybin,double obin,double mag,int arraysize)
{
	double d_x, d_y, d_o, v_x, v_y, v_o, temp;
	int x0, y0, o0, xb, yb, ob, x, y, o;

	x0 = cvFloor( xbin );
	y0 = cvFloor( ybin );
	o0 = cvFloor( obin );
	d_x = xbin - x0;
	d_y = ybin - y0;
	d_o = obin - o0;

	/*
	The entry is distributed into up to 8 bins.  Each entry into a bin
	is multiplied by a weight of 1 - d for each dimension, where d is the
	distance from the center value of the bin measured in bin units.
	*/
	for( x = 0; x <= 1; x++ )
	{
		xb = x0 + x;
		if( xb >= 0  &&  xb < arraysize )
		{
			v_x = mag * ( ( x == 0 )? 1.0 - d_x : d_x );
			for( y = 0; y <= 1; y++ )
			{
				yb = y0 + y;
				if( yb >= 0  &&  yb < arraysize )
				{
					v_y = v_x * ( ( y == 0 )? 1.0 - d_y : d_y );
					for( o = 0; o <= 1; o++ )
					{
						ob = ( o0 + o ) % 8;
						v_o = v_y * ( ( o == 0 )? 1.0 - d_o : d_o );
						temp = cvmGet(hist,0,xb*8*arraysize+yb*8+ob);
						temp += v_o;
						cvmSet(hist,0,xb*8*arraysize+yb*8+ob,temp);              
					}
				}
			}
		}
	}
	return;
}

CvMat* DescriptorinImage(IplImage* image,int arraysize,int x,int y,int level,double domangle)
{
	int i,m,n,c=0;
	double temp,sum=0,mag,the,gaussian;
	double x_rot,y_rot,cost,sint,xbin,ybin,obin;
	int dim = arraysize*arraysize*8;
	int size = cvFloor(sqrt(2.0)*2*(arraysize+1));
	cost = cos(domangle/180*CV_PI);
	sint = sin(domangle/180*CV_PI);
	CvMat* descriptor = cvCreateMat(1,dim,CV_32FC1);
	for(i=0;i<dim;i++) cvmSet(descriptor,0,i,0);


	for(m=-size;m<=size;m+=4)                 //范围到底取多少？？
	{
		for(n=-size;n<=size;n+=4)
		{
			x_rot = (n*cost - m*sint)/4.0;
			y_rot = (n*sint + m*cost)/4.0;
			xbin = x_rot+arraysize/2-0.5;           //是否要减0.5
			ybin = y_rot+arraysize/2-0.5;

			if(xbin>-1 && xbin<arraysize && ybin>-1 && ybin<arraysize)
			{
				mag = CalculateMagnitude(image,x+m,y+n);
				gaussian = GaussianWeighed(x_rot*4,y_rot*4,6*arraysize);           
				the = CalculateTheta(image,x+m,y+n);
				the -= domangle;
				while(the<0.0) the+=360;
				while(the>=360) the-=360;
				obin =  the*8/360;
				AddtoHistbyweight(descriptor,xbin,ybin,obin,mag*gaussian,arraysize);
			}
			
		}
	}
	NormalizeDescriptor(descriptor,dim);
	for(i=0;i<dim;i++)
	{
		temp = cvmGet(descriptor,0,i);
		if(cvmGet(descriptor,0,i) > SIFT_DES_NORM_THR) cvmSet(descriptor,0,i,SIFT_DES_NORM_THR);
	}
	NormalizeDescriptor(descriptor,dim);
	return descriptor;
}

void NormalizeDescriptor(CvMat* descriptor,int dim)
{
	int i;
	double temp,sum = 0;
	for(i=0;i<dim;i++)                             
	{
		temp = cvmGet(descriptor,0,i);
		sum += temp*temp;
	}
	sum = 1.0/sqrt(sum);
	for(i=0;i<dim;i++)
	{
		temp = cvmGet(descriptor,0,i);
		temp = temp*sum;
		cvmSet(descriptor,0,i,temp);
	}
	return;
}


void FormDescriptor(IplImage* image,int level,int x,int y,double sigma,double scale_k,CvSeq* decriptorseq,int oc)
{	
	int i,j;
	CvMat* his36 = cvCreateMat(1,36,CV_32FC1);
	for(i=0;i<36;i++)cvmSet(his36,0,i,0);
	double sigmasize = 1.5*sigma*pow(scale_k,level);
	int size = cvRound(3.0*sigmasize);
	for(i = -1*size;i<=size;i++)
	{
		for(j = -1*size;j<=size;j++)
		{
		double mag = CalculateMagnitude(image,x+j,y+i);
		double the = CalculateTheta(image,x+j,y+i);
		double gaussian = GaussianWeighed(i,j,sigmasize);
		AddtoOritationHistogram(his36,36,the,mag,gaussian);
		}
	}
	SmoothHist(his36,36);                              //平滑
	SmoothHist(his36,36);
	SmoothHist(his36,36);
	int num = FindDominatOritation(his36);
	double q = cvmGet(his36,0,num);
	int dim = SIFT_ARRAY_DIM*SIFT_ARRAY_DIM*SIFT_DESCRIPTOR_DIM;

	int l,r;
	double bin,lv,rv,cv;
	for(i=0;i<36;i++)
	{
		l = ( i == 0 )? 36 - 1 : i-1;
		r = ( i + 1 ) % 36;
		lv = cvmGet(his36,0,l);
		rv = cvmGet(his36,0,r);
		cv = cvmGet(his36,0,i);

		if( cv > lv  &&  cv > rv  &&  cv >= 0.8*q )
		{
			bin = i + interp_hist_peak( lv, cv, rv );
			bin = ( bin < 0 )? 36 + bin : ( bin >= 36 )? bin - 36 : bin;
			CvMat* mat = DescriptorinImage(image,SIFT_ARRAY_DIM,x,y,level,bin*10);
			CvMat* des = Mattodes(mat,x,y,level+2*oc,dim);
			cvSeqPush(decriptorseq,des);
			cvReleaseMat(&mat);
		}
	}
	cvReleaseMat(&his36);
	return;
}

CvMat* Mattodes(CvMat* mat,int x,int y,int level,int dim)
{
	int i;
	CvMat* des = cvCreateMat(1,dim+3,CV_32FC1);
	cvmSet(des,0,0,double(x));
	cvmSet(des,0,1,double(y));
	cvmSet(des,0,2,double(level));
	for(i=3;i<dim+3;i++)
	{
		cvmSet(des,0,i,cvmGet(mat,0,i-3));
	}
	return des;
}

//Octave相关函数
Octave CreateOctave(int level)
{
	Octave octave;
	octave.level = level;
	int i;
	for(i=0;i<level;i++) {octave.scales[i] = 0;}
	return octave;
}


Octave FormOctave(Octave oc,IplImage *image,int level,double sigma,double scale_k)
{
	double diff_sigma,scale_sigma,scale_prev;
	int i;
	oc.level = level;
	oc.scales[0] = image;

	for(i=1;i<level;i++)
	{
		scale_prev = sigma*pow(scale_k,i-1);
		scale_sigma = scale_prev*scale_k;
		diff_sigma = sqrt( scale_sigma * scale_sigma - scale_prev * scale_prev );
		oc.scales[i] = cvCloneImage(image);
		cvSmooth(oc.scales[i-1],oc.scales[i],CV_GAUSSIAN,0,0,diff_sigma);           
	}
	return oc;
}

CvMat* DOG(Octave srcoc,int level,int rows,int cols)
{
	CvMat* dogmat = cvCreateMat(rows,cols,CV_32FC1);
	int i,j;
	for(i=0;i<rows;i++)
	{
		for(j=0;j<cols;j++)
		{
			int a = (((uchar*)(srcoc.scales[level-1]->imageData+srcoc.scales[level-1]->widthStep*i)))[j];
			int b = (((uchar*)(srcoc.scales[level]->imageData+srcoc.scales[level]->widthStep*i)))[j];
			cvmSet(dogmat,i,j,double(b-a));
		}
	}

	return dogmat;
}

void ReleaseOctave(Octave oc)
{
	int i =0;
	for(i=0;i<oc.level;i++)
	{
		cvReleaseImage(&oc.scales[i]);
	}
	return;
}

void FindDetailExtremum(CvMat** DoGMat,CvSeq* pointseq,CvSeq* featseq)
{
	Feat* point = (Feat*)malloc(sizeof(Feat));
	int i,j=0;
	int num = pointseq->total;
	double points[27] = {0},dx,dy,ds;
	CvMat* derivative = cvCreateMat(3,3,CV_32FC1);
	CvMat* Hmatrix = cvCreateMat(2,2,CV_32FC1);
	CvMat* div = cvCreateMat(3,1,CV_32FC1);
	CvMat* destdiv = cvCreateMat(3,3,CV_32FC1);
	CvMat* deltaX = cvCreateMat(3,1,CV_32FC1);   

	for(i=0;i<num;i++)
	{
		cvSeqPopFront(pointseq,point);
             //deltaX未取负
		int x = point->x;
		int y = point->y;
		int level = point->level;

		while(j<SIFT_MAX_INTERP_STEPS)
		{
			double pvalue = cvmGet(DoGMat[level],y,x);
			Get26points(DoGMat,level,points,y,x);

			//DXX,DYY    
			double value = points[4]+points[22]-pvalue*2;				
			cvmSet(derivative,2,2,value);
			value = points[12]+points[14]-pvalue*2;
			cvmSet(derivative,0,0,value);
			cvmSet(Hmatrix,0,0,value);
			value = points[10]+points[16]-pvalue*2;
			cvmSet(derivative,1,1,value);
			cvmSet(Hmatrix,1,1,value);

			//DXY,DYX
			value = points[9]+points[17]-points[11]-points[15];
			value = value/4;
			cvmSet(derivative,0,1,value);
			cvmSet(Hmatrix,0,1,value);
			cvmSet(Hmatrix,1,0,value);
			cvmSet(derivative,1,0,value);
			value = points[23]+points[3]-points[21]-points[5];
			value = value/4;
			cvmSet(derivative,0,2,value);
			cvmSet(derivative,2,0,value);
			value = points[25]+points[1]-points[19]-points[7];
			value = value/4;
			cvmSet(derivative,1,2,value);
			cvmSet(derivative,2,1,value);

			value = (points[14]-points[12])/2.0;   
			cvmSet(div,0,0,value);
			value = (points[16]-points[10])/2.0;   
			cvmSet(div,1,0,value);
			value = (points[22]-points[4])/2.0;   
			cvmSet(div,2,0,value);

			cvInvert( derivative, destdiv, CV_SVD );

			cvMatMul(destdiv,div,deltaX);
			dx = cvmGet(deltaX,0,0);
			dy = cvmGet(deltaX,1,0);
			ds = cvmGet(deltaX,2,0);

			if(fabs(dx) < 0.5 || fabs(dy) < 0.5 || fabs(ds) < 0.5)
			{
				CvMat* transdiv = cvCreateMat(1,3,CV_32FC1);
				cvTranspose(div,transdiv);
				CvMat* mul = cvCreateMat(1,1,CV_32FC1);
				cvMatMul(transdiv,deltaX,mul);
	
				double Dvalue = pvalue-cvDet(mul)*0.5;
				double Rvalue = cvTrace(Hmatrix).val[0]*cvTrace(Hmatrix).val[0]/cvDet(Hmatrix);          
				if(fabs(Dvalue) < SIFT_DVALUE_THR*256 || cvDet(Hmatrix)<=0 || Rvalue > SIFT_RVALUE_THR) {}
				else
				{
					Feat fpoint;
					fpoint.x = x;
					fpoint.y = y;
					fpoint.level = level;
					cvSeqPush(featseq,&fpoint);
				}                                                          
				cvReleaseMat(&transdiv);
				cvReleaseMat(&mul);
				break;
			}

			x += cvRound( dx );
			y += cvRound( dy );
			level += cvRound( ds );
			if( level<1 || level>SIFT_PEROCTAVE || x<5 || y<5 || x >= DoGMat[level]->cols-5 || y >= DoGMat[level]->height-5 )break;
			j++;
		}
		j = 0;
	}
	free(point);
	cvReleaseMat(&derivative);
	cvReleaseMat(&Hmatrix);
	cvReleaseMat(&div);
	cvReleaseMat(&destdiv);
	cvReleaseMat(&deltaX);
	return;
}

