#include "ui_dialog.h"
#include "visiondialog.h"
#include "visionclass.h"
#include <QtGui>
#include <QImage>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QObject>

using namespace cv;

visionDialog::visionDialog(QWidget *parent,visionClass *vision) :
    QDialog(parent),waitingF(0),waitingB(0),test(false),mission(false)
{
    Vision=vision;
    setupUi(this);
    ImageF=imread("/home/nwpu/Code/America_test2.0/Pictures/save1_654.jpg");
    ImageB=imread("/home/nwpu/Code/America_test2.0/Pictures/save1_180.jpg");
    //修改图片大小
    cv::resize(ImageF,ImageF,Size(),0.4,0.4);
    cv::resize(ImageB,ImageB,Size(),0.4,0.4);

    //转换Mat类型
    cvtColor(ImageF,ImageF,COLOR_BGR2RGB);
    cvtColor(ImageB,ImageB,COLOR_BGR2RGB);

    //创建Qimage
    QImage tmpf(ImageF.cols,ImageF.rows,QImage::Format_RGB888);
    memcpy(tmpf.bits(),ImageF.data,ImageF.cols*ImageF.rows*ImageF.elemSize());
    QImage tmpb(ImageB.cols,ImageB.rows,QImage::Format_RGB888);
    memcpy(tmpb.bits(),ImageB.data,ImageB.cols*ImageB.rows*ImageB.elemSize());

    //显示
    camF->setPixmap(QPixmap::fromImage(tmpf));
    camB->setPixmap(QPixmap::fromImage(tmpb));

    connect(this,SIGNAL(onqpacketSize(int )),Vision,SLOT(qpacketSize(int)));
    connect(this,SIGNAL(onqwhiteBalanceMode(int )),Vision,SLOT(qwhiteBalanceMode(int)));
    connect(this,SIGNAL(onqexposureMax(int )),Vision,SLOT(qexposureMax(int )));
    connect(this,SIGNAL(onqwhiteBalance_Red(float )),Vision,SLOT(qwhiteBalance_Red(float)));
    connect(this,SIGNAL(onqwhiteBalance_Blue(float )),Vision,SLOT(qwhiteBalance_Blue(float )));
    connect(this,SIGNAL(onqexposureAutoTarget(int )),Vision,SLOT(qexposureAutoTarget(int )));
    connect(this,SIGNAL(onqcameraNum(int )),Vision,SLOT(qcameraNum(int )));
    connect(this,SIGNAL(onqexposureMode(int )),Vision,SLOT(qexposureMode(int )));
}

void visionDialog::closeEvent(QCloseEvent *event)
{
    test=false;
    mission=false;
    this->hide();
    disconnect(Vision,SIGNAL(frameGottenF(Mat*)),this,SLOT(onFrameGottenF(Mat*)));
    //disconnect(cam,SIGNAL(cameraClosed(bool)),this,SLOT(onCameraClosed(bool)));
    disconnect(Vision,SIGNAL(cameraStartedF(bool)),this,SLOT(onCameraStartedF(bool)));
    disconnect(Vision,SIGNAL(cameraStoppedF(bool)),this,SLOT(onCameraStoppedF(bool)));
    disconnect(Vision,SIGNAL(frameGottenB(Mat*)),this,SLOT(onFrameGottenB(Mat*)));
    //disconnect(cam,SIGNAL(cameraOpened(bool)),this,SLOT(onCameraOpened(bool)));
    //disconnect(cam,SIGNAL(cameraClosed(bool)),this,SLOT(onCameraClosed(bool)));
    disconnect(Vision,SIGNAL(cameraStartedB(bool)),this,SLOT(onCameraStartedB(bool)));
    disconnect(Vision,SIGNAL(cameraStoppedB(bool)),this,SLOT(onCameraStoppedB(bool)));
    //QDialog::closeEvent(event);
}

