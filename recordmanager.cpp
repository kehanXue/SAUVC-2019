#include "recordmanager.h"
#include <ctime>
#include <QDebug>
#include <QTimer>

recordManager::recordManager(QObject *parent) : QObject(parent),
    trans(nullptr),
    ctr(nullptr),
    vision(nullptr),
    acosCmdBusyFlag(false),
    acosDataSocket(nullptr),
    acosCmdSocket(nullptr),
    status(0),
    inited(0)
{

}

recordManager::~recordManager()
{
    if(acosCmdSocket!=nullptr){
        delete acosCmdSocket;
    }
    if(acosDataSocket!=nullptr){
        delete acosDataSocket;
    }
}

void recordManager::setTrans(transceiver *Trans)
{
    trans=Trans;
}

void recordManager::setCtr(controller *Ctr)
{
    ctr=Ctr;
}

void recordManager::setVision(visionClass *Vision)
{
    vision=Vision;
    connect(this,&recordManager::setPhotoB,vision,&visionClass::visionsave_photo_b);
    connect(this,&recordManager::setPhotoF,vision,&visionClass::visionsave_photo_f);
    connect(this,&recordManager::setVisDebugRecord,vision,&visionClass::visionsave_debug);
}

void recordManager::setStatus(int sw)
{
    if( (sw&CONTROLLER_RECORD)!=0 &&
            initRecord(CONTROLLER_RECORD)){
        status|=CONTROLLER_RECORD;
        //connect signals and slots!
        connect(ctr,&controller::setHMotors,this,&recordManager::setHMotors);
        connect(ctr,&controller::setAutoDeep,this,&recordManager::setAutoDeep);
        connect(ctr,&controller::setVMotors,this,&recordManager::setVMotors);
        connect(ctr,&controller::setRelay,this,&recordManager::setRelay);
        connect(ctr,&controller::setGoal,this,&recordManager::setGoal);
        connect(ctr,&controller::setPitchPID,this,&recordManager::setPitchPID);
        connect(ctr,&controller::setRollPID,this,&recordManager::setRollPID);
        connect(ctr,&controller::debugCtr,this,&recordManager::debugCtr);
    }
    else{
        status&=(~CONTROLLER_RECORD);
        //disconnect them.
        disconnect(ctr,&controller::setHMotors,this,&recordManager::setHMotors);
        disconnect(ctr,&controller::setAutoDeep,this,&recordManager::setAutoDeep);
        disconnect(ctr,&controller::setVMotors,this,&recordManager::setVMotors);
        disconnect(ctr,&controller::setRelay,this,&recordManager::setRelay);
        disconnect(ctr,&controller::setGoal,this,&recordManager::setGoal);
        disconnect(ctr,&controller::setPitchPID,this,&recordManager::setPitchPID);
        disconnect(ctr,&controller::setRollPID,this,&recordManager::setRollPID);
        disconnect(ctr,&controller::debugCtr,this,&recordManager::debugCtr);
    }

    if( (sw&TRANSCEIVER_RECORD)!=0 &&
            initRecord(TRANSCEIVER_RECORD)){
        status|=TRANSCEIVER_RECORD;
        //connect signals and slots!
        connect(trans,&transceiver::acosRecv,this,&recordManager::acosResRecv);
        connect(trans,&transceiver::HTRecv,this,&recordManager::HTRecv);
        connect(trans,&transceiver::sbgRecv,this,&recordManager::sbgRecv);
        connect(trans,&transceiver::deepRecv,this,&recordManager::deepRecv);
        connect(trans,&transceiver::speedRecv,this,&recordManager::speedRecv);
    }
    else{
        status&=(~TRANSCEIVER_RECORD);
        //disconnect them.
        disconnect(trans,&transceiver::acosRecv,this,&recordManager::acosResRecv);
        disconnect(trans,&transceiver::HTRecv,this,&recordManager::HTRecv);
        disconnect(trans,&transceiver::sbgRecv,this,&recordManager::sbgRecv);
        disconnect(trans,&transceiver::deepRecv,this,&recordManager::deepRecv);
        disconnect(trans,&transceiver::speedRecv,this,&recordManager::speedRecv);
    }

    if((sw&PHOTO_RECORD_B)!=0){
        status|=PHOTO_RECORD_B;
        emit setPhotoB(true);
    }
    else{
        status&=(~PHOTO_RECORD_B);
        emit setPhotoB(false);
        disconnect(this,&recordManager::setPhotoB,vision,&visionClass::visionsave_photo_b);
    }

    if((sw&PHOTO_RECORD_F)!=0){
        status|=PHOTO_RECORD_F;
        emit setPhotoF(true);
    }
    else{
        status&=(~PHOTO_RECORD_F);
        emit setPhotoF(false);
        disconnect(this,&recordManager::setPhotoF,vision,&visionClass::visionsave_photo_f);
    }
    if((sw&VISION_RECORD)!=0 && initRecord(VISION_RECORD)){
        status|=VISION_RECORD;
        emit setVisDebugRecord(true);
    }
    else{
        status&=(~VISION_RECORD);
        emit setVisDebugRecord(false);
        disconnect(this,&recordManager::setVisDebugRecord,vision,&visionClass::visionsave_debug);
    }


    if((sw&ACOS_RECORD)!=0 &&
            acosCmdSocket!=nullptr &&
            acosCmdSocket->state()==QAbstractSocket::ConnectedState &&
            initAcosRecord()){
        status|=ACOS_RECORD;
        acosCmdSocket->write(acosStartCmd);
        qDebug() << "Acos started!\n";
    }
    else{
        if(acosCmdSocket!=nullptr &&
                acosCmdSocket->state()==QAbstractSocket::ConnectedState){
            acosCmdSocket->write(acosStopCmd);
            qDebug() << "Acos stopped!\n";
        }

        closeAcosFile();
        status&=~(ACOS_RECORD);
    }

    emit statusSet(status);
}

