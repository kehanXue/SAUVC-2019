
#include "remotedialog.h"
#include "stdio.h"
#include <QDebug>

remoteDialog::remoteDialog(QWidget *parent)
    : QDialog(parent),
      //由于使用了整型到枚举的强制转换，所以需要保证此处顺序与global.h一致
      motorNameStr({
          QString::fromWCharArray(L"左前"),
          QString::fromWCharArray(L"左后"),
          QString::fromWCharArray(L"右前"),
          QString::fromWCharArray(L"右后"),
          QString::fromWCharArray(L"主左"),
          QString::fromWCharArray(L"主右"),
          QString::fromWCharArray(L"侧前"),
          QString::fromWCharArray(L"侧后")
      }),
      motorNameGroup(NUMBER_OF_MOTORS,nullptr),
      motorNameLayout(NUMBER_OF_MOTORS,nullptr),
      motorCtr(NUMBER_OF_MOTORS,nullptr),
      motorRadio(NUMBER_OF_MOTORS,vector<QRadioButton *>(3,nullptr)),
      combineStr({
          QString::fromWCharArray(L"前进"),
          QString::fromWCharArray(L"后退"),
          QString::fromWCharArray(L"上升"),
          QString::fromWCharArray(L"下潜"),
          QString::fromWCharArray(L"逆时针转"),
          QString::fromWCharArray(L"顺时针转"),
          QString::fromWCharArray(L"向左侧移"),
          QString::fromWCharArray(L"向右侧移")
      }),
      combineButton(NUMBER_OF_COMBINES,nullptr),
      combineFunc({
          &remoteDialog::onForward,
          &remoteDialog::onBackward,
          &remoteDialog::onMoveUp,
          &remoteDialog::onMoveDown,
          &remoteDialog::onClockwise,
          &remoteDialog::onAntiClockwise,
          &remoteDialog::onLeftward,
          &remoteDialog::onRightward
                  }),
      relayNameStr({
          QString::fromWCharArray(L"DSP"),
          QString::fromWCharArray(L"数采"),
          QString::fromWCharArray(L"继电器3"),
          QString::fromWCharArray(L"继电器4"),
          QString::fromWCharArray(L"继电器5"),
          QString::fromWCharArray(L"继电器6"),
          QString::fromWCharArray(L"继电器7"),
          QString::fromWCharArray(L"继电器8"),
          QString::fromWCharArray(L"继电器9"),
          QString::fromWCharArray(L"继电器10"),
          QString::fromWCharArray(L"继电器11"),
          QString::fromWCharArray(L"继电器12")
      }),
      TransDofStr({
          QString::fromWCharArray(L"FORBACK"),
          QString::fromWCharArray(L"UPDOWN"),
          QString::fromWCharArray(L"LEFTRIGHT")
      }),
      relayCtr(NUMBER_OF_RELAYS,nullptr),
      relayNameGroup(NUMBER_OF_RELAYS,nullptr),
      relayNameLayout(NUMBER_OF_RELAYS,nullptr),
      relayRadio(NUMBER_OF_RELAYS,{nullptr,nullptr})
{
    motorGroup=new QGroupBox(QString::fromWCharArray(L"推进器"));
    motorLayout=new QGridLayout;

    for(int i=0;i<NUMBER_OF_MOTORS;i++){
        BOOST_ASSERT(motorNameStr.size() == NUMBER_OF_MOTORS);
        motorNameGroup[i]=new QGroupBox(motorNameStr[i]);
        motorCtr[i]=new QButtonGroup;
        motorNameLayout[i]=new QVBoxLayout;
        //需要与枚举保持一致
        motorRadio[i][0]=new QRadioButton(QString::fromWCharArray(L"正转"));
        motorRadio[i][1]=new QRadioButton(QString::fromWCharArray(L"反转"));
        motorRadio[i][2]=new QRadioButton(QString::fromWCharArray(L"关闭"));
        for(int j=0;j<3;j++){
            motorNameLayout[i]->addWidget(motorRadio[i][j]);
            motorCtr[i]->addButton(motorRadio[i][j],j);
        }
        motorCtr[i]->button(STOP)->setChecked(true);
        motorNameGroup[i]->setLayout(motorNameLayout[i]);
        motorLayout->addWidget(motorNameGroup[i],(i/4)*2+1,i%4+1,2,1);
        motorCtr[i]->setExclusive(true);

        switch(MOTORS(i)){
        case MAIN_LEFT:
        case MAIN_RIGHT:
        case SIDE_UP:
        case SIDE_DOWN:
            connect(motorCtr[i],static_cast<void(QButtonGroup::*)(QAbstractButton *)>(&QButtonGroup::buttonClicked),
                    this,&remoteDialog::onHMotorChanged);
            break;
        case LEFT_UP:
        case LEFT_DOWN:
        case RIGHT_UP:
        case RIGHT_DOWN:
            connect(motorCtr[i],static_cast<void(QButtonGroup::*)(QAbstractButton *)>(&QButtonGroup::buttonClicked),
                    this,&remoteDialog::onVMotorChanged);
            break;
        default:
            break;
        }
    }


    for(int i=0;i<NUMBER_OF_COMBINES;i++){
        BOOST_ASSERT(combineStr.size() == NUMBER_OF_COMBINES);
        combineButton[i]=new QPushButton(combineStr[i]);
        connect(combineButton[i],&QPushButton::clicked,this,combineFunc[i]);
        motorLayout->addWidget(combineButton[i],i/2+1,i%2+1+4,1,1);
    }

    TransDofSelect=new QComboBox;
    for(int i=0;i<NUMBER_OF_TRANDOF;i++)
    {
        TransDofSelect->addItem(TransDofStr[i]);
    }
    motorUp=new QPushButton(QString::fromWCharArray(L"+"));
    connect(motorUp,&QPushButton::clicked,this,&remoteDialog::onIncrease);
    motorDown=new QPushButton(QString::fromWCharArray(L"-"));
    connect(motorDown,&QPushButton::clicked,this,&remoteDialog::onDecrease);
    motorClose=new QPushButton(QString::fromWCharArray(L"全部关闭"));
    connect(motorClose,&QPushButton::clicked,this,&remoteDialog::onMotorClose);

    motorLayout->addWidget(TransDofSelect,1,7,1,1);
    motorLayout->addWidget(motorUp,2,7,1,1);
    motorLayout->addWidget(motorDown,3,7,1,1);
    motorLayout->addWidget(motorClose,4,7,1,1);

    motorGroup->setLayout(motorLayout);

    sevoLayout=new QGridLayout;
    sevoGroup=new QGroupBox(QString::fromWCharArray(L"舵机"));
    sevoOpen=new QPushButton(QString::fromWCharArray(L"开启"));
    sevoClose=new QPushButton(QString::fromWCharArray(L"关闭"));
    sevoPulseButton=new QPushButton(QString::fromWCharArray(L"改变脉冲"));
    sevoPulse=new QLineEdit;
    sevoLayout->addWidget(sevoOpen,1,1,1,1);
    sevoLayout->addWidget(sevoClose,2,1,1,1);
    sevoLayout->addWidget(sevoPulse,3,1,1,1);
    sevoLayout->addWidget(sevoPulseButton,4,1,1,1);
    connect(sevoOpen,&QPushButton::clicked,this,&remoteDialog::onOpenSevo);
    connect(sevoClose,&QPushButton::clicked,this,&remoteDialog::onCloseSevo);
    connect(sevoPulseButton,&QPushButton::clicked,this,&remoteDialog::onChangeSevoPulse);
    sevoGroup->setLayout(sevoLayout);

    stepLayout=new QGridLayout;
    stepGroup=new QGroupBox(QString::fromWCharArray(L"步进电机"));
    stepOpen=new QPushButton(QString::fromWCharArray(L"开启"));
    stepClose=new QPushButton(QString::fromWCharArray(L"关闭"));
    stepHandControlButton=new QPushButton(QString::fromWCharArray(L"允许手动"));
    stepTime=new QLineEdit;
    stepLayout->addWidget(stepOpen,1,1,1,1);
    stepLayout->addWidget(stepClose,2,1,1,1);
    stepLayout->addWidget(stepTime,3,1,1,1);
    stepLayout->addWidget(stepHandControlButton,4,1,1,1);
    connect(stepOpen,&QPushButton::clicked,this,&remoteDialog::onOpenStep);
    connect(stepClose,&QPushButton::clicked,this,&remoteDialog::onCloseStep);
    connect(stepHandControlButton,&QPushButton::clicked,this,&remoteDialog::onChangeHandControlStep);
    stepGroup->setLayout(stepLayout);

    DeepLimitLayout=new QGridLayout;
    DeepLimitGroup=new QGroupBox(QString::fromWCharArray(L"自动定深限幅"));
    DeepCtrDown=new QLineEdit;
    DeepCtrUp=new QLineEdit;
    DeepIntDown=new QLineEdit;
    DeepIntUp=new QLineEdit;
    ChangeDeepLimit=new QPushButton(QString::fromWCharArray(L"更改配置"));
    DefaultDeepLimit=new QPushButton(QString::fromWCharArray(L"默认配置"));
    DeepIntUpLabel=new QLabel(QString::fromWCharArray(L"积分饱和上限"));
    DeepIntDownLabel=new QLabel(QString::fromWCharArray(L"积分饱和下限"));
    DeepCtrUpLabel=new QLabel(QString::fromWCharArray(L"PID控制上限"));
    DeepCtrDownLabel=new QLabel(QString::fromWCharArray(L"PID控制下限"));
    DeepLimitLayout->addWidget(DeepIntUpLabel,1,1,1,1);
    DeepLimitLayout->addWidget(DeepIntUp,1,3,1,1);
    DeepLimitLayout->addWidget(DeepIntDownLabel,2,1,1,1);
    DeepLimitLayout->addWidget(DeepIntDown,2,3,1,1);
    DeepLimitLayout->addWidget(DeepCtrUpLabel,3,1,1,1);
    DeepLimitLayout->addWidget(DeepCtrUp,3,3,1,1);
    DeepLimitLayout->addWidget(DeepCtrDownLabel,4,1,1,1);
    DeepLimitLayout->addWidget(DeepCtrDown,4,3,1,1);
    DeepLimitLayout->addWidget(ChangeDeepLimit,5,1,1,1);
    DeepLimitLayout->addWidget(DefaultDeepLimit,5,2,1,1);
    DeepLimitGroup->setLayout(DeepLimitLayout);

    connect(ChangeDeepLimit,&QPushButton::clicked,this,&remoteDialog::onChangeDeepLimit);
    connect(DefaultDeepLimit,&QPushButton::clicked,this,&remoteDialog::onDefaultDeepLimit);


    relayGroup=new QGroupBox(QString::fromWCharArray(L"继电器"));
    relayLayout=new QGridLayout;

    for(int i=0;i<NUMBER_OF_RELAYS;i++){
        BOOST_ASSERT(relayNameStr.size() == NUMBER_OF_RELAYS);
        relayNameGroup[i]=new QGroupBox(relayNameStr[i]);
        relayCtr[i]=new QButtonGroup;
        relayNameLayout[i]=new QVBoxLayout;
        //需要与枚举保持一致
        relayRadio[i].first=new QRadioButton(QString::fromWCharArray(L"连通"));
        relayRadio[i].second=new QRadioButton(QString::fromWCharArray(L"断开"));
        relayNameLayout[i]->addWidget(relayRadio[i].first);
        relayCtr[i]->addButton(relayRadio[i].first,0);
        relayNameLayout[i]->addWidget(relayRadio[i].second);
        relayCtr[i]->addButton(relayRadio[i].second,1);
        relayNameGroup[i]->setLayout(relayNameLayout[i]);
        relayLayout->addWidget(relayNameGroup[i],(i/6)*2+1,i%6+1,2,1);

        relayCtr[i]->setExclusive(true);
        relayCtr[i]->button(DISCONNECTED)->setChecked(true);
        connect(relayCtr[i],static_cast<void(QButtonGroup::*)(QAbstractButton *)>(&QButtonGroup::buttonClicked),
                this,&remoteDialog::onRelayChanged);
    }
    resetAcos=new QPushButton(QString::fromWCharArray(L"水声重启"));
    resetStm=new QPushButton(QString::fromWCharArray(L"32复位"));
    allOpen=new QPushButton(QString::fromWCharArray(L"全部连通"));
    allClose=new QPushButton(QString::fromWCharArray(L"全部断开"));
    connect(allOpen,&QPushButton::clicked,this,&remoteDialog::onRelayOn);
    connect(allClose,&QPushButton::clicked,this,&remoteDialog::onRelayoff);
    relayLayout->addWidget(resetAcos,1,7,1,1);
    relayLayout->addWidget(resetStm,2,7,1,1);
    relayLayout->addWidget(allOpen,3,7,1,1);
    relayLayout->addWidget(allClose,4,7,1,1);
    relayGroup->setLayout(relayLayout);

    mainLayout=new QGridLayout;
    mainLayout->addWidget(motorGroup,1,1,4,7);
    mainLayout->addWidget(relayGroup,5,1,4,7);
    mainLayout->addWidget(sevoGroup,1,8,2,2);
    mainLayout->addWidget(stepGroup,1,10,2,2);
    mainLayout->addWidget(DeepLimitGroup,3,8,3,4);

    setLayout(mainLayout);

}

