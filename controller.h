#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QList>
#include <QTimer>

#include <vector>
#include <unordered_map>
#include <queue>
#include <stack>
#include <algorithm>
#include <cstring>
#include <ctime>
#include <sys/time.h>
#include <utility>

#include <boost/shared_ptr.hpp>

#include "global.h"
#include "configmanager.h"
#include "visionclass.h"

using std::vector;
using std::unordered_map;
using std::queue;
using std::stack;
using std::min;
using std::pair;

#define     m1_left_center 312
#define     m1_right_center 312

using boost::shared_ptr;
class controller : public QObject
{
    Q_OBJECT

    typedef void (controller::*pMemFunc)();


    struct taskConfig{
        pMemFunc init;              //任务起始函数
        pMemFunc frame;             //控制帧函数
        pMemFunc end;               //任务结束函数
        TASKS_ID id;                //任务ID
    };

    struct actionConfig{
        pMemFunc init;              //任务起始函数
        pMemFunc frame;             //控制帧函数
        pMemFunc end;               //任务结束函数
        ACTION_ID id;               //任务ID
    };

    typedef shared_ptr<const taskConfig> pTaskCfg;
    typedef shared_ptr<const actionConfig> pActionCfg;

    struct PIDValue{
        float p;
        float i;
        float d;
    };



    enum ActionRePlayFlag{
        NO = 0,
        REPLAY = 1,
        MINUS = 2
    };

    struct actionPara{
        ACTION_ID actionName;
        unsigned int PlanCount;
        unsigned int PlanAngle;
        ActionRePlayFlag ReplayFlag;
    };


    stack<actionPara> ActionParaStack;

    struct taskStatus{
        pTaskCfg currentTask;       //当前任务
        pActionCfg curAction;       //当前Action
        unsigned int ms;            //帧间隔
        vector<int> cnt;            //各任务自行分配使用的计数器
        QList<ACTION_ID> ActionList;//主任务下的ACTION列表
        actionPara curPara;            //action参数
        ACTION_ID ActionFlag;       //ACTION标志位
        ACTION_ID ReplayActionFlag; //ACTION重复位
        ACTION_ID finished;         //ACTION完成标志位
        PIDValue val[NUMBER_OF_MOTORS];
    };

    
    struct sbgValue{
        float goal;
        float roll;
        float pitch;
        float yaw;
        float magNorm;
        float rollLast;
        float pitchLast;
        float yawLast;
        time_t tNow;
        time_t tLast;
        float yawErrorLast;
    };

    struct Taskdif{
        float direction;
        bool list;
        int cnt;
    };

    struct acosValue{
        float goal;
        float theta1;
        float phi1;
        float theta2;
        float phi2;
        float theta1Last;
        float phi1Last;
        float theta2Last;
        float phi2Last;
        float theta1ErrorLast;
        float theta2ErrorLast;
        time_t tNow;
        time_t tLast;
    };

    struct relayValue{
        bool value[NUMBER_OF_RELAYS];
    };

    struct speedValue{
        float value[NUMBER_OF_MOTORS];
    };

    struct deepValue{
        float value;
        float deepRev;
        float deepErrorLast;
        bool ctr;
        time_t tNow;
        time_t tLast;
    };

    struct imageValue{
        //float angle_goal;
        //float bin_angle_goal;
        //float path_dx;std::ofstream outfile_yaw
        //float path_dy;
        float ball_dx;
        float ball_dy;
        float ball_dy_diff;
        float ball_dx_diff;
        float ball_dx_last;
        float ball_dy_last;
        float drum3_dx_last;
        float drum3_dx;
        float drum3_dy_last;
        float drum3_dy;
        float drum3_dx_diff;
        float drum3_dy_diff;
        float angle;
        float flare_dx_last;
        float flare_dx;
        float flare_dx_diff;
        float drum_dx_last;
        float drum_dx;
        float drum_dy_last;
        float drum_dy;
        float drum_dx_diff;
        float drum_dy_diff;
        bool  flareStarted;
        bool  flareImgFlag;
        float gate_dx;
        float gate_dx_last;
        float gate_dx_diff;
        /*
        float bin_dx;
        float bin_dy;
        float bin_angle;
        float path_angle;
        float path_dx_last;
        float path_dy_last;
        float bin_dx_last;
        float bin_dy_last;
        float path_angle_last;
        float bin_angle_last;
        float path_angle_Errorlast;
        float bin_angle_Errorlast;
        */
        time_t t_now;
        time_t t_last;
    };