void recordManager::initAcosSocket()
{
    acosCmdBusyFlag=false;
    if(nullptr==acosCmdSocket){
        acosCmdSocket=new QTcpSocket;
        connect(acosCmdSocket,&QTcpSocket::disconnected,this,&recordManager::onAcosCmdDisconnected);
        connect(acosCmdSocket,&QTcpSocket::connected,this,&recordManager::onAcosCmdConnected);
        connect(acosCmdSocket,
                static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error)
                ,this,&recordManager::failAcosCmdConnect);
        //connect(acosCmdSocket,&QTcpSocket::stateChanged,this,&recordManager::onAcosStateChanged);
    }
    if(nullptr==acosDataSocket){
        acosDataSocket=new QUdpSocket;
        connect(acosDataSocket,&QUdpSocket::readyRead,this,&recordManager::onAcosDataRecv);
        acosDataSocket->bind(acosDataPort);
    }

    acosCmdSocket->connectToHost(acosAddress,acosCmdPort);
    acosCmdSocket->waitForConnected(3000);

    //QAbstractSocket::SocketState temp=acosCmdSocket->state();
}

bool recordManager::initRecord(RECORD_TYPE t)
{
    if((inited&t)!=0){
        return true;
    }

    QString filename;
    std::fstream * fout=nullptr;
    switch (t) {
    case CONTROLLER_RECORD:
        filename="controller";
        fout=&ctrOut;
        break;
    case TRANSCEIVER_RECORD:
        filename="transceiver";
        fout=&transOut;
        break;
    case VISION_RECORD:
        filename="vision";
        fout=&visionOut;
        break;
    default:
        qDebug() << "Illegal record type " << t << "\n";
        return false;
        break;
    }
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

    if(access(fullpath.data(),0)!=-1){
        fout->open(fullpath.data(),std::ios_base::out|std::ios_base::app);
    }
    else{
        fout->open(fullpath.data(),std::ios_base::out|std::ios_base::trunc);
    }

    if(!fout->is_open()){
        qDebug() << "Cannot open file "<< fullpath.data()<< ". Maybe there are some permission issues.\n";
        inited &= (~t);
        fout->close();
        return false;
    }

    inited|=t;
    return true;
}