void remoteDialog::setCtr(controller *pCtr)
{
    ctr=pCtr;
    connect(this,&remoteDialog::setHMotors,ctr,&controller::onSetHMotors);
    connect(this,&remoteDialog::setVMotors,ctr,&controller::onSetVMotors);
    connect(this,&remoteDialog::setRelay,ctr,&controller::onSetRelay);
}

void remoteDialog::setTrans(transceiver *pTrans)
{
    trans=pTrans;
    connect(this,&remoteDialog::setSevo,trans,&transceiver::setSevo);
    connect(this,&remoteDialog::setStep,trans,&transceiver::setStepMotor);
    connect(this,&remoteDialog::setDeepPIDRev,trans,&transceiver::setDeepPIDRev);
}

void remoteDialog::onHMotorChanged()
{
    QList<pair<MOTORS,float>> tmp;

    for(int i=0;i<4;i++){
       tmp.push_back(make_pair<>(hList[i],0));
       motorValue[hList[i]]=0;
    }
    emit setHMotors(tmp);
}

void remoteDialog::onVMotorChanged()
{
    const static MOTORS vList[4]={LEFT_UP,LEFT_DOWN,RIGHT_UP,RIGHT_DOWN};
    QList<pair<MOTORS,float>> tmp;

    for(int i=0;i<4;i++){
       tmp.push_back(make_pair<>(vList[i],0));
       motorValue[vList[i]]=0;
    }
    emit setVMotors(tmp);
}

