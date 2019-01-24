#include "transceiver.h"
#include <QDebug>
#include <QEventLoop>

transceiver::transceiver(QObject *parent) : QObject(parent),
    socket(nullptr)
{
    qRegisterMetaType<QHostAddress>("QHostAddress");
    qRegisterMetaType<QList<configManager::PIDValue>>("QList<configManager::PIDValue>");
    qRegisterMetaType<QList<pair<int,bool>>>("QList<pair<int,bool>>");
    qRegisterMetaType<QList<pair<MOTORS,float>>>("QList<pair<MOTORS,float>>");

}

transceiver::~transceiver()
{
    if(socket!=nullptr){
        delete socket;
    }
}

void transceiver::stmReset()
{
    if(socket->state()!=QAbstractSocket::ConnectedState){
        qDebug() << "The socket is not connected.";
        return;
    }

    char toBeSent[]={0x03,0x00,0xFF,0xFF};

    socket->write(toBeSent,toBeSent[0]+1);
}

void transceiver::setHMotors(QList<pair<MOTORS, float> > value)
{
    if(socket->state()!=QAbstractSocket::ConnectedState){
        qDebug() << "The socket is not connected.";
        return;
    }

    char toBeSent[18]={17,0x01,0};
    for(auto & t:value){
        switch (t.first) {
        case MAIN_LEFT:
            memcpy(toBeSent+2+0*sizeof(float),&t.second,sizeof(float));
            break;
        case MAIN_RIGHT:
            memcpy(toBeSent+2+1*sizeof(float),&t.second,sizeof(float));
            break;
        case SIDE_UP:
            memcpy(toBeSent+2+2*sizeof(float),&t.second,sizeof(float));
            break;
        case SIDE_DOWN:
            memcpy(toBeSent+2+3*sizeof(float),&t.second,sizeof(float));
            break;
        default:
            break;
        }
    }

    socket->write(toBeSent,toBeSent[0]+1);
}

void transceiver::setAutoDeep(bool sw, QList<configManager::PIDValue> pid)
{
    if(socket->state()!=QAbstractSocket::ConnectedState){
        qDebug() << "The socket is not connected.";
        return;
    }

    char toBeSent[27]={26,0x02,0};
    if(sw){
        toBeSent[2]=0x0F;
        int16_t temp[3];
        for(auto & t:pid){
            temp[0]=10*t.pValue;
            temp[1]=10*t.iValue;
            temp[2]=10*t.dValue;
            switch (t.motorID) {
            case LEFT_UP:
                memcpy(toBeSent+3+0*3*sizeof(int16_t),temp,3*sizeof(int16_t));
                break;
            case LEFT_DOWN:
                memcpy(toBeSent+3+1*3*sizeof(int16_t),temp,3*sizeof(int16_t));
                break;
            case RIGHT_UP:
                memcpy(toBeSent+3+2*3*sizeof(int16_t),temp,3*sizeof(int16_t));
                break;
            case RIGHT_DOWN:
                memcpy(toBeSent+3+3*3*sizeof(int16_t),temp,3*sizeof(int16_t));
                break;
            default:
                break;
            }
        }
    }

    socket->write(toBeSent,toBeSent[0]+1);
}

void transceiver::setVMotors(QList<pair<MOTORS, float> > value)
{
    if(socket->state()!=QAbstractSocket::ConnectedState){
        qDebug() << "The socket is not connected.";
        return;
    }

    char toBeSent[19]={18,0x02,0x00,0};
    for(auto & t:value){
        switch (t.first) {
        case LEFT_UP:
            memcpy(toBeSent+3+0*sizeof(float),&t.second,sizeof(float));
            break;
        case LEFT_DOWN:
            memcpy(toBeSent+3+1*sizeof(float),&t.second,sizeof(float));
            break;
        case RIGHT_UP:
            memcpy(toBeSent+3+2*sizeof(float),&t.second,sizeof(float));
            break;
        case RIGHT_DOWN:
            memcpy(toBeSent+3+3*sizeof(float),&t.second,sizeof(float));
            break;
        default:
            break;
        }
    }

    socket->write(toBeSent,toBeSent[0]+1);
}

