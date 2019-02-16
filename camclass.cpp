#include "camclass.h"

// constructor
camClass::camClass()
{
}


// deconstructor
camClass::~camClass()
{
}


bool camClass::initialize()
{
    // initialize the API
    if( PvInitialize() != 0)
    {
        qDebug() << "Fail to initialize the API" << endl;
        return false;
    }

    // check if there are enough cameras wanted
    // try MAX_TRY_COUNT times at most
    int temp = 0, camera_cnt = 0;
    qDebug()<<PvCameraCount()<<"sssssssssssssssssss";
    while( camera_cnt = PvCameraCount() != CAMERAS_WANTED && temp < MAX_TRY_COUNT )
    {
        usleep(500000);
        temp++;
    }
    if( temp == MAX_TRY_COUNT)
    {
        if(CAMERAS_WANTED==1)
        {
            qDebug() << "Fail to find cameras of specified number." << endl;
            return false;
        }
        if(CAMERAS_WANTED==2)
        {
            if( PvCameraCount()==0 )
            {
                qDebug() << "Fail to find all cameras." << endl;
                return false;
            }
            if( PvCameraCount()==1 )
            {
                qDebug() << "Fail to find another camera." << endl;
                return false;
            }
        }
    }
    else
    {
        qDebug() << CAMERAS_WANTED << " cameras found." << endl;
    }

    /**
      Never change it until you buy another manta Camera !!!!!!!!!!!!!!!!
      **/
    if( CAMERAS_WANTED==1 )
    {
        tPvCameraInfoEx   camInfo[1];
        PvCameraListEx(camInfo, CAMERAS_WANTED, NULL, sizeof(tPvCameraInfoEx) );
        for (int i = 0; i < CAMERAS_WANTED; i++)
            camera[i].UID = camInfo[i].UniqueId;

        // open these cameras
        if( PvCameraOpen( camera[0].UID, ePvAccessMaster, &(camera[0].Handle)) == ePvErrSuccess )
        {
            if(camera[0].UID==Cam_Front)
                qDebug() << "Front_camera opened." << endl;
            if(camera[0].UID==Cam_Bottom)
                qDebug() << "Bottom_camera opened." << endl;
        }
        else
        {
            qDebug() << "Fail to open all the cameras." << endl;
            return false;
        }

        // configure common settings for these cameras
        // AcquisitionMode is set to be continuous
        if( PvAttrEnumSet(camera[0].Handle, "AcquisitionMode", "Continuous") == ePvErrSuccess )
        {
            qDebug() << "Acquisition Mode done." << endl;
        }
        else
        {
            qDebug() << "Fail to set the Acquisition Mode." << endl;
            return false;
        }

        // image format is set to be Bgr24
        if( PvAttrEnumSet(camera[0].Handle, "PixelFormat", "Bgr24") == ePvErrSuccess )
        {
            qDebug() << "Pixel Format done." <<  endl;
        }
        else
        {
            qDebug() << "Fail to set the Pixel Format." <<  endl;
            return false;
        }

        // set the buffer to store frames temporarily
        tPvUint32 frameSize;
        PvAttrUint32Get(camera[0].Handle, "TotalBytesPerFrame", &frameSize);
        memset(&camera[0].Frame, 0, sizeof(tPvFrame));
        camera[0].Frame.ImageBufferSize = frameSize;
        camera[0].Frame.ImageBuffer = new char[frameSize];

        // set the PacketSize, 8228 implicitly
        //if( !packetSize(camera[0].UID, packetSize::Jumbo_8K))
       if( !packetSize(camera[0].UID, packetSize::No_Jumbo) )
            return false;

    }
    /** } **/

    /**
     * @brief 这里使用了新的相机，是下视的初始化函数。
     */
    int count;
    CameraGetCount(&count);
    if (count > 0) {
        CameraInit(0);
        CameraGetGPIO(0, &camB);

        CameraSetStrobePolarity(0, 0);
        CameraSetTriggerPolarity(0, 0);
        CameraSetSnapMode(0, 0);
        CameraSetTriggerPolarity(0, 0);
        CameraSetGPIO(0, CAMERA_IO_OUT, CAMERA_IO_OUT);
        // while(CameraSetResolution(0, 4, &camB_width, &camB_height) != API_OK);
        m_thread = new CaptureThread(this);
        // qDebug() << "init ok 23333333333333333333333333333";
        connect(m_thread, SIGNAL(captured(QImage, IplImage*)), this, SLOT(process_Cam_B(QImage, IplImage *)));
    } else {
        qDebug() << "No Camera Found !!!!!!!!!!!" ;
        return false;
    }
    while(!m_thread ->isWorking()) {
        m_thread -> stream();
        m_thread -> start();
    }
    /**
    if( CAMERAS_WANTED==2 )
    {
        tPvCameraInfoEx   camInfo[2];
        PvCameraListEx(camInfo, CAMERAS_WANTED, NULL, sizeof(tPvCameraInfoEx) );
        for (int i = 0; i < CAMERAS_WANTED; i++)
            camera[i].UID = camInfo[i].UniqueId;

        // open these cameras
        if( PvCameraOpen( camera[0].UID, ePvAccessMaster, &(camera[0].Handle)) == ePvErrSuccess
                    && PvCameraOpen( camera[1].UID, ePvAccessMaster, &(camera[1].Handle)) == ePvErrSuccess)
        {
            qDebug() << "Cameras opened." << endl;
        }
        else
        {
            qDebug() << "Fail to open all the cameras." << endl;
            return false;
        }

        // configure common settings for these cameras
        // AcquisitionMode is set to be continuous
        if( PvAttrEnumSet(camera[0].Handle, "AcquisitionMode", "Continuous") == ePvErrSuccess
                && PvAttrEnumSet(camera[1].Handle, "AcquisitionMode", "Continuous") == ePvErrSuccess)
        {
            qDebug() << "Acquisition Mode done." << endl;
        }
        else
        {
            qDebug() << "Fail to set the Acquisition Mode." << endl;
            return false;
        }

        // image format is set to be Bgr24
        if( PvAttrEnumSet(camera[0].Handle, "PixelFormat", "Bgr24") == ePvErrSuccess
                && PvAttrEnumSet(camera[1].Handle, "PixelFormat", "Bgr24") == ePvErrSuccess)
        {
            qDebug() << "Pixel Format done." <<  endl;
        }
        else
        {
            qDebug() << "Fail to set the Pixel Format." <<  endl;
            return false;
        }

        // set the buffer to store frames temporarily
        tPvUint32 frameSize;
        PvAttrUint32Get(camera[0].Handle, "TotalBytesPerFrame", &frameSize);
        memset(&camera[0].Frame, 0, sizeof(tPvFrame));
        camera[0].Frame.ImageBufferSize = frameSize;
        camera[0].Frame.ImageBuffer = new char[frameSize];
        memset(&camera[1].Frame, 0, sizeof(tPvFrame));
        camera[1].Frame.ImageBufferSize = frameSize;
        camera[1].Frame.ImageBuffer = new char[frameSize];

        // set the PacketSize, 8228 implicitly
        //if( !packetSize(Cam_Front, packetSize::Jumbo_8K) || !packetSize(Cam_Bottom, packetSize::Jumbo_8K))
       if( !packetSize(Cam_Front, packetSize::No_Jumbo) || !packetSize(Cam_Bottom, packetSize::No_Jumbo) )
            return false;
    }*/
    // All Done
    return true;
}

