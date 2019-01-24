#include "visionclass.h"
#include "camclass.h"
#include <unistd.h>
#include <QDebug>
#include <QApplication>

//#define AUTO

using cv::Size;
static const int MAX_TRY_COUNT=10;

//构造函数
//初始化各图像任务变量
visionClass::visionClass():
    timer(NULL),savetimer_f(NULL),savetimer_b(NULL),
    run_flag(true)
{
    //通信数据初始化
    data.m1_centerdx = 999;
    data.m1_leftdx= 999;
    data.m1_rightdx=999;
    data.m1_angledx=999;
    data.m2_drum_dx = 999;
    data.m2_drum_dy = 999;
    data.m3_ball_dx = 999;
    data.m3_ball_dy = 999;
    data.m3_drum_dx = 999;
    data.m3_drum_dy = 999;
    data.m3_ball_acquire=0;// 抓取成功（累计5帧）是-999 未成功（累计5帧）是999
    data.m3_rotate_cw_angle=999;
    data.m4_flare_dx = 999;

    data.m1_runningState = false;
    data.m1_gateFound=false;
    data.m2_runningState = false;
    data.m3_runningState = false;
    data.m3_acquireFinish = false;
    data.m4_runningState = false;

    m1_start = false;
    m2_start = false;
    m3_start = false;
    m4_start = false;
    save_start=false;

    //qRegisterMetaType<TASK_ID>("visionClass::missionSwitch");
}

visionClass::~visionClass()
{
    close();

    if(timer!=NULL){
        delete timer;
    }
    if(savetimer_f!=NULL){
        delete savetimer_f;
    }
    if(savetimer_b!=NULL){
        delete savetimer_b;
    }
}

void visionClass::GateCamSetting()
{
    //whiteBalance_Red(Cam_Front, 3.0);
    //whiteBalance_Blue(Cam_Front, 0.98);
    //exposureMax(Cam_Front,39253);
    exposureMode(Cam_Front, exposureMode::Auto);
    exposureAutoTarget(Cam_Front, 50);
    whiteBalanceMode(Cam_Front, whiteBalanceMode::Auto);
}

void visionClass::FlareCamSetting()
{
    //whiteBalance_Red(Cam_Front, 3.0);
    //whiteBalance_Blue(Cam_Front, 0.96);
    //exposureMax(Cam_Front,39253);
    exposureMode(Cam_Front, exposureMode::Auto);
    exposureAutoTarget(Cam_Front, 50);
    whiteBalanceMode(Cam_Front, whiteBalanceMode::Auto);
}

void visionClass::DropCamSetting()
{
    //whiteBalance_Red(Cam_Bottom, 3.0);
    //whiteBalance_Blue(Cam_Bottom, 1.35);
    //exposureMax(Cam_Bottom,37000);
    exposureMode(Cam_Bottom, exposureMode::Auto);
    exposureAutoTarget(Cam_Bottom, 30);
    whiteBalanceMode(Cam_Bottom, whiteBalanceMode::AutoOnce);
}

void visionClass::AcquireCamSetting()
{
    //whiteBalance_Red(Cam_Bottom, 3.0);
    //whiteBalance_Blue(Cam_Bottom, 1.35);
    //exposureMax(Cam_Bottom,37000);
    exposureMode(Cam_Bottom, exposureMode::Auto);
    exposureAutoTarget(Cam_Bottom, 35);
    whiteBalanceMode(Cam_Bottom, whiteBalanceMode::AutoOnce);
}

void visionClass::camInitialize()
{
    if( initialize() && setting() )
        qDebug() << "Two cameras have been prepared." << endl;
}


//图像任务启动函数
//启动指定的图像任务
void visionClass::mission_Start(TASKS_ID ms)
{

    switch (ms)
    {
    case Gate:
        m1_start = true;
        missionGate();
        break;

    case Drop:
        m2_start = true;
        missionDrop();
        break;

    case Acquire:
        m3_start = true;
        missionAcquire();
        break;

    case Flare:
        m4_start = true;
        missionFlare();
        break;
    }
}


