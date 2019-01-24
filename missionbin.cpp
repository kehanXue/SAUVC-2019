#include "missionbin.h"
#include "visionclass.h"
#include <unistd.h>
#include <QApplication>

void visionClass::missionBin()
{
    // tell the main thread that this mission will start
    data.m5_runningState = true;
    qDebug()<<"missionbin state"<<data.m5_runningState<<endl;

    // start capturing
    visionClass::start(Cam_Bottom);
    binsetting();

    Mat m5_imgsrc;          // image from file or from camera
    Mat m5_imgblur;         // image after median blur
    Mat m5_imgres;          // result image
    Mat m5_imglid;          // image of lid after ranging
    Mat m5_imgbin;          // image of bin after ranging

    int m5_heightscaled = 580;      // height of an image after compression
    int m5_widthscaled = 780;       // width of an image after compression
    int m5_count1 = 0;              // the number of continuous images that the lib is vertical to the robot
    int m5_count2 = 0;              // the number of continuous images that a valid bin is found
    int m5_errbin = 0;              // record last distance between lid and bin
    int m5_switchstep = 0;
    int drop_count_true=0;
    int drop_count_copy=0;
    int bin_dx_copy=999;
    int bin_dy_copy=999;
    int bin_angle_copy=999;
    int bin_dx_true=999;
    int bin_dy_true=999;
    int bin_angle_true=999;
    int delay_count=0;
    int uncovered_locate=0;

    const float m5_imgscale = 0.5;              // compression rate
    const int m5_saveinterval = 3;            // image saving interval
    const int m5_psumlid_Thresh = 10000*m5_imgscale*m5_imgscale;        // threshold for non-zero points in m5_imglid
    const int m5_psumbin_Thresh = 10000*m5_imgscale*m5_imgscale;         // threshold for non-zero points in m5_imgbin
    const int m5_psumdrop_Thresh = 10000*m5_imgscale*m5_imgscale;       // threshold for non-zero points when dropping
    const int m5_count1_Thresh = 15;        // threshold for m5_count1
    const int m5_count2_Thresh = 3;        // threshold for m5_count2

    bool m5_findbin = false;            // if it's time to detect bin after finding the lid
    bool m5_stage = false;              // indicate stage, false to grab lid, true to drop torpedo
    data.m5_goback = false;             // true when the uncovered one is in front of the lid

    // threshold for orange lid
    m5_COLORTHRESH m5_colorThresh_lid;
    m5_colorThresh_lid.H_max = 255;   //58
    m5_colorThresh_lid.H_min = 96;  //124
    m5_colorThresh_lid.R_max = 131; //254
    m5_colorThresh_lid.R_min = 60;   //52
    m5_colorThresh_lid.B_max = 255;  //224
    m5_colorThresh_lid.B_min = 0;  //60

    // threshold for black bin
    m5_COLORTHRESH m5_colorThresh_bin;
    m5_colorThresh_bin.H_max = 255;
    m5_colorThresh_bin.H_min = 97;
    m5_colorThresh_bin.R_max = 18;
    m5_colorThresh_bin.R_min = 0;
    m5_colorThresh_bin.B_max = 58;
    m5_colorThresh_bin.B_min = 0;

    m5_COLORTHRESH m5_colorThresh_binw;
    m5_colorThresh_binw.H_max = 95;
    m5_colorThresh_binw.H_min = 0;
    m5_colorThresh_binw.R_max = 255;
    m5_colorThresh_binw.R_min = 157;
    m5_colorThresh_binw.B_max = 255;
    m5_colorThresh_binw.B_min = 0;

    // struct for bounding of lid
    m5_RECT m5_lidRect;
    m5_lidRect.areaLowThresh = 10000*m5_imgscale*m5_imgscale;
    m5_lidRect.areaUpThresh = 120000*m5_imgscale*m5_imgscale;
    m5_lidRect.disLowThresh = 200*m5_imgscale;
    m5_lidRect.hwratio_UpThresh = 3;
    m5_lidRect.hwratio_LowThresh = 1;
    m5_lidRect.valid = false;


    m5_RECT m5_binRect;
    m5_binRect.areaLowThresh = 10000*m5_imgscale*m5_imgscale;
    m5_binRect.areaUpThresh = 500*700*m5_imgscale*m5_imgscale;
    m5_binRect.disLowThresh = 120*m5_imgscale;
    m5_binRect.hwratio_UpThresh = 3;
    m5_binRect.hwratio_LowThresh = 1;
    m5_binRect.angleThresh = 70;
    m5_binRect.valid = false;

    // struct for bounding of bin and dropping torpedoes
    m5_RECT m5_dropRect;
    m5_dropRect.areaUpThresh = 500*700*m5_imgscale*m5_imgscale;
    m5_dropRect.areaLowThresh = 10000*m5_imgscale*m5_imgscale;
	m5_dropRect.disLowThresh = 120*m5_imgscale;
    m5_dropRect.hwratio_UpThresh = 3;
    m5_dropRect.hwratio_LowThresh = 1;
    m5_dropRect.angleThresh = 70;
    m5_dropRect.valid = false;

    // create windows
    emit createWindow("missionBin");
    emit createWindow("lid_Hranged");
    emit createWindow("lid_Rranged");
    emit createWindow("lid_Branged");
    emit createWindow("lid_ranged");
    emit createWindow("bin_Hranged");
    emit createWindow("bin_Rranged");
    emit createWindow("bin_Branged");
    emit createWindow("bin_ranged");
    emit createWindow("result");

    // create trackbar
    emit createBar("Hmax","lid_Hranged",&m5_colorThresh_lid.H_max,180);
    emit createBar("Hmin","lid_Hranged",&m5_colorThresh_lid.H_min,180);
    emit createBar("Rmax","lid_Rranged",&m5_colorThresh_lid.R_max,255);
    emit createBar("Rmin","lid_Rranged",&m5_colorThresh_lid.R_min,255);
    emit createBar("Bmax","lid_Branged",&m5_colorThresh_lid.B_max,255);
    emit createBar("Bmin","lid_Branged",&m5_colorThresh_lid.B_min,255);

    emit createBar("Hmax","bin_Hranged",&m5_colorThresh_bin.H_max,180);
    emit createBar("Hmin","bin_Hranged",&m5_colorThresh_bin.H_min,180);
    emit createBar("Rmax","bin_Rranged",&m5_colorThresh_bin.R_max,255);
    emit createBar("Rmin","bin_Rranged",&m5_colorThresh_bin.R_min,255);
    emit createBar("Bmax","bin_Branged",&m5_colorThresh_bin.B_max,255);
    emit createBar("Bmin","bin_Branged",&m5_colorThresh_bin.B_min,255);

    emit createBar("wHmax","bin_Hranged",&m5_colorThresh_binw.H_max,255);
    emit createBar("wHmin","bin_Hranged",&m5_colorThresh_binw.H_min,255);
    emit createBar("wRmax","bin_Rranged",&m5_colorThresh_binw.R_max,255);
    emit createBar("wRmin","bin_Rranged",&m5_colorThresh_binw.R_min,255);
    emit createBar("wBmax","bin_Branged",&m5_colorThresh_binw.B_max,255);
    emit createBar("wBmin","bin_Branged",&m5_colorThresh_binw.B_min,255);
    int expotime=0;

    while(m5_start)
    {
        expotime++;
        if(expotime<10)
            binsetting();
        // increase steps
        m5_iteration++;
        qDebug() << "This is step " << m5_iteration << endl;

        // get an image from file or from camera
        visionClass::frame2Mat(Cam_Bottom, m5_imgsrc);

        // rotate image due to installation fault of the bottom camera
        transpose(m5_imgsrc,m5_imgsrc);
        flip(m5_imgsrc,m5_imgsrc,1);

        // save images regularly
        // image saved should be original one without compression
        if(m5_iteration % m5_saveinterval == 0)
        {
            char name[15] = "save5_";
            char count[10];
            sprintf(count, "%d", m5_iteration);
            strcat(name, count);
            strcat(name, ".jpg");
            imwrite(name, m5_imgsrc);
        }

        // compress source image
        m5_heightscaled = m5_imgsrc.rows * m5_imgscale;
        m5_widthscaled = m5_imgsrc.cols * m5_imgscale;
        resize(m5_imgsrc, m5_imgsrc, Size(), m5_imgscale, m5_imgscale);

        // median blur
        medianBlur(m5_imgsrc, m5_imgblur, 5);
        emit imageShow( &m5_imgblur, "missionBin");

        // copy the source image to the result
        m5_imgres = m5_imgsrc.clone();

        // that's the first stage
        // we attempt to find the lid and remove it
        if(!m5_stage)
        {
            m5_colorRecognize_1(m5_imgblur, m5_imglid, m5_colorThresh_lid);
            if(countNonZero(m5_imglid) >= m5_psumlid_Thresh)
                m5_rectBoundary_lid(m5_imglid, m5_imgres, m5_lidRect);
            else
                m5_lidRect.valid = false;

            // at this stage, drive the robot to put the lid at the center of the image
            // if dx > 0, robot should go
            // if dy > 0, robot should go left sideway
            if (m5_lidRect.valid)
            {
                data.m5_lid_dx = (0.5*m5_imgsrc.cols +10- m5_lidRect.rect.center.x) / m5_imgscale;
                data.m5_lid_dy = (0.5*m5_imgsrc.rows +20- m5_lidRect.rect.center.y) / m5_imgscale;
                data.m5_lid_angle = m5_lidRect.angle;
                char lid_angle[10];
                sprintf(lid_angle, "%d", data.m5_lid_angle);
                putText(m5_imgres, lid_angle, Point(5,10), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0), 1, 8);

            }
            else
            {
                data.m5_lid_dx = 999;
                data.m5_lid_dy = 999;
                data.m5_lid_angle = 999;
            }

            if( abs(m5_lidRect.angle) > 85 && abs(data.m5_lid_dx) < 50 && abs(data.m5_lid_dy) < 30)
            {
                m5_count1++;
                if(m5_count1 >= m5_count1_Thresh)
                {
                    data.m5_lid_dx = -999;
                    data.m5_lid_dy = -999;
                    data.m5_lid_angle = -999;
                    m5_switchstep = m5_iteration;
                    if(uncovered_locate>=0)
                    data.m5_goback=true;
                    else
                    data.m5_goback=false;
                    m5_stage = true;

                }
            }
            else
            {
                m5_count1 = 0;
            }


            if( abs(m5_lidRect.angle) > 80 && abs(data.m5_lid_dx) < 100 && abs(data.m5_lid_dy) < 60)
            {
                m5_colorRecognize_2(m5_imgblur, m5_imgbin, m5_colorThresh_binw,1);
                qDebug()<<"indi  1"<<countNonZero(m5_imgbin);
                if( countNonZero(m5_imgbin) >= m5_psumbin_Thresh)
                    m5_rectBoundary_bin(m5_imgbin, m5_imgres, m5_binRect, m5_lidRect);
                else
                    m5_binRect.valid = false;

                qDebug()<<"indi  2"<<m5_binRect.valid;
                // indicate where is the bin, in front of or back of the lid
                int temp = m5_lidRect.rect.center.y - m5_binRect.center.y;
                if(m5_binRect.valid)
                    m5_errbin = temp;
                else
                    m5_errbin = 0;

                if(m5_errbin > 0)
                   uncovered_locate += 1;
                else
                   uncovered_locate -=1;
                qDebug()<<"uncovered_locate:"<<uncovered_locate;

            }
        }


         /*   // how many continuous images that the lid is vertical to the robot
            if( abs(m5_lidRect.angle) > 85 && abs(data.m5_lid_dx) < 50 && abs(data.m5_lid_dy) < 30)
            {
                m5_count1++;              
                if(m5_count1 >= m5_count1_Thresh)
                    m5_findbin = true;
            }
            else
            {
                m5_count1 = 0;
                m5_findbin = false;
            }

            // it's time to detect the bin
            // divide source image into two ROI, in front of and back of the lid respectively
            // pay attention to set m5_count2 as false if some conditions are not met
            if(m5_findbin)
            {
                m5_colorRecognize_2(m5_imgblur, m5_imgbin, m5_colorThresh_bin,0);
                qDebug()<<"indi  1"<<countNonZero(m5_imgbin);
                if( countNonZero(m5_imgbin) >= m5_psumbin_Thresh)
                    m5_rectBoundary_bin(m5_imgbin, m5_imgres, m5_binRect, m5_lidRect);
                else
                    m5_binRect.valid = false;

                qDebug()<<"indi  2"<<m5_binRect.valid;
                // indicate where is the bin, in front of or back of the lid
                int temp = m5_lidRect.rect.center.y - m5_binRect.center.y;
                if(m5_binRect.valid && (m5_errbin==0 || m5_errbin*temp > 0) )
                    m5_errbin = temp;
                else
                    m5_errbin = 0;

                if(m5_errbin != 0)
                {
                    qDebug()<<"m5_count2"<<m5_count2;
                    m5_count2++;
                    if(m5_count2 >= m5_count2_Thresh)
                    {
                        m5_stage = true;
                        data.m5_lid_dx = -999;
                        data.m5_lid_dy = -999;
                        data.m5_lid_angle = -999;

                        if(m5_errbin > 0)
                            data.m5_goback = true;
                        else
                            data.m5_goback = false;

                        m5_switchstep = m5_iteration;

                        // wait for several seconds to completely remove the lid
                      //  qDebug() << "Wait to completely remove the lid" << endl;
                        //QEventLoop loop;
                       // QTimer::singleShot(10000,&loop,SLOT(quit()));
                       // loop.exec();
                    }
                }
                else
                    m5_count2 = 0;
            }
            else
                m5_count2 = 0;
        }*/

        // that's the second stage
        // lid has been removed and then we will drop torpedoes
        if(m5_stage && (m5_iteration-m5_switchstep) > 100)
        {
            qDebug()<<"the second stage";
            m5_colorRecognize_2(m5_imgblur, m5_imgbin, m5_colorThresh_binw,1);
            if( countNonZero(m5_imgbin) >= m5_psumdrop_Thresh)
               {
                m5_rectBoundary_drop(m5_imgbin, m5_imgres, m5_dropRect, data.m5_goback,drop_count_true);
                char count[10];
                sprintf(count,"%d",drop_count_true);
                putText(m5_imgres,count,Point(100,20),FONT_HERSHEY_SIMPLEX,1,Scalar(0,0,255),2,8);
            }
            else
                m5_dropRect.valid = false;

            if (m5_dropRect.valid)
            {
                bin_dx_true= (0.5*m5_imgsrc.cols+10 - m5_dropRect.rect.center.x) / m5_imgscale;
                bin_dy_true = (0.5*m5_imgsrc.rows +20- m5_dropRect.rect.center.y) / m5_imgscale;
                bin_angle_true = m5_dropRect.angle;
                qDebug()<<"bin_dis"<<abs(bin_dy_copy-bin_dy_true);
                if(abs(bin_dy_copy-bin_dy_true)>100*m5_imgscale && bin_dx_copy!=999 &&delay_count==0)
                   {
                    delay_count=1;
                    putText(m5_imgres, "delay 1" , Point(40,100 ), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0), 1, 8);
                   }
                else
                {
                     delay_count=0;
                     bin_dx_copy=bin_dx_true;
                     bin_dy_copy=bin_dy_true;
                     bin_angle_copy=bin_angle_true;
                     drop_count_copy=drop_count_true;
                 }



                data.m5_bin_dx = bin_dx_copy;
                data.m5_bin_dy = bin_dy_copy;
                data.m5_bin_angle = bin_angle_copy;
                drop_count_copy=drop_count_true;
                char bin_angle[10];
                sprintf(bin_angle, "%d", data.m5_bin_angle);
                putText(m5_imgres, bin_angle, Point(25,10), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0), 1, 8);


            }
            else
            {
                data.m5_bin_dx = 999;
                data.m5_bin_dy = 999;
                data.m5_bin_angle = 999;
                bin_dx_true=999;
                bin_dy_true=999;
                bin_angle_true=999;
                delay_count=0;
            }
            qDebug()<<"bin_dx"<<data.m5_bin_dx;
            qDebug()<<"bin_dy"<<data.m5_bin_dy;
            qDebug()<<"bin_angle"<<data.m5_bin_angle;
            qDebug()<<"bin_dx_true"<<bin_dx_true;
            qDebug()<<"bin_dy_true"<<bin_dy_true;
            qDebug()<<"bin_angle_true"<<bin_angle_true;
            qDebug()<<"drop_count_true"<<drop_count_true;
            qDebug()<<"drop_count_copy"<<drop_count_copy;

        }




        // save images regularly
        // image saved should be original one without compression
        if(m5_iteration % m5_saveinterval == 0)
        {
            char name[20] = "save5_res_";
            char count[10];
            sprintf(count, "%d", m5_iteration);
            strcat(name, count);
            strcat(name, ".jpg");
            imwrite(name, m5_imgres);
        }

        // display source image
        emit imageShow( &m5_imgres, "result");
        qDebug() << "Stage : " << m5_stage << endl;
        qDebug() << "lid_dx:" << data.m5_lid_dx << "  lid_dy:" << data.m5_lid_dy
                 << "  lid_angle:" << data.m5_lid_angle << " goback:" << data.m5_goback << endl;
        qDebug() << "bin_dx:" << data.m5_bin_dx << "  bin_dy:" << data.m5_bin_dy
                 << "  bin_angle:" << data.m5_bin_angle << endl;

        // add time stamp<< "lid_dx:" << data.m5_lid_dx << "  lid_dy:" << data.m5_lid_dy << "  lid_angle:" << data.m5_lid_angle << endl;
        timeval tempt;
        gettimeofday(&tempt,NULL);
        data.t_now=tempt.tv_sec*1000+tempt.tv_usec/1000;

        // wait for a while
        QEventLoop loop;
        QTimer::singleShot(100,&loop,SLOT(quit()));
        loop.exec();
    }

    // destroy all windows
    emit destroyWindow();
    // tell the main thread that this mission will stop
    data.m5_runningState = false;
    qDebug() << "m5_runningState : " << data.m5_runningState << endl;

    return;
}