bool camClass::packetSize(int UID, packetSize::SIZE mode)
{
    // find out the camera specified by the UID
    int cam_type = 0;
    for( ; cam_type < CAMERAS_WANTED; cam_type++)
        if(camera[cam_type].UID == UID)
            break;

    switch(mode)
    {
    case packetSize::No_Jumbo:
        if ( PvAttrUint32Set(camera[cam_type].Handle, "PacketSize", 1500) == ePvErrSuccess )
        {
            qDebug() << "PacketSize done." << endl;
        }
        else
        {
            qDebug() << "Fail to set the PacketSize." << endl;
            return false;
        }

       /* if( PvAttrUint32Set(camera[cam_type].Handle, "StreamBytesPerSecond", 115000000) == ePvErrSuccess )
        {
            qDebug() << "StreamBytesPerSecond done." << endl;
        }
        else
        {
            qDebug() << "Fail to set StreamBytesPerSecond." << endl;
            return false;
        }*/
        break;

    case packetSize::Jumbo_4K:
        if ( PvAttrUint32Set(camera[cam_type].Handle, "PacketSize", 4000) == ePvErrSuccess )
        {
            qDebug() << "PacketSize done." << endl;
        }
        else
        {
            qDebug() << "Fail to set the PacketSize." << endl;
            return false;
        }

        /*if( PvAttrUint32Set(camera[cam_type].Handle, "StreamBytesPerSecond", 46000000) == ePvErrSuccess )
        {
            qDebug() << "StreamBytesPerSecond done." << endl;
        }
        else
        {
            qDebug() << "Fail to set StreamBytesPerSecond." << endl;
            return false;
        }*/
        break;

    case packetSize::Jumbo_8K:
        if ( PvAttrUint32Set(camera[cam_type].Handle, "PacketSize", 8228) == ePvErrSuccess )
        {
            qDebug() << "PacketSize done." << endl;
        }
        else
        {
            qDebug() << "Fail to set the PacketSize." << endl;
            return false;
        }

        /*if( PvAttrUint32Set(camera[cam_type].Handle, "StreamBytesPerSecond", 115000000) == ePvErrSuccess )
        {
            qDebug() << "StreamBytesPerSecond done." << endl;
        }
        else
        {
            qDebug() << "Fail to set StreamBytesPerSecond." << endl;
            return false;
        }*/
        break;
    }

    return true;
}


