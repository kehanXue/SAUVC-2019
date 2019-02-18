#ifndef VISIONCLASS_H
#define VISIONCLASS_H

//前置摄像机是否接入
//#define SAUV_HAS_FRONT_CAMERA 0
//下视摄像机是否接入
//#define SAUV_HAS_BOTTOM_CAMERA 0

#include "include/PvApi.h"
#include "camclass.h"
#include <ctime>
#include <sys/time.h>
#include <QObject>
#include <QTimer>
#include <vector>
#include <QDebug>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "global.h"

#include <unistd.h>
#include <string>
#include <fstream>

using namespace cv;
using std::vector;
using std::string;
using std::fstream;

//vision_COLORTHRESH

/*
#define     m2_RB_max  76
#define     m2_RB_min   0
#define     m2_RH_max  29
#define     m2_RH_min 127
*/

#define     m2_RB_max  52
#define     m2_RB_min   0
#define     m2_RH_max  30
#define     m2_RH_min 160

#define     m3_RB_max  80
#define     m3_RB_min   0
#define     m3_RH_max  30
#define     m3_RH_min 140

#define     m3G_RB_max 146
#define     m3G_RB_min   0
#define     m3G_RH_max 106
#define     m3G_RH_min  34

#define     m4_RB_max 171
#define     m4_RB_min  47
#define     m4_RH_max   0
#define     m4_RH_min   0

//管理相机
class visionClass: public camClass
{
    Q_OBJECT

public:
    visionClass();
    ~visionClass();


    enum COLOR
    {
        GREEN=1,
        RED=2,
        BLACK=3,
        ANGLE_G=4,
        ANGLE_R=5
    };

    //主控通信数据结构体
    typedef struct
    {
        bool m1_runningState,
             m1_gateFound,
             m2_runningState,
             m3_runningState,
             m3_acquireFinish,
             m4_runningState;

        int m1_centerdx,
            m1_leftdx,
            m1_rightdx,
            m1_angledx,
            m2_drum_dx,
            m2_drum_dy,
            m3_drum_dx,
            m3_drum_dy,
            m3_ball_dx,
            m3_ball_dy,
            m3_ball_acquire,
            m3_rotate_cw_angle,
            m3_green_area,
            m4_flare_dx;
        time_t t_now;
    }visionData;

    //主控索要数据函数
    visionData getData();



    bool isColor(cv::Mat &frame, COLOR color);
private:

    bool run_flag;

    bool m1_start,
         m2_start,
         m3_start,
         m4_start,
         save_start;

    //主控通信数据
    visionData data;

    QTimer *timer, *savetimer_f,*savetimer_b;

    int savenum_f=0;

    int savenum_b=0;

    int camnum=0;

    float deepValue;

    fstream v_fout;

    Mat img_front;
    Mat img_bottom;



signals:

    void imageShow(Mat *,const char *);

    void createWindow(const char *);

    void destroyWindow();

    void createBar(const char *,const char *, int *,int count=255);

    //图像界面
    void frameGottenF(Mat *imgSrc);

    void cameraStartedF(bool ok);

    void cameraStoppedF(bool ok);

    void frameGottenB(Mat *imgSrc);

    void cameraStartedB(bool ok);

    void cameraStoppedB(bool ok);

    void cameraStartedF_m(bool ok);

    void cameraStartedB_m(bool ok);

public slots:

    void onCreateTimer();

    void camInitialize();

    void onGetHandclosed(bool ok);

    void onGodownFinished(bool ok);

    //任务一：过门
    void missionGate();

    //任务二：抛球
    void missionDrop();

    //任务三：抓球
    void missionAcquire();

    //任务四：撞杆
    void missionFlare();

    //图像处理程序启动函数
    void mission_Start(TASKS_ID ms);

    //图像处理程序终止函数
    void mission_Off(TASKS_ID ms);

    void setDeep(float value);

    //任务相机配置函数
    void GateCamSetting();

    void DropCamSetting();

    void FlareCamSetting();

    void AcquireCamSetting();


    /******************************************************************************/

    //存图    
    void visionsave_photo_f(bool ok);

    void visionsave_photo_b(bool ok);


    //信息存储
    void visionsave_debug(bool ok);