void remoteDialog::onRelayChanged()
{
    QList<pair<int,bool>> tmp;
    for(int i=0;i<NUMBER_OF_RELAYS;i++){
        switch(STATE(relayCtr[i]->checkedId())){
        case CONNECTED:
            tmp.push_back(make_pair<>(i,true));
            break;
        case DISCONNECTED:
            tmp.push_back(make_pair<>(i,false));
            break;
        }
    }

    emit setRelay(tmp);
}

int remoteDialog::getTransDof(QString& Str)
{
    if(Str == "FORBACK")return 1;
    else if(Str == "UPDOWN")return 2;
    else if(Str == "LEFTRIGHT")return 3;
    else
    {
        qDebug()<<"Invalid Dof String";
        return 0;
    }
}


void remoteDialog::onIncrease()
{
    //垂推
    bool deepctr;
    QList<pair<MOTORS,float>> tmp;
    QString currentDofStr = TransDofSelect->currentText();
    int && dof = this->getTransDof(currentDofStr);
    switch(dof)
    {
       case UPDOWN:
    {
    for(int i=0;i<4;i++){
        switch(FORREV(motorCtr[vList[i]]->checkedId())){
             case FORWARD:
             {
                 motorValue[vList[i]]+=STEP;
                 if(motorValue[vList[i]]>=SPEED_LIMIT){
                     motorValue[vList[i]]=SPEED_LIMIT;
                     qDebug() << "The motor with id " << vList[i] << "has reached top speed.";
                 }
                 break;
             }
             case REVERSE:
             {
                 motorValue[vList[i]]-=STEP;
                 if(motorValue[vList[i]]<=-SPEED_LIMIT){
                     motorValue[vList[i]]=-SPEED_LIMIT;
                     qDebug() << "The motor with id " << vList[i] << "has reached top speed.";
                 }
                 break;
             }
             case STOP:
             {
                 motorValue[vList[i]]=0;
                 break;
             }
             default:
                 break;
        }
        tmp.push_back(make_pair<>(vList[i],motorValue[vList[i]]));

    }
    deepctr=ctr->getDeepCtr();
    if(deepctr==0){
        emit setVMotors(tmp);
    }
    break;
    }

    //主推侧推
    case FORBACK:
    {
    tmp.clear();
    for(int i=0;i<2;i++){
        switch(FORREV(motorCtr[xList[i]]->checkedId())){
        case FORWARD:
        {
            motorValue[xList[i]]+=STEP;
            if(motorValue[xList[i]]>=SPEED_LIMIT){
                motorValue[xList[i]]=SPEED_LIMIT;
                qDebug() << "The motor with id " << xList[i] << "has reached top speed.";
            }
            break;
        }
        case REVERSE:
        {
            motorValue[xList[i]]-=STEP;
            if(motorValue[xList[i]]<=-SPEED_LIMIT){
                motorValue[xList[i]]=-SPEED_LIMIT;
                qDebug() << "The motor with id " << xList[i] << "has reached top speed.";
            }
            break;
        }
        case STOP:
        {
            motorValue[xList[i]]=0;
            break;
        }
        default:
            break;
        }
        tmp.push_back(make_pair<>(xList[i],motorValue[xList[i]]));
    }
    emit setHMotors(tmp);
    break;
    }
    case LEFTRIGHT:
    {
    tmp.clear();
    for(int i=0;i<2;i++){
        switch(FORREV(motorCtr[zList[i]]->checkedId())){
        case FORWARD:
        {
            motorValue[zList[i]]+=STEP;
            if(motorValue[zList[i]]>=SPEED_LIMIT){
                motorValue[zList[i]]=SPEED_LIMIT;
                qDebug() << "The motor with id " << zList[i] << "has reached top speed.";
            }
            break;
        }
        case REVERSE:
        {
            motorValue[zList[i]]-=STEP;
            if(motorValue[zList[i]]<=-SPEED_LIMIT){
                motorValue[zList[i]]=-SPEED_LIMIT;
                qDebug() << "The motor with id " << zList[i] << "has reached top speed.";
            }
            break;
        }
        case STOP:
        {
            motorValue[zList[i]]=0;
            break;
        }
        default:
            break;
        }
        tmp.push_back(make_pair<>(zList[i],motorValue[zList[i]]));
    }
    emit setHMotors(tmp);
    break;
}
}
}

