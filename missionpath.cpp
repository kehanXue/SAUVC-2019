#include "missionpath.h"
#include "visionclass.h"
#include <unistd.h>
#include <QApplication>


void visionClass::missionPath()
{

    //修改运行状态，便于主控查询
    data.m1_runningState = true;
     qDebug()<<"missionpath state"<<data.m1_runningState<<endl;
    //任务变量
    Mat m1_imgsrc,          //源图像
        m1_imgresult,
        m1_imgcanny,
        m1_imgranged;           //颜色识别结果图


    int m1_camSetttingStart = 0;
    int m1_pixelsum = 0;				//颜色识别后像素点计数
    int cnt_findpath=0;
    int cnt_losepath=0;

    static const float m1_imgscale = 0.5;          //图像缩放比例
    static const int m1_saveinterval = 3;           //图像保存周期
    static const int m1_findpathThresh=50;
    static const int m1_losepathThresh=6;
    static const int m1_pixelThresh = 1500*m1_imgscale*m1_imgscale;	//颜色识别后像素点下阈值


    int m1_heightscaled = 0,            //缩放后，源图像的高
    m1_widthscaled = 0;                 //缩放后，源图像的宽


    bool m1_camSetting = false;


    m1_COLORTHRESH m1_ColorThresh;
    m1_ColorThresh.YH_min = 0;
    m1_ColorThresh.YH_max =150;
    m1_ColorThresh.YB_min = 0;
    m1_ColorThresh.YB_max = 255;
    m1_ColorThresh.YR_min = 83;
    m1_ColorThresh.YR_max = 255;


    m1_RECT m1_Rect;
    m1_Rect.valid = false;
    m1_Rect.area = 0;
    m1_Rect.areaLowThresh = 1200;



    //启动相机
    visionClass::start(Cam_Bottom);
        pathsetting();
	
	emit createWindow("Result");
    emit createWindow("B_ranged");
    emit createWindow("H_ranged");
    emit createWindow("R_ranged");
    emit createBar("RB_max","B_ranged",&m1_ColorThresh.YB_max,255);
    emit createBar("RB_min","B_ranged",&m1_ColorThresh.YB_min,255);
    emit createBar("RH_max","H_ranged",&m1_ColorThresh.YH_max,255);
    emit createBar("RH_min","H_ranged",&m1_ColorThresh.YH_min,255);
    emit createBar("RR_max","R_ranged",&m1_ColorThresh.YR_max,255);
    emit createBar("RR_min","R_ranged",&m1_ColorThresh.YR_min,255);

    int expotime_p=0;


    while(m1_start)
    {
        expotime_p++;
        if(expotime_p<10)
             pathsetting();
        //任务执行次数+1
        m1_iteration++;

        //读入相机图像
        visionClass::frame2Mat(Cam_Bottom, m1_imgsrc);

        //满足条件，即保存图像
        if( m1_iteration % m1_saveinterval == 0)
        {
            char imgnamesave[15] = "save1_";
            char countsave[10];
            sprintf( countsave, "%d", m1_iteration);
            strcat(imgnamesave, countsave);
            strcat(imgnamesave, ".jpg");
            imwrite(imgnamesave, m1_imgsrc);
        }

        //归一化
        //vector<Mat> img_channels;
        //split(m1_imgsrc,img_channels);
        //normalize(img_channels.at(0),img_channels.at(0),0,255,NORM_MINMAX);
        //normalize(img_channels.at(1),img_channels.at(1),0,255,NORM_MINMAX);
        //normalize(img_channels.at(2),img_channels.at(2),0,255,NORM_MINMAX);
        //merge(img_channels,m1_imgsrc);

        //缩放源图像
        m1_heightscaled = m1_imgsrc.rows * m1_imgscale;
        m1_widthscaled = m1_imgsrc.cols * m1_imgscale;
        resize( m1_imgsrc, m1_imgsrc, Size(), m1_imgscale, m1_imgscale);
        transpose(m1_imgsrc,m1_imgsrc);
        flip(m1_imgsrc,m1_imgsrc,1);
        medianBlur(m1_imgsrc,m1_imgsrc,15);
        //处理结果要画在结果图上
        m1_imgresult = m1_imgsrc.clone();
        m1_colorRecognize(m1_imgsrc, m1_imgranged, m1_ColorThresh);
            //计算像素点数
            //如果像素点数大于阈值，进行矩形拟合
                m1_pixelsum = countNonZero( m1_imgranged);
                if( m1_pixelsum >= m1_pixelThresh)
                {
                    //进行canny边缘检测，提高矩形拟合的准确度
                    Canny( m1_imgranged, m1_imgcanny, 20, 50);
                    m1_rectBoundary( m1_imgcanny, m1_imgresult, m1_Rect);
                }
                else
                {
                    //切记：如果像素点数不满足阈值条件，也是valid=fasle的条件
                    m1_Rect.valid = false;
                }

                //如果本次矩形拟合有效，给主控发送数据，显示图像
                //当m1_path_dx>0时，航行器应当向左平移；当m1_path_dx<0时，航行器应当向右平移
                //当m1_path_dy>0时，航行器应当向前平移；当m1_path_dy<0时，航行器应当向后平移
                if( m1_Rect.valid == true )
                {
                    data.m1_path_dx = (0.5*m1_imgsrc.cols  - m1_Rect.center.x) / m1_imgscale;
                    data.m1_path_dy = (0.5*m1_imgsrc.rows  - m1_Rect.center.y) / m1_imgscale ;
                    data.m1_path_angle=m1_Rect.angle;
                    cnt_findpath++;
                    cnt_losepath=0;
                }
                //如果本次矩形拟合无效，给999
                else
                {
                    data.m1_path_dx = 999;
                    data.m1_path_dy = 999;
                    data.m1_path_angle=999;
                    cnt_findpath--;
                    cnt_losepath++;
                }

                emit imageShow(&m1_imgresult, "missionPath");

                //输出调试信息
                qDebug() << "m1_iteration:" << m1_iteration;
                qDebug() << "m1_pixelSum：" << m1_pixelsum;
                qDebug() << "m1_maxArea:" << m1_Rect.area;
                qDebug() << "m1_path_dx:" << data.m1_path_dx;
                qDebug() << "m1_path_dy:" << data.m1_path_dy;
                qDebug() << "m1_path_angle:" << data.m1_path_angle;

        //at the end of the path
        if(cnt_findpath>m1_findpathThresh &&  cnt_losepath>m1_losepathThresh)
        {
           data.m1_path_dx = -999;
        }



        //输出提示信息
        qDebug() << "迭代次数：" << m1_iteration;
        if(m1_Rect.valid == true)
            qDebug() << "最大矩形面积：" << m1_Rect.area;

        emit imageShow(&m1_imgresult, "Result");

        if( m1_iteration % m1_saveinterval == 0)
        {
            char imgnamesave[15] = "save_m1_";
            char countsave[10];
            sprintf( countsave, "%d", m1_iteration);
            strcat(imgnamesave, countsave);
            strcat(imgnamesave, ".jpg");
            imwrite(imgnamesave, m1_imgresult);
        }

        timeval tempt;
        gettimeofday(&tempt,NULL);
        data.t_now=tempt.tv_sec*1000+tempt.tv_usec/1000;
        //等待！
        QEventLoop loop;
        QTimer::singleShot(50,&loop,SLOT(quit()));
        loop.exec();
    }
    //相机停止采集
    visionClass::stop(Cam_Bottom);

    //销毁所有窗口
    emit destroyWindow();

    //程序退出之前，m1_runningState清0,便于主控查询
    data.m1_runningState = false;
    qDebug() << "m1_runningState : " << data.m1_runningState << endl;

    return;
}