bool camClass::setting()
{
    if( !whiteBalanceMode(Cam_Front, whiteBalanceMode::Manual) )
        return false;
    if( !exposureMax(Cam_Front, 843919) )
        return false;
    // if( !exposureMode(Cam_Front, exposureMode::AutoOnce)  )
        // return false;
    if( !saturation(Cam_Front, 1.5) )
        return false;
    if( !gain(Cam_Front, 2.0) )
        return false;

    if( !frameHeightWidth(Cam_Front)  )
        return false;

    if( !exposureMode(Cam_Bottom, exposureMode::AutoOnce)  )
        return false;

    if( !whiteBalanceMode(Cam_Bottom, whiteBalanceMode::Auto) )
        return false;

    if( !exposureMax(Cam_Bottom, 35000) )
        return false;

    if( !gain(Cam_Bottom, 100)  )
        return false;

    /**
      * 因为相机问题修改 2019-1-16
      */
    /**
    if( CAMERAS_WANTED==1 )
    {
        if( !whiteBalanceMode(camera[0].UID, whiteBalanceMode::AutoOnce) )
            return false;
        if( !exposureMax(camera[0].UID, 800000) )
            return false;
        if( !exposureMode(camera[0].UID, exposureMode::AutoOnce) )
            return false;
        if( !gain(camera[0].UID, 0.0) )
            return false;
        if( !saturation(camera[0].UID, 1.0) )
            return false;
        if( !frameHeightWidth(camera[0].UID) )
            return false;
    }
    if( CAMERAS_WANTED==2 )
    {
        if( !whiteBalanceMode(Cam_Front, whiteBalanceMode::Manual) || !whiteBalanceMode(Cam_Bottom, whiteBalanceMode::AutoOnce) )
            return false;
        if( !exposureMax(Cam_Front, 800000) || !exposureMax(Cam_Bottom, 800000) )
            return false;
        if( !exposureMode(Cam_Front, exposureMode::AutoOnce) || !exposureMode(Cam_Bottom, exposureMode::AutoOnce) )
            return false;
        if( !saturation(Cam_Front, 1.0) || !saturation(Cam_Bottom, 1.0) )
            return false;
        //if( !gain(Cam_Front, 0.0) || !gain(Cam_Bottom, 0.0) )
            //return false;
        if( !frameHeightWidth(Cam_Front) || !frameHeightWidth(Cam_Bottom) )
            return false;
    }*/
    return true;
}


