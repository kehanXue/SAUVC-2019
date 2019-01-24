#include "dialog.h"
#include <QDebug>

Dialog::Dialog(QWidget *parent)
    : QDialog(parent),
      sbgLists(SBG_PARA_NUM,{nullptr,nullptr}),
      sbgStrs({
          QString::fromWCharArray(L"航向角yaw:"),
          QString::fromWCharArray(L"横滚角roll:"),
          QString::fromWCharArray(L"俯仰角pitch:"),
          QString::fromWCharArray(L"磁场相关度:")
      }),
      acosLists(ACOS_PARA_NUM,{nullptr,nullptr}),
      acosStrs({
          QString::fromWCharArray(L"theta1:"),
          QString::fromWCharArray(L"phi1:"),
          QString::fromWCharArray(L"theta2:"),
          QString::fromWCharArray(L"phi2:")
      }),
      deepLists(DEEP_PARA_NUM,{nullptr,nullptr}),
      deepStrs({
          QString::fromWCharArray(L"深度:"),
          QString::fromWCharArray(L"深度修正量:")
      }),
      humiAndHeatLists(HUH_PARA_NUM,{nullptr,nullptr}),
      humiAndHeatStrs({
          QString::fromWCharArray(L"湿度1/%:"),
          QString::fromWCharArray(L"温度1/℃:"),
          QString::fromWCharArray(L"湿度2/%:"),
          QString::fromWCharArray(L"温度2/℃:")
      }),
      relayLists(RELAY_PARA_NUM,{nullptr,nullptr}),
      relayStrs({
          QString::fromWCharArray(L"继电器:")
      }),
      connectionLists(CONNECTION_PARA_NUM,{nullptr,nullptr}),
      connectionStrs({
          QString::fromWCharArray(L"目的地址:"),
          QString::fromWCharArray(L"状态:"),
          QString::fromWCharArray(L"水声地址:"),
          QString::fromWCharArray(L"状态:")
      }),
      speedLists(SPEED_PARA_NUM,{nullptr,nullptr}),
      speedStrs({
          QString::fromWCharArray(L"左前:"),
          QString::fromWCharArray(L"左后:"),
          QString::fromWCharArray(L"右前:"),
          QString::fromWCharArray(L"右后:"),
          QString::fromWCharArray(L"主左:"),
          QString::fromWCharArray(L"主右:"),
          QString::fromWCharArray(L"侧前:"),
          QString::fromWCharArray(L"侧后:")
      }),
      remoteDlg(nullptr),
      visDlg(nullptr),
      pidDlg(nullptr),
      config(nullptr),
      record_status(0)
{

    taskGroup=new QGroupBox(QString::fromWCharArray(L"任务"));
    taskListLabel=new QLabel(QString::fromWCharArray(L"任务清单:"));
    taskList=new QListWidget;
    taskPckLabel=new QLabel(QString::fromWCharArray(L"任务包："));
    //curTaskStatusLabel=new QLabel(QString::fromWCharArray(L"任务状态："));
    curactionStatusLabel=new QLabel(QString::fromWCharArray(L"子任务状态："));
    selectTaskPck=new QComboBox;
    //通过读取文件插入taskList和selectTaskPck
    startTask=new QPushButton(QString::fromWCharArray(L"开始"));
    stopTask=new QPushButton(QString::fromWCharArray(L"结束"));
    taskLayout=new QGridLayout;
    taskLayout->addWidget(taskPckLabel,1,1,1,1);
    taskLayout->addWidget(selectTaskPck,1,2,1,2);
    taskLayout->addWidget(taskListLabel,2,1,1,1);
    //taskLayout->addWidget(curTaskStatusLabel,3,1,1,1);
    taskLayout->addWidget(taskList,3,1,6,3);
    taskLayout->addWidget(startTask,9,1,1,1);
    taskLayout->addWidget(stopTask,9,3,1,1);
    taskLayout->addWidget(curactionStatusLabel,10,1,1,1);
    taskGroup->setLayout(taskLayout);

    PIDGroup=new QGroupBox(QString::fromWCharArray(L"PID参数"));
    PIDStatus=new QLabel(QString::fromWCharArray(L"等待读取信息..."));
    editPID=new QPushButton(QString::fromWCharArray(L"编辑参数"));
    PIDLayout=new QHBoxLayout;
    PIDLayout->addWidget(PIDStatus,2);
    PIDLayout->addWidget(editPID,1);
    PIDGroup->setLayout(PIDLayout);

    monitorGroup=new QGroupBox(QString::fromWCharArray(L"仪表板"));
    monitorLayout=new QGridLayout;

    sbgGroup=new QGroupBox(QString::fromWCharArray(L"SBG"));
    sbgLayout=new QGridLayout;
    initLabelArray(sbgLists,sbgLayout,sbgStrs);
    sbgGroup->setLayout(sbgLayout);
    monitorLayout->addWidget(sbgGroup,1,1,2,2);

    acosGroup=new QGroupBox(QString::fromWCharArray(L"水声"));
    acosLayout=new QGridLayout;
    initLabelArray(acosLists,acosLayout,acosStrs);
    acosGroup->setLayout(acosLayout);
    monitorLayout->addWidget(acosGroup,1,3,2,2);

    deepGroup=new QGroupBox(QString::fromWCharArray(L"深度"));
    deepLayout=new QGridLayout;
    initLabelArray(deepLists,deepLayout,deepStrs);
    enterNewRev=new QLabel(QString::fromWCharArray(L"输入新修正量:"));
    newRev=new QLineEdit;
    setNewRev=new QPushButton(QString::fromWCharArray(L"保存"));
    connect(setNewRev,&QPushButton::clicked,this,&Dialog::onSetRev);
    deepLayout->addWidget(enterNewRev,3,1,1,1);
    deepLayout->addWidget(newRev,3,2,1,2);
    deepLayout->addWidget(setNewRev,3,4,1,1);
    deepGroup->setLayout(deepLayout);
    monitorLayout->addWidget(deepGroup,3,1,2,2);

    humiAndHeatGroup=new QGroupBox(QString::fromWCharArray(L"温湿度"));
    humiAndHeatLayout=new QGridLayout;
    initLabelArray(humiAndHeatLists,humiAndHeatLayout,humiAndHeatStrs);
    humiAndHeatGroup->setLayout(humiAndHeatLayout);
    monitorLayout->addWidget(humiAndHeatGroup,3,3,2,2);
    
    relayGroup=new QGroupBox(QString::fromWCharArray(L"继电器"));
    relayLayout=new QGridLayout;
    initLabelArray(relayLists,relayLayout,relayStrs);
    resetAcos=new QPushButton(QString::fromWCharArray(L"水声重启"));
    resetStm=new QPushButton(QString::fromWCharArray(L"STM32复位"));
    openRmt=new QPushButton(QString::fromWCharArray(L"遥控"));
    openVis=new QPushButton(QString::fromWCharArray(L"图像窗口"));
    relayLayout->addWidget(resetAcos,1,3,1,1);
    relayLayout->addWidget(resetStm,1,4,1,1);
    relayLayout->addWidget(openRmt,1,5,1,1);
    relayGroup->setLayout(relayLayout);
    monitorLayout->addWidget(relayGroup,5,1,1,4);

    connectionGroup=new QGroupBox(QString::fromWCharArray(L"通信连接"));
    for(int i=0;i<CONNECTION_PARA_NUM;i++){
        initLabel(connectionLists[i],connectionStrs[i]);
    }
    connectionLayout=new QHBoxLayout;
    connectionLayout->addWidget(connectionLists[CONNECTION_TARGET].first,1);
    connectionLayout->addWidget(connectionLists[CONNECTION_TARGET].second,1);
    connectionLayout->addWidget(connectionLists[CONNECTION_STATUS].first,1);
    connectionLayout->addWidget(connectionLists[CONNECTION_STATUS].second,1);
    connectionLayout->addWidget(connectionLists[CONNECTION_ACOS_TARGET].first,1);
    connectionLayout->addWidget(connectionLists[CONNECTION_ACOS_TARGET].second,1);
    connectionLayout->addWidget(connectionLists[CONNECTION_ACOS_STATUS].first,1);
    connectionLayout->addWidget(connectionLists[CONNECTION_ACOS_STATUS].second,1);
    //openAcos=new QPushButton(QString::fromWCharArray(L"开启水声采集"));
    //closeAcos=new QPushButton(QString::fromWCharArray(L"关闭水声采集"));
    //connect(openAcos,&QPushButton::clicked,this,&Dialog::onOpenAcosClicked);
    //connect(closeAcos,&QPushButton::clicked,this,&Dialog::onCloseAcosClicked);
    //connectionLayout->addWidget(openAcos,1);
    //connectionLayout->addWidget(closeAcos,1);
    connectionGroup->setLayout(connectionLayout);
    monitorLayout->addWidget(connectionGroup,6,1,1,4);

    speedGroup=new QGroupBox(QString::fromWCharArray(L"推进器"));
    speedLayout=new QGridLayout;
    initLabelArray(speedLists,speedLayout,speedStrs);
    speedGroup->setLayout(speedLayout);
    monitorLayout->addWidget(speedGroup,1,5,3,3);

    visionGroup=new QGroupBox(QString::fromWCharArray(L"图像"));
    visionLayout=new QGridLayout;
    //filenameLabel=new QLabel(QString::fromWCharArray(L"已读取的文件:"));
    //filenameList=new QListWidget;
    //freshFile=new QPushButton(QString::fromWCharArray(L"刷新"));
    //visionLayout->addWidget(filenameLabel,1,1,1,1);
    //visionLayout->addWidget(filenameList,2,1,3,3);
    //visionLayout->addWidget(freshFile,1,3,1,1);
    visionLayout->addWidget(openVis,1,1,1,1);
    visionGroup->setLayout(visionLayout);
    monitorLayout->addWidget(visionGroup,4,5,1,3);

    recordGroup=new QGroupBox(QString::fromWCharArray(L"数据记录开关"));
    recordLayout=new QGridLayout;
    acosRecord=new QPushButton(QString::fromWCharArray(L"水声数据\n当前关闭"));
    ctrRecord=new QPushButton(QString::fromWCharArray(L"控制输出\n当前关闭"));
    transRecord=new QPushButton(QString::fromWCharArray(L"外部输入\n当前关闭"));
    visionRecord=new QPushButton(QString::fromWCharArray(L"图像信息\n当前关闭"));
    //photoRecord=new QPushButton(QString::fromWCharArray(L"图像采集\n当前关闭"));
    photoFRecord=new QPushButton(QString::fromWCharArray(L"前视图像\n当前关闭"));
    photoBRecord=new QPushButton(QString::fromWCharArray(L"下视图像\n当前关闭"));
    allCloseRecord=new QPushButton(QString::fromWCharArray(L"全部关闭"));
    allOpenRecord=new QPushButton(QString::fromWCharArray(L"全部打开"));
    connect(acosRecord,&QPushButton::clicked,this,&Dialog::onOpenAcosRecord);
    connect(ctrRecord,&QPushButton::clicked,this,&Dialog::onOpenCtrRecord);
    connect(transRecord,&QPushButton::clicked,this,&Dialog::onOpenTransRecord);
    connect(visionRecord,&QPushButton::clicked,this,&Dialog::onOpenVisionRecord);
    //connect(photoRecord,&QPushButton::clicked,this,&Dialog::onOpenPhotoRecord);
    connect(photoFRecord,&QPushButton::clicked,this,&Dialog::onOpenPhotoFRecord);
    connect(photoBRecord,&QPushButton::clicked,this,&Dialog::onOpenPhotoBRecord);
    connect(allCloseRecord,&QPushButton::clicked,this,&Dialog::onAllCloseRecord);
    connect(allOpenRecord,&QPushButton::clicked,this,&Dialog::onAllOpenRecord);
    recordLayout->addWidget(acosRecord,1,1,1,1);
    recordLayout->addWidget(ctrRecord,1,2,1,1);
    recordLayout->addWidget(transRecord,2,1,1,1);
    recordLayout->addWidget(visionRecord,2,2,1,1);
    recordLayout->addWidget(photoFRecord,3,1,1,1);
    recordLayout->addWidget(photoBRecord,3,2,1,1);
    recordLayout->addWidget(allCloseRecord,4,1,1,1);
    recordLayout->addWidget(allOpenRecord,4,2,1,1);
    recordGroup->setLayout(recordLayout);
    monitorLayout->addWidget(recordGroup,5,5,3,3);

    monitorGroup->setLayout(monitorLayout);

    mainLayout=new QGridLayout;
    mainLayout->addWidget(monitorGroup,1,1,7,4);
    mainLayout->addWidget(PIDGroup,1,5,1,4);
    mainLayout->addWidget(taskGroup,2,5,8,4);

    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(mainLayout); 

    config=new configManager(this);
    config->setTaskBasePath("/home/nwpu/Code/Robosub_test2018-2-8/tasks");
    config->loadTask();
    updatePackCombo();
    connect(selectTaskPck,
            static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this,
            &Dialog::updateTaskList);

    pidDlg=new PIDDialog(this);
    connect(pidDlg,&PIDDialog::PIDInfo,this,&Dialog::onPIDInfo);
    connect(editPID,&QPushButton::clicked,this,&Dialog::onEditPID);
    pidDlg->setConfig(config);

    visionThread=new QThread(this);
    vision=new visionClass;
    connect(vision,&visionClass::imageShow,this,&Dialog::onImageShow);
    connect(vision,&visionClass::createWindow,this,&Dialog::onCreateWindow);
    connect(vision,&visionClass::createBar,this,&Dialog::createBar);
    connect(vision,&visionClass::destroyWindow,this,&Dialog::onDestroyWindow);
    connect(this,&Dialog::initAll,vision,&visionClass::onCreateTimer);
    //为了确保visionClass未完成时编译通过,完成后必须去掉注释!
    connect(this,&Dialog::initAll,vision,&visionClass::camInitialize);
    vision->moveToThread(visionThread);
    visionThread->start();

    ctrThread=new QThread(this);
    ctr=new controller;
    startTask->setEnabled(false);
    stopTask->setEnabled(false);
    connect(this,&Dialog::startSig,ctr,&controller::excecute);
    connect(this,&Dialog::stopSig,ctr,&controller::stop);
    connect(startTask,&QPushButton::clicked,this,&Dialog::onStartTask);
    connect(stopTask,&QPushButton::clicked,this,&Dialog::onStopTask);
    //connect(ctr,&controller::changeTasksLabel,this,&Dialog::onChangeCurActionLabel);
    connect(ctr,&controller::changeCurActionLabel,this,&Dialog::onChangeCurActionLabel);
    connect(this,&Dialog::setDeepRev,ctr,&controller::onSetDeepRev);
    connect(ctr,&controller::setHMotors,this,&Dialog::onSetHMotors);
    connect(ctr,&controller::setVMotors,this,&Dialog::onSetVMotors);
    connect(ctr,&controller::setRelay,this,&Dialog::onSetRelay);
    connect(this,&Dialog::initAll,ctr,&controller::createTimer);
    connect(ctr,&controller::allFinished,this,&Dialog::onFinishedTask);
    ctr->setConfig(config);
    ctr->setVision(vision);
    ctr->moveToThread(ctrThread);
    ctrThread->start();
    startTask->setEnabled(true);

    transThread=new QThread(this);
    trans=new transceiver;
    connect(this,&Dialog::initAll,trans,&transceiver::attempt);
    connect(trans,&transceiver::connected,this,&Dialog::onConnected);
    connect(trans,&transceiver::sbgRecv,this,&Dialog::onSbgRecv);
    connect(trans,&transceiver::acosRecv,this,&Dialog::onAcosRecv);
    connect(trans,&transceiver::deepRecv,this,&Dialog::onDeepRecv);
    connect(trans,&transceiver::speedRecv,this,&Dialog::onSpeedRecv);
    connect(trans,&transceiver::HTRecv,this,&Dialog::onHTRecv);
    connect(this,&Dialog::setDeepRev,trans,&transceiver::setDeepRev);
    connect(ctr, SIGNAL(controller::stop_thread), vision, SLOT(visionClass::stop_Thread));

    trans->setCtr(ctr);
    trans->moveToThread(transThread);
    transThread->start();

    recordThread=new QThread(this);
    record=new recordManager;
    connect(this,&Dialog::setRecordStatus,record,&recordManager::setStatus);
    connect(record,&recordManager::statusSet,this,&Dialog::onSetRecordStatus);
    connect(this,&Dialog::initAll,record,&recordManager::initAcosSocket);
    connect(record,&recordManager::acosConnected,this,&Dialog::onAcosConnected);
    record->setCtr(ctr);
    record->setTrans(trans);
    record->setVision(vision);
    record->moveToThread(recordThread);
    recordThread->start();

    remoteDlg=new remoteDialog(this);
    remoteDlg->setCtr(ctr);
    remoteDlg->setTrans(trans);
    connect(openRmt,&QPushButton::clicked,this,&Dialog::onOpenRmt);
    connect(resetStm,&QPushButton::clicked,trans,&transceiver::stmReset);

    visDlg = new visionDialog(this,vision);
    connect(openVis,&QPushButton::clicked,this,&Dialog::onOpenVis);
    emit initAll();
}

