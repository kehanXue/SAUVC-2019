#ifndef VISIONDIALOG
#define VISIONDIALOG

#include<QObject>
#include <QDialog>
#include <QDebug>
#include "ui_dialog.h"
#include "visionclass.h"
#include <QTimer>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

namespace Ui {
class Dialog;
}

class visionDialog : public QDialog,public Ui::Dialog
{    Q_OBJECT

public:
    visionDialog(QWidget *parent,visionClass *vision);
    ~visionDialog();
     //void setVision(visionClass *Vision);


 public slots:
     void onFrameGottenF(Mat *imgSrc);
     void onCameraStartedF(bool ok);
     void onCameraStoppedF(bool ok);
     void onCameraStartedB( bool ok);
     void onTimerF();
     void onFrameGottenB(Mat *imgSrc);
     void onCameraStoppedB(bool ok);
     void onTimerB();

private slots:

    void on_cameraspinBox_valueChanged(int arg1);

    void on_epmodespinBox_valueChanged(int arg1);

    void on_wbmodespinBox_valueChanged(int arg1);

    void on_psmodespinBox_valueChanged(int arg1);

    void on_epsmaxspinBox_valueChanged(int arg1);

    void on_epattgspinBox_valueChanged(int arg1);

    void on_wb_redSpinBox_valueChanged(double arg1);

    void on_wbbluespinBox_valueChanged(double arg1);


    void on_epsmaxspinBox_editingFinished();

    void on_cameraspinBox_editingFinished();

    void on_epmodespinBox_editingFinished();

    void on_wbmodespinBox_editingFinished();

    void on_psmodespinBox_editingFinished();

    void on_epattgspinBox_editingFinished();

    void on_wb_redSpinBox_editingFinished();

    void on_wbbluespinBox_editingFinished();

    void on_startButton_clicked();

    void on_stopButton_clicked();

    void on_missionButton_clicked();

    void on_testButton_clicked();

signals:
    void getFrameF(Mat * imgSrc);
    //void openCamera(int value);
    //void closeCamera(int value);
    void startCameraF(int value);
    void stopCameraF(int value);
    void getFrameB(Mat * imgSrc);
    void startCameraB(int value);
    void stopCameraB(int value);

    void onqpacketSize(int value);
    void onqexposureMode(int value);
    void onqwhiteBalanceMode(int value);
    void onqexposureMax(int value);
    void onqwhiteBalance_Red(float value);
    void onqwhiteBalance_Blue(float value);
    void onqexposureAutoTarget(int value);
    void onqcameraNum(int value);

private:
    //Ui::Dialog *ui;
    //Mat image=imread("save1_654.jpg");
    //设置标志，在界面输入成功时执行程序，0为所有设置初始值
    int cameraN;                       //相机
    int exposuremode=0;                 //曝光模式
    int whitebalacemode=0;              //白平衡模式
    int packetsizemode=0;               //矩形帧模式
    int exposuremax=0;                  //曝光最大值
    int exposureAutoTarget=0;           //曝光百分比
    double whitebalace_red=0.0;         //白平衡值

    double whitebalace_blue=0.0;

    bool mission;
    bool test;
    bool stop_flagF;
    bool stop_flagB;
    bool start_flagF;
    bool start_flagB;
    Mat ImageF,ImageB;

     QTimer timerF,timerB;
     int waitingF,waitingB;
     visionClass *Vision;

    void closeEvent(QCloseEvent * event);
    void showEvent(QShowEvent * event);
    //void getImage(Mat &img1);
    //void stopcam(int value);
    //void startcam(int value);

};


#endif // VISIONDIALOG