void visionClass::m5_colorRecognize_1(const Mat &img_input, Mat &img_output, m5_COLORTHRESH &ColorThresh)
{
    Mat img_rgb, img_hsv;
    Mat img_temp1, img_temp2;
    static Mat ranged, Hranged, Rranged, Branged;
    vector<Mat> channels_RGB, channels_HSV;

    img_rgb = img_input.clone();
    cvtColor(img_rgb, img_hsv, COLOR_BGR2HSV);
    split(img_rgb, channels_RGB);
    split(img_hsv, channels_HSV);

    // use inRange for channel H, R, B respectively
    if( ColorThresh.H_max >= ColorThresh.H_min)
    {
        inRange( channels_HSV.at(0), Scalar( ColorThresh.H_min, 0, 0), Scalar( ColorThresh.H_max, 0, 0), Hranged);
    }
    else
    {
        inRange( channels_HSV.at(0), Scalar( ColorThresh.H_min, 0, 0), Scalar( 180, 0, 0), img_temp1);
        inRange( channels_HSV.at(0), Scalar( 0, 0, 0), Scalar( ColorThresh.H_max, 0, 0), img_temp2);
        bitwise_or( img_temp1, img_temp2, Hranged);
    }

    if( ColorThresh.B_max >= ColorThresh.B_min)
    {
        inRange( channels_RGB.at(0), Scalar( ColorThresh.B_min, 0, 0), Scalar( ColorThresh.B_max, 0, 0), Branged);
    }
    else
    {
        inRange( channels_RGB.at(0), Scalar( ColorThresh.B_min, 0, 0), Scalar( 255, 0, 0), img_temp1);
        inRange( channels_RGB.at(0), Scalar( 0, 0, 0), Scalar( ColorThresh.B_max, 0, 0), img_temp2);
        bitwise_or( img_temp1, img_temp2, Branged);
    }

    if( ColorThresh.R_max >= ColorThresh.R_min)
    {
        inRange( channels_RGB.at(2), Scalar( ColorThresh.R_min, 0, 0), Scalar( ColorThresh.R_max, 0, 0), Rranged);
    }
    else
    {
        inRange( channels_RGB.at(2), Scalar( ColorThresh.R_min, 0, 0), Scalar( 255, 0, 0), img_temp1);
        inRange( channels_RGB.at(2), Scalar( 0, 0, 0), Scalar( ColorThresh.R_max, 0, 0), img_temp2);
        bitwise_or( img_temp1, img_temp2, Rranged);
    }


    // combine results of separate channels
    bitwise_and(Hranged, Rranged, ranged);
    bitwise_and(Branged, ranged, ranged);

    // avoid impacts from edge
    ranged.rowRange(Range(0,1)).setTo(Scalar::all(0));
    ranged.rowRange(Range(ranged.rows-2,ranged.rows-1)).setTo(Scalar::all(0));
    ranged.colRange(Range(0,1)).setTo(Scalar::all(0));
    ranged.colRange(Range(ranged.cols-2,ranged.cols-1)).setTo(Scalar::all(0));

    // output result
    img_output = ranged.clone();

    Mat element=getStructuringElement(MORPH_RECT,Size(5,5));
    dilate(img_output,img_output,element);
    // display images
    emit imageShow(&Hranged, "lid_Hranged");
    emit imageShow(&Rranged, "lid_Rranged");
    emit imageShow(&Branged, "lid_Branged");
    emit imageShow(&ranged, "lid_ranged");

    return;
}


