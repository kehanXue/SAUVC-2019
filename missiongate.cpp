#include "visionclass.h"
#include <unistd.h>
#include <QApplication>
#include<QDebug>

enum COLOR
{
    GREEN=1,
    RED=2,
    BLACK=3,
    ANGLE_G=4,
    ANGLE_R=5
};

bool visionClass::isColor(cv::Mat& frame, COLOR color) {
    cv::Mat ranged;
    if (color == RED) {
        int R_low = 7, G_low = 102, B_low = 47;
        int R_high = 82, G_high = 182, B_high = 77;
        inRange(frame, cv::Scalar(R_low, G_low, B_low), cv::Scalar(R_high, G_high, B_high), ranged);
        if (cv::countNonZero(ranged) / (double)(frame.cols * frame.rows) > 0.7){
            return true;
        } else {
            return false;
        }
    } else if (color  == GREEN) {
        int R_low = 27, G_low = 24, B_low = 55;
        int R_high = 119, G_high = 74, B_high = 102;
        inRange(frame, cv::Scalar(R_low, G_low, B_low), cv::Scalar(R_high, G_high, B_high), ranged);
        if (cv::countNonZero(ranged) / (double)(frame.cols * frame.rows) > 0.7){
            return true;
        } else {
            return false;
        }

    }
    return false;
}

