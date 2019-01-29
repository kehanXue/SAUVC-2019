//#include "missionflare.h"
#include "visionclass.h"
#include <unistd.h>
#include <QApplication>

//任务四：撞杆
//阶段一：多组模板匹配
//阶段二：m4_findflare==1后，多组模板匹配+颜色识别
//阶段三：m4_coloronly==1后，只使用颜色识别
void visionClass::missionFlare()
{
    //修改运行状态，便于主控查询
    data.m4_runningState = true;

    //任务变量
    Mat m4_imgsrc,          //源图像
    m4_imgresult,
    m4_imgranged;           //颜色识别结果图

    vector<Mat> m4_TemplVec;        //模板组
    int m4_templcount = 0;          //模板组计数
    int m4_iteration = 0;           //程序执行次数
    int m4_camSetttingStart = 0;


    static const float m4_imgscale = 0.5;          //图像缩放比例
    static const int m4_saveinterval = 3;           //图像保存周期
    static const int m4_templfindflarecountThresh = 5;     //模板匹配累计有效帧数上限阈值
    static const int m4_colorfindflarecountThresh = 3;      //颜色识别累计有效帧数上限阈值
    static const int m4_areaUpThresh = 20000*m4_imgscale*m4_imgscale;    //矩形拟合面积超过这个上限一定帧数后，认为接近Flare
    static const int m4_pixelsumUpThresh = 20000*m4_imgscale*m4_imgscale;
    static const int m4_areaupcountThresh = 2;              //矩形拟合面积超过这个上限该帧数后，认为接近Flare
    static const int m4_centerxThresh = 150*m4_imgscale;
    static const int m4_centeryThresh = 200*m4_imgscale;

    int m4_heightscaled = 0,            //缩放后，源图像的高
    m4_widthscaled = 0;                 //缩放后，源图像的宽

    int m4_templfindflarecount = 0,     //模板匹配累计有效帧数
    m4_colorfindflarecount = 0,         //颜色识别累计有效帧数
    m4_pixelsum=0,                      //颜色识别像素点统计值
    m4_areaupcount = 0;                 //矩形拟合面积超过上阈值的帧数

    bool m4_templfindflare = false,         //模板匹配已连续数帧定位Flare
    m4_colorfindflare = false,              //颜色识别已连续数帧定位Flare
    m4_closetoflare = false;                //接近Flare，该任务结束

    m4_MATCH m4_MatchResult;
    m4_MatchResult.valid = false;

    m4_COLORTHRESH m4_ColorThresh;
    m4_ColorThresh.RH_min = m4_RH_min;
    m4_ColorThresh.RH_max = m4_RH_max;
    m4_ColorThresh.RB_min = m4_RB_min;
    m4_ColorThresh.RB_max = m4_RB_max;

    m4_RECT m4_Rect;
    m4_Rect.valid = false;
    m4_Rect.area = 0;
    m4_Rect.areaLowThresh = 3000*m4_imgscale*m4_imgscale;

    //载入模板组
    Mat m4_imgtempl;
    while(1)
    {
        char imgname[100] = "/home/nwpu/Code/Robosub_test2018-2-8/m4_";
        char count1[10];
        memset( count1, 0, sizeof(count1));
        m4_templcount++;
        sprintf( count1, "%d", m4_templcount);
        strcat( count1, ".jpg");
        strcat( imgname, count1);
        m4_imgtempl = imread( imgname);
        if( m4_imgtempl.data == 0)
        {
            m4_templcount--;
            //如果没有模板载入，提示
            if(m4_templcount == 0)
            {
                if(v_fout&&v_fout.is_open())
                {
                    v_fout << "No template has been loaded. Please check!"<<std::endl;
                }
                qDebug() << "No template has been loaded. Please check!";
            }
            break;
        }
        else
        {
            //如果模板载入成功，进行缩放
            resize( m4_imgtempl, m4_imgtempl, Size(), m4_imgscale, m4_imgscale);
            m4_TemplVec.push_back(m4_imgtempl);
            if(v_fout&&v_fout.is_open())
            {
                v_fout << "Template" << m4_templcount << endl << "height" << m4_imgtempl.rows << " width" << m4_imgtempl.cols<<std::endl;
            }
            qDebug() << "Template" << m4_templcount << endl << "height" << m4_imgtempl.rows << " width" << m4_imgtempl.cols;
        }
    }
    imshow("TEMP", m4_TemplVec[0]);

    //创建窗口
    emit createWindow("Result");
    emit createBar("Result", "HB", &m4_ColorThresh.RH_min);


    //开始采集
    visionClass::start(Cam_Front);
    FlareCamSetting();

    int expotime_f=0;

    while(m4_start)
    {
        expotime_f++;
        if(expotime_f<10)
             FlareCamSetting();
        emit cameraStartedF_m(true);
        //任务执行次数+1
        m4_iteration++;

        //读入相机图像
        visionClass::frame2Mat(Cam_Front, m4_imgsrc);

        //满足条件，即保存图像
        if( m4_iteration % m4_saveinterval == 0)
        {
            char imgnamesave[15] = "save4_";
            char countsave[10];
            sprintf( countsave, "%d", m4_iteration);
            strcat(imgnamesave, countsave);
            strcat(imgnamesave, ".jpg");
            imwrite(imgnamesave, m4_imgsrc);
        }

        //归一化
        //vector<Mat> img_channels;
        //split(m4_imgsrc,img_channels);
        //normalize(img_channels.at(0),img_channels.at(0),0,255,NORM_MINMAX);
        //normalize(img_channels.at(1),img_channels.at(1),0,255,NORM_MINMAX);
        //normalize(img_channels.at(2),img_channels.at(2),0,255,NORM_MINMAX);
        //merge(img_channels,m4_imgsrc);

        //缩放源图像
        m4_heightscaled = m4_imgsrc.rows * m4_imgscale;
        m4_widthscaled = m4_imgsrc.cols * m4_imgscale;
        resize( m4_imgsrc, m4_imgsrc, Size(), m4_imgscale, m4_imgscale);

        //创建感兴趣区域
        Mat m4_imgROI(m4_imgsrc, Rect(0,0.1*m4_heightscaled,m4_widthscaled,0.8*m4_heightscaled));

        //处理结果要画在结果图上
        m4_imgresult = m4_imgROI.clone();

        //模板匹配
        //当m4_flare_dx>0时，航行器应当左转;当m4_flare_dx<0时，航行器应当右转
        if( m4_colorfindflare == false)
        {
            templatematch_vector( m4_imgROI, m4_imgresult, m4_TemplVec, m4_MatchResult);
            if(m4_MatchResult.valid == true)
            {
                //只要模板匹配是有效的，就给主控发送有效数据
                data.m4_flare_dx = ( 0.5*m4_widthscaled - 0.5*(m4_MatchResult.tl_x + m4_MatchResult.br_x))/m4_imgscale;

                //累计模板匹配有效的帧数，超过一定帧数后，置位templfindflare，开启颜色识别
                m4_templfindflarecount++;
                if(m4_templfindflarecount > m4_templfindflarecountThresh)
                {
                    m4_templfindflare = true;  // 2019-1-28
                }
            }
            else
            {
                //本次模板匹配无效，发送999
                data.m4_flare_dx = 999;
            }
            if(v_fout&&v_fout.is_open())
            {
                v_fout <<"m4_templfindflare：" << m4_templfindflare<<std::endl;
            }

            qDebug()<<"m4_templfindflare：" << m4_templfindflare<<endl;
        }


        //颜色识别
        if(m4_templfindflare == true)
        {
            colorRecognize(m4_imgROI, m4_imgranged, m4_ColorThresh);
            m4_pixelsum = countNonZero( m4_imgranged);
            rectBoundary( m4_imgranged, m4_imgresult, m4_Rect);
            imshow("www", m4_imgranged);
            if(m4_Rect.valid == true && abs(m4_Rect.tl_x+m4_Rect.br_x-m4_MatchResult.tl_x-m4_MatchResult.br_x) < m4_centerxThresh)
                    //&& abs(m4_Rect.tl_y+m4_Rect.br_y-m4_MatchResult.tl_y-m4_MatchResult.br_y) < m4_centeryThresh)
            {
                //累计颜色识别有效的帧数，超过一定帧数后，置位colorfindflare，关闭模板匹配
                m4_colorfindflarecount++;
                if(m4_colorfindflarecount > m4_colorfindflarecountThresh)
                {
                    m4_colorfindflare = true;
                }
            }
            if(v_fout&&v_fout.is_open())
            {
                v_fout <<"m4_colorfindflare：" << m4_colorfindflare<<std::endl;
            }
            qDebug()<<"m4_colorfindflare：" << m4_colorfindflare<<endl;
        }

        //颜色识别给主控发送数据
        if(m4_colorfindflare == true && m4_Rect.valid == false)
        {
            data.m4_flare_dx = 999;
        }
        if(m4_colorfindflare == true && m4_Rect.valid == true)
        {
            data.m4_flare_dx = ( 0.5*m4_widthscaled - 0.5*(m4_Rect.tl_x + m4_Rect.br_x))/m4_imgscale;
            if((m4_Rect.area > m4_areaUpThresh) && (m4_pixelsum > m4_pixelsumUpThresh))
            {
                m4_areaupcount++;
                if( m4_areaupcount > m4_areaupcountThresh)
                {
                    m4_closetoflare = true;
                }
            }
            if(v_fout&&v_fout.is_open())
            {
                v_fout <<"m4_closetoflare：" << m4_closetoflare<<std::endl;
            }
            qDebug()<<"m4_closetoflare：" << m4_closetoflare<<endl;
        }


        //通知主控，任务即将结束
        if(m4_closetoflare == true)
        {
            data.m4_flare_dx = -999;
            qDebug() << "[+] !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!   data.m4_flare_dx : " << data.m4_flare_dx;
        }

        //输出提示信息
        if(v_fout&&v_fout.is_open())
        {
            v_fout <<"迭代次数：" << m4_iteration<<std::endl;
        }
        qDebug() << "迭代次数：" << m4_iteration;
        if(m4_colorfindflare == false)
        {
            if(v_fout&&v_fout.is_open())
            {
                v_fout << "模板匹配置信度：" << m4_MatchResult.confidence<<std::endl;
            }
            qDebug() << "模板匹配置信度：" << m4_MatchResult.confidence;
        }
        if(m4_templfindflare == true)
        {
            if(v_fout&&v_fout.is_open())
            {
                v_fout << "最大矩形面积：" << m4_Rect.area<<std::endl;
            }
            qDebug() << "最大矩形面积：" << m4_Rect.area;
        }


        char m4_flare_dx[10];

        sprintf(m4_flare_dx,"%d",data.m4_flare_dx);

        putText( m4_imgresult, m4_flare_dx, Point( 20, 20), FONT_HERSHEY_SIMPLEX, 1, Scalar(255,0,0), 2, 8);

        if(v_fout&&v_fout.is_open())
        {
            v_fout <<"m4_flare_dx：" << data.m4_flare_dx<<std::endl;
        }

        qDebug()<<data.m4_flare_dx<<endl;

        emit imageShow(&m4_imgresult, "Result");
        if (m4_iteration % m4_saveinterval == 0)
        {
            char imgnamesave[15] = "save_m4_";
            char countsave[10];
            sprintf(countsave, "%d", m4_iteration);
            strcat(imgnamesave, countsave);
            strcat(imgnamesave, ".jpg");
            imwrite(imgnamesave, m4_imgresult);
        }



        timeval tempt;
        gettimeofday(&tempt,NULL);
        data.t_now=tempt.tv_sec*1000+tempt.tv_usec/1000;
        //等待！
        QEventLoop loop;
        QTimer::singleShot(50,&loop,SLOT(quit()));
        loop.exec();

        if(v_fout&&v_fout.is_open())
        {
            v_fout << "m4_start" << m4_start <<"\n"<<"\n"<<std::endl;
        }
        qDebug() << "m4_start" << m4_start << endl;
    }
    //相机停止采集
    visionClass::stop(Cam_Front);
    emit cameraStoppedF(true);
    //销毁所有窗口
    emit destroyWindow();

    //程序退出之前，m4_runningState清0,便于主控查询
    data.m4_runningState = false;
    if(v_fout&&v_fout.is_open())
    {
        v_fout << "m4_runningState : " << data.m4_runningState << "\n"<<"\n"<<"\n"<<std::endl;
    }
    qDebug() << "m4_runningState : " << data.m4_runningState << endl;

    return;
}


//多组模板匹配函数
/*void visionClass::templatematch_vector( const Mat &img_input, Mat &img_output, const vector<Mat> &TemplVec, m4_MATCH &MatchResult)
{
    Mat img_result;
    double maxVal = 0.0;

    for( int i = 0; i < TemplVec.size(); i++)
    {
        int result_cols = img_input.cols - TemplVec[i].cols + 1;
        int result_rows = img_input.rows - TemplVec[i].rows + 1;
        if(v_fout&&v_fout.is_open())
        {
            v_fout << "the height of imgsrc" << img_input.rows << " width" << img_input.cols<<"\n";
            v_fout << "the height of templ" << TemplVec[i].rows << " width" << TemplVec[i].cols<<"\n";
        }
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
*/
