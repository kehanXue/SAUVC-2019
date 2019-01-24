#include "missionball.h"
#include "visionclass.h"
#include <unistd.h>
#include <QApplication>

//任务四：撞杆
//阶段一：多组模板匹配
//阶段二：m2_findball==1后，多组模板匹配+颜色识别
//阶段三：m2_coloronly==1后，只使用颜色识别
void visionClass::missionBall()
{
    //修改运行状态，便于主控查询
    data.m2_runningState = true;

    //任务变量
    Mat m2_imgsrc,          //源图像
    m2_imgresult,
    m2_imgranged;           //颜色识别结果图

    vector<Mat> m2_TemplVec;        //模板组
    int m2_templcount = 0;          //模板组计数
    int m2_camSetttingStart = 0;

    static const float m2_imgscale = 0.3;          //图像缩放比例
    static const int m2_saveinterval = 3;           //图像保存周期
    static const int m2_templfindballcountThresh = 5;     //模板匹配累计有效帧数上限阈值
    static const int m2_colorfindballcountThresh = 3;      //颜色识别累计有效帧数上限阈值
    static const int m2_areaUpThresh = 200000*m2_imgscale*m2_imgscale;	//矩形拟合面积超过这个上限一定帧数后，认为接近ball
    static const int m2_areaupcountThresh = 3;              //矩形拟合面积超过这个上限该帧数后，认为接近ball
    static const int m2_centerxThresh = 150*m2_imgscale;
    static const int m2_centeryThresh = 200*m2_imgscale;

    int m2_heightscaled = 0,            //缩放后，源图像的高
    m2_widthscaled = 0;                 //缩放后，源图像的宽

    int m2_templfindballcount = 0,     //模板匹配累计有效帧数
    m2_colorfindballcount = 0,         //颜色识别累计有效帧数
    m2_areaupcount = 0;                 //矩形拟合面积超过上阈值的帧数

    bool m2_templfindball = false,         //模板匹配已连续数帧定位ball
    m2_colorfindball = false,              //颜色识别已连续数帧定位ball
    m2_closetoball = false;                //接近ball，该任务结束
    bool m2_camSetting = false;

    m2_MATCH m2_MatchResult;
    m2_MatchResult.valid = false;

    m2_COLORTHRESH m2_ColorThresh;
    m2_ColorThresh.YH_min = 114;
    m2_ColorThresh.YH_max = 255;
    m2_ColorThresh.YB_min = 46;
    m2_ColorThresh.YB_max = 255;

    m2_RECT m2_Rect;
    m2_Rect.valid = false;
    m2_Rect.area = 0;
    m2_Rect.areaLowThresh = 1000*m2_imgscale*m2_imgscale;


    //载入模板组
    Mat m2_imgtempl;
    while(1)
    {
        char imgname[15] = "m2_";
        char count1[10];
        memset( count1, 0, sizeof(count1));
        m2_templcount++;
        sprintf( count1, "%d", m2_templcount);
        strcat( count1, ".jpg");
        strcat( imgname, count1);
        m2_imgtempl = imread( imgname);
        if( m2_imgtempl.data == 0)
        {
            m2_templcount--;
            //如果没有模板载入，提示
            if(m2_templcount == 0)
            {
                qDebug() << "No template has been loaded. Please check!";
            }
            break;
        }
        else
        {
            //如果模板载入成功，进行缩放
            resize( m2_imgtempl, m2_imgtempl, Size(), m2_imgscale, m2_imgscale);
            m2_TemplVec.push_back(m2_imgtempl);
            qDebug() << "Template" << m2_templcount << endl << "height" << m2_imgtempl.rows << " width" << m2_imgtempl.cols;
        }
    }

    //创建窗口
    emit createWindow("Result");
   // emit createWindow("img_ranged");



    //开始采集
    visionClass::start(Cam_Front);
    ballsetting();

    while(m2_start)
    {
        //任务执行次数+1
        m2_iteration++;

        if( m2_MatchResult.valid && !m2_camSetting)
         {
             m2_camSetting = true;
             m2_camSetttingStart = m2_iteration;
         }
         if( m2_camSetting && m2_iteration-m2_camSetttingStart < 10)
         {
             exposureMode(Cam_Front, exposureMode::AutoOnce);
             qDebug() << "Front camera has been reset." << endl;
         }

        //读入相机图像
        visionClass::frame2Mat(Cam_Front,m2_imgsrc);
        if(m2_imgsrc.data==0)
        {
            qDebug() << "Invalid image" << endl;
            return;
        }

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

        //归一化
        //vector<Mat> img_channels;
        //split(m2_imgsrc,img_channels);
        //normalize(img_channels.at(0),img_channels.at(0),0,255,NORM_MINMAX);
        //normalize(img_channels.at(1),img_channels.at(1),0,255,NORM_MINMAX);
        //normalize(img_channels.at(2),img_channels.at(2),0,255,NORM_MINMAX);
        //merge(img_channels,m2_imgsrc);

        //缩放源图像
        m2_heightscaled = m2_imgsrc.rows * m2_imgscale;
        m2_widthscaled = m2_imgsrc.cols * m2_imgscale;
        resize( m2_imgsrc, m2_imgsrc, Size(), m2_imgscale, m2_imgscale);


        //处理结果要画在结果图上
        m2_imgresult = m2_imgsrc.clone();

        //模板匹配
        //当m2_red_dx>0时，航行器应当左转;当m2_red_dx<0时，航行器应当右转
        if( m2_colorfindball == false)
        {
            templatematch_vector( m2_imgsrc, m2_imgresult, m2_TemplVec, m2_MatchResult);
            if(m2_MatchResult.valid == true)
            {
                //只要模板匹配是有效的，就给主控发送有效数据
                data.m2_red_dx = ( 0.5*m2_widthscaled - 0.5*(m2_MatchResult.tl_x + m2_MatchResult.br_x))/m2_imgscale;
                data.m2_red_dy = ( 0.5*m2_widthscaled - 0.5*(m2_MatchResult.tl_y + m2_MatchResult.br_y))/m2_imgscale;
                //累计模板匹配有效的帧数，超过一定帧数后，置位templfindball，开启颜色识别
                m2_templfindballcount++;
                qDebug()<<"m2_templfindballcount"<<m2_templfindballcount<<endl;
                if(m2_templfindballcount > m2_templfindballcountThresh)
                {
                    m2_templfindball = true;
                }
            }
            else
            {
                //本次模板匹配无效，发送999
                data.m2_red_dx = 999;
                data.m2_red_dy = 999;
            }
        }

        //颜色识别
        if(m2_templfindball == true)
        {
            colorRecognize(m2_imgsrc, m2_imgranged, m2_ColorThresh);
            rectBoundary( m2_imgranged, m2_imgresult, m2_Rect);
            if(m2_Rect.valid == true && abs(m2_Rect.tl_x+m2_Rect.br_x-m2_MatchResult.tl_x-m2_MatchResult.br_x) < m2_centerxThresh
                    && abs(m2_Rect.tl_y+m2_Rect.br_y-m2_MatchResult.tl_y-m2_MatchResult.br_y) < m2_centeryThresh)
            {
                //累计颜色识别有效的帧数，超过一定帧数后，置位colorfindball，关闭模板匹配
                m2_colorfindballcount++;
                if(m2_colorfindballcount > m2_colorfindballcountThresh)
                {
                    m2_colorfindball = true;
                }
            }
        }

        //颜色识别给主控发送数据
        if(m2_colorfindball == true && m2_Rect.valid == false)
        {
            data.m2_red_dx = 999;
            data.m2_red_dy = 999;
        }
        if(m2_colorfindball == true && m2_Rect.valid == true)
        {
            data.m2_red_dx = ( 0.5*m2_widthscaled - 0.5*(m2_Rect.tl_x + m2_Rect.br_x))/m2_imgscale;
            data.m2_red_dy = ( 0.5*m2_widthscaled - 0.5*(m2_Rect.tl_y + m2_Rect.br_y))/m2_imgscale;
            if(m2_Rect.area > m2_areaUpThresh)
            {
                qDebug()<<"m2_areaupcount"<<m2_areaupcount<<endl;
                m2_areaupcount++;
                if( m2_areaupcount > m2_areaupcountThresh)
                {
                    m2_closetoball = true;
                }
            }
        }

        //通知主控，任务即将结束
        if(m2_closetoball == true)
        {
            data.m2_red_dx = -999;
            data.m2_red_dy = -999;
        }

        //输出提示信息
        qDebug() << "迭代次数：" << m2_iteration;
        if(m2_colorfindball == false)
            qDebug() << "模板匹配置信度：" << m2_MatchResult.confidence;
        if(m2_templfindball == true)
            qDebug() << "最大矩形面积：" << m2_Rect.area;
        qDebug()<<"m2_red_dx:"<<data.m2_red_dx<<endl;
        qDebug()<<"m2_red_dy:"<<data.m2_red_dy<<endl;

        emit imageShow(&m2_imgresult, "Result");

        timeval tempt;
        gettimeofday(&tempt,NULL);
        data.t_now=tempt.tv_sec*1000+tempt.tv_usec/1000;
        //等待！
        QEventLoop loop;
        QTimer::singleShot(50,&loop,SLOT(quit()));
        loop.exec();
    }
    //相机停止采集
    visionClass::stop(Cam_Front);

    //销毁所有窗口
    emit destroyWindow();

    //程序退出之前，m2_runningState清0,便于主控查询
    data.m2_runningState = false;
    qDebug() << "m2_runningState : " << data.m2_runningState << endl;

    return;
}