bool camClass::exposureMode(int UID,  exposureMode::EXPOSURE mode)
{
    // find out the camera specified by the UID
    int cam_type = 0;
    for( ; cam_type < CAMERAS_WANTED; cam_type++)
        if(camera[cam_type].UID == UID)
            break;

    switch(mode)
    {
    case 0:
        if (UID == Cam_Front) {
            if( PvAttrEnumSet(camera[cam_type].Handle, "ExposureAuto", "Auto") == ePvErrSuccess)
            {
                qDebug() << "ExposureMode is Continuous." << endl;
            }
            else
            {
                qDebug() << "Fail to set the ExposureMode." <<  endl;
                return false;
            }
        } else {
            bool tmp = false;
            CameraGetAEC(0, &tmp);
            if (!tmp){
                while(CameraSetAEC(0, true) != API_OK);
            }
        }
        break;

    case 1:
        if (UID == Cam_Front) {
            if( PvAttrEnumSet(camera[cam_type].Handle, "ExposureAuto", "AutoOnce") == ePvErrSuccess)
            {
                qDebug() << "ExposureMode is Once." << endl;
            }
            else
            {
                qDebug() << "Fail to set the ExposureMode." <<  endl;
                return false;
            }
        } else {
            bool tmp = false;
            CameraGetAEC(0, &tmp);
            if (!tmp){
                while(CameraSetAEC(0, true) != API_OK);
            }        }
        break;

    case 2:
        if (UID == Cam_Front) {
            if( PvAttrEnumSet(camera[cam_type].Handle, "ExposureAuto", "Manual") == ePvErrSuccess)
            {
                qDebug() << "ExposureMode is Off." << endl;
            }
            else
            {
                qDebug() << "Fail to set the ExposureMode." <<  endl;
                return false;
            }
        } else {
            while(CameraSetAEC(0, false) != API_OK);
            while(CameraSetExposure(0, 2000) != API_OK);
        }
        break;
    }

    return true;
}


bool camClass::exposureTime(int UID, int value)
{
    /**
      * 2018-2-17   ??
      */
    // find out the camera specified by the UID
    int cam_type = 0;
    for( ; cam_type < CAMERAS_WANTED; cam_type++)
        if(camera[cam_type].UID == UID)
            break;
    if(UID == Cam_Front) {
        if( PvAttrUint32Set(camera[cam_type].Handle, "ExposureValue", value) == ePvErrSuccess)
        {
            qDebug() << "ExposureMax is " << value << endl;
        }
        else
        {
            qDebug() << "Fail to set the ExposureMax." << endl;
            return false;
        }
    } else {
        qDebug() << "Set Camera Bottom !!!!";
    }
    return true;

}

bool camClass::whiteBalanceMode(int UID, whiteBalanceMode::WHITEBALANCE mode)
{
    // find out the camera specified by the UID
    int cam_type = 0;
    for( ; cam_type < CAMERAS_WANTED; cam_type++)
        if(camera[cam_type].UID == UID)
            break;

    switch(mode)
    {
    case 0:
        if(UID == Cam_Front) {
            if( PvAttrEnumSet(camera[cam_type].Handle, "BalanceWhiteAuto", "Auto") == ePvErrSuccess )
            {
                qDebug() << "WhiteBalanceMode is Continuous." << endl;
            }
            else
            {
                qDebug() << "Fail to set the WhiteBalanceMode." << endl;
                return false;
            }
        } else {
            int cnt = 10;
            while(CameraOnePushWB(0) != API_OK && cnt > 0) cnt--;
        }
        break;

    case 1:
        if (UID == Cam_Front) {
            if( PvAttrEnumSet(camera[cam_type].Handle, "BalanceWhiteAuto", "AutoOnce") == ePvErrSuccess )
            {
                qDebug() << "WhiteBalanceMode is Once." << endl;
            }
            else
            {
                qDebug() << "Fail to set the WhiteBalanceMode." << endl;
                return false;
            }
        } else {
            CameraOnePushWB(0);

        }
        break;

    case 2:
        if( PvAttrEnumSet(camera[cam_type].Handle, "BalanceWhiteAuto", "Manual") == ePvErrSuccess )
        {
            qDebug() << "WhiteBalanceMode is Off." << endl;
        }
        else
        {
            qDebug() << "Fail to set the WhiteBalanceMode." << endl;
            return false;
        }
        break;
    }

    return true;
}