void visionClass::m5_colorRecognize_2(const Mat &img_input, Mat &img_output, m5_COLORTHRESH &ColorThresh,bool indicate)
{
    Mat img_rgb, img_hsv;
    Mat img_temp1, img_temp2;
    static Mat ranged, Hranged, Rranged, Branged,selective_channel;
    vector<Mat> channels_RGB, channels_HSV;

    img_rgb = img_input.clone();
    cvtColor(img_rgb, img_hsv, COLOR_BGR2HSV);
    split(img_rgb, channels_RGB);
    split(img_hsv, channels_HSV);
    if(indicate==0)
        selective_channel=channels_HSV.at(0);
    if(indicate==1)
        selective_channel=channels_HSV.at(1);
    // use inRange for channel H, R, B respectively
    if( ColorThresh.H_max >= ColorThresh.H_min)
    {
        inRange( selective_channel, Scalar( ColorThresh.H_min, 0, 0), Scalar( ColorThresh.H_max, 0, 0), Hranged);
    }
    else
    {
        inRange( selective_channel, Scalar( ColorThresh.H_min, 0, 0), Scalar( 255, 0, 0), img_temp1);
        inRange( selective_channel, Scalar( 0, 0, 0), Scalar( ColorThresh.H_max, 0, 0), img_temp2);
        bitwise_or( img_temp1, img_temp2, Hranged);
    }

    if( ColorThresh.B_max >= ColorThresh.B_min)
    {
        inRange( channels_RGB.at(0), Scalar( ColorThresh.B_min, 0, 0), Scalar( ColorThresh.B_max, 0, 0), Branged);
    }
    else
    {
        inRange( channels_RGB.at(0), Scalar( ColorThresh.B_min, 0, 0), Scalar( 255, 0, 0), img_temp1);
        inRange( channels_RGB.at(0), Scalar( 0, 0, 0), Scalar( ColorThresh.B_max, 0, 0), img_temp2);
        bitwise_or( img_temp1, img_temp2, Branged);
    }

    if( ColorThresh.R_max >= ColorThresh.R_min)
    {
        inRange( channels_RGB.at(2), Scalar( ColorThresh.R_min, 0, 0), Scalar( ColorThresh.R_max, 0, 0), Rranged);
    }
    else
    {
        inRange( channels_RGB.at(2), Scalar( ColorThresh.R_min, 0, 0), Scalar( 255, 0, 0), img_temp1);
        inRange( channels_RGB.at(2), Scalar( 0, 0, 0), Scalar( ColorThresh.R_max, 0, 0), img_temp2);
        bitwise_or( img_temp1, img_temp2, Rranged);
    }
    // combine results of separate channels
    bitwise_and(Hranged, Rranged, ranged);
    bitwise_and(Branged, ranged, ranged);

    // avoid impacts from edge
    ranged.rowRange(Range(0,1)).setTo(Scalar::all(0));
    ranged.rowRange(Range(ranged.rows-2,ranged.rows-1)).setTo(Scalar::all(0));
    ranged.colRange(Range(0,1)).setTo(Scalar::all(0));
    ranged.colRange(Range(ranged.cols-2,ranged.cols-1)).setTo(Scalar::all(0));

    // output result
    img_output = ranged.clone();

    // display images
    emit imageShow(&Hranged, "bin_Hranged");
    emit imageShow(&Rranged, "bin_Rranged");
    emit imageShow(&Branged, "bin_Branged");
    emit imageShow(&ranged, "bin_ranged");

    return;
}