bool recordManager::initAcosRecord()
{
    if((inited&ACOS_RECORD)!=0){
        return true;
    }

    QString filename="acos";
    //获取当前日期
    time_t clock0=std::time(NULL);
    std::tm * localTime=std::localtime(&clock0);
    //6-length(24bit) postfix
    QString clkPostfix=QString::number(clock0%(1<<24),16);
    if(clkPostfix.length()<6){
        clkPostfix=QString(6-clkPostfix.length(),'0')+clkPostfix;
    }
    else if(clkPostfix.length()>6){
        qDebug() << "It seems some strange thing happened...the length of clkPostfix exceeded 6, as " << clkPostfix << ".\n";
        clkPostfix=clkPostfix.left(6);
    }

    filename+="-";
    filename+=QString::number(localTime->tm_year-2000+1900);
    filename+="-";
    filename+=QString::number(localTime->tm_mon+1);
    filename+="-";
    filename+=QString::number(localTime->tm_mday);
    filename+="-";
    filename+=clkPostfix;
    filename+=".bin";
    QByteArray fullpath=(recordBasePath+filename).toLocal8Bit();

    if(access(fullpath.data(),0)!=-1){
        acosOut.open(fullpath.data(),std::ios_base::out|std::ios_base::app|std::ios_base::binary);
    }
    else{
        acosOut.open(fullpath.data(),std::ios_base::out|std::ios_base::trunc|std::ios_base::binary);
    }

    if(!acosOut.is_open()){
        qDebug() << "Cannot open file "<< fullpath.data()<< ". Maybe there are some permission issues.\n";
        closeAcosFile();
        return false;
    }

    inited|=ACOS_RECORD;
    acosWanted=ACOS_HEAD_LENGTH;
    acosHeadRecv=false;
    return true;
}

void recordManager::closeAcosFile()
{
    inited &= (~ACOS_RECORD);
    acosOut.close();
}

void recordManager::parseAcosData()
{
    if(0==(inited&ACOS_RECORD) || !acosOut.is_open()){
        qDebug() << "The acostic file is not opened. Ignored.\n";
        return;
    }
    if(0!=acosCurHead[2]){
        qDebug() << "Received invalid packet type " << acosCurHead[2] << ".\n";
        return;
    }

    if(!(acosOut.write(acosData.constData(),acosData.length()))){
        qDebug() << "write_failed!\n";
    }
}

void recordManager::onAcosStateChanged()
{
    qDebug() << acosCmdSocket->error();
}

void recordManager::failAcosCmdConnect()
{
    if(acosCmdBusyFlag){
        return;
    }
    else {
        acosCmdBusyFlag=true;
    }
    qDebug() << acosCmdSocket->error();
    qDebug() << "Connection to acos board failed!\n";
    status&=~(ACOS_RECORD);
    closeAcosFile();
    acosCmdSocket->abort();
    while(acosCmdSocket->state()!=QAbstractSocket::UnconnectedState);
    QTimer::singleShot(5000,this,&recordManager::initAcosSocket);
    emit acosConnected(acosAddress,false);
}

void recordManager::onAcosCmdConnected()
{
    qDebug() << "Connection to acos board successed!\n";
    status&=~(ACOS_RECORD);
    closeAcosFile();
    acosCmdSocket->write(acosStopCmd);
    emit acosConnected(acosAddress,true);
}

void recordManager::onAcosCmdDisconnected()
{
    failAcosCmdConnect();
}