void transceiver::setRelay(QList<pair<int, bool> > value)
{
    if(socket->state()!=QAbstractSocket::ConnectedState){
        qDebug() << "The socket is not connected.";
        return;
    }

    char toBeSent[26]={25,0x03,0};
    for(auto & t:value){
        //继电器编号从0开始
        if(t.first>=NUMBER_OF_RELAYS){
            qDebug() << "Illegal relay id.";
            continue;
        }

        toBeSent[2+t.first*2]=0x0F;
        toBeSent[2+t.first*2+1]=t.second?0x0F:0x00;
    }

    socket->write(toBeSent,toBeSent[0]+1);
}

void transceiver::setGoal(float deep, float pitch, float roll)
{
    if(socket->state()!=QAbstractSocket::ConnectedState){
        qDebug() << "The socket is not connected.";
        return;
    }

    char toBeSent[14]={13,0x09,0};
    memcpy(toBeSent+2,&deep,sizeof(float));
    memcpy(toBeSent+2+sizeof(float),&pitch,sizeof(float));
    memcpy(toBeSent+2+2*sizeof(float),&roll,sizeof(float));

    socket->write(toBeSent,toBeSent[0]+1);
}

void transceiver::setDeepRev(float rev)
{
    if(socket->state()!=QAbstractSocket::ConnectedState){
        qDebug() << "The socket is not connected.";
        return;
    }

    char toBeSent[6]={5,0x0A,0};
    memcpy(toBeSent+2,&rev,sizeof(float));

    socket->write(toBeSent,toBeSent[0]+1);
}

void transceiver::setPitchPID(QList<configManager::PIDValue> value)
{
    if(socket->state()!=QAbstractSocket::ConnectedState){
        qDebug() << "The socket is not connected.";
        return;
    }

    char toBeSent[27]={26,0x0B,0};
    toBeSent[2]=0xF0;
    int16_t temp[3];
    for(auto & t:value){
        temp[0]=10*t.pValue;
        temp[1]=10*t.iValue;
        temp[2]=10*t.dValue;
        switch (t.motorID) {
        case LEFT_UP:
            memcpy(toBeSent+3+0*3*sizeof(int16_t),temp,3*sizeof(int16_t));
            break;
        case LEFT_DOWN:
            memcpy(toBeSent+3+1*3*sizeof(int16_t),temp,3*sizeof(int16_t));
            break;
        case RIGHT_UP:
            memcpy(toBeSent+3+2*3*sizeof(int16_t),temp,3*sizeof(int16_t));
            break;
        case RIGHT_DOWN:
            memcpy(toBeSent+3+3*3*sizeof(int16_t),temp,3*sizeof(int16_t));
            break;
        default:
            break;
        }
    }

    socket->write(toBeSent,toBeSent[0]+1);
}

void transceiver::setRollPID(QList<configManager::PIDValue> value)
{
    if(socket->state()!=QAbstractSocket::ConnectedState){
        qDebug() << "The socket is not connected.";
        return;
    }

    char toBeSent[27]={26,0x0B,0};
    toBeSent[2]=0x0F;
    int16_t temp[3];
    for(auto & t:value){
        temp[0]=10*t.pValue;
        temp[1]=10*t.iValue;
        temp[2]=10*t.dValue;
        switch (t.motorID) {
        case LEFT_UP:
            memcpy(toBeSent+3+0*3*sizeof(int16_t),temp,3*sizeof(int16_t));
            break;
        case LEFT_DOWN:
            memcpy(toBeSent+3+1*3*sizeof(int16_t),temp,3*sizeof(int16_t));
            break;
        case RIGHT_UP:
            memcpy(toBeSent+3+2*3*sizeof(int16_t),temp,3*sizeof(int16_t));
            break;
        case RIGHT_DOWN:
            memcpy(toBeSent+3+3*3*sizeof(int16_t),temp,3*sizeof(int16_t));
            break;
        default:
            break;
        }
    }

    socket->write(toBeSent,toBeSent[0]+1);
}