Dialog::~Dialog()
{
    ctrThread->quit();
    transThread->quit();
    visionThread->quit();
    recordThread->quit();
}

void Dialog::updateTaskList()
{
    taskList->clear();
    const vector<TASKS_ID> && tempID=config->getTaskIDList(selectTaskPck->currentText());
    const vector<QString> && tempStr=config->getTaskStrList(selectTaskPck->currentText());
    QListWidgetItem * tItem=nullptr;
    auto str=tempStr.cbegin();
    auto id=tempID.cbegin();
    for(;
        str!=tempStr.cend() && id!=tempID.cend();
        str++,id++){
        tItem=new QListWidgetItem();
        tItem->setData(TASK_ID_ROLE,*id);
        tItem->setText(*str);
        tItem->setFlags(tItem->flags()|Qt::ItemIsUserCheckable);
        tItem->setCheckState(Qt::Unchecked);  //之后改成Checked
        taskList->addItem(tItem);
    }
}

void Dialog::updatePackCombo()
{
    disconnect(selectTaskPck,
            static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this,
            &Dialog::updateTaskList);
    selectTaskPck->clear();
    const vector<QString> && temp=config->getPackList();
    for(auto & str:temp){
        selectTaskPck->addItem(str);
    }
    updateTaskList();
    connect(selectTaskPck,
            static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this,
            &Dialog::updateTaskList);
}

