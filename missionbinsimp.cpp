#include "missionbin.h"
#include "visionclass.h"
#include <unistd.h>
#include <QApplication>


void visionClass::missionBinsimp()
{

    //修改运行状态，便于主控查询
    data.m6_runningState = true;
     qDebug()<<"missionbinsim state"<<data.m6_runningState<<endl;
    //任务变量
    Mat m6_imgsrc,          //源图像
        m6_imgresult,
        m6_imgcanny,
        m6_imgranged;           //颜色识别结果图


    int m6_camSetttingStart = 0;
    int m6_pixelsum = 0;				//颜色识别后像素点计数
    int cnt_findbinsim=0;
    int cnt_losebinsim=0;

    static const float m6_imgscale = 0.5;          //图像缩放比例
    static const int m6_saveinterval = 3;           //图像保存周期
    static const int m6_findbinsimThresh=100;
    static const int m6_losebinsimThresh=10;
    static const int m6_pixelThresh = 10000*m6_imgscale*m6_imgscale;	//颜色识别后像素点下阈值


    int m6_heightscaled = 0,            //缩放后，源图像的高
    m6_widthscaled = 0;                 //缩放后，源图像的宽



    m6_COLORTHRESH m6_ColorThresh;
    m6_ColorThresh.YH_min = 0;
    m6_ColorThresh.YH_max = 140;
    m6_ColorThresh.YB_min = 91;
    m6_ColorThresh.YB_max = 255;
    m6_ColorThresh.YR_min = 73;
    m6_ColorThresh.YR_max = 255;


    m6_RECT m6_Rect;
    m6_Rect.valid = false;
    m6_Rect.area = 0;
    m6_Rect.areaLowThresh = 2500;



    //启动相机
    visionClass::start(Cam_Bottom);
        binsetting();
	
	emit createWindow("Result");
    emit createWindow("B_ranged");
    emit createWindow("S_ranged");
    emit createWindow("R_ranged");
    emit createBar("RB_max","B_ranged",&m6_ColorThresh.YB_max,255);
    emit createBar("RB_min","B_ranged",&m6_ColorThresh.YB_min,255);
    emit createBar("RH_max","S_ranged",&m6_ColorThresh.YH_max,255);
    emit createBar("RH_min","S_ranged",&m6_ColorThresh.YH_min,255);
    emit createBar("RR_max","R_ranged",&m6_ColorThresh.YR_max,255);
    emit createBar("RR_min","R_ranged",&m6_ColorThresh.YR_min,255);

    int expotime_bins=0;


    while(m6_start)
    {
        expotime_bins++;
        if(expotime_bins<10)
            binsetting();
        //任务执行次数+1
        m6_iteration++;


        //读入相机图像
        visionClass::frame2Mat(Cam_Bottom, m6_imgsrc);

        //满足条件，即保存图像
        if( m6_iteration % m6_saveinterval == 0)
        {
            char imgnamesave[15] = "save6_";
            char countsave[10];
            sprintf( countsave, "%d", m6_iteration);
            strcat(imgnamesave, countsave);
            strcat(imgnamesave, ".jpg");
            imwrite(imgnamesave, m6_imgsrc);
        }

        //归一化
        //vector<Mat> img_channels;
        //split(m6_imgsrc,img_channels);
        //normalize(img_channels.at(0),img_channels.at(0),0,255,NORM_MINMAX);
        //normalize(img_channels.at(1),img_channels.at(1),0,255,NORM_MINMAX);
        //normalize(img_channels.at(2),img_channels.at(2),0,255,NORM_MINMAX);
        //merge(img_channels,m6_imgsrc);

        //缩放源图像
        m6_heightscaled = m6_imgsrc.rows * m6_imgscale;
        m6_widthscaled = m6_imgsrc.cols * m6_imgscale;
        resize( m6_imgsrc, m6_imgsrc, Size(), m6_imgscale, m6_imgscale);
        transpose(m6_imgsrc,m6_imgsrc);
        flip(m6_imgsrc,m6_imgsrc,1);
        medianBlur(m6_imgsrc,m6_imgsrc,5);
        //处理结果要画在结果图上
        m6_imgresult = m6_imgsrc.clone();
        m6_colorRecognize(m6_imgsrc, m6_imgranged, m6_ColorThresh);
            //计算像素点数
            //如果像素点数大于阈值，进行矩形拟合
                m6_pixelsum = countNonZero( m6_imgranged);
                if( m6_pixelsum >= m6_pixelThresh)
                {
                    //进行canny边缘检测，提高矩形拟合的准确度
                    Canny( m6_imgranged, m6_imgcanny, 20, 50);
                    m6_rectBoundary( m6_imgcanny, m6_imgresult, m6_Rect);
                }
                else
                {
                    //切记：如果像素点数不满足阈值条件，也是valid=fasle的条件
                    m6_Rect.valid = false;
                }

                //如果本次矩形拟合有效，给主控发送数据，显示图像
                //当m6_binsim_dx>0时，航行器应当向左平移；当m6_binsim_dx<0时，航行器应当向右平移
                //当m6_binsim_dy>0时，航行器应当向前平移；当m6_binsim_dy<0时，航行器应当向后平移
                if( m6_Rect.valid == true )
                {
                    data.m6_binsim_dx = (0.5*m6_imgsrc.cols  - m6_Rect.center.x) / m6_imgscale;
                    data.m6_binsim_dy = (0.5*m6_imgsrc.rows+5  - m6_Rect.center.y) / m6_imgscale ;
                    data.m6_binsim_angle=m6_Rect.angle;
                                        
                }
                //如果本次矩形拟合无效，给999
                else
                {
                    data.m6_binsim_dx = 999;
                    data.m6_binsim_dy = 999;
                    data.m6_binsim_angle=999;                 
                }
				

				
                emit imageShow(&m6_imgresult, "missionbinsim");

                //输出调试信息
                qDebug() << "m6_iteration:" << m6_iteration;
                qDebug() << "m6_pixelSum：" << m6_pixelsum;
                qDebug() << "m6_maxArea:" << m6_Rect.area;
                qDebug() << "m6_binsim_dx:" << data.m6_binsim_dx;
                qDebug() << "m6_binsim_dy:" << data.m6_binsim_dy;
                qDebug() << "m6_binsim_angle:" << data.m6_binsim_angle;





        //输出提示信息
        qDebug() << "迭代次数：" << m6_iteration;
        if(m6_Rect.valid == true)
            qDebug() << "最大矩形面积：" << m6_Rect.area;

        emit imageShow(&m6_imgresult, "Result");

        if( m6_iteration % m6_saveinterval == 0)
        {
            char imgnamesave[15] = "save_m6_";
            char countsave[10];
            sprintf( countsave, "%d", m6_iteration);
            strcat(imgnamesave, countsave);
            strcat(imgnamesave, ".jpg");
            imwrite(imgnamesave, m6_imgresult);
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

    //程序退出之前，m6_runningState清0,便于主控查询
    data.m6_runningState = false;
    qDebug() << "m6_runningState : " << data.m6_runningState << endl;

    return;
}

//双边阈值颜色识别
void visionClass::m6_colorRecognize( const Mat &img_input, Mat &img_output, m6_COLORTHRESH &ColorThresh)
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
    img_H = channels_HSV.at(1);
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


    emit imageShow(&img_H_ranged,"S_ranged");

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



void visionClass::m6_rectBoundary( const Mat &img_input, Mat &img_output, m6_RECT &m6_Rect)
{
    double maxarea = 0.0;

    //定义findContours()需要的vector容器
    vector< vector<Point> > contours;
    vector<Vec4i> hierarchy;

    //创建容器用于寻找矩形
    RotatedRect m6_FindingRect;
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

        m6_FindingRect = minAreaRect(contours[i]);
       // m6_FindingRect_area=contourArea(contours[i],false);
        if( m6_FindingRect.size.area()> maxarea)
        {
            maxarea =m6_FindingRect.size.area();
            m6_Rect.area =m6_FindingRect.size.area();
            m6_Rect.center=m6_FindingRect.center;
            m6_Rect.br=m6_FindingRect.boundingRect().br();
            m6_Rect.tl=m6_FindingRect.boundingRect().tl();
            m6_Rect.height = m6_FindingRect.size.height;
            m6_Rect.width = m6_FindingRect.size.width;
            if(m6_FindingRect.size.width<=m6_FindingRect.size.height)
               m6_Rect.angle=m6_FindingRect.angle;
           if(m6_FindingRect.size.width>m6_FindingRect.size.height)
               m6_Rect.angle=90+m6_FindingRect.angle;
            m6_FindingRect.points(vertices);//获取矩形的四个点
        }
    }

    //如果面积最大的矩形还满足其他的约束条件，就认为本次识别有效，并且画出这个矩形
    //maxarea > m6_areaThresh意味着当前帧图像中有非0矩形，也即m6_Rect中的数据不会是上一次的
   // if( maxarea > m6_Rect.areaLowThresh && m6_Rect.height > m6_Rect.width*m6_Rect.heightwidthratio)
    if( maxarea > m6_Rect.areaLowThresh)
    {
        m6_Rect.valid = true;
        circle(img_output,m6_Rect.center,1,Scalar(255,0,0),2);
        for (int i = 0; i < 4; i++)
         line(img_output, vertices[i], vertices[(i+1)%4], Scalar(255,0,0),2);
    }
    else
    {
        m6_Rect.valid = false;
    }
    return;
}