void transceiver::setSevo(bool ok,int pulse)
{
    if(socket->state()!=QAbstractSocket::ConnectedState){
        qDebug() << "The socket is not connected.";
        return;
    }
    char toBeSent[10]={9,0x0C,0};
    toBeSent[2]=ok?0x0F:0x00;
    if(pulse<0)
    {
        toBeSent[3]=0xFF;
        pulse = abs(pulse);
    }
    else toBeSent[3]=0x00;
    uint8_t temp;
    temp=(uint8_t)pulse;
    toBeSent[4]=temp;
    socket->write(toBeSent,toBeSent[0]+1);
}

void transceiver::setStepMotor(bool ok, unsigned int time, bool isHandControl,bool direction)
{
    if(socket->state()!=QAbstractSocket::ConnectedState){
        qDebug() << "The socket is not connected.";
        return;
    }
    char toBeSent[15]={14,0x0D,0};
    toBeSent[2]=ok?0x0F:0x00;
    char _isHandControl=isHandControl?0x0F:0x00;
    memcpy(toBeSent+3,&_isHandControl,1);
    memcpy(toBeSent+4,&direction,1);
    memcpy(toBeSent+5,&time,sizeof(unsigned int));
    qDebug()<<"Data Sent";
    socket->write(toBeSent,toBeSent[0]+1);
}
void transceiver::setDeepPIDRev(int dIntUp, int dIntDown, int dCtrUp, int dCtrDown)
{
    if(socket->state()!=QAbstractSocket::ConnectedState){
        qDebug() << "The socket is not connected.";
        return;
    }
    char toBeSent[26]={25,0x0E,0};
    memcpy(toBeSent+2,&dIntUp,sizeof(int));
    memcpy(toBeSent+2+sizeof(int),&dIntDown,sizeof(int));
    memcpy(toBeSent+2+2*sizeof(int),&dCtrUp,sizeof(int));
    memcpy(toBeSent+2+3*sizeof(int),&dCtrDown,sizeof(int));
    socket->write(toBeSent,toBeSent[0]+1);
}

void transceiver::setMissionStarted(TASKS_ID id)
{
    if(socket->state()!=QAbstractSocket::ConnectedState){
        qDebug() << "The socket is not connected.";
        return;
    }
    bool ok=true;
    char toBeSent[10]={9,0x1A,0};
    toBeSent[3]=ok;
    toBeSent[4]=id;
    socket->write(toBeSent,toBeSent[0]+1);
}

void transceiver::setMissionFinished(TASKS_ID id)
{
    if(socket->state()!=QAbstractSocket::ConnectedState){
        qDebug() << "The socket is not connected.";
        return;
    }
    bool ok=false;
    char toBeSent[10]={9,0x1A,0};
    toBeSent[3]=ok;
    toBeSent[4]=id;
    socket->write(toBeSent,toBeSent[0]+1);
}

void transceiver::attempt()
{
    if(nullptr==socket){
        socket=new QTcpSocket;
        connect(socket,&QTcpSocket::connected,this,&transceiver::sucConnect);
        connect(socket,
                static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error)
                ,this,&transceiver::failConnect);
        connect(socket,&QTcpSocket::disconnected,this,&transceiver::failConnect);
        connect(socket,&QTcpSocket::readyRead,this,&transceiver::onRecvData);
    }
    socket->connectToHost(QHostAddress::LocalHost,6666);
}

