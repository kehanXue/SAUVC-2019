#include "visionclass.h"
#include <unistd.h>

//m1矩形拟合
void visionClass:: rectBoundary( const Mat &img_input, Mat &img_output, m1_RECT &m1_Rect, int color)
{
    double maxarea = 0.0;

    //定义findContours()需要的vector容器
    vector< vector<Point> > contours;
    vector<Vec4i> hierarchy;

    //创建容器用于寻找矩形
    Rect m1_FindingRect;

    //寻找轮廓
    findContours( img_input, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );

    //开始搜索符合条件的矩形
    for( int i = 0; i < contours.size(); i++)
    {
        if( contours.at(i).size() < 1 )
        {
            continue;
        }

        m1_FindingRect =  boundingRect(contours[i]);
        if( m1_FindingRect.area()> maxarea)
        {
            maxarea = m1_FindingRect.area();
            m1_Rect.area = m1_FindingRect.area();
            m1_Rect.tl_x = m1_FindingRect.tl().x;
            m1_Rect.tl_y = m1_FindingRect.tl().y;
            m1_Rect.br_x = m1_FindingRect.br().x;
            m1_Rect.br_y = m1_FindingRect.br().y;
            m1_Rect.center_x = 0.5*(m1_Rect.tl_x+m1_Rect.br_x);
            m1_Rect.center_y = 0.5*(m1_Rect.tl_y+m1_Rect.br_y);
        }
    }
    //如果面积最大的矩形还满足其他的约束条件，就认为本次识别有效，并且画出这个矩形
    //maxarea > m1_areaThresh意味着当前帧图像中有非0矩形，也即m1_Rect中的数据不会是上一次的
   // if( maxarea > m1_Rect.areaLowThresh && m1_Rect.height > m1_Rect.width*m1_Rect.heightwidthratio)
    if( maxarea > m1_Rect.areaLowThresh)
    {
        m1_Rect.valid = true;
        if (color==Qt::red)
        rectangle( img_output, Point(m1_Rect.tl_x,m1_Rect.tl_y), Point(m1_Rect.br_x,m1_Rect.br_y), Scalar(0,0,255), 2, 8, 0 );
        if (color==Qt::green)
        rectangle( img_output, Point(m1_Rect.tl_x,m1_Rect.tl_y), Point(m1_Rect.br_x,m1_Rect.br_y), Scalar(0,255,0), 2, 8, 0 );
    }
    else
    {
        m1_Rect.valid = false;
    }
    return;
}

//m2矩形拟合
void visionClass::rectBoundary( const Mat &img_input, Mat &img_output, m2_RECT &m2_Rect)
{
    //记录矩形的最大面积
    double maxarea = 0.0;

    //定义findContours()需要的vector容器
    vector< vector<Point> > contours;
    vector<Vec4i> hierarchy;

    //创建容器用于寻找矩形
    Rect m2_FindingRect;

    //寻找轮廓
    findContours( img_input, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );

    //开始搜索符合条件的矩形
    for( int i = 0; i < contours.size(); i++)
    {
        if( contours.at(i).size() < 1 )
        {
            continue;
        }

        m2_FindingRect = boundingRect( contours[i]);
        if( m2_FindingRect.area() > maxarea)
        {
            maxarea = m2_FindingRect.area();
            m2_Rect.area = m2_FindingRect.area();
            m2_Rect.tl_x = m2_FindingRect.tl().x;
            m2_Rect.tl_y = m2_FindingRect.tl().y;
            m2_Rect.br_x = m2_FindingRect.br().x;
            m2_Rect.br_y = m2_FindingRect.br().y;
            m2_Rect.center_x = 0.5*(m2_Rect.tl_x+m2_Rect.br_x);
            m2_Rect.center_y = 0.5*(m2_Rect.tl_y+m2_Rect.br_y);
        }
    }

    //如果面积最大的矩形还满足其他的约束条件，就认为本次识别有效，并且画出这个矩形
    //maxarea > m2_areaThresh意味着当前帧图像中有非0矩形，也即m2_Rect中的数据不会是上一次的
    if( maxarea > m2_Rect.areaLowThresh)
    {
        m2_Rect.valid = true;
        rectangle( img_output, Point(m2_Rect.tl_x, m2_Rect.tl_y), Point(m2_Rect.br_x, m2_Rect.br_y), Scalar(0,0,255), 2, 8, 0 );
    }
    else
    {
        rectangle( img_output, Point(m2_Rect.tl_x, m2_Rect.tl_y), Point(m2_Rect.br_x, m2_Rect.br_y), Scalar(0,0,255), 2, 8, 0 );
        m2_Rect.valid = false;
    }
    return;
}