void recordManager::onAcosDataRecv()
{
    while(acosDataSocket->hasPendingDatagrams()){

        bool validFlag = 0!=(status&ACOS_RECORD) ||
                acosCmdSocket->state()!=QAbstractSocket::ConnectedState ||
                0!=(inited&ACOS_RECORD);
        if(!validFlag){
            qDebug() << "Invalid state. The data will be dropped.\n";
            acosWanted=ACOS_HEAD_LENGTH;
            acosHeadRecv=false;
            acosData.clear();
        }

        QByteArray datagram;
        datagram.resize(acosDataSocket->pendingDatagramSize());
        QHostAddress sender;
        uint16_t senderPort;

        acosDataSocket->readDatagram(datagram.data(), datagram.size(),
                                &sender, &senderPort);
        //if(acosAddress!=sender || acosDataPort!=senderPort){
        //    qDebug() << "Unexpected sender address " << sender << " at port " << senderPort << ".\n";
        //    continue;
        //}
        if(!validFlag){
            continue;
        }
        if(datagram.length()==0){
            qDebug() << "A empty UDP packet??\n";
            continue;
        }

        //状态机，不要使用[]运算符，否则复制开销极大
        while(!datagram.isEmpty()){
            //1包头处理
            if(!acosHeadRecv){
                //1.1 acosCurHead为空的情况
                //1.1.1数据包长度大于2
                if(ACOS_HEAD_LENGTH==acosWanted && datagram.length()>=2){
                    //1.1.1.1包头的包头正确，开始parse
                    if(acosSig[0]==datagram.at(0) && acosSig[1]==datagram.at(1)){
                        acosCurHead[0]=datagram.at(0);
                        acosCurHead[1]=datagram.at(1);
                        datagram=datagram.right(datagram.length()-2);
                        acosWanted-=2;
                    } //end 1.1.1.1
                    //1.1.1.2包头不在起始位置，需要查找
                    else {
                        int i;
                        for(i=0;i<datagram.length()-1;i++){
                            if(acosSig[0]==datagram.at(i) && acosSig[1]==datagram.at(i+1)){
                                break;
                            }
                        }
                        //找到完整acosSig
                        if(i<=datagram.length()-1){
                            datagram=datagram.right(datagram.length()-i);
                        }
                        //找到尾部acosSig[0]
                        else if(acosSig[0]==datagram.at(datagram.length()-1)){
                            datagram=datagram.right(1);
                        }
                        //未找到，此段数据无效
                        else {
                            datagram.clear();
                        }
                    } //end 1.1.1.2
                } // end 1.1.1
                //1.1.2数据包长度为1
                else if(ACOS_HEAD_LENGTH==acosWanted && datagram.length()==1){
                    acosCurHead[0]=datagram.at(0);
                    datagram.clear();
                    acosWanted-=1;
                } //end 1.1.2
                //end 1.1
                //1.2 acosCurHead有一个数据的情况
                else if(ACOS_HEAD_LENGTH-1==acosWanted){
                    //如果能接上,有效，不能接上，前面的acosSig[0]为虚假，清掉
                    if(acosSig[1]==datagram.at(0)){
                        acosCurHead[1]=datagram.at(0);
                        acosWanted-=1;
                        datagram=datagram.right(datagram.length()-1);
                    }
                    else {
                        acosWanted=ACOS_HEAD_LENGTH;
                        datagram=datagram.right(datagram.length()-1);
                    }
                } //end 1.2
                //1.3 两个包头字节已收到
                else if(ACOS_HEAD_LENGTH-2>=acosWanted){
                    //如果已经足够，清除acosData并切换到记录数据模式
                    if(datagram.length()>=acosWanted){
                        memcpy(acosCurHead+ACOS_HEAD_LENGTH-acosWanted,datagram.constData(),acosWanted);
                        datagram=datagram.right(datagram.length()-acosWanted);
                        acosWanted=*((uint16_t *)(acosCurHead+4));
                        acosData.clear();
                        acosHeadRecv=true;
                    }
                    else {
                        memcpy(acosCurHead+ACOS_HEAD_LENGTH-acosWanted,datagram.constData(),datagram.length());
                        acosWanted-=datagram.length();
                        datagram.clear();
                    }
                }// end 1.3
            } //end 1
            //2 接收数据
            else {
                if(datagram.length()>=acosWanted){
                    acosData.append(datagram.left(acosWanted));
                    datagram=datagram.right(datagram.length()-acosWanted);
                    parseAcosData();
                    acosHeadRecv=false;
                    acosWanted=ACOS_HEAD_LENGTH;
                    //acosData.clear();
                }
                else {
                    acosData.append(datagram);
                    acosWanted-=datagram.length();
                    datagram.clear();
                }
            } //end 2
        }
    }
}

