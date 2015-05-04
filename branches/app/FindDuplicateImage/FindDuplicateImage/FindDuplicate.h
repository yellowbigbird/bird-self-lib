#pragma once

#include <string>
#include <opencv2/core/mat.hpp>

class CFindDup
{
public:
	CFindDup();
	virtual ~CFindDup();
	
	void TestOpencv();
	void GetImageHash(const std::string& str);

	UINT GetAverageGray(cv::Mat& I);
	UINT64 GetImageHash(cv::Mat& mat, UINT grayAvg);
};

//int CompareHist(const char* imagefile1, const char* imagefile2);


