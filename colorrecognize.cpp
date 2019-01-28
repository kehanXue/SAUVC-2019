#include "visionclass.h"
#include <unistd.h>

Mat m1_img_rgb,
m1_img_hsv,
m1_img_temp,
m1_img_ranged;

vector<Mat> m1_channels_RGB,
m1_channels_HSV;

//m1双通道双边阈值颜色识别
void visionClass::colorRecognize(const Mat &img_input, Mat &img_output, m1_COLORTHRESH &ColorThresh,int color)
{
    static Mat img_rgb,
    img_hsv,
    img_H,
    img_H_ranged,
    img_temp1,
    img_temp2,
    img_B,
    img_R,
    img_B_ranged,
    img_R_ranged,
    img_ranged;

    vector<Mat> channels_RGB;
    vector<Mat> channels_HSV;
    img_rgb = img_input.clone();
    cvtColor( img_rgb, img_hsv, COLOR_BGR2HSV);
    split( img_rgb, channels_RGB);
    split( img_hsv, channels_HSV);
    img_H = channels_HSV.at(1);
    img_B = channels_RGB.at(0);
    img_R = channels_RGB.at(2);

    //鉴于H通道是个环形的，需要考虑转一圈的情形
    if( ColorThresh.H_max >= ColorThresh.H_min)
    {
        inRange( img_H, Scalar( ColorThresh.H_min, 0, 0), Scalar( ColorThresh.H_max, 0, 0), img_H_ranged);
    }
    else
    {
        inRange( img_H, Scalar( ColorThresh.H_min, 0, 0), Scalar( 255, 0, 0), img_temp1);
        inRange( img_H, Scalar( 0, 0, 0), Scalar( ColorThresh.H_max, 0, 0), img_temp2);
        bitwise_or( img_temp1, img_temp2, img_H_ranged);
    }

    if( ColorThresh.B_max >= ColorThresh.B_min)
    {
        inRange( img_B, Scalar( ColorThresh.B_min, 0, 0), Scalar( ColorThresh.B_max, 0, 0), img_B_ranged);
    }
    else
    {
        inRange( img_B, Scalar( ColorThresh.B_min, 0, 0), Scalar( 255, 0, 0), img_temp1);
        inRange( img_B, Scalar( 0, 0, 0), Scalar( ColorThresh.B_max, 0, 0), img_temp2);
        bitwise_or( img_temp1, img_temp2, img_B_ranged);
    }

    if( ColorThresh.R_max >= ColorThresh.R_min)
    {
        inRange( img_R, Scalar( ColorThresh.R_min, 0, 0), Scalar( ColorThresh.R_max, 0, 0), img_R_ranged);
    }
    else
    {
        inRange( img_R, Scalar( ColorThresh.R_min, 0, 0), Scalar( 255, 0, 0), img_temp1);
        inRange( img_R, Scalar( 0, 0, 0), Scalar( ColorThresh.R_max, 0, 0), img_temp2);
        bitwise_or( img_temp1, img_temp2, img_R_ranged);
    }


    if (color==Qt::green)
    {
     emit imageShow(&img_B_ranged,"GB_ranged");
     emit imageShow(&img_H_ranged,"GH_ranged");
     emit imageShow(&img_R_ranged,"GR_ranged");

     bitwise_and( img_H_ranged, img_B_ranged, img_ranged);
     bitwise_and( img_R_ranged, img_ranged, img_ranged);
     emit imageShow(&img_ranged,"Ginranged");
    }

    if (color==Qt::red)
    {
    emit imageShow(&img_B_ranged,"RB_ranged");
    emit imageShow(&img_H_ranged,"RH_ranged");
    emit imageShow(&img_R_ranged,"RR_ranged");

    bitwise_and( img_H_ranged, img_B_ranged, img_ranged);
    bitwise_and( img_R_ranged, img_ranged, img_ranged);
    emit imshow("Rinranged",img_ranged);
    }

    //结果图输出
    img_ranged.copyTo( img_output);

    return;
    }

//m2双通道双边阈值颜色识别
void visionClass::colorRecognize( const Mat &img_input, Mat &img_output, m2_COLORTHRESH &ColorThresh)
{
    Mat img_rgb,
    img_hsv,
    img_H,
    img_H_ranged,
    img_temp1,
    img_temp2,
    img_B,
    img_B_ranged,
    img_V,
    img_ranged;

    vector<Mat> channels_RGB;
    vector<Mat> channels_HSV;
    img_rgb = img_input.clone();
    cvtColor( img_rgb, img_hsv, COLOR_BGR2HSV);
    split( img_rgb, channels_RGB);
    split( img_hsv, channels_HSV);
    img_H = channels_HSV.at(0);
    img_B = channels_RGB.at(1);
    //img_V=  channels_HSV.at(2);

    //鉴于H通道是个环形的，需要考虑转一圈的情形
    if( ColorThresh.RH_max >= ColorThresh.RH_min)
    {
        inRange( img_H, Scalar( ColorThresh.RH_min, 0, 0), Scalar( ColorThresh.RH_max, 0, 0), img_H_ranged);
    }
    else
    {
        inRange( img_H, Scalar( ColorThresh.RH_min, 0, 0), Scalar( 180, 0, 0), img_temp1);
        inRange( img_H, Scalar( 0, 0, 0), Scalar( ColorThresh.RH_max, 0, 0), img_temp2);
        bitwise_or( img_temp1, img_temp2, img_H_ranged);
    }

    //再考虑B通道的阈值
    inRange( img_B, Scalar( ColorThresh.RB_min, 0, 0), Scalar( ColorThresh.RB_max, 0, 0), img_B_ranged);

    //两幅图像膨胀之后再按位取与
    dilate( img_H_ranged, img_H_ranged, 5);
    dilate( img_B_ranged, img_B_ranged, 5);
    bitwise_and( img_H_ranged, img_B_ranged, img_ranged);

    imshow("AAAAAA", img_ranged);

    //结果图输出
    img_ranged.copyTo( img_output);

    return;
}

