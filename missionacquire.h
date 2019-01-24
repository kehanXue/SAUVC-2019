#ifndef MISSIONACQUIRE
#define MISSIONACQUIRE

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <cmath>

using namespace std;
using namespace cv;



//查找表函数
Mat m3_applyLookUp(const Mat& image, const Mat& lookup);

//计算直方图
Mat m3_getHistogram(const Mat &img_input);

//拉伸直方图
Mat m3_strech(const Mat &img_input, const Mat &hist, int minValue = 100, int start_offset = 5);

//画出直方图
Mat m3_getHistogramImage(const Mat &hist);


bool m3_testball = 0;

bool m3_downfinished =0;

#endif