//图像任务终止函数
//考虑到任意时刻最多只有一个图像任务在运行，因此该函数用于终止所有图像任务
void visionClass::mission_Off(TASKS_ID ms)
{
    switch (ms)
    {
    case ALL:
        m1_start=false;
        m2_start=false;
        m3_start=false;
        m4_start=false;
        break;
    case Gate:
        m1_start = false;
        qDebug()<<"Close m1"<<m1_start;
        break;

    case Drop:
        m2_start = false;
        qDebug()<<"Close m2"<<m2_start;
        break;

    case Acquire:
        m3_start = false;
        break;

    case Flare:
        m4_start = false;
        break;
    }
}

void visionClass::setDeep(float value)
{
    deepValue=value;
}

void visionClass::onCreateTimer()
{
    if(NULL==timer){
        timer=new QTimer;
    }
}

//主控索要数据函数
//主控可以获得一份通信数据的拷贝
visionClass::visionData visionClass::getData()
{
    return data;
}


//前视定时器存图函数
void visionClass::visionsave_photo_f(bool ok)
{
    if(!start_f)
        start(Cam_Front);
#ifdef AUTO
        if(savenum_f%5==0)
        {
            int AutoTarget=(savenum_f/5)%100;
            exposureAutoTarget(Cam_Front, AutoTarget);
        }
#endif
    if(frame2Mat(Cam_Front, img_front))
    {
        savenum_f++;
        char imgsave_f[100] = "/home/nwpu/Code/Robosub_test2018-2-8/record/photoF/";
        char save_f[10];
        char exposure_target_f[10];
        sprintf(save_f,"%d",savenum_f);
        strcat(imgsave_f, save_f);
        strcat(imgsave_f,"_");
        sprintf(exposure_target_f,"%d",get_exposureAutoTarget(Cam_Front));
        strcat(imgsave_f,exposure_target_f);
        strcat(imgsave_f, ".jpg");
        imwrite(imgsave_f, img_front);
    }
    else
     {
        qDebug()<<savenum_f<<"_f: Fail to save!!!"<<endl;
     }
}

//下视定时器存图函数
void visionClass::visionsave_photo_b(bool ok)
{
    if(!start_b)
        start(Cam_Bottom);
#ifdef AUTO
        if(savenum_b%5==0)
        {
            int AutoTarget=(savenum_f/5)%100;
            exposureAutoTarget(Cam_Bottom, AutoTarget);
        }
#endif
    if(frame2Mat(Cam_Bottom, img_bottom))
    {
        qDebug() << img_bottom.channels() << endl;
        savenum_b++;
        char imgsave_b[100] = "/home/nwpu/Code/Robosub_test2018-2-8/record/photoB/";
        char save_b[10];
        char exposure_target_b[30];
        sprintf(save_b,"%d",savenum_b);
        strcat(imgsave_b, save_b);
        strcat(imgsave_b,"_");
        sprintf(exposure_target_b,"%d",get_exposureAutoTarget(Cam_Bottom));
        strcat(imgsave_b,exposure_target_b);
        strcat(imgsave_b, ".jpg");
        imwrite(imgsave_b, img_bottom);
    }
    else
    {
        qDebug()<<savenum_b<<"_b: Fail to save!!!"<<endl;
    }

}

//数据输出文件
void visionClass::visionsave_debug(bool ok)
{
    if(ok) {
        QString filename;
        const QString recordBasePath="/home/nwpu/Code/America_test2.0/record/";
        filename="vision";

        //获取当前日期
        time_t clock0=std::time(NULL);
        std::tm * localTime=std::localtime(&clock0);

        filename+="-";
        filename+=QString::number(localTime->tm_year-2000+1900);
        filename+="-";
        filename+=QString::number(localTime->tm_mon+1);
        filename+="-";
        filename+=QString::number(localTime->tm_mday);
        filename+=".csv";
        QByteArray fullpath=(recordBasePath+filename).toLocal8Bit();

        if(access(fullpath.data(),0)!=-1)
        {
            v_fout.open(fullpath.data(),std::ios_base::out|std::ios_base::app);
        }
        else
        {
            v_fout.open(fullpath.data(),std::ios_base::out|std::ios_base::trunc);
        }

        if(!v_fout.is_open())
        {
            qDebug() << "Cannot open file "<< fullpath.data()<< ". Maybe there are some permission issues.\n";
            v_fout.close();
        }
    }
    else v_fout.close();
}