void Dialog::onPIDInfo(bool ok, QString info)
{
    PIDStatus->setText(info);
    if(!ok){
        PIDStatus->setPalette(QPalette(Qt::red));
    }
}

void Dialog::onEditPID()
{
    pidDlg->show();
}

void Dialog::onOpenRmt()
{
    remoteDlg->show();
}

void Dialog::onOpenVis()
{
    visDlg->show();
}

void Dialog::onStartTask()
{
    startTask->setEnabled(false);
    taskList->setEnabled(false);
    selectTaskPck->setEnabled(false);
    QList<TASKS_ID> temp;
    QListWidgetItem * tPtr=nullptr;
    for(int i=0;i<taskList->count();i++){
        tPtr=taskList->item(i);
        if(tPtr!=nullptr && tPtr->checkState()==Qt::Checked){
            qDebug() << tPtr->data(TASK_ID_ROLE);
            temp << (TASKS_ID)(tPtr->data(TASK_ID_ROLE).value<int>());
            qDebug() << temp;
            qDebug() << tPtr->text();
        }
    }
    if(temp.empty()){
        qDebug() << "The selected task list is empty. Cannot start.\n";
        startTask->setEnabled(true);
        taskList->setEnabled(true);
        selectTaskPck->setEnabled(true);
        return;
    }
    ctr->setTaskList(temp);
    emit startSig();
    stopTask->setEnabled(true);
}