void visionDialog::showEvent(QShowEvent *event)
{
    connect(this,SIGNAL(getFrameF(Mat*)),Vision,SLOT(onGetFrameF(Mat*)));
    connect(Vision,SIGNAL(frameGottenF(Mat*)),this,SLOT(onFrameGottenF(Mat*)));
    //connect(this,SIGNAL(openCamera(int)),cam,SLOT(onOpenCamera(int)));
    //connect(this,SIGNAL(closeCamera(int)),cam,SLOT(onCloseCamera(int)));
    connect(this,SIGNAL(startCameraF(int)),Vision,SLOT(onStartCameraF(int)));
    connect(this,SIGNAL(stopCameraF(int)),Vision,SLOT(onStopCameraF(int)));
    //connect(cam,SIGNAL(cameraOpened(bool)),this,SLOT(onCameraOpened(bool)));
    //connect(cam,SIGNAL(cameraClosed(bool)),this,SLOT(onCameraClosed(bool)));
    connect(Vision,SIGNAL(cameraStartedF(bool)),this,SLOT(onCameraStartedF(bool)));
    connect(Vision,SIGNAL(cameraStoppedF(bool)),this,SLOT(onCameraStoppedF(bool)));
    connect(this,SIGNAL(getFrameB(Mat*)),Vision,SLOT(onGetFrameB(Mat*)));
    connect(Vision,SIGNAL(frameGottenB(Mat*)),this,SLOT(onFrameGottenB(Mat*)));
    connect(this,SIGNAL(startCameraB(int)),Vision,SLOT(onStartCameraB(int)));
    connect(this,SIGNAL(stopCameraB(int)),Vision,SLOT(onStopCameraB(int)));
    connect(Vision,SIGNAL(cameraStartedB(bool)),this,SLOT(onCameraStartedB(bool)));
    connect(Vision,SIGNAL(cameraStoppedB(bool)),this,SLOT(onCameraStoppedB(bool)));
    QDialog::showEvent(event);
}

visionDialog::~visionDialog()
{
    //delete ui;
}


void visionDialog::on_cameraspinBox_valueChanged(int arg1)              //改变值时存放于camera中
{
    cameraN=arg1;
}


void visionDialog::on_epmodespinBox_valueChanged(int arg1)
{
    exposuremode=arg1;
}

void visionDialog::on_wbmodespinBox_valueChanged(int arg1)
{
    whitebalacemode=arg1;
}

void visionDialog::on_psmodespinBox_valueChanged(int arg1)
{
    packetsizemode=arg1;
}

void visionDialog::on_epsmaxspinBox_valueChanged(int arg1)
{
    exposuremax=arg1;
}

void visionDialog::on_epattgspinBox_valueChanged(int arg1)
{
    exposureAutoTarget=arg1;
}

void visionDialog::on_wb_redSpinBox_valueChanged(double arg1)
{
    whitebalace_red =arg1;
}

void visionDialog::on_wbbluespinBox_valueChanged(double arg1)
{
    whitebalace_blue=arg1;
}

void visionDialog::on_cameraspinBox_editingFinished()                //执行操作
{
      emit onqcameraNum(cameraN);
}

void visionDialog::on_epmodespinBox_editingFinished()
{
    emit  onqexposureMode(exposuremode);
}

void visionDialog::on_wbmodespinBox_editingFinished()
{
    emit onqwhiteBalanceMode(whitebalacemode);
}

void visionDialog::on_psmodespinBox_editingFinished()
{
    emit onqpacketSize(packetsizemode);
}

void visionDialog::on_epsmaxspinBox_editingFinished()
{
    emit onqexposureMax(exposuremax);
}

void visionDialog::on_epattgspinBox_editingFinished()
{
    emit onqexposureAutoTarget(exposureAutoTarget);
}

void visionDialog::on_wb_redSpinBox_editingFinished()
{
    emit onqwhiteBalance_Red((float)whitebalace_red);
}

void visionDialog::on_wbbluespinBox_editingFinished()
{
    emit onqwhiteBalance_Blue((float)whitebalace_blue);
}


