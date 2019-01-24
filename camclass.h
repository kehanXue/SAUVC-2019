#ifndef CAMCLASS
#define CAMCLASS

#include "include/PvApi.h"
#include "include/JHCap.h"
#include "include/capturethread.h"
#include "include/imageutil.h"
#include <QObject>
#include <QDebug>
#include <unistd.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

using cv::Mat;


typedef int Camera;

enum
{
    /**
      * 2018年12月20号，由于下视相机损坏，将前视和下视的UID交换在，以下视作为前视。以下是修改之前的代码。
      * Cam_Front = 5044711,
      * //下视相机UID
      * Cam_Bottom = 5008191
      **/
    //前视相机UID
    Cam_Front = 5008191,
    //下视相机UID
    Cam_Bottom = 5044711
};


class packetSize
{
public:
    enum SIZE
    {
        No_Jumbo = 0,
        Jumbo_4K = 1,
        Jumbo_8K = 2
    };
};


class exposureMode
{
public:
    enum EXPOSURE
    {
        Auto = 0,
        AutoOnce = 1,
        Manual = 2
    };
};


class whiteBalanceMode
{
public:
    enum WHITEBALANCE
    {
        Auto = 0,
        AutoOnce = 1,
        Manual = 2
    };
};

class camClass : public QObject
{
    Q_OBJECT

private:
    const int MAX_TRY_COUNT = 50;
    // const int CAMERAS_WANTED = 2;
    /**
     * @brief CAMERAS_WANTED 2019-1-5 1 -> 0;
     */
    const int CAMERAS_WANTED = 1;
    int height = 580;
    int width = 780;
    int camB_width = 800;
    int camB_height = 600;

    typedef struct
    {
        unsigned long   UID;    // UID
        tPvHandle       Handle; // Handle of the camera
        tPvFrame        Frame;  // the buffer to store acquired frames
    }tCamera;

protected:
    bool start_f=false;
    bool start_b=false;

    tCamera camera[2];
    Camera camB;
    CaptureThread* m_thread;
    IplImage* camB_imagedata;

    // initialize the API
    // return false if anything goes wrong
    bool initialize();

    // set other parameters implicitly for both cameras
    bool setting();

    // set PacketSize
    bool packetSize(int UID, packetSize::SIZE mode);

    // set ExposureMode
    bool exposureMode(int UID, exposureMode::EXPOSURE mode);

    // set WhiteBalanceMode
    bool whiteBalanceMode(int UID, whiteBalanceMode::WHITEBALANCE mode);

    // set ExposureMax
    bool exposureMax(int UID, int value);

    // set Gain
    bool gain(int UID, float value);

    // set whitebalance for Red channel
    bool whiteBalance_Red(int UID, float value);

    // set whitebalance for Blue channel
    bool whiteBalance_Blue(int UID, float value);

    // set Saturation
    bool saturation(int UID, float value);

    // set the height and width of a frame, 580 for height and 780 for width implicitly
    bool frameHeightWidth(int UID, int height = 580, int width = 780);

    // set ExposureAutoTarget
    bool exposureAutoTarget(int UID, int value);

    // get the current ExposureValue
    int getExposureValue(int UID);

    // get the whitebalance for Red channel
    float getWhiteBalance_Red(int UID);

    // get the whitebalance for Blue channel
    float getWhiteBalance_Blue(int UID);


    // get the current ExposureAutoTarget
    int get_exposureAutoTarget(int UID);

    // start the acquisition of a specified camera
    bool start(int UID);

    // convert from a frame to a Mat in OpenCV
    bool frame2Mat(int UID, Mat& imgsrc);

    // stop the acquisition of a specified camera
    bool stop(int UID);

    // close cameras and uninitialize the API
    // please call this function iff the programe ends
    bool close();


/*//图像界面

signals:
    void frameGottenF(Mat & imgSrc);
    void cameraOpened(bool ok);
    void cameraClosed(bool ok);
    void cameraStartedF(bool ok);
    void cameraStoppedF(bool ok);
    void frameGottenB(Mat & imgSrc);
    void cameraStartedB(bool ok);
    void cameraStoppedB(bool ok);

public slots:
    void qpacketSize(int value);
    void qexposureMode(int value);
    void qwhiteBalanceMode(int value);
    void qexposureMax(int value);
    void qwhiteBalance_Red(float value);
    void qwhiteBalance_Blue(float value);
    void qexposureAutoTarget(int value);
    void qcameraNum(int value);

    void onOpenCamera(int value);
    void onCloseCamera(int value);
    void onStartCameraF(int value);
    void onStopCameraF(int value);
    void onGetFrameF(Mat* imgSrc);
    void onStartCameraB(int value);
    void onStopCameraB(int value);
    void onGetFrameB(Mat* imgSrc);
    */
public:
    camClass();
    ~camClass();
public slots:
    void process_Cam_B(QImage qimage, IplImage* data);
};

#endif // CAMCLASS