void Dialog::onChangeCurActionLabel(ACTION_ID ms)
{
    switch(ms)
    {
    case NOP:
        curactionStatusLabel -> setText(QString::fromWCharArray(L"子任务状态："));
        break;
    case FORWARDACTION:
        curactionStatusLabel -> setText(QString::fromWCharArray(L"FORWORD"));
        break;
    case BACKWARDACTION:
        curactionStatusLabel -> setText(QString::fromWCharArray(L"BACKWORD"));
        break;
    case ROTATEACTION:
        curactionStatusLabel -> setText(QString::fromWCharArray(L"ROTATE"));
        break;
    default:
        break;
    }
    //qDebug()<<"Success Changed";
}
void Dialog::onStopTask()
{
    emit stopSig();
}

void Dialog::onFinishedTask()
{
    stopTask->setEnabled(false);
    startTask->setEnabled(true);
    taskList->setEnabled(true);
    selectTaskPck->setEnabled(true);
}

void Dialog::onConnected(bool connected, QHostAddress addr, int port)
{
    if(!connected){
        getLabel(connectionLists[CONNECTION_TARGET])->setText("null");
        getLabel(connectionLists[CONNECTION_STATUS])->setText(_WSTR(L"未连接"));
    }
    else{
        getLabel(connectionLists[CONNECTION_TARGET])->setText(addr.toString()+":"+QString::number(port));
        getLabel(connectionLists[CONNECTION_STATUS])->setText(_WSTR(L"已连接"));
    }
}