void remoteDialog::onDecrease()
{
    //垂推
    bool deepctr;
    QList<pair<MOTORS,float>> tmp;
    QString currentDofStr = TransDofSelect->currentText();
    int && dof = this->getTransDof(currentDofStr);
    switch(dof)
    {
    case UPDOWN:
    {
    for(int i=0;i<4;i++){
        switch(FORREV(motorCtr[vList[i]]->checkedId())){
        case FORWARD:
        {
            motorValue[vList[i]]-=STEP;
            if(motorValue[vList[i]]<=0){
                motorValue[vList[i]]=0;
                qDebug() << "The motor with id " << vList[i] << "has reached zero.";
            }
            break;
        }
        case REVERSE:
        {
            motorValue[vList[i]]+=STEP;
            if(motorValue[vList[i]]>=0){
                motorValue[vList[i]]=0;
                qDebug() << "The motor with id " << vList[i] << "has reached zero.";
            }
            break;
        }
        case STOP:
        {
            motorValue[vList[i]]=0;
            break;
        }
        default:
            break;
        }
        tmp.push_back(make_pair<>(vList[i],motorValue[vList[i]]));
    }
    deepctr=ctr->getDeepCtr();
    if(deepctr==0){
        emit setVMotors(tmp);
    }
    break;
    }
    //主推侧推
    case FORBACK:
    {
    tmp.clear();
    for(int i=0;i<2;i++){
        switch(FORREV(motorCtr[xList[i]]->checkedId())){
        case FORWARD:
        {
            motorValue[xList[i]]-=STEP;
            if(motorValue[xList[i]]<=0){
                motorValue[xList[i]]=0;
                qDebug() << "The motor with id " << xList[i] << "has reached zero.";
            }
            break;
        }
        case REVERSE:
        {
            motorValue[xList[i]]+=STEP;
            if(motorValue[xList[i]]>=0){
                motorValue[xList[i]]=0;
                qDebug() << "The motor with id " << xList[i] << "has reached zero.";
            }
            break;
        }
        case STOP:
        {
            motorValue[xList[i]]=0;
            break;
        }
        default:
            break;
        }
        tmp.push_back(make_pair<>(xList[i],motorValue[xList[i]]));
    }
    emit setHMotors(tmp);
    break;
    }
    case LEFTRIGHT:
    {
    tmp.clear();
    for(int i=0;i<2;i++){
        switch(FORREV(motorCtr[zList[i]]->checkedId())){
        case FORWARD:
        {
            motorValue[zList[i]]-=STEP;
            if(motorValue[zList[i]]<=0){
                motorValue[zList[i]]=0;
                qDebug() << "The motor with id " << zList[i] << "has reached zero.";
            }
            break;
        }
        case REVERSE:
        {
            motorValue[zList[i]]+=STEP;
            if(motorValue[zList[i]]>=0){
                motorValue[zList[i]]=0;
                qDebug() << "The motor with id " << zList[i] << "has reached zero.";
            }
            break;
        }
        case STOP:
        {
            motorValue[zList[i]]=0;
            break;
        }
        default:
            break;
        }
        tmp.push_back(make_pair<>(zList[i],motorValue[zList[i]]));
    }
    emit setHMotors(tmp);
    break;
    }
}
}