void visionClass:: missionGate()
{

    data.m1_runningState = true;
    data.m1_gateFound=false;
    if(v_fout&&v_fout.is_open())
    {
        v_fout<<"missionUgate state"<<data.m1_runningState<< std::endl;
    }


    /********************************variables**************************************************************************/
    int m1_black_dx=999;//compared to the center
    int m1_redtemp_dx=999;//compared to 0.1*width of the image
    int m1_redtemp=999;//true location of the red flare
    int m1_greentemp_dx=999;//compared to 0.9*width of the image
    int m1_greentemp=999;//true location of the green flare
    int m1_angle_r_temp_dx=999;//compared to 0.1*width of the image
    int m1_angle_r_temp=999;//true location of the red flare
    int m1_angle_g_temp_dx=999;//compared to 0.9*width of the image
    int m1_angle_g_temp=999;//true location of the green flare

    int m1_redcolor_dx=999;//compared to 0.1*width of the image
    int m1_redcolor=999;//true location of the red flare
    int m1_greencolor_dx=999;//compared to 0.9*width of the image
    int m1_greencolor=999;//true location of the green flare

    int m1_heightscaled = 0,            //缩放后，源图像的高
        m1_widthscaled = 0;                 //缩放后，源图像的宽

    int cnt_gateFound=0;

    int m1_iteration = 0;
    int m1_pixelsum = 0;

    Mat m1_imgsrc,
        m1_imgresult,
        m1_imgcanny,
        m1_imgranged;

    static const float m1_imgscale = 0.5;          //图像缩放比例
    static const int m1_saveinterval = 3;           //图像保存周期
    static const int m1_pixelThresh = 2000*m1_imgscale*m1_imgscale;//颜色识别后像素点下阈值

/*********************************loading template****************************************************************/
         vector<Mat> m1_TemplVecB;
         m1_TemplVecB=loadTempl(BLACK,m1_imgscale);
         vector<Mat> m1_TemplVecG;
         m1_TemplVecG=loadTempl(GREEN,m1_imgscale);
         vector<Mat> m1_TemplVecR;
         m1_TemplVecR=loadTempl(RED,m1_imgscale);
         vector<Mat> m1_TemplVecA_G;
         m1_TemplVecA_G=loadTempl(ANGLE_G,m1_imgscale);
         vector<Mat> m1_TemplVecA_R;
         m1_TemplVecA_R=loadTempl(ANGLE_R,m1_imgscale);

         m1_MATCH m1_MatchResultB;
         m1_MatchResultB.valid = false;
         m1_MATCH m1_MatchResultG;
         m1_MatchResultG.valid = false;
         m1_MATCH m1_MatchResultR;
         m1_MatchResultR.valid = false;
         m1_MATCH m1_MatchResultA_G;
         m1_MatchResultA_G.valid = false;
         m1_MATCH m1_MatchResultA_R;
         m1_MatchResultA_R.valid = false;

 /*********************************color threshold******************************************************************/
        m1_COLORTHRESH m1_ColorThreshR;
        m1_ColorThreshR.H_min = 127;
        m1_ColorThreshR.H_max = 49;//put h_value to 0-255 to ignore this channel's threshold
        m1_ColorThreshR.B_min = 0;
        m1_ColorThreshR.B_max = 90;
        m1_ColorThreshR.R_min = 0;
        m1_ColorThreshR.R_max = 162;

        m1_COLORTHRESH m1_ColorThreshG;
        m1_ColorThreshG.H_min = 156;
        m1_ColorThreshG.H_max = 31;//put h_value to 0-255 to ignore this channel's threshold
        m1_ColorThreshG.B_min = 39;
        m1_ColorThreshG.B_max = 79;
        m1_ColorThreshG.R_min = 0;
        m1_ColorThreshG.R_max = 89;

        m1_RECT m1_RectR;
        m1_RectR.valid = false;
        m1_RectR.area = 0;
        m1_RectR.areaLowThresh = 2000*m1_imgscale*m1_imgscale;

        m1_RECT m1_RectG;
        m1_RectG.valid = false;
        m1_RectG.area = 0;
        m1_RectG.areaLowThresh = 2000*m1_imgscale*m1_imgscale;


/*************************************Trackbar***********************************************************************/

     emit createWindow("Result");
     emit createWindow("RH_ranged");
     emit createWindow("RB_ranged");
     emit createWindow("RR_ranged");
     emit createWindow("GH_ranged");
     emit createWindow("GB_ranged");
     emit createWindow("GR_ranged");

     emit createBar("RB_max","RB_ranged",&m1_ColorThreshR.B_max,255);
     emit createBar("RB_min","RB_ranged",&m1_ColorThreshR.B_min,255);
     emit createBar("RH_max","RH_ranged",&m1_ColorThreshR.H_max,255);
     emit createBar("RH_min","RH_ranged",&m1_ColorThreshR.H_min,255);
     emit createBar("RR_max","RR_ranged",&m1_ColorThreshR.R_max,255);
     emit createBar("RR_min","RR_ranged",&m1_ColorThreshR.R_min,255);

     emit createBar("GB_max","GB_ranged",&m1_ColorThreshG.B_max,255);
     emit createBar("GB_min","GB_ranged",&m1_ColorThreshG.B_min,255);
     emit createBar("GH_max","GH_ranged",&m1_ColorThreshG.H_max,255);
     emit createBar("GH_min","GH_ranged",&m1_ColorThreshG.H_min,255);
     emit createBar("GR_max","GR_ranged",&m1_ColorThreshG.R_max,255);
     emit createBar("GR_min","GR_ranged",&m1_ColorThreshG.R_min,255);



    visionClass::start(Cam_Front);
    GateCamSetting();
    resize( m1_TemplVecA_G[0], m1_TemplVecA_G[0], Size(), m1_imgscale, m1_imgscale);
    resize( m1_TemplVecA_R[0], m1_TemplVecA_R[0], Size(), m1_imgscale, m1_imgscale);
    //imshow("eee", m1_TemplVecA_G[0]);

    int expotime_g=0;

    while (m1_start)
    {
        m1_black_dx=999;
        m1_redtemp_dx=999;
        m1_redtemp=999;
        m1_greentemp_dx=999;
        m1_greentemp=999;
        m1_angle_r_temp_dx=999;
        m1_angle_r_temp=999;
        m1_angle_g_temp_dx=999;
        m1_angle_g_temp=999;
        m1_redcolor_dx=999;
        m1_redcolor=999;
        m1_greencolor_dx=999;
        m1_greencolor=999;
        expotime_g++;
        if(expotime_g<60)
             GateCamSetting();
        emit cameraStartedF_m(true);
        m1_iteration++;

        visionClass::frame2Mat(Cam_Front, m1_imgsrc);

        if (m1_iteration % m1_saveinterval == 0)
        {
            char imgnamesave[15] = "save1_";
            char countsave[10];
            sprintf( countsave, "%d", m1_iteration);
            strcat(imgnamesave, countsave);
            strcat(imgnamesave, ".jpg");
            imwrite(imgnamesave, m1_imgsrc);
        }
        else
            continue;


/*****************************************************************template**************************************************************/

        m1_heightscaled = m1_imgsrc.rows * m1_imgscale;
        m1_widthscaled = m1_imgsrc.cols * m1_imgscale;
        resize( m1_imgsrc, m1_imgsrc, Size(), m1_imgscale, m1_imgscale);


        Mat m1_imgROI(m1_imgsrc, Rect(0,0.1*m1_heightscaled,m1_widthscaled,0.8*m1_heightscaled));
        m1_imgsrc=m1_imgROI;
        m1_imgresult = m1_imgsrc.clone();

#pragma omp parallel sections
            {
#pragma omp section
                {
                templatematch_vector( m1_imgsrc, m1_imgresult, m1_TemplVecG, m1_MatchResultG);
                if(m1_MatchResultG.valid == true)
                {  m1_greentemp_dx = ( 0.9*m1_imgsrc.cols - 0.5*(m1_MatchResultG.tl_x + m1_MatchResultG.br_x));
                   m1_greentemp= 0.5*(m1_MatchResultG.tl_x + m1_MatchResultG.br_x);}
                else
                { m1_greentemp_dx= 999;
                  m1_greentemp=999;}

                }
#pragma omp section
                {
                templatematch_vector( m1_imgsrc, m1_imgresult, m1_TemplVecR, m1_MatchResultR);
                if(m1_MatchResultR.valid == true)
                {  m1_redtemp_dx= ( 0.1*m1_imgsrc.cols - 0.5*(m1_MatchResultR.tl_x + m1_MatchResultR.br_x));
                   m1_redtemp= 0.5*(m1_MatchResultR.tl_x + m1_MatchResultR.br_x);}
                else
                { m1_redtemp_dx = 999;
                  m1_redtemp=999;}

                }
#pragma omp section
                {
                templatematch_vector( m1_imgsrc, m1_imgresult, m1_TemplVecA_R, m1_MatchResultA_R);
                if(m1_MatchResultA_R.valid == true)
                {  m1_angle_r_temp_dx= ( 0.1*m1_imgsrc.cols - 0.5*(m1_MatchResultA_R.tl_x + m1_MatchResultA_R.br_x));
                   m1_angle_r_temp= 0.5*(m1_MatchResultA_R.tl_x + m1_MatchResultA_R.br_x);}
                else
                { m1_angle_r_temp_dx = 999;
                  m1_angle_r_temp=999;}

                }
#pragma omp section
                {
                templatematch_vector( m1_imgsrc, m1_imgresult, m1_TemplVecA_G, m1_MatchResultA_G);
                if(m1_MatchResultA_G.valid == true)
                {  m1_angle_g_temp_dx = ( 0.9*m1_imgsrc.cols - 0.5*(m1_MatchResultA_G.tl_x + m1_MatchResultA_G.br_x));
                   m1_angle_g_temp= 0.5*(m1_MatchResultA_G.tl_x + m1_MatchResultA_G.br_x);}
                else
                { m1_angle_g_temp_dx= 999;
                  m1_angle_g_temp=999;}
                }
            }


/**
        templatematch_vector( m1_imgsrc, m1_imgresult, m1_TemplVecA_G, m1_MatchResultA_G);
        if(m1_MatchResultA_G.valid == true)
           m1_black_dx = ( 0.5*m1_imgsrc.cols - 0.5*(m1_MatchResultA_G.tl_x + m1_MatchResultA_G.br_x));
        else
           m1_black_dx = 999;
        templatematch_vector( m1_imgsrc, m1_imgresult, m1_TemplVecA_R, m1_MatchResultA_R);
        if(m1_MatchResultA_R.valid == true)
           m1_black_dx = ( 0.5*m1_imgsrc.cols - 0.5*(m1_MatchResultA_R.tl_x + m1_MatchResultA_R.br_x));
        else
           m1_black_dx = 999;

        templatematch_vector( m1_imgsrc, m1_imgresult, m1_TemplVecG, m1_MatchResultG);
        if(m1_MatchResultG.valid == true)
           m1_black_dx = ( 0.5*m1_imgsrc.cols - 0.5*(m1_MatchResultG.tl_x + m1_MatchResultG.br_x));
        else
           m1_black_dx = 999;
        templatematch_vector( m1_imgsrc, m1_imgresult, m1_TemplVecR, m1_MatchResultR);
        if(m1_MatchResultR.valid == true)
           m1_black_dx = ( 0.5*m1_imgsrc.cols - 0.5*(m1_MatchResultR.tl_x + m1_MatchResultR.br_x));
        else
           m1_black_dx = 999;   */
        templatematch_vector( m1_imgsrc, m1_imgresult, m1_TemplVecB, m1_MatchResultB);
        if(m1_MatchResultB.valid == true)
           m1_black_dx = ( 0.5*m1_imgsrc.cols - 0.5*(m1_MatchResultB.tl_x + m1_MatchResultB.br_x));
        else
           m1_black_dx = 999;
        Rect red(m1_MatchResultR.minLoc, m1_MatchResultR.maxLoc);
        Rect green(m1_MatchResultG.minLoc, m1_MatchResultG.maxLoc);
        bool RED_valid = false;
        bool GREEN_valid =false;
        cv::Mat rPole = m1_imgsrc(red);
        cv::Mat gPole = m1_imgsrc(green);
        RED_valid = isColor(rPole, RED);
        GREEN_valid = isColor(gPole, GREEN);

/****************************************************************color red**************************************************************/

         // colorRecognize(m1_imgsrc, m1_imgranged,  m1_ColorThreshR, RED);
         //计算像素点数
         //如果像素点数大于阈值，进行矩形拟合
         m1_pixelsum = countNonZero( m1_imgranged);
         if( m1_pixelsum >= m1_pixelThresh)
         {
              //进行canny边缘检测，提高矩形拟合的准确度
              Canny( m1_imgranged, m1_imgcanny, 20, 50);
              rectBoundary( m1_imgcanny, m1_imgresult, m1_RectR,RED);
         }
        else
        {
            //切记：如果像素点数不满足阈值条件，也是valid=fasle的条件
            m1_RectR.valid = false;
        }

        //如果本次矩形拟合有效，....,if fails,set the value=999
        //当m1_redcolor_dx>0时，航行器应当向左平移；当m1_redcolor_dx<0时，航行器应当向右平移
        if( m1_RectR.valid == true && RED_valid)
        {
            m1_redcolor_dx = (0.2*m1_imgsrc.cols - m1_RectR.center_x);
            m1_redcolor=m1_RectR.center_x;
        }
        else
        {
            m1_redcolor_dx=999;
            m1_redcolor=999;
        }
/****************************************************************color green**************************************************************/
            // colorRecognize(m1_imgsrc, m1_imgranged,  m1_ColorThreshG, GREEN);
        m1_pixelsum = countNonZero( m1_imgranged);
        if( m1_pixelsum >= m1_pixelThresh)
        {
            Canny( m1_imgranged, m1_imgcanny, 20, 50);
            rectBoundary( m1_imgcanny, m1_imgresult, m1_RectG,GREEN);
        }
        else
        {
            m1_RectG.valid = false;
        }
        if( m1_RectG.valid == true && GREEN_valid)
        {
            m1_greencolor_dx = (0.8*m1_imgsrc.cols - m1_RectG.center_x) ;
            m1_greencolor=m1_RectG.center_x;
        }
        else
        {
            m1_greencolor_dx=999;
            m1_greencolor=999;
        }

/*****************************************final judges*****************************************************************************/
              int redlocation=999;
              int redlocation_dx=999;
              if(m1_angle_r_temp!=999)
              {
               redlocation=m1_angle_r_temp;
               redlocation_dx=m1_angle_r_temp_dx;
               }
              else if( m1_redtemp!=999)
              {
               redlocation=m1_redtemp;
               redlocation_dx=m1_redtemp_dx;
              }
              else if (m1_redcolor!=999)
              {
               redlocation=m1_redcolor;
               redlocation_dx=m1_redcolor_dx;
              }

              int greenlocation=999;
              int greenlocation_dx=999;
              if(m1_angle_r_temp!=999)
              {
               greenlocation=m1_angle_g_temp;
               greenlocation_dx=m1_angle_g_temp_dx;
              }
              else if( m1_greentemp!=999)
              {
               greenlocation=m1_greentemp;
               greenlocation_dx=m1_greentemp_dx;
              }
              else if (m1_redcolor!=999)
              {
               greenlocation=m1_greencolor;
               greenlocation_dx=m1_greencolor_dx;
              }

              if(abs(greenlocation-redlocation)<10)
              {
                  redlocation=999;
                  redlocation_dx=999;
                  greenlocation=999;
                  greenlocation_dx=999;
              }

               if (redlocation!=999 && greenlocation!=999)
                    data.m1_centerdx=(0.5*m1_imgsrc.cols-0.5*(redlocation+greenlocation))/m1_imgscale;
               else if (m1_black_dx!=999)
                data.m1_centerdx=m1_black_dx/m1_imgscale;
               else data.m1_centerdx=999;

               if (greenlocation!=999)
               data.m1_rightdx=greenlocation_dx/m1_imgscale;
               else  data.m1_rightdx=999;
               if (redlocation!=999)
               data.m1_leftdx=redlocation_dx/m1_imgscale;
               else  data.m1_leftdx=999;

               if(m1_angle_r_temp!=999 && m1_angle_g_temp!=999)
                   data.m1_angledx=(0.5*m1_imgsrc.cols-0.5*(m1_angle_r_temp+m1_angle_g_temp))/m1_imgscale;
               else
                   data.m1_angledx=999;

               if((m1_black_dx!=999 && redlocation!=999)||(m1_black_dx!=999 && greenlocation!=999)||(m1_angle_r_temp!=999 && m1_angle_g_temp!=999))
                   cnt_gateFound++;
               if ( cnt_gateFound>2)
                   data.m1_gateFound=true;





 /*****************************************display**********************************************************************************/
               char center_dx[10];
               sprintf(center_dx,"%d",data.m1_centerdx);
               putText( m1_imgresult, center_dx, Point( 150, 20), FONT_HERSHEY_SIMPLEX, 1, Scalar(255,0,0), 2, 8);

               char left_dx[10];
               sprintf(left_dx,"%d",data.m1_leftdx);
               putText( m1_imgresult, left_dx, Point( 20, 20), FONT_HERSHEY_SIMPLEX, 1, Scalar(255,0,0), 2, 8);

               char right_dx[10];
               sprintf(right_dx,"%d",data.m1_rightdx);
               putText( m1_imgresult, right_dx, Point(300, 20), FONT_HERSHEY_SIMPLEX, 1, Scalar(255,0,0), 2, 8);

               char angle_dx[10];
               sprintf(angle_dx,"%d",data.m1_angledx);
               putText( m1_imgresult, angle_dx, Point(300, 60), FONT_HERSHEY_SIMPLEX, 1, Scalar(255,0,0), 2, 8);




        if(v_fout&&v_fout.is_open())
        {

            v_fout<<"m1_iteration:" << m1_iteration<<std::endl;
            v_fout<<"green templ: "<<m1_greentemp<<std::endl;
            v_fout<<"green templ dx: "<<m1_greentemp_dx<<std::endl;
            v_fout<<"green color: "<<m1_greencolor<<std::endl;
            v_fout<<"green color dx: "<<m1_greencolor_dx<<std::endl;
            v_fout<<"red templ: "<<m1_redtemp<<std::endl;
            v_fout<<"red templ dx: "<<m1_redtemp_dx<<std::endl;
            v_fout<<"red color: "<<m1_redcolor<<std::endl;
            v_fout<<"red color dx: "<<m1_redcolor_dx<<std::endl;
            v_fout<<"right_dx: "<<data.m1_rightdx<<std::endl;
            v_fout<<"left_color: "<<data.m1_leftdx<<std::endl;
            v_fout<<"center_dx: "<<data.m1_centerdx<<std::endl;
            v_fout<<"angle_dx: "<<data.m1_angledx<<std::endl;
            v_fout<<"m1_gateFound"<<data.m1_gateFound<<std::endl;
         }

        qDebug()<<"m1_iteration:" << m1_iteration;
        qDebug()<<"center_dx"<<data.m1_centerdx;
         qDebug()<<"angle_dx"<<data.m1_angledx;


        emit imageShow(&m1_imgresult,"missionGate");
        if (m1_iteration % m1_saveinterval == 0)
        {
            char imgnamesave[15] = "save_m1_";
            char countsave[10];
            sprintf(countsave, "%d", m1_iteration);
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
        if(v_fout&&v_fout.is_open())
        {
            v_fout << "m1_start" << m1_start <<"\n"<<"\n"<<std::endl;
        }
        qDebug() << "m1_start" << m1_start << endl;
    }
    //相机停止采集
    qDebug()<<"1"<<endl;
    visionClass::stop(Cam_Front);
    emit cameraStoppedF(true);
    qDebug()<<"stop successfully!!!"<<endl;

    //销毁所有窗口
    emit destroyWindow();

    //程序退出之前，m1_runningState清0,便于主控查询
    data.m1_runningState = false;
    if(v_fout&&v_fout.is_open())
    {
        v_fout << "m1_runningState : " << data.m1_runningState <<"\n"<<"\n"<<"\n"<<std::endl;
    }
    return ;

}

vector<Mat> visionClass::loadTempl(int color, float imgscale)
{
    Mat imgtempl;
    int templcount=0;
    vector<Mat> TemplVec;
    while(1)
    {
        char imgname[100]="/home/nwpu/Code/Robosub_test2018-2-8/m1";
        if (color==BLACK)
         strcat( imgname, "b_");
        if (color==RED)
         strcat( imgname, "r_");
        if (color==GREEN)
         strcat( imgname, "g_");
        if (color==ANGLE_G)
         strcat( imgname, "ag_");
        if (color==ANGLE_R)
         strcat( imgname, "ar_");

        char count1[10];
        memset( count1, 0, sizeof(count1));
        templcount++;
        sprintf( count1, "%d", templcount);
        strcat( count1, ".jpg");
        strcat( imgname, count1);
        imgtempl = imread(imgname);
        if(imgtempl.data == 0)
        {
            templcount--;
            //如果没有模板载入，提示
            if(templcount == 0)
            {
                if(v_fout&&v_fout.is_open())
                {
                    v_fout << "No template has been loaded. Please check!\n";
                }
                qDebug() << "No template has been loaded. Please check!";
            }
            break;
        }
        else
        {
            //如果模板载入成功，进行缩放
            resize( imgtempl, imgtempl, Size(), imgscale, imgscale);
            TemplVec.push_back(imgtempl);
           if(v_fout&&v_fout.is_open())
            {
                v_fout << "Template" << templcount << endl << "height" << imgtempl.rows << " width" << imgtempl.cols<<"\n";
            }
            qDebug() << "Template" << templcount << endl << "height" << imgtempl.rows << " width" << imgtempl.cols;
        }
    }

    return TemplVec;
}