void Dialog::onSbgRecv(float roll, float pitch, float yaw, float magNorm)
{
    getLabel(sbgLists[SBG_ROLL])->setText(QString::number(roll,'f',3));
    getLabel(sbgLists[SBG_PITCH])->setText(QString::number(pitch,'f',3));
    getLabel(sbgLists[SBG_YAW])->setText(QString::number(yaw,'f',3));
    getLabel(sbgLists[SBG_MAG])->setText(QString::number(magNorm,'f',3));
}

void Dialog::onAcosRecv(float theta1, float phi1, float theta2, float phi2)
{
    getLabel(acosLists[ACOS_THETA1])->setText(QString::number(theta1,'f',1));
    getLabel(acosLists[ACOS_PHI1])->setText(QString::number(phi1,'f',1));
    getLabel(acosLists[ACOS_THETA2])->setText(QString::number(theta2,'f',1));
    getLabel(acosLists[ACOS_PHI2])->setText(QString::number(phi2,'f',1));
}

void Dialog::onDeepRecv(float deep)
{
    getLabel(deepLists[DEEP_VALUE])->setText(QString::number(deep,'f',3));
}

void Dialog::onSpeedRecv(float lu, float ld, float ru, float rd)
{
    getLabel(speedLists[SPEED_LEFT_UP])->setText(QString::number(lu,'f',1));
    getLabel(speedLists[SPEED_LEFT_DOWN])->setText(QString::number(ld,'f',1));
    getLabel(speedLists[SPEED_RIGHT_UP])->setText(QString::number(ru,'f',1));
    getLabel(speedLists[SPEED_RIGHT_DOWN])->setText(QString::number(rd,'f',1));
}