//多组模板匹配函数
void templatematch_vector( const Mat &img_input, Mat &img_output, const vector<Mat> &TemplVec, m2_MATCH &MatchResult)
{
    Mat img_result;
    double maxVal = 0.0;

    for( int i = 0; i < TemplVec.size(); i++)
    {
        int result_cols = img_input.cols - TemplVec[i].cols + 1;
        int result_rows = img_input.rows - TemplVec[i].rows + 1;
        qDebug() << "the height of imgsrc" << img_input.rows << " width" << img_input.cols;
        qDebug() << "the height of templ" << TemplVec[i].rows << " width" << TemplVec[i].cols;
        img_result.create( result_rows, result_cols, CV_32FC1 );
        matchTemplate( img_input, TemplVec[i], img_result, CV_TM_CCOEFF_NORMED );
        minMaxLoc( img_result, &MatchResult.minVal, &MatchResult.maxVal, &MatchResult.minLoc,
            &MatchResult.maxLoc );
        MatchResult.matchLoc = MatchResult.maxLoc;

        //如果本次匹配结果的最大值比之前匹配都大，更新数据
        if( MatchResult.maxVal > maxVal)
        {
            //切记要更新参考最大值
            maxVal = MatchResult.maxVal;

            //更新匹配结果
            MatchResult.confidence = maxVal;
            MatchResult.tl_x = MatchResult.matchLoc.x;
            MatchResult.tl_y = MatchResult.matchLoc.y;
            MatchResult.br_x = MatchResult.matchLoc.x + TemplVec[i].cols;
            MatchResult.br_y = MatchResult.matchLoc.y + TemplVec[i].rows;

            //如果某个模板的置信度大于0.85，就采用这个模板，并且停止匹配
            if( MatchResult.confidence >= 0.85)
            {
                MatchResult.valid = true;
                rectangle( img_output, Point(MatchResult.tl_x,MatchResult.tl_y), Point(MatchResult.br_x,MatchResult.br_y), Scalar(255,0,0), 2, 8, 0);
                return;
            }
        }
    }

    //如果本次匹配结果的最大置信度大于0.75，就认为该次匹配是有效的
    if( MatchResult.confidence >= 0.75)
    {
        MatchResult.valid = true;
        rectangle( img_output, Point(MatchResult.tl_x,MatchResult.tl_y), Point(MatchResult.br_x,MatchResult.br_y), Scalar(255,0,0), 2, 8, 0);
    }
    else
    {
        MatchResult.valid = false;
    }

    return;
}