bool camClass::exposureMax(int UID, int value)
{
    /**
      * 2018-1-16修改
      */
    // find out the camera specified by the UID
    int cam_type = 0;
    for( ; cam_type < CAMERAS_WANTED; cam_type++)
        if(camera[cam_type].UID == UID)
            break;
    if(UID == Cam_Front) {
        if( PvAttrUint32Set(camera[cam_type].Handle, "ExposureAutoMax", value) == ePvErrSuccess)
        {
            qDebug() << "ExposureMax is " << value << endl;
        }
        else
        {
            qDebug() << "Fail to set the ExposureMax." << endl;
            return false;
        }
    } else {
        while(CameraSetExposure(0, value) != API_OK);
    }

    return true;
}


bool camClass::gain(int UID, float value)
{
    // find out the camera specified by the UID
    if (UID == Cam_Front) {
        if( PvAttrUint32Set(camera[0].Handle, "Gain", value) == ePvErrSuccess)
        {
            qDebug() << "Gain is " << value << endl;
        }
        else
        {
            qDebug() << "Fail to set the Gain." << endl;
            return false;
        }
    } else {
        if (value == -1) {
            while(CameraSetAGC(0, true) != API_OK );
        } else {
            while(CameraSetGain(0, value) != API_OK);
        }
    }

    return true;
}


bool camClass::exposureAutoTarget(int UID, int value)
{
    // find out the camera specified by the UID
    int cam_type = 0;
    for( ; cam_type < CAMERAS_WANTED; cam_type++)
        if(camera[cam_type].UID == UID)
            break;
    if (UID == Cam_Front) {
    if( PvAttrUint32Set(camera[cam_type].Handle, "ExposureAutoTarget", value) == ePvErrSuccess)
    {
        qDebug() << "ExposureAutoTarget is " << value << endl;
    }
    else
    {
        qDebug() << "Fail to set the ExposureAutoTarget." << endl;
        return false;
    }
    } else {
        if (value == -1) {
            bool tmp = false;
            CameraGetAEC(0, &tmp);
            if (!tmp){
                while(CameraSetAEC(0, true) != API_OK);
            }
        } else {
            while(CameraSetExposure(0, value) != API_OK);
        }
    }

    return true;
}


bool camClass::whiteBalance_Red(int UID, float value)
{
    int cam_type = 0;
    for( ; cam_type < CAMERAS_WANTED; cam_type++)
        if(camera[cam_type].UID == UID)
            break;

    if( PvAttrFloat32Set(camera[cam_type].Handle, "WhitebalValueRed", value) == ePvErrSuccess )
    {
        qDebug() << "WhitebalValueRed is " << value << endl;
    }
    else
    {
        qDebug() << "Fail to set the WhitebalValueRed." << endl;
        return false;
    }

    return true;
}


bool camClass::whiteBalance_Blue(int UID, float value)
{
    // find out the camera specified by the UID
    int cam_type = 0;
    for( ; cam_type < CAMERAS_WANTED; cam_type++)
        if(camera[cam_type].UID == UID)
            break;

    if(UID == Cam_Front) {
    if( PvAttrFloat32Set(camera[cam_type].Handle, "WhitebalValueBlue", value) == ePvErrSuccess )
    {
        qDebug() << "WhitebalValueBlue is " << value << endl;
    } else {
        qDebug() << "Fail to set the WhitebalValueBlue." << endl;
        return false;
    }
    } else {
        while(CameraSetExposure(0, value * 3) != API_OK);
    }

    return true;
}


bool camClass::saturation(int UID, float value)
{
    // find out the camera specified by the UID
    int cam_type = 0;
    for( ; cam_type < CAMERAS_WANTED; cam_type++)
        if(camera[cam_type].UID == UID)
            break;

    if( PvAttrFloat32Set(camera[cam_type].Handle, "Saturation", value ) == ePvErrSuccess )
    {
        qDebug() << "Saturation is " << value <<  endl;
    }
    else
    {
        qDebug() << "Fail to set the Saturation." << endl;
        return false;
    }

    return true;
}