void visionClass::m5_rectBoundary_lid(const Mat &img_input, Mat &img_output, m5_RECT &m5_Rect)
{
    // set valid to false and then find out if there is a valid one in this image
    m5_Rect.valid = false;

    // current maximal area of rectangles
    double maxarea = 0.0;
    int longer, shorter;

    Mat img_canny;
    vector< vector<Point> > contours;
    vector<Vec4i> hierarchy;
    RotatedRect rect;
    Point2f vertices[4];

    Canny(img_input, img_canny, 20, 50);
    findContours(img_canny, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
    for(int i = 0; i < contours.size(); i++)
    {
        // ignore contours that are too small
        if (contours.at(i).size() < 10)
            continue;
        else
            rect = minAreaRect(contours[i]);

        // find out which side is longer
        if(rect.size.height >= rect.size.width)
        {
            longer = rect.size.height;
            shorter = rect.size.width;
        }
        else
        {
            longer = rect.size.width;
            shorter = rect.size.height;
        }

        // check if there is a rectanlge that meets all the constraints
        // if its area is larger than any other valid one, record it
        if(rect.size.area() > maxarea && rect.size.area() > m5_Rect.areaLowThresh && rect.size.area() < m5_Rect.areaUpThresh/* &&
                longer > shorter*m5_Rect.hwratio_LowThresh && longer < shorter*m5_Rect.hwratio_UpThresh*/)
        {
            maxarea = rect.size.area();
            m5_Rect.valid = true;
            m5_Rect.rect = rect;
        }
    }

    // if this is a valid detection, modify angle and draw result
    if(m5_Rect.valid)
    {
        if(m5_Rect.rect.size.width > m5_Rect.rect.size.height)
            m5_Rect.angle = m5_Rect.rect.angle + 90;
        else
            m5_Rect.angle = m5_Rect.rect.angle;

        circle(img_output, m5_Rect.rect.center, 1, Scalar(255, 0, 0), 2);
        m5_Rect.rect.points(vertices);
        for (int i = 0; i < 4; i++)
            line(img_output, vertices[i], vertices[(i + 1) % 4], Scalar(255, 0, 0), 2);
    }

    return;
}




void visionClass::m5_rectBoundary_bin(const Mat &img_input, Mat &img_output, m5_RECT &m5_Rect, m5_RECT &m5_lidRect)
{
    // current maximal area of rectangles
    double maxarea = 0.0;

    int width = img_input.cols;
    int height = img_input.rows;
    int longer, shorter;
    double angle;

    Mat img_canny;
    vector< vector<Point> > contours;
    vector<Vec4i> hierarchy;

    RotatedRect rect;
    m5_ROTATEDRECT rectF;
    m5_ROTATEDRECT rectB;
    rectF.valid = false;
    rectB.valid = false;

    Point2f vertices[4];
    Point2f top;
    Point2f bottom;
    Point2f tlF, brF;
    Point2f tlB, brB;

    m5_lidRect.rect.points(vertices);
    top = vertices[0];
    bottom = vertices[0];
    for(int i = 0; i < 4; i++)
    {
        if(vertices[i].y < top.y)
            top = vertices[i];
        if(vertices[i].y > bottom.y)
            bottom = vertices[i];
    }

    // divide the original image into two parts, in front of the lid, back of the lid
    tlF = Point2f(0,0);
    brF = Point2f(width, 0.8*top.y);
    if(brF.y <= tlF.y)
        brF.y = tlF.y + 2;
    tlB = Point2f(0, 0.2*height+0.8*bottom.y);
    brB = Point2f(width, height);
    if(brB.y <= tlB.y)
        tlB.y = brB.y - 2;
    Mat img_inputF(img_input, Rect(tlF, brF));
    Mat img_inputB(img_input, Rect(tlB, brB));

    // deal with the forward part first
    maxarea = 0.0;
    Canny(img_inputF, img_canny, 20, 50);
    findContours(img_canny, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
    for(int i = 0; i < contours.size(); i++)
    {
        // ignore contours that are too small
        if (contours.at(i).size() < 10)
            continue;
        else
            rect = minAreaRect(contours[i]);

        // modify angle
        if(rect.size.width > rect.size.height)
            angle = rect.angle + 90;
        else
            angle = rect.angle;

        // find out which side is longer
        if(rect.size.height >= rect.size.width)
        {
            longer = rect.size.height;
            shorter = rect.size.width;
        }
        else
        {
            longer = rect.size.width;
            shorter = rect.size.height;
        }

        // find the largest rectangle that meets all contraints
        if(rect.size.area() > maxarea && rect.size.area() > m5_Rect.areaLowThresh &&
                rect.size.area() < m5_Rect.areaUpThresh /*&& abs(angle) > m5_Rect.angleThresh &&
                longer > shorter*m5_Rect.hwratio_LowThresh && longer < shorter*m5_Rect.hwratio_UpThresh*/)
        {
            rectF.valid = true;
            maxarea = rect.size.area();
            rectF.rect = rect;
        }
    }

    // then deal with the backward part
    maxarea = 0.0;
    Canny(img_inputB, img_canny, 20, 50);
    findContours(img_canny, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
    for(int i = 0; i < contours.size(); i++)
    {
        // ignore contours that are too small
        if (contours.at(i).size() < 10)
            continue;
        else
            rect = minAreaRect(contours[i]);

        // modify angle
        if(rect.size.width > rect.size.height)
            angle = rect.angle + 90;
        else
            angle = rect.angle;

        // find out which side is longer
        if(rect.size.height >= rect.size.width)
        {
            longer = rect.size.height;
            shorter = rect.size.width;
        }
        else
        {
            longer = rect.size.width;
            shorter = rect.size.height;
        }

        // find the largest rectangle that meets all contraints
        if(rect.size.area() > maxarea && rect.size.area() > m5_Rect.areaLowThresh &&
                rect.size.area() < m5_Rect.areaUpThresh/* && abs(angle) > m5_Rect.angleThresh &&
                longer > shorter*m5_Rect.hwratio_LowThresh && longer < shorter*m5_Rect.hwratio_UpThresh*/)
        {
            rectB.valid = true;
            maxarea = rect.size.area();
            rectB.rect = rect;
        }
    }

    // compare and decide which part to use
    if( !rectF.valid && !rectB.valid)
        m5_Rect.valid = false;
    else if( !rectB.valid)
    {
        m5_Rect.valid = true;
        m5_Rect.rect = rectF.rect;
        m5_Rect.center = tlF + rectF.rect.center;
    }
    else if( !rectF.valid)
    {
        m5_Rect.valid = true;
        m5_Rect.rect = rectB.rect;
        m5_Rect.center = tlB + rectB.rect.center;
    }
    else if( rectF.rect.size.area() >= rectB.rect.size.area())
    {
        m5_Rect.valid = true;
        m5_Rect.rect = rectF.rect;
        m5_Rect.center = tlF + rectF.rect.center;
    }
    else
    {
        m5_Rect.valid = true;
        m5_Rect.rect = rectB.rect;
        m5_Rect.center = tlB + rectB.rect.center;
    }

    // draw result if there is valid rectangle
    if(m5_Rect.valid)
        circle(img_output, m5_Rect.center, 2, Scalar(0, 0, 255), 2);

    return;
}


void visionClass::m5_rectBoundary_drop(const Mat &img_input, Mat &img_output, m5_RECT &m5_Rect, bool goback,int bin_num)
{
    // current maximal area of rectangles
    double maxarea1 = 0.0;
    double maxarea2 = 0.0;
    double angle = 0.0;
    double longer, shorter;

    //Mat img_inputF, img_inputB;
    Mat img_canny;
    vector< vector<Point> > contours;
    vector<Vec4i> hierarchy;
    vector<RotatedRect> resRect;

    Point2f vertices[4];
    RotatedRect rect;
    m5_ROTATEDRECT rect1, rect2;
    rect1.valid = false;
    rect2.valid = false;

    Canny(img_input, img_canny, 20, 50);
    findContours(img_canny, contours, hierarchy, RETR_LIST, CHAIN_APPROX_NONE, Point(0, 0));
    for(int i = 0; i < contours.size(); i++)
    {
        // ignore contours that are too small
        if (contours.at(i).size() < 10)
            continue;
        else
            rect = minAreaRect(contours[i]);

        // modify angle
        if(rect.size.width > rect.size.height)
            angle = rect.angle + 90;
        else
            angle = rect.angle;

        // find out which side is longer
        if(rect.size.height >= rect.size.width)
        {
            longer = rect.size.height;
            shorter = rect.size.width;
        }
        else
        {
            longer = rect.size.width;
            shorter = rect.size.height;
        }

        // if there is a rectangle that meets all contraints
        // check if it can be the largerst one or the second largest one
        if(rect.size.area() > m5_Rect.areaLowThresh &&
                rect.size.area() < m5_Rect.areaUpThresh/* && abs(angle) > m5_Rect.angleThresh &&
                longer > shorter*m5_Rect.hwratio_LowThresh && longer < shorter*m5_Rect.hwratio_UpThresh*/)
        {
            // if it can be the largest one
            if(rect.size.area() > maxarea1)
            {
                // if the largest one doesn't have a value
                if(!rect1.valid)
                {
                    maxarea1 = rect.size.area();
                    rect1.valid = true;
                    rect1.rect = rect;
                }
                // if the largest one has a value
                // chech if the second largest one needs to be update
                else if((rect.center-rect1.rect.center).dot(rect.center-rect1.rect.center) >= pow(m5_Rect.disLowThresh,2) )
                {
                    maxarea1 = rect.size.area();
                    maxarea2 = rect1.rect.size.area();
                    rect2.valid = true;
                    rect2.rect = rect1.rect;
                    rect1.rect = rect;
                }
                else
                {
                    maxarea1 = rect.size.area();
                    rect1.rect = rect;
                }
            }
            // if it can be the second largest one
            else if(rect.size.area() > maxarea2)
            {
                maxarea2 = rect.size.area();
                rect2.valid = true;
                rect2.rect = rect;
            }
        }
    }

    // compare and decide which is the once covered bin
    if(!rect1.valid && !rect2.valid)
       {
        m5_Rect.valid = false;
        bin_num=0;
        }
    else if(!rect2.valid)
    {
        m5_Rect.valid = true;
        m5_Rect.rect = rect1.rect;
        bin_num=1;
    }
    else if(goback)
    {
        bin_num=2;
        m5_Rect.valid = true;
        if(rect1.rect.center.y > rect2.rect.center.y)
            m5_Rect.rect = rect1.rect;
        else
            m5_Rect.rect = rect2.rect;
    }
    else
    {
        bin_num=2;
        m5_Rect.valid = true;
        if(rect1.rect.center.y < rect2.rect.center.y)
            m5_Rect.rect = rect1.rect;
        else
            m5_Rect.rect = rect2.rect;
    }

    // if there is a valid rectangle, modify angle and draw result
    if(m5_Rect.valid)
    {
        if(m5_Rect.rect.size.width > m5_Rect.rect.size.height)
            m5_Rect.angle = m5_Rect.rect.angle + 90;
        else
            m5_Rect.angle = m5_Rect.rect.angle;

        circle(img_output, m5_Rect.rect.center, 2, Scalar(0, 0, 255), 2);
        m5_Rect.rect.points(vertices);
        for (int i = 0; i < 4; i++)
            line(img_output, vertices[i], vertices[(i + 1) % 4], Scalar(0, 0, 255), 2);
    }

    return;
}
