#ifndef MISSIONFLARE
#define MISSIONFLARE

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <cmath>

using namespace std;
using namespace cv;

//用于反馈模板匹配结果的结构体
typedef struct
{
    int tl_x;
    int tl_y;
    int br_x;
    int br_y;
    double confidence;
    bool valid;
    double minVal;
    double maxVal;
    Point minLoc;
    Point maxLoc;
    Point matchLoc;
}m4_MATCH;

//用于双边阈值颜色识别的结构体
typedef struct
{
    int YH_max;
    int YH_min;
    int YB_max;
    int YB_min;
}m4_COLORTHRESH;

//用于矩形拟合的结构体
typedef struct
{
    int tl_x;
    int tl_y;
    int br_x;
    int br_y;
    int height;
    int width;
    double area;
    int areaLowThresh;
    const int heightwidthratio =3;
    bool valid;
}m4_RECT;

//多组模板匹配函数
void templatematch_vector( const Mat &img_input, Mat &img_output, const vector<Mat> &TemplVec, m4_MATCH &MatchResult);

//双边阈值颜色识别
void colorRecognize( const Mat &img_input, Mat &img_output, m4_COLORTHRESH &ColorThresh);

//矩形拟合
void rectBoundary(const Mat &img_input, Mat &img_output, m4_RECT &m4_Rect);

#endif // MISSIONFLAREs