void recordManager::acosResRecv(float theta1, float phi1, float theta2, float phi2)
{
    if(!transOut || !transOut.is_open()){
        return;
    }

    transOut << ctr->getTime() << ",Acoutic,theta1," << theta1 << ",phi1," << phi1 << ",theta2," << theta2 << ",phi2," << phi2 <<"\n";
}

void recordManager::sbgRecv(float roll, float pitch, float yaw, float magNorm)
{
    if(!transOut || !transOut.is_open()){
        return;
    }

    transOut << ctr->getTime() << ",Sbg,roll," << roll << ",pitch," << pitch;
    transOut << ",yaw," << yaw << "magNorm" << magNorm << "\n";
}

void recordManager::deepRecv(float deep)
{
    if(!transOut || !transOut.is_open()){
        return;
    }

    transOut << ctr->getTime() << ",Deep,deep," << deep << "\n";
}

void recordManager::speedRecv(float lu, float ld, float ru, float rd)
{
    if(!transOut || !transOut.is_open()){
        return;
    }

    transOut << ctr->getTime() << ",Speed,left_up," << lu << ",left_down," << ld;
    transOut << ",right_up," << ru << "right_down" << rd << "\n";
}

void recordManager::HTRecv(float humi1, float tem1, float humi2, float tem2)
{
    if(!transOut || !transOut.is_open()){
        return;
    }

    transOut << ctr->getTime() << ",HumiHeat,humi1," << humi1 << ",tem1," << tem1;
    transOut << ",humi2," << humi2 << "tem2" << tem2 << "\n";
}

void recordManager::setHMotors(QList<pair<MOTORS, float> > value)
{
    float tmp[4]={999.9,999.9,999.9,999.9};
    for(auto & t:value){
        switch (t.first) {
        case MAIN_LEFT:
            tmp[0]=t.second;
            break;
        case MAIN_RIGHT:
            tmp[1]=t.second;
            break;
        case SIDE_UP:
            tmp[2]=t.second;
            break;
        case SIDE_DOWN:
            tmp[3]=t.second;
            break;
        default:
            break;
        }
    }

    ctrOut << std::fixed << std::setprecision(1);
    ctrOut << ctr->getTime() << ",HMotors";
    if(tmp[0]<999)ctrOut << ",main_left," << tmp[0];
    if(tmp[1]<999)ctrOut << ",main_right," << tmp[1];
    if(tmp[2]<999)ctrOut << ",side_up," << tmp[2];
    if(tmp[3]<999)ctrOut << ",side_down," << tmp[3];
    ctrOut << "\n";
}

void recordManager::setAutoDeep(bool sw, QList<configManager::PIDValue> pid)
{
    if(!sw){
        ctrOut << ctr->getTime() << ",AutoDeep,status,off\n";
        return;
    }

    ctrOut << ctr->getTime() << ",AutoDeep,status,on";
    ctrOut << std::fixed << std::setprecision(1);
    for(auto & t:pid){
        switch (t.motorID) {
        case LEFT_UP:
            ctrOut << ",left_up_p," << t.pValue;
            ctrOut << ",left_up_i," << t.iValue;
            ctrOut << ",left_up_d," << t.dValue;
            break;
        case LEFT_DOWN:
            ctrOut << ",left_down_p," << t.pValue;
            ctrOut << ",left_down_i," << t.iValue;
            ctrOut << ",left_down_d," << t.dValue;
            break;
        case RIGHT_UP:
            ctrOut << ",right_up_p," << t.pValue;
            ctrOut << ",right_up_i," << t.iValue;
            ctrOut << ",right_up_d," << t.dValue;
            break;
        case RIGHT_DOWN:
            ctrOut << ",right_down_p," << t.pValue;
            ctrOut << ",right_down_i," << t.iValue;
            ctrOut << ",right_down_d," << t.dValue;
            break;
        default:
            break;
        }
    }
    ctrOut << "\n";
}