    /*******************************************************************************/

    //图像界面    
    void qpacketSize(int value);

    void qexposureMode(int value);

    void qwhiteBalanceMode(int value);

    void qexposureMax(int value);

    void qwhiteBalance_Red(float value);

    void qwhiteBalance_Blue(float value);

    void qexposureAutoTarget(int value);

    void qcameraNum(int value);

    void onStartCameraF(int value);

    void onStopCameraF(int value);

    void onGetFrameF(Mat* imgSrc);

    void onStartCameraB(int value);

    void onStopCameraB(int value);

    void onGetFrameB(Mat* imgSrc);

    void stop_Thread();

private:

    /**********************************missionGate**********************************/


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
    }m1_MATCH;

    //用于双边阈值颜色识别的结构体
    typedef struct
    {
        int H_max;
        int H_min;
        int B_max;
        int B_min;
        int R_max;
        int R_min;
    }m1_COLORTHRESH;

    //用于矩形拟合的结构体
    typedef struct
    {

        int tl_x;
        int tl_y;
        int br_x;
        int br_y;
        int center_x;
        int center_y;
        int height;
        int width;
        double area;
        int areaLowThresh;
        bool valid;
        const int heightwidthratio = 3;

    }m1_RECT;


    //多组模板匹配函数
    void rectBoundary( const Mat &img_input, Mat &img_output, m1_RECT &m1_Rect, int color);

    void templatematch_vector( const Mat &img_input, Mat &img_output, const vector<Mat> &TemplVec, m1_MATCH &MatchResult);

    void colorRecognize(const Mat &img_input, Mat &img_output, m1_COLORTHRESH &ColorThresh,int color);

    vector<Mat> loadTempl(int color, float imgscale);


       /*****************************missiondrop************************************/

       //用于双边阈值颜色识别的结构体
       typedef struct
       {
           int RH_max;
           int RH_min;
           int RB_max;
           int RB_min;
           int RV_max;
           int RV_min;
       }m2_COLORTHRESH;

       //用于矩形拟合的结构体
       typedef struct
       {
           int tl_x;
           int tl_y;
           int br_x;
           int br_y;
           int center_x;
           int center_y;
           double area;
           int areaLowThresh;
           bool valid;
       }m2_RECT;

       void colorRecognize( const Mat &img_input, Mat &img_output, m2_COLORTHRESH &ColorThresh);

       void rectBoundary( const Mat &img_input, Mat &img_output, m2_RECT &m2_Rect);


       /*****************************missionacquire************************************/

             //用于双边阈值颜色识别的结构体
             typedef struct
             {
                 int RH_max;
                 int RH_min;
                 int RB_max;
                 int RB_min;
                 int RV_max;
                 int RV_min;
             }m3_COLORTHRESH;



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
                 int tl_x;
                 int tl_y;
                 int br_x;
                 int br_y;
                 int height;
                 int width;
                 double area;
                 int areaLowThresh;
                 int heightwidthratio;
                 bool valid;
                 int center_x;
                 int center_y;
             }m3_RECT;


             //双通道双边阈值颜色识别
             void colorRecognize( const Mat &img_input, Mat &img_output, m3_COLORTHRESH &ColorThresh);

             //矩形拟合
             void rectBoundary( const Mat &img_input, Mat &img_output, m3_RECT &m3_Rect);

             //多组模板匹配函数
             void templatematch_vector(const Mat &img_input, Mat &img_output, const vector<Mat> &TemplVec, m3_MATCH &MatchResult);



        /*****************************missionflare************************************/

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
            int RH_max;
            int RH_min;
            int RB_max;
            int RB_min;
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
            const int heightwidthratio = 2;
            bool valid;
        }m4_RECT;

        //多组模板匹配函数
        void templatematch_vector( const Mat &img_input, Mat &img_output, const vector<Mat> &TemplVec, m4_MATCH &MatchResult);

        //双边阈值颜色识别
        void colorRecognize( const Mat &img_input, Mat &img_output, m4_COLORTHRESH &ColorThresh);

        //矩形拟合
        void rectBoundary(const Mat &img_input, Mat &img_output, m4_RECT &m4_Rect);
};

#endif