//m3双通道双边阈值颜色识别
void visionClass::colorRecognize( const Mat &img_input, Mat &img_output, m3_COLORTHRESH &ColorThresh)
{
    Mat img_rgb,
    img_hsv,
    img_H,
    img_H_ranged,
    img_temp1,
    img_temp2,
    img_B,
    img_B_ranged,
    img_ranged;

    vector<Mat> channels_RGB;
    vector<Mat> channels_HSV;
    img_rgb = img_input.clone();
    cvtColor( img_rgb, img_hsv, COLOR_BGR2HSV);
    split( img_rgb, channels_RGB);
    split( img_hsv, channels_HSV);
    img_H = channels_HSV.at(0);
    img_B = channels_RGB.at(1);

    //鉴于H通道是个环形的，需要考虑转一圈的情形
    if( ColorThresh.RH_max >= ColorThresh.RH_min)
    {
        inRange( img_H, Scalar( ColorThresh.RH_min, 0, 0), Scalar( ColorThresh.RH_max, 0, 0), img_H_ranged);
    }
    else
    {
        inRange( img_H, Scalar( ColorThresh.RH_min, 0, 0), Scalar( 180, 0, 0), img_temp1);
        inRange( img_H, Scalar( 0, 0, 0), Scalar( ColorThresh.RH_max, 0, 0), img_temp2);
        bitwise_or( img_temp1, img_temp2, img_H_ranged);
    }

    //再考虑B通道的阈值
    inRange( img_B, Scalar( ColorThresh.RB_min, 0, 0), Scalar( ColorThresh.RB_max, 0, 0), img_B_ranged);

    //两幅图像膨胀之后再按位取与
    dilate( img_H_ranged, img_H_ranged, 5);
    dilate( img_B_ranged, img_B_ranged, 5);
    bitwise_and( img_H_ranged, img_B_ranged, img_ranged);

    //emit imageShow(&img_H_ranged,"H_ranged");
    //emit imageShow(&img_B_ranged,"B_ranged");

    //结果图输出
    img_ranged.copyTo( img_output);

    return;
}

//m4双通道双边阈值颜色识别
void visionClass::colorRecognize( const Mat &img_input, Mat &img_output, m4_COLORTHRESH &ColorThresh)
{
    Mat img_rgb,
    img_hsv,
    img_H,
    img_H_ranged,
    img_temp1,
    img_temp2,
    img_B,
    img_B_ranged,
    img_ranged;

    vector<Mat> channels_RGB;
    vector<Mat> channels_HSV;
    img_rgb = img_input.clone();
    cvtColor( img_rgb, img_hsv, COLOR_BGR2HSV);
    split( img_rgb, channels_RGB);
    split( img_hsv, channels_HSV);
    img_H = channels_HSV.at(0);
    img_B = channels_RGB.at(1);

    //鉴于H通道是个环形的，需要考虑转一圈的情形
    if( ColorThresh.RH_max >= ColorThresh.RH_min)
    {
        inRange( img_H, Scalar( ColorThresh.RH_min, 0, 0), Scalar( ColorThresh.RH_max, 0, 0), img_H_ranged);
    }
    else
    {
        inRange( img_H, Scalar( ColorThresh.RH_min, 0, 0), Scalar( 180, 0, 0), img_temp1);
        inRange( img_H, Scalar( 0, 0, 0), Scalar( ColorThresh.RH_max, 0, 0), img_temp2);
        bitwise_or( img_temp1, img_temp2, img_H_ranged);
    }

    //再考虑B通道的阈值
    inRange( img_B, Scalar( ColorThresh.RB_min, 0, 0), Scalar( ColorThresh.RB_max, 0, 0), img_B_ranged);

    //两幅图像膨胀之后再按位取与
    dilate( img_H_ranged, img_H_ranged, 5);
    dilate( img_B_ranged, img_B_ranged, 5);
    bitwise_and( img_H_ranged, img_B_ranged, img_ranged);

    //结果图输出
    img_ranged.copyTo( img_output);

    return;
}