bool camClass::frameHeightWidth(int UID, int height, int width)
{
    // check validity of specified parameters
    if( height <= 0 || height > 580 || width <= 0 || width > 780)
    {
        qDebug() << "Bad parameters." << endl;
        return false;
    }

    // find out the camera specified by the UID
    int cam_type = 0;
    for( ; cam_type < CAMERAS_WANTED; cam_type++)
        if(camera[cam_type].UID == UID)
            break;

    if( PvAttrUint32Set(camera[cam_type].Handle, "Width", width) == ePvErrSuccess
            && PvAttrUint32Set(camera[cam_type].Handle, "Height", height) == ePvErrSuccess)
    {
        qDebug() << "The width is" << width << " and the height is " << height << endl;
    }
    else
    {
        qDebug() << "Fail to set the width and height." << endl;
        return false;
    }

    this->height = height;
    this->width = width;
    return true;
}


int camClass::getExposureValue(int UID)
{
    // find out the camera specified by the UID
    int cam_type = 0;
    for( ; cam_type < CAMERAS_WANTED; cam_type++)
        if(camera[cam_type].UID == UID)
            break;

    tPvUint32 value = 0;
    if( PvAttrUint32Get(camera[cam_type].Handle, "ExposureValue", &value) == ePvErrSuccess )
        return (int)value;
    else
        return -1;
}


float camClass::getWhiteBalance_Red(int UID)
{
    // find out the camera specified by the UID
    int cam_type = 0;
    for( ; cam_type < CAMERAS_WANTED; cam_type++)
        if(camera[cam_type].UID == UID)
            break;

    tPvFloat32 value;
    if( PvAttrFloat32Get(camera[cam_type].Handle, "WhitebalValueRed", &value) == ePvErrSuccess )
        return (float)value;
    else
        return -1.0;
}


float camClass::getWhiteBalance_Blue(int UID)
{
    // find out the camera specified by the UID
    int cam_type = 0;
    for( ; cam_type < CAMERAS_WANTED; cam_type++)
        if(camera[cam_type].UID == UID)
            break;

    tPvFloat32 value;
    if( PvAttrFloat32Get(camera[cam_type].Handle, "WhitebalValueBlue", &value) == ePvErrSuccess )
        return (float)value;
    else
        return -1.0;
}


int camClass::get_exposureAutoTarget(int UID)
{
    // find out the camera specified by the UID
    if(UID = Cam_Front) {
        int cam_type = 0;
        for( ; cam_type < CAMERAS_WANTED; cam_type++)
            if(camera[cam_type].UID == UID)
                break;

        tPvUint32 value = 0;
        if( PvAttrUint32Get(camera[cam_type].Handle, "ExposureAutoTarget", &value) == ePvErrSuccess )
            return (int)value;
        else
            return -1;
    } else {
        bool tmp = false;
        CameraGetAEC(0, &tmp);
        if (!tmp){
            while(CameraSetAEC(0, true) != API_OK);
        }
        // while(CameraSetExposure(0, exp) != API_OK);
        int exp = -1;
        while(CameraGetExposure(0, &exp) != API_OK);
        return exp;
    }
    return -1;
}

bool camClass::start(int UID)
{
    //
    // find out thetr camera specified by the UID
    /**
     * @brief 由于下视相机的问题，以下的代码逻辑上没用。
     */


    if (UID == Cam_Front) {
        qDebug() << "OKKKKK";
        int cam_type = 0;
        for( ; cam_type < CAMERAS_WANTED; cam_type++)
            if(camera[cam_type].UID == UID)
                break;

        if( PvCaptureStart(camera[0].Handle) == ePvErrSuccess)
        {
            qDebug() << "Capture has started." << endl;
        }
        else
        {
            qDebug() << "Fail to start the capture." << endl;
            return false;
        }

        if( PvCommandRun(camera[0].Handle, "AcquisitionStart") == ePvErrSuccess)
        {
            qDebug() << "Acquisition has started." << endl;
        }
        else
        {
            qDebug() << "Fail to start the Acquisition." << endl;
            return false;
        }

        if( PvAttrEnumSet(camera[0].Handle, "FrameStartTriggerMode", "Freerun") == ePvErrSuccess)
        {
            qDebug() << "TriggerMode has been set to Freerun." << endl;
        }
        else
        {
            qDebug() << "Fail to set the Trigger Mode." << endl;
            return false;
        }
    } else {
        if(!m_thread ->isWorking()) {
            m_thread -> stream();
            m_thread -> start();
        }
    }

    if(UID==Cam_Front)
        start_f=true;
    if(UID==Cam_Bottom)
        start_b=true;

    return true;
}