void remoteDialog::onMotorClose()
{
    bool deepctr;
    for(int i=0;i<NUMBER_OF_MOTORS;i++){
        motorCtr[i]->button(STOP)->setChecked(true);
    }
    onHMotorChanged();
    deepctr=ctr->getDeepCtr();
    if(deepctr==0){
        onVMotorChanged();
    }
}

void remoteDialog::onForward()
{
    motorCtr[MAIN_LEFT]->button(FORWARD)->setChecked(true);
    motorCtr[MAIN_RIGHT]->button(FORWARD)->setChecked(true);
    onHMotorChanged();
}

void remoteDialog::onBackward()
{
    motorCtr[MAIN_LEFT]->button(REVERSE)->setChecked(true);
    motorCtr[MAIN_RIGHT]->button(REVERSE)->setChecked(true);
    onHMotorChanged();
}

void remoteDialog::onMoveUp()
{
    for(int i=0;i<4;i++){
        motorCtr[vList[i]]->button(REVERSE)->setChecked(true);
    }
    onVMotorChanged();
}

void remoteDialog::onMoveDown()
{
    for(int i=0;i<4;i++){
        motorCtr[vList[i]]->button(FORWARD)->setChecked(true);
    }
    onVMotorChanged();
}

void remoteDialog::onClockwise()
{
    motorCtr[SIDE_UP]->button(FORWARD)->setChecked(true);
    motorCtr[SIDE_DOWN]->button(REVERSE)->setChecked(true);
    onHMotorChanged();
}