void transceiver::setCtr(controller *pCtr)
{
    ctr=pCtr;
    connect(this,&transceiver::acosRecv,ctr,&controller::onAcosRecv);
    connect(this,&transceiver::sbgRecv,ctr,&controller::onSbgRecv);
    connect(this,&transceiver::speedRecv,ctr,&controller::onSpeedRecv);
    connect(this,&transceiver::deepRecv,ctr,&controller::onDeepRecv);
    connect(this,&transceiver::StepRecv,ctr,&controller::onStepRecv);
    connect(ctr,&controller::setAutoDeep,this,&transceiver::setAutoDeep);
    connect(ctr,&controller::setGoal,this,&transceiver::setGoal);
    connect(ctr,&controller::setHMotors,this,&transceiver::setHMotors);
    connect(ctr,&controller::setPitchPID,this,&transceiver::setPitchPID);
    connect(ctr,&controller::setRelay,this,&transceiver::setRelay);
    connect(ctr,&controller::setRollPID,this,&transceiver::setRollPID);
    connect(ctr,&controller::setVMotors,this,&transceiver::setVMotors);
    connect(ctr,&controller::setStep,this,&transceiver::setStepMotor);
    connect(ctr,&controller::setSevo,this,&transceiver::setSevo);
    connect(ctr,&controller::missionStarted,this,&transceiver::setMissionStarted);
    connect(ctr,&controller::missionFinished,this,&transceiver::setMissionFinished);
}

void transceiver::sucConnect()
{
    emit connected(true,socket->peerAddress(),socket->peerPort());
}

void transceiver::failConnect()
{
    emit connected(false);
    socket->abort();
//    QEventLoop loop;
//    QTimer::singleShot(500,&loop,&QEventLoop::quit);
//    loop.exec();
    while(socket->state()!=QAbstractSocket::UnconnectedState);
    QTimer::singleShot(1000,this,&transceiver::attempt);
}

void transceiver::onRecvData()
{
    recvBuffer.append(socket->readAll());
    while(recvBuffer.size()!=0 && recvBuffer.length() > recvBuffer.at(0)){
        if((int)recvBuffer[0]!=0){
            switch (recvBuffer[1]) {
            case 0x04:
                parseSbg();
                break;
            case 0x05:
                parseAcos();
                break;
            case 0x06:
                parseDeep();
                break;
            case 0x07:
                parseHT();
                break;
            case 0x08:
                parseSpeed();
                break;
            case 0x0D:
                parseStepRec();
                break;
            default:
                qDebug() << "Illegal tcp pack ID.";
                break;
            }
        }
        recvBuffer=recvBuffer.remove(0,(int)recvBuffer[0]+1);
    }
}

void transceiver::parseAcos()
{
    float theta1,theta2,phi1,phi2;
    memcpy(&theta1,recvBuffer.data()+2,sizeof(float));
    memcpy(&phi1,recvBuffer.data()+2+sizeof(float),sizeof(float));
    memcpy(&theta2,recvBuffer.data()+2+2*sizeof(float),sizeof(float));
    memcpy(&phi2,recvBuffer.data()+2+3*sizeof(float),sizeof(float));
    emit acosRecv(theta1,phi1,theta2,phi2);
}

void transceiver::parseSbg()
{
    float res[4];
    memcpy(res,recvBuffer.data()+2,4*sizeof(float));
    if(res[2]>180)res[2]=res[2]-360;
    emit sbgRecv(res[2],res[1],res[0],0);
}

void transceiver::parseDeep()
{
    float deep;
    memcpy(&deep,recvBuffer.data()+2,sizeof(float));
    emit deepRecv(deep);
}

void transceiver::parseSpeed()
{
    float res[4];
    int16_t tmp[4];
    memcpy(tmp,recvBuffer.data()+2,4*sizeof(int16_t));
    for(int i=0;i<4;i++){
        res[i]=tmp[i]/10.0;
    }
    emit speedRecv(res[0],res[1],res[2],res[3]);
}

void transceiver::parseHT()
{
    float res[4];
    memcpy(res,recvBuffer.data()+2,4*sizeof(float));
    emit HTRecv(res[0],res[1],res[2],res[3]);
}
void transceiver::parseStepRec()
{
    bool stepflag;
    memcpy(&stepflag,recvBuffer.data()+2,sizeof(bool));
    emit StepRecv(stepflag);
}