//图像界面

void visionClass::qpacketSize(int value)
{
    switch(value)
    {
    case 0:
        if ( PvAttrUint32Set(camera[camnum].Handle, "PacketSize", 1500) == ePvErrSuccess )
        {
            qDebug() << "PacketSize done." << endl;
        }
        else
        {
            qDebug() << "Fail to set the PacketSize." << endl;
        }

       /* if( PvAttrUint32Set(camera[cam_type].Handle, "StreamBytesPerSecond", 115000000) == ePvErrSuccess )
        {
            qDebug() << "StreamBytesPerSecond done." << endl;
        }
        else
        {
            qDebug() << "Fail to set StreamBytesPerSecond." << endl;
        }*/
        break;

    case 1:
        if ( PvAttrUint32Set(camera[camnum].Handle, "PacketSize", 4000) == ePvErrSuccess )
        {
            qDebug() << "PacketSize done." << endl;
        }
        else
        {
            qDebug() << "Fail to set the PacketSize." << endl;
        }

        /*if( PvAttrUint32Set(camera[cam_type].Handle, "StreamBytesPerSecond", 46000000) == ePvErrSuccess )
        {
            qDebug() << "StreamBytesPerSecond done." << endl;
        }
        else
        {
            qDebug() << "Fail to set StreamBytesPerSecond." << endl;

        }*/
        break;

    case 2:
        if ( PvAttrUint32Set(camera[camnum].Handle, "PacketSize", 8228) == ePvErrSuccess )
        {
            qDebug() << "PacketSize done." << endl;
        }
        else
        {
            qDebug() << "Fail to set the PacketSize." << endl;
        }

        /*if( PvAttrUint32Set(camera[camnum].Handle, "StreamBytesPerSecond", 115000000) == ePvErrSuccess )
        {
            qDebug() << "StreamBytesPerSecond done." << endl;
        }
        else
        {
            qDebug() << "Fail to set StreamBytesPerSecond." << endl;

        }*/
        break;
    }
}


void visionClass::qexposureMode(int value)
{
    switch(value)
    {
    case 0:
        if( PvAttrEnumSet(camera[camnum].Handle, "ExposureAuto", "Auto") == ePvErrSuccess)
        {
            qDebug() << "ExposureMode is Continuous." << endl;
        }
        else
        {
            qDebug() << "Fail to set the ExposureMode." <<  endl;
        }
        break;

    case 1:
        if( PvAttrEnumSet(camera[camnum].Handle, "ExposureAuto", "AutoOnce") == ePvErrSuccess)
        {
            qDebug() << "ExposureMode is Once." << endl;
        }
        else
        {
            qDebug() << "Fail to set the ExposureMode." <<  endl;
        }
        break;

    case 2:
        if( PvAttrEnumSet(camera[camnum].Handle, "ExposureAuto", "Manual") == ePvErrSuccess)
        {
            qDebug() << "ExposureMode is Off." << endl;
        }
        else
        {
            qDebug() << "Fail to set the ExposureMode." <<  endl;
        }
        break;
   }
}


void visionClass::qwhiteBalanceMode(int value)
{
    switch(value)
    {
    case 0:
        if( PvAttrEnumSet(camera[camnum].Handle, "BalanceWhiteAuto", "Auto") == ePvErrSuccess )
        {
            qDebug() << "WhiteBalanceMode is Continuous." << endl;
        }
        else
        {
            qDebug() << "Fail to set the WhiteBalanceMode." << endl;
        }
        break;

    case 1:
        if( PvAttrEnumSet(camera[camnum].Handle, "BalanceWhiteAuto", "AutoOnce") == ePvErrSuccess )
        {
            qDebug() << "WhiteBalanceMode is Once." << endl;
        }
        else
        {
            qDebug() << "Fail to set the WhiteBalanceMode." << endl;
        }
        break;

    case 2:
        if( PvAttrEnumSet(camera[camnum].Handle, "BalanceWhiteAuto", "Manual") == ePvErrSuccess )
        {
            qDebug() << "WhiteBalanceMode is Off." << endl;
        }
        else
        {
            qDebug() << "Fail to set the WhiteBalanceMode." << endl;
        }
        break;
    }

}