    unordered_map<TASKS_ID,pTaskCfg,std::hash<int>> taskList;   //所有任务的列表
    unordered_map<ACTION_ID,pActionCfg,std::hash<int>> actionList;
    queue<pTaskCfg> currentTaskList;                            //当前执行的任务列表
    queue<pActionCfg> currentActionList;                        //准备执行的Action列表
    taskStatus status;
    //SubtaskStatus Substatus;
    //stack<SubtaskStatus> SubstatusStack;
    stack<taskStatus> statusStack,ReplaystatusStack;
    QTimer * mainTimer;
    //QTimer * actionTimer;
    time_t t_now;
    time_t t_start;
    QTimer * timerForTime;

    configManager * config;
    visionClass * vision;

    unsigned int tried;
    bool isForwardInitial;
    bool isLeftBarDetect;
    bool isRightBarDetect;
    bool first_Turned;
    bool step_flag=0;

    float Gate_Straight;

    sbgValue sbg;
    acosValue acos;
    deepValue deep;
    relayValue relay;
    speedValue speed;
    imageValue img;
    Taskdif task;

public:

    explicit controller(QObject *parent = 0);

    void setConfig(configManager * cfg);
    void setVision(visionClass * vis);
    void setTaskList(const QList<TASKS_ID> & idList);
    void setActionList(const QList<ACTION_ID>&);
    void excecute();
    void stop();

    void createTimer();

    void onAcosRecv(float theta1, float phi1, float theta2, float phi2);
    void onSbgRecv(float roll, float pitch, float yaw, float magNorm);
    void onDeepRecv(float value);
    void onSpeedRecv(float lu,float ld,float ru,float rd);
    void onStepRecv(bool ok);

    void onSetDeepRev(float rev);

    void onSetHMotors(QList<pair<MOTORS,float>> value);
    void onSetVMotors(QList<pair<MOTORS,float>> value);
    void onSetRelay(QList<pair<int,bool>> value);

    void setDeepCtr(bool dc) { deep.ctr=dc; }
    bool getDeepCtr() { return deep.ctr; }
    time_t getTime() { return t_now; }

private:

    std::ofstream outfile_yaw;
    const double forward_deep = 1.0;

    void appendConfig(CONFIG_ID id);
    void clearConfig();
    void loadConfig(CONFIG_ID id);
    void updateConfig();
    void setZero();
    void setZero1();
    void onNextTask();
    void onEndTask();
    void ctrFrame();
    void onSwitchAction(ACTION_ID);     //直接转换，不将当前状态入栈
    void onEnterAction(ACTION_ID);      //当前状态入栈后转换到新阶段
    void onReturnAction(ACTION_ID);             //当前状态出栈，返回原来状态；如果栈为空，认为此函数等同于调用endTask()
    void onstopAction();
    void clean();

    void onGrabIt(bool toClose,unsigned int pulse);
    void onDropMark8(bool toDrop);
    void onDropMark9(bool toDrop);
    void onSetSevo(bool ok,int pulse);
    void onSetStep(bool ok,int ms);

    void setFrameInteval(unsigned int ms);      //此函数自动启动Timer并在timer为nullptr时调用createTimer
    void stopTimer();
    void destroyTimer();
    void updateTime();

    time_t getStartTime() { return t_start; }

    void wait(unsigned int ms);

    //DEEP_TEST
    void initDeepTest();
    void ctrDeepTest();
    void endDeepTest();

    //START
    void initStart();
    void ctrStart();
    void endStart();

    //YAW_TEST
    void initYawTest();
    void ctrYawTest();
    void endYawTest();

    //YAW_TEST1
    void initYawTest1();
    void ctrYawTest1();
    void endYawTest1();

    //FORWARD
    void initForward();
    void ctrForward();
    void endForward();

    //FORWARD1
    void initForward1();
    void ctrForward1();
    void endForward1();

    //GO_UP
    void initGoup();
    void ctrGoup();
    void endGoup();

    //PATH_DIR
    void initPathDir();
    void ctrPathDir();
    void endPathDir();

    //PATH_DIR1
    void initPathDir1();
    void ctrPathDir1();
    void endPathDir1();

    //RANDOM_PINGER
    void initRandomPinger();
    void ctrRandomPinger();
    void endRandomPinger();

    //TURN
    void initTurn();
    void ctrTurn();
    void endTurn();

    //HIT_BALL
    void initHitBall();
    void ctrHitBall();
    void endHitBall();

    //PASS_GATE
    void initPassGate();
    void ctrPassGate();
    void endPassGate();

    //DROP_MARK
    void initDropMark();
    void ctrDropMark();
    void endDropMark();

    //OPEN_BIN
    void initOpenBin();
    void ctrOpenBin();
    void endOpenBin();

    //DROP_MARK1
    void initDropMark1();
    void ctrDropMark1();
    void endDropMark1();

    //DROP_MARKSIMP
    void initDropMarksimp();
    void ctrDropMarksimp();
    void endDropMarksimp();

    //DROP_IT
    void initDropIt();
    void ctrDropIt();
    void endDropIt();