//双边阈值颜色识别
void visionClass::m1_colorRecognize( const Mat &img_input, Mat &img_output, m1_COLORTHRESH &ColorThresh)
{
    Mat img_rgb,
    img_hsv,
    img_temp1,
    img_temp2,
    img_B,
    img_R,
    img_H;
    static Mat img_H_ranged,
            img_B_ranged,
            img_R_ranged,
            img_ranged;



    vector<Mat> channels_RGB;
    vector<Mat> channels_HSV;
    img_rgb = img_input.clone();
    cvtColor( img_rgb, img_hsv, COLOR_BGR2HSV);
    split( img_rgb, channels_RGB);
    split( img_hsv, channels_HSV);
    img_H = channels_HSV.at(0);
    img_B = channels_RGB.at(0);
    img_R = channels_RGB.at(2);


    if( ColorThresh.YH_max >= ColorThresh.YH_min)
    {
        inRange( img_H, Scalar( ColorThresh.YH_min, 0, 0), Scalar( ColorThresh.YH_max, 0, 0), img_H_ranged);
    }
    else
    {
        inRange( img_H, Scalar( ColorThresh.YH_min, 0, 0), Scalar( 255, 0, 0), img_temp1);
        inRange( img_H, Scalar( 0, 0, 0), Scalar( ColorThresh.YH_max, 0, 0), img_temp2);
        bitwise_or( img_temp1, img_temp2, img_H_ranged);
    }


    emit imageShow(&img_H_ranged,"H_ranged");

    if (ColorThresh.YB_max >= ColorThresh.YB_min)
    {
        inRange(img_B, Scalar(ColorThresh.YB_min, 0, 0), Scalar(ColorThresh.YB_max, 0, 0), img_B_ranged);
    }
    else
    {
        inRange(img_B, Scalar(ColorThresh.YB_min, 0, 0), Scalar(255, 0, 0), img_temp1);
        inRange(img_B, Scalar(0, 0, 0), Scalar(ColorThresh.YB_max, 0, 0), img_temp2);
        bitwise_or(img_temp1, img_temp2, img_B_ranged);
    }

    emit imageShow(&img_B_ranged,"B_ranged");


    if (ColorThresh.YR_max >= ColorThresh.YR_min)
    {
        inRange(img_R, Scalar(ColorThresh.YR_min, 0, 0), Scalar(ColorThresh.YR_max, 0, 0), img_R_ranged);
    }
    else
    {
        inRange(img_R, Scalar(ColorThresh.YR_min, 0, 0), Scalar(255, 0, 0), img_temp1);
        inRange(img_R, Scalar(0, 0, 0), Scalar(ColorThresh.YR_max, 0, 0), img_temp2);
        bitwise_or(img_temp1, img_temp2, img_R_ranged);
    }

    emit imageShow(&img_R_ranged,"R_ranged");


    bitwise_and(img_H_ranged, img_B_ranged, img_ranged);
    bitwise_and(img_ranged, img_R_ranged, img_ranged);
    emit imageShow(&img_ranged,"inranged");

    img_ranged.copyTo( img_output);

    return;
}