void Dialog::onHTRecv(float humi1, float tem1, float humi2, float tem2)
{
    getLabel(humiAndHeatLists[HUH_HUMI1])->setText(QString::number(humi1,'f',1));
    getLabel(humiAndHeatLists[HUH_HEAT1])->setText(QString::number(tem1,'f',1));
    getLabel(humiAndHeatLists[HUH_HUMI2])->setText(QString::number(humi2,'f',1));
    getLabel(humiAndHeatLists[HUH_HEAT2])->setText(QString::number(tem2,'f',1));
}

void Dialog::onSetHMotors(QList<pair<MOTORS, float> > value)
{
    for(auto & t:value){
        //使用switch检查输入是否合法
        switch (t.first) {
        case MAIN_LEFT:
            getLabel(speedLists[SPEED_MAIN_LEFT])->setText(QString::number(t.second,'f',1));
            break;
        case MAIN_RIGHT:
            getLabel(speedLists[SPEED_MAIN_RIGHT])->setText(QString::number(t.second,'f',1));
            break;
        case SIDE_UP:
            getLabel(speedLists[SPEED_SIDE_UP])->setText(QString::number(t.second,'f',1));
            break;
        case SIDE_DOWN:
            getLabel(speedLists[SPEED_SIDE_DOWN])->setText(QString::number(t.second,'f',1));
            break;
        default:
            break;
        }
    }
}

void Dialog::onSetVMotors(QList<pair<MOTORS, float> > value)
{
    for(auto & t:value){
        //使用switch检查输入是否合法
        switch (t.first) {
        case LEFT_UP:
            getLabel(speedLists[SPEED_LEFT_UP])->setText(QString::number(t.second,'f',1));
            break;
        case LEFT_DOWN:
            getLabel(speedLists[SPEED_LEFT_DOWN])->setText(QString::number(t.second,'f',1));
            break;
        case RIGHT_UP:
            getLabel(speedLists[SPEED_RIGHT_UP])->setText(QString::number(t.second,'f',1));
            break;
        case RIGHT_DOWN:
            getLabel(speedLists[SPEED_RIGHT_DOWN])->setText(QString::number(t.second,'f',1));
            break;
        default:
            break;
        }
    }
}

void Dialog::onSetRelay(QList<pair<int, bool> > value)
{
    static QByteArray stored(NUMBER_OF_RELAYS,'0');
    for(auto & t:value){
        if(t.first<NUMBER_OF_RELAYS && t.first>=0){
            stored[t.first]=t.second;
        }
    }

    getLabel(relayLists[RELAY_VALUE])->setText(stored.mid(0,4)+' '+stored.mid(4,4)+' '+stored.mid(8,4));
}

void Dialog::onSetRev()
{
    bool ok;
    float tVal=newRev->text().toFloat(&ok);
    if(ok){
        emit setDeepRev(tVal);
    }
    else {
        qDebug() << "Invalid input '" << newRev->text() << "'.";
    }
}

void Dialog::onOpenAcosClicked()
{
    emit setRecordStatus(recordManager::ACOS_RECORD);
}

void Dialog::onCloseAcosClicked()
{
    emit setRecordStatus(0);
}

void Dialog::onOpenAcosRecord()
{
    acosRecord->setEnabled(false);
    disconnect(acosRecord,&QPushButton::clicked,this,&Dialog::onOpenAcosRecord);
    emit setRecordStatus(record_status|recordManager::ACOS_RECORD);
}

void Dialog::onCloseAcosRecord()
{
    acosRecord->setEnabled(false);
    disconnect(acosRecord,&QPushButton::clicked,this,&Dialog::onCloseAcosRecord);
    emit setRecordStatus(record_status&(~recordManager::ACOS_RECORD));
}

void Dialog::onOpenCtrRecord()
{
    ctrRecord->setEnabled(false);
    disconnect(ctrRecord,&QPushButton::clicked,this,&Dialog::onOpenCtrRecord);
    emit setRecordStatus(record_status|recordManager::CONTROLLER_RECORD);
}

void Dialog::onCloseCtrRecord()
{
    ctrRecord->setEnabled(false);
    disconnect(ctrRecord,&QPushButton::clicked,this,&Dialog::onCloseCtrRecord);
    emit setRecordStatus(record_status&(~recordManager::CONTROLLER_RECORD));
}

void Dialog::onOpenTransRecord()
{
    transRecord->setEnabled(false);
    disconnect(transRecord,&QPushButton::clicked,this,&Dialog::onOpenTransRecord);
    emit setRecordStatus(record_status|recordManager::TRANSCEIVER_RECORD);
}