    //GATE_ACT
    void initGateAct();
    void ctrGateAct();
    void endGateAct();
    
    //ACTION_TEST
    void initActionTest();
    void ctrActionTest();
    void endActionTest();

    void initPreTask();
    void ctrPreTask();
    void endPreTask();

    void initGate();
    void ctrGate();
    void endGate();

    void initGate3();
    void ctrGate3();
    void endGate3();

    void initGate2();
    void ctrGate2();
    void endGate2();

    void initGate4();
    void ctrGate4();
    void endGate4();


    void initFlare();
    void ctrFlare();
    void endFlare();

    void initDrop();
    void ctrDrop();
    void endDrop();

    void initAcquire();
    void ctrAcquire();
    void endAcquire();

    void initForward_SBG();
    void ctrForward_SBG();
    void endForward_SBG();

    void initBackward_SBG();
    void ctrBackward_SBG();
    void endBackward_SBG();

    void initRotate_SBG();
    void ctrRotate_SBG();
    void endRotate_SBG();

    void initHang_SBG();
    void ctrHang_SBG();
    void endHang_SBG();

    void initSwing_SBG();
    void ctrSwing_SBG();
    void endSwing_SBG();

    void initGetPhotoF();
    void ctrGetPhotoF();
    void endGetPhotoF();

    void initGetPhotoB();
    void ctrGetPhotoB();
    void endGetPhotoB();

    void initSwingAction();
    void ctrSwingAction();
    void endSwingAction();

    void initFind_The_Gate();
    void ctrFind_The_Gate();
    void endFind_The_Gate();

    void initFind_The_Gate2();
    void ctrFind_The_Gate2();
    void endFind_The_Gate2();

    void initFind_The_Gate3();
    void ctrFind_The_Gate3();
    void endFind_The_Gate3();

    void initForward_Gate();
    void ctrForward_Gate();
    void endForward_Gate();

    void initForwardAction();
    void ctrForwardAction();
    void endForwardAction();

    void initBackwardAction();
    void ctrBackwardAction();
    void endBackwardAction();

    void initRotateAction();
    void ctrRotateAction();
    void endRotateAction();

    void initHangAction();
    void ctrHangAction();
    void endHangAction();

    void initArm_Down();
    void ctrArm_Down();
    void endArm_Down();

    void initArm_Up();
    void ctrArm_Up();
    void endArm_Up();

    void initAcosRevise();
    void ctrAcosRevise();
    void endAcosRevise();

    void initDrumRevise();
    void ctrDrumRevise();
    void endDrumRevise();

    void initBallRevise();
    void ctrBallRevise();
    void endBallRevise();

    void initDivingAction();
    void ctrDivingAction();
    void endDivingAction();

    void initBackwardDrum();
    void ctrBackwardDrum();
    void endBackwardDrum();

    void initGettingAction();
    void ctrGettingAction();
    void endGettingAction();

    void initGrabAction();
    void ctrGrabAction();
    void endGrabAction();

    void initAbandon();
    void ctrAbandon();
    void endAbandon();




signals:
    void allFinished();
    void missionStarted(TASKS_ID);          //在init函数里自行调用
    void missionFinished(TASKS_ID);         //在end函数里自行调用
    void nextTask();
    void endTask();
    void enterAction(ACTION_ID);
    void switchAction(ACTION_ID);
    void changeCurActionLabel(ACTION_ID);
    void returnAction(ACTION_ID);
    void stopAction();

    void setHMotors(QList<pair<MOTORS,float>> value);
    void setAutoDeep(bool sw,QList<configManager::PIDValue> pid={});
    void setVMotors(QList<pair<MOTORS,float>> value);
    void setRelay(QList<pair<int,bool>> value);
    void setGoal(float deep,float pitch=0,float roll=0);
    void setPitchPID(QList<configManager::PIDValue> value);
    void setRollPID(QList<configManager::PIDValue> value);
    void debugCtr(const QString & info);
    //void setArm(bool ok,int pulse);
    void setStepDown(bool ok = true,int ms=-6000);
    void setSevoOpen(bool ok = true,int pulse=0);
    void setStepUp(bool ok = true,int ms=6000);
    void setSevoClose(bool ok = true,int pulse=80);
    void setStep(bool ok,unsigned int ms,bool isHandControl=false,bool direction=false);
    void setSevo(bool ok,int pulse);
    void CloseSevoToVision(bool ok);
    void DownStepToVision(bool ok);

    void SavePhotoB(bool ok);
    void SavePhotoF(bool ok);
    void SetVisDebugRecord(bool ok);

    void grabIt(bool ok, unsigned pulse=0);
    void drop8(bool);
    void drop9(bool);
    void stop_thread();

public slots:
};

#endif // CONTROLLER_H
