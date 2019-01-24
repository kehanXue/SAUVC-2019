#ifndef RECORDMANAGER_H
#define RECORDMANAGER_H

#include <QObject>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QHostAddress>
#include <QByteArray>

#include <unistd.h>
#include <string>
#include <fstream>
#include <iomanip>

#include "transceiver.h"
#include "controller.h"
#include "visionclass.h"

class recordManager : public QObject
{
    Q_OBJECT

public:
    enum RECORD_TYPE{
        CONTROLLER_RECORD=1,
        TRANSCEIVER_RECORD=1<<1,
        PHOTO_RECORD_B=1<<2,
        PHOTO_RECORD_F=1<<3,
        VISION_RECORD=1<<4,
        ACOS_RECORD=1<<5,
    };


    explicit recordManager(QObject *parent = 0);
    ~recordManager();

    void setTrans(transceiver * Trans);
    void setCtr(controller * Ctr);
    void setVision(visionClass * Vision);

    void setStatus(int sw);

    void initAcosSocket();

private:
    const QString recordBasePath="/home/nwpu/Code/Robosub_test2018-2-8/record/";
    const QHostAddress acosAddress=QHostAddress(QString("192.168.1.100"));
    const uint16_t acosCmdPort=10001;
    const uint16_t acosDataPort=10002;
    const char acosStartCmdChar[8]={0x55,0xAA,0x04,0x01,0x00,0x00,0x00,0x00};
    const char acosStopCmdChar[8]={0x55,0xAA,0x04,0x00,0x00,0x00,0x00,0x00};
    const QByteArray acosStartCmd=QByteArray(acosStartCmdChar,8);
    const QByteArray acosStopCmd=QByteArray(acosStopCmdChar,8);
    const static int ACOS_HEAD_LENGTH=6;
    const char acosSig[2]={0x55,0xAA};

    transceiver * trans;
    controller * ctr;
    visionClass * vision;

    bool acosCmdBusyFlag;  //failAcosCmdConnect存在多径调用问题，通过此标志位解决。

    QUdpSocket * acosDataSocket;
    QTcpSocket * acosCmdSocket;
    QByteArray acosData;
    char acosCurHead[ACOS_HEAD_LENGTH]={0};
    //acosHeadRecv 为false,包头未接收全
    //acosWanted 此时指示还应该接收几个字节的包头
    //acosHeadRecv 为true,包头已经接收全
    //acosWanted 此时指示还应该接收多少字节的数据
    int acosWanted;
    bool acosHeadRecv;

    int status;

    std::fstream ctrOut,transOut,visionOut;
    std::fstream acosOut;
    int inited;

    bool initRecord(RECORD_TYPE t);
    bool initAcosRecord();
    void closeAcosFile();
    void parseAcosData();

    void onAcosStateChanged();
    void failAcosCmdConnect();
    void onAcosCmdConnected();
    void onAcosCmdDisconnected();
    void onAcosDataRecv();

signals:
    void acosConnected(QHostAddress, bool);
    void statusSet(int sw);
    void setPhotoF(bool ok);
    void setPhotoB(bool ok);
    void setVisDebugRecord(bool ok);
private slots:

public slots:
    //slots with transceiver signals
    void acosResRecv(float theta1, float phi1, float theta2, float phi2);
    void sbgRecv(float roll, float pitch, float yaw, float magNorm);
    void deepRecv(float deep);
    void speedRecv(float lu,float ld,float ru,float rd);
    void HTRecv(float humi1,float tem1,float humi2,float tem2);

    //slots with controller signals
    void setHMotors(QList<pair<MOTORS,float>> value);
    void setAutoDeep(bool sw,QList<configManager::PIDValue> pid={});
    void setVMotors(QList<pair<MOTORS,float>> value);
    void setRelay(QList<pair<int,bool>> value);
    void setGoal(float deep,float pitch=0,float roll=0);
    void setPitchPID(QList<configManager::PIDValue> value);
    void setRollPID(QList<configManager::PIDValue> value);
    void debugCtr(const QString & info);


    //slots with visionClass signals
    //void debugVision(const QString & info);


};

#endif // RECORDMANAGER_H