void remoteDialog::onAntiClockwise()
{
    motorCtr[SIDE_UP]->button(REVERSE)->setChecked(true);
    motorCtr[SIDE_DOWN]->button(FORWARD)->setChecked(true);
    onHMotorChanged();
}

void remoteDialog::onLeftward()
{
    motorCtr[SIDE_UP]->button(FORWARD)->setChecked(true);
    motorCtr[SIDE_DOWN]->button(FORWARD)->setChecked(true);
    onHMotorChanged();
}

void remoteDialog::onRightward()
{
    motorCtr[SIDE_UP]->button(REVERSE)->setChecked(true);
    motorCtr[SIDE_DOWN]->button(REVERSE)->setChecked(true);
    onHMotorChanged();
}

void remoteDialog::onRelayOn()
{
    for(int i=0;i<NUMBER_OF_RELAYS;i++){
        relayCtr[i]->button(CONNECTED)->setChecked(true);
    }
    onRelayChanged();
}

void remoteDialog::onRelayoff()
{
    for(int i=0;i<NUMBER_OF_RELAYS;i++){
        relayCtr[i]->button(DISCONNECTED)->setChecked(true);
    }
    onRelayChanged();
}

void remoteDialog::onOpenSevo()
{
    int pulse;
    if(!sevoPulse->text().isEmpty())pulse = sevoPulse->text().toInt();
    else
    {
        qDebug()<<"No pulse number";
        return;
    }
    emit setSevo(true,pulse);
}

