#ifndef MISSIONUGATE
#define MISSIONUGATE

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <cmath>

using namespace std;
using namespace cv;

typedef struct
{

    int templ_h_center;
    int templ_v1;
    int templ_v2;
    int color_h_center;
    int color_v1;
    int color_v2;
}m3_Locate;

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
}m3_MATCH;



//用于矩形拟合的结构体
typedef struct
{
    Rect rect_org;
    int areaLowThresh;
   float widthheightratio;
    bool valid;
}m3_RECT;

typedef struct
{
    int hor_Rect_num;
    int vec_Rect_num;
    vector<Rect> hor;
    vector<Rect>vec;

}m3_RectJudge;

//多组模板匹配函数
void m3_templatematch_vector( const Mat &img_input, Mat &img_output, const vector<Mat> &TemplVec, m3_MATCH &MatchResult);

//矩形拟合
void m3_rectBoundary(const Mat &img_input, Mat &img_output, m3_RectJudge &m3_Boundary_out, float m3_imgscale);

#endif // MISSIONFLAREs
