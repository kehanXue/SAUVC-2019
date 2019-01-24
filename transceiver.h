#ifndef TRANSCEIVER_H
#define TRANSCEIVER_H

#include <QObject>
#include <QTcpSocket>
#include <QList>
#include <QHostAddress>
#include <QTimer>

#include <utility>

#include "global.h"
#include "configmanager.h"
#include "controller.h"

using std::pair;

Q_DECLARE_METATYPE(QHostAddress)
#define DEFAULT_DINT_UP         170
#define DEFAULT_DINT_DOWN       140
#define DEFAULT_DCTR_UP         190
#define DEFAULT_DCTR_DOWN       125

class transceiver : public QObject
{
    Q_OBJECT

public:
    explicit transceiver(QObject *parent = 0);
    ~transceiver();

    void stmReset();
    void setHMotors(QList<pair<MOTORS,float>> value);
    void setAutoDeep(bool sw,QList<configManager::PIDValue> pid={});
    void setVMotors(QList<pair<MOTORS,float>> value);
    void setRelay(QList<pair<int,bool>> value);
    void setGoal(float deep,float pitch=0,float roll=0);
    void setDeepRev(float rev);
    void setPitchPID(QList<configManager::PIDValue> value);
    void setRollPID(QList<configManager::PIDValue> value);
    void setSevo(bool ok, int pulse);
    void setStepMotor(bool ok, unsigned int time, bool isHandControl = false, bool direction = 0);
    void setDeepPIDRev(int dIntUp,int dIntDown,int dCtrUp,int dCtrDown);
    void setMissionStarted(TASKS_ID id);
    void setMissionFinished(TASKS_ID id);

    void attempt();

    void setCtr(controller * pCtr);

private:
    controller * ctr;
    QTcpSocket * socket;
    QByteArray recvBuffer;
    void sucConnect();
    void failConnect();
    void onRecvData();
    void parseAcos();
    void parseSbg();
    void parseDeep();
    void parseSpeed();
    void parseHT();
    void parseStepRec();

signals:
    void connected(bool connected,QHostAddress addr=QHostAddress::Null,int port=-1);
    void acosRecv(float theta1, float phi1, float theta2, float phi2);
    void sbgRecv(float roll, float pitch, float yaw, float magNorm);
    void deepRecv(float deep);
    void speedRecv(float lu,float ld,float ru,float rd);
    void HTRecv(float humi1,float tem1,float humi2,float tem2);
    void StepRecv(bool ok);
public slots:
};
#endif // TRANSCEIVER_H