void Dialog::onCloseTransRecord()
{
    transRecord->setEnabled(false);
    disconnect(transRecord,&QPushButton::clicked,this,&Dialog::onCloseTransRecord);
    emit setRecordStatus(record_status&(~recordManager::TRANSCEIVER_RECORD));
}

void Dialog::onOpenVisionRecord()
{
    visionRecord->setEnabled(false);
    disconnect(visionRecord,&QPushButton::clicked,this,&Dialog::onOpenVisionRecord);
    emit setRecordStatus(record_status|recordManager::VISION_RECORD);
}

void Dialog::onCloseVisionRecord()
{
    visionRecord->setEnabled(false);
    disconnect(visionRecord,&QPushButton::clicked,this,&Dialog::onCloseVisionRecord);
    emit setRecordStatus(record_status&(~recordManager::VISION_RECORD));
}
/*
void Dialog::onOpenPhotoRecord()
{
    photoRecord->setEnabled(false);
    disconnect(photoRecord,&QPushButton::clicked,this,&Dialog::onOpenPhotoRecord);
    emit setRecordStatus(record_status|recordManager::PHOTO_RECORD_B);
    emit setRecordStatus(record_status|recordManager::PHOTO_RECORD_F);
}
*/
void Dialog::onOpenPhotoFRecord()
{
    photoFRecord->setEnabled(false);
    disconnect(photoFRecord,&QPushButton::clicked,this,&Dialog::onOpenPhotoFRecord);
    emit setRecordStatus(record_status|recordManager::PHOTO_RECORD_F);
}
void Dialog::onOpenPhotoBRecord()
{
    photoBRecord->setEnabled(false);
    disconnect(photoBRecord,&QPushButton::clicked,this,&Dialog::onOpenPhotoBRecord);
    emit setRecordStatus(record_status|recordManager::PHOTO_RECORD_B);
}

void Dialog::onClosePhotoFRecord()
{
    photoFRecord->setEnabled(false);
    disconnect(photoFRecord,&QPushButton::clicked,this,&Dialog::onClosePhotoFRecord);
    emit setRecordStatus(record_status&(~recordManager::PHOTO_RECORD_F));
}
void Dialog::onClosePhotoBRecord()
{
    photoBRecord->setEnabled(false);
    disconnect(photoBRecord,&QPushButton::clicked,this,&Dialog::onClosePhotoBRecord);
    emit setRecordStatus(record_status&(~recordManager::PHOTO_RECORD_B));
}

void Dialog::onAllCloseRecord()
{
    disconnect(acosRecord,&QPushButton::clicked,this,&Dialog::onCloseAcosRecord);
    disconnect(ctrRecord,&QPushButton::clicked,this,&Dialog::onCloseCtrRecord);
    disconnect(transRecord,&QPushButton::clicked,this,&Dialog::onCloseTransRecord);
    disconnect(visionRecord,&QPushButton::clicked,this,&Dialog::onCloseVisionRecord);
    //disconnect(photoRecord,&QPushButton::clicked,this,&Dialog::onClosePhotoRecord);
    disconnect(photoFRecord,&QPushButton::clicked,this,&Dialog::onClosePhotoFRecord);
    disconnect(photoBRecord,&QPushButton::clicked,this,&Dialog::onClosePhotoBRecord);
    acosRecord->setEnabled(false);
    ctrRecord->setEnabled(false);
    transRecord->setEnabled(false);
    visionRecord->setEnabled(false);
    //photoRecord->setEnabled(false);
    photoFRecord->setEnabled(false);
    photoBRecord->setEnabled(false);
    allCloseRecord->setEnabled(false);
    allOpenRecord->setEnabled(false);
    emit setRecordStatus(0);
}

void Dialog::onAllOpenRecord()
{
    disconnect(acosRecord,&QPushButton::clicked,this,&Dialog::onOpenAcosRecord);
    disconnect(ctrRecord,&QPushButton::clicked,this,&Dialog::onOpenCtrRecord);
    disconnect(transRecord,&QPushButton::clicked,this,&Dialog::onOpenTransRecord);
    disconnect(visionRecord,&QPushButton::clicked,this,&Dialog::onOpenVisionRecord);
    //disconnect(photoRecord,&QPushButton::clicked,this,&Dialog::onOpenPhotoRecord);
    disconnect(photoFRecord,&QPushButton::clicked,this,&Dialog::onOpenPhotoFRecord);
    disconnect(photoBRecord,&QPushButton::clicked,this,&Dialog::onOpenPhotoBRecord);

    acosRecord->setEnabled(false);
    ctrRecord->setEnabled(false);
    transRecord->setEnabled(false);
    visionRecord->setEnabled(false);
    //photoRecord->setEnabled(false);
    photoFRecord->setEnabled(false);
    photoBRecord->setEnabled(false);
    allCloseRecord->setEnabled(false);
    allOpenRecord->setEnabled(false);
    emit setRecordStatus(recordManager::ACOS_RECORD|recordManager::CONTROLLER_RECORD|
                         recordManager::TRANSCEIVER_RECORD|recordManager::VISION_RECORD|
                         recordManager::PHOTO_RECORD_B|recordManager::PHOTO_RECORD_F);
}