//m3矩形拟合
void visionClass::rectBoundary(const Mat &img_input, Mat &img_output, m3_RECT &m3_Rect)
{
    //记录矩形的最大面积
    double maxarea = 0.0;

    //定义findContours()需要的vector容器
    vector< vector<Point> > contours;
    vector<Vec4i> hierarchy;

    //创建容器用于寻找矩形
    Rect m3_FindingRect;

    //寻找轮廓
    findContours(img_input, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

    //开始搜索符合条件的矩形
    for (int i = 0; i < contours.size(); i++)
    {
        if (contours.at(i).size() < 1)
        {
            continue;
        }

        m3_FindingRect = boundingRect(contours[i]);
        if (m3_FindingRect.area() > maxarea)
        {
            maxarea = m3_FindingRect.area();
            m3_Rect.area = m3_FindingRect.area();
            m3_Rect.tl_x = m3_FindingRect.tl().x;
            m3_Rect.tl_y = m3_FindingRect.tl().y;
            m3_Rect.br_x = m3_FindingRect.br().x;
            m3_Rect.br_y = m3_FindingRect.br().y;
            m3_Rect.center_x = 0.5*(m3_Rect.tl_x + m3_Rect.br_x);
            m3_Rect.center_y = 0.5*(m3_Rect.tl_y + m3_Rect.br_y);
        }
    }

    //如果面积最大的矩形还满足其他的约束条件，就认为本次识别有效，并且画出这个矩形
    //maxarea > m3_areaThresh意味着当前帧图像中有非0矩形，也即m3_Rect中的数据不会是上一次的
    if (maxarea > m3_Rect.areaLowThresh)
    {
        m3_Rect.valid = true;
        rectangle(img_output, Point(m3_Rect.tl_x, m3_Rect.tl_y), Point(m3_Rect.br_x, m3_Rect.br_y), Scalar(0, 0, 255), 2, 8, 0);
    }
    else
    {
        m3_Rect.valid = false;
    }
    return;
}

//m4矩形拟合
void visionClass::rectBoundary( const Mat &img_input, Mat &img_output, m4_RECT &m4_Rect)
{
    double maxarea = 0.0;

    //定义findContours()需要的vector容器
    vector< vector<Point> > contours;
    vector<Vec4i> hierarchy;

    //创建容器用于寻找矩形
    Rect m4_FindingRect;

    //寻找轮廓
    findContours( img_input, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );

    //开始搜索符合条件的矩形
    for( int i = 0; i < contours.size(); i++)
    {
        if( contours.at(i).size() < 1 )
        {
            continue;
        }

        m4_FindingRect = boundingRect( contours[i]);
        if( m4_FindingRect.area() > maxarea)
        {
            maxarea = m4_FindingRect.area();
            m4_Rect.area = m4_FindingRect.area();
            m4_Rect.tl_x = m4_FindingRect.tl().x;
            m4_Rect.tl_y = m4_FindingRect.tl().y;
            m4_Rect.br_x = m4_FindingRect.br().x;
            m4_Rect.br_y = m4_FindingRect.br().y;
            m4_Rect.height = m4_FindingRect.height;
            m4_Rect.width = m4_FindingRect.width;
        }
    }

    //如果面积最大的矩形还满足其他的约束条件，就认为本次识别有效，并且画出这个矩形
    //maxarea > m4_areaThresh意味着当前帧图像中有非0矩形，也即m4_Rect中的数据不会是上一次的
    if( maxarea > m4_Rect.areaLowThresh && m4_Rect.height > m4_Rect.width*m4_Rect.heightwidthratio)
    {
        m4_Rect.valid = true;
        rectangle( img_output, Point(m4_Rect.tl_x, m4_Rect.tl_y), Point(m4_Rect.br_x, m4_Rect.br_y), Scalar(0,0,255), 2, 8, 0 );
    }
    else
    {
        m4_Rect.valid = false;
    }
    return;
}
