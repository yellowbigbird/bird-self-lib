#include "stdafx.h"
#include "FindDuplicate.h"

#include "config.h"
#include "FileManager.h"

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/highgui/highgui.hpp>  //compare
#include "opencv/cv.hpp"

#include <iostream>

using namespace cv;
using namespace std;
/////////
CFindDup::CFindDup()
{}

CFindDup::~CFindDup()
{}

//////////////////

void CFindDup::TestOpencv()
{
	CConfig::Get().LoadFile();
	CFileManager::Get().StartSearchTread();
	CFileManager::Get().StartSearchPath();  //why need 2 cmd

	//const CString cstr = _T("d:\\1.jpg");
	const string str0 = ("d:\\0.jpg");
	UINT64 hash0 = GetImageHash(str0);

	string str1 = ("d:\\1.jpg");
	UINT64 hash1 = GetImageHash(str1);

	//Mat image;
 //   image = imread("d:\\1.jpg", IMREAD_COLOR); // Read the file
 //   if( image.empty() ) // Check for invalid input
 //   {
 //       cout << "Could not open or find the image" << std::endl ;
 //       return ;
 //   }
 //   namedWindow( "Display window", WINDOW_AUTOSIZE ); // Create a window for display.
 //   imshow( "Display window", image ); // Show our image inside it.
    //waitKey(0); // Wait for a keystroke in the window
}

UINT64 CFindDup::GetImageHash(const std::string& str)
{
	if(str.empty() )
		return 0;

	//open image
	Mat imgSrc;
	Mat imgTemp, imgTempGray;
    imgSrc = imread(str.c_str(), IMREAD_COLOR); // Read the file
    if( imgSrc.empty() ) // Check for invalid input
    {
        cout << "Could not open or find the image" << std::endl ;
        return 0;
    }

	//resize 8*8
	resize(imgSrc, imgTemp, Size(8,8), 0, 0, CV_INTER_AREA);

	//change to gray
	cvtColor( imgTemp, imgTempGray, CV_RGB2GRAY );

	//get avg 
	const UINT grayAvg = GetAverageGray(imgTempGray);
	UINT64 imghash = GetMatHash(imgTempGray, grayAvg );

	//namedWindow( "Display_window", WINDOW_AUTOSIZE ); // Create a window for display.
    //imshow( "Display_window", imgTempGray ); // Show our image inside it.
    //waitKey(0); // Wait for a keystroke in the window
	//img.resize();
	return imghash;
}

UINT CFindDup::GetAverageGray(Mat& mat)
{
    // accept only char type matrices
    CV_Assert(mat.depth() != sizeof(uchar));     
    
	UINT valueAll = 0;
	const UINT pixelCount = mat.rows * mat.cols;
	if(pixelCount < 1) 
		return 0;
	UINT valueAvg = 0;
    const int channels = mat.channels();
    switch(channels)
    {
    case 1: 
        {
            MatIterator_<uchar> it, end; 
            for( it = mat.begin<uchar>(), end = mat.end<uchar>(); it != end; ++it){
                valueAll += *it;
			}
            break;
        }
    case 3: 
        {
            MatIterator_<Vec3b> it, end; 
            for( it = mat.begin<Vec3b>(), end = mat.end<Vec3b>(); it != end; ++it)
            {
                //(*it)[0] = table[(*it)[0]];
                //(*it)[1] = table[(*it)[1]];
                //(*it)[2] = table[(*it)[2]];
            }
        }
    }
    
	valueAvg = valueAll / pixelCount ;
    return valueAvg; 
}

UINT64 CFindDup::GetMatHash(cv::Mat& mat, UINT grayAvg)
{
	CV_Assert(mat.depth() != sizeof(uchar));     
    
	UINT64 value = 0; //must be 64
	const UINT pixelCount = mat.rows * mat.cols;
	if(pixelCount < 1) 
		return 0;
    const int channels = mat.channels();
	if(channels != 1)
		return 0;
         
	MatIterator_<uchar> it, end; 
	UINT64 idx = 0;
	for( it = mat.begin<uchar>(), end = mat.end<uchar>();
		it != end;
		++it, idx++){
		if(*it >= grayAvg){
			value += (UINT64)1 << idx;
		}
	}
      
	return value;
}

//int HistogramBins = 256;
//float HistogramRange1[2]={0,255};
//float *HistogramRange[1]={&HistogramRange1[0]};
//
//int CompareHist(const char* imagefile1, const char* imagefile2)
//{
//    IplImage *image1= cvLoadImage(imagefile1, 0);
//    IplImage *image2= cvLoadImage(imagefile2, 0);
//
//    CvHistogram *Histogram1 = cvCreateHist(1, &HistogramBins, CV_HIST_ARRAY,HistogramRange);
//    CvHistogram *Histogram2 = cvCreateHist(1, &HistogramBins, CV_HIST_ARRAY,HistogramRange);
//
//    cvCalcHist(&image1, Histogram1);
//    cvCalcHist(&image2, Histogram2);
//
//    cvNormalizeHist(Histogram1, 1);
//    cvNormalizeHist(Histogram2, 1);
//
//    // CV_COMP_CHISQR,CV_COMP_BHATTACHARYYA
//    printf("CV_COMP_CHISQR : %.4f\n", cvCompareHist(Histogram1, Histogram2, CV_COMP_CHISQR));
//    printf("CV_COMP_BHATTACHARYYA : %.4f\n", cvCompareHist(Histogram1, Histogram2, CV_COMP_BHATTACHARYYA));
//
//
//    // CV_COMP_CORREL, CV_COMP_INTERSECT
//    printf("CV_COMP_CORREL : %.4f\n", cvCompareHist(Histogram1, Histogram2, CV_COMP_CORREL));
//    printf("CV_COMP_INTERSECT : %.4f\n", cvCompareHist(Histogram1, Histogram2, CV_COMP_INTERSECT));
//
//    cvReleaseImage(&image1);
//    cvReleaseImage(&image2);
//    cvReleaseHist(&Histogram1);
//    cvReleaseHist(&Histogram2);
//    return 0;
//}
//////////////