void remoteDialog::onCloseSevo()
{
    emit setSevo(false);
}

void remoteDialog::onChangeSevoPulse()
{
    int pulse;
    if(!sevoPulse->text().isEmpty())pulse = sevoPulse->text().toInt();
    else
    {
        qDebug()<<"No pulse number";
        return;
    }
    emit setSevo(true,pulse);
}

void remoteDialog::onOpenStep()
{
    if(!stepTime->text().isEmpty())
    {
       int ms = stepTime->text().toInt();
       step_ok = true;
       if(ms>=0)emit setStep(true,ms,0);
       else emit setStep(true,abs(ms),0,1);
    }
    else
    {
        qDebug()<<"No valid times";
        return;
    }
}
void remoteDialog::onCloseStep()
{
    emit setStep(false);
    step_ok = false;
}
void remoteDialog::onChangeHandControlStep()
{
    if(!hand_status)
    {
        hand_status = true;
        emit setStep(true,0,true);
        stepHandControlButton->setText("禁止手动");
    }
    else
    {
        hand_status = false;
        emit setStep(true,0,false);
        stepHandControlButton->setText("允许手动");
    }
}
void remoteDialog::onChangeDeepLimit()
{
    int dIntUp,dIntDown,dCtrUp,dCtrDown;
    if(!DeepIntUp->text().isEmpty()   &&
       !DeepIntDown->text().isEmpty() &&
       !DeepCtrUp->text().isEmpty()   &&
       !DeepCtrDown->text().isEmpty())
    {
        qDebug()<<"Set Limiter Successed";
        dIntUp = DeepIntUp->text().toInt();
        dIntDown = DeepIntDown->text().toInt();
        dCtrUp = DeepCtrUp->text().toInt();
        dCtrDown = DeepCtrDown->text().toInt();
        emit setDeepPIDRev(dIntUp,dIntDown,dCtrUp,dCtrDown);
    }
    else qDebug()<<"Invalid Limiter";
}
void remoteDialog::onDefaultDeepLimit()
{
    int dIntUp,dIntDown,dCtrUp,dCtrDown;
    dIntUp = DEFAULT_DINT_UP;
    dIntDown = DEFAULT_DINT_DOWN;
    dCtrUp = DEFAULT_DCTR_UP;
    dCtrDown = DEFAULT_DCTR_DOWN;
    DeepIntUp->setText(QString::number(dIntUp,10));
    DeepIntDown->setText(QString::number(dIntDown,10));
    DeepCtrUp->setText(QString::number(dCtrUp,10));
    DeepCtrDown->setText(QString::number(dCtrDown,10));
}

void remoteDialog::closeEvent(QCloseEvent *event)
{
    this->hide();
}
