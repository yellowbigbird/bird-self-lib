#pragma once

#include <string>
#include <opencv2/core/mat.hpp>

class CFindDup
{
public:
	CFindDup();
	virtual ~CFindDup();
	
	void TestOpencv();
	UINT64 GetImageHash(const std::string& str);

	UINT GetAverageGray(cv::Mat& I);
	UINT64 GetMatHash(cv::Mat& mat, UINT grayAvg);
};

//int CompareHist(const char* imagefile1, const char* imagefile2);