void visionDialog::onFrameGottenF(cv::Mat *imgSrc)
{
        qDebug()<<"1"<<endl;
        waitingF--;
        cv::resize(*imgSrc,*imgSrc,Size(),0.4,0.4);
        QImage img(imgSrc->data,imgSrc->cols,imgSrc->rows,imgSrc->step,QImage::Format_RGB888);
        camF->setPixmap(QPixmap::fromImage(img.rgbSwapped()));
        //qDebug()<<"1"<<endl;
        timerF.singleShot(200,this,SLOT(onTimerF()));
}

/*void visionDialog::onCameraOpened(bool ok)
{
    if(ok){
        working_flag=true;
        startButton->setEnabled(true);
        stopButton->setEnabled(true);
        closeButton->setEnabled(true);
    }
}

void visionDialog::onCameraClosed(bool ok)
{
    working_flag=false;
}*/

void visionDialog::onCameraStartedF(bool ok)
{
        if(ok){
            waitingF=0;
            timerF.singleShot(200,this,SLOT(onTimerF()));
            start_flagF=true;
            stop_flagF=false;
            //connect(stop_button,SIGNAL(clicked(bool)),this,SLOT(stopcam()));
            //stop_button->setEnabled(true);
        }
        else {
            //connect(begin_button,SIGNAL(clicked(bool)),this,SLOT(begincam()));
            //begin_button->setEnabled(true);
            start_flagF=false;
            stop_flagF=true;
        }
        if(mission)
        {
            disconnect(Vision,SIGNAL(cameraStartedF_m(bool)),this,SLOT(onCameraStartedF(bool)));

        }

}

void visionDialog::onCameraStoppedF(bool ok)
{
        if(ok){
            stop_flagF=true;
            start_flagF=false;
        }
        else {
            stop_flagF=false;
            start_flagF=true;

        }

}

void visionDialog::onTimerF()
{
     emit getFrameF(&ImageF);
     waitingF++;
     //qDebug()<<"1"<<endl;
}


void visionDialog::onFrameGottenB(cv::Mat *imgSrc)
{
        waitingB--;
        cv::resize(*imgSrc,*imgSrc,Size(),0.4,0.4);
        QImage img(imgSrc->data,imgSrc->cols,imgSrc->rows,imgSrc->step,QImage::Format_RGB888);
        camB->setPixmap(QPixmap::fromImage(img.rgbSwapped()));
        timerB.singleShot(20,this,SLOT(onTimerB()));
}


void visionDialog::onCameraStartedB( bool ok)
{
        if(ok){
            waitingB=0;
            timerB.singleShot(20,this,SLOT(onTimerB()));
            start_flagB=true;
            stop_flagB=false;
        }
        else {
            start_flagB=false;
            stop_flagB=true;
        }
        if(mission)
        {
            disconnect(Vision,SIGNAL(cameraStartedB_m(bool)),this,SLOT(onCameraStartedB(bool)));

        }

}

void visionDialog::onCameraStoppedB(bool ok)
{

        if(ok){
            stop_flagB=true;
            start_flagB=false;
        }
        else {
            stop_flagB=false;
            start_flagB=true;
        }

}

void visionDialog::onTimerB()
{
     emit getFrameB(&ImageB);
     waitingB++;
}


void visionDialog::on_startButton_clicked()
{
   if(cameraN==0)
   {
       emit startCameraF(1);
   }
   else
       emit startCameraB(1);
}

void visionDialog::on_stopButton_clicked()
{
    if(cameraN==0)
        emit stopCameraF(1);
    else
        emit stopCameraB(1);
}

void visionDialog::on_missionButton_clicked()
{
    mission=true;
    connect(Vision,SIGNAL(cameraStartedF_m(bool)),this,SLOT(onCameraStartedF(bool)));
    connect(Vision,SIGNAL(cameraStartedB_m(bool)),this,SLOT(onCameraStartedB(bool)));
}

void visionDialog::on_testButton_clicked()
{
    test=true;
    startButton->setEnabled(true);
    stopButton->setEnabled(true);
}




