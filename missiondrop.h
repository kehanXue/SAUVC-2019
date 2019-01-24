#ifndef MISSIONDROP
#define MISSIONDROP

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <cmath>

using namespace std;
using namespace cv;


//查找表函数
Mat applyLookUp( const Mat& image, const Mat& lookup);

//计算直方图
Mat getHistogram( const Mat &img_input);

//拉伸直方图
Mat strech( const Mat &img_input, const Mat &hist, int minValue = 100, int start_offset = 5);

//画出直方图
Mat getHistogramImage( const Mat &hist);



#endif