bool camClass::frame2Mat(int UID, Mat& imgsrc)
{
    // find out the camera specified by the UID
    int cam_type = 0;
    if (UID == Cam_Front) {
        for( ; cam_type < CAMERAS_WANTED; cam_type++)
            if(camera[cam_type].UID == UID)
                break;

        if( PvCaptureQueueFrame(camera[cam_type].Handle, &(camera[cam_type].Frame), NULL) == ePvErrSuccess)
        {
            while(PvCaptureWaitForFrameDone(camera[cam_type].Handle, &(camera[cam_type].Frame), 100) == ePvErrTimeout);
            imgsrc.create( this->height, this->width, CV_8UC3);
            imgsrc.data = (uchar *)camera[cam_type].Frame.ImageBuffer;
        }
        else
        {
            qDebug() << "Fail to queue the frame." << endl;
            return false;
        }
    } else {
        // cv::Mat temp = cv::Mat::zeros();
        cv::Mat temp;

        try{
            temp = cv::cvarrToMat(camB_imagedata);
        } catch(cv::Exception& e) {
            // qDebug() << e.what()  << endl;
            return false;
        }
        // imshow("aaa", temp);
        imgsrc = temp.clone();
        temp.release();
    }

    return true;
}


bool camClass::stop(int UID)
{
    // find out the camera specified by the UID
    int cam_type = 0;
    for( ; cam_type < CAMERAS_WANTED; cam_type++)
        if(camera[cam_type].UID == UID)
            break;

    if( PvCommandRun(camera[cam_type].Handle, "AcquisitionStop") == ePvErrSuccess)
    {
        qDebug() << "Acquisition has stoped." << endl;
    }
    else
    {
        qDebug() << "Fail to stop the Acquisition." << endl;
        return false;
    }

    //终止数据流
    if( PvCaptureEnd(camera[cam_type].Handle) == ePvErrSuccess)
    {
        qDebug() << "Capture has stoped." << endl;
    }
    else
    {
        qDebug() << "Fail to stop the Capure." << endl;
        return false;
    }

    if(m_thread->isWorking()) {
        m_thread->stop();
    }

    if(UID==Cam_Front)
        start_f=false;
    if(UID==Cam_Bottom)
        start_b=false;

    return true;
}


bool camClass::close()
{
    /**
      * 2018 年12月20，由于相机问题修改。
      */
    /**
    if(CAMERAS_WANTED==1)
    {
    **/
    if (m_thread -> isWorking() == true) {
        m_thread -> stop();
        return true;
    } else {
        if( PvCameraClose(camera[0].Handle) == ePvErrSuccess )
        {
            qDebug() << "Cameras closed." << endl;
            PvUnInitialize();
            return true;
        }
        else
            return false;
    }


    /** } **/
    /**
    if(CAMERAS_WANTED==2)
    {
        if( PvCameraClose(camera[0].Handle) == ePvErrSuccess
                && PvCameraClose((camera[1].Handle)) == ePvErrSuccess)
        {
            qDebug() << "Cameras closed." << endl;
            PvUnInitialize();
            return true;
        }
        else
            return false;
    }**/
    return true;
}

void camClass::process_Cam_B(QImage qimage, IplImage* data)
{
    /**
      * 用于处理来自下视相机的图片的槽函数。
      **/
    // qDebug() << "aaaaaaaaaaaaaa"  ;
    camB_imagedata = data;
}