void recordManager::setVMotors(QList<pair<MOTORS, float> > value)
{
    float tmp[4]={999.9,999.9,999.9,999.9};
    for(auto & t:value){
        switch (t.first) {
        case LEFT_UP:
            tmp[0]=t.second;
            break;
        case LEFT_DOWN:
            tmp[1]=t.second;
            break;
        case RIGHT_UP:
            tmp[2]=t.second;
            break;
        case RIGHT_DOWN:
            tmp[3]=t.second;
            break;
        default:
            break;
        }
    }

    ctrOut << std::fixed << std::setprecision(1);
    ctrOut << ctr->getTime() << ",VMotors";
    if(tmp[0]<999)ctrOut << ",left_up," << tmp[0];
    if(tmp[1]<999)ctrOut << ",left_down," << tmp[1];
    if(tmp[2]<999)ctrOut << ",right_up," << tmp[2];
    if(tmp[3]<999)ctrOut << ",right_down," << tmp[3];
    ctrOut << "\n";
}

void recordManager::setRelay(QList<pair<int, bool> > value)
{
    ctrOut << ctr->getTime() << ",Relay";
    for(auto & t:value){
        //继电器编号从0开始
        ctrOut << "," << t.first << "," << t.second;
    }
    ctrOut << "\n";
}

void recordManager::setGoal(float deep, float pitch, float roll)
{
    ctrOut << ctr->getTime() << ",Goal";
    ctrOut << ",deep," << deep;
    ctrOut << ",pitch," << pitch;
    ctrOut << ",roll," << roll;
    ctrOut << "\n";
}

void recordManager::setPitchPID(QList<configManager::PIDValue> value)
{
    ctrOut << ctr->getTime() << ",PitchPID";
    ctrOut << std::fixed << std::setprecision(1);
    for(auto & t:value){
        switch (t.motorID) {
        case LEFT_UP:
            ctrOut << ",left_up_p," << t.pValue;
            ctrOut << ",left_up_i," << t.iValue;
            ctrOut << ",left_up_d," << t.dValue;
            break;
        case LEFT_DOWN:
            ctrOut << ",left_down_p," << t.pValue;
            ctrOut << ",left_down_i," << t.iValue;
            ctrOut << ",left_down_d," << t.dValue;
            break;
        case RIGHT_UP:
            ctrOut << ",right_up_p," << t.pValue;
            ctrOut << ",right_up_i," << t.iValue;
            ctrOut << ",right_up_d," << t.dValue;
            break;
        case RIGHT_DOWN:
            ctrOut << ",right_down_p," << t.pValue;
            ctrOut << ",right_down_i," << t.iValue;
            ctrOut << ",right_down_d," << t.dValue;
            break;
        default:
            break;
        }
    }
    ctrOut << "\n";
}

void recordManager::setRollPID(QList<configManager::PIDValue> value)
{
    ctrOut << ctr->getTime() << ",RollPID";
    ctrOut << std::fixed << std::setprecision(1);
    for(auto & t:value){
        switch (t.motorID) {
        case LEFT_UP:
            ctrOut << ",left_up_p," << t.pValue;
            ctrOut << ",left_up_i," << t.iValue;
            ctrOut << ",left_up_d," << t.dValue;
            break;
        case LEFT_DOWN:
            ctrOut << ",left_down_p," << t.pValue;
            ctrOut << ",left_down_i," << t.iValue;
            ctrOut << ",left_down_d," << t.dValue;
            break;
        case RIGHT_UP:
            ctrOut << ",right_up_p," << t.pValue;
            ctrOut << ",right_up_i," << t.iValue;
            ctrOut << ",right_up_d," << t.dValue;
            break;
        case RIGHT_DOWN:
            ctrOut << ",right_down_p," << t.pValue;
            ctrOut << ",right_down_i," << t.iValue;
            ctrOut << ",right_down_d," << t.dValue;
            break;
        default:
            break;
        }
    }
    ctrOut << "\n";
}

void recordManager::debugCtr(const QString &info)
{
    ctrOut << ctr->getTime() << "," << info.toStdString() << "\n";
}