void visionClass::qexposureMax(int value)
{
    if (camnum == 0) {
    if( PvAttrUint32Set(camera[camnum].Handle, "ExposureAutoMax", value) == ePvErrSuccess)
    {
        qDebug() << "ExposureMax is " << value << endl;
    }
    else
    {
        qDebug() << "Fail to set the ExposureMax." << endl;
    }
    } else {
        while(CameraSetAEC(0, true) != API_OK);
    }
}


void visionClass::qwhiteBalance_Red(float value)
{
    if( PvAttrFloat32Set(camera[camnum].Handle, "WhitebalValueRed", value) == ePvErrSuccess )
    {
        qDebug() << "WhitebalValueRed is " << value << endl;
    }
    else
    {
        qDebug() << "Fail to set the WhitebalValueRed." << endl;
    }

}


void visionClass::qwhiteBalance_Blue(float value)
{
    if( PvAttrFloat32Set(camera[camnum].Handle, "WhitebalValueBlue", value) == ePvErrSuccess )
    {
        qDebug() << "WhitebalValueBlue is " << value << endl;
    }
    else
    {
        qDebug() << "Fail to set the WhitebalValueBlue." << endl;
    }

}


void visionClass::qexposureAutoTarget(int value)
{
    if( PvAttrUint32Set(camera[camnum].Handle, "ExposureAutoTarget", value) == ePvErrSuccess)
    {
        qDebug() << "ExposureAutoTarget is " << value << endl;
    }
    else
    {
        qDebug() << "Fail to set the ExposureAutoTarget." << endl;
    }
}


void visionClass::qcameraNum(int value)
{
    if(value==0)
    {
        camnum=1;
        qDebug() << "Camera is Cam_Front." << endl;
    }
    else if(value==1)
    {
        camnum=0;
        qDebug() << "Camera is Cam_Bottom." << endl;
    }

    else
        qDebug() << "Fail to set Camera." << endl;
}

void visionClass::onStartCameraF(int value)
{
    bool ok;
    if(!start_f)
        ok=start(Cam_Front);
    else ok=false;
    emit cameraStartedF(ok);
}

void visionClass::onStopCameraF(int value)
{
    bool ok;
    if(!start_b)
        ok=stop(Cam_Front);
    else ok=false;
    emit cameraStoppedF(ok);
}


void visionClass::onGetFrameF(Mat *imgSrc)
{
    if(frame2Mat(Cam_Front,*imgSrc))
    {
        emit frameGottenF(imgSrc);
        imshow("a",*imgSrc);
    }
    else
        qDebug()<<"Fail to get the photos!"<<endl;
}

void visionClass::onStartCameraB(int value)
{
    bool ok;
    ok=start(Cam_Bottom);
    emit cameraStartedB(ok);
}

void visionClass::onStopCameraB(int value)
{
    bool ok;
    ok=stop(Cam_Bottom);
    emit cameraStoppedB(ok);
}


void visionClass::onGetFrameB(Mat *imgSrc)
{
    if(frame2Mat(Cam_Bottom,*imgSrc))
        emit frameGottenB(imgSrc);
    else
        qDebug()<<"Fail to get the photos"<<endl;

}

void visionClass::stop_Thread()
{
    while(m_thread->isWorking())m_thread->stop();
    qDebug() << "Camera Thread Shut down." ;
}

/********************************* Function **********************************************/

void readTemps(std::vector<std::string> &temp_paths, std::vector<cv::Mat> &dst)
{
    dst.clear();
    for( std::vector<std::string>::iterator it = temp_paths.begin(); it != temp_paths.end(); ++it ){
        if ((*it)[0] == 'N'){
            qDebug() << "Did You Forget Change The Path ? ";
            break;
        }
        dst.push_back(cv::imread(*it));
    }
}