void Dialog::onSetRecordStatus(int sw)
{
    record_status=sw;
    if(recordManager::ACOS_RECORD&record_status){
        acosRecord->setText(QString::fromWCharArray(L"水声数据\n当前打开"));
        connect(acosRecord,&QPushButton::clicked,this,&Dialog::onCloseAcosRecord);
    }
    else{
        acosRecord->setText(QString::fromWCharArray(L"水声数据\n当前关闭"));
        connect(acosRecord,&QPushButton::clicked,this,&Dialog::onOpenAcosRecord);
    }
    if(recordManager::CONTROLLER_RECORD&record_status){
        ctrRecord->setText(QString::fromWCharArray(L"控制输出\n当前打开"));
        connect(ctrRecord,&QPushButton::clicked,this,&Dialog::onCloseCtrRecord);
    }
    else{
        ctrRecord->setText(QString::fromWCharArray(L"控制输出\n当前关闭"));
        connect(ctrRecord,&QPushButton::clicked,this,&Dialog::onOpenCtrRecord);
    }

    if(recordManager::TRANSCEIVER_RECORD&record_status){
        transRecord->setText(QString::fromWCharArray(L"外部输入\n当前打开"));
        connect(transRecord,&QPushButton::clicked,this,&Dialog::onCloseTransRecord);
    }
    else{
        transRecord->setText(QString::fromWCharArray(L"外部输入\n当前关闭"));
        connect(transRecord,&QPushButton::clicked,this,&Dialog::onOpenTransRecord);
    }

    if(recordManager::VISION_RECORD&record_status){
        visionRecord->setText(QString::fromWCharArray(L"图像信息\n当前打开"));
        connect(visionRecord,&QPushButton::clicked,this,&Dialog::onCloseVisionRecord);
    }
    else{
        visionRecord->setText(QString::fromWCharArray(L"图像信息\n当前关闭"));
        connect(visionRecord,&QPushButton::clicked,this,&Dialog::onOpenVisionRecord);
    }

    if(recordManager::PHOTO_RECORD_B&record_status){
        photoBRecord->setText(QString::fromWCharArray(L"下视图像\n当前打开"));
        connect(photoBRecord,&QPushButton::clicked,this,&Dialog::onClosePhotoBRecord);
    }
    else{
        photoBRecord->setText(QString::fromWCharArray(L"下视图像\n当前关闭"));
        connect(photoBRecord,&QPushButton::clicked,this,&Dialog::onOpenPhotoBRecord);
    }
    if(recordManager::PHOTO_RECORD_F&record_status){
        photoFRecord->setText(QString::fromWCharArray(L"前视图像\n当前打开"));
        connect(photoFRecord,&QPushButton::clicked,this,&Dialog::onClosePhotoFRecord);
    }
    else{
        photoFRecord->setText(QString::fromWCharArray(L"前视图像\n当前关闭"));
        connect(photoFRecord,&QPushButton::clicked,this,&Dialog::onOpenPhotoFRecord);
    }


    acosRecord->setEnabled(true);
    ctrRecord->setEnabled(true);
    transRecord->setEnabled(true);
    visionRecord->setEnabled(true);
    //photoRecord->setEnabled(true);
    photoFRecord->setEnabled(true);
    photoBRecord->setEnabled(true);
    allCloseRecord->setEnabled(true);
    allOpenRecord->setEnabled(true);
}

void Dialog::onAcosConnected(QHostAddress addr, bool ok)
{
    getLabel(connectionLists[CONNECTION_ACOS_TARGET])->setText(addr.toString());
    if(!ok){
        getLabel(connectionLists[CONNECTION_ACOS_STATUS])->setText(QString::fromWCharArray(L"未连接"));
    }
    else{
        getLabel(connectionLists[CONNECTION_ACOS_STATUS])->setText(QString::fromWCharArray(L"已连接"));
    }
}

void Dialog::onImageShow(Mat *mat, const char *name)
{
    cv::imshow(name,*mat);

}

void Dialog::onCreateWindow(const char *name)
{
    cv::namedWindow(name);
}

void Dialog::onDestroyWindow()
{
    cv::destroyAllWindows();
}

void Dialog::createBar(const char * barName, const char *winName, int * val, int count)
{
    cv::createTrackbar(barName,winName,val,count);
}


