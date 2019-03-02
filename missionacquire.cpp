#include "missionacquire.h"
#include "visionclass.h"
#include <unistd.h>
#include <QApplication>
#include <QEventLoop>
//??????

void visionClass::missionAcquire()
{
    //?????????????
    data.m3_runningState = true;
    data.m3_green_area=0;

    m3_testball = 0;

    //????
    Mat m3_imgsrc,					//???
        m3_hsv,
        m3_imgblur,					//??????
        m3_bgr_streched,			//????????RGB??
        m3_imgranged,				//?????????????
        m3g_imgranged,
        m3_imgcanny,				//?????????
        m3_imgROI;

    vector<Mat> m3_bgr_channels;	//BGR???

    // bool m3_start = true;				//??????????
    bool m3_camSetting1 = false;
    bool m3_camSetting2 = false;
    bool m3_camSetting3 = false;

    int m3_iteration = 0;				//??????
    int m3_pixelsum = 0;				//??????????
    int m3_camSetttingStart = 0;
    int m3_green_pixelsum=0;
    int m3_green_closenum = 0;
    int m3_green_farnum = 0;

    int ball_dx = 999;
    int ball_dy = 999;
    int drum_dx = 999;
    int drum_dy = 999;
    int ball_dx_copy = 999;
    int ball_dy_copy = 999;
    int ball_x=999;
    int ball_y=999;


    int cnt_delay=0;

    static const float m3_imgscale = 0.5;			//??????
    static const int m3_saveinterval = 3;			//??????
    static const int m3_pixelThresh = 20000 * m3_imgscale*m3_imgscale;	//???????????
    //static const int m3_green_closenum = 3;
    //static const int m3_green_farnum = 3;

    //??????????
    m3_COLORTHRESH colorThresh;
    colorThresh.RH_max = m3_RH_max;
    colorThresh.RH_min = m3_RH_min;
    colorThresh.RS_max = m3_RS_max;
    colorThresh.RS_min = m3_RS_min;
    colorThresh.RV_max = m3_RV_max;
    colorThresh.RV_min = m3_RV_min;

    //??????????
    m3_RECT m3_Rect;
    m3_Rect.heightwidthratio = 3;
    m3_Rect.area = 0;
    m3_Rect.areaLowThresh = 20000 * m3_imgscale*m3_imgscale;
    m3_Rect.valid = false;

    vector<Mat> m3_TemplVec;        //???
    int m3_templcount = 0;          //?????
    vector<Mat> m3_TemplVec_b;        //???
    int m3_templcount_b = 0;          //?????
    vector<Mat> m3_TemplVec_c;        //???
    int m3_templcount_c = 0;          //?????


    //static const int m3_templfindballcountThresh = 10;     //??????????????


    int m3_heightscaled = 0,            //?????????
        m3_widthscaled = 0;                 //?????????

    //int m3_templfindballcount = 0,     //??????????
    //	m3_areaupcount = 0;                 //??????????????
    int cnt_test1=0;
    int cnt_test2=0;

    //bool m3_templfindball = false;         //???????????ball

    m3_MATCH m3_MatchResult;
    m3_MatchResult.valid = false;

    //?????
    Mat m3_imgtempl;
    Mat m3_imgtempl_b;
    Mat m3_imgtempl_c;


    //?????????
    emit createWindow("missionggg");
    emit createWindow("imgROI");

    emit createWindow("imagesrc");
    emit createWindow("ranged");


    //????
    visionClass::start(Cam_Bottom);
    AcquireCamSetting();

   int expotime_a=0, ball_pixelsum = 0;
   Rect ret, prev, drum;
   cv::Mat drum_ranged;
   int RR_min = 0, RR_max = 52,
       RG_min = 0, RG_max = 48,
       RB_min = 18, RB_max = 160;
   int R_low = 6, G_low = 84, B_low = 0;
   int R_high = 63, G_high = 255, B_high = 177;
    emit createBar("RR_max","ranged",&RR_max,255);
    emit createBar("RR_min","ranged",&RR_min,255);
    emit createBar("RG_max","ranged",&RG_max,255);
    emit createBar("RG_min","ranged",&RG_min,255);
    emit createBar("RB_max","ranged",&RB_max,255);
    emit createBar("RB_min","ranged",&RB_min,255);

    emit createBar("R_high","ranged",&R_high,255);
    emit createBar("R_low","ranged",&R_low,255);
    emit createBar("G_high","ranged",&G_high,255);
    emit createBar("G_low","ranged",&G_low,255);
    emit createBar("B_high","ranged",&B_high,255);
    emit createBar("B_low","ranged",&B_low,255);
    while (m3_start)
    {
        expotime_a++;
        if(expotime_a<50)
              AcquireCamSetting();
        emit cameraStartedB_m(true);
        //??????+1
        m3_iteration++;

        frame2Mat(Cam_Bottom, m3_imgsrc);
        transpose(m3_imgsrc,m3_imgsrc);
        flip(m3_imgsrc,m3_imgsrc,0);

        /** Reset !!!!!!!!!!!!!!!!! **/
        ball_dx = 999;
        ball_dy = 999;
        drum_dx = 999;
        drum_dy = 999;

        m3_heightscaled = m3_imgsrc.rows * m3_imgscale;
        m3_widthscaled = m3_imgsrc.cols * m3_imgscale;

        //????
        resize(m3_imgsrc, m3_imgsrc, cv::Size(), m3_imgscale, m3_imgscale);

        cv::Scalar low(R_low, G_low, B_low), high(R_high, G_high, B_high);
        local_colorranged(m3_imgsrc, low, high, m3_imgranged, ret, prev);
        prev = ret;

        /** RGB **/
        inRange(m3_imgsrc, Scalar(RR_min, RG_min, RB_min), Scalar(RR_max, RG_max, RB_max), drum_ranged);
        rectBound_color(drum_ranged, drum);

        m3_pixelsum = countNonZero(drum_ranged); // for ball.
        ball_pixelsum = countNonZero(m3_imgranged);

        double hw =  (double)drum.width / (double)drum.height;
        if (m3_pixelsum > m3_pixelThresh && (drum.contains(ret.br()) && drum.contains(ret.tl())) && (hw > 0.5 && hw < 1.66)) {
            if(m3_testball == 0 ) {
                ball_dx = (ret.br().x + ret.tl().x) / 2.0;
                ball_dy = (ret.br().y + ret.tl().y) / 2.0;
                drum_dx = 0.5*m3_imgsrc.cols - (drum.br().x + drum.tl().x) / 2.0;
                drum_dy = 0.5*m3_imgsrc.rows - (drum.br().y + drum.tl().y) / 2.0;
            }
        }

        if(m3_pixelsum > m3_pixelThresh) {
            ball_dx = 999;
            ball_dy = 999;
            drum_dx = 0.5*m3_imgsrc.cols - (drum.br().x + drum.tl().x) / 2.0;
            drum_dy = 0.5*m3_imgsrc.rows - (drum.br().y + drum.tl().y) / 2.0;
        }








        data.m3_ball_dx = ball_dx;
        data.m3_ball_dy = ball_dy;
        data.m3_drum_dx = drum_dx;
        data.m3_drum_dy = drum_dy;

        char m3_drum_dx[10],
             m3_drum_dy[10],
             m3_ball_dx[10],
             m3_ball_dy[10];

        sprintf( m3_drum_dx, "%d", data.m3_drum_dx);
        sprintf( m3_drum_dy, "%d", data.m3_drum_dy);
        sprintf( m3_ball_dx, "%d", data.m3_ball_dx);
        sprintf( m3_ball_dy, "%d", data.m3_ball_dy );
        putText( m3_imgsrc, m3_drum_dx, Point( 20, 20), FONT_HERSHEY_SIMPLEX, 1, Scalar(255,0,0), 2, 8);
        putText( m3_imgsrc, m3_drum_dy, Point( 100, 20), FONT_HERSHEY_SIMPLEX, 1, Scalar(255,0,0), 2, 8);
        putText( m3_imgsrc, m3_ball_dx, Point( 20, 100), FONT_HERSHEY_SIMPLEX, 1, Scalar(100,0,0), 2, 8);
        putText( m3_imgsrc, m3_ball_dy, Point( 100, 100), FONT_HERSHEY_SIMPLEX, 1, Scalar(100,0,0), 2, 8);

        rectangle(m3_imgsrc, ret, Scalar(0, 255, 0), 2);
        rectangle(m3_imgsrc, drum, Scalar(0, 0, 255), 2);

        emit imageShow(&drum_ranged, "ranged");
        emit imageShow(&m3_imgranged, "ball_ranged");
        emit imageShow(&m3_imgsrc, "imagesrc");

        /*
        m3_bgr_streched=m3_imgsrc.clone();
        //????imgROI
        medianBlur(m3_imgsrc, m3_imgblur, 31);
        //???????????????????????????????????
        if (m3_testball == 0) {
            cnt_test1 = 0;
            cnt_test2 = 0;

            //????????????HSV
            colorRecognize(m3_imgblur, m3_imgranged, colorThresh);
            local_colorranged(m3_imgblur, m3_imgranged, ret, prev);

            //??????
            //?????????????????
            m3_pixelsum = countNonZero(m3_imgranged);
            if (m3_pixelsum >= m3_pixelThresh) {
                //??canny???????????????
                //Canny(m3_imgranged, m3_imgcanny, 20, 50);
                // rectBoundary(m3_imgranged, m3_bgr_streched, m3_Rect);
                //?????????????
                if(m3_downfinished==0) {
                    m3_camSetting2=false;
                    if( m3_Rect.valid && !m3_camSetting3) {
                        m3_camSetting3 = true;
                        m3_camSetttingStart = m3_iteration;
                    }
                    if( m3_camSetting3 && m3_iteration-m3_camSetttingStart < 10) {
                        AcquireCamSetting();
                    }
                    // templatematch_vector(m3_imgsrc, m3_bgr_streched, m3_TemplVec, m3_MatchResult);
                    if (m3_MatchResult.valid == true) {
                        ball_dx = (0.5*m3_imgsrc.cols - 0.5*(ret.tl().x + ret.br().x)) / m3_imgscale;
                        ball_dy = (0.45*m3_imgsrc.rows - 0.5*(ret.tl().y + ret.br().y)) / m3_imgscale;
                        rectangle(m3_bgr_streched, Point(288*m3_imgscale, 304*m3_imgscale), Point(292*m3_imgscale,308*m3_imgscale), Scalar(255, 255, 0), 2, 8, 0);
                    }
                    else {
                        //????????
                        ball_dx = 999;
                        ball_dy = 999;
                    }
                    ball_dx_copy=ball_dx;
                    ball_dy_copy=ball_dy;
                }  else if(m3_downfinished == 1) {
                    putText( m3_bgr_streched, "m3_downfinished true", Point( 20, 250), FONT_HERSHEY_SIMPLEX, 1, Scalar(255,0,0), 2, 8);
                    if( !m3_camSetting2) {
                        m3_camSetting2 = true;
                        m3_camSetttingStart = m3_iteration;
                    }
                    if( m3_camSetting2 && m3_iteration-m3_camSetttingStart < 10) {
                        AcquireCamSetting();
                        putText( m3_bgr_streched, "Bottom camera has been reset", Point( 20, 250), FONT_HERSHEY_SIMPLEX, 1, Scalar(255,0,0), 2, 8);
                        if(v_fout&&v_fout.is_open()) {
                            v_fout << "Bottom camera has been reset." <<std::endl;
                        }
                        qDebug() << "Bottom camera has been reset." << endl;
                    }
                    templatematch_vector(m3_imgsrc, m3_bgr_streched, m3_TemplVec_b, m3_MatchResult);
                    if (m3_MatchResult.valid == true) {
                        cnt_delay=0;
                        ball_dx = (0.5*m3_imgsrc.cols - 0.5*(m3_MatchResult.tl_x + m3_MatchResult.br_x)) / m3_imgscale;
                        ball_dy = (0.5*m3_imgsrc.rows- 0.5*(m3_MatchResult.tl_y + m3_MatchResult.br_y)) / m3_imgscale;
                        ball_dx_copy=ball_dx;
                        ball_dy_copy=ball_dy;
                        rectangle(m3_bgr_streched, Point(286*m3_imgscale, 336*m3_imgscale), Point(294*m3_imgscale,344*m3_imgscale), Scalar(255, 255, 0), 2, 8, 0);
                    }
                    else {
                        if(ball_dx_copy!=999 && cnt_delay<2) {
                            cnt_delay++;
                            //copy??
                        }
                        else {
                            //????????
                            ball_dx = 999;
                            ball_dy = 999;
                            ball_dx_copy=999;
                            ball_dy_copy=999;
                            cnt_delay=0;
                        }
                    }
                }
            } else {
                //???????????????????valid=false???
                m3_Rect.valid = false;
            }

            //???????????????????????
                //?m3_drum_dx>0?????????????m3_drum_dx<0???????????
            if (m3_Rect.valid == true) {    //???????m3_runningState?0,??????
                    data.m3_runningState = false;
                    double ball_center_x = 0.5*(m3_MatchResult.tl_x + m3_MatchResult.br_x);
                    double ball_center_y = 0.5*(m3_MatchResult.tl_y + m3_MatchResult.br_y);
                    Mat ROI_left,
                        ROI_right,
                        ROI_up,
                        ROI_down;
                    int point_left=0,
                        point_right=0,
                        point_up=0,
                        point_down=0;

                    data.m3_rotate_cw_angle =0;

                    data.m3_drum_dx = ((0.5*m3_imgsrc.cols - 0.5*(m3_Rect.tl_x + m3_Rect.br_x)) / m3_imgscale)+30;
                    data.m3_drum_dy = ((0.5*m3_imgsrc.rows - 0.5*(m3_Rect.tl_y + m3_Rect.br_y)) / m3_imgscale)+30;
                    data.m3_ball_dx = ball_dx_copy;
                    data.m3_ball_dy = ball_dy_copy;
            }  else { //????????????999
                data.m3_drum_dx = 999;
                data.m3_drum_dy = 999;
                data.m3_ball_dx = 999;
                data.m3_ball_dy = 999;
                ball_dx = 999;
                ball_dy = 999;
                ball_dx_copy=999;
                ball_dy_copy=999;
                cnt_delay=0;
            }

            char m3_drum_dx[10],
                 m3_drum_dy[10],
                 m3_ball_dx[10],
                 m3_ball_dy[10],
                 m3_confidence[10];

            sprintf( m3_drum_dx, "%d", data.m3_drum_dx);
            sprintf( m3_drum_dy, "%d", data.m3_drum_dy);
            sprintf( m3_ball_dx, "%d", data.m3_ball_dx);
            sprintf( m3_ball_dy, "%d", data.m3_ball_dy );
            sprintf( m3_confidence, "%f", m3_MatchResult.confidence);
            putText( m3_bgr_streched, m3_drum_dx, Point( 20, 20), FONT_HERSHEY_SIMPLEX, 1, Scalar(255,0,0), 2, 8);
            putText( m3_bgr_streched, m3_drum_dy, Point( 100, 20), FONT_HERSHEY_SIMPLEX, 1, Scalar(255,0,0), 2, 8);
            putText( m3_bgr_streched, m3_ball_dx, Point( 20, 100), FONT_HERSHEY_SIMPLEX, 1, Scalar(100,0,0), 2, 8);
            putText( m3_bgr_streched, m3_ball_dy, Point( 100, 100), FONT_HERSHEY_SIMPLEX, 1, Scalar(100,0,0), 2, 8);
           // putText( m3_bgr_streched, m3_confidence, Point( 20, 200), FONT_HERSHEY_SIMPLEX, 1, Scalar(255,0,0), 2, 8);

            emit imageShow(&m3_bgr_streched,"missionggg");

            //??????
            if(v_fout&&v_fout.is_open())
            {
                v_fout << "m3_iteration:" << m3_iteration <<std::endl;
                v_fout << "m3_pixelsum" << m3_pixelsum <<std::endl;
                v_fout << "m3_maxArea:" << m3_Rect.area <<std::endl;
                v_fout << "m3_drum_dx:" << data.m3_drum_dx <<std::endl;
                v_fout << "m3_drum_dy:" << data.m3_drum_dy <<std::endl;
                v_fout << "m3_ball_dx:" << data.m3_ball_dx <<std::endl;
                v_fout << "m3_ball_dy:" << data.m3_ball_dy <<std::endl;
                v_fout << "ball_dx_copy:" << ball_dx_copy <<std::endl;
                v_fout << "ball_dy_copy:" << ball_dy_copy <<std::endl;
                v_fout << "ball_dx:" << ball_dx <<std::endl;
                v_fout << "ball_dy:" << ball_dy <<std::endl;
                v_fout << "cnt_delay:" << cnt_delay <<std::endl;

            }
            qDebug() << "m3_iteration:" << m3_iteration << endl;
            qDebug() << "m3_pixelsum" << m3_pixelsum << endl;
            qDebug() << "m3_maxArea:" << m3_Rect.area << endl;
            qDebug() << "m3_drum_dx:" << data.m3_drum_dx << endl;
            qDebug() << "m3_drum_dy:" << data.m3_drum_dy << endl;
            qDebug() << "m3_ball_dx:" << data.m3_ball_dx << endl;
            qDebug() << "m3_ball_dy:" << data.m3_ball_dy << endl;
            qDebug() << "ball_dx_copy:" << ball_dx_copy << endl;
            qDebug() << "ball_dy_copy:" << ball_dy_copy << endl;
            qDebug() << "ball_dx:" << ball_dx << endl;
            qDebug() << "ball_dy:" << ball_dy << endl;
            qDebug() << "cnt_delay:" << cnt_delay << endl;

        } // endif (m3_testball == 0)
        else {
            data.m3_acquireFinish=true;// ????????
            data.m3_ball_acquire=0;
            //???????
            //rect(x,y,width,height) x+width<580 y+height<780
            m3_imgROI=m3_imgsrc(Rect(200 * m3_imgscale, 394*m3_imgscale, 227*m3_imgscale, 186*m3_imgscale));
            templatematch_vector(m3_imgROI, m3_imgROI, m3_TemplVec_c, m3_MatchResult);

            emit imageShow(&m3_imgROI,"img_ROI");
            if (m3_MatchResult.valid == true) {
                //??????
                ball_x = 0.5*(m3_MatchResult.tl_x + m3_MatchResult.br_x);
                ball_y = 0.5*(m3_MatchResult.tl_y + m3_MatchResult.br_y);
                if(v_fout&&v_fout.is_open()) {
                    v_fout << "ball_x" << ball_x << "   ball_y" << ball_y <<std::endl;
                }
                qDebug() << "ball_x" << ball_x << "   ball_y" << ball_y << endl;
                if (ball_x >0* m3_imgscale && ball_x < 227* m3_imgscale && ball_y>0 * m3_imgscale && ball_y < 186 * m3_imgscale) {
                    cnt_test1++;//??
                    cnt_test2 = 0;
                } else {
                    cnt_test2++;
                    cnt_test1 = 0;
                }
            } else {
                cnt_test2++;
                cnt_test1 = 0;
            }
            if(v_fout&&v_fout.is_open()) {
                v_fout<<"cnt_test1"<<cnt_test1<<std::endl;
                v_fout<<"cnt_test2"<<cnt_test2<<std::endl;
            }
            qDebug()<<"cnt_test1"<<cnt_test1;
            qDebug()<<"cnt_test2"<<cnt_test2;
            if (cnt_test1>5) {
                data.m3_ball_acquire=-999;
                if(v_fout&&v_fout.is_open())
                {
                    v_fout <<"????"<<std::endl;
                }
                qDebug() << "????" << endl;
                m3_start=false;
            }
            if (cnt_test2>13) {
                data.m3_ball_acquire=999;
                if(v_fout&&v_fout.is_open())
                {
                    v_fout <<"?????"<<std::endl;
                }
                qDebug() << "?????" << endl;
            }
        } // end else
        **/
        timeval tempt;
        gettimeofday(&tempt,NULL);
        data.t_now=tempt.tv_sec*1000+tempt.tv_usec/1000;
        //???
        QEventLoop loop;
        QTimer::singleShot(50,&loop,SLOT(quit()));
        loop.exec();
        if(v_fout&&v_fout.is_open())
        {
             v_fout << "m3_start : " << m3_start<<"\n"<<"\n"<<std::endl;
        }
        qDebug() << "m3_start : " << m3_start << endl;
    } // while end

    //??????
    // stop(Cam_Bottom);
    emit cameraStoppedB(true);

    //??????
    emit destroyWindow();
    //???????m3_runningState?0,??????
    data.m3_runningState = false;
    if(v_fout&&v_fout.is_open()) {
         v_fout << "m3_runningState : " << data.m3_runningState<<"\n"<<"\n"<<"\n"<<std::endl;
    }
    qDebug() << "m3_runningState : " << data.m3_runningState << endl;
    return ;
}

void visionClass::onGetHandclosed(bool ok) {
     m3_testball = ok;
}

void visionClass::onGodownFinished(bool ok) {
     m3_downfinished = ok;
}
