#include "missiondrop.h"
#include "visionclass.h"
#include <unistd.h>
#include <QApplication>
#include <QEventLoop>

//任务二：抛球
//任务不会即刻启动：只有当水声测得的锥角小于40°后，主控调用任务二
//直方图拉伸：R、G通道拉伸程度要大于B通道
//超大核中值滤波：，31*31
//双通道双边阈值颜色识别：H通道，B通道
void visionClass::missionDrop()
{
	//修改运行状态，便于主控查询
	data.m2_runningState = true;

	//任务变量
	Mat m2_imgsrc,					//源图像
		m2_imgblur,					//中值滤波图像
		m2_imghsv,					//HSV颜色空间
		m2_bgr_streched,			//双通道拉伸之后的RGB图像
		m2_imgranged,				//双边阈值颜色识别之后的图像
        m2_imgcanny,				//边缘识别之后的图像
        m2_imgresult;

	vector<Mat> m2_bgr_channels,		//BGR双通道
		m2_hsv_channels;				//HSV双通道

    //bool m2_start = true;				//主控调用任务二标志位


	int m2_iteration = 0;				//程序执行次数
	int m2_pixelsum = 0;				//颜色识别后像素点计数

    static const float m2_imgscale = 0.5;			//图像缩放比例
    static const int m2_saveinterval = 3;			//图像保存周期
	static const int m2_pixelThresh = 50000*m2_imgscale*m2_imgscale;	//颜色识别后像素点下阈值

	//用于颜色识别的结构体
	m2_COLORTHRESH colorThresh;

    /** RGB **/
    colorThresh.RB_max = 36;
    colorThresh.RB_min = 0;
    colorThresh.RH_max = 144;
    colorThresh.RH_min = 0;
    colorThresh.RV_max = 105;
    colorThresh.RV_min = 40;

    //用于矩形拟合的结构体
	m2_RECT m2_Rect;
	m2_Rect.area = 0;
	m2_Rect.areaLowThresh = 50000*m2_imgscale*m2_imgscale;
	m2_Rect.valid = false;

	//创建窗口，定义滑动条
    emit createWindow("Color");
    emit createBar( "B_rangedm", "Color", &colorThresh.RB_max, 255);
    emit createBar( "B_ranged", "Color", &colorThresh.RB_min, 255);
    emit createBar( "H_rangedm", "Color",  &colorThresh.RH_max, 255);
    emit createBar( "H_ranged", "Color",  &colorThresh.RH_min, 255);
    emit createBar( "V_rangedm", "Color",  &colorThresh.RV_max, 255);
    emit createBar( "V_ranged", "Color", &colorThresh.RV_min, 255);


	//开启采集
    visionClass::start(Cam_Bottom);
    DropCamSetting();

    int expotime_d=0;

	while(m2_start)
	{

        expotime_d++;
        if(expotime_d<10)
              DropCamSetting();
        emit cameraStartedB_m(true);
        //程序执行次数+1
		m2_iteration++;

        qDebug()<<"m2_iteration "<<m2_iteration;

		//读入图像
		//m2_imgsrc = imread("src.jpg");
        frame2Mat(Cam_Bottom, m2_imgsrc);
        transpose(m2_imgsrc,m2_imgsrc);
        flip(m2_imgsrc,m2_imgsrc,0);

        qDebug()<<"m2_iteration "<<m2_iteration;

        //满足条件，即保存图像
        if( m2_iteration % m2_saveinterval == 0)
        {
            char imgnamesave[15] = "save2_";
            char countsave[10];
            sprintf( countsave, "%d", m2_iteration);
            strcat(imgnamesave, countsave);
            strcat(imgnamesave, ".jpg");
            imwrite(imgnamesave, m2_imgsrc);
        }

		//图像缩放
		resize( m2_imgsrc, m2_imgsrc, cv::Size(), m2_imgscale, m2_imgscale);

        emit imageShow(&m2_imgsrc, "m2_imgsrc");

		//中值滤波
        medianBlur( m2_imgsrc, m2_imgblur, 35);

		//双通道双边阈值颜色识别：H通道、B通道
        // colorRecognize(  m2_imgblur, m2_imgranged, colorThresh);
        inRange(m2_imgsrc, Scalar(colorThresh.RB_min, colorThresh.RH_min, colorThresh.RV_min), Scalar(colorThresh.RB_max, colorThresh.RH_max, colorThresh.RV_max), m2_imgranged);
        imshow("AAAAA", m2_imgranged);
        // bitwise_not(m2_imgranged, m2_imgranged);

		//计算像素点数
		//如果像素点数大于阈值，进行矩形拟合
		m2_pixelsum = countNonZero( m2_imgranged);
		if( m2_pixelsum >= m2_pixelThresh)
		{
			//进行canny边缘检测，提高矩形拟合的准确度
            //Canny( m2_imgranged, m2_imgcanny, 20, 50);
            rectBoundary( m2_imgranged, m2_imgblur, m2_Rect);
		}
		else
		{
			//切记：如果像素点数不满足阈值条件，也是valid=fasle的条件
            rectBoundary( m2_imgranged, m2_imgblur, m2_Rect);
			m2_Rect.valid = false;
		}

		//如果本次矩形拟合有效，给主控发送数据，显示图像
		//当m2_drum_dx>0时，航行器应当向左平移；当m2_drum_dx<0时，航行器应当向右平移
		//当m2_drum_dy>0时，航行器应当向前平移；当m2_drum_dy<0时，航行器应当向后平移
		if( m2_Rect.valid == true)
		{
            data.m2_drum_dx = (0.5*m2_imgsrc.cols - 0.5*( m2_Rect.tl_x + m2_Rect.br_x)) / m2_imgscale;
            data.m2_drum_dy = (0.43*m2_imgsrc.rows - 0.5*( m2_Rect.tl_y + m2_Rect.br_y)) / m2_imgscale;
		}
		//如果本次矩形拟合无效，给999
		else
		{
			data.m2_drum_dx = 999;
			data.m2_drum_dy = 999;
		}

        m2_imgresult=m2_imgblur.clone() ;

        char m2_drum_dx[10],
            m2_drum_dy[10];

        sprintf( m2_drum_dx, "%d", data.m2_drum_dx);
        sprintf( m2_drum_dy, "%d", data.m2_drum_dy);

        putText( m2_imgresult, m2_drum_dx, Point( 20, 20), FONT_HERSHEY_SIMPLEX, 1, Scalar(255,0,0), 2, 8);
        putText( m2_imgresult, m2_drum_dy, Point( 100, 20), FONT_HERSHEY_SIMPLEX, 1, Scalar(255,0,0), 2, 8);

        emit imageShow(&m2_imgresult, "missionDrop");

        if (m2_iteration % m2_saveinterval == 0)
        {
            char imgnamesave[15] = "save_m2_";
            char countsave[10];
            sprintf(countsave, "%d", m2_iteration);
            strcat(imgnamesave, countsave);
            strcat(imgnamesave, ".jpg");
            imwrite(imgnamesave, m2_imgresult);
        }

        if(v_fout&&v_fout.is_open())
        {
            qDebug()<<"111111";
            v_fout << "m2_iteration:" << m2_iteration<<std::endl;
            v_fout << "m2_pixelSum：" << m2_pixelsum<<std::endl;
            v_fout << "m2_maxArea:" << m2_Rect.area<<std::endl;
            v_fout << "m2_drum_dx:" << data.m2_drum_dx<<std::endl;
            v_fout << "m2_drum_dy:" << data.m2_drum_dy<<std::endl;
        }
		//输出调试信息
        qDebug() << "m2_iteration:" << m2_iteration;
        qDebug() << "m2_pixelSum：" << m2_pixelsum;
        qDebug() << "m2_maxArea:" << m2_Rect.area;
        qDebug() << "m2_drum_dx:" << data.m2_drum_dx;
        qDebug() << "m2_drum_dy:" << data.m2_drum_dy;

        timeval tempt;
        gettimeofday(&tempt,NULL);
        data.t_now=tempt.tv_sec*1000+tempt.tv_usec/1000;
        //等待！
        QEventLoop loop;
        QTimer::singleShot(50,&loop,SLOT(quit()));
        loop.exec();
        if(v_fout&&v_fout.is_open())
        {
            v_fout << "m2_start" << m2_start <<"\n"<<"\n"<<std::endl;
        }
        qDebug() << "m2_start" << m2_start << endl;
	}

	//停止调用相机
    stop(Cam_Bottom);
    emit cameraStoppedB(true);

	//销毁所有窗口
    emit destroyWindow();

    //程序退出之前，m2_runningState清0,便于主控查询
    data.m2_runningState = false;
    if(v_fout&&v_fout.is_open())
    {
        v_fout << "m2_runningState : " << data.m2_runningState <<"\n"<<"\n"<<"\n"<< std::endl;

    }
    qDebug() << "m2_runningState : " << data.m2_runningState << endl;

	return;
}