void visionClass::m1_rectBoundary( const Mat &img_input, Mat &img_output, m1_RECT &m1_Rect)
{
    double maxarea = 0.0;

    //定义findContours()需要的vector容器
    vector< vector<Point> > contours;
    vector<Vec4i> hierarchy;

    //创建容器用于寻找矩形
    RotatedRect m1_FindingRect;
    Point2f vertices[4];

    //寻找轮廓
    findContours( img_input, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );

    //开始搜索符合条件的矩形
    for( int i = 0; i < contours.size(); i++)
    {
        if( contours.at(i).size() < 1 )
        {
            continue;
        }

        m1_FindingRect = minAreaRect(contours[i]);
       // m1_FindingRect_area=contourArea(contours[i],false);
        if( m1_FindingRect.size.area()> maxarea)
        {
            maxarea =m1_FindingRect.size.area();
            m1_Rect.area =m1_FindingRect.size.area();
            m1_Rect.center=m1_FindingRect.center;
            m1_Rect.br=m1_FindingRect.boundingRect().br();
            m1_Rect.tl=m1_FindingRect.boundingRect().tl();
            m1_Rect.height = m1_FindingRect.size.height;
            m1_Rect.width = m1_FindingRect.size.width;
            if(m1_FindingRect.size.width<=m1_FindingRect.size.height)
               m1_Rect.angle=m1_FindingRect.angle;
           if(m1_FindingRect.size.width>m1_FindingRect.size.height)
               m1_Rect.angle=90+m1_FindingRect.angle;
            m1_FindingRect.points(vertices);//获取矩形的四个点
        }
    }

    //如果面积最大的矩形还满足其他的约束条件，就认为本次识别有效，并且画出这个矩形
    //maxarea > m1_areaThresh意味着当前帧图像中有非0矩形，也即m1_Rect中的数据不会是上一次的
   // if( maxarea > m1_Rect.areaLowThresh && m1_Rect.height > m1_Rect.width*m1_Rect.heightwidthratio)
    if( maxarea > m1_Rect.areaLowThresh)
    {
        m1_Rect.valid = true;
        circle(img_output,m1_Rect.center,1,Scalar(255,0,0),2);
        for (int i = 0; i < 4; i++)
         line(img_output, vertices[i], vertices[(i+1)%4], Scalar(255,0,0),2);
    }
    else
    {
        m1_Rect.valid = false;
    }
    return;
}