//双边阈值颜色识别
void visionClass::colorRecognize( const Mat &img_input, Mat &img_output, m2_COLORTHRESH &ColorThresh)
{
    Mat img_rgb,
    img_hsv,
    img_H,
    img_temp1,
    img_temp2,
    img_B;
    static Mat img_B_ranged,
    img_H_ranged,
    img_ranged;

    vector<Mat> channels_RGB;
    vector<Mat> channels_HSV;
    img_rgb = img_input.clone();
    cvtColor( img_rgb, img_hsv, COLOR_BGR2HSV);
    split( img_rgb, channels_RGB);
    split( img_hsv, channels_HSV);
    img_H = channels_HSV.at(0);
    img_B = channels_RGB.at(2);

    //鉴于H通道是个环形的，需要考虑转一圈的情形
    if( ColorThresh.YH_max >= ColorThresh.YH_min)
    {
        inRange( img_H, Scalar( ColorThresh.YH_min, 0, 0), Scalar( ColorThresh.YH_max, 0, 0), img_H_ranged);
    }
    else
    {
        inRange( img_H, Scalar( ColorThresh.YH_min, 0, 0), Scalar( 180, 0, 0), img_temp1);
        inRange( img_H, Scalar( 0, 0, 0), Scalar( ColorThresh.YH_max, 0, 0), img_temp2);
        bitwise_or( img_temp1, img_temp2, img_H_ranged);
    }

    //再考虑B通道的阈值
    inRange( img_B, Scalar( ColorThresh.YB_min, 0, 0), Scalar( ColorThresh.YB_max, 0, 0), img_B_ranged);

    bitwise_and( img_H_ranged, img_B_ranged, img_ranged);
    //emit imageShow(&img_ranged, "img_ranged");

    //结果图输出
    img_ranged.copyTo( img_output);

    return;
}



void rectBoundary( const Mat &img_input, Mat &img_output, m2_RECT &m2_Rect)
{
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
            m2_Rect.height = m2_FindingRect.height;
            m2_Rect.width = m2_FindingRect.width;
        }
    }

    //如果面积最大的矩形还满足其他的约束条件，就认为本次识别有效，并且画出这个矩形
    //maxarea > m2_areaThresh意味着当前帧图像中有非0矩形，也即m2_Rect中的数据不会是上一次的
    qDebug()<<"max_area"<<maxarea<<endl;
    if( maxarea > m2_Rect.areaLowThresh)
    {
        m2_Rect.valid = true;
        rectangle( img_output, Point(m2_Rect.tl_x, m2_Rect.tl_y), Point(m2_Rect.br_x, m2_Rect.br_y), Scalar(0,0,255), 2, 8, 0 );
    }
    else
    {
        m2_Rect.valid = false;
    }
    return;
}
