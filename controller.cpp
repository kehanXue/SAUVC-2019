#include "controller.h"
#include <QDebug>
#include <QEventLoop>
#include <QApplication>

controller::controller(QObject *parent) : QObject(parent),
    mainTimer(nullptr),
    timerForTime(nullptr),
    config(nullptr)
{
    qRegisterMetaType<TASKS_ID>("TASKS_ID");
    qRegisterMetaType<ACTION_ID>("ACTION_ID");

    connect(this,&controller::nextTask,this,&controller::onNextTask);
    connect(this,&controller::endTask,this,&controller::onEndTask);
    connect(this,&controller::enterAction,this,&controller::onEnterAction);
    connect(this,&controller::returnAction,this,&controller::onReturnAction);
    connect(this,&controller::stopAction,this,&controller::onstopAction);
    // connect(this,&controller::setDeepRev,  trans,&transceiver::setDeepRev);

    // connect(this,&controller::setDeepRev,ctr,&controller::onSetDeepRev);


    {
        pTaskCfg temp = shared_ptr<taskConfig>(new taskConfig{
                                                        &controller::initPreTask,
                                                        &controller::ctrPreTask,
                                                        &controller::endPreTask,
                                                        PreTask
                                                    });
        taskList.insert(make_pair<>(temp->id,temp));
    }

    {
        pTaskCfg temp = shared_ptr<taskConfig>(new taskConfig{
                                                        &controller::initGate,
                                                        &controller::ctrGate,
                                                        &controller::endGate,
                                                        Gate
                                                    });
        taskList.insert(make_pair<>(temp->id,temp));
    }
    {
        pTaskCfg temp = shared_ptr<taskConfig>(new taskConfig{
                                                        &controller::initGate2,
                                                        &controller::ctrGate2,
                                                        &controller::endGate2,
                                                        Gate2
                                                    });
        taskList.insert(make_pair<>(temp->id,temp));
    }
    {
        pTaskCfg temp = shared_ptr<taskConfig>(new taskConfig{
                                                        &controller::initGate3,
                                                        &controller::ctrGate3,
                                                        &controller::endGate3,
                                                        Gate3
                                                    });
        taskList.insert(make_pair<>(temp->id,temp));
    }
    {
        pTaskCfg temp = shared_ptr<taskConfig>(new taskConfig{
                                                        &controller::initGate4,
                                                        &controller::ctrGate4,
                                                        &controller::endGate4,
                                                        Gate4
                                                    });
        taskList.insert(make_pair<>(temp->id,temp));
    }
    {
        pTaskCfg temp = shared_ptr<taskConfig>(new taskConfig{
                                                        &controller::initDrop,
                                                        &controller::ctrDrop,
                                                        &controller::endDrop,
                                                        Drop
                                                    });
        taskList.insert(make_pair<>(temp->id,temp));
    }

    {
        pTaskCfg temp = shared_ptr<taskConfig>(new taskConfig{
                                                        &controller::initAcquire,
                                                        &controller::ctrAcquire,
                                                        &controller::endAcquire,
                                                        Acquire
                                                    });
        taskList.insert(make_pair<>(temp->id,temp));
    }
    {
        pTaskCfg temp = shared_ptr<taskConfig>(new taskConfig{
                                                        &controller::initFlare,
                                                        &controller::ctrFlare,
                                                        &controller::endFlare,
                                                        Flare
                                                    });
        taskList.insert(make_pair<>(temp->id,temp));
    }
    {
        pTaskCfg temp = shared_ptr<taskConfig>(new taskConfig{
                                                        &controller::initForward_SBG,
                                                        &controller::ctrForward_SBG,
                                                        &controller::endForward_SBG,
                                                        FORWARD_SBG
                                                    });
        taskList.insert(make_pair<>(temp->id,temp));
    }
    {
        pTaskCfg temp = shared_ptr<taskConfig>(new taskConfig{
                                                        &controller::initBackward_SBG,
                                                        &controller::ctrBackward_SBG,
                                                        &controller::endBackward_SBG,
                                                        BACKWARD_SBG
                                                    });
        taskList.insert(make_pair<>(temp->id,temp));
    }
    {
        pTaskCfg temp = shared_ptr<taskConfig>(new taskConfig{
                                                        &controller::initRotate_SBG,
                                                        &controller::ctrRotate_SBG,
                                                        &controller::endRotate_SBG,
                                                        ROTATE_SBG
                                                    });
        taskList.insert(make_pair<>(temp->id,temp));
    }
    {
        pTaskCfg temp = shared_ptr<taskConfig>(new taskConfig{
                                                        &controller::initHang_SBG,
                                                        &controller::ctrHang_SBG,
                                                        &controller::endHang_SBG,
                                                        HANG_SBG
                                                    });
        taskList.insert(make_pair<>(temp->id,temp));
    }
    {
        pTaskCfg temp = shared_ptr<taskConfig>(new taskConfig{
                                                        &controller::initGetPhotoF,
                                                        &controller::ctrGetPhotoF,
                                                        &controller::endGetPhotoF,
                                                        GET_PHOTOF
                                                    });
        taskList.insert(make_pair<>(temp->id,temp));
    }
    {
        pTaskCfg temp = shared_ptr<taskConfig>(new taskConfig{
                                                        &controller::initGetPhotoB,
                                                        &controller::ctrGetPhotoB,
                                                        &controller::endGetPhotoB,
                                                        GET_PHOTOB
                                                    });
        taskList.insert(make_pair<>(temp->id,temp));
    }

    {
        pTaskCfg temp = shared_ptr<taskConfig>(new taskConfig{
                                                        &controller::initSwing_SBG,
                                                        &controller::ctrSwing_SBG,
                                                        &controller::endSwing_SBG,
                                                        SWING_SBG
                                                    });
        taskList.insert(make_pair<>(temp->id,temp));
    }
    /*
    {
        pTaskCfg temp = shared_ptr<taskConfig>(new taskConfig{
                                                        &controller::initImgTest_Drum,
                                                        &controller::ctrImgTest_Drum,
                                                        &controller::endImgTest_Drum,
                                                        IMGTEST_DRUM
                                                    });
        taskList.insert(make_pair<>(temp->id,temp));
    }
    */
    /*2016 USA
    {
        //DEEP_TEST
        pTaskCfg temp=shared_ptr<taskConfig>(new taskConfig{
                                                 &controller::initDeepTest,
                                                 &controller::ctrDeepTest,
                                                 &controller::endDeepTest,
                                                 DEEP_TEST
                                             });

        taskList.insert(make_pair<>(temp->id,temp));
    }

    {
        //YAW_TEST
        pTaskCfg temp=shared_ptr<taskConfig>(new taskConfig{
                                                 &controller::initYawTest,
                                                 &controller::ctrYawTest,
                                                 &controller::endYawTest,
                                                 YAW_TEST
                                             });
        taskList.insert(make_pair<>(temp->id,temp));
    }

    {
        //YAW_TEST1
        pTaskCfg temp=shared_ptr<taskConfig>(new taskConfig{
                                                 &controller::initYawTest1,
                                                 &controller::ctrYawTest1,
                                                 &controller::endYawTest1,
                                                 YAW_TEST1
                                             });
        taskList.insert(make_pair<>(temp->id,temp));
    }

    {
          //FORWARD
          pTaskCfg temp=shared_ptr<taskConfig>(new taskConfig{
                                                     &controller::initForward,
                                                     &controller::ctrForward,
                                                     &controller::endForward,
                                                     FORWARD
                                                 });

          taskList.insert(make_pair<>(temp->id,temp));
     }

     {
          //FORWARD1
          pTaskCfg temp=shared_ptr<taskConfig>(new taskConfig{
                                                     &controller::initForward1,
                                                     &controller::ctrForward1,
                                                     &controller::endForward1,
                                                     FORWARD1
                                                 });

          taskList.insert(make_pair<>(temp->id,temp));
     }

     {
          //START
          pTaskCfg temp=shared_ptr<taskConfig>(new taskConfig{
                                                   &controller::initStart,
                                                   &controller::ctrStart,
                                                   &controller::endStart,
                                                   START
                                               });
          taskList.insert(make_pair<>(temp->id,temp));
     }

     {
          //GO_UP
          pTaskCfg temp=shared_ptr<taskConfig>(new taskConfig{
                                                     &controller::initGoup,
                                                     &controller::ctrGoup,
                                                     &controller::endGoup,
                                                     GO_UP
                                                 });

          taskList.insert(make_pair<>(temp->id,temp));
     }

     {
          //PATH_DIR
          pTaskCfg temp=shared_ptr<taskConfig>(new taskConfig{
                                                     &controller::initPathDir,
                                                     &controller::ctrPathDir,
                                                     &controller::endPathDir,
                                                     PATH_DIR
                                                 });
          taskList.insert(make_pair<>(temp->id,temp));
      }

      {
          //PATH_DIR1
          pTaskCfg temp=shared_ptr<taskConfig>(new taskConfig{
                                                     &controller::initPathDir1,
                                                     &controller::ctrPathDir1,
                                                     &controller::endPathDir1,
                                                     PATH_DIR1
                                                 });
          taskList.insert(make_pair<>(temp->id,temp));
       }

       {
          //RANDOM_PINGER
          pTaskCfg temp=shared_ptr<taskConfig>(new taskConfig{
                                                 &controller::initRandomPinger,
                                                 &controller::ctrRandomPinger,
                                                 &controller::endRandomPinger,
                                                 RANDOM_PINGER
                                             });
          taskList.insert(make_pair<>(temp->id,temp));
       }

       {
          //HIT_BALL
          pTaskCfg temp=shared_ptr<taskConfig>(new taskConfig{
                                                 &controller::initHitBall,
                                                 &controller::ctrHitBall,
                                                 &controller::endHitBall,
                                                 HIT_BALL
                                             });
          taskList.insert(make_pair<>(temp->id,temp));
       }

       {
          //TURN
          pTaskCfg temp=shared_ptr<taskConfig>(new taskConfig{
                                              &controller::initTurn,
                                              &controller::ctrTurn,
                                              &controller::endTurn,
                                              TURN
                                          });
          taskList.insert(make_pair<>(temp->id,temp));
       }

       {
          //PASS_GATE
          pTaskCfg temp=shared_ptr<taskConfig>(new taskConfig{
                                                 &controller::initPassGate,
                                                 &controller::ctrPassGate,
                                                 &controller::endPassGate,
                                                 PASS_GATE
                                             });
          taskList.insert(make_pair<>(temp->id,temp));
      }

      {
          //DROP_MARK
          pTaskCfg temp=shared_ptr<taskConfig>(new taskConfig{
                                                 &controller::initDropMark,
                                                 &controller::ctrDropMark,
                                                 &controller::endDropMark,
                                                 DROP_MARK
                                             });
          taskList.insert(make_pair<>(temp->id,temp));
      }

      {
         //OPEN_BIN
          pTaskCfg temp=shared_ptr<taskConfig>(new taskConfig{
                                                 &controller::initOpenBin,
                                                 &controller::ctrOpenBin,
                                                 &controller::endOpenBin,
                                                 OPEN_BIN
                                             });
          taskList.insert(make_pair<>(temp->id,temp));
      }

      {
          //DROP_MARK1
          pTaskCfg temp=shared_ptr<taskConfig>(new taskConfig{
                                                 &controller::initDropMark1,
                                                 &controller::ctrDropMark1,
                                                 &controller::endDropMark1,
                                                 DROP_MARK1
                                             });
          taskList.insert(make_pair<>(temp->id,temp));
      }

      {
          //DROP_MARK_SIMP
          pTaskCfg temp=shared_ptr<taskConfig>(new taskConfig{
                                               &controller::initDropMarksimp,
                                               &controller::ctrDropMarksimp,
                                               &controller::endDropMarksimp,
                                               DROP_MARK_SIMP
                                           });
          taskList.insert(make_pair<>(temp->id,temp));
      }
      {
          //GATE_ACT
          pTaskCfg temp=shared_ptr<taskConfig>(new taskConfig{
                                                 &controller::initGateAct,
                                                 &controller::ctrGateAct,
                                                 &controller::endGateAct,
                                                 GATE_ACT
                                             });
          taskList.insert(make_pair<>(temp->id,temp));
      }
      {
         //DROP_IT
          pTaskCfg temp=shared_ptr<taskConfig>(new taskConfig{
                                               &controller::initDropIt,
                                               &controller::ctrDropIt,
                                               &controller::endDropIt,
                                               DROP_IT
                                           });
          taskList.insert(make_pair<>(temp->id,temp));
      }
    /*
      {
        //ACTION_TEST
          pTaskCfg temp=shared_ptr<taskConfig>(new taskConfig{
                                             &controller::initActionTest,
                                             &controller::ctrActionTest,
                                             &controller::endActionTest,
                                             ACTION_TEST
                                         });
          taskList.insert(make_pair<>(temp->id,temp));
      }
      {
        //ACTION1
          pActionCfg tempa=shared_ptr<actionConfig>(
                      new actionConfig{
                          &controller::initAction1,
                          &controller::ctrAction1,
                          &controller::endAction1,
                          ACTION1
                      });
          actionList.insert(make_pair<>(tempa->id,tempa));
    }
    {
      //ACTION2
        pActionCfg tempa=shared_ptr<actionConfig>(
                    new actionConfig{
                        &controller::initAction2,
                        &controller::ctrAction2,
                        &controller::endAction2,
                        ACTION2
                    });
        actionList.insert(make_pair<>(tempa->id,tempa));
  }
    {
      //ACTION3
        pActionCfg tempa=shared_ptr<actionConfig>(
                    new actionConfig{
                        &controller::initAction3,
                        &controller::ctrAction3,
                        &controller::endAction3,
                        ACTION3
                    });
        actionList.insert(make_pair<>(tempa->id,tempa));
  }
    {
        pActionCfg tempa=shared_ptr<actionConfig>(
                    new actionConfig{
                        &controller::initAction4,
                        &controller::ctrAction4,
                        &controller::endAction4,
                        ACTION4
                    });
        actionList.insert(make_pair<>(tempa->id,tempa));
  }
  */
    {
        pActionCfg tempa = shared_ptr<actionConfig>(new actionConfig{
                                                            &controller::initForwardAction,
                                                            &controller::ctrForwardAction,
                                                            &controller::endForwardAction,
                                                            FORWARDACTION
                                                        });
        actionList.insert(make_pair<>(tempa->id,tempa));
    }
    {
        pActionCfg tempa = shared_ptr<actionConfig>(new actionConfig{
                                                              &controller::initBackwardAction,
                                                              &controller::ctrBackwardAction,
                                                              &controller::endBackwardAction,
                                                              BACKWARDACTION
                                                        });
        actionList.insert(make_pair<>(tempa->id,tempa));
    }

    {
        pActionCfg tempa = shared_ptr<actionConfig>(new actionConfig{
                                                              &controller::initRotateAction,
                                                              &controller::ctrRotateAction,
                                                              &controller::endRotateAction,
                                                              ROTATEACTION
                                                          });
        actionList.insert(make_pair<>(tempa->id,tempa));
    }

    {
        pActionCfg tempa = shared_ptr<actionConfig>(new actionConfig{
                                                              &controller::initHangAction,
                                                              &controller::ctrHangAction,
                                                              &controller::endHangAction,
                                                              HANGACTION
                                                          });
        actionList.insert(make_pair<>(tempa->id,tempa));
    }

    {
        pActionCfg tempa = shared_ptr<actionConfig>(new actionConfig{
                                                              &controller::initAcosRevise,
                                                              &controller::ctrAcosRevise,
                                                              &controller::endAcosRevise,
                                                              ACOSREVISE
                                                          });
        actionList.insert(make_pair<>(tempa->id,tempa));
    }
    {
        pActionCfg tempa = shared_ptr<actionConfig>(new actionConfig{
                                                              &controller::initDrumRevise,
                                                              &controller::ctrDrumRevise,
                                                              &controller::endDrumRevise,
                                                              DRUMREVISE
                                                          });
        actionList.insert(make_pair<>(tempa->id,tempa));
    }
    {
        pActionCfg tempa = shared_ptr<actionConfig>(new actionConfig{
                                                              &controller::initBallRevise,
                                                              &controller::ctrBallRevise,
                                                              &controller::endBallRevise,
                                                              BALLREVISE
                                                          });
        actionList.insert(make_pair<>(tempa->id,tempa));
    }
    {
        pActionCfg tempa = shared_ptr<actionConfig>(new actionConfig{
                                                              &controller::initDivingAction,
                                                              &controller::ctrDivingAction,
                                                              &controller::endDivingAction,
                                                              DIVINGACTION
                                                          });
        actionList.insert(make_pair<>(tempa->id,tempa));
    }
    {
        pActionCfg tempa = shared_ptr<actionConfig>(new actionConfig{
                                                              &controller::initGettingAction,
                                                              &controller::ctrGettingAction,
                                                              &controller::endGettingAction,
                                                              GETTINGACTION
                                                          });
        actionList.insert(make_pair<>(tempa->id,tempa));
    }
    {
        pActionCfg tempa = shared_ptr<actionConfig>(new actionConfig{
                                                              &controller::initGrabAction,
                                                              &controller::ctrGrabAction,
                                                              &controller::endGrabAction,
                                                              GRABACTION
                                                          });
        actionList.insert(make_pair<>(tempa->id,tempa));
    }
    {
        pActionCfg tempa = shared_ptr<actionConfig>(new actionConfig{
                                                              &controller::initAbandon,
                                                              &controller::ctrAbandon,
                                                              &controller::endAbandon,
                                                              ABANDON
                                                          });
        actionList.insert(make_pair<>(tempa->id,tempa));
    }
    {
        pActionCfg tempa = shared_ptr<actionConfig>(new actionConfig{
                                                              &controller::initSwingAction,
                                                              &controller::ctrSwingAction,
                                                              &controller::endSwingAction,
                                                              SWINGACTION
                                                          });
        actionList.insert(make_pair<>(tempa->id,tempa));
    }
    {
        pActionCfg tempa = shared_ptr<actionConfig>(new actionConfig{
                                                              &controller::initFind_The_Gate,
                                                              &controller::ctrFind_The_Gate,
                                                              &controller::endFind_The_Gate,
                                                              FIND_THE_GATE
                                                          });
        actionList.insert(make_pair<>(tempa->id,tempa));
    }
    {
        pActionCfg tempa = shared_ptr<actionConfig>(new actionConfig{
                                                              &controller::initForward_Gate,
                                                              &controller::ctrForward_Gate,
                                                              &controller::endForward_Gate,
                                                              FORWARD_GATE
                                                          });
        actionList.insert(make_pair<>(tempa->id,tempa));
    }
    {
        pActionCfg tempa = shared_ptr<actionConfig>(new actionConfig{
                                                              &controller::initArm_Down,
                                                              &controller::ctrArm_Down,
                                                              &controller::endArm_Down,
                                                              ARM_DOWN
                                                          });
        actionList.insert(make_pair<>(tempa->id,tempa));
    }
    {
        pActionCfg tempa = shared_ptr<actionConfig>(new actionConfig{
                                                              &controller::initArm_Up,
                                                              &controller::ctrArm_Up,
                                                              &controller::endArm_Up,
                                                              ARM_UP
                                                          });
        actionList.insert(make_pair<>(tempa->id,tempa));
    }
    {
        pActionCfg tempa = shared_ptr<actionConfig>(new actionConfig{
                                                              &controller::initBackwardDrum,
                                                              &controller::ctrBackwardDrum,
                                                              &controller::endBackwardDrum,
                                                              BACKWARDDRUM
                                                          });
        actionList.insert(make_pair<>(tempa->id,tempa));
    }

    connect(this,&controller::grabIt,this,&controller::onGrabIt);
    connect(this,&controller::drop8,this,&controller::onDropMark8);
    connect(this,&controller::drop9,this,&controller::onDropMark9);
    connect(this,&controller::setStepDown,this,&controller::onSetStep);
    connect(this,&controller::setStepUp,this,&controller::onSetStep);
    connect(this,&controller::setSevoOpen,this,&controller::onSetSevo);
    connect(this,&controller::setSevoClose,this,&controller::onSetSevo);
}
void controller::setConfig(configManager *cfg)
{
    BOOST_ASSERT((cfg!=nullptr && cfg!=NULL));
    config=cfg;
}

void controller::setVision(visionClass *vis)
{
    BOOST_ASSERT((vis!=nullptr && vis!=NULL));
    vision=vis;
    //进行connect,未完工
    connect(this,&controller::missionStarted,vision,&visionClass::mission_Start);
    connect(this,&controller::missionFinished,vision,&visionClass::mission_Off);
    connect(this,&controller::SavePhotoB,vision,&visionClass::visionsave_photo_b);
    connect(this,&controller::SavePhotoF,vision,&visionClass::visionsave_photo_f);
    connect(this,&controller::CloseSevoToVision,vision,&visionClass::onGetHandclosed);
    connect(this,&controller::DownStepToVision,vision,&visionClass::onGodownFinished);
}

void controller::setTaskList(const QList<TASKS_ID> &idList)
{
    while(!currentTaskList.empty()){
        currentTaskList.pop();
    }

    for(auto & t:idList){
        auto && p=taskList.find(t);
        if(p!=taskList.end()){
            currentTaskList.push(p->second);
        }
        else{
            qDebug() << "Unexpected task id " << t << ", ignored.";
            continue;
        }

    }
}

void controller::setActionList(const QList<ACTION_ID>& actionid)
{
    while(!currentActionList.empty()){
        currentActionList.pop();
    }

    for(auto & t:actionid){
        auto && p=actionList.find(t);
        if(p!=actionList.end()){
            currentActionList.push(p->second);
        }
        else{
            qDebug() << "Unexpected action id " << t << ", ignored.";
            continue;
        }

    }
}
void controller::excecute()
{
    clean();
    if(!currentTaskList.empty()){
        emit nextTask();
    }
    else{
        qDebug() << "Empty task list.";
        emit allFinished();
    }
}

void controller::stop()
{
    stopTimer();
    status.cnt.clear();
    emit missionFinished(ALL);
    setZero1();
    wait(min(status.ms+100,500U));
    while(!currentTaskList.empty()){
        currentTaskList.pop();
    }
    (this->*(status.currentTask->end))();
    clean();
    //发射结束信号,销毁控制定时器，关闭控制时钟，未完工
    destroyTimer();
    emit allFinished();
}

void controller::onAcosRecv(float theta1, float phi1, float theta2, float phi2)
{
    acos.phi1Last=acos.phi1;
    acos.theta1Last=acos.theta1;
    acos.phi2Last=acos.phi2;
    acos.theta2Last=acos.theta2;
    acos.tLast=acos.tNow;
    acos.theta1=theta1;
    acos.phi1=phi1;
    acos.theta2=theta2;
    acos.phi2=phi2;
    acos.tNow=getTime();
}

void controller::onSbgRecv(float roll, float pitch, float yaw, float magNorm)
{
    sbg.rollLast=sbg.roll;
    sbg.pitchLast=sbg.pitch;
    sbg.yawLast=sbg.yaw;
    sbg.tLast=sbg.tNow;
    sbg.roll=roll;
    sbg.pitch=pitch;
    sbg.yaw=yaw;
    sbg.magNorm=magNorm;
    sbg.tNow=getTime();
}

void controller::onDeepRecv(float value)
{
    deep.value=value;
}
void controller::onStepRecv(bool ok)
{
    step_flag=ok;
}

void controller::onSpeedRecv(float lu, float ld, float ru, float rd)
{
    speed.value[LEFT_UP]=lu;
    speed.value[LEFT_DOWN]=ld;
    speed.value[RIGHT_UP]=ru;
    speed.value[RIGHT_DOWN]=rd;
}

void controller::onSetDeepRev(float rev)
{
    deep.deepRev=rev;
    sbg.tLast=sbg.tNow;
    sbg.tNow=getTime();
}

void controller::onSetTarget(float rev)
{
    set_yaw_target = rev;
    qDebug() << "set yaw target:" << set_yaw_target;
}

void controller::onSetHMotors(QList<pair<MOTORS, float> > value)
{
    for(auto & t:value){
        //使用switch检查输入是否合法
        switch (t.first) {
        case MAIN_LEFT:
        case MAIN_RIGHT:
        case SIDE_UP:
        case SIDE_DOWN:
            speed.value[t.first]=t.second;
            break;
        default:
            break;
        }
    }
    emit setHMotors(value);
}

void controller::onSetVMotors(QList<pair<MOTORS, float> > value)
{
    //emit setAutoDeep(false);
    for(auto & t:value){
        //使用switch检查输入是否合法
        switch (t.first) {
        case LEFT_UP:
        case LEFT_DOWN:
        case RIGHT_UP:
        case RIGHT_DOWN:
            speed.value[t.first]=t.second;
            break;
        default:
            break;
        }
    }
    emit setVMotors(value);
}

void controller::onSetRelay(QList<pair<int, bool> > value)
{
    for(auto & t:value){
        if(t.first<NUMBER_OF_RELAYS && t.first>=0){
            relay.value[t.first]=t.second;
        }
    }
    emit setRelay(value);
}

void controller::appendConfig(CONFIG_ID id)
{
    auto & value=config->getPID(id);
    for(auto & t:value){
        status.val[t.motorID].p=t.pValue;
        status.val[t.motorID].i=t.iValue;
        status.val[t.motorID].d=t.dValue;
    }
}

void controller::clearConfig()
{
    memset(&(status.val),0,NUMBER_OF_MOTORS*sizeof(PIDValue));
}

void controller::loadConfig(CONFIG_ID id)
{
    clearConfig();
    appendConfig(id);
}

void controller::updateConfig()
{
    const static MOTORS vList[4]={LEFT_UP,LEFT_DOWN,RIGHT_UP,RIGHT_DOWN};
    configManager::PIDValue tVal;
    QList<configManager::PIDValue> tmp;
    for(int i=0;i<4;i++){
        tVal.motorID=vList[i];
        tVal.pValue=status.val[vList[i]].p;
        tVal.iValue=status.val[vList[i]].i;
        tVal.dValue=status.val[vList[i]].d;
        tmp.push_back(tVal);
    }
    emit setAutoDeep(true,tmp);
}

void controller::setZero()
{
    QList<pair<MOTORS,float>> tmp;
    emit setHMotors(tmp);
}

void controller::setZero1()
{
    emit setAutoDeep(false);
    QEventLoop loop;
    QTimer::singleShot(200,&loop,&QEventLoop::quit);
    loop.exec();
    QList<pair<MOTORS,float>> tmp;
    emit setHMotors(tmp);
}

void controller::onNextTask()
{
    //此时上个任务已经清理完毕。
    //通用的工作
    if(currentTaskList.empty()){
        //此时认为发生了不正常情况，因为如果为空就不可能调度此函数。
        qDebug() << "Function nextTask() called unexpectedly when the currentTaskList is empty.\n";
        stop();
        return;
    }
    status.currentTask=currentTaskList.front();
    status.cnt.clear();
    //默认200ms一次,init里面可能会更改
    status.ms=200;
    while(!statusStack.empty()){
        statusStack.pop();
    }
    //init里面可能会修改
    status.ActionFlag = NOP;
    status.finished = NOP;
    status.curPara = {NOP,0,0};
    setZero();

    (this->*(status.currentTask->init))();

    setFrameInteval(status.ms);
}
void controller::onEndTask()
{
    stopTimer();
    QApplication::processEvents();
    if(currentTaskList.empty()){
        return;
    }
    status.finished = NOP;
    (this->*(status.currentTask->end))();
    while(!currentActionList.empty())currentActionList.pop();   //clear Action queue.
    while(!ActionParaStack.empty())ActionParaStack.pop();
    while(!ReplaystatusStack.empty())ReplaystatusStack.pop();
    while(!statusStack.empty())statusStack.pop();
    status.ActionList.clear();
    currentTaskList.pop();
    status.cnt.clear();
    if(!currentTaskList.empty()){
        emit nextTask();
    }
    else{
        qDebug() << "All finished.";
        setZero1();
        emit allFinished();
    }
}

void controller::ctrFrame()
{
    if(status.ActionFlag == NOP) {
        (this->*(status.currentTask->frame))();                          //调用MainTask函数
        //qDebug() << "调用MainTask函数 调用MainTask函数 调用MainTask函数";
    }
    else {
        (this->*(status.curAction->frame))();                            //调用Action函数
        //qDebug() << "调用Action函数 调用Action函数 调用Action函数";
    }
}



void controller::onEnterAction(ACTION_ID id)
{
    stopTimer();                       //停止计时
    statusStack.push(status);          //保存当前任务状态
    status.cnt.clear();                //清空计数器
    status.ActionFlag = id;            //变更Flag
    status.finished = NOP;             //清空完成标志位
    if(id == ABANDON)ActionParaStack.push({ABANDON,0,0,NO});//临时压入ABANDON数据
    if(status.ReplayActionFlag != id)                  //首次执行
    {
    status.curAction = currentActionList.front();      //提取队头Action函数
    status.curPara = ActionParaStack.top();            //提取Action参数栈首
    ActionParaStack.pop();                             //参数出栈
    if(id != ABANDON)currentActionList.pop();          //队头Action函数出队
    (this->*(status.curAction->init))();               //执行当前Action的Init
    qDebug()<<"First Action Initial";
    }
    else {                                             //重复执行
        status = ReplaystatusStack.top();
        ReplaystatusStack.pop();
        qDebug()<<"Replay Action"<<status.cnt[0];
        setFrameInteval(status.ms);
    }
}

/*void controller::onReturnSubtask()
{
    stopTimer();
    (this->*(status.curAction->end))();
    if(statusStack.empty()){
        emit endTask();
        return;
    }
    status=statusStack.top();
    statusStack.pop();
    emit changeCurActionLabel(status.ActionFlag);
    setFrameInteval(status.ms);
}
*/
void controller::onReturnAction(ACTION_ID id)
{
    stopTimer();                         //停止计时
    (this->*(status.curAction->end))();  //调用Action结束函数
    if(statusStack.empty()){
        return;
    }
    status=statusStack.top();            //提取任务栈首，即MainTask或上一个Action
    statusStack.pop();                   //出栈
    if(id == ABANDON)
    {
        if(status.curAction->id == DIVINGACTION)
        {
            statusStack.pop();
            status=statusStack.top();
        }
        else if(status.curAction->id == GETTINGACTION)
        {
            statusStack.pop();
            statusStack.pop();
            statusStack.pop();
            statusStack.pop();
            status=statusStack.top();
        }
    }
    if(id == GRABACTION)emit endTask();
    status.finished = id;               //结束标志位
    setFrameInteval(status.ms);
}
void controller::onstopAction()
{
    stopTimer();
    if(status.curPara.ReplayFlag == MINUS)
    {
        status.cnt[0]--;
        if(status.cnt[0] < 0)status.cnt[0] = 0;
    }
    ReplaystatusStack.push(status);
    status=statusStack.top();
    statusStack.pop();
    status.ReplayActionFlag = ReplaystatusStack.top().ActionFlag;
    qDebug()<<"Action Stopped!";
    setFrameInteval(status.ms);
}

void controller::clean()
{
    clearConfig();
    //发射各种信号清零状态
    setZero();
    emit setAutoDeep(false);
    emit missionFinished(ALL);
}

void controller::onGrabIt(bool toClose,unsigned int pulse)
{
    //QList<pair<int,bool>> tmp;
    //tmp.push_back(make_pair<>(2,toClose));
    //tmp.push_back(make_pair<>(7,toClose));
    //emit setArm(toClose,pulse);
}

void controller::onDropMark8(bool toDrop)
{
    qDebug()<<"Drop_Success.";
    QList<pair<int,bool>> tmp;
    tmp.push_back(make_pair<>(8,toDrop));
    emit setRelay(tmp);
}

void controller::onDropMark9(bool toDrop)
{
    qDebug()<<"Drop_Success.";
    QList<pair<int,bool>> tmp;
    tmp.push_back(make_pair<>(9,toDrop));
    emit setRelay(tmp);
}

void controller::setFrameInteval(unsigned int ms)
{
    if(nullptr==mainTimer){
        createTimer();
    }

    mainTimer->setInterval(ms);
    mainTimer->start();
}

void controller::createTimer()
{
    destroyTimer();
    mainTimer=new QTimer(this);
    connect(mainTimer,&QTimer::timeout,this,&controller::ctrFrame);
    timerForTime=new QTimer(this);
    connect(timerForTime,&QTimer::timeout,this,&controller::updateTime);
    timeval temp;
    gettimeofday(&temp,NULL);
    t_start=temp.tv_sec*1000+temp.tv_usec/1000;
    t_now=1;
    timerForTime->setInterval(50);
    qDebug() << "Starting counting time...\n";
    timerForTime->start();
}

void controller::stopTimer()
{
    mainTimer->stop();
}

void controller::destroyTimer()
{
    if(mainTimer!=nullptr){
        disconnect(mainTimer,&QTimer::timeout,this,&controller::ctrFrame);
        delete mainTimer;
        mainTimer=nullptr;
    }
    if(timerForTime!=nullptr){
        disconnect(timerForTime,&QTimer::timeout,this,&controller::updateTime);
        delete timerForTime;
        timerForTime=nullptr;
    }
}

void controller::updateTime()
{
    timeval temp;
    gettimeofday(&temp,NULL);
    t_now=temp.tv_sec*1000+temp.tv_usec/1000;
}

void controller::wait(unsigned int ms)
{
    QEventLoop loop;
    QTimer::singleShot(ms,&loop,&QEventLoop::quit);
    loop.exec();
}

void controller::initPreTask()
{
    loadConfig(STOP);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.ms = 200;
    setFrameInteval(status.ms);
    qDebug()<<"PreTask";
    qDebug()<<"With Best Wishes For Our Homeland";
}
void controller::ctrPreTask()
{
    status.cnt[0]++;
    /*
    if(deep.value>=0.1)
    {
        qDebug()<<"Deep Completed";
        status.cnt[1]++;
        if(status.cnt[1]>=10)
        {
            emit endTask();
        }
    }
    */
    if(status.cnt[0]>=150)emit endTask();
}
void controller::endPreTask()
{
    status.cnt.clear();
    //setZero();
    wait(2000);
    qDebug() << "EndPreTask,Good Luck";
}

void controller::initGate()      //正常
{
    wait(45000);

    qDebug() << "initGate initGate initGate initGate initGate initGate";

    emit setGoal(global_deep);
    loadConfig(HANG);
    updateConfig();
    setDeepCtr(true);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.ActionList << HANGACTION
                      << FORWARDACTION
                      << SWINGACTION
                      << FIND_THE_GATE
                      << FORWARD_GATE;

    ActionParaStack.push({FORWARD_GATE, 170, 0, NO});
    ActionParaStack.push({FIND_THE_GATE, 0, 0, NO});
    ActionParaStack.push({SWINGACTION, 10, 0, NO});
    ActionParaStack.push({FORWARDACTION, 87, 0, NO});
    ActionParaStack.push({HANGACTION, 0, 0, NO});

    this->setActionList(status.ActionList);
    emit missionStarted(Gate);
    isForwardInitial = false;
    status.ms = 200;
    setFrameInteval(status.ms);
    qDebug() << "Pore Gate";
}

void controller::ctrGate()
{
    emit setGoal(global_deep);
    loadConfig(HANG);
    updateConfig();
    status.cnt[0]++;
    qDebug() << "Gate MainTask" << status.cnt[0];
    if(status.cnt[0]==1)
    {
        emit enterAction(FORWARDACTION);
    }

    // TODO
    visionClass::visionData tmp = vision->getData();
    if(tmp.m1_centerdx == -999)
    {

        status.cnt[1]++;
        if(status.cnt[1] > 8)
        {
            emit endTask();
        }
    }
}
void controller::endGate()
{
    status.cnt.clear();
    // setZero();
    emit missionFinished(Gate);
    wait(3000);
    qDebug() << "EndGate";
}
void controller::initGate2()
{
    emit setGoal(1.3);
    loadConfig(HANG);
    updateConfig();
    setDeepCtr(true);
    status.cnt.push_back(0);
    status.ActionList << FORWARDACTION
                      << FIND_THE_GATE
                      << FORWARDACTION;
    ActionParaStack.push({FORWARDACTION,20,0,NO});
    ActionParaStack.push({FIND_THE_GATE2,0,0,NO});
    ActionParaStack.push({FORWARDACTION,0,0,NO});
    this->setActionList(status.ActionList);
    isForwardInitial = false;
    status.ms = 200;
    setFrameInteval(status.ms);
    qDebug() << "Pore Gate2";
}
void controller::ctrGate2()
{
    status.cnt[0]++;
    qDebug() << "Gate2 MainTask" << status.cnt[0];
    if(status.cnt[0]==1)emit enterAction(FORWARDACTION);
}
void controller::endGate2()
{
    status.cnt.clear();
    //setZero();
    emit missionFinished(Gate);
    qDebug() << "EndGate2";
}
void controller::initGate3()     //灵魂
{
    emit setGoal(1.3);
    loadConfig(HANG);
    updateConfig();
    setDeepCtr(true);
    status.cnt.push_back(0);
    status.ActionList << FORWARDACTION
                      << FIND_THE_GATE3
                      << BACKWARDACTION;
    ActionParaStack.push({BACKWARDACTION,20,0,NO});
    ActionParaStack.push({FIND_THE_GATE3,0,0,NO});
    ActionParaStack.push({FORWARDACTION,0,0,NO});
    this->setActionList(status.ActionList);
    isForwardInitial = false;
    status.ms = 200;
    setFrameInteval(status.ms);
    qDebug() << "Pore Gate3";
}
void controller::ctrGate3()
{
    status.cnt[0]++;
    qDebug() << "Gate3 MainTask" << status.cnt[0];
    if(status.cnt[0]==1)emit enterAction(FORWARDACTION);
}
void controller::endGate3()
{
    status.cnt.clear();
    //setZero();
    emit missionFinished(Gate);
    qDebug() << "EndGate3";
}
void controller::initGate4()
{
    emit setGoal(0.6);
    loadConfig(HANG);
    updateConfig();
    setDeepCtr(true);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    this->setActionList(status.ActionList);
    //emit missionStarted(Gate);
    //isForwardInitial = false;
    sbg.goal = sbg.yaw;
    status.ms = 200;
    setFrameInteval(status.ms);
    qDebug() << "Pore Gate4";
}
void controller::ctrGate4()
{
    status.cnt[0]++;
    qDebug()<<"HANG"<<status.cnt[0];
    if(abs(0.6-deep.value)<0.05)
    {
        status.cnt[1]++;
    }
    if(status.cnt[1]>=10)
    {
        status.cnt[2]++;
        emit setGoal(0.6);
        loadConfig(HANG);
        updateConfig();
        loadConfig(FORWARD_SLOW);
        //visionClass::visionData && tmp = vision->getData();
        const static MOTORS hList[4]={MAIN_LEFT,MAIN_RIGHT,SIDE_UP,SIDE_DOWN};
        const static MOTORS zList[2]={SIDE_UP,SIDE_DOWN};
        float tempValue[NUMBER_OF_MOTORS];
        if(status.cnt[0]==1)
        {
           // sbg.goal = sbg.yaw;
           // Gate_Straight = sbg.yaw;
        }
        float sbgError=sbg.goal-sbg.yaw;
        if(sbgError>180){
            sbgError=-360+sbgError;
        }
        if(sbgError<-180){
            sbgError=360+sbgError;
        }
        qDebug()<<sbgError;
        //float sbgDiffT=(sbg.tNow-sbg.tLast)/1000.0;
        float sbgDiffT=0.01;
        float sbgDiff=sbgError-sbg.yawErrorLast;
        sbg.yawErrorLast=sbgError;

        //tempValue[MAIN_LEFT]=50+status.val[MAIN_LEFT].p*sbgError/100.0
        //         +status.val[MAIN_LEFT].d*sbgDiff/sbgDiffT;
        //tempValue[MAIN_RIGHT]=50+status.val[MAIN_RIGHT].p*sbgError/100.0
        //         +status.val[MAIN_RIGHT].d*sbgDiff/sbgDiffT;
        tempValue[MAIN_LEFT]=50;
        tempValue[MAIN_RIGHT]=50;
        tempValue[SIDE_UP]=0;
        tempValue[SIDE_DOWN]=0;

        if(abs((int)sbgError)>=55){
             tempValue[SIDE_UP]=status.val[SIDE_UP].p*sbgError/100.0
                      +status.val[SIDE_UP].d*sbgDiff/sbgDiffT;
             tempValue[SIDE_DOWN]=status.val[SIDE_DOWN].p*sbgError/100.0
                      +status.val[SIDE_DOWN].d*sbgDiff/sbgDiffT;
         }
        for(int i=0;i<4;i++)
        {
            if(tempValue[hList[i]]>=70){
                tempValue[hList[i]]=70;
            }
            else if(tempValue[hList[i]]<=-70){
                tempValue[hList[i]]=-70;
            }
        }
        for(int i=0;i<2;i++)
        {
            if(tempValue[zList[i]]>=45){
                tempValue[zList[i]]=45;
            }
            else if(tempValue[zList[i]]<=-45){
                tempValue[zList[i]]=-45;
            }
        }
        qDebug()<<"FORWARDWARDACTION"<<status.cnt[0];
        QList<pair<MOTORS,float>> tempList;
        for(int i=0;i<4;i++){
            tempList.push_back(make_pair<>(hList[i],tempValue[hList[i]]));
        }
        emit setHMotors(tempList);
        if(status.cnt[2]>=150)emit endTask();
    }
}
void controller::endGate4()
{
    status.cnt.clear();
    //setZero();
    //emit missionFinished(Gate);
    wait(3000);
    qDebug() << "EndGate";
}

void controller::initFlare()
{

    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.ms = 200;
    acos.goal = 90;
    img.flareStarted = false;
    img.flareImgFlag = false;

    sbg.goal = sbg.yaw;

    emit missionStarted(Flare);
    setFrameInteval(status.ms);

    loadConfig(HANG);
    emit setGoal(global_deep);
    updateConfig();
    qDebug()<<"Pore Flare";
}
void controller::ctrFlare()
{
//    if(status.cnt[0] >= 50)
//    {
//        emit endTask();
//    }

    status.cnt[0]++;
    qDebug()<<"Flare MainTask";
    visionClass::visionData && tmp = vision->getData();
    float tempValue[NUMBER_OF_MOTORS];

    const static MOTORS hList[4]={MAIN_LEFT,MAIN_RIGHT,SIDE_UP,SIDE_DOWN};


    static int pushed_repeat_cnt = 0;
    static std::deque<double> deq_acos_theta2(1);

    const int pushed_repeat_cnt_max = 5;   //acos Hz: 1Hz, controller Hz: 5Hz
    const int deq_max_length = 5;


    if(acos.theta2 == acos.theta2Last)
    {
        pushed_repeat_cnt++;
    }
    else if(pushed_repeat_cnt > pushed_repeat_cnt_max || acos.theta2 != acos.theta2Last)
    {
        deq_acos_theta2.push_back(acos.theta2);
        qDebug() << "raw acos.theta2 pushed back: " << acos.theta2;
        pushed_repeat_cnt = 1;
    }

    acos.theta2Last = acos.theta2;

    if(deq_acos_theta2.size() > deq_max_length)
    {
        while (deq_acos_theta2.size() > deq_max_length) {
            deq_acos_theta2.pop_front();
        }
    }

    qDebug() << "deque elems:";
    for(int i = 0; i < deq_acos_theta2.size(); i++)
    {
        qDebug() << deq_acos_theta2.at(i);
    }
    qDebug();

    std::deque<double> tmp_deq_acos_theta2 = deq_acos_theta2;
    std::sort(tmp_deq_acos_theta2.begin(), tmp_deq_acos_theta2.end());

    double tmp_acos_theta2 = tmp_deq_acos_theta2.at((tmp_deq_acos_theta2.size()-1)/2>=0 ? (tmp_deq_acos_theta2.size()-1)/2 : 0);

    qDebug() << "Choose acos.theta2:   " << tmp_acos_theta2;


    static bool flag_start_timeout = false;
    if(tmp.m4_flare_dx == -999)//图像导引结束，已识别到杆，切换直航
    {
        qDebug() << "图像导引结束，已识别到杆，切换直航 图像导引结束，已识别到杆，切换直航 图像导引结束，已识别到杆，切换直航";
        if(status.cnt[1] == 0)
        {
            sbg.goal = sbg.yaw;
        }
        status.cnt[1]++;
        qDebug()<<"Recognized, now Crash the bar";

        loadConfig(FORWARD_SLOW);

        float sbgError = sbg.goal-sbg.yaw;
        if(sbgError > 180)
        {
            sbgError = -360+sbgError;
        }
        if(sbgError < -180)
        {
            sbgError = 360+sbgError;
        }

        float sbgDiff = sbgError-sbg.yawErrorLast;
        sbg.yawErrorLast = sbgError;

        // const int max_main_speed = 45;
        const int max_main_speed = acos_max_main_speed+8;
        const int max_side_speed = 50;

        if (sbgError < 0)
        {
            tempValue[MAIN_LEFT] = max_main_speed + status.val[MAIN_LEFT].p*sbgError + status.val[MAIN_LEFT].d*sbgDiff;
            tempValue[MAIN_RIGHT] = max_main_speed;
        }
        else if (sbgError >= 0)
        {
            tempValue[MAIN_LEFT] = max_main_speed;
            tempValue[MAIN_RIGHT] = max_main_speed -status.val[MAIN_RIGHT].p*sbgError - status.val[MAIN_RIGHT].d*sbgDiff;
        }
        tempValue[SIDE_UP] = 0;
        tempValue[SIDE_DOWN] = 0;

        for(int i = 0; i < 2; i++)
        {
            if(tempValue[hList[i]] >= max_main_speed)
            {
                tempValue[hList[i]] = max_main_speed;
            }
            else if(tempValue[hList[i]] <= -max_main_speed)
            {
                tempValue[hList[i]] = -max_main_speed;
            }
        }
        for(int i = 2; i < 4; i++)
        {
            if(tempValue[hList[i]] >= max_side_speed)
            {
                tempValue[hList[i]] = max_side_speed;
            }
            else if(tempValue[hList[i]] <= -max_side_speed)
            {
                tempValue[hList[i]] = -max_side_speed;
            }
        }

        if(status.cnt[1] >= 25)
        {
            tempValue[MAIN_LEFT]=0;
            tempValue[MAIN_RIGHT]=0;
            tempValue[SIDE_UP]=0;
            tempValue[SIDE_DOWN]=0;
            emit endTask();
        }
    }

    else if(tmp.m4_flare_dx!=-999 && tmp.m4_flare_dx!=999 && img.flareStarted && abs(90-tmp_acos_theta2)<45)//图像导引
    {

        loadConfig(FORWARD_FORSEE);

        status.cnt[3]++;
        if(status.cnt[3] >= 30)
        {
            img.flareImgFlag = true;
        }
        qDebug()<<"Navigated,now Recognized";

        float delta_t=(img.t_now-img.t_last)/1000.0;

        img.flare_dx_last=img.flare_dx;

        img.t_last=img.t_now;

        img.flare_dx = tmp.m4_flare_dx/10;
        img.flare_dx_diff = img.flare_dx-img.flare_dx_last;

        img.t_now= tmp.t_now;


        // const int max_main_speed = 40;
        const int max_main_speed = acos_max_main_speed+3;
        const int max_side_speed = 50;

        if (img.flare_dx < 0)
        {
            tempValue[MAIN_RIGHT] = max_main_speed + status.val[MAIN_RIGHT].p*img.flare_dx + status.val[MAIN_RIGHT].d*img.flare_dx_diff;
            tempValue[MAIN_LEFT] = max_main_speed;
            // tempValue[SIDE_UP] = 0 + status.val[SIDE_UP].p*img.flare_dx + status.val[SIDE_DOWN].d*img.flare_dx_diff;
            // tempValue[SIDE_DOWN] = 0 - status.val[SIDE_UP].p*img.flare_dx - status.val[SIDE_DOWN].d*img.flare_dx_diff;

        }
        else if (img.flare_dx >= 0)
        {
            tempValue[MAIN_RIGHT] = max_main_speed;
            tempValue[MAIN_LEFT] = max_main_speed - status.val[MAIN_LEFT].p*img.flare_dx - status.val[MAIN_LEFT].d*img.flare_dx_diff;
            // tempValue[SIDE_UP] = 0 + status.val[SIDE_UP].p*img.flare_dx + status.val[SIDE_DOWN].d*img.flare_dx_diff;
            // tempValue[SIDE_DOWN] = 0 - status.val[SIDE_UP].p*img.flare_dx - status.val[SIDE_DOWN].d*img.flare_dx_diff;
        }

        if (img.flare_dx > 0)
        {
            tempValue[SIDE_UP] = 25*((img.flare_dx)/fabs(img.flare_dx)) + status.val[SIDE_UP].p*img.flare_dx + status.val[SIDE_DOWN].d*img.flare_dx_diff; // - status.val[SIDE_UP].p*yaw_swing_radio*sbgError - status.val[SIDE_UP].d*yaw_swing_radio*sbgDiff;
            tempValue[SIDE_DOWN] = 25*((img.flare_dx)/fabs(img.flare_dx)) + status.val[SIDE_UP].p*img.flare_dx + status.val[SIDE_DOWN].d*img.flare_dx_diff; // + status.val[SIDE_DOWN].p*yaw_swing_radio*sbgError + status.val[SIDE_DOWN].d*yaw_swing_radio*sbgDiff;
        }
        else if (img.flare_dx < 0)
        {
            tempValue[SIDE_UP] = 18*((img.flare_dx)/fabs(img.flare_dx)) + status.val[SIDE_UP].p*img.flare_dx + status.val[SIDE_DOWN].d*img.flare_dx_diff; // - status.val[SIDE_UP].p*yaw_swing_radio*sbgError - status.val[SIDE_UP].d*yaw_swing_radio*sbgDiff;
            tempValue[SIDE_DOWN] = 18*((img.flare_dx)/fabs(img.flare_dx)) + status.val[SIDE_UP].p*img.flare_dx + status.val[SIDE_DOWN].d*img.flare_dx_diff; // + status.val[SIDE_DOWN].p*yaw_swing_radio*sbgError + status.val[SIDE_DOWN].d*yaw_swing_radio*sbgDiff;
        }
        else if (img.flare_dx == 0)
        {
            tempValue[SIDE_UP] = 0 + status.val[SIDE_UP].p*img.flare_dx + status.val[SIDE_DOWN].d*img.flare_dx_diff; // - status.val[SIDE_UP].p*yaw_swing_radio*sbgError - status.val[SIDE_UP].d*yaw_swing_radio*sbgDiff;
            tempValue[SIDE_DOWN] = 0 + status.val[SIDE_UP].p*img.flare_dx + status.val[SIDE_DOWN].d*img.flare_dx_diff; // + status.val[SIDE_DOWN].p*yaw_swing_radio*sbgError + status.val[SIDE_DOWN].d*yaw_swing_radio*sbgDiff;
        }

        tempValue[SIDE_UP] = 0;
        tempValue[SIDE_DOWN] = 0;

        for(int i = 0; i < 2; i++)
        {
            if(tempValue[hList[i]] >= max_main_speed)
            {
                tempValue[hList[i]] = max_main_speed;
            }
            else if(tempValue[hList[i]] <= -max_main_speed)
            {
                tempValue[hList[i]] = -max_main_speed;
            }
        }
        for(int i = 2; i < 4; i++)
        {
            if(tempValue[hList[i]] >= max_side_speed)
            {
                tempValue[hList[i]] = max_side_speed;
            }
            else if(tempValue[hList[i]] <= -max_side_speed)
            {
                tempValue[hList[i]] = -max_side_speed;
            }
        }
    }
    else if(tmp.m4_flare_dx != 999 && !img.flareStarted && abs(90-tmp_acos_theta2) < 45)
    {
        loadConfig(FORWARD_FORSEE);
        flag_start_timeout = true;
        status.cnt[3]++;
        if(status.cnt[3]>=15)
        {
            img.flareImgFlag = true;
        }
        qDebug()<<"Found the bar,now Navigate";

        float delta_t=(img.t_now-img.t_last)/1000.0;

        flag_start_timeout = true;
        img.flareStarted=true;

        img.flare_dx_last=0;
        img.flare_dx_diff=0;

        img.t_last=0;
        img.t_now= tmp.t_now;

        // const int max_main_speed = 35;
        const int max_main_speed = acos_max_main_speed-2;
        const int max_side_speed = 50;

        if (img.flare_dx < 0)
        {
            tempValue[MAIN_RIGHT] = max_main_speed + status.val[MAIN_RIGHT].p*img.flare_dx + status.val[MAIN_RIGHT].d*img.flare_dx_diff;
            tempValue[MAIN_LEFT] = max_main_speed;
        }
        else if (img.flare_dx >= 0)
        {
            tempValue[MAIN_RIGHT] = max_main_speed;
            tempValue[MAIN_LEFT] = max_main_speed - status.val[MAIN_LEFT].p*img.flare_dx - status.val[MAIN_LEFT].d*img.flare_dx_diff;

        }
        tempValue[SIDE_UP] = 0;
        tempValue[SIDE_DOWN] = 0;

        for(int i = 0; i < 2; i++)
        {
            if(tempValue[hList[i]] >= max_main_speed)
            {
                tempValue[hList[i]] = max_main_speed;
            }
            else if(tempValue[hList[i]] <= -max_main_speed)
            {
                tempValue[hList[i]] = -max_main_speed;
            }
        }
        for(int i = 2; i < 4; i++)
        {
            if(tempValue[hList[i]] >= max_side_speed)
            {
                tempValue[hList[i]] = max_side_speed;
            }
            else if(tempValue[hList[i]] <= -max_side_speed)
            {
                tempValue[hList[i]] = -max_side_speed;
            }
        }

    }
    else if(status.cnt[5]>=15)                                       //水声导引
    {
        loadConfig(FORWARD_ACOS);
        qDebug() << "AcosRevise";
        img.flareStarted = false;
        img.flareImgFlag = false;

        status.cnt[3] = 0;

        static double tmp_acos_theta2_last = 0;
        if(tmp_acos_theta2 - tmp_acos_theta2_last != 0)
        {
            tmp_acos_theta2_last = tmp_acos_theta2;

            qDebug() << "use acos.theta2:  " << tmp_acos_theta2;

            float acosError = 90 - tmp_acos_theta2;
            if(acosError > 180)
            {
                acosError = -360 + acosError;
            }
            else if(acosError < -180)
            {
                acosError = 360 + acosError;
            }

            sbg.goal = sbg.yaw+acosError;

        }

        float sbgError = sbg.goal-sbg.yaw;
        if(sbgError > 180)
        {
            sbgError = -360+sbgError;
        }
        if(sbgError < -180)
        {
            sbgError = 360+sbgError;
        }

        float sbgDiff = sbgError-sbg.yawErrorLast;
        sbg.yawErrorLast = sbgError;


        // const int max_main_speed = 37;
        const int max_main_speed = acos_max_main_speed;
        const int max_side_speed = 35;

        qDebug() << "sbgError                                                                sbgError:" << sbgError;
        if (sbgError < 0)
        {
            tempValue[MAIN_LEFT] = max_main_speed + status.val[MAIN_LEFT].p*sbgError + status.val[MAIN_LEFT].d*sbgDiff;
            tempValue[MAIN_RIGHT] = max_main_speed;

            tempValue[SIDE_UP] = 15 - status.val[SIDE_UP].p*sbgError - status.val[SIDE_DOWN].d*sbgDiff;
            tempValue[SIDE_DOWN] = -15 + status.val[SIDE_UP].p*sbgError + status.val[SIDE_DOWN].d*sbgDiff;
        }
        else if (sbgError >= 0)
        {
            tempValue[MAIN_LEFT] = max_main_speed;
            tempValue[MAIN_RIGHT] = max_main_speed - status.val[MAIN_RIGHT].p*sbgError - status.val[MAIN_RIGHT].d*sbgDiff;

            tempValue[SIDE_UP] = -15 - status.val[SIDE_UP].p*sbgError - status.val[SIDE_DOWN].d*sbgDiff;
            tempValue[SIDE_DOWN] = 15 + status.val[SIDE_UP].p*sbgError + status.val[SIDE_DOWN].d*sbgDiff;
        }

        for(int i = 0; i < 2; i++)
        {
            if(tempValue[hList[i]] >= max_main_speed)
            {
                tempValue[hList[i]] = max_main_speed;
            }
            else if(tempValue[hList[i]] <= -max_main_speed)
            {
                tempValue[hList[i]] = -max_main_speed;
            }
        }
        for(int i = 2; i < 4; i++)
        {
            if(tempValue[hList[i]] >= max_side_speed)
            {
                tempValue[hList[i]] = max_side_speed;
            }
            else if(tempValue[hList[i]] <= -max_side_speed)
            {
                tempValue[hList[i]] = -max_side_speed;
            }
        }
    }
    else
    {
        qDebug()<<"Prepare AcosRevise,Hanging";
        status.cnt[5]++;
        loadConfig(HANG);
        emit setGoal(global_deep);
        updateConfig();
        tempValue[MAIN_LEFT]=0;
        tempValue[MAIN_RIGHT]=0;
        tempValue[SIDE_UP]=0;
        tempValue[SIDE_DOWN]=0;
    }

    if(flag_start_timeout)
    {
        status.cnt[2]++;
    }
    if(status.cnt[2] >= 450)
    {
        qDebug() << "//////超时放弃";
        emit endTask();//超时放弃
    }
    QList<pair<MOTORS,float>> tempList;
    qDebug() << "tmp.m4_flare_dx:      " << tmp.m4_flare_dx;
    qDebug() << "img.flare_dx:         " << img.flare_dx;
    qDebug() << "MAIN LEFT     MAIN RIGHT:            " << tempValue[MAIN_LEFT] << "    " << tempValue[MAIN_RIGHT];
    qDebug() << "SIDE_UP       SIDE_DOWN :            " << tempValue[SIDE_UP]   << "    " << tempValue[SIDE_DOWN];
    for(int i=0;i<4;i++)
    {
        tempList.push_back(make_pair<>(hList[i],tempValue[hList[i]]));
    }
    emit setHMotors(tempList);
}


void controller::endFlare()
{
    status.cnt.clear();
    // setZero();
    emit missionFinished(Flare);
    wait(10000);
    qDebug() << "EndFlare";
}

void controller::initDrop()
{
     loadConfig(HANG);
//      emit setGoal(global_deep - 0.6); // changed 0.4 ... FUCK YOU !!!!  Wang huigang
     emit setGoal(global_deep);
     updateConfig();

     setDeepCtr(true);


    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);

    status.ms = 200;
    emit missionStarted(Drop);
    setFrameInteval(status.ms);
    qDebug()<<"Pore Drop!";
}
void controller::ctrDrop()
{
    static bool flag_begin_arm_down = false;
    if (flag_begin_arm_down)
    {
        status.cnt[5]++;
        qDebug()<<"Arm Down!"<<status.cnt[5];
        if(status.cnt[5]==1)
        {
            emit setStepDown();
            qDebug()<<"Step Down"<<status.cnt[5];
        }
        if(status.cnt[5]>1 && status.cnt[5]<=3)
        {
            qDebug()<<step_flag;
            if(step_flag!=true)
            {
                emit setStepDown();
                qDebug()<<"Step Down"<<status.cnt[5];
            }
        }
        if(status.cnt[5]==5)
        {
            step_flag=0;
        }
    }

    qDebug()<<"Drop MainTask.5..5...5....5...5";

    visionClass::visionData && tmp = vision->getData();
    qDebug()<<"Drop MainTask";
    float tempValue[NUMBER_OF_MOTORS];

    // float pitch_rev=atan2(0.535,1.7-deep.value)*180/3.1415926;

    const static MOTORS hList[4]={MAIN_LEFT,MAIN_RIGHT,SIDE_UP,SIDE_DOWN};

    static int pushed_repeat_cnt = 0;
    static std::deque<double> deq_acos_theta1(1);

    const int pushed_repeat_cnt_max = 5;   //acos Hz: 1Hz, controller Hz: 5Hz
    const int deq_max_length = 5;


    if(acos.theta1 == acos.theta1Last)
    {
        pushed_repeat_cnt++;
    }
    else if(pushed_repeat_cnt > pushed_repeat_cnt_max || acos.theta1 != acos.theta1Last)
    {
        deq_acos_theta1.push_back(acos.theta1);
        qDebug() << "raw acos.theta1 pushed back: " << acos.theta1;
        pushed_repeat_cnt = 1;
    }

    acos.theta1Last = acos.theta1;

    if(deq_acos_theta1.size() > deq_max_length)
    {
        while (deq_acos_theta1.size() > deq_max_length) {
            deq_acos_theta1.pop_front();
        }
    }

    qDebug() << "deque elems:";
    for(int i = 0; i < deq_acos_theta1.size(); i++)
    {
        qDebug() << deq_acos_theta1.at(i);
    }
    qDebug();

    std::deque<double> tmp_deq_acos_theta1 = deq_acos_theta1;
    std::sort(tmp_deq_acos_theta1.begin(), tmp_deq_acos_theta1.end());

    double tmp_acos_theta1 = tmp_deq_acos_theta1.at((tmp_deq_acos_theta1.size()-1)/2>=0 ? (tmp_deq_acos_theta1.size()-1)/2 : 0);

    qDebug() << "Choose acos.theta1:   " << tmp_acos_theta1;


    static bool flag_start_recognzing_drum = true;

    static bool flag_first_acos = true;

    if(tmp.m2_drum_dx!=999 && tmp.m2_drum_dy!=999)
    {
        status.cnt[0] = 0;

        loadConfig(LOCATE_BOTTOM);
        qDebug()<<"Found the Drum, Now Recognizing";

        flag_first_acos = false;
        if(flag_start_recognzing_drum)
        {
            sbg.goal = sbg.yaw;
            flag_start_recognzing_drum = false;
        }
        flag_begin_arm_down = true;


        float sbgError=sbg.goal-sbg.yaw;
        if(sbgError > 180)
        {
            sbgError = -360+sbgError;
        }
        else if(sbgError < -180)
        {
            sbgError = 360+sbgError;

        }
        qDebug() << "Error" << sbgError;

        float sbgDiff = sbgError-sbg.yawErrorLast;
        sbg.yawErrorLast = sbgError;


        img.t_last = img.t_now;
        img.t_now = tmp.t_now;

        img.drum_dx_last = img.drum_dx;
        img.drum_dy_last = img.drum_dy;

        img.drum_dy = tmp.m2_drum_dx/40.0;
        img.drum_dx = -tmp.m2_drum_dy/40.0;

        img.drum_dx_diff = img.drum_dx-img.drum_dx_last;
        img.drum_dy_diff = img.drum_dy-img.drum_dy_last;


        if(tmp.m2_hand_in == 1)
        {
            status.cnt[1]++;
        }
        else if(abs(img.drum_dx)<37/10.0 && abs(img.drum_dy)<37/10.0 && abs(img.drum_dx_diff)<37.0/10 && abs(img.drum_dy_diff)<37/10.0)
        {
            status.cnt[1]++;
            qDebug() << "Recognized,Now Drop" << status.cnt[1]++;
        }
        else
        {
            qDebug() << "Not Recognized";
            if(status.cnt[1] > 0)
            {
                status.cnt[1]--;
            }
            else
            {
                status.cnt[1] = 0;
            }
        }

        if(status.cnt[1] >= 35)
        {
            tempValue[MAIN_LEFT]=0;
            tempValue[MAIN_RIGHT]=0;
            tempValue[SIDE_UP]=0;
            tempValue[SIDE_DOWN]=0;

            qDebug() << "begin to open the Sevo";
            emit setSevoOpen();
            status.cnt[6]++;
            if(status.cnt[6] >= 5)
            {
                emit endTask();
            }
        }

        // const int max_main_speed = 50;
        const int max_main_speed = drum_max_main_speed;
        const int max_side_speed = 40;

        if(img.drum_dy > 0)
        {
            tempValue[MAIN_LEFT] = 20*((img.drum_dy)/fabs(img.drum_dy)) + status.val[MAIN_LEFT].p*img.drum_dy + status.val[MAIN_LEFT].d*img.drum_dy_diff;
            tempValue[MAIN_RIGHT] = 20*((img.drum_dy)/fabs(img.drum_dy)) + status.val[MAIN_LEFT].p*img.drum_dy + status.val[MAIN_LEFT].d*img.drum_dy_diff;
        }
        else if(img.drum_dy < 0)
        {
            tempValue[MAIN_LEFT] = 4*((img.drum_dy)/fabs(img.drum_dy)) + status.val[MAIN_LEFT].p*img.drum_dy + status.val[MAIN_LEFT].d*img.drum_dy_diff;
            tempValue[MAIN_RIGHT] = 6*((img.drum_dy)/fabs(img.drum_dy)) + status.val[MAIN_LEFT].p*img.drum_dy + status.val[MAIN_LEFT].d*img.drum_dy_diff;
        }
        else
        {
            tempValue[MAIN_LEFT] = 0 + status.val[MAIN_LEFT].p*img.drum_dy + status.val[MAIN_LEFT].d*img.drum_dy_diff;
            tempValue[MAIN_RIGHT] = 0 + status.val[MAIN_LEFT].p*img.drum_dy + status.val[MAIN_LEFT].d*img.drum_dy_diff;
        }

        if (img.drum_dx > 0)
        {
            tempValue[SIDE_UP] = 10*((img.drum_dx)/fabs(img.drum_dx)) + status.val[SIDE_UP].p*img.drum_dx + status.val[SIDE_UP].d*img.drum_dx_diff;
            tempValue[SIDE_DOWN] = 4*((img.drum_dx)/fabs(img.drum_dx)) + status.val[SIDE_DOWN].p*img.drum_dx + status.val[SIDE_DOWN].d*img.drum_dx_diff;
        }
        else if (img.drum_dx < 0)
        {
            tempValue[SIDE_UP] = 4*((img.drum_dx)/fabs(img.drum_dx)) + status.val[SIDE_UP].p*img.drum_dx + status.val[SIDE_UP].d*img.drum_dx_diff;
            tempValue[SIDE_DOWN] = 6*((img.drum_dx)/fabs(img.drum_dx)) + status.val[SIDE_DOWN].p*img.drum_dx + status.val[SIDE_DOWN].d*img.drum_dx_diff;
        }
        else
        {
            tempValue[SIDE_UP] = 0 + status.val[SIDE_UP].p*img.drum_dx + status.val[SIDE_UP].d*img.drum_dx_diff;
            tempValue[SIDE_DOWN] = 0 + status.val[SIDE_DOWN].p*img.drum_dx + status.val[SIDE_DOWN].d*img.drum_dx_diff;
        }

        /********the version could reach a well result*********
        if(img.drum_dy != 0)
        {
            tempValue[MAIN_LEFT] = 8*((img.drum_dy)/fabs(img.drum_dy)) + status.val[MAIN_LEFT].p*img.drum_dy + status.val[MAIN_LEFT].d*img.drum_dy_diff;
            tempValue[MAIN_RIGHT] = 8*((img.drum_dy)/fabs(img.drum_dy)) + status.val[MAIN_LEFT].p*img.drum_dy + status.val[MAIN_LEFT].d*img.drum_dy_diff;
        }
        else
        {
            tempValue[MAIN_LEFT] = 0 + status.val[MAIN_LEFT].p*img.drum_dy + status.val[MAIN_LEFT].d*img.drum_dy_diff;
            tempValue[MAIN_RIGHT] = 0 + status.val[MAIN_LEFT].p*img.drum_dy + status.val[MAIN_LEFT].d*img.drum_dy_diff;
        }

        if (img.drum_dx > 0)
        {
            tempValue[SIDE_UP] = 12*((img.drum_dx)/fabs(img.drum_dx)) + status.val[SIDE_UP].p*img.drum_dx + status.val[SIDE_UP].d*img.drum_dx_diff;
            tempValue[SIDE_DOWN] = 6*((img.drum_dx)/fabs(img.drum_dx)) + status.val[SIDE_DOWN].p*img.drum_dx + status.val[SIDE_DOWN].d*img.drum_dx_diff;
        }
        else if (img.drum_dx < 0)
        {
            tempValue[SIDE_UP] = 6*((img.drum_dx)/fabs(img.drum_dx)) + status.val[SIDE_UP].p*img.drum_dx + status.val[SIDE_UP].d*img.drum_dx_diff;
            tempValue[SIDE_DOWN] = 6*((img.drum_dx)/fabs(img.drum_dx)) + status.val[SIDE_DOWN].p*img.drum_dx + status.val[SIDE_DOWN].d*img.drum_dx_diff;
        }
        else
        {
            tempValue[SIDE_UP] = 0 + status.val[SIDE_UP].p*img.drum_dx + status.val[SIDE_UP].d*img.drum_dx_diff;
            tempValue[SIDE_DOWN] = 0 + status.val[SIDE_DOWN].p*img.drum_dx + status.val[SIDE_DOWN].d*img.drum_dx_diff;
        }
        */


        /** Didn't consider the different between different motors **/
//        if(img.drum_dy != 0)
//        {
//            tempValue[MAIN_LEFT] = 10*((img.drum_dy)/fabs(img.drum_dy)) + status.val[MAIN_LEFT].p*img.drum_dy + status.val[MAIN_LEFT].d*img.drum_dy_diff;
//            tempValue[MAIN_RIGHT] = 10*((img.drum_dy)/fabs(img.drum_dy)) + status.val[MAIN_LEFT].p*img.drum_dy + status.val[MAIN_LEFT].d*img.drum_dy_diff;
//        }
//        else
//        {
//            tempValue[MAIN_LEFT] = 0 + status.val[MAIN_LEFT].p*img.drum_dy + status.val[MAIN_LEFT].d*img.drum_dy_diff;
//            tempValue[MAIN_RIGHT] = 0 + status.val[MAIN_LEFT].p*img.drum_dy + status.val[MAIN_LEFT].d*img.drum_dy_diff;
//        }

//        if (img.drum_dx > 0)
//        {
//            tempValue[SIDE_UP] = 20*((img.drum_dx)/fabs(img.drum_dx)) + status.val[SIDE_UP].p*img.drum_dx + status.val[SIDE_UP].d*img.drum_dx_diff;
//            tempValue[SIDE_DOWN] = 20*((img.drum_dx)/fabs(img.drum_dx)) + status.val[SIDE_DOWN].p*img.drum_dx + status.val[SIDE_DOWN].d*img.drum_dx_diff;
//        }
//        else if (img.drum_dx < 0)
//        {
//            tempValue[SIDE_UP] = 15*((img.drum_dx)/fabs(img.drum_dx)) + status.val[SIDE_UP].p*img.drum_dx + status.val[SIDE_UP].d*img.drum_dx_diff;
//            tempValue[SIDE_DOWN] = 15*((img.drum_dx)/fabs(img.drum_dx)) + status.val[SIDE_DOWN].p*img.drum_dx + status.val[SIDE_DOWN].d*img.drum_dx_diff;
//        }
//        else
//        {
//            tempValue[SIDE_UP] = 0 + status.val[SIDE_UP].p*img.drum_dx + status.val[SIDE_UP].d*img.drum_dx_diff;
//            tempValue[SIDE_DOWN] = 0 + status.val[SIDE_DOWN].p*img.drum_dx + status.val[SIDE_DOWN].d*img.drum_dx_diff;
//        }


        /** Add the sbg info **/
//        tempValue[MAIN_LEFT] += (status.val[MAIN_LEFT].p*sbgError + status.val[MAIN_LEFT].d*sbgDiff);
//        tempValue[MAIN_RIGHT] -= (status.val[MAIN_RIGHT].p*sbgError + status.val[MAIN_RIGHT].d*sbgDiff);
//        tempValue[SIDE_UP] -= (status.val[SIDE_UP].p*sbgError + status.val[SIDE_UP].d*sbgDiff);
//        tempValue[SIDE_DOWN] += (status.val[SIDE_DOWN].p*sbgError + status.val[SIDE_DOWN].d*sbgDiff);


        qDebug() << "img.drum_dx: " << img.drum_dx;
        qDebug() << "img.drum_dy: " << img.drum_dy;
        qDebug() << "tempValue[MAIN_LEFT] tempValue[MAIN_RIGHT] tempValue[SIDE_UP] tempValue[SIDE_DOWN]" << tempValue[MAIN_LEFT] << " " << tempValue[MAIN_LEFT] << " " << tempValue[SIDE_UP] << " " << tempValue[SIDE_DOWN];

        for(int i = 0; i < 2; i++)
        {
            if(tempValue[hList[i]] >= max_main_speed)
            {
                tempValue[hList[i]] = max_main_speed;
            }
            else if(tempValue[hList[i]] <= -max_main_speed)
            {
                tempValue[hList[i]] = -max_main_speed;
            }
        }
        for(int i = 2; i < 4; i++)
        {
            if(tempValue[hList[i]] >= max_side_speed)
            {
                tempValue[hList[i]] = max_side_speed;
            }
            else if(tempValue[hList[i]] <= -max_side_speed)
            {
                tempValue[hList[i]] = -max_side_speed;
            }
        }
    }
    else if(img.drum_dx_last  !=999 && img.drum_dy_last != 999 && status.cnt[0] <= 25)
    {
        status.cnt[0]++;

        loadconfig(locate_bottom);
        qdebug()<<"found the drum, now use vision recognizing";

        img.t_last = img.t_now;
        img.t_now = tmp.t_now;

        img.drum_dy = img.drum_dx_last;
        img.drum_dx = img.drum_dy_last;

        img.drum_dx_diff = img.drum_dx-img.drum_dx_last;
        img.drum_dy_diff = img.drum_dy-img.drum_dy_last;


        const int max_main_speed = drum_max_main_speed;
        const int max_side_speed = 40;

        /** Didn't consider the different between different motors **/
        if(img.drum_dy != 0)
        {
            tempValue[MAIN_LEFT] = 10*((img.drum_dy)/fabs(img.drum_dy)) + status.val[MAIN_LEFT].p*img.drum_dy + status.val[MAIN_LEFT].d*img.drum_dy_diff;
            tempValue[MAIN_RIGHT] = 10*((img.drum_dy)/fabs(img.drum_dy)) + status.val[MAIN_LEFT].p*img.drum_dy + status.val[MAIN_LEFT].d*img.drum_dy_diff;
        }
        else
        {
            tempValue[MAIN_LEFT] = 0 + status.val[MAIN_LEFT].p*img.drum_dy + status.val[MAIN_LEFT].d*img.drum_dy_diff;
            tempValue[MAIN_RIGHT] = 0 + status.val[MAIN_LEFT].p*img.drum_dy + status.val[MAIN_LEFT].d*img.drum_dy_diff;
        }

        if(img.drum_dx != 0)
        {
            tempValue[SIDE_UP] = 10*((img.drum_dx)/fabs(img.drum_dx)) + status.val[SIDE_UP].p*img.drum_dx + status.val[SIDE_UP].d*img.drum_dx_diff;
            tempValue[SIDE_DOWN] = 10*((img.drum_dx)/fabs(img.drum_dx)) + status.val[SIDE_DOWN].p*img.drum_dx + status.val[SIDE_DOWN].d*img.drum_dx_diff;
        }
        else
        {
            tempValue[SIDE_UP] = 0 + status.val[SIDE_UP].p*img.drum_dx + status.val[SIDE_UP].d*img.drum_dx_diff;
            tempValue[SIDE_DOWN] = 0 + status.val[SIDE_DOWN].p*img.drum_dx + status.val[SIDE_DOWN].d*img.drum_dx_diff;
        }

        qDebug() << "img.drum_dx: " << img.drum_dx;
        qDebug() << "img.drum_dy: " << img.drum_dy;
        qDebug() << "tempValue[MAIN_LEFT] tempValue[MAIN_RIGHT] tempValue[SIDE_UP] tempValue[SIDE_DOWN]" << tempValue[MAIN_LEFT] << " " << tempValue[MAIN_LEFT] << " " << tempValue[SIDE_UP] << " " << tempValue[SIDE_DOWN];

        for(int i = 0; i < 2; i++)
        {
            if(tempValue[hList[i]] >= max_main_speed)
            {
                tempValue[hList[i]] = max_main_speed;
            }
            else if(tempValue[hList[i]] <= -max_main_speed)
            {
                tempValue[hList[i]] = -max_main_speed;
            }
        }
        for(int i = 2; i < 4; i++)
        {
            if(tempValue[hList[i]] >= max_side_speed)
            {
                tempValue[hList[i]] = max_side_speed;
            }
            else if(tempValue[hList[i]] <= -max_side_speed)
            {
                tempValue[hList[i]] = -max_side_speed;
            }
        }


    }
    else if(status.cnt[0] >25)
    {
        img.drum_dx_last = 999;
        img.drum_dy_last = 999;

        loadConfig(FORWARD_ACOS);
        qDebug() << "AcosRevise";

        static double tmp_acos_theta1_last = 0;
        if(tmp_acos_theta1 - tmp_acos_theta1_last != 0)
        {
            tmp_acos_theta1_last = tmp_acos_theta1;

            qDebug() << "use acos.theta1:  " << tmp_acos_theta1;

            float acosError = 90 - tmp_acos_theta1;
            if(acosError > 180)
            {
                acosError = -360 + acosError;
            }
            else if(acosError < -180)
            {
                acosError = 360 + acosError;
            }

            sbg.goal = sbg.yaw+acosError;

        }

        float sbgError = sbg.goal-sbg.yaw;
        if(sbgError > 180)
        {
            sbgError = -360+sbgError;
        }
        if(sbgError < -180)
        {
            sbgError = 360+sbgError;
        }

        float sbgDiff = sbgError-sbg.yawErrorLast;
        sbg.yawErrorLast = sbgError;


        // const int max_main_speed = 37;
        const int max_main_speed = acos_max_main_speed;
        const int max_side_speed = 35;

        qDebug() << "sbgError                                                                sbgError:" << sbgError;
        if (sbgError < 0)
        {
            tempValue[MAIN_LEFT] = max_main_speed + status.val[MAIN_LEFT].p*sbgError + status.val[MAIN_LEFT].d*sbgDiff;
            tempValue[MAIN_RIGHT] = max_main_speed;

            tempValue[SIDE_UP] = 25 - status.val[SIDE_UP].p*sbgError - status.val[SIDE_DOWN].d*sbgDiff;
            tempValue[SIDE_DOWN] = -15 + status.val[SIDE_UP].p*sbgError + status.val[SIDE_DOWN].d*sbgDiff;
        }
        else if (sbgError >= 0)
        {
            tempValue[MAIN_LEFT] = max_main_speed;
            tempValue[MAIN_RIGHT] = max_main_speed - status.val[MAIN_RIGHT].p*sbgError - status.val[MAIN_RIGHT].d*sbgDiff;

            tempValue[SIDE_UP] = -25 - status.val[SIDE_UP].p*sbgError - status.val[SIDE_DOWN].d*sbgDiff;
            tempValue[SIDE_DOWN] = 15 + status.val[SIDE_UP].p*sbgError + status.val[SIDE_DOWN].d*sbgDiff;
        }

        for(int i = 0; i < 2; i++)
        {
            if(tempValue[hList[i]] >= max_main_speed)
            {
                tempValue[hList[i]] = max_main_speed;
            }
            else if(tempValue[hList[i]] <= -max_main_speed)
            {
                tempValue[hList[i]] = -max_main_speed;
            }
        }
        for(int i = 2; i < 4; i++)
        {
            if(tempValue[hList[i]] >= max_side_speed)
            {
                tempValue[hList[i]] = max_side_speed;
            }
            else if(tempValue[hList[i]] <= -max_side_speed)
            {
                tempValue[hList[i]] = -max_side_speed;
            }
        }

    }
    else if(status.cnt[4] >= 20 && flag_first_acos == true)
    {
        loadConfig(FORWARD_ACOS);
        qDebug() << "AcosRevise";


        flag_start_recognzing_drum = true;


        static double tmp_acos_theta1_last = 0;
        if(tmp_acos_theta1 - tmp_acos_theta1_last != 0)
        {
            tmp_acos_theta1_last = tmp_acos_theta1;

            qDebug() << "use acos.theta1:  " << tmp_acos_theta1;

            float acosError = 90 - tmp_acos_theta1;
            if(acosError > 180)
            {
                acosError = -360 + acosError;
            }
            else if(acosError < -180)
            {
                acosError = 360 + acosError;
            }

            sbg.goal = sbg.yaw+acosError;

        }

        float sbgError = sbg.goal-sbg.yaw;
        if(sbgError > 180)
        {
            sbgError = -360+sbgError;
        }
        if(sbgError < -180)
        {
            sbgError = 360+sbgError;
        }

        float sbgDiff = sbgError-sbg.yawErrorLast;
        sbg.yawErrorLast = sbgError;


        // const int max_main_speed = 37;
        const int max_main_speed = acos_max_main_speed;
        const int max_side_speed = 35;

        qDebug() << "sbgError:" << sbgError;
        if (sbgError < 0)
        {
            tempValue[MAIN_LEFT] = max_main_speed + status.val[MAIN_LEFT].p*sbgError + status.val[MAIN_LEFT].d*sbgDiff;
            tempValue[MAIN_RIGHT] = max_main_speed;

            tempValue[SIDE_UP] = 15 - status.val[SIDE_UP].p*sbgError - status.val[SIDE_DOWN].d*sbgDiff;
            tempValue[SIDE_DOWN] = -15 + status.val[SIDE_UP].p*sbgError + status.val[SIDE_DOWN].d*sbgDiff;
        }
        else if (sbgError >= 0)
        {
            tempValue[MAIN_LEFT] = max_main_speed;
            tempValue[MAIN_RIGHT] = max_main_speed - status.val[MAIN_RIGHT].p*sbgError - status.val[MAIN_RIGHT].d*sbgDiff;

            tempValue[SIDE_UP] = -15 - status.val[SIDE_UP].p*sbgError - status.val[SIDE_DOWN].d*sbgDiff;
            tempValue[SIDE_DOWN] = 15 + status.val[SIDE_UP].p*sbgError + status.val[SIDE_DOWN].d*sbgDiff;
        }

        for(int i = 0; i < 2; i++)
        {
            if(tempValue[hList[i]] >= max_main_speed)
            {
                tempValue[hList[i]] = max_main_speed;
            }
            else if(tempValue[hList[i]] <= -max_main_speed)
            {
                tempValue[hList[i]] = -max_main_speed;
            }
        }
        for(int i = 2; i < 4; i++)
        {
            if(tempValue[hList[i]] >= max_side_speed)
            {
                tempValue[hList[i]] = max_side_speed;
            }
            else if(tempValue[hList[i]] <= -max_side_speed)
            {
                tempValue[hList[i]] = -max_side_speed;
            }
        }
    }
    else
    {
        status.cnt[4]++;

        emit setGoal(global_deep);
        loadConfig(HANG);
        updateConfig();

        tempValue[MAIN_LEFT] = 0;
        tempValue[MAIN_RIGHT] = 0;
        tempValue[SIDE_UP] = 0;
        tempValue[SIDE_DOWN] = 0;
        qDebug()<<"Backward to leave the bar!";
    }

    QList<pair<MOTORS,float>> tempList;
    for(int i = 0; i < 4; i++)
    {
        tempList.push_back(make_pair<>(hList[i],tempValue[hList[i]]));
    }
    emit setHMotors(tempList);
}

void controller::endDrop()
{
    status.cnt.clear();
    setZero();
    setDeepCtr(false);
    emit missionFinished(Drop);
    wait(2000);
    qDebug() << "EndDrop";
}

void controller::initAcquire()
{
    emit setGoal(global_deep);
    loadConfig(HANG);
    updateConfig();
    setDeepCtr(true);
    status.cnt.push_back(0);
    // status.ActionList << BACKWARDACTION
    status.ActionList << ACOSREVISE
                      << BALLREVISE;

    ActionParaStack.push({BALLREVISE,5,0,REPLAY});
    ActionParaStack.push({ACOSREVISE,0,0,NO});
    // ActionParaStack.push({BACKWARDACTION,150,0,NO});

    status.ms = 200;
    tried = 0;
    this->setActionList(status.ActionList);
    emit missionStarted(Acquire);
    setFrameInteval(status.ms);
    qDebug() << "Pore Acquire!";
}

void controller::ctrAcquire()
{
    status.cnt[0]++;
    qDebug()<<"Acquire Main Task";
    // emit enterAction(BACKWARDACTION);
    emit enterAction(ACOSREVISE);
}

void controller::endAcquire()
{
    status.cnt.clear();
    setZero();
    setDeepCtr(false);
    emit missionFinished(Acquire);
    qDebug() << "endAcquire";
}

void controller::initForward_SBG()
{
    wait(40000);
    // emit onSetDeepRev(-deep.value);

    init_deep = deep.value;
    forward_deep += deep.value;
    global_deep += deep.value;

    // print yaw to txt file
    time_t clock0=std::time(NULL);
    std::tm * yaw_localTime=std::localtime(&clock0);
    QString yaw_filename = "yaw_out";
    yaw_filename+="-";
    yaw_filename+=QString::number(yaw_localTime->tm_year-2000+1900);
    yaw_filename+="-";
    yaw_filename+=QString::number(yaw_localTime->tm_mon+1);
    yaw_filename+="-";
    yaw_filename+=QString::number(yaw_localTime->tm_mday);
    yaw_filename+=".txt";
    outfile_yaw.open("/home/nwpu/"+yaw_filename.toStdString(), std::ios_base::out);
    qDebug() << yaw_filename << endl;

    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.ms = 200;
    emit setGoal(forward_deep);
    loadConfig(HANG);
    updateConfig();
    setFrameInteval(status.ms);
    qDebug()<<"PID_Test";
}

void controller::ctrForward_SBG()
{
    qDebug() << "foward_deep: foward_deep: foward_deep: foward_deep: foward_deep" << forward_deep;
    qDebug() << "init_deep: init_deep: init_deep: init_deep: init_deep" << init_deep;



    status.cnt[0]++;
    qDebug()<<"AutoDeep"<<status.cnt[0];
    qDebug()<<"deep"<<deep.value;
    // qDebug() << sbg.yaw;
    outfile_yaw << sbg.yaw << std::endl;

    /*
    if(abs(forward_deep-deep.value) < 0.05)
    {
        if(status.cnt[0]>=15)
        {
            status.cnt[1]++;
            qDebug()<<"Deep Complete";
        }
    }
    */

    if(deep.value - init_deep >= 0.20)
    {
        emit setGoal(forward_deep);
        loadConfig(HANG);
        updateConfig();

        status.cnt[2]++;
        if(status.cnt[2]==1)
        {
            if(set_yaw_target != -999)
            {
                sbg.goal = set_yaw_target;
            }
            else
            {
                sbg.goal = sbg.yaw;
            }
        }

        loadConfig(FORWARD_SLOW);
        qDebug() << "Forward_SBG MainTask" << status.cnt[2];

        const static MOTORS hList[4] = {MAIN_LEFT, MAIN_RIGHT, SIDE_UP, SIDE_DOWN};

        const int max_main_speed = 75;
        const int max_side_speed = 30;

        float tempValue[NUMBER_OF_MOTORS];

        float sbgError = sbg.goal-sbg.yaw;
        if(sbgError > 180)
        {
            sbgError = -360+sbgError;
        }
        if(sbgError < -180)
        {
            sbgError = 360+sbgError;
        }
        qDebug()<<"Error"<<sbgError;


        float sbgDiff=sbgError-sbg.yawErrorLast;
        sbg.yawErrorLast=sbgError;

        qDebug() << sbgError;

        if (sbgError < 0)
        {
            tempValue[MAIN_LEFT] = max_main_speed + status.val[MAIN_LEFT].p*sbgError + status.val[MAIN_LEFT].d*sbgDiff;
            tempValue[MAIN_RIGHT] = max_main_speed;

            tempValue[SIDE_UP] = 15 - status.val[SIDE_UP].p*sbgError - status.val[SIDE_DOWN].d*sbgDiff;
            tempValue[SIDE_DOWN] = -10 + status.val[SIDE_UP].p*sbgError + status.val[SIDE_DOWN].d*sbgDiff;
        }
        else if (sbgError >= 0)
        {
            tempValue[MAIN_LEFT] = max_main_speed;
            tempValue[MAIN_RIGHT] = max_main_speed - status.val[MAIN_RIGHT].p*sbgError - status.val[MAIN_RIGHT].d*sbgDiff;

            tempValue[SIDE_UP] = -10 - status.val[SIDE_UP].p*sbgError - status.val[SIDE_DOWN].d*sbgDiff;
            tempValue[SIDE_DOWN] = 10 + status.val[SIDE_UP].p*sbgError + status.val[SIDE_DOWN].d*sbgDiff;
        }

        for(int i = 0; i < 2; i++)
        {
            if(tempValue[hList[i]] >= max_main_speed)
            {
                tempValue[hList[i]] = max_main_speed;
            }
            else if(tempValue[hList[i]] <= -max_main_speed)
            {
                tempValue[hList[i]] = -max_main_speed;
            }
        }
        for(int i = 2; i < 4; i++)
        {
            if(tempValue[hList[i]] >= max_side_speed)
            {
                tempValue[hList[i]] = max_side_speed;
            }
            else if(tempValue[hList[i]] <= -max_side_speed)
            {
                tempValue[hList[i]] = -max_side_speed;
            }
        }

        qDebug() << "tempValue[MAIN_LEFT]:    " << tempValue[MAIN_LEFT];
        qDebug() << "tempValue[MAIN_RIGHT]:   " << tempValue[MAIN_RIGHT];
        qDebug() << "tempValue[SIDE_UP]:      " << tempValue[SIDE_UP];
        qDebug() << "tempValue[SIDE_DOWN]:    " << tempValue[SIDE_DOWN];


        QList<pair<MOTORS,float>> tempList;
        for(int i = 0; i < 4; i++)
        {
            tempList.push_back(make_pair<>(hList[i],tempValue[hList[i]]));
        }
        emit setHMotors(tempList);

        qDebug() << "Time:" << status.cnt[2];
        if(status.cnt[2]>=300)       //working during 60s
        {

            tempValue[MAIN_LEFT] = 0;
            tempValue[MAIN_RIGHT] = 0;
            tempValue[SIDE_UP] = 0;
            tempValue[SIDE_DOWN] = 0;
            emit endTask();
        }
    }
}

void controller::endForward_SBG()
{
    outfile_yaw.close();

    status.cnt.clear();
    setZero();
    qDebug()<<"End Forward_SBG";
}

void controller::initBackward_SBG()
{
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.ms = 200;
    sbg.goal = sbg.yaw;
    emit setGoal(0);
    loadConfig(HANG);
    updateConfig();
    setFrameInteval(status.ms);
    qDebug()<<"Backward_SBG";
}
void controller::ctrBackward_SBG()
{
    status.cnt[0]++;
    qDebug()<<"AutoDeep"<<status.cnt[0];
    qDebug()<<"deep"<<deep.value;
    if(abs(0-deep.value)<0.05)
    {
        status.cnt[1]++;
        qDebug()<<"Deep Complete";
    }
    if(status.cnt[1]>=10)
    {
        status.cnt[2]++;
    qDebug()<<"Backward_SBG MainTask"<<status.cnt[2];
    const static MOTORS hList[4]={MAIN_LEFT,MAIN_RIGHT,SIDE_UP,SIDE_DOWN};
    float tempValue[NUMBER_OF_MOTORS];
    float sbgError=sbg.goal-sbg.yaw;
    if(sbgError>180){
        sbgError=-360+sbgError;
    }
    if(sbgError<-180){
        sbgError=360+sbgError;
    }
    qDebug()<<"Error"<<sbgError;
    float sbgDiffT=(sbg.tNow-sbg.tLast)/1000.0;
    float sbgDiff=sbgError-sbg.yawErrorLast;
    sbg.yawErrorLast=sbgError;
    tempValue[MAIN_LEFT]=-32+status.val[MAIN_LEFT].p*sbgError/100.0
             +status.val[MAIN_LEFT].d*sbgDiff/sbgDiffT;
    tempValue[MAIN_RIGHT]=-32+status.val[MAIN_RIGHT].p*sbgError/100.0
             +status.val[MAIN_RIGHT].d*sbgDiff/sbgDiffT;

    if(abs((int)sbgError)>=55){
         tempValue[SIDE_UP]=status.val[SIDE_UP].p*sbgError/100.0
                  +status.val[SIDE_UP].d*sbgDiff/sbgDiffT;
         tempValue[SIDE_DOWN]=status.val[SIDE_DOWN].p*sbgError/100.0
                  +status.val[SIDE_DOWN].d*sbgDiff/sbgDiffT;
     }

    QList<pair<MOTORS,float>> tempList;
    for(int i=0;i<4;i++){
        if(tempValue[hList[i]]>=75){
            tempValue[hList[i]]=75;
        }
        else if(tempValue[hList[i]]<=-75){
            tempValue[hList[i]]=-75;
        }
    }
    for(int i=0;i<4;i++){
        tempList.push_back(make_pair<>(hList[i],tempValue[hList[i]]));
    }
    emit setHMotors(tempList);
    }
}
void controller::endBackward_SBG()
{
    status.cnt.clear();
    setZero();
    qDebug()<<"End Backward_SBG";
}
void controller::initRotate_SBG()
{
    status.cnt.push_back(0);
    status.ms = 200;
    sbg.goal = 90+sbg.yaw;
    loadConfig(HANG);
    emit setGoal(0);
    updateConfig();
    setFrameInteval(status.ms);
    qDebug()<<"Rotate_SBG";
}
void controller::ctrRotate_SBG()
{
    status.cnt[0]++;
    qDebug()<<"AutoDeep";
    qDebug()<<"deep"<<deep.value;
    if(abs(0-deep.value)<0.05)
    {
        status.cnt[1]++;
        qDebug()<<"Deep Complete";
    }
    if(status.cnt[1]>=10)
    {
    loadConfig(ROTATING);
    status.cnt[2]++;
    qDebug()<<"Rotate_SBG MainTask"<<status.cnt[2];
    const static MOTORS hList[4]={MAIN_LEFT,MAIN_RIGHT,SIDE_UP,SIDE_DOWN};
    float tempValue[NUMBER_OF_MOTORS];
    float sbgError=sbg.goal-sbg.yaw;
    if(sbgError>180){
        sbgError=-360+sbgError;
    }
    if(sbgError<-180){
        sbgError=360+sbgError;
    }
    qDebug()<<"Error"<<sbgError;
    float sbgDiffT=(sbg.tNow-sbg.tLast)/1000.0;
    float sbgDiff=sbgError-sbg.yawErrorLast;
    sbg.yawErrorLast=sbgError;
    if(abs((int)sbgError)>=5){
         tempValue[SIDE_UP]=status.val[SIDE_UP].p*sbgError/100.0
                  +status.val[SIDE_UP].d*sbgDiff/sbgDiffT;
         tempValue[SIDE_DOWN]=status.val[SIDE_DOWN].p*sbgError/100.0
                  +status.val[SIDE_DOWN].d*sbgDiff/sbgDiffT;
         tempValue[MAIN_LEFT]=0;
         tempValue[MAIN_RIGHT]=0;
     }
    else
    {
        tempValue[SIDE_UP]=0;
        tempValue[SIDE_DOWN]=0;
        tempValue[MAIN_LEFT]=0;
        tempValue[MAIN_RIGHT]=0;
        emit endTask();
    }
    for(int i=0;i<4;i++){
        if(tempValue[hList[i]]>=40){
            tempValue[hList[i]]=40;
        }
        else if(tempValue[hList[i]]<=-40){
            tempValue[hList[i]]=-40;
        }
    }
    QList<pair<MOTORS,float>> tempList;
    for(int i=0;i<4;i++){
        tempList.push_back(make_pair<>(hList[i],tempValue[hList[i]]));
    }
    emit setHMotors(tempList);
    }
}
void controller::endRotate_SBG()
{
    status.cnt.clear();
    setZero();
    qDebug()<<"End Rotate_SBG";
}
void controller::initHang_SBG()
{
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.ms = 200;
    emit setGoal(1);
    loadConfig(HANG);
    updateConfig();
    setFrameInteval(status.ms);
    qDebug()<<"Hang_SBG";
}
void controller::ctrHang_SBG()
{
    status.cnt[0]++;
    qDebug()<<"HANG_SBG MainTask"<<status.cnt[0];
    qDebug()<<"deep"<<deep.value;
    emit setGoal(1.0);
    loadConfig(HANG);
    updateConfig();
    if(abs(1-deep.value)<0.05)
    {
        qDebug()<<"Deep Target"<<1;
        status.cnt[1]++;
        //if(status.cnt[1]>=20)emit endTask();
    }
    else status.cnt[1]=0;
    if(status.cnt[0]>=300)emit endTask();
    /*
    if(status.cnt[1]>=30)
    {
        qDebug()<<"Deep Target"<<0.5;
        emit setGoal(0.5);
        loadConfig(HANG);
        updateConfig();
    }
    */
}
void controller::endHang_SBG()
{
    status.cnt.clear();
    setZero();
    qDebug()<<"End Rotate_SBG";
}
void controller::initGetPhotoF()
{
    status.cnt.push_back(0);
    status.ms = 1000;
    emit setGoal(1.0);
    loadConfig(HANG);
    updateConfig();
    setFrameInteval(status.ms);
    qDebug()<<"Get PhotoF";
}

void controller::ctrGetPhotoF()
{
    status.cnt[0]++;
    if(1-deep.value<0.05)
    {
        status.cnt[1]++;
        qDebug()<<"GetPhotoF MainTask"<<status.cnt[1];
        emit SavePhotoF(true);
    }
}

void controller::endGetPhotoF()
{
    status.cnt.clear();
    setZero();
    emit SavePhotoF(false);
    qDebug()<<"End Get PhotoF";
}

void controller::initGetPhotoB()
{
    status.cnt.push_back(0);
    status.ms = 1000;
//    emit setGoal(1);
//    loadConfig(HANG);
//    updateConfig();
    setFrameInteval(status.ms);
    qDebug()<<"Get PhotoB";
}

void controller::ctrGetPhotoB()
{
    status.cnt[0]++;
    qDebug()<<"GetPhotoB MainTask"<<status.cnt[0];
    emit SavePhotoB(true);
}

void controller::endGetPhotoB()
{
    status.cnt.clear();
    setZero();
    emit SavePhotoB(false);
    qDebug()<<"End Get PhotoB";
}

void controller::initSwing_SBG()
{
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    //emit setGoal(0.0);
    //loadConfig(HANG);
    //updateConfig();
    status.ms = 200;
    //sbg.goal = sbg.yaw;
    setFrameInteval(status.ms);
    qDebug()<<"SWING_SBG";
}

void controller::ctrSwing_SBG()
{
    status.cnt[0]++;
    qDebug()<<"AutoDeep"<<status.cnt[0];
    qDebug()<<"deep"<<deep.value;
    if(abs(0-deep.value)<0.05)
    {
        status.cnt[1]++;
        qDebug()<<"Deep Complete";
    }
    if(status.cnt[1]>=10)
    {
        loadConfig(SWING);
        status.cnt[2]++;
        if(status.cnt[2]==1)sbg.goal=sbg.yaw;
        qDebug()<<"Swing_SBG Maintask"<<status.cnt[2];
        const static MOTORS hList[4]={MAIN_LEFT,MAIN_RIGHT,SIDE_UP,SIDE_DOWN};
        float tempValue[NUMBER_OF_MOTORS];
        float sbgError=sbg.goal-sbg.yaw;
        if(sbgError>180){
            sbgError=-360+sbgError;
        }
        if(sbgError<-180){
            sbgError=360+sbgError;
        }
        qDebug()<<"Error"<<sbgError;
        float sbgDiffT=(sbg.tNow-sbg.tLast)/1000.0;
        float sbgDiff=sbgError-sbg.yawErrorLast;
        sbg.yawErrorLast=sbgError;
        tempValue[SIDE_UP]=50+status.val[SIDE_UP].p*sbgError/100.0
                 +status.val[SIDE_UP].d*sbgDiff/sbgDiffT;
        tempValue[SIDE_DOWN]=50+status.val[SIDE_DOWN].p*sbgError/100.0
                 +status.val[SIDE_DOWN].d*sbgDiff/sbgDiffT;
        tempValue[MAIN_LEFT]=0;
        tempValue[MAIN_RIGHT]=0;
    QList<pair<MOTORS,float>> tempList;
    for(int i=0;i<4;i++){
        if(tempValue[hList[i]]>=70){
            tempValue[hList[i]]=70;
        }
        else if(tempValue[hList[i]]<=-70){
            tempValue[hList[i]]=-70;
        }
    }
    for(int i=0;i<4;i++){
        tempList.push_back(make_pair<>(hList[i],tempValue[hList[i]]));
    }
    emit setHMotors(tempList);
    }
}

void controller::endSwing_SBG()
{
    status.cnt.clear();
    setZero();
    qDebug()<<"End Swing_SBG";
}

void controller::initForwardAction()
{
    loadConfig(HANG);
    emit setGoal(global_deep);

    updateConfig();
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    setFrameInteval(status.ms);
    qDebug()<<"Forward!";
}

void controller::ctrForwardAction()
{

    qDebug() << "initForward initForward initForward";

    loadConfig(FORWARD_SLOW);
    status.cnt[0]++;
    visionClass::visionData && tmp = vision->getData();
    const static MOTORS hList[4]={MAIN_LEFT,MAIN_RIGHT,SIDE_UP,SIDE_DOWN};

    float tempValue[NUMBER_OF_MOTORS];

    if(status.cnt[0]==1)
    {
        if(set_yaw_target != -999)
        {
            sbg.goal = set_yaw_target;
        }
        else
        {
            sbg.goal = sbg.yaw;
        }
    }

    float sbgError = sbg.goal-sbg.yaw;
    if(sbgError > 180)
    {
        sbgError = -360+sbgError;
    }
    if(sbgError < -180)
    {
        sbgError = 360+sbgError;
    }
    qDebug()<<sbgError;

    float sbgDiff = sbgError-sbg.yawErrorLast;
    sbg.yawErrorLast = sbgError;


    const int max_main_speed = forward_max_side_speed;
    const int max_side_speed = 50;

    if (sbgError < 0)
    {
        tempValue[MAIN_LEFT] = max_main_speed + status.val[MAIN_LEFT].p*sbgError + status.val[MAIN_LEFT].d*sbgDiff;
        tempValue[MAIN_RIGHT] = max_main_speed;
    }
    else if (sbgError >= 0)
    {
        tempValue[MAIN_LEFT] = max_main_speed;
        tempValue[MAIN_RIGHT] = max_main_speed -status.val[MAIN_RIGHT].p*sbgError - status.val[MAIN_RIGHT].d*sbgDiff;
    }
    tempValue[SIDE_UP] = 0;
    tempValue[SIDE_DOWN] = 0;


    for(int i = 0; i < 2; i++)
    {
        if(tempValue[hList[i]] >= max_main_speed)
        {
            tempValue[hList[i]] = max_main_speed;
        }
        else if(tempValue[hList[i]] <= -max_main_speed)
        {
            tempValue[hList[i]] = -max_main_speed;
        }
    }
    for(int i = 2; i < 4; i++)
    {
        if(tempValue[hList[i]] >= max_side_speed)
        {
            tempValue[hList[i]] = max_side_speed;
        }
        else if(tempValue[hList[i]] <= -max_side_speed)
        {
            tempValue[hList[i]] = -max_side_speed;
        }
    }

    // TODO
    /* 
     * 1.判断任务是否结束的条件有问题
     * 2.swing找不到门的话也没有写处理方案
     */
    qDebug() << "FORWARDW ARDACTION" << status.cnt[0];
    if(status.currentTask->id == Gate)
    {
        if(status.cnt[0] >= status.curPara.PlanCount)
        {
            if(isForwardInitial)    //判断是起始直航还是终末直航
            {
                tempValue[MAIN_LEFT]=0;
                tempValue[MAIN_RIGHT]=0;
                tempValue[SIDE_UP]=0;
                tempValue[SIDE_DOWN]=0;
                emit endTask();
            }

            else emit enterAction(SWINGACTION);
        }
    }
    else
    {
        if(tmp.m1_gateFound)
        {
            status.cnt[2]++;
            if(status.cnt[2] >= 3)
            {
                emit enterAction(FIND_THE_GATE);
            }
        }
    }

    // set motors value
    QList<pair<MOTORS,float>> tempList;
    for(int i = 0; i < 4; i++)
    {
        tempList.push_back(make_pair<>(hList[i],tempValue[hList[i]]));
    }
    emit setHMotors(tempList);
}

void controller::endForwardAction()
{
    status.cnt.clear();
    status.finished = FORWARDACTION;
    qDebug() << "End ForWardAction!";
}

void controller::initBackwardAction()
{
    emit setGoal(global_deep);
    loadConfig(HANG);
    updateConfig();
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);

    if(set_yaw_target != -999)
    {
        sbg.goal = set_yaw_target;
    }
    else
    {
        sbg.goal = sbg.yaw;
    }


    setFrameInteval(status.ms);
    qDebug() << "Backward!";
}

void controller::ctrBackwardAction()
{
    //由于姿态传感器数值不稳，采用杆水声导引退出绿色区域
    const static MOTORS hList[4]={MAIN_LEFT,MAIN_RIGHT,SIDE_UP,SIDE_DOWN};
    qDebug() << "Get in BackAction";

    float tempValue[NUMBER_OF_MOTORS];

    // visionClass::visionData && tmp = vision->getData();

    status.cnt[0]++;
    loadConfig(FORWARD_SLOW);

    qDebug()<<"sbgRevise Backward"<<status.cnt[0];

    float sbgError = sbg.goal-sbg.yaw;
    if(sbgError > 180)
    {
        sbgError = -360+sbgError;
    }
    if(sbgError < -180)
    {
        sbgError = 360+sbgError;
    }
    float sbgDiff = sbgError-sbg.yawErrorLast;
    sbg.yawErrorLast = sbgError;

    const int max_main_speed = acos_max_main_speed;
    const int max_side_speed = 50;


    if (sbgError < 0)
    {
        tempValue[MAIN_LEFT] = -max_main_speed;
        tempValue[MAIN_RIGHT] = -max_main_speed - status.val[MAIN_LEFT].p*sbgError - status.val[MAIN_LEFT].d*sbgDiff;
    }
    else if (sbgError >= 0)
    {
        tempValue[MAIN_LEFT] = -max_main_speed + status.val[MAIN_RIGHT].p*sbgError + status.val[MAIN_RIGHT].d*sbgDiff;
        tempValue[MAIN_RIGHT] = -max_main_speed;
    }
    tempValue[SIDE_UP] = 0;
    tempValue[SIDE_DOWN] = 0;


    for(int i = 0; i < 2; i++)
    {
        if(tempValue[hList[i]] >= max_main_speed)
        {
            tempValue[hList[i]] = max_main_speed;
        }
        else if(tempValue[hList[i]] <= -max_main_speed)
        {
            tempValue[hList[i]] = -max_main_speed;
        }
    }
    for(int i = 2; i < 4; i++)
    {
        if(tempValue[hList[i]] >= max_side_speed)
        {
            tempValue[hList[i]] = max_side_speed;
        }
        else if(tempValue[hList[i]] <= -max_side_speed)
        {
            tempValue[hList[i]] = -max_side_speed;
        }
    }

    QList<pair<MOTORS,float>> tempList;
    for(int i = 0; i < 4; i++)
    {
        tempList.push_back(make_pair<>(hList[i],tempValue[hList[i]]));
    }
    emit setHMotors(tempList);


//    if(tmp.m3_green_area == 999 && tmp.m3_drum_dx == 999 && tmp.m3_drum_dy == 999)
//    {
//        status.cnt[2]++;
//        qDebug()<<"Exit Green Area"<<status.cnt[2];
//    }
//    else status.cnt[2]=0;
//    if(status.cnt[2] >= 20)
    if(status.cnt[0] >= 160)
    {
        emit enterAction(ACOSREVISE);
    }
}

void controller::endBackwardAction()
{
    status.cnt.clear();
    status.finished = BACKWARDACTION;
    qDebug() << "End BackWardAction!";
}


void controller::initHangAction()
{
    emit setGoal(global_deep);
    loadConfig(HANG);
    updateConfig();
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    setFrameInteval(status.ms);
    sbg.goal = sbg.yaw;
    qDebug()<<"HangAction!";
}
void controller::ctrHangAction()
{
    qDebug() << "initHang initHang initHang initHang initHang initHang";

    status.cnt[0]++;
    qDebug()<<"HANG"<<status.cnt[0];
    if(abs(1-deep.value)<0.07)
    {
        status.cnt[1]++;
        if(status.cnt[1]==10)
        {
            emit enterAction(FORWARDACTION);
        }
    }
    //Subete wa Steins:Gate no Sentaku!
}

void controller::endHangAction()
{
    status.cnt.clear();
    status.finished = HANGACTION;
    qDebug()<<"End HangAction";
}

void controller::initSwingAction()
{
    emit setGoal(global_deep);
    loadConfig(HANG);
    updateConfig();
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    setFrameInteval(status.ms);
    emit missionStarted(Gate);

    if(!isForwardInitial)
    {
        isForwardInitial = true;
    }
    qDebug()<<"Swing!";

    visionClass::visionData && tmp = vision->getData();
    tmp.m1_gateFound = false;
}

void controller::ctrSwingAction()
{
    qDebug() << "initSwingAction initSwingAction initSwingAction";

    loadConfig(SWING);
    visionClass::visionData && tmp = vision->getData();

    status.cnt[0]++;
    qDebug() << "SWING" << status.cnt[0];

    const int max_main_speed = forward_max_side_speed;
    const int max_side_speed = 50;
    const double yaw_swing_radio = 5.0;

    if(status.cnt[0] == 1)
    {
        /****
        sbg.goal = sbg.yaw;
            ****/
    }

    /*
     * swing判断是否找到门的确认（滤波）方式可能有点问题
     * 如果误判严重的话条件改极端
     * 或者采取一些更为科学的方式来判断，比如中值等方式。
     *
     * 未找到门的话一直swing，bug
     */

    qDebug() <<"233333333333333333333333333333 "<< tmp.m1_gateFound;
    if(tmp.m1_gateFound)
    {
        qDebug() << "GateFound!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << status.cnt[1];
        status.cnt[1]++;
        if(status.cnt[1] >= status.curPara.PlanCount)
        {
            emit enterAction(FIND_THE_GATE);
        }
    }
    else
    {
        qDebug() << "Gate Not found";
        status.cnt[1]--;
        if(status.cnt[1] < 0)
        {
            status.cnt[1] = 0;
        }
    }

    const static MOTORS hList[4] = {MAIN_LEFT,MAIN_RIGHT,SIDE_UP,SIDE_DOWN};
    float tempValue[NUMBER_OF_MOTORS];

    float sbgError=sbg.goal-sbg.yaw;

    if(sbgError > 180)
    {
        sbgError = -360+sbgError;
    }
    else if(sbgError < -180)
    {
        sbgError = 360+sbgError;

    }
    qDebug() << "Error" << sbgError;

    float sbgDiff = sbgError-sbg.yawErrorLast;
    sbg.yawErrorLast = sbgError;

#ifdef LEFT
    tempValue[SIDE_UP] = 40 - status.val[SIDE_UP].p*yaw_swing_radio*sbgError - status.val[SIDE_UP].d*yaw_swing_radio*sbgDiff;
    tempValue[SIDE_DOWN] = 40 + status.val[SIDE_DOWN].p*yaw_swing_radio*sbgError + status.val[SIDE_DOWN].d*yaw_swing_radio*sbgDiff;
#endif

#ifdef RIGHT
   tempValue[SIDE_UP] = -22 - status.val[SIDE_UP].p*yaw_swing_radio*sbgError - status.val[SIDE_UP].d*yaw_swing_radio*sbgDiff;
   tempValue[SIDE_DOWN] = -22 + status.val[SIDE_DOWN].p*yaw_swing_radio*sbgError + status.val[SIDE_DOWN].d*yaw_swing_radio*sbgDiff;
#endif

//#ifdef LEFT
//    tempValue[SIDE_UP] = 40 - status.val[SIDE_UP].p*yaw_swing_radio*sbgError - status.val[SIDE_UP].d*yaw_swing_radio*sbgDiff;
//    tempValue[SIDE_DOWN] = 40 + status.val[SIDE_DOWN].p*yaw_swing_radio*sbgError + status.val[SIDE_DOWN].d*yaw_swing_radio*sbgDiff;
//#endif

//#ifdef RIGHT
//   tempValue[SIDE_UP] = -28 - status.val[SIDE_UP].p*yaw_swing_radio*sbgError - status.val[SIDE_UP].d*yaw_swing_radio*sbgDiff;
//   tempValue[SIDE_DOWN] = -28 + status.val[SIDE_DOWN].p*yaw_swing_radio*sbgError + status.val[SIDE_DOWN].d*yaw_swing_radio*sbgDiff;
//#endif

    qDebug() << "initSwingAction tempValue[SIDE_UP]:" << tempValue[SIDE_UP];
    qDebug() << "initSwingAction tempValue[SIDE_DOWN]:" << tempValue[SIDE_DOWN];

    // tempValue[SIDE_UP]=30+status.val[SIDE_UP].p*sbgError/100.0
    //         +status.val[SIDE_UP].d*sbgDiff/sbgDiffT;
    // tempValue[SIDE_DOWN]=30+status.val[SIDE_DOWN].p*sbgError/100.0
    //         +status.val[SIDE_DOWN].d*sbgDiff/sbgDiffT;

    tempValue[MAIN_LEFT]=0;
    tempValue[MAIN_RIGHT]=0;

    for(int i = 0; i < 2; i++)
    {
        if(tempValue[hList[i]] >= max_main_speed)
        {
            tempValue[hList[i]] = max_main_speed;
        }
        else if(tempValue[hList[i]] <= -max_main_speed)
        {
            tempValue[hList[i]] = -max_main_speed;
        }
    }
    for(int i = 2; i < 4; i++)
    {
        if(tempValue[hList[i]] >= max_side_speed)
        {
            tempValue[hList[i]] = max_side_speed;
        }
        else if(tempValue[hList[i]] <= -max_side_speed)
        {
            tempValue[hList[i]] = -max_side_speed;
        }
    }

    QList<pair<MOTORS,float>> tempList;
    for(int i = 0; i < 4; i++)
    {
        tempList.push_back(make_pair<>(hList[i],tempValue[hList[i]]));
    }

    emit setHMotors(tempList);

}

void controller::endSwingAction()
{
    status.cnt.clear();
    status.finished = SWINGACTION;
    qDebug()<<"End SwingAction";
}

void controller::initFind_The_Gate()
{

    emit setGoal(global_deep);
    loadConfig(HANG);
    updateConfig();
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    loadConfig(GATE_FORSEE);
    setFrameInteval(status.ms);
    qDebug() << "Find the Gate!";
}

void controller::ctrFind_The_Gate()
{
    qDebug() << "initFind_The_Gate initFind_The_Gate initFind_The_Gate";

    emit setGoal(global_deep);
    loadConfig(HANG);
    updateConfig();
    status.cnt[0]++;
    qDebug()<<"FIND_THE_GATE"<<status.cnt[0];
    loadConfig(GATE_FORSEE);

    const static MOTORS hList[4]={MAIN_LEFT,MAIN_RIGHT,SIDE_UP,SIDE_DOWN};
    // const static MOTORS zList[2]={SIDE_UP,SIDE_DOWN};

    const int max_main_speed = forward_max_side_speed;
    const int max_side_speed = 50;
    //const double yaw_swing_radio = 8;
    const double yaw_swing_radio = 3;

    visionClass::visionData && tmp = vision->getData();

    // float delta_t=(img.t_now-img.t_last)/1000.0;
    // img.t_last=img.t_now;
    // img.t_now=tmp.t_now;
    float tempValue[NUMBER_OF_MOTORS];

    float sbgError=sbg.goal-sbg.yaw;
    if(sbgError > 180)
    {
        sbgError = -360+sbgError;
    }
    else if(sbgError < -180)
    {
        sbgError = 360+sbgError;

    }
    float sbgDiff = sbgError-sbg.yawErrorLast;
    sbg.yawErrorLast = sbgError;

    img.gate_dx_diff = img.gate_dx-img.gate_dx_last;
    img.gate_dx_last = img.gate_dx;


    if(tmp.m1_centerdx != 999)
    {
        qDebug() << "find centerdx find centerdx find centerdx find centerdx";
        status.cnt[2] = 0;
        qDebug() << "Found the black stick.";
        img.gate_dx = tmp.m1_centerdx/10;
        qDebug() << "Gate Found";
        qDebug() << "img.gate_dx: " << img.gate_dx;
        qDebug() << "sbgError   : " << sbgError;

        // tempValue[MAIN_LEFT]=3+status.val[MAIN_LEFT].p*img.gate_dx/100.0+status.val[MAIN_LEFT].d*img.gate_dx_diff/delta_t;
        // tempValue[MAIN_RIGHT]=3+status.val[MAIN_RIGHT].p*img.gate_dx/100.0+status.val[MAIN_RIGHT].d*img.gate_dx_diff/delta_t;
        tempValue[MAIN_LEFT] = 0;
        tempValue[MAIN_RIGHT] = 0;
        //tempValue[SIDE_UP] = 0 + status.val[SIDE_UP].p*img.gate_dx + status.val[SIDE_DOWN].d*img.gate_dx - status.val[SIDE_UP].p*yaw_swing_radio*sbgError - status.val[SIDE_UP].d*yaw_swing_radio*sbgDiff;
        //tempValue[SIDE_DOWN] = 0 + status.val[SIDE_UP].p*img.gate_dx + status.val[SIDE_DOWN].d*img.gate_dx + status.val[SIDE_DOWN].p*yaw_swing_radio*sbgError + status.val[SIDE_DOWN].d*yaw_swing_radio*sbgDiff;

        if(img.gate_dx != 0)
        {
            tempValue[SIDE_UP] = 10*((img.gate_dx)/fabs(img.gate_dx)) + status.val[SIDE_UP].p*img.gate_dx + status.val[SIDE_DOWN].d*img.gate_dx_diff - status.val[SIDE_UP].p*yaw_swing_radio*sbgError - status.val[SIDE_UP].d*yaw_swing_radio*sbgDiff;
            tempValue[SIDE_DOWN] = 10*((img.gate_dx)/fabs(img.gate_dx)) + status.val[SIDE_UP].p*img.gate_dx + status.val[SIDE_DOWN].d*img.gate_dx_diff + status.val[SIDE_DOWN].p*yaw_swing_radio*sbgError + status.val[SIDE_DOWN].d*yaw_swing_radio*sbgDiff;
        }
        else
        {
            tempValue[SIDE_UP] = 0 + status.val[SIDE_UP].p*img.gate_dx + status.val[SIDE_DOWN].d*img.gate_dx_diff - status.val[SIDE_UP].p*yaw_swing_radio*sbgError - status.val[SIDE_UP].d*yaw_swing_radio*sbgDiff;
            tempValue[SIDE_DOWN] = 0 + status.val[SIDE_UP].p*img.gate_dx + status.val[SIDE_DOWN].d*img.gate_dx_diff + status.val[SIDE_DOWN].p*yaw_swing_radio*sbgError + status.val[SIDE_DOWN].d*yaw_swing_radio*sbgDiff;
        }


        qDebug() << "initFind_The_Gate initFind_The_Gate initFind_The_Gate SIDE_UP  :" << tempValue[SIDE_UP];
        qDebug() << "initFind_The_Gate initFind_The_Gate initFind_The_Gate SIDE_DOWN:" << tempValue[SIDE_DOWN];
    }
    else if(tmp.m1_centerdx==999 && ((tmp.m1_leftdx!=999)||(tmp.m1_rightdx!=999))) //若只找到一根竖杆
    {
        qDebug() << "find m1_rightdx find m1_rightdx find m1_rightdx find m1_rightdx";
        status.cnt[2] = 0;
        if(tmp.m1_leftdx != 999)            //只找到红杆
        {
            qDebug() << "Only find red bar";
            img.gate_dx = tmp.m1_leftdx/10;
        }
        else if(tmp.m1_rightdx != 999)      //只找到绿杆
        {
            qDebug() << "Only find green bar";
            img.gate_dx = tmp.m1_rightdx/10;
        }
        tempValue[MAIN_LEFT] = 0;
        tempValue[MAIN_RIGHT] = 0;
        //tempValue[SIDE_UP] = 0 - status.val[SIDE_UP].p*img.gate_dx - status.val[SIDE_DOWN].d*img.gate_dx - status.val[SIDE_UP].p*sbgError - status.val[SIDE_UP].d*sbgDiff;
        //tempValue[SIDE_DOWN] = 0 + status.val[SIDE_UP].p*img.gate_dx + status.val[SIDE_DOWN].d*img.gate_dx + status.val[SIDE_DOWN].p*sbgError + status.val[SIDE_DOWN].d*sbgDiff;

        //tempValue[SIDE_UP] = 0 + status.val[SIDE_UP].p*img.gate_dx + status.val[SIDE_UP].d*img.gate_dx - status.val[SIDE_UP].p*yaw_swing_radio*sbgError - status.val[SIDE_UP].d*yaw_swing_radio*sbgDiff;
        //tempValue[SIDE_DOWN] = 0 + status.val[SIDE_DOWN].p*img.gate_dx + status.val[SIDE_DOWN].d*img.gate_dx + status.val[SIDE_DOWN].p*yaw_swing_radio*sbgError + status.val[SIDE_DOWN].d*yaw_swing_radio*sbgDiff;
        if(img.gate_dx != 0)
        {
            tempValue[SIDE_UP] = 10*((img.gate_dx)/fabs(img.gate_dx)) + status.val[SIDE_UP].p*img.gate_dx + status.val[SIDE_DOWN].d*img.gate_dx_diff - status.val[SIDE_UP].p*yaw_swing_radio*sbgError - status.val[SIDE_UP].d*yaw_swing_radio*sbgDiff;
            tempValue[SIDE_DOWN] = 10*((img.gate_dx)/fabs(img.gate_dx)) + status.val[SIDE_UP].p*img.gate_dx + status.val[SIDE_DOWN].d*img.gate_dx_diff + status.val[SIDE_DOWN].p*yaw_swing_radio*sbgError + status.val[SIDE_DOWN].d*yaw_swing_radio*sbgDiff;
        }
        else
        {
            tempValue[SIDE_UP] = 0 + status.val[SIDE_UP].p*img.gate_dx + status.val[SIDE_DOWN].d*img.gate_dx_diff - status.val[SIDE_UP].p*yaw_swing_radio*sbgError - status.val[SIDE_UP].d*yaw_swing_radio*sbgDiff;
            tempValue[SIDE_DOWN] = 0 + status.val[SIDE_UP].p*img.gate_dx + status.val[SIDE_DOWN].d*img.gate_dx_diff + status.val[SIDE_DOWN].p*yaw_swing_radio*sbgError + status.val[SIDE_DOWN].d*yaw_swing_radio*sbgDiff;
        }
    }
    else if(tmp.m1_centerdx == 999 && tmp.m1_leftdx == 999 && tmp.m1_rightdx == 999)
    {
        qDebug() << "Not Found the black and red and green stick.";
        status.cnt[2]++;
        img.gate_dx = 999;
        if(status.cnt[2] < 5)
        {
            qDebug() << "Gate Not Found";
            tempValue[MAIN_LEFT] = 0;
            tempValue[MAIN_RIGHT] = 0;
            tempValue[SIDE_UP] = 0;
            tempValue[SIDE_DOWN] = 0;
        }
        else
        {
            qDebug()<<"Gate Lost, Rotate";
            // TODO
            if(img.gate_dx_last >= 0)
            {
                tempValue[MAIN_LEFT] = 0;
                tempValue[MAIN_RIGHT] = 0;
                tempValue[SIDE_UP] = 14 - status.val[SIDE_UP].p*yaw_swing_radio*sbgError - status.val[SIDE_UP].d*yaw_swing_radio*sbgDiff;
                tempValue[SIDE_DOWN] = 14 + status.val[SIDE_DOWN].p*yaw_swing_radio*sbgError + status.val[SIDE_DOWN].d*yaw_swing_radio*sbgDiff;
            }
            else
            {
                tempValue[MAIN_LEFT] = 0;
                tempValue[MAIN_RIGHT] = 0;
                tempValue[SIDE_UP] = -14 - status.val[SIDE_UP].p*yaw_swing_radio*sbgError - status.val[SIDE_UP].d*yaw_swing_radio*sbgDiff;
                tempValue[SIDE_DOWN] = -14 + status.val[SIDE_DOWN].p*yaw_swing_radio*sbgError + status.val[SIDE_DOWN].d*yaw_swing_radio*sbgDiff;
            }
        }
    }

    if(abs(img.gate_dx) < 70/10)           //航行器对正，直航
    {
        qDebug() << "Straight, Forward";
        status.cnt[1]++;
        if(status.cnt[1] >= 8)         //时间可能有点长
        {
            emit enterAction(FORWARD_GATE);
        }
    }
    else
    {
        status.cnt[1] = 0;              //条件判断滤波可能有问题
    }

    for(int i = 0; i < 2; i++)
    {
        if(tempValue[hList[i]] >= max_main_speed)
        {
            tempValue[hList[i]] = max_main_speed;
        }
        else if(tempValue[hList[i]] <= -max_main_speed)
        {
            tempValue[hList[i]] = -max_main_speed;
        }
    }
    for(int i = 2; i < 4; i++)
    {
        if(tempValue[hList[i]] >= max_side_speed)
        {
            tempValue[hList[i]] = max_side_speed;
        }
        else if(tempValue[hList[i]] <= -max_side_speed)
        {
            tempValue[hList[i]] = -max_side_speed;
        }
    }

    QList<pair<MOTORS,float>> tempList;
    for(int i = 0; i < 4; i++)
    {
        tempList.push_back(make_pair<>(hList[i],tempValue[hList[i]]));
    }

    emit setHMotors(tempList);
}


void controller::endFind_The_Gate()
{
    status.cnt.clear();
    status.finished = FIND_THE_GATE;
    qDebug()<<"End Find_The_Gate";
}


void controller::initFind_The_Gate2()
{
    emit setGoal(1.2);
    loadConfig(HANG);
    updateConfig();
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    loadConfig(FORWARD_FORSEE);
    setFrameInteval(status.ms);
    qDebug() << "Find the Gate2!";
}
void controller::ctrFind_The_Gate2()
{
    status.cnt[0]++;
    qDebug()<<"FIND_THE_GATE2"<<status.cnt[0];
    const static MOTORS hList[4]={MAIN_LEFT,MAIN_RIGHT,SIDE_UP,SIDE_DOWN};
    const static MOTORS zList[2]={SIDE_UP,SIDE_DOWN};
    visionClass::visionData && tmp = vision->getData();
    float delta_t=(img.t_now-img.t_last)/1000.0;
    float tempValue[NUMBER_OF_MOTORS];
    img.gate_dx_last = img.gate_dx;
    if(tmp.m1_centerdx==999) //若只找到一根杆
    {
        if(tmp.m1_leftdx!=999)//只找到红杆
        {
            qDebug()<<"Only find red bar";
            img.gate_dx = tmp.m1_leftdx;
        }
        else if(tmp.m1_rightdx!=999)//只找到绿杆
        {
            qDebug()<<"Only find green bar";
            img.gate_dx = -tmp.m1_rightdx;
        }
    }
    else if(tmp.m1_leftdx!=999 && tmp.m1_rightdx!=999)       //找到两根杆
    {
        if(abs(tmp.m1_leftdx) <= abs(tmp.m1_rightdx))img.gate_dx = -tmp.m1_leftdx;
        else img.gate_dx = tmp.m1_rightdx;
    }
    if(abs(img.gate_dx)<60)//航行器对正，直航
    {
        qDebug()<<"Straight,Forward";
        status.cnt[1]++;
        if(status.cnt[1]>=10)emit enterAction(FORWARDACTION);
    }
    img.t_last=img.t_now;
    img.t_now=tmp.t_now;
    img.gate_dx_diff = img.gate_dx-img.gate_dx_last;
    if(tmp.m1_gateFound)   //图像未丢失，正常导引
    {
        qDebug()<<"Gate Found";
        //暂定不设推进器基准值
        tempValue[MAIN_LEFT]=status.val[MAIN_LEFT].p*img.gate_dx/100+status.val[MAIN_LEFT].d*img.gate_dx_diff/delta_t;
        tempValue[MAIN_RIGHT]=status.val[MAIN_RIGHT].p*img.gate_dx/100+status.val[MAIN_RIGHT].d*img.gate_dx_diff/delta_t;
        tempValue[SIDE_UP]=0;
        tempValue[SIDE_DOWN]=0;
    }
    else    //若进入图像导引后图像丢失，则航行器旋转找门
    {
        qDebug()<<"Gate Lost,Rotate";
        if(img.gate_dx_last>=0)
        {
            tempValue[MAIN_LEFT]=0;
            tempValue[MAIN_RIGHT]=0;
            tempValue[SIDE_UP]=-25;
            tempValue[SIDE_DOWN]=25;
        }
        else
        {
            tempValue[MAIN_LEFT]=0;
            tempValue[MAIN_RIGHT]=0;
            tempValue[SIDE_UP]=25;
            tempValue[SIDE_DOWN]=-25;
        }
    }
    for(int i=0;i<4;i++){
        if(tempValue[hList[i]]>=40){
            tempValue[hList[i]]=40;
        }
        else if(tempValue[hList[i]]<=-40){
            tempValue[hList[i]]=-40;
        }
    }
    for(int i=0;i<2;i++)
    {
        if(tempValue[zList[i]]>=30){
            tempValue[zList[i]]=30;
        }
        else if(tempValue[zList[i]]<=-30){
            tempValue[zList[i]]=-30;
        }
    }

    QList<pair<MOTORS,float>> tempList;
    for(int i=0;i<4;i++){
        tempList.push_back(make_pair<>(hList[i],tempValue[hList[i]]));
    }
    emit setHMotors(tempList);
}
void controller::endFind_The_Gate2()
{
    status.cnt.clear();
    status.finished = FIND_THE_GATE2;
    qDebug()<<"End Find_The_Gate2";
}
void controller::initFind_The_Gate3()
{
    emit setGoal(1.2);
    loadConfig(HANG);
    updateConfig();
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    loadConfig(FORWARD_FORSEE);
    setFrameInteval(status.ms);
    isRightBarDetect = false;
    isLeftBarDetect = false;
    qDebug() << "Find the Gate3!";
}
void controller::ctrFind_The_Gate3()
{
    status.cnt[0]++;
    qDebug()<<"FIND_THE_GATE3"<<status.cnt[0];
    const static MOTORS hList[4]={MAIN_LEFT,MAIN_RIGHT,SIDE_UP,SIDE_DOWN};
    const static MOTORS zList[2]={SIDE_UP,SIDE_DOWN};
    visionClass::visionData && tmp = vision->getData();
    float delta_t=(img.t_now-img.t_last)/1000.0;
    float tempValue[NUMBER_OF_MOTORS];
    img.gate_dx_last = img.gate_dx;
    img.gate_dx_diff = img.gate_dx-img.gate_dx_last;
    if(img.gate_dx!=-999)
    {
        if(tmp.m1_leftdx!=999 && tmp.m1_rightdx!=999)
        {
            qDebug()<<"Detect All Bar,Use Green one";
            img.gate_dx = tmp.m1_rightdx;
            isRightBarDetect = true;
            isLeftBarDetect = false;
        }
        else if(tmp.m1_leftdx!=999 && tmp.m1_rightdx==999)
        {
            qDebug()<<"Only Detect Red Bar";
            img.gate_dx = tmp.m1_leftdx;
            isLeftBarDetect = true;
            isRightBarDetect = false;
        }
        else if(tmp.m1_leftdx==999 && tmp.m1_rightdx!=999)
        {
            qDebug()<<"Only Detect Green Bar";
            img.gate_dx = tmp.m1_rightdx;
            isRightBarDetect = true;
            isLeftBarDetect = false;
        }
        if(!tmp.m1_gateFound)
        {
            qDebug()<<"Gate Lost,Rotate";
            if(img.gate_dx_last>=0)
            {
                tempValue[MAIN_LEFT]=0;
                tempValue[MAIN_RIGHT]=0;
                tempValue[SIDE_UP]=-25;
                tempValue[SIDE_DOWN]=25;
            }
            else
            {
                tempValue[MAIN_LEFT]=0;
                tempValue[MAIN_RIGHT]=0;
                tempValue[SIDE_UP]=25;
                tempValue[SIDE_DOWN]=-25;
            }
        }
        else
        {
            qDebug()<<"Gate Found";
            tempValue[MAIN_LEFT]=30+status.val[MAIN_LEFT].p*img.gate_dx/100+status.val[MAIN_LEFT].d*img.gate_dx_diff/delta_t;
            tempValue[MAIN_RIGHT]=30+status.val[MAIN_RIGHT].p*img.gate_dx/100+status.val[MAIN_RIGHT].d*img.gate_dx_diff/delta_t;
            tempValue[SIDE_UP]=0;
            tempValue[SIDE_DOWN]=0;
        }
    }
    else                    //灵魂跑法，旋转过门
    {
        qDebug()<<"Spiritual Pass Gate";
        status.cnt[1]++;
        tempValue[MAIN_LEFT]=0;
        tempValue[MAIN_RIGHT]=0;
        tempValue[SIDE_UP]=0;
        if(isLeftBarDetect)tempValue[SIDE_DOWN]=-30;
        else if(isRightBarDetect)tempValue[SIDE_DOWN]=30;
        if(status.cnt[1]>=30)
        {
            emit enterAction(BACKWARDACTION);
        }
    }
    for(int i=0;i<4;i++){
        if(tempValue[hList[i]]>=40){
            tempValue[hList[i]]=40;
        }
        else if(tempValue[hList[i]]<=-40){
            tempValue[hList[i]]=-40;
        }
    }
    for(int i=0;i<2;i++)
    {
        if(tempValue[zList[i]]>=30){
            tempValue[zList[i]]=30;
        }
        else if(tempValue[zList[i]]<=-30){
            tempValue[zList[i]]=-30;
        }
    }

    QList<pair<MOTORS,float>> tempList;
    for(int i=0;i<4;i++){
        tempList.push_back(make_pair<>(hList[i],tempValue[hList[i]]));
    }
    emit setHMotors(tempList);
}

void controller::initForward_Gate()
{
    emit setGoal(global_deep);
    loadConfig(HANG);
    updateConfig();
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    setFrameInteval(status.ms);
    qDebug() << "Forward_Gate!";
}

void controller::ctrForward_Gate()
{
    qDebug() << "initForward_Gate initForward_Gate initForward_Gate";

    qDebug() << "img.gate_dx img.gate_dx img.gate_dx img.gate_dx img.gate_dx img.gate_dx:" << img.gate_dx;
    if(fabs(img.gate_dx) <= 70/10)
    {
        status.cnt[0]++;
        qDebug() << "status.cnt[0] status.cnt[0] status.cnt[0] status.cnt[0]:" << status.cnt[0];
    }

    qDebug()<<"FORWARD_GATE"<< status.cnt[0];
    visionClass::visionData && tmp = vision->getData();

    const static MOTORS hList[4]={MAIN_LEFT,MAIN_RIGHT,SIDE_UP,SIDE_DOWN};
    float tempValue[NUMBER_OF_MOTORS];

    const int max_main_speed = forward_max_side_speed;
    const int max_side_speed = 50;

    img.gate_dx_last = img.gate_dx;
    img.gate_dx_diff = img.gate_dx-img.gate_dx_last;
    
    qDebug() << "Start Forward !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
    status.cnt[1]++;
    status.cnt[2]++;

    visionClass::visionData && tmp = vision->getData();

    if(status.cnt[2]==1)
    {
       loadConfig(FORWARD_SLOW);
    }
    float sbgError=sbg.goal-sbg.yaw;

    if(sbgError > 180)
    {
        sbgError = -360+sbgError;
    }
    if(sbgError < -180)
    {
        sbgError = 360+sbgError;
    }

    qDebug()<<sbgError;

    float sbgDiff = sbgError-sbg.yawErrorLast;
    sbg.yawErrorLast=sbgError;

    qDebug() << "Error :                         "<< sbgError;

    const int max_main_speed = 60;

    if (sbgError < 0)
    {
        tempValue[MAIN_LEFT] = max_main_speed + status.val[MAIN_LEFT].p*sbgError + status.val[MAIN_LEFT].d*sbgDiff;
        tempValue[MAIN_RIGHT] = max_main_speed;
    }
    else if (sbgError >= 0) 
    {
        tempValue[MAIN_LEFT] = max_main_speed;
        tempValue[MAIN_RIGHT] = max_main_speed -status.val[MAIN_RIGHT].p*sbgError - status.val[MAIN_RIGHT].d*sbgDiff;
    }
    tempValue[SIDE_UP] = 0;
    tempValue[SIDE_DOWN] = 0;


    qDebug()<<tempValue[MAIN_LEFT]<<"Left";
    qDebug()<<tempValue[MAIN_RIGHT]<<"Right";

    for(int i = 0; i < 2; i++)
    {
        if(tempValue[hList[i]] >= max_main_speed)
        {
            tempValue[hList[i]] = max_main_speed;
        }
        else if(tempValue[hList[i]] <= -max_main_speed)
        {
            tempValue[hList[i]] = -max_main_speed;
        }
    }
    for(int i = 2; i < 4; i++)
    {
        if(tempValue[hList[i]] >= max_side_speed)
        {
            tempValue[hList[i]] = max_side_speed;
        }
        else if(tempValue[hList[i]] <= -max_side_speed)
        {
            tempValue[hList[i]] = -max_side_speed;
        }
    }

    QList<pair<MOTORS,float>> tempList;
    for(int i = 0; i < 4; i++)
    {
        tempList.push_back(make_pair<>(hList[i],tempValue[hList[i]]));
    }

    emit setHMotors(tempList);

    if(status.cnt[1] >= status.curPara.PlanCount)
    {
        tempValue[MAIN_LEFT] = 0;
        tempValue[MAIN_RIGHT] = 0;
        tempValue[SIDE_UP] = 0;
        tempValue[SIDE_DOWN] = 0;
        emit endTask();
    }
}

void controller::endForward_Gate()
{
    status.cnt.clear();
    status.finished = FORWARD_GATE;
    qDebug()<<"End Forward_Gate";
}


void controller::initArm_Down()
{

    if(status.currentTask->id == Drop)
    {
        emit setGoal(1.4);
        loadConfig(HANG);
        updateConfig();
    }
    else if(status.currentTask->id == Acquire)
    {
        emit setGoal(1.4);
        loadConfig(HANG);
        updateConfig();
    }
    status.cnt.push_back(0);
    setFrameInteval(status.ms);
    qDebug()<<"Arm_Down";
}
void controller::ctrArm_Down()
{
    if(status.currentTask->id == Drop)
    {
        emit setGoal(global_deep);
        loadConfig(HANG);
        updateConfig();
    }
    status.cnt[0]++;
    const static MOTORS hList[4]={MAIN_LEFT,MAIN_RIGHT,SIDE_UP,SIDE_DOWN};
    float tempValue[NUMBER_OF_MOTORS];
    tempValue[MAIN_LEFT]=0;
    tempValue[MAIN_RIGHT]=0;
    tempValue[SIDE_UP]=0;
    tempValue[SIDE_DOWN]=0;
    qDebug()<<"Arm Down!"<<status.cnt[0];
    if(status.cnt[0]==1)
    {
        emit setStepDown();
        qDebug()<<"Step Down"<<status.cnt[0];
    }
    if(status.cnt[0]>1 && status.cnt[0]<=3)
    {
        qDebug()<<step_flag;
        if(step_flag!=true)
        {
            emit setStepDown();
            qDebug()<<"Step Down"<<status.cnt[0];
        }
    }
    if(status.cnt[0]==5)
    {
        step_flag=0;
    }
    if(status.cnt[0]>=42 && status.cnt[0]<=44 && status.currentTask->id == Drop)
    {
        // emit setSevoOpen();
        qDebug()<<"Servo Open but can't.";
    }
    if(status.cnt[0]==47)
    {
        if(status.currentTask->id == Drop)
        {
            qDebug() << "task end.";
            emit endTask();
        }
        else if(status.currentTask->id == Acquire)
        {
            currentActionList.pop();
            emit enterAction(BACKWARDDRUM);
        }
    }
    QList<pair<MOTORS,float>> tempList;
    for(int i=0;i<4;i++){
        tempList.push_back(make_pair<>(hList[i],tempValue[hList[i]]));
    }
    emit setHMotors(tempList);
}
void controller::endArm_Down()
{
    status.cnt.clear();
    status.finished = ARM_DOWN;
    qDebug()<<"End Arm_Down";
}
void controller::initArm_Up()
{
    loadConfig(HANG);
    emit setGoal(0.8);
    updateConfig();
    status.cnt.push_back(0);
    setFrameInteval(status.ms);
    qDebug()<<"Arm_Up";
}
void controller::ctrArm_Up()
{
    status.cnt[0]++;
    const static MOTORS hList[4]={MAIN_LEFT,MAIN_RIGHT,SIDE_UP,SIDE_DOWN};
    float tempValue[NUMBER_OF_MOTORS];
    tempValue[MAIN_LEFT]=0;
    tempValue[MAIN_RIGHT]=0;
    tempValue[SIDE_UP]=0;
    tempValue[SIDE_DOWN]=0;
    qDebug()<<"Arm Up!"<<status.cnt[0];
    if(status.cnt[0]==1)
    {
        qDebug()<<"Servo Close";
       // emit setSevoClose();
    }
    if(status.cnt[0]==10)
    {
        emit setStepUp();
        qDebug()<<"Step Up";
    }
    if(status.cnt[0]==40)emit endTask();
}
void controller::endArm_Up()
{
    status.cnt.clear();
    status.finished = ARM_UP;
    qDebug()<<"End Arm_UP";
}

void controller::initAcosRevise()
{
    loadConfig(HANG);
    emit setGoal(global_deep);
    updateConfig();

    loadConfig(FORWARD_ACOS);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);

    status.ms = 200;
    setFrameInteval(status.ms);
    qDebug() << "AcosRevise!";
}
void controller::ctrAcosRevise()
{
    emit setGoal(global_deep-0.6);
    loadConfig(HANG);
    updateConfig();
    loadConfig(FORWARD_ACOS);
    status.cnt[0]++;

    visionClass::visionData && tmp = vision->getData();
    qDebug()<<"Drop MainTask";
    float tempValue[NUMBER_OF_MOTORS];

    const static MOTORS hList[4]={MAIN_LEFT,MAIN_RIGHT,SIDE_UP,SIDE_DOWN};


    static int pushed_repeat_cnt = 0;
    static std::deque<double> deq_acos_theta1(1);

    const int pushed_repeat_cnt_max = 5;   //acos Hz: 1Hz, controller Hz: 5Hz
    const int deq_max_length = 5;

    if(acos.theta1 == acos.theta1Last)
    {
        pushed_repeat_cnt++;
    }
    else if(pushed_repeat_cnt > pushed_repeat_cnt_max || acos.theta1 != acos.theta1Last)
    {
        deq_acos_theta1.push_back(acos.theta1);
        qDebug() << "raw acos.theta1 pushed back: " << acos.theta1;
        pushed_repeat_cnt = 1;
    }

    acos.theta1Last = acos.theta1;

    if(deq_acos_theta1.size() > deq_max_length)
    {
        while (deq_acos_theta1.size() > deq_max_length) {
            deq_acos_theta1.pop_front();
        }
    }

    std::deque<double> tmp_deq_acos_theta1 = deq_acos_theta1;
    std::sort(tmp_deq_acos_theta1.begin(), tmp_deq_acos_theta1.end());

    double tmp_acos_theta1 = tmp_deq_acos_theta1.at((tmp_deq_acos_theta1.size()-1)/2>=0 ? (tmp_deq_acos_theta1.size()-1)/2 : 0);

    qDebug() << "Choose acos.theta1:   " << tmp_acos_theta1;

    static bool flag_start_recognzing_drum = true;
    static bool flag_first_acos = true;

    if(tmp.m3_drum_dx!=999 && tmp.m3_drum_dy!=999)
    {
        status.cnt[0] = 0;

        loadConfig(LOCATE_BOTTOM);
        qDebug()<<"Found the Drum, Now Recognizing";

        flag_first_acos = false;
        if(flag_start_recognzing_drum)
        {
            sbg.goal = sbg.yaw;
            flag_start_recognzing_drum = false;
        }

        float sbgError=sbg.goal-sbg.yaw;
        if(sbgError > 180)
        {
            sbgError = -360+sbgError;
        }
        else if(sbgError < -180)
        {
            sbgError = 360+sbgError;

        }
        qDebug() << "Error" << sbgError;

        float sbgDiff = sbgError-sbg.yawErrorLast;
        sbg.yawErrorLast = sbgError;


        img.t_last = img.t_now;
        img.t_now = tmp.t_now;

        float imgDiffT = (img.t_now-img.t_last)/1000.0;
        img.drum_dx_last = img.drum_dx;
        img.drum_dy_last = img.drum_dy;

        img.drum_dy = tmp.m3_drum_dx/40.0;
        img.drum_dx = -tmp.m3_drum_dy/40.0;

        img.drum_dx_diff = img.drum_dx-img.drum_dx_last;
        img.drum_dy_diff = img.drum_dy-img.drum_dy_last;

        imgDiffT = 0.25;

        // if(abs(img.drum_dx)<27/10.0 && abs(img.drum_dy)<27/10.0 && abs(img.drum_dx_diff)<27.0/10 && abs(img.drum_dy_diff)<27/10.0)
        if(abs(img.drum_dx)<40/10.0 && abs(img.drum_dy)<40/10.0 && abs(img.drum_dx_diff)<40.0/10 && abs(img.drum_dy_diff)<40/10.0)
        {
            status.cnt[1]++;
            qDebug() << "Recognized,Now begin to catch ball" << status.cnt[1]++;

            // if(status.cnt[1] >= 30)
            if(status.cnt[1] >= 35)
            {
                tempValue[MAIN_LEFT]=0;
                tempValue[MAIN_RIGHT]=0;
                tempValue[SIDE_UP]=0;
                tempValue[SIDE_DOWN]=0;

                qDebug() << "ready to catch ball";
                emit setSevoOpen();
                status.cnt[6]++;
                if(status.cnt[6] >= 5)
                {
                    // emit enterAction(BALLREVISE);
                    emit endTask();
                }
            }
        }
        else
        {
            qDebug() << "Not Recognized";
            if(status.cnt[1] > 0)
            {
                status.cnt[1]--;
            }
            else
            {
                status.cnt[1] = 0;
            }
        }

        const int max_main_speed = drum_max_main_speed;
        const int max_side_speed = 35;

        if(img.drum_dy > 0)
        {
            tempValue[MAIN_LEFT] = 10*((img.drum_dy)/fabs(img.drum_dy)) + status.val[MAIN_LEFT].p*img.drum_dy + status.val[MAIN_LEFT].d*img.drum_dy_diff;
            tempValue[MAIN_RIGHT] = 10*((img.drum_dy)/fabs(img.drum_dy)) + status.val[MAIN_LEFT].p*img.drum_dy + status.val[MAIN_LEFT].d*img.drum_dy_diff;
        }
        else if(img.drum_dy < 0)
        {
            tempValue[MAIN_LEFT] = 4*((img.drum_dy)/fabs(img.drum_dy)) + status.val[MAIN_LEFT].p*img.drum_dy + status.val[MAIN_LEFT].d*img.drum_dy_diff;
            tempValue[MAIN_RIGHT] = 6*((img.drum_dy)/fabs(img.drum_dy)) + status.val[MAIN_LEFT].p*img.drum_dy + status.val[MAIN_LEFT].d*img.drum_dy_diff;
        }
        else
        {
            tempValue[MAIN_LEFT] = 0 + status.val[MAIN_LEFT].p*img.drum_dy + status.val[MAIN_LEFT].d*img.drum_dy_diff;
            tempValue[MAIN_RIGHT] = 0 + status.val[MAIN_LEFT].p*img.drum_dy + status.val[MAIN_LEFT].d*img.drum_dy_diff;
        }

        if (img.drum_dx > 0)
        {
            tempValue[SIDE_UP] = 10*((img.drum_dx)/fabs(img.drum_dx)) + status.val[SIDE_UP].p*img.drum_dx + status.val[SIDE_UP].d*img.drum_dx_diff;
            tempValue[SIDE_DOWN] = 4*((img.drum_dx)/fabs(img.drum_dx)) + status.val[SIDE_DOWN].p*img.drum_dx + status.val[SIDE_DOWN].d*img.drum_dx_diff;
        }
        else if (img.drum_dx < 0)
        {
            tempValue[SIDE_UP] = 4*((img.drum_dx)/fabs(img.drum_dx)) + status.val[SIDE_UP].p*img.drum_dx + status.val[SIDE_UP].d*img.drum_dx_diff;
            tempValue[SIDE_DOWN] = 6*((img.drum_dx)/fabs(img.drum_dx)) + status.val[SIDE_DOWN].p*img.drum_dx + status.val[SIDE_DOWN].d*img.drum_dx_diff;
        }
        else
        {
            tempValue[SIDE_UP] = 0 + status.val[SIDE_UP].p*img.drum_dx + status.val[SIDE_UP].d*img.drum_dx_diff;
            tempValue[SIDE_DOWN] = 0 + status.val[SIDE_DOWN].p*img.drum_dx + status.val[SIDE_DOWN].d*img.drum_dx_diff;
        }

        qDebug() << "img.drum_dx: " << img.drum_dx;
        qDebug() << "img.drum_dy: " << img.drum_dy;
        qDebug() << "tempValue[MAIN_LEFT] tempValue[MAIN_RIGHT] tempValue[SIDE_UP] tempValue[SIDE_DOWN]" << tempValue[MAIN_LEFT] << " " << tempValue[MAIN_LEFT] << " " << tempValue[SIDE_UP] << " " << tempValue[SIDE_DOWN];

        for(int i = 0; i < 2; i++)
        {
            if(tempValue[hList[i]] >= max_main_speed)
            {
                tempValue[hList[i]] = max_main_speed;
            }
            else if(tempValue[hList[i]] <= -max_main_speed)
            {
                tempValue[hList[i]] = -max_main_speed;
            }
        }
        for(int i = 2; i < 4; i++)
        {
            if(tempValue[hList[i]] >= max_side_speed)
            {
                tempValue[hList[i]] = max_side_speed;
            }
            else if(tempValue[hList[i]] <= -max_side_speed)
            {
                tempValue[hList[i]] = -max_side_speed;
            }
        }
    }
    else if(img.drum_dx_last  !=999 && img.drum_dy_last != 999 && status.cnt[0] <= 25)
    {
        status.cnt[0]++;

        loadConfig(LOCATE_BOTTOM);

        img.t_last = img.t_now;
        img.t_now = tmp.t_now;

        img.drum_dy = img.drum_dx_last;
        img.drum_dx = img.drum_dy_last;

        img.drum_dx_diff = img.drum_dx-img.drum_dx_last;
        img.drum_dy_diff = img.drum_dy-img.drum_dy_last;


        const int max_main_speed = drum_max_main_speed;
        const int max_side_speed = 40;

        if(img.drum_dy != 0)
        {
            tempValue[MAIN_LEFT] = 10*((img.drum_dy)/fabs(img.drum_dy)) + status.val[MAIN_LEFT].p*img.drum_dy + status.val[MAIN_LEFT].d*img.drum_dy_diff;
            tempValue[MAIN_RIGHT] = 10*((img.drum_dy)/fabs(img.drum_dy)) + status.val[MAIN_LEFT].p*img.drum_dy + status.val[MAIN_LEFT].d*img.drum_dy_diff;
        }
        else
        {
            tempValue[MAIN_LEFT] = 0 + status.val[MAIN_LEFT].p*img.drum_dy + status.val[MAIN_LEFT].d*img.drum_dy_diff;
            tempValue[MAIN_RIGHT] = 0 + status.val[MAIN_LEFT].p*img.drum_dy + status.val[MAIN_LEFT].d*img.drum_dy_diff;
        }

        if(img.drum_dx != 0)
        {
            tempValue[SIDE_UP] = 10*((img.drum_dx)/fabs(img.drum_dx)) + status.val[SIDE_UP].p*img.drum_dx + status.val[SIDE_UP].d*img.drum_dx_diff;
            tempValue[SIDE_DOWN] = 10*((img.drum_dx)/fabs(img.drum_dx)) + status.val[SIDE_DOWN].p*img.drum_dx + status.val[SIDE_DOWN].d*img.drum_dx_diff;
        }
        else
        {
            tempValue[SIDE_UP] = 0 + status.val[SIDE_UP].p*img.drum_dx + status.val[SIDE_UP].d*img.drum_dx_diff;
            tempValue[SIDE_DOWN] = 0 + status.val[SIDE_DOWN].p*img.drum_dx + status.val[SIDE_DOWN].d*img.drum_dx_diff;
        }

        qDebug() << "img.drum_dx: " << img.drum_dx;
        qDebug() << "img.drum_dy: " << img.drum_dy;
        qDebug() << "tempValue[MAIN_LEFT] tempValue[MAIN_RIGHT] tempValue[SIDE_UP] tempValue[SIDE_DOWN]" << tempValue[MAIN_LEFT] << " " << tempValue[MAIN_LEFT] << " " << tempValue[SIDE_UP] << " " << tempValue[SIDE_DOWN];

        for(int i = 0; i < 2; i++)
        {
            if(tempValue[hList[i]] >= max_main_speed)
            {
                tempValue[hList[i]] = max_main_speed;
            }
            else if(tempValue[hList[i]] <= -max_main_speed)
            {
                tempValue[hList[i]] = -max_main_speed;
            }
        }
        for(int i = 2; i < 4; i++)
        {
            if(tempValue[hList[i]] >= max_side_speed)
            {
                tempValue[hList[i]] = max_side_speed;
            }
            else if(tempValue[hList[i]] <= -max_side_speed)
            {
                tempValue[hList[i]] = -max_side_speed;
            }
        }


    }
    else if(status.cnt[0] > 25)
    {
        img.drum_dx_last = 999;
        img.drum_dy_last = 999;

        loadConfig(FORWARD_ACOS);
        qDebug() << "AcosRevise";

        static double tmp_acos_theta1_last = 0;
        if(tmp_acos_theta1 - tmp_acos_theta1_last != 0)
        {
            tmp_acos_theta1_last = tmp_acos_theta1;

            qDebug() << "use acos.theta1:  " << tmp_acos_theta1;

            float acosError = 90 - tmp_acos_theta1;
            if(acosError > 180)
            {
                acosError = -360 + acosError;
            }
            else if(acosError < -180)
            {
                acosError = 360 + acosError;
            }

            sbg.goal = sbg.yaw+acosError;

        }

        float sbgError = sbg.goal-sbg.yaw;
        if(sbgError > 180)
        {
            sbgError = -360+sbgError;
        }
        if(sbgError < -180)
        {
            sbgError = 360+sbgError;
        }

        float sbgDiff = sbgError-sbg.yawErrorLast;
        sbg.yawErrorLast = sbgError;


        // const int max_main_speed = 37;
        const int max_main_speed = acos_max_main_speed;
        const int max_side_speed = 35;

        qDebug() << "sbgError:" << sbgError;
        if (sbgError < 0)
        {
            tempValue[MAIN_LEFT] = max_main_speed + status.val[MAIN_LEFT].p*sbgError + status.val[MAIN_LEFT].d*sbgDiff;
            tempValue[MAIN_RIGHT] = max_main_speed;

            tempValue[SIDE_UP] = 25 - status.val[SIDE_UP].p*sbgError - status.val[SIDE_DOWN].d*sbgDiff;
            tempValue[SIDE_DOWN] = -15 + status.val[SIDE_UP].p*sbgError + status.val[SIDE_DOWN].d*sbgDiff;
        }
        else if (sbgError >= 0)
        {
            tempValue[MAIN_LEFT] = max_main_speed;
            tempValue[MAIN_RIGHT] = max_main_speed - status.val[MAIN_RIGHT].p*sbgError - status.val[MAIN_RIGHT].d*sbgDiff;

            tempValue[SIDE_UP] = -25 - status.val[SIDE_UP].p*sbgError - status.val[SIDE_DOWN].d*sbgDiff;
            tempValue[SIDE_DOWN] = 15 + status.val[SIDE_UP].p*sbgError + status.val[SIDE_DOWN].d*sbgDiff;
        }

        for(int i = 0; i < 2; i++)
        {
            if(tempValue[hList[i]] >= max_main_speed)
            {
                tempValue[hList[i]] = max_main_speed;
            }
            else if(tempValue[hList[i]] <= -max_main_speed)
            {
                tempValue[hList[i]] = -max_main_speed;
            }
        }
        for(int i = 2; i < 4; i++)
        {
            if(tempValue[hList[i]] >= max_side_speed)
            {
                tempValue[hList[i]] = max_side_speed;
            }
            else if(tempValue[hList[i]] <= -max_side_speed)
            {
                tempValue[hList[i]] = -max_side_speed;
            }
        }

    }
    else if(status.cnt[4] >= 20 && flag_first_acos == true)
    {
        loadConfig(FORWARD_ACOS);
        qDebug() << "AcosRevise";

        flag_start_recognzing_drum = true;

        static double tmp_acos_theta1_last = 0;
        if(tmp_acos_theta1 - tmp_acos_theta1_last != 0)
        {
            tmp_acos_theta1_last = tmp_acos_theta1;

            qDebug() << "use acos.theta1:  " << tmp_acos_theta1;

            float acosError = 90 - tmp_acos_theta1;
            if(acosError > 180)
            {
                acosError = -360 + acosError;
            }
            else if(acosError < -180)
            {
                acosError = 360 + acosError;
            }
            sbg.goal = sbg.yaw+acosError;

        }

        float sbgError = sbg.goal-sbg.yaw;
        if(sbgError > 180)
        {
            sbgError = -360+sbgError;
        }
        if(sbgError < -180)
        {
            sbgError = 360+sbgError;
        }

        float sbgDiff = sbgError-sbg.yawErrorLast;
        sbg.yawErrorLast = sbgError;

        const int max_main_speed = acos_max_main_speed;
        const int max_side_speed = 35;

        qDebug() << "sbgError:" << sbgError;
        if (sbgError < 0)
        {
            tempValue[MAIN_LEFT] = max_main_speed + status.val[MAIN_LEFT].p*sbgError + status.val[MAIN_LEFT].d*sbgDiff;
            tempValue[MAIN_RIGHT] = max_main_speed;

            tempValue[SIDE_UP] = 15 - status.val[SIDE_UP].p*sbgError - status.val[SIDE_DOWN].d*sbgDiff;
            tempValue[SIDE_DOWN] = -15 + status.val[SIDE_UP].p*sbgError + status.val[SIDE_DOWN].d*sbgDiff;
        }
        else if (sbgError >= 0)
        {
            tempValue[MAIN_LEFT] = max_main_speed;
            tempValue[MAIN_RIGHT] = max_main_speed - status.val[MAIN_RIGHT].p*sbgError - status.val[MAIN_RIGHT].d*sbgDiff;

            tempValue[SIDE_UP] = -15 - status.val[SIDE_UP].p*sbgError - status.val[SIDE_DOWN].d*sbgDiff;
            tempValue[SIDE_DOWN] = 15 + status.val[SIDE_UP].p*sbgError + status.val[SIDE_DOWN].d*sbgDiff;
        }

        for(int i = 0; i < 2; i++)
        {
            if(tempValue[hList[i]] >= max_main_speed)
            {
                tempValue[hList[i]] = max_main_speed;
            }
            else if(tempValue[hList[i]] <= -max_main_speed)
            {
                tempValue[hList[i]] = -max_main_speed;
            }
        }
        for(int i = 2; i < 4; i++)
        {
            if(tempValue[hList[i]] >= max_side_speed)
            {
                tempValue[hList[i]] = max_side_speed;
            }
            else if(tempValue[hList[i]] <= -max_side_speed)
            {
                tempValue[hList[i]] = -max_side_speed;
            }
        }
    }


    QList<pair<MOTORS,float>> tempList;
    for(int i = 0; i < 4; i++)
    {
        tempList.push_back(make_pair<>(hList[i],tempValue[hList[i]]));
    }
    emit setHMotors(tempList);

    status.cnt[5]++;
    if(status.cnt[5] >= 600)
    {
        emit endTask();
    }
//    if(999!=tmp.m3_drum_dx &&
//       999!=tmp.m3_drum_dy &&
//       status.cnt[1]>=10)emit enterAction(DRUMREVISE);
}

void controller::endAcosRevise()
{
    status.cnt.clear();
    status.finished = ACOSREVISE;
    qDebug() << "End AcosRevise!";
}

void controller::initDrumRevise()
{
    loadConfig(LOCATE_BOTTOM);
    status.cnt.push_back(0);
    setFrameInteval(status.ms);
    qDebug() << "DrumRevise";
}
void controller::ctrDrumRevise()
{
    status.cnt[0]++;
    visionClass::visionData && tmp = vision->getData();
    qDebug()<<"DRUMREVISE"<<status.cnt[0];
    const static MOTORS hList[4]={MAIN_LEFT,MAIN_RIGHT,SIDE_UP,SIDE_DOWN};
    const static MOTORS zList[2]={SIDE_UP,SIDE_DOWN};
    float tempValue[NUMBER_OF_MOTORS];
    float delta_t=(img.t_now-img.t_last)/1000.0;
    img.drum3_dx_last=img.drum3_dx;
    img.drum3_dy_last=img.drum3_dy;
    img.ball_dx_last=img.ball_dx;
    img.ball_dy_last=img.ball_dy;
    img.drum3_dx=tmp.m3_drum_dx;
    img.drum3_dy=tmp.m3_drum_dy;
    img.ball_dx=tmp.m3_ball_dx;
    img.ball_dy=tmp.m3_ball_dy;
    img.drum3_dx_diff=img.drum3_dx-img.drum3_dx_last;
    img.drum3_dy_diff=img.drum3_dy-img.drum3_dy_last;
    img.ball_dx_diff=img.ball_dx-img.ball_dx_last;
    img.ball_dy_diff=img.ball_dy-img.ball_dy_last;
    img.angle=tmp.m3_rotate_cw_angle;
    //img.t_last=img.t_now;
    //img.t_now=tmp.t_now;
    float rev_ball_dx=1;
    float rev_ball_dy=1;
    if(abs(img.ball_dx)<90 && abs(img.ball_dx)>50){
        rev_ball_dx=1.5;
    }
    if(abs(img.ball_dy+10)<90 && abs(img.ball_dy+10)>50){
        rev_ball_dy=1.5;
    }
    tempValue[MAIN_LEFT]=status.val[MAIN_LEFT].p*rev_ball_dy*img.ball_dy+status.val[MAIN_LEFT].d*rev_ball_dy*img.ball_dy_diff/delta_t;
    tempValue[MAIN_RIGHT]=status.val[MAIN_RIGHT].p*rev_ball_dy*img.ball_dy+status.val[MAIN_RIGHT].d*rev_ball_dy*img.ball_dy_diff/delta_t;
    tempValue[SIDE_UP]=status.val[SIDE_UP].p*rev_ball_dx*img.ball_dx+status.val[SIDE_UP].d*rev_ball_dx*img.ball_dx_diff/delta_t;
    tempValue[SIDE_DOWN]=status.val[SIDE_DOWN].p*rev_ball_dx*img.ball_dx+status.val[SIDE_DOWN].d*rev_ball_dx*img.ball_dx_diff/delta_t;
    for(int i=0;i<4;i++){
        if(tempValue[hList[i]]>=35){
            tempValue[hList[i]]=35;
        }
        else if(tempValue[hList[i]]<=-35){
            tempValue[hList[i]]=-35;
        }
    }
    for(int i=0;i<2;i++){
        if(tempValue[zList[i]]>=30){
            tempValue[zList[i]]=30;
        }
        else if(tempValue[zList[i]]<=-30){
            tempValue[zList[i]]=-30;
        }
    }

    QList<pair<MOTORS,float>> tempList;
    for(int i=0;i<4;i++){
        tempList.push_back(make_pair<>(hList[i],tempValue[hList[i]]));
    }
    emit setHMotors(tempList);
    img.drum3_dx = 999;
    img.drum3_dy = 999;
    if(!(999!=img.drum3_dx && 999!=img.drum3_dy))emit stopAction();
    if(999!=img.ball_dx && 999!=img.ball_dy)emit enterAction(BALLREVISE);
}
void controller::endDrumRevise()
{
    status.cnt.clear();
    status.finished = DRUMREVISE;
    qDebug() << "End DrumRevise!";
}

void controller::initBallRevise()
{
    loadConfig(LOCATE_BOTTOM);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    setFrameInteval(status.ms);
    qDebug() << "BallRevise!";
}

void controller::ctrBallRevise()
{
    status.cnt[0]++;
    visionClass::visionData && tmp=vision->getData();
    qDebug()<<"BALLREVISE"<<status.cnt[0];
    const static MOTORS hList[4]={MAIN_LEFT,MAIN_RIGHT,SIDE_UP,SIDE_DOWN};
    float tempValue[NUMBER_OF_MOTORS];

    float delta_t=(img.t_now-img.t_last)/1000.0;

    static int pushed_repeat_cnt = 0;
    static std::deque<double> deq_acos_theta1(1);

    const int pushed_repeat_cnt_max = 5;   //acos Hz: 1Hz, controller Hz: 5Hz
    const int deq_max_length = 5;

    if(acos.theta1 == acos.theta1Last)
    {
        pushed_repeat_cnt++;
    }
    else if(pushed_repeat_cnt > pushed_repeat_cnt_max || acos.theta1 != acos.theta1Last)
    {
        deq_acos_theta1.push_back(acos.theta1);
        qDebug() << "raw acos.theta1 pushed back: " << acos.theta1;
        pushed_repeat_cnt = 1;
    }

    acos.theta1Last = acos.theta1;

    if(deq_acos_theta1.size() > deq_max_length)
    {
        while (deq_acos_theta1.size() > deq_max_length) {
            deq_acos_theta1.pop_front();
        }
    }

    qDebug() << "deque elems:";
    for(int i = 0; i < deq_acos_theta1.size(); i++)
    {
        qDebug() << deq_acos_theta1.at(i);
    }
    qDebug();

    std::deque<double> tmp_deq_acos_theta1 = deq_acos_theta1;
    std::sort(tmp_deq_acos_theta1.begin(), tmp_deq_acos_theta1.end());

    double tmp_acos_theta1 = tmp_deq_acos_theta1.at((tmp_deq_acos_theta1.size()-1)/2>=0 ? (tmp_deq_acos_theta1.size()-1)/2 : 0);

    qDebug() << "Choose acos.theta1:   " << tmp_acos_theta1;


    static bool flag_start_recognzing_ball = true;

    static bool flag_first_acos = true;


    if(tmp.m3_ball_dx != 999 && tmp.m3_ball_dy != 999)
    {
        qDebug << "Found the ball, Now Recognizing";

        status.cnt[0] = 0;

        loadConfig(LOCATE_BOTTOM);
        flag_first_acos = false;
        if(flag_start_recognzing_ball)
        {
            sbg.goal = sbg.yaw;
            flag_start_recognzing_ball = false;
        }

        float sbgError=sbg.goal-sbg.yaw;
        if(sbgError > 180)
        {
            sbgError = -360+sbgError;
        }
        else if(sbgError < -180)
        {
            sbgError = 360+sbgError;

        }
        qDebug() << "Error" << sbgError;

        float sbgDiff = sbgError-sbg.yawErrorLast;
        sbg.yawErrorLast = sbgError;
        
        img.drum3_dx_last = img.drum3_dx;
        img.drum3_dy_last = img.drum3_dy;
        img.ball_dx_last = img.ball_dx;
        img.ball_dy_last = img.ball_dy;

        img.drum3_dx = tmp.m3_drum_dx;
        img.drum3_dy = tmp.m3_drum_dy;
        img.ball_dx = tmp.m3_ball_dx;
        img.ball_dy = tmp.m3_ball_dy;

        img.drum3_dx_diff = img.drum3_dx-img.drum3_dx_last;
        img.drum3_dy_diff = img.drum3_dy-img.drum3_dy_last;
        img.ball_dx_diff = img.ball_dx-img.ball_dx_last;
        img.ball_dy_diff = img.ball_dy-img.ball_dy_last;

        if(abs(img.ball_dx)<37/10.0 && abs(img.ball_dy)<37/10.0 && abs(img.ball_dx_diff)<37/10.0 && abs(img.ball_dy_diff)<37/10.0)
        {
           status.cnt[1]++;
           qDebug() << "begin catch stable timer" << status.cnt[1];
        }
        else
        {
            qDebug() << "Not Recognized";
            if(status.cnt[1] > 0)
            {
                status.cnt[1]--;
            }
            else
            {
                status.cnt[1] = 0;
            }
            
        }

        if(status.cnt[1] >= 35)
        {
            tempValue[MAIN_LEFT]=0;
            tempValue[MAIN_RIGHT]=0;
            tempValue[SIDE_UP]=0;
            tempValue[SIDE_DOWN]=0;

            qDebug() << "begin to open the Sevo";
            emit setSevoOpen();
            status.cnt[6]++;
            if(status.cnt[6] >= 5)
            {
                emit endTask();
            }
        }

        const int max_main_speed = drum_max_main_speed;
        const int max_side_speed = 40;
        
        if(img.ball_dy > 0)
        {
            tempValue[MAIN_LEFT] = 20*((img.ball_dy)/fabs(img.ball_dy)) + status.val[MAIN_LEFT].p*img.ball_dy + status.val[MAIN_LEFT].d*img.ball_dy_diff;
            tempValue[MAIN_RIGHT] = 20*((img.ball_dy)/fabs(img.ball_dy)) + status.val[MAIN_LEFT].p*img.ball_dy + status.val[MAIN_LEFT].d*img.ball_dy_diff;
        }
        else if(img.ball_dy < 0)
        {
            tempValue[MAIN_LEFT] = 4*((img.ball_dy)/fabs(img.ball_dy)) + status.val[MAIN_LEFT].p*img.ball_dy + status.val[MAIN_LEFT].d*img.ball_dy_diff;
            tempValue[MAIN_RIGHT] = 6*((img.ball_dy)/fabs(img.ball_dy)) + status.val[MAIN_LEFT].p*img.ball_dy + status.val[MAIN_LEFT].d*img.ball_dy_diff;
        }
        else
        {
            tempValue[MAIN_LEFT] = 0 + status.val[MAIN_LEFT].p*img.ball_dy + status.val[MAIN_LEFT].d*img.ball_dy_diff;
            tempValue[MAIN_RIGHT] = 0 + status.val[MAIN_LEFT].p*img.ball_dy + status.val[MAIN_LEFT].d*img.ball_dy_diff;
        }

        if (img.ball_dx > 0)
        {
            tempValue[SIDE_UP] = 10*((img.ball_dx)/fabs(img.ball_dx)) + status.val[SIDE_UP].p*img.ball_dx + status.val[SIDE_UP].d*img.ball_dx_diff;
            tempValue[SIDE_DOWN] = 4*((img.ball_dx)/fabs(img.ball_dx)) + status.val[SIDE_DOWN].p*img.ball_dx + status.val[SIDE_DOWN].d*img.ball_dx_diff;
        }
        else if (img.ball_dx < 0)
        {
            tempValue[SIDE_UP] = 4*((img.ball_dx)/fabs(img.ball_dx)) + status.val[SIDE_UP].p*img.ball_dx + status.val[SIDE_UP].d*img.ball_dx_diff;
            tempValue[SIDE_DOWN] = 6*((img.ball_dx)/fabs(img.ball_dx)) + status.val[SIDE_DOWN].p*img.ball_dx + status.val[SIDE_DOWN].d*img.ball_dx_diff;
        }
        else
        {
            tempValue[SIDE_UP] = 0 + status.val[SIDE_UP].p*img.ball_dx + status.val[SIDE_UP].d*img.ball_dx_diff;
            tempValue[SIDE_DOWN] = 0 + status.val[SIDE_DOWN].p*img.ball_dx + status.val[SIDE_DOWN].d*img.ball_dx_diff;
        }
        for(int i = 0; i < 2; i++)
        {
            if(tempValue[hList[i]] >= max_main_speed)
            {
                tempValue[hList[i]] = max_main_speed;
            }
            else if(tempValue[hList[i]] <= -max_main_speed)
            {
                tempValue[hList[i]] = -max_main_speed;
            }
        }
        for(int i = 2; i < 4; i++)
        {
            if(tempValue[hList[i]] >= max_side_speed)
            {
                tempValue[hList[i]] = max_side_speed;
            }
            else if(tempValue[hList[i]] <= -max_side_speed)
            {
                tempValue[hList[i]] = -max_side_speed;
            }
        }
    }
    else if(img.ball_dx_last  !=999 && img.ball_dy_last != 999 && status.cnt[0] <= 25)
    {
        status.cnt[0]++;

        loadconfig(locate_bottom);
        qdebug()<<"found the drum, now use vision recognizing";

        img.drum3_dx_last = img.drum3_dx;
        img.drum3_dy_last = img.drum3_dy;
        img.ball_dx_last = img.ball_dx;
        img.ball_dy_last = img.ball_dy;

        img.drum3_dx = tmp.m3_drum_dx;
        img.drum3_dy = tmp.m3_drum_dy;
        img.ball_dx = tmp.m3_ball_dx;
        img.ball_dy = tmp.m3_ball_dy;

        img.drum3_dx_diff = img.drum3_dx-img.drum3_dx_last;
        img.drum3_dy_diff = img.drum3_dy-img.drum3_dy_last;
        img.ball_dx_diff = img.ball_dx-img.ball_dx_last;
        img.ball_dy_diff = img.ball_dy-img.ball_dy_last;

        const int max_main_speed = drum_max_main_speed;
        const int max_side_speed = 40;
        
        if(img.ball_dy > 0)
        {
            tempValue[MAIN_LEFT] = 20*((img.ball_dy)/fabs(img.ball_dy)) + status.val[MAIN_LEFT].p*img.ball_dy + status.val[MAIN_LEFT].d*img.ball_dy_diff;
            tempValue[MAIN_RIGHT] = 20*((img.ball_dy)/fabs(img.ball_dy)) + status.val[MAIN_LEFT].p*img.ball_dy + status.val[MAIN_LEFT].d*img.ball_dy_diff;
        }
        else if(img.ball_dy < 0)
        {
            tempValue[MAIN_LEFT] = 4*((img.ball_dy)/fabs(img.ball_dy)) + status.val[MAIN_LEFT].p*img.ball_dy + status.val[MAIN_LEFT].d*img.ball_dy_diff;
            tempValue[MAIN_RIGHT] = 6*((img.ball_dy)/fabs(img.ball_dy)) + status.val[MAIN_LEFT].p*img.ball_dy + status.val[MAIN_LEFT].d*img.ball_dy_diff;
        }
        else
        {
            tempValue[MAIN_LEFT] = 0 + status.val[MAIN_LEFT].p*img.ball_dy + status.val[MAIN_LEFT].d*img.ball_dy_diff;
            tempValue[MAIN_RIGHT] = 0 + status.val[MAIN_LEFT].p*img.ball_dy + status.val[MAIN_LEFT].d*img.ball_dy_diff;
        }

        if (img.ball_dx > 0)
        {
            tempValue[SIDE_UP] = 10*((img.ball_dx)/fabs(img.ball_dx)) + status.val[SIDE_UP].p*img.ball_dx + status.val[SIDE_UP].d*img.ball_dx_diff;
            tempValue[SIDE_DOWN] = 4*((img.ball_dx)/fabs(img.ball_dx)) + status.val[SIDE_DOWN].p*img.ball_dx + status.val[SIDE_DOWN].d*img.ball_dx_diff;
        }
        else if (img.ball_dx < 0)
        {
            tempValue[SIDE_UP] = 4*((img.ball_dx)/fabs(img.ball_dx)) + status.val[SIDE_UP].p*img.ball_dx + status.val[SIDE_UP].d*img.ball_dx_diff;
            tempValue[SIDE_DOWN] = 6*((img.ball_dx)/fabs(img.ball_dx)) + status.val[SIDE_DOWN].p*img.ball_dx + status.val[SIDE_DOWN].d*img.ball_dx_diff;
        }
        else
        {
            tempValue[SIDE_UP] = 0 + status.val[SIDE_UP].p*img.ball_dx + status.val[SIDE_UP].d*img.ball_dx_diff;
            tempValue[SIDE_DOWN] = 0 + status.val[SIDE_DOWN].p*img.ball_dx + status.val[SIDE_DOWN].d*img.ball_dx_diff;
        }
        for(int i = 0; i < 2; i++)
        {
            if(tempValue[hList[i]] >= max_main_speed)
            {
                tempValue[hList[i]] = max_main_speed;
            }
            else if(tempValue[hList[i]] <= -max_main_speed)
            {
                tempValue[hList[i]] = -max_main_speed;
            }
        }
        for(int i = 2; i < 4; i++)
        {
            if(tempValue[hList[i]] >= max_side_speed)
            {
                tempValue[hList[i]] = max_side_speed;
            }
            else if(tempValue[hList[i]] <= -max_side_speed)
            {
                tempValue[hList[i]] = -max_side_speed;
            }
        }
    }
    else if(status.cnt[0] >25)
    {
        img.drum_dx_last = 999;
        img.drum_dy_last = 999;

        loadConfig(FORWARD_ACOS);
        qDebug() << "AcosRevise";

        static double tmp_acos_theta1_last = 0;
        if(tmp_acos_theta1 - tmp_acos_theta1_last != 0)
        {
            tmp_acos_theta1_last = tmp_acos_theta1;

            qDebug() << "use acos.theta1:  " << tmp_acos_theta1;

            float acosError = 90 - tmp_acos_theta1;
            if(acosError > 180)
            {
                acosError = -360 + acosError;
            }
            else if(acosError < -180)
            {
                acosError = 360 + acosError;
            }

            sbg.goal = sbg.yaw+acosError;

        }

        float sbgError = sbg.goal-sbg.yaw;
        if(sbgError > 180)
        {
            sbgError = -360+sbgError;
        }
        if(sbgError < -180)
        {
            sbgError = 360+sbgError;
        }

        float sbgDiff = sbgError-sbg.yawErrorLast;
        sbg.yawErrorLast = sbgError;


        // const int max_main_speed = 37;
        const int max_main_speed = acos_max_main_speed;
        const int max_side_speed = 35;

        qDebug() << "sbgError:" << sbgError;
        if (sbgError < 0)
        {
            tempValue[MAIN_LEFT] = max_main_speed + status.val[MAIN_LEFT].p*sbgError + status.val[MAIN_LEFT].d*sbgDiff;
            tempValue[MAIN_RIGHT] = max_main_speed;

            tempValue[SIDE_UP] = 25 - status.val[SIDE_UP].p*sbgError - status.val[SIDE_DOWN].d*sbgDiff;
            tempValue[SIDE_DOWN] = -15 + status.val[SIDE_UP].p*sbgError + status.val[SIDE_DOWN].d*sbgDiff;
        }
        else if (sbgError >= 0)
        {
            tempValue[MAIN_LEFT] = max_main_speed;
            tempValue[MAIN_RIGHT] = max_main_speed - status.val[MAIN_RIGHT].p*sbgError - status.val[MAIN_RIGHT].d*sbgDiff;

            tempValue[SIDE_UP] = -25 - status.val[SIDE_UP].p*sbgError - status.val[SIDE_DOWN].d*sbgDiff;
            tempValue[SIDE_DOWN] = 15 + status.val[SIDE_UP].p*sbgError + status.val[SIDE_DOWN].d*sbgDiff;
        }

        for(int i = 0; i < 2; i++)
        {
            if(tempValue[hList[i]] >= max_main_speed)
            {
                tempValue[hList[i]] = max_main_speed;
            }
            else if(tempValue[hList[i]] <= -max_main_speed)
            {
                tempValue[hList[i]] = -max_main_speed;
            }
        }
        for(int i = 2; i < 4; i++)
        {
            if(tempValue[hList[i]] >= max_side_speed)
            {
                tempValue[hList[i]] = max_side_speed;
            }
            else if(tempValue[hList[i]] <= -max_side_speed)
            {
                tempValue[hList[i]] = -max_side_speed;
            }
        }

    }
    else if(status.cnt[4] >= 20 && flag_first_acos == true)
    {
        loadConfig(FORWARD_ACOS);
        qDebug() << "AcosRevise";


        flag_start_recognzing_ball = true;


        static double tmp_acos_theta1_last = 0;
        if(tmp_acos_theta1 - tmp_acos_theta1_last != 0)
        {
            tmp_acos_theta1_last = tmp_acos_theta1;

            qDebug() << "use acos.theta1:  " << tmp_acos_theta1;

            float acosError = 90 - tmp_acos_theta1;
            if(acosError > 180)
            {
                acosError = -360 + acosError;
            }
            else if(acosError < -180)
            {
                acosError = 360 + acosError;
            }

            sbg.goal = sbg.yaw+acosError;

        }

        float sbgError = sbg.goal-sbg.yaw;
        if(sbgError > 180)
        {
            sbgError = -360+sbgError;
        }
        if(sbgError < -180)
        {
            sbgError = 360+sbgError;
        }

        float sbgDiff = sbgError-sbg.yawErrorLast;
        sbg.yawErrorLast = sbgError;


        // const int max_main_speed = 37;
        const int max_main_speed = acos_max_main_speed;
        const int max_side_speed = 35;

        qDebug() << "sbgError:" << sbgError;
        if (sbgError < 0)
        {
            tempValue[MAIN_LEFT] = max_main_speed + status.val[MAIN_LEFT].p*sbgError + status.val[MAIN_LEFT].d*sbgDiff;
            tempValue[MAIN_RIGHT] = max_main_speed;

            tempValue[SIDE_UP] = 15 - status.val[SIDE_UP].p*sbgError - status.val[SIDE_DOWN].d*sbgDiff;
            tempValue[SIDE_DOWN] = -15 + status.val[SIDE_UP].p*sbgError + status.val[SIDE_DOWN].d*sbgDiff;
        }
        else if (sbgError >= 0)
        {
            tempValue[MAIN_LEFT] = max_main_speed;
            tempValue[MAIN_RIGHT] = max_main_speed - status.val[MAIN_RIGHT].p*sbgError - status.val[MAIN_RIGHT].d*sbgDiff;

            tempValue[SIDE_UP] = -15 - status.val[SIDE_UP].p*sbgError - status.val[SIDE_DOWN].d*sbgDiff;
            tempValue[SIDE_DOWN] = 15 + status.val[SIDE_UP].p*sbgError + status.val[SIDE_DOWN].d*sbgDiff;
        }

        for(int i = 0; i < 2; i++)
        {
            if(tempValue[hList[i]] >= max_main_speed)
            {
                tempValue[hList[i]] = max_main_speed;
            }
            else if(tempValue[hList[i]] <= -max_main_speed)
            {
                tempValue[hList[i]] = -max_main_speed;
            }
        }
        for(int i = 2; i < 4; i++)
        {
            if(tempValue[hList[i]] >= max_side_speed)
            {
                tempValue[hList[i]] = max_side_speed;
            }
            else if(tempValue[hList[i]] <= -max_side_speed)
            {
                tempValue[hList[i]] = -max_side_speed;
            }
        }

    }
    else
    {
        status.cnt[4]++;

        tempValue[MAIN_LEFT] = 0;
        tempValue[MAIN_RIGHT] = 0;
        tempValue[SIDE_UP] = 0;
        tempValue[SIDE_DOWN] = 0;
    }



    QList<pair<MOTORS,float>> tempList;
    for(int i=0;i<4;i++){
        tempList.push_back(make_pair<>(hList[i],tempValue[hList[i]]));
    }
    emit setHMotors(tempList);
    if(!(img.ball_dx!=999 && img.ball_dy!=999))emit stopAction();
}

void controller::endBallRevise()
{
    status.cnt.clear();
    status.finished = BALLREVISE;
    qDebug() << "End BallRevise!";
}

void controller::initRotateAction()
{
    loadConfig(ROTATING);
    updateConfig();
    sbg.goal=sbg.yaw+img.angle;
    if(sbg.goal>180){
        sbg.goal-=360;
    }
    else if(sbg.goal<-180){
        sbg.goal+=360;
    }
    status.cnt.push_back(0);
    setFrameInteval(status.ms);
    qDebug() << "Rotate!";
}
void controller::ctrRotateAction()
{
    status.cnt[0]++;
    const static MOTORS hList[4]={MAIN_LEFT,MAIN_RIGHT,SIDE_UP,SIDE_DOWN};
    float tempValue[NUMBER_OF_MOTORS];
    float sbgError=sbg.goal-sbg.yaw;
    float sbgDiff=sbgError-sbg.yawErrorLast;
    float sbgDiffT=(sbg.tNow-sbg.tLast)/1000.0;
    sbg.yawErrorLast=sbgError;
    if(abs(sbgError<5) && abs(sbgDiff<5))
    {
        status.cnt[1]++;
        if(status.cnt[1]>status.curPara.PlanCount)emit returnAction(ROTATEACTION);
    }
    else{
        status.cnt[1]--;
        if(status.cnt[1]<0){
            status.cnt[1]=0;
        }
    }
    tempValue[MAIN_LEFT]=status.val[MAIN_LEFT].p*sbgError/100.0
             +status.val[MAIN_LEFT].d*sbgDiff/sbgDiffT;
    tempValue[MAIN_RIGHT]=status.val[MAIN_RIGHT].p*sbgError/100.0
             +status.val[MAIN_RIGHT].d*sbgDiff/sbgDiffT;

    if(abs((int)sbgError)>=5){
         tempValue[SIDE_UP]=status.val[SIDE_UP].p*sbgError/100.0
                  +status.val[SIDE_UP].d*sbgDiff/sbgDiffT;
         tempValue[SIDE_DOWN]=status.val[SIDE_DOWN].p*sbgError/100.0
                  +status.val[SIDE_DOWN].d*sbgDiff/sbgDiffT;
     }
    for(int i=0;i<4;i++){
        if(tempValue[hList[i]]>=45){
            tempValue[hList[i]]=45;
        }
        else if(tempValue[hList[i]]<=-45){
            tempValue[hList[i]]=-45;
        }
    }

    QList<pair<MOTORS,float>> tempList;
    for(int i=0;i<4;i++){
        tempList.push_back(make_pair<>(hList[i],tempValue[hList[i]]));
    }
    emit setHMotors(tempList);
}
void controller::endRotateAction()
{
    status.cnt.clear();
    status.finished = ROTATEACTION;
    qDebug() << "End Rotate!";
}

void controller::initDivingAction()
{
    emit setGoal(1.4);
    loadConfig(HANG);
    updateConfig();
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    setFrameInteval(status.ms);
    qDebug() << "Diving Action!";
}
void controller::ctrDivingAction()
{
    status.cnt[0]++;
    emit setGoal(1.4);
    loadConfig(HANG);
    updateConfig();
    visionClass::visionData && tmp=vision->getData();
    qDebug()<<"DivingAction"<<status.cnt[0];
    const static MOTORS hList[4]={MAIN_LEFT,MAIN_RIGHT,SIDE_UP,SIDE_DOWN};
    const static MOTORS zList[2]={SIDE_UP,SIDE_DOWN};
    float tempValue[NUMBER_OF_MOTORS];
    float delta_t=(img.t_now-img.t_last)/1000.0;
    img.drum3_dx_last=img.drum3_dx;
    img.drum3_dy_last=img.drum3_dy;
    img.ball_dx_last=img.ball_dx;
    img.ball_dy_last=img.ball_dy;
    img.drum3_dx=tmp.m3_drum_dx;
    img.drum3_dy=tmp.m3_drum_dy;
    img.ball_dx=tmp.m3_ball_dx;
    img.ball_dy=tmp.m3_ball_dy;
    img.drum3_dx_diff=img.drum3_dx-img.drum3_dx_last;
    img.drum3_dy_diff=img.drum3_dy-img.drum3_dy_last;
    img.ball_dx_diff=img.ball_dx-img.ball_dx_last;
    img.ball_dy_diff=img.ball_dy-img.ball_dy_last;
    img.angle=tmp.m3_rotate_cw_angle;
    //img.t_last=img.t_now;
    //img.t_now=tmp.t_now;
    if(999!=img.ball_dx && 999!=img.ball_dy)
    {
        qDebug()<<"Find the Ball!";
        loadConfig(LOCATE_BOTTOM);
        float rev=0.5;
        tempValue[MAIN_LEFT]=status.val[MAIN_LEFT].p*rev*img.ball_dy+status.val[MAIN_LEFT].d*rev*img.ball_dy_diff/delta_t;
        tempValue[MAIN_RIGHT]=status.val[MAIN_RIGHT].p*rev*img.ball_dy+status.val[MAIN_RIGHT].d*rev*img.ball_dy_diff/delta_t;
        tempValue[SIDE_UP]=status.val[SIDE_UP].p*rev*img.ball_dx+status.val[SIDE_UP].d*rev*img.ball_dx_diff/delta_t;
        tempValue[SIDE_DOWN]=status.val[SIDE_DOWN].p*rev*img.ball_dx+status.val[SIDE_DOWN].d*rev*img.ball_dx_diff/delta_t;
        if(abs(deep.value-1.4)<0.05)
        {
            qDebug()<<"Deep value available";
            status.cnt[2]++;
        }
        else
        {
            qDebug()<<"Deep value uncompleted";
            status.cnt[2]--;
            if(status.cnt[2]<0)status.cnt[2]=0;
        }
        if(abs(img.ball_dx)     <45 &&
           abs(img.ball_dy)     <65 &&
           abs(img.ball_dx_diff)<20 &&
           abs(img.ball_dy_diff)<20 &&
                status.cnt[2]>=50)
        {
            qDebug()<<"ImgRevise successful,prepare to down the Arm";
            status.cnt[1]++;
            tempValue[MAIN_LEFT]=0;
            tempValue[MAIN_RIGHT]=0;
            tempValue[SIDE_UP]=0;
            tempValue[SIDE_DOWN]=0;
            emit DownStepToVision(true);//定深结束后给图像发信号
            emit enterAction(ARM_DOWN);
            /*
             qDebug()<<"New ImgRevise successful,Prepare to grub";
             status.cnt[1]++;
             if(status.cnt[1]>=20)
             {
                 emit DownStepToVision(true);//定深结束后给图像发信号
                 currentActionList.pop();
                 emit enterAction(GETTINGACTION);
             }
             */
        //else status.cnt[1]=0;
        }
    }
    else {
        qDebug()<<"Not Find Ball";
        loadConfig(HANG);
        emit setGoal(1.4);
        updateConfig();
        //status.cnt[1]=0;
    }
    status.cnt[3]++;
    if(status.cnt[3]>400)
    {
        qDebug()<<"Ball Found Failed,Abandon";
        tried++;
        loadConfig(HANG);
        setGoal(0.5);
        updateConfig();
        emit enterAction(ABANDON);
    }
    for(int i=0;i<4;i++){
        if(tempValue[hList[i]]>=25){
            tempValue[hList[i]]=25;
        }
        else if(tempValue[hList[i]]<=-25){
            tempValue[hList[i]]=-25;
        }
    }
    for(int i=0;i<2;i++){
        if(tempValue[zList[i]]>=20){
            tempValue[zList[i]]=20;
        }
        else if(tempValue[zList[i]]<=-20){
            tempValue[zList[i]]=-20;
        }
    }

    QList<pair<MOTORS,float>> tempList;
    for(int i=0;i<4;i++){
        tempList.push_back(make_pair<>(hList[i],tempValue[hList[i]]));
    }
    emit setHMotors(tempList);
}
void controller::endDivingAction()
{
    status.cnt.clear();
    status.finished = DIVINGACTION;
    qDebug() << "End Diving Action!";
}
void controller::initBackwardDrum()
{
    emit setGoal(1.4);
    loadConfig(HANG);
    updateConfig();
    status.cnt.push_back(0);
    setFrameInteval(status.ms);
    qDebug()<<"Backward Drum!";
}
void controller::ctrBackwardDrum()
{
    status.cnt[0]++;
    qDebug()<<"BACKWARD DRUM"<<status.cnt[0];
    loadConfig(BACKWARD);
    //visionClass::visionData && tmp = vision->getData();
    if(status.cnt[0] == 1)sbg.goal = sbg.yaw;
    const static MOTORS hList[4]={MAIN_LEFT,MAIN_RIGHT,SIDE_UP,SIDE_DOWN};
    float tempValue[NUMBER_OF_MOTORS];
    static float sbgErrorI;
    float sbgError=sbg.goal-sbg.yaw;
    if(sbgError>180){
        sbgError=-360+sbgError;
    }
    if(sbgError<-180){
        sbgError=360+sbgError;
    }
    sbgErrorI += sbgError;
    float sbgDiffT=(sbg.tNow-sbg.tLast)/1000.0;
    float sbgDiff=sbgError-sbg.yawErrorLast;
    sbg.yawErrorLast=sbgError;
    tempValue[MAIN_LEFT]=-30+status.val[MAIN_LEFT].p*sbgError/100.0
                +status.val[MAIN_LEFT].i*sbgErrorI/100.0
                +status.val[MAIN_LEFT].d*sbgDiff/sbgDiffT;
    tempValue[MAIN_RIGHT]=-30+status.val[MAIN_RIGHT].p*sbgError/100.0
                +status.val[MAIN_RIGHT].i*sbgErrorI/100.0
                +status.val[MAIN_RIGHT].d*sbgDiff/sbgDiffT;
    tempValue[MAIN_LEFT]=-30;
    tempValue[MAIN_LEFT]=-30;
    tempValue[SIDE_UP]=0;
    tempValue[SIDE_DOWN]=0;
    for(int i=0;i<4;i++){
        if(tempValue[hList[i]]>=35){
            tempValue[hList[i]]=35;
        }
        else if(tempValue[hList[i]]<=-35){
            tempValue[hList[i]]=-35;
        }
    }
    QList<pair<MOTORS,float>> tempList;
    for(int i=0;i<4;i++){
        tempList.push_back(make_pair<>(hList[i],tempValue[hList[i]]));
    }
    emit setHMotors(tempList);
    if(status.cnt[0]>=20)emit enterAction(GETTINGACTION);
}
void controller::endBackwardDrum()
{
    status.cnt.clear();
    status.finished = BACKWARDDRUM;
    qDebug() << "End Backward Drum!";
}

void controller::initGettingAction()
{
    emit setGoal(1.8);
    loadConfig(HANG);
    updateConfig();
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    setFrameInteval(status.ms);
    qDebug()<<"Getting Action!";
}
void controller::ctrGettingAction()
{
    status.cnt[0]++;
    emit setGoal(1.7);
    loadConfig(HANG);
    updateConfig();
    visionClass::visionData && tmp=vision->getData();
    qDebug()<<"GettingAction"<<status.cnt[0];
    img.drum3_dx_last=img.drum3_dx;
    img.drum3_dy_last=img.drum3_dy;
    img.ball_dx_last=img.ball_dx;
    img.ball_dy_last=img.ball_dy;
    img.drum3_dx=tmp.m3_drum_dx;
    img.drum3_dy=tmp.m3_drum_dy;
    img.ball_dx=tmp.m3_ball_dx;
    img.ball_dy=tmp.m3_ball_dy;
    img.drum3_dx_diff=img.drum3_dx-img.drum3_dx_last;
    img.drum3_dy_diff=img.drum3_dy-img.drum3_dy_last;
    img.ball_dx_diff=img.ball_dx-img.ball_dx_last;
    img.ball_dy_diff=img.ball_dy-img.ball_dy_last;
    img.angle=tmp.m3_rotate_cw_angle;
    float delta_t=(img.t_now-img.t_last)/1000.0;
    float tempValue[NUMBER_OF_MOTORS];
    //img.t_last=img.t_now;
    //img.t_now=tmp.t_now;
    loadConfig(LOCATE_BOTTOM);
    float rev=0.5;
    tempValue[MAIN_LEFT]=status.val[MAIN_LEFT].p*rev*img.ball_dy+status.val[MAIN_LEFT].d*rev*img.ball_dy_diff/delta_t;
    tempValue[MAIN_RIGHT]=status.val[MAIN_RIGHT].p*rev*img.ball_dy+status.val[MAIN_RIGHT].d*rev*img.ball_dy_diff/delta_t;
    tempValue[SIDE_UP]=status.val[SIDE_UP].p*rev*img.ball_dx+status.val[SIDE_UP].d*rev*img.ball_dx_diff/delta_t;
    tempValue[SIDE_DOWN]=status.val[SIDE_DOWN].p*rev*img.ball_dx+status.val[SIDE_DOWN].d*rev*img.ball_dx_diff/delta_t;
    const static MOTORS hList[4]={MAIN_LEFT,MAIN_RIGHT,SIDE_UP,SIDE_DOWN};
    if(abs(deep.value-1.8)<0.05)
    {
        qDebug()<<"Final Deep Control Successful";
        status.cnt[1]++;
    }
    else
    {
        qDebug()<<"Final Deep Control Failed";
        status.cnt[1]--;
        if(status.cnt[1]<0)status.cnt[1]=0;
    }
    if(status.cnt[1]>=30){
        qDebug()<<"Arm Downed,Forward to Grab the Ball";
        loadConfig(FORWARD_SLOW);
        if(status.cnt[0]==1)sbg.goal=sbg.yaw;
        static float sbgErrorI;
        float sbgError=sbg.goal-sbg.yaw;
        if(sbgError>180){
            sbgError=-360+sbgError;
        }
        if(sbgError<-180){
            sbgError=360+sbgError;
        }
        sbgErrorI += sbgError;
        float sbgDiffT=(sbg.tNow-sbg.tLast)/1000.0;
        float sbgDiff=sbgError-sbg.yawErrorLast;
        sbg.yawErrorLast=sbgError;

        tempValue[MAIN_LEFT]=30+status.val[MAIN_LEFT].p*sbgError/100.0
                    +status.val[MAIN_LEFT].i*sbgErrorI/100.0
                    +status.val[MAIN_LEFT].d*sbgDiff/sbgDiffT;
        tempValue[MAIN_RIGHT]=30+status.val[MAIN_RIGHT].p*sbgError/100.0
                    +status.val[MAIN_RIGHT].i*sbgErrorI/100.0
                    +status.val[MAIN_RIGHT].d*sbgDiff/sbgDiffT;
        tempValue[MAIN_LEFT]=30;
        tempValue[MAIN_RIGHT]=31;
        tempValue[SIDE_UP]=0;
        tempValue[SIDE_DOWN]=0;
    }
    if(status.cnt[1]==50){
        qDebug()<<"Grab!";
        loadConfig(HANG);
        tempValue[MAIN_LEFT]=0;
        tempValue[MAIN_RIGHT]=0;
        tempValue[SIDE_UP]=0;
        tempValue[SIDE_DOWN]=0;
        emit setSevoClose();
        emit CloseSevoToVision(true);
    }
    else if(tmp.m3_ball_acquire!=-999 && status.cnt[1]>50){
        qDebug()<<"Not Found Ball in the hand,Hanging...";
        loadConfig(HANG);
        tempValue[MAIN_LEFT]=0;
        tempValue[MAIN_RIGHT]=0;
        tempValue[SIDE_UP]=0;
        tempValue[SIDE_DOWN]=0;
        if(status.cnt[0]>100){
            tried++;
            qDebug()<<"Not Found Ball in the hand finally,Abandon"<<tried;
            emit setSevoOpen();
            loadConfig(HANG);
            emit setGoal(0.8);
            updateConfig();
            emit enterAction(ABANDON);
        }
    }
    else if(tmp.m3_ball_acquire==-999){
        qDebug()<<"Found ball in the hand,BRAVO!";
        qDebug()<<"Can we see this moment?";
        loadConfig(HANG);
        emit setGoal(0);
        updateConfig();
        currentActionList.pop();
        //emit enterAction(GRABACTION);
        emit endTask();
    }
    for(int i=0;i<4;i++){
        if(tempValue[hList[i]]>=40){
            tempValue[hList[i]]=40;
        }
        else if(tempValue[hList[i]]<=-40){
            tempValue[hList[i]]=-40;
        }
    }

    QList<pair<MOTORS,float>> tempList;
    for(int i=0;i<4;i++){
        tempList.push_back(make_pair<>(hList[i],tempValue[hList[i]]));
    }
    emit setHMotors(tempList);
}
void controller::endGettingAction()
{
    status.cnt.clear();
    status.finished = GETTINGACTION;
    qDebug() << "End Getting Action!";
}
void controller::initGrabAction()
{
    emit setGoal(0.75);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    setFrameInteval(status.ms);
}
void controller::ctrGrabAction()
{
    status.cnt[0]++;
    float tempValue[NUMBER_OF_MOTORS];
    qDebug()<<"GRABACTION"<<status.cnt[0];
    const static MOTORS hList[4]={MAIN_LEFT,MAIN_RIGHT,SIDE_UP,SIDE_DOWN};
    if(status.cnt[0]>50){
        //应该先旋转到指定角度
        loadConfig(ROTATING);
        static float sbgErrorI;
        float sbgError=sbg.goal-sbg.yaw;
        if(sbgError>180){
            sbgError=-360+sbgError;
        }
        if(sbgError<-180){
            sbgError=360+sbgError;
        }
        sbgErrorI += sbgError;
        float sbgDiffT=(sbg.tNow-sbg.tLast)/1000.0;
        float sbgDiff=sbgError-sbg.yawErrorLast;
        sbg.yawErrorLast=sbgError;

        tempValue[SIDE_UP]=status.val[MAIN_LEFT].p*sbgError/100.0
                    +status.val[MAIN_LEFT].i*sbgErrorI/100.0
                    +status.val[MAIN_LEFT].d*sbgDiff/sbgDiffT;
        tempValue[SIDE_DOWN]=status.val[MAIN_RIGHT].p*sbgError/100.0
                    +status.val[MAIN_RIGHT].i*sbgErrorI/100.0
                    +status.val[MAIN_RIGHT].d*sbgDiff/sbgDiffT;
        tempValue[MAIN_LEFT]=0;
         tempValue[MAIN_RIGHT]=0;
    }
    if(status.cnt[0]>status.curPara.PlanCount){
        //status.cnt[0]=0;
        //waiting++;
        //emit grabIt(true,50);
        emit endTask();
    }
    for(int i=0;i<4;i++){
        if(tempValue[hList[i]]>=45){
            tempValue[hList[i]]=45;
        }
        else if(tempValue[hList[i]]<=-45){
            tempValue[hList[i]]=-45;
        }
    }

    QList<pair<MOTORS,float>> tempList;
    for(int i=0;i<4;i++){
        tempList.push_back(make_pair<>(hList[i],tempValue[hList[i]]));
    }
    emit setHMotors(tempList);
}
void controller::endGrabAction()
{
    status.cnt.clear();
    status.finished = GRABACTION;
    qDebug() << "End Grab Action!";
}
void controller::initAbandon()
{
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    setGoal(0.8);
    setFrameInteval(status.ms);
    qDebug()<<"ABANON START!";
}
void controller::ctrAbandon()
{
    status.cnt[0]++;
    if(status.cnt[0]==1 && statusStack.top().currentTask->id == GETTINGACTION)emit setStepUp();
    if(tried<3){
        qDebug()<<"Try once more";
        if(abs(deep.value-0.8)<0.05){
            status.cnt[1]++;
            qDebug()<<"ABANDON COUNT"<<status.cnt[1];
            if(status.cnt[1]>40){
                emit DownStepToVision(false);
                emit CloseSevoToVision(false);
                //emit returnAction(ABANDON);
                emit endTask();
            }
        }
    }
    else {
        qDebug()<<"No time to try,float";
        qDebug()<<"As The Last Result";
        emit endTask();
    }
}
void controller::endAbandon()
{
    status.cnt.clear();
    status.finished = GRABACTION;
    qDebug() << "End Abandon";
}

void controller::onSetSevo(bool ok,int pulse)
{
    emit setSevo(ok,pulse);
}
void controller::onSetStep(bool ok, int ms)
{
    bool direction;
    direction = (ms>=0)?false:true;
    emit setStep(ok,abs(ms),false,direction);
}

//2016 USA
/*
void controller::initDeepTest()
{
    emit setGoal(0.5);
    loadConfig(HANG);
    updateConfig();
    setDeepCtr(true);
    status.cnt.push_back(0); //使用一个定时器
    qDebug() << "Inited!";
}

void controller::ctrDeepTest()
{
    status.cnt[0]++;
    //qDebug() << status.cnt[0];
    if(status.cnt[0]*status.ms>=100*1000){
        emit endTask();
        return;
    }
}
void controller::endDeepTest()
{
    status.cnt.clear();
    //loadConfig(STOP);
    setZero();
    setDeepCtr(false);
    qDebug() << "Stopped!";
}

void controller::initStart()
{
    status.cnt.push_back(0); //使用一个定时器
    qDebug() << "Start!";
}

void controller::ctrStart()
{
    float deep1=deep.value;
    if(deep1>=0.2){
        setZero();
        emit endTask();
        return;
    }
}

void controller::endStart()
{
    status.cnt.clear();
    //loadConfig(STOP);
    setZero();
    qDebug() << "Stopped!";
    //emit setAutoDeep(false);
}

void controller::initYawTest()
{
    emit setGoal(1.5);
    loadConfig(HANG);
    updateConfig();
    setDeepCtr(true);
    sbg.goal=sbg.yaw;
    loadConfig(FORWARD_SLOW);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    qDebug() << "YawTest!";
}

void controller::ctrYawTest()
{
    status.cnt[0]++;
    if(status.cnt[0]*status.ms>=71*1000 && status.cnt[0]*status.ms<80*1000){
        static float acosyaw;
        status.cnt[1]++;
        if(status.cnt[1]*status.ms<=4*2000){
            if(abs(acos.theta)>=60){
                acos.theta=0;
                acosyaw+=acos.theta;
            }
        }
        else if(status.cnt[1]*status.ms>4*2000){
            status.cnt[2]++;
            if(status.cnt[2]<=1){
                 acosyaw=acosyaw/40;
                 task.direction=acosyaw;
                 qDebug()<<"acosyaw:"<<acosyaw;
            }
        }
    }
    if(status.cnt[0]*status.ms>=80*1000){
        setZero();
        emit endTask();
        return;
    }

    float tempValue[NUMBER_OF_MOTORS];
    const static MOTORS hList[4]={MAIN_LEFT,MAIN_RIGHT,SIDE_UP,SIDE_DOWN};
    static float sbgErrorI;
    float sbgError=sbg.goal-sbg.yaw;
    if(sbgError>190){
        sbgError=-360+sbgError;
    }
    if(sbgError<-190){
        sbgError=360+sbgError;
    }
    sbgErrorI += sbgError;
    float sbgDiffT=(sbg.tNow-sbg.tLast)/1000.0;
    float sbgDiff=sbgError-sbg.yawErrorLast;
    sbg.yawErrorLast=sbgError;

    tempValue[MAIN_LEFT]=32+status.val[MAIN_LEFT].p*sbgError/100.0
                +status.val[MAIN_LEFT].i*sbgErrorI/100.0
                +status.val[MAIN_LEFT].d*sbgDiff/sbgDiffT;
    tempValue[MAIN_RIGHT]=32+status.val[MAIN_RIGHT].p*sbgError/100.0
                +status.val[MAIN_RIGHT].i*sbgErrorI/100.0
                +status.val[MAIN_RIGHT].d*sbgDiff/sbgDiffT;
    if(abs((int)sbgError)>=5){
        tempValue[SIDE_UP]=status.val[SIDE_UP].p*sbgError/100.0
                +status.val[SIDE_UP].i*sbgErrorI/100.0
                +status.val[SIDE_UP].d*sbgDiff/sbgDiffT;
         tempValue[SIDE_UP]=status.val[SIDE_DOWN].p*sbgError/100.0
                +status.val[SIDE_DOWN].i*sbgErrorI/100.0
                +status.val[SIDE_DOWN].d*sbgDiff/sbgDiffT;
    }
    tempValue[SIDE_UP]=0;
     tempValue[SIDE_DOWN]=0;
    for(int i=0;i<4;i++){
        if(tempValue[hList[i]]>=45){
            tempValue[hList[i]]=45;
        }
        else if(tempValue[hList[i]]<=-45){
            tempValue[hList[i]]=-45;
        }
    }

    QList<pair<MOTORS,float>> tempList;
    for(int i=0;i<4;i++){
        tempList.push_back(make_pair<>(hList[i],tempValue[hList[i]]));
    }
    emit setHMotors(tempList);
}

void controller::endYawTest()
{
    status.cnt.clear();
    //loadConfig(STOP);
    setZero();
    setDeepCtr(false);
    qDebug()<<"EndYaw";
}

void controller::initYawTest1()
{
    emit setGoal(1.5);
    loadConfig(HANG);
    updateConfig();
    setDeepCtr(true);
    loadConfig(FORWARD_SLOW);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    qDebug() << "YawTest1!";
}

void controller::ctrYawTest1()
{
    status.cnt[0]++;
    if(status.cnt[0]*status.ms>=50*1000){
        setZero();
        emit endTask();
        return;
    }

    float tempValue[NUMBER_OF_MOTORS];
    const static MOTORS hList[4]={MAIN_LEFT,MAIN_RIGHT,SIDE_UP,SIDE_DOWN};
    float sbgError=sbg.goal-sbg.yaw;
    if(sbgError>190){
        sbgError=-360+sbgError;
    }
    if(sbgError<-190){
        sbgError=360+sbgError;
    }
    float sbgDiffT=(sbg.tNow-sbg.tLast)/1000.0;
    float sbgDiff=sbgError-sbg.yawErrorLast;
    sbg.yawErrorLast=sbgError;

    tempValue[MAIN_LEFT]=32+status.val[MAIN_LEFT].p*sbgError/100.0
            +status.val[MAIN_LEFT].d*sbgDiff/sbgDiffT;
    tempValue[MAIN_RIGHT]=32+status.val[MAIN_RIGHT].p*sbgError/100.0
            +status.val[MAIN_RIGHT].d*sbgDiff/sbgDiffT;

    if(abs((int)sbgError)>=5){
        tempValue[SIDE_UP]=status.val[SIDE_UP].p*sbgError/100.0
                +status.val[SIDE_UP].d*sbgDiff/sbgDiffT;
        tempValue[SIDE_DOWN]=status.val[SIDE_DOWN].p*sbgError/100.0
                +status.val[SIDE_DOWN].d*sbgDiff/sbgDiffT;
    }

    for(int i=0;i<4;i++){
        if(tempValue[hList[i]]>=45){
            tempValue[hList[i]]=45;
        }
        else if(tempValue[hList[i]]<=-45){
            tempValue[hList[i]]=-45;
        }
    }

    QList<pair<MOTORS,float>> tempList;
    for(int i=0;i<4;i++){
        tempList.push_back(make_pair<>(hList[i],tempValue[hList[i]]));
    }

    emit setHMotors(tempList);
}

void controller::endYawTest1()
{
    status.cnt.clear();
    //loadConfig(STOP);
    setZero();
    setDeepCtr(false);
    qDebug()<<"EndYaw1";
}

void controller::initForward()
{
    emit setGoal(1.5);
    loadConfig(HANG);
    updateConfig();
    setDeepCtr(true);
    status.cnt.push_back(0); //使用一个定时器
    qDebug() << "InitForward!";
}

void controller::ctrForward()
{
    status.cnt[0]++;
    float tempValue[NUMBER_OF_MOTORS];
    const static MOTORS hList[4]={MAIN_LEFT,MAIN_RIGHT,SIDE_UP,SIDE_DOWN};
    tempValue[MAIN_LEFT]=33.5;
    tempValue[MAIN_RIGHT]=35;
    tempValue[SIDE_UP]=0;
    tempValue[SIDE_DOWN]=0;
    if(status.cnt[0]*status.ms>=40*1000){
        setZero();
        emit endTask();
        return;
    }

    QList<pair<MOTORS,float>> tempList;
    for(int i=0;i<4;i++){
        tempList.push_back(make_pair<>(hList[i],tempValue[hList[i]]));
    }

    emit setHMotors(tempList);

}

void controller::endForward()
{
    status.cnt.clear();
    //loadConfig(STOP);
    setZero();
    setDeepCtr(false);
    qDebug() << "EndForward!";
}

void controller::initForward1()
{
    emit setGoal(1.5);
    loadConfig(HANG);
    updateConfig();
    setDeepCtr(true);
    status.cnt.push_back(0); //使用一个定时器
    qDebug() << "InitForward1!";
}

void controller::ctrForward1()
{
    status.cnt[0]++;
    float tempValue[NUMBER_OF_MOTORS];
    const static MOTORS hList[4]={MAIN_LEFT,MAIN_RIGHT,SIDE_UP,SIDE_DOWN};
    if(status.cnt[0]*status.ms<=2*1000){
        tempValue[MAIN_LEFT]=0;
        tempValue[MAIN_RIGHT]=0;
        tempValue[SIDE_UP]=-35;
        tempValue[SIDE_DOWN]=35;
    }

    if(status.cnt[0]*status.ms>2*1000 && status.cnt[0]*status.ms<40*1000){
        tempValue[MAIN_LEFT]=35;
        tempValue[MAIN_RIGHT]=35;
        tempValue[SIDE_UP]=0;
        tempValue[SIDE_DOWN]=0;
    }

    if(status.cnt[0]*status.ms>=40*1000){
        setZero();
        emit endTask();
        return;
    }

    QList<pair<MOTORS,float>> tempList;
    for(int i=0;i<4;i++){
        tempList.push_back(make_pair<>(hList[i],tempValue[hList[i]]));
    }

    emit setHMotors(tempList);

}

void controller::endForward1()
{
    status.cnt.clear();
    //loadConfig(STOP);
    setZero();
    setDeepCtr(false);
    qDebug() << "EndForward1!";
}

void controller::initGoup()
{
    status.cnt.push_back(0);
    qDebug() << "Go up!";
}

void controller::ctrGoup()
{
    float tempValue[NUMBER_OF_MOTORS];
    const static MOTORS hList[4]={LEFT_UP,LEFT_DOWN,RIGHT_UP,RIGHT_DOWN};
    QList<pair<MOTORS,float>> tempList;
    status.cnt[0]++;
    tempValue[LEFT_UP]=0;
    tempValue[LEFT_DOWN]=0;
    tempValue[RIGHT_UP]=0;
    tempValue[RIGHT_DOWN]=0;

    for(int i=0;i<4;i++){
        tempList.push_back(make_pair<>(hList[i],tempValue[hList[i]]));
    }
    emit setVMotors(tempList);
    if(status.cnt[0]*status.ms>=50){
       if(task.cnt==1){
          QList<TASKS_ID> idList1;
          idList1.push_back(RANDOM_PINGER);
          for(auto t:idList1){
              auto p=taskList.find(t);
              if(p!=taskList.end()){
                    currentTaskList.push(p->second);
              }
           }
           task.list=0;
           task.cnt=2;
       }
       setZero1();
       emit endTask();
       return;
    }
}

void controller::endGoup()
{
    status.cnt.clear();
    //loadConfig(STOP);
    setZero();
}

void controller::initPathDir()
{
    img.angle_goal=0;
    emit setGoal(1.5);
    loadConfig(HANG);
    updateConfig();
    setDeepCtr(true);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    emit missionStarted(PATH_DIR);
    qDebug()<<"initPathDir";
}

void controller::ctrPathDir()
{
    float tempValue[NUMBER_OF_MOTORS];
    const static MOTORS hList[4]={MAIN_LEFT,MAIN_RIGHT,SIDE_UP,SIDE_DOWN};
    float direction;

    status.cnt[5]++;
    if(status.cnt[5]*status.ms>=120*1000)
    {
        QList<TASKS_ID> idList1;
        idList1.push_back(PASS_GATE);
        for(auto t:idList1){
            auto p=taskList.find(t);
            if(p!=taskList.end()){
                  currentTaskList.push(p->second);
            }
        }
        setZero();
        emit endTask();
        return;
    }

    visionClass::visionData && tmp=vision->getData();
    if(tmp.t_now==img.t_now){
        return;
    }

    if(tmp.m1_path_angle==999 || tmp.m1_path_dx==999 || tmp.m1_path_dy==999){
        //处理没有找到的情况。现在未完工
        qDebug() << "Not found";
        status.cnt[6]++;
        if(status.cnt[6]*status.ms>=20*1000)
        {
            QList<TASKS_ID> idList1;
            idList1.push_back(RANDOM_PINGER);
            for(auto t:idList1){
                auto p=taskList.find(t);
                if(p!=taskList.end()){
                      currentTaskList.push(p->second);
                }
             }
            setZero();
            emit endTask();
            return;
        }
        if(FORWARD_SLOW!=status.cnt[2]){
            return;
        }
        return;
    }

    img.path_angle_last=img.path_angle;
    img.path_dx_last=img.path_dx;
    img.path_dy_last=img.path_dy;
    img.t_last=img.t_now;
    img.path_angle=tmp.m1_path_angle;
    img.path_dx=tmp.m1_path_dx;
    img.path_dy=tmp.m1_path_dy;
    img.t_now=tmp.t_now;
    status.cnt[3]++;
    if(status.cnt[3]<=1){
       direction=sbg.yaw;
    }

    float img_angleError=img.angle_goal-img.path_angle;
    static float img_angleErrorI;
    img_angleErrorI += img_angleError;
    float img_angleDiffT=(img.t_now-img.t_last)/1000.0;
    float img_angleDiff=img_angleError-img.path_angle_Errorlast;
    img.path_angle_Errorlast=img_angleError;

    float img_dxError=img.path_dx;
    float img_dxDiffT=(img.t_now-img.t_last)/1000.0;
    float img_dxDiff=img_dxError-img.path_dx_last;
    img.path_dx_last=img.path_dx;

    float img_dyError=img.path_dy;
    float img_dyDiffT=(img.t_now-img.t_last)/1000.0;
    float img_dyDiff=img_dyError-img.path_dy_last;
    img.path_dy_last=img.path_dy;

    if(status.cnt[2]==FORWARD_SLOW)
    {
        status.cnt[1]=0;
        status.cnt[6]=0;
        float dirError=direction-sbg.goal;
        if(dirError>180){
             dirError=-360+dirError;
        }
        else if(dirError<-180){
             dirError=360+dirError;
        }
        if(abs(dirError)>90){
             sbg.goal=sbg.goal+180;
             if(sbg.goal>180){
                sbg.goal=-360+sbg.goal;
             }
             float sbgError=sbg.goal-sbg.yaw;
             if(sbgError>190){
                sbgError=-360+sbgError;
             }
             if(sbgError<-190){
                sbgError=360+sbgError;
             }
             float sbgDiffT=(sbg.tNow-sbg.tLast)/1000.0;
             float sbgDiff=sbgError-sbg.yawErrorLast;
             sbg.yawErrorLast=sbgError;

             if(abs(sbgError)>5 && status.cnt[0]<1){
                 tempValue[MAIN_LEFT]=0;
                 tempValue[MAIN_RIGHT]=0;
                 tempValue[SIDE_UP]=status.val[SIDE_UP].p*sbgError/100.0
                         +status.val[SIDE_UP].d*sbgDiff/sbgDiffT;
                 tempValue[SIDE_DOWN]=status.val[SIDE_DOWN].p*sbgError/100.0
                         +status.val[SIDE_DOWN].d*sbgDiff/sbgDiffT;
             }
             else if(abs(sbgError)<=5){
                 tempValue[MAIN_LEFT]=32+status.val[MAIN_LEFT].p*sbgError/100.0
                           +status.val[MAIN_LEFT].d*sbgDiff/sbgDiffT;
                 tempValue[MAIN_RIGHT]=32+status.val[MAIN_RIGHT].p*sbgError/100.0
                           +status.val[MAIN_RIGHT].d*sbgDiff/sbgDiffT;
                 tempValue[SIDE_UP]=status.val[SIDE_UP].p*sbgError/100.0
                           +status.val[SIDE_UP].d*sbgDiff/sbgDiffT;
                 tempValue[SIDE_DOWN]=status.val[SIDE_DOWN].p*sbgError/100.0
                           +status.val[SIDE_DOWN].d*sbgDiff/sbgDiffT;
                 status.cnt[0]++;
                 if(status.cnt[0]*status.ms>=10*1000)
                 {
                      setZero();
                      emit endTask();
                      return;
                 }
             }
        }
        else
        {
              float sbgError=sbg.goal-sbg.yaw;
              if(sbgError>190){
                 sbgError=-360+sbgError;
              }
              if(sbgError<-190){
                 sbgError=360+sbgError;
              }
              float sbgDiffT=(sbg.tNow-sbg.tLast)/1000.0;
              float sbgDiff=sbgError-sbg.yawErrorLast;
              sbg.yawErrorLast=sbgError;

              tempValue[MAIN_LEFT]=32+status.val[MAIN_LEFT].p*sbgError/100.0
                        +status.val[MAIN_LEFT].d*sbgDiff/sbgDiffT;
              tempValue[MAIN_RIGHT]=32+status.val[MAIN_RIGHT].p*sbgError/100.0
                        +status.val[MAIN_RIGHT].d*sbgDiff/sbgDiffT;

              if(abs((int)sbgError)>=5){
                    tempValue[SIDE_UP]=status.val[SIDE_UP].p*sbgError/100.0
                            +status.val[SIDE_UP].d*sbgDiff/sbgDiffT;
                    tempValue[SIDE_DOWN]=status.val[SIDE_DOWN].p*sbgError/100.0
                            +status.val[SIDE_DOWN].d*sbgDiff/sbgDiffT;
               }
              status.cnt[0]++;
              if(status.cnt[0]*status.ms>=10*1000)
              {
                   setZero();
                   emit endTask();
                   return;
              }
         }
    }
    else if(abs(img.path_dx)<=75 && abs(img.path_dy)<=100 && abs(img.path_angle)<=8)
    {
        status.cnt[1]++;
        status.cnt[6]=0;
        if(status.cnt[1]>=5)
        {
           status.cnt[2]=FORWARD_SLOW;
           sbg.goal=sbg.yaw;
           loadConfig(FORWARD_SLOW);
        }
    }
    else if(abs(img.path_dx)>75 || abs(img.path_dy)>100 || abs(img.path_angle)<=8)
    {
        status.cnt[1]=0;
        status.cnt[2]=LOCATE_BOTTOM;
        loadConfig(LOCATE_BOTTOM);
        //updateConfig();
        tempValue[MAIN_LEFT]=status.val[MAIN_LEFT].p*img_dyError/100.0
                +status.val[MAIN_LEFT].d*img_dyDiff/img_dyDiffT;
        tempValue[MAIN_RIGHT]=status.val[MAIN_RIGHT].p*img_dyError/100.0
                +status.val[MAIN_RIGHT].d*img_dyDiff/img_dyDiffT;

        tempValue[SIDE_UP]=status.val[SIDE_UP].p*img_dxError/100.0
                +status.val[SIDE_UP].d*img_dxDiff/img_dxDiffT;
        tempValue[SIDE_DOWN]=status.val[SIDE_DOWN].p*img_dxError/100.0
                +status.val[SIDE_DOWN].d*img_dxDiff/img_dxDiffT;

    }
    else if(abs(img.path_dx)<=75 && abs(img.path_dy)<=100)
    {
        status.cnt[1]=0;
        status.cnt[6]=0;
        status.cnt[2]=ROTATING;
        loadConfig(ROTATING);
        //updateConfig();
        tempValue[MAIN_LEFT]=0;
        tempValue[MAIN_RIGHT]=0;
        tempValue[SIDE_UP]=status.val[SIDE_UP].p*img_angleError/100.0
                +status.val[SIDE_UP].i*img_angleErrorI/100.0
                +status.val[SIDE_UP].d*img_angleDiff/img_angleDiffT;
        tempValue[SIDE_DOWN]=status.val[SIDE_DOWN].p*img_angleError/100.0
                +status.val[SIDE_DOWN].i*img_angleErrorI/100.0
                +status.val[SIDE_DOWN].d*img_angleDiff/img_angleDiffT;

    }
    for(int i=0;i<4;i++){
        if(tempValue[hList[i]]>=45){
            tempValue[hList[i]]=45;
        }
        else if(tempValue[hList[i]]<=-45){
            tempValue[hList[i]]=-45;
        }
    }

    QList<pair<MOTORS,float>> tempList;
    for(int i=0;i<4;i++){
        tempList.push_back(make_pair<>(hList[i],tempValue[hList[i]]));
    }

    emit setHMotors(tempList);

}

void controller::endPathDir()
{
    status.cnt.clear();
    //loadConfig(STOP);
    setZero();
    setDeepCtr(false);
    emit missionFinished(PATH_DIR);
    qDebug()<<"endPathDir";
}

void controller::initPathDir1()
{
    img.angle_goal=0;
    emit setGoal(1.5);
    loadConfig(HANG);
    updateConfig();
    setDeepCtr(true);
    status.cnt.push_back(0);//time
    status.cnt.push_back(0);//
    status.cnt.push_back(0);//state_flag
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    emit missionStarted(PATH_DIR);
    qDebug() << "InitPathDir1";
}

void controller::ctrPathDir1()
{
    float tempValue[NUMBER_OF_MOTORS];
    const static MOTORS hList[4]={MAIN_LEFT,MAIN_RIGHT,SIDE_UP,SIDE_DOWN};
    float direction;

    visionClass::visionData && tmp=vision->getData();
    if(tmp.t_now==img.t_now){
        return;
    }

    if(tmp.m1_path_angle==999 || tmp.m1_path_dx==999 || tmp.m1_path_dy==999){
        //处理没有找到的情况。现在未完工
        qDebug() << "Not found";
        if(status.cnt[2]==0){
            float sbgError=sbg.goal-sbg.yaw;
            if(sbgError>190){
                sbgError=-360+sbgError;
            }
            if(sbgError<-190){
                sbgError=360+sbgError;
            }
            float sbgDiffT=(sbg.tNow-sbg.tLast)/1000.0;
            float sbgDiff=sbgError-sbg.yawErrorLast;
            sbg.yawErrorLast=sbgError;

            tempValue[MAIN_LEFT]=32+status.val[MAIN_LEFT].p*sbgError/100.0
                    +status.val[MAIN_LEFT].d*sbgDiff/sbgDiffT;
            tempValue[MAIN_RIGHT]=32+status.val[MAIN_RIGHT].p*sbgError/100.0
                    +status.val[MAIN_RIGHT].d*sbgDiff/sbgDiffT;

            if(abs((int)sbgError)>=5){
                tempValue[SIDE_UP]=status.val[SIDE_UP].p*sbgError/100.0
                        +status.val[SIDE_UP].d*sbgDiff/sbgDiffT;
                tempValue[SIDE_DOWN]=status.val[SIDE_DOWN].p*sbgError/100.0
                        +status.val[SIDE_DOWN].d*sbgDiff/sbgDiffT;
            }
        }
        if(FORWARD_SLOW!=status.cnt[2]){
            return;
        }
    }

    img.path_angle_last=img.path_angle;
    img.path_dx_last=img.path_dx;
    img.path_dy_last=img.path_dy;
    img.t_last=img.t_now;
    img.path_angle=tmp.m1_path_angle;
    img.path_dx=tmp.m1_path_dx;
    img.path_dy=tmp.m1_path_dy;
    img.t_now=tmp.t_now;
    status.cnt[3]++;
    if(status.cnt[3]<=1){
       direction=sbg.yaw;
    }

    float img_angleError=img.angle_goal-img.path_angle;
    static float img_angleErrorI;
    img_angleErrorI += img_angleError;
    float img_angleDiffT=(img.t_now-img.t_last)/1000.0;
    float img_angleDiff=img_angleError-img.path_angle_Errorlast;
    img.path_angle_Errorlast=img_angleError;

    float img_dxError=img.path_dx;
    float img_dxDiffT=(img.t_now-img.t_last)/1000.0;
    float img_dxDiff=img_dxError-img.path_dx_last;
    img.path_dx_last=img.path_dx;

    float img_dyError=img.path_dy;
    float img_dyDiffT=(img.t_now-img.t_last)/1000.0;
    float img_dyDiff=img_dyError-img.path_dy_last;
    img.path_dy_last=img.path_dy;

    if(status.cnt[2]==FORWARD_SLOW)
    {
        float dirError=direction-sbg.goal;
        if(dirError>180){
            dirError=-360+dirError;
        }
        else if(dirError<-180){
            dirError=360+dirError;
        }
        if(abs(dirError)>90){
            sbg.goal=sbg.goal+180;
            if(sbg.goal>180){
                sbg.goal=-360+sbg.goal;
            }
            float sbgError=sbg.goal-sbg.yaw;
            if(sbgError>190){
                sbgError=-360+sbgError;
            }
            if(sbgError<-190){
                sbgError=360+sbgError;
            }
            float sbgDiffT=(sbg.tNow-sbg.tLast)/1000.0;
            float sbgDiff=sbgError-sbg.yawErrorLast;
            sbg.yawErrorLast=sbgError;

            if(abs(sbgError)>5 && status.cnt[0]<1){
                 tempValue[MAIN_LEFT]=0;
                 tempValue[MAIN_RIGHT]=0;
                 tempValue[SIDE_UP]=status.val[SIDE_UP].p*sbgError/100.0
                           +status.val[SIDE_UP].d*sbgDiff/sbgDiffT;
                 tempValue[SIDE_DOWN]=status.val[SIDE_DOWN].p*sbgError/100.0
                           +status.val[SIDE_DOWN].d*sbgDiff/sbgDiffT;
            }
            else if(abs(sbgError)<=5 || status.cnt[0]>=1){
                 tempValue[MAIN_LEFT]=32+status.val[MAIN_LEFT].p*sbgError/100.0
                          +status.val[MAIN_LEFT].d*sbgDiff/sbgDiffT;
                 tempValue[MAIN_RIGHT]=32+status.val[MAIN_RIGHT].p*sbgError/100.0
                          +status.val[MAIN_RIGHT].d*sbgDiff/sbgDiffT;
                 tempValue[SIDE_UP]=status.val[SIDE_UP].p*sbgError/100.0
                          +status.val[SIDE_UP].d*sbgDiff/sbgDiffT;
                 tempValue[SIDE_DOWN]=status.val[SIDE_DOWN].p*sbgError/100.0
                          +status.val[SIDE_DOWN].d*sbgDiff/sbgDiffT;
                 status.cnt[0]++;
                 if(status.cnt[0]*status.ms<=15*1000){
                      static float acosyaw;
                      status.cnt[4]++;
                      if(status.cnt[4]*status.ms<=4*2000){
                          acosyaw+=acos.theta;
                      }
                      else if(status.cnt[4]*status.ms>4*2000){
                          status.cnt[5]++;
                          if(status.cnt[5]<=1){
                               acosyaw=acosyaw/4;
                               task.direction=acosyaw;
                               qDebug()<<"acosyaw:"<<acosyaw;
                           }
                       }
                  }
                  else if(status.cnt[0]*status.ms>15*1000)
                  {
                      setZero();
                      emit endTask();
                      return;
                  }
             }
        }
        else
        {
              float sbgError=sbg.goal-sbg.yaw;
              if(sbgError>190){
                  sbgError=-360+sbgError;
              }
              if(sbgError<-190){
                  sbgError=360+sbgError;
              }
              float sbgDiffT=(sbg.tNow-sbg.tLast)/1000.0;
              float sbgDiff=sbgError-sbg.yawErrorLast;
              sbg.yawErrorLast=sbgError;

              tempValue[MAIN_LEFT]=32+status.val[MAIN_LEFT].p*sbgError/100.0
                       +status.val[MAIN_LEFT].d*sbgDiff/sbgDiffT;
              tempValue[MAIN_RIGHT]=32+status.val[MAIN_RIGHT].p*sbgError/100.0
                       +status.val[MAIN_RIGHT].d*sbgDiff/sbgDiffT;

              if(abs((int)sbgError)>=5){
                   tempValue[SIDE_UP]=status.val[SIDE_UP].p*sbgError/100.0
                            +status.val[SIDE_UP].d*sbgDiff/sbgDiffT;
                   tempValue[SIDE_DOWN]=status.val[SIDE_DOWN].p*sbgError/100.0
                            +status.val[SIDE_DOWN].d*sbgDiff/sbgDiffT;
               }
               status.cnt[0]++;
               if(status.cnt[0]*status.ms<=10*1000){
                   static float acosyaw;
                   float direction1;
                   status.cnt[4]++;
                   if(status.cnt[4]*status.ms<=4*2000){
                       acosyaw+=acos.theta;
                   }
                   else if(status.cnt[4]*status.ms>4*2000){
                       status.cnt[5]++;
                       if(status.cnt[5]<=1){
                           acosyaw=acosyaw/4;
                           direction1=sbg.yaw+acosyaw;
                           if(direction1<-180){
                               direction1=360+direction1;
                           }
                           if(direction1>180){
                               direction1=-360+direction1;
                           }
                           task.direction=direction1;
                           qDebug()<<"acosyaw:"<<acosyaw;
                           qDebug()<<"task.direction:"<<task.direction;
                        }
                    }
               }
               else if(status.cnt[0]*status.ms>15*1000)
               {
                   setZero();
                   emit endTask();
                   return;
               }
         }
    }
    else if(abs(img.path_dx)<=75 && abs(img.path_dy)<=100 && abs(img.path_angle)<=8)
    {
        status.cnt[1]++;
        if(status.cnt[1]>=5)
        {
           status.cnt[2]=FORWARD_SLOW;
           sbg.goal=sbg.yaw;
           loadConfig(FORWARD_SLOW);
        }
    }

    else if(abs(img.path_dx)>75 || abs(img.path_dy)>100 || abs(img.path_angle)<=8)
    {
        status.cnt[1]=0;
        status.cnt[2]=LOCATE_BOTTOM;
        loadConfig(LOCATE_BOTTOM);
        //updateConfig();
        tempValue[MAIN_LEFT]=status.val[MAIN_LEFT].p*img_dyError/100.0
                +status.val[MAIN_LEFT].d*img_dyDiff/img_dyDiffT;
        tempValue[MAIN_RIGHT]=status.val[MAIN_RIGHT].p*img_dyError/100.0
                +status.val[MAIN_RIGHT].d*img_dyDiff/img_dyDiffT;

        tempValue[SIDE_UP]=status.val[SIDE_UP].p*img_dxError/100.0
                +status.val[SIDE_UP].d*img_dxDiff/img_dxDiffT;
        tempValue[SIDE_DOWN]=status.val[SIDE_DOWN].p*img_dxError/100.0
                +status.val[SIDE_DOWN].d*img_dxDiff/img_dxDiffT;

    }
    else if(abs(img.path_dx)<=75 && abs(img.path_dy)<=100)
    {
        status.cnt[1]=0;
        status.cnt[2]=ROTATING;
        loadConfig(ROTATING);
        //updateConfig();
        tempValue[MAIN_LEFT]=0;
        tempValue[MAIN_RIGHT]=0;
        tempValue[SIDE_UP]=status.val[SIDE_UP].p*img_angleError/100.0
                +status.val[SIDE_UP].i*img_angleErrorI/100.0
                +status.val[SIDE_UP].d*img_angleDiff/img_angleDiffT;
        tempValue[SIDE_DOWN]=status.val[SIDE_DOWN].p*img_angleError/100.0
                +status.val[SIDE_DOWN].i*img_angleErrorI/100.0
                +status.val[SIDE_DOWN].d*img_angleDiff/img_angleDiffT;

    }
    for(int i=0;i<4;i++){
        if(tempValue[hList[i]]>=45){
            tempValue[hList[i]]=45;
        }
        else if(tempValue[hList[i]]<=-45){
            tempValue[hList[i]]=-45;
        }
    }

    QList<pair<MOTORS,float>> tempList;
    for(int i=0;i<4;i++){
        tempList.push_back(make_pair<>(hList[i],tempValue[hList[i]]));
    }

    emit setHMotors(tempList);
}

void controller::endPathDir1()
{
    status.cnt.clear();
    setZero();
    setDeepCtr(false);
    emit missionFinished(PATH_DIR);
}
void controller::initGateAct()
{
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    loadConfig(FORWARD_SLOW);
}

void controller::ctrGateAct()
{
    float tempValue[NUMBER_OF_MOTORS];
    const static MOTORS hList[4]={MAIN_LEFT,MAIN_RIGHT,SIDE_DOWN,SIDE_UP};
    status.cnt[0]++;
    if(status.cnt[1]==0){
       sbg.goal=sbg.yaw+180;
       if(sbg.goal>180){
          sbg.goal=-360+sbg.goal;
       }
    }
    status.cnt[1]++;
    if(status.cnt[0]*status.ms<=30*1000){
          tempValue[MAIN_LEFT]=0;
          tempValue[MAIN_RIGHT]=0;
          tempValue[SIDE_UP]=-45;
          tempValue[SIDE_DOWN]=45;
    }

    else if(status.cnt[0]*status.ms>30*1000)
    {
          QList<TASKS_ID> idList1;
          idList1.push_back(RANDOM_PINGER);
          for(auto t:idList1){
              auto p=taskList.find(t);
              if(p!=taskList.end()){
                    currentTaskList.push(p->second);
              }
          }
          setZero();
          emit endTask();
          return;
    }
    for(int i=0;i<4;i++){
        if(tempValue[hList[i]]>=45){
            tempValue[hList[i]]=45;
        }
        else if(tempValue[hList[i]]<=-45){
            tempValue[hList[i]]=-45;
        }
    }

    QList<pair<MOTORS,float>> tempList;
    for(int i=0;i<4;i++){
        tempList.push_back(make_pair<>(hList[i],tempValue[hList[i]]));
    }

    emit setHMotors(tempList);

}

void controller::endGateAct()
{
    status.cnt.clear();
    //loadConfig(STOP);
    setZero();
}

void controller::initRandomPinger()
{
    emit setGoal(1.5);
    loadConfig(HANG);
    updateConfig();
    setDeepCtr(true);
    loadConfig(FORWARD_SLOW);
    acos.goal=0;
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    emit missionStarted(RANDOM_PINGER);
    qDebug()<<"initRandomPinger";
}

void controller::ctrRandomPinger()
{
   if(task.cnt==0){
      if(abs(task.direction)>=25){
           task.list=0;
      }
      else{
           task.list=1;
      }
      task.cnt=1;
   }
   if(task.list==0){
      status.cnt[1]++;
      if(status.cnt[1]*status.ms>=240*1000){
           QList<TASKS_ID> idList1;
           idList1.push_back(DROP_MARK_SIMP);
           for(auto t:idList1){
               auto p=taskList.find(t);
               if(p!=taskList.end()){
                    currentTaskList.push(p->second);
               }
           }
           setZero();
           emit endTask();
           return;
      }
   }
   if(acos.phi>=75 && acos.phiLast!=acos.phi){
      status.cnt[0]++;
      if(status.cnt[0]>=3*(2000/200)){
          QList<TASKS_ID> idList1;
          if(task.cnt==0){
             task.list=0;
             task.cnt=1;
          }
          if(task.list){
              idList1.push_back(GO_UP);
          }
          else{
              idList1.push_back(DROP_MARK_SIMP);
          }
          for(auto t:idList1){
              auto p=taskList.find(t);
              if(p!=taskList.end()){
                     currentTaskList.push(p->second);
              }
          }
          setZero();
          emit endTask();
          return;
      }
    }
    else{
       status.cnt[0]=0;
       float tempValue[NUMBER_OF_MOTORS];
       const static MOTORS hList[4]={MAIN_LEFT,MAIN_RIGHT,SIDE_DOWN,SIDE_UP};
       float acosError=-acos.goal+acos.theta;
       float acosDiffT=(acos.tNow-acos.tLast)/1000.0;
       float acosDiff=acosError-acos.thetaErrorLast;
       acos.thetaErrorLast=acosError;
       float DelTheta=acos.theta-acos.thetaLast;
       if(DelTheta>180){
           DelTheta=-360+DelTheta;
       }
       if(DelTheta<-180){
           DelTheta=360+DelTheta;
       }
       if(DelTheta>=30){
           acos.theta=acos.thetaLast+30;
           if(acos.theta>180){
               acos.theta=-360+acos.theta;
           }
       }
       if(DelTheta<=-30){
           acos.theta=acos.thetaLast-30;
           if(acos.theta<-180){
               acos.theta=360+acos.theta;
           }
       }
       if(acos.theta == acos.thetaLast && acos.phi == acos.phiLast){
           tempValue[MAIN_LEFT]=32;
           tempValue[MAIN_RIGHT]=32;
           tempValue[SIDE_UP]=0;
           tempValue[SIDE_DOWN]=0;
       }
       else{
           tempValue[MAIN_LEFT]=32+status.val[MAIN_LEFT].p*acosError/100.0
                            +status.val[MAIN_LEFT].d*acosDiff/acosDiffT;
           tempValue[MAIN_RIGHT]=32+status.val[MAIN_RIGHT].p*acosError/100.0
                            +status.val[MAIN_RIGHT].d*acosDiff/acosDiffT;

           if(abs((int)acosError)>=5){
                tempValue[SIDE_UP]=status.val[SIDE_UP].p*acosError/100.0
                               +status.val[SIDE_UP].d*acosDiff/acosDiffT;
                tempValue[SIDE_DOWN]=status.val[SIDE_DOWN].p*acosError/100.0
                                 +status.val[SIDE_DOWN].d*acosDiff/acosDiffT;
           }
       }

       for(int i=0;i<4;i++){
           if(tempValue[hList[i]]>=45){
               tempValue[hList[i]]=45;
           }
           else if(tempValue[hList[i]]<=-45){
               tempValue[hList[i]]=-45;
           }
       }

       QList<pair<MOTORS,float>> tempList;
       for(int i=0;i<4;i++){
           tempList.push_back(make_pair<>(hList[i],tempValue[hList[i]]));
       }

   //    emit setHMotors(tempList);
   }
}

void controller::endRandomPinger()
{
    status.cnt.clear();
   // loadConfig(STOP);
    setZero();
    setDeepCtr(false);
    emit missionFinished(RANDOM_PINGER);
    qDebug()<<"endRandomPinger";
}

void controller::initTurn()
{
    emit setGoal(1.5);
    loadConfig(HANG);
    updateConfig();
    setDeepCtr(true);
    status.cnt.push_back(0); //使用一个定时器
    qDebug() << "InitTurn!";
}

void controller::ctrTurn()
{
    status.cnt[0]++;
    float tempValue[NUMBER_OF_MOTORS];
    const static MOTORS hList[4]={MAIN_LEFT,MAIN_RIGHT,SIDE_UP,SIDE_DOWN};
    tempValue[MAIN_LEFT]=0;
    tempValue[MAIN_RIGHT]=0;
    tempValue[SIDE_UP]=-35;
    tempValue[SIDE_DOWN]=35;
    if(status.cnt[0]*status.ms>=3*1000){
        setZero();
        emit endTask();
        return;
    }

    QList<pair<MOTORS,float>> tempList;
    for(int i=0;i<4;i++){
        tempList.push_back(make_pair<>(hList[i],tempValue[hList[i]]));
    }

    emit setHMotors(tempList);

}

void controller::endTurn()
{
    status.cnt.clear();
    //loadConfig(STOP);
    setZero();
    setDeepCtr(false);
    qDebug() << "EndTurn!";
}

void controller::initHitBall()
{
    status.cnt.push_back(0);//time
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    emit missionStarted(HIT_BALL);
    qDebug()<<"initHitBall";
}

void controller::ctrHitBall()
{

    float tempValue[NUMBER_OF_MOTORS];
    const static MOTORS hList[8]={MAIN_LEFT,MAIN_RIGHT,SIDE_UP,SIDE_DOWN,LEFT_UP,LEFT_DOWN,RIGHT_UP,RIGHT_DOWN};
    QList<pair<MOTORS,float>> tempList;

    status.cnt[3]++;
    if(status.cnt[3]*status.ms>=120*1000)
    {
        setZero();
        emit endTask();
        return;
    }

    visionClass::visionData && tmp=vision->getData();
    if(tmp.t_now==img.t_now){
        return;
    }
    img.t_last=img.t_now;
    img.ball_dx=tmp.m2_red_dx;
    img.ball_dy=tmp.m2_red_dy;
    img.t_now=tmp.t_now;

    float img_dxError=img.ball_dx;
    float img_dxDiffT=(img.t_now-img.t_last)/1000.0;
    float img_dxDiff=img_dxError-img.ball_dx_last;
    img.ball_dx_last=img.ball_dx;

    float img_dyError=img.ball_dy;
    float img_dyDiffT=(img.t_now-img.t_last)/1000.0;
    float img_dyDiff=img_dyError-img.ball_dy_last;
    img.ball_dy_last=img.ball_dy;

    if(tmp.m2_red_dx==999 || tmp.m2_red_dy==999){
        emit setGoal(2.0);
        loadConfig(HANG);
        updateConfig();
        setDeepCtr(true);
        status.cnt[2]++;
        if(status.cnt[2]*status.ms>=10*1000){
            setZero();
            emit endTask();
            return;
        }

    }
    else if(tmp.m2_red_dx==-999 || tmp.m2_red_dy==-999){
        status.cnt[0]++;
        emit setGoal(2.0);
        loadConfig(HANG);
        updateConfig();
        setDeepCtr(true);
        if(status.cnt[0]*status.ms<=5*1000){
            tempValue[MAIN_LEFT]=35;
            tempValue[MAIN_RIGHT]=35;
            tempValue[SIDE_UP]=0;
            tempValue[SIDE_DOWN]=0;
            for(int i=0;i<4;i++){
                 tempList.push_back(make_pair<>(hList[i],tempValue[hList[i]]));
            }
            emit setHMotors(tempList);
        }
        else if (status.cnt[0]*status.ms>5*1000){
            setZero();
            emit endTask();
            return;
        }
    }
    else if(tmp.m2_red_dx!=-999 && tmp.m2_red_dy!=-999 && tmp.m2_red_dx!=999 && tmp.m2_red_dy!=999){
        loadConfig(FORWARD_UPDOWN);

        tempValue[LEFT_UP]=27+status.val[LEFT_UP].p*img_dyError/100.0
                   +status.val[LEFT_UP].d*img_dyDiff/img_dyDiffT;
        tempValue[LEFT_DOWN]=27+status.val[LEFT_DOWN].p*img_dyError/100.0
                   +status.val[LEFT_DOWN].d*img_dyDiff/img_dyDiffT;
        tempValue[RIGHT_UP]=27+status.val[RIGHT_UP].p*img_dyError/100.0
                   +status.val[RIGHT_UP].d*img_dyDiff/img_dyDiffT;
        tempValue[RIGHT_DOWN]=27+status.val[RIGHT_DOWN].p*img_dyError/100.0
                   +status.val[RIGHT_DOWN].d*img_dyDiff/img_dyDiffT;

    for(int i=4;i<8;i++){
        if(tempValue[hList[i]]>=40){
           tempValue[hList[i]]=40;
        }
        else if(tempValue[hList[i]]<=-40){
           tempValue[hList[i]]=-40;
        }
    }
    for(int i=4;i<8;i++){
        tempList.push_back(make_pair<>(hList[i],tempValue[hList[i]]));
    }
    emit setVMotors(tempList);

    loadConfig(FORWARD_FORSEE);

    tempValue[MAIN_LEFT]=32+status.val[MAIN_LEFT].p*img_dxError/100.0
                 +status.val[MAIN_LEFT].d*img_dxDiff/img_dxDiffT;
    tempValue[MAIN_RIGHT]=32+status.val[MAIN_RIGHT].p*img_dxError/100.0
                 +status.val[MAIN_RIGHT].d*img_dxDiff/img_dxDiffT;

    tempValue[SIDE_UP]=0;
    tempValue[SIDE_DOWN]=0;
    for(int i=0;i<4;i++){
          if(tempValue[hList[i]]>=45){
            tempValue[hList[i]]=45;
          }
          else if(tempValue[hList[i]]<=-40){
            tempValue[hList[i]]=-40;
          }
      }
    for(int i=0;i<4;i++){
         tempList.push_back(make_pair<>(hList[i],tempValue[hList[i]]));
     }
    emit setHMotors(tempList);
    }
}

void controller::endHitBall()
{
    status.cnt.clear();
    setZero();
    setDeepCtr(false);
    emit missionFinished(HIT_BALL);
    qDebug()<<"endHitBall";
}

void controller::initPassGate()
{
    emit setGoal(1.5);
    loadConfig(HANG);
    updateConfig();
    setDeepCtr(true);
    status.cnt.push_back(0);//time
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    emit missionStarted(PASS_GATE);
    qDebug()<<"initPassGate";
}

void controller::ctrPassGate()
{
    float tempValue[NUMBER_OF_MOTORS];
    const static MOTORS hList[4]={MAIN_LEFT,MAIN_RIGHT,SIDE_UP,SIDE_DOWN};
    QList<pair<MOTORS,float>> tempList;

    status.cnt[3]++;
    if(status.cnt[3]*status.ms>=180*1000)
    {
        QList<TASKS_ID> idList1;
        idList1.push_back(RANDOM_PINGER);
        for(auto t:idList1){
            auto p=taskList.find(t);
            if(p!=taskList.end()){
                  currentTaskList.push(p->second);
            }
         }
        setZero();
        emit endTask();
        return;
    }
    visionClass::visionData && tmp=vision->getData();
    if(tmp.t_now==img.t_now){
        return;
    }
    img.t_last=img.t_now;
    img.gate_dx=tmp.m3_ugate_dx;
    img.t_now=tmp.t_now;

    float img_dxError=img.gate_dx;
    float img_dxDiffT=(img.t_now-img.t_last)/1000.0;
    float img_dxDiff=img_dxError-img.gate_dx_last;
    img.gate_dx_last=img.gate_dx;
    if(abs(tmp.m3_ugate_dx)>30 && status.cnt[1]==0){
        loadConfig(FORWARD_FORSEE);
        tempValue[MAIN_LEFT]=32+status.val[MAIN_LEFT].p*img_dxError/100.0
                 +status.val[MAIN_LEFT].d*img_dxDiff/img_dxDiffT;
        tempValue[MAIN_RIGHT]=32+status.val[MAIN_RIGHT].p*img_dxError/100.0
                 +status.val[MAIN_RIGHT].d*img_dxDiff/img_dxDiffT;
        tempValue[SIDE_UP]=0;
        tempValue[SIDE_DOWN]=0;
        status.cnt[4]=0;
    }
    if(abs(tmp.m3_ugate_dx)<=30){
        status.cnt[4]++;
    }
    if(status.cnt[4]>=3){
        tempValue[MAIN_LEFT]=32;
        tempValue[MAIN_RIGHT]=32;
        tempValue[SIDE_UP]=0;
        tempValue[SIDE_DOWN]=0;
        status.cnt[1]++;
        if(status.cnt[1]*status.ms>=50*1000){
            QList<TASKS_ID> idList1;
            idList1.push_back(GATE_ACT);
            for(auto t:idList1){
                auto p=taskList.find(t);
                if(p!=taskList.end()){
                      currentTaskList.push(p->second);
                }
            }
            setZero();
            emit endTask();
            return;
        }

    }

    for(int i=0;i<4;i++){
          if(tempValue[hList[i]]>=45){
            tempValue[hList[i]]=45;
          }
          else if(tempValue[hList[i]]<=-45){
            tempValue[hList[i]]=-45;
          }
      }
    for(int i=0;i<4;i++){
         tempList.push_back(make_pair<>(hList[i],tempValue[hList[i]]));
     }
   // emit setHMotors(tempList);
}

void controller::endPassGate()
{
    status.cnt.clear();
   // loadConfig(STOP);
    setZero();
    setDeepCtr(false);
    emit missionFinished(PASS_GATE);
    qDebug()<<"endPassGate";
}

void controller::initDropMark()
{
    emit setGoal(4.0);
    loadConfig(HANG);
    updateConfig();
    setDeepCtr(true);
    img.bin_angle_goal=89.9;
    status.cnt.push_back(0);//time
    status.cnt.push_back(0);//time
    status.cnt.push_back(0);
    emit missionStarted(DROP_MARK);
    qDebug()<<"initDropMark";
}

void controller::ctrDropMark()
{
    visionClass::visionData && tmp=vision->getData();
    if(tmp.t_now==img.t_now){
        return;
    }
    status.cnt[2]++;
    if(status.cnt[2]*status.ms>=120*1000){
        QList<TASKS_ID> idList1;
        idList1.push_back(DROP_MARK1);
        for(auto t:idList1){
            auto p=taskList.find(t);
            if(p!=taskList.end()){
                 currentTaskList.push(p->second);
            }
        }
        setZero();
        emit endTask();
        return;
    }
    if(tmp.m5_lid_angle==999 || tmp.m5_lid_dx==999 || tmp.m5_lid_dy==999){
        //处理没有找到的情况。现在未完工
        return;
    }
    if(tmp.m5_lid_angle==-999 || tmp.m5_lid_dx==-999 || tmp.m5_lid_dy==-999){
        QList<TASKS_ID> idList1;
        idList1.push_back(OPEN_BIN);
        for(auto t:idList1){
            auto p=taskList.find(t);
            if(p!=taskList.end()){
                 currentTaskList.push(p->second);
            }
        }
        setZero();
        emit endTask();
        return;
    }
    float tempValue[NUMBER_OF_MOTORS];
    const static MOTORS hList[4]={MAIN_LEFT,MAIN_RIGHT,SIDE_UP,SIDE_DOWN};
    QList<pair<MOTORS,float>> tempList;
    img.bin_angle_last=img.bin_angle;
    img.bin_dx_last=img.bin_dx;
    img.bin_dy_last=img.bin_dy;
    img.t_last=img.t_now;
    img.bin_angle=tmp.m5_lid_angle;
    img.bin_dx=tmp.m5_lid_dx;
    img.bin_dy=tmp.m5_lid_dy;
    img.t_now=tmp.t_now;

    //根据angle修正,取与yaw相同pid值
    float img_angleError;
    img_angleError=img.bin_angle_goal-img.bin_angle;
    if(img_angleError>90){
       img_angleError=-180+img_angleError;
    }
    static float img_angleErrorI;
    img_angleErrorI += img_angleError;
    float img_angleDiffT=(img.t_now-img.t_last)/1000.0;
    float img_angleDiff=img_angleError-img.bin_angle_Errorlast;
    img.bin_angle_Errorlast=img_angleError;

    float img_dxError=img.bin_dx;
    static float img_dxErrorI;
    img_dxErrorI += img_dxError;
    float img_dxDiffT=(img.t_now-img.t_last)/1000.0;
    float img_dxDiff=img_dxError-img.bin_dx_last;
    img.bin_dx_last=img.bin_dx;

    float img_dyError=img.bin_dy;
    static float img_dyErrorI;
    img_dyErrorI += img_dyError;
    float img_dyDiffT=(img.t_now-img.t_last)/1000.0;
    float img_dyDiff=img_dyError-img.bin_dy_last;
    img.bin_dy_last=img.bin_dy;
    if(acos.phi<=70){
        status.cnt[1]=0;
        float acosError=-acos.goal+acos.theta;
        float acosDiffT=(acos.tNow-acos.tLast)/1000.0;
        float acosDiff=acosError-acos.thetaErrorLast;
        acos.thetaErrorLast=acosError;
        float DelTheta=acos.theta-acos.thetaLast;
        if(DelTheta>180){
            DelTheta=-360+DelTheta;
        }
        if(DelTheta<-180){
            DelTheta=360+DelTheta;
        }
        if(DelTheta>=30){
            acos.theta=acos.thetaLast+30;
            if(acos.theta>180){
                acos.theta=-360+acos.theta;
            }
        }
        if(DelTheta<=-30){
            acos.theta=acos.thetaLast-30;
            if(acos.theta<-180){
                acos.theta=360+acos.theta;
            }
        }
        if(acos.theta == acos.thetaLast && acos.phi == acos.phiLast){
            tempValue[MAIN_LEFT]=32;
            tempValue[MAIN_RIGHT]=32;
            tempValue[SIDE_UP]=0;
            tempValue[SIDE_DOWN]=0;
        }
        else{
            tempValue[MAIN_LEFT]=32+status.val[MAIN_LEFT].p*acosError/100.0
                             +status.val[MAIN_LEFT].d*acosDiff/acosDiffT;
            tempValue[MAIN_RIGHT]=32+status.val[MAIN_RIGHT].p*acosError/100.0
                             +status.val[MAIN_RIGHT].d*acosDiff/acosDiffT;

            if(abs((int)acosError)>=5){
                 tempValue[SIDE_UP]=status.val[SIDE_UP].p*acosError/100.0
                                +status.val[SIDE_UP].d*acosDiff/acosDiffT;
                 tempValue[SIDE_DOWN]=status.val[SIDE_DOWN].p*acosError/100.0
                                  +status.val[SIDE_DOWN].d*acosDiff/acosDiffT;
            }
        }
    }
  //  else if(acos.phi>70){
        status.cnt[1]++;
        if(status.cnt[1]*status.ms>=3*2000){
            if(abs(img.bin_dx)>30 || abs(img.bin_dy)>20 || abs(img_angleError)<=5)
            {
                status.cnt[0]=0;
                loadConfig(LOCATE_BOTTOM_BIN);
                //updateConfig();
                tempValue[MAIN_LEFT]=status.val[MAIN_LEFT].p*img_dyError/100.0
                           +status.val[MAIN_LEFT].i*img_dyErrorI/100.0
                           +status.val[MAIN_LEFT].d*img_dyDiff/img_dyDiffT;
                tempValue[MAIN_RIGHT]=status.val[MAIN_RIGHT].p*img_dyError/100.0
                           +status.val[MAIN_RIGHT].i*img_dyErrorI/100.0
                           +status.val[MAIN_RIGHT].d*img_dyDiff/img_dyDiffT;

                tempValue[SIDE_UP]=status.val[SIDE_UP].p*img_dxError/100.0
                          +status.val[SIDE_UP].i*img_dxErrorI/100.0
                          +status.val[SIDE_UP].d*img_dxDiff/img_dxDiffT;
                tempValue[SIDE_DOWN]=status.val[SIDE_DOWN].p*img_dxError/100.0
                          +status.val[SIDE_DOWN].i*img_dxErrorI/100.0
                          +status.val[SIDE_DOWN].d*img_dxDiff/img_dxDiffT;

            }
            else if(abs(img.bin_dx)<=30 && abs(img.bin_dy)<=20)
            {
                status.cnt[0]=0;
                loadConfig(ROTATING);
                //updateConfig();
                tempValue[MAIN_LEFT]=0;
                tempValue[MAIN_RIGHT]=0;
                tempValue[SIDE_UP]=status.val[SIDE_UP].p*img_angleError/100.0
                          +status.val[SIDE_UP].i*img_angleErrorI/100.0
                          +status.val[SIDE_UP].d*img_angleDiff/img_angleDiffT;
                tempValue[SIDE_DOWN]=status.val[SIDE_DOWN].p*img_angleError/100.0
                          +status.val[SIDE_DOWN].i*img_angleErrorI/100.0
                          +status.val[SIDE_DOWN].d*img_angleDiff/img_angleDiffT;

            }
        }
   // }
    for(int i=0;i<4;i++){
        if(tempValue[hList[i]]>=45){
            tempValue[hList[i]]=45;
        }
        else if(tempValue[hList[i]]<=-45){
            tempValue[hList[i]]=-45;
        }
    }
    for(int i=0;i<4;i++){
        tempList.push_back(make_pair<>(hList[i],tempValue[hList[i]]));
    }

    emit setHMotors(tempList);

}

void controller::endDropMark()
{
    status.cnt.clear();
    //loadConfig(STOP);
    setZero();
    setDeepCtr(false);
    //emit missionFinished(DROP_MARK);
    qDebug()<<"endDropMark";
}

void controller::initOpenBin()
{
    status.cnt.push_back(0);//time
    status.cnt.push_back(0);//
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
   // emit missionStarted(DROP_MARK);
    qDebug()<<"initOpenBin";
}

void controller::ctrOpenBin()
{
    visionClass::visionData && tmp=vision->getData();
    if(tmp.t_now==img.t_now){
        return;
    }
    static bool down_or_for;
    float deep1=deep.value;
    down_or_for=tmp.m5_goback;
    status.cnt[4]++;
    if(status.cnt[4]<=1){
       sbg.goal = sbg.yaw;
    }
    float tempValue[NUMBER_OF_MOTORS];
    const static MOTORS hList[8]={MAIN_LEFT,MAIN_RIGHT,SIDE_UP,SIDE_DOWN,LEFT_UP,LEFT_DOWN,RIGHT_UP,RIGHT_DOWN};
    QList<pair<MOTORS,float>> tempList;
    if(status.cnt[2]<=1){
        if(deep1<5.5){
            tempValue[LEFT_UP]=43;
            tempValue[LEFT_DOWN]=43;
            tempValue[RIGHT_UP]=43;
            tempValue[RIGHT_DOWN]=43;
            for(int i=4;i<8;i++){
                   tempList.push_back(make_pair<>(hList[i],tempValue[hList[i]]));
            }
            emit setVMotors(tempList);
            status.cnt[2]=1;
        }
        else{
            qDebug() << "Down The Hand.";
            emit grabIt(true);
            tempValue[LEFT_UP]=-60;
            tempValue[LEFT_DOWN]=-60;
            tempValue[RIGHT_UP]=-60;
            tempValue[RIGHT_DOWN]=-60;
            for(int i=4;i<8;i++){
                   tempList.push_back(make_pair<>(hList[i],tempValue[hList[i]]));
            }
            emit setVMotors(tempList);
            status.cnt[2]=2;
        }
    }
    status.cnt[0]++;
    if(status.cnt[0]*status.ms>=25*1000){
        if(status.cnt[0]*status.ms>=30*1000){
           loadConfig(FORWARD_SLOW);
           float sbgError=sbg.goal-sbg.yaw;
           if(sbgError>190){
              sbgError=-360+sbgError;
           }
           if(sbgError<-190){
              sbgError=360+sbgError;
           }
           float sbgDiffT=(sbg.tNow-sbg.tLast)/1000.0;
           float sbgDiff=sbgError-sbg.yawErrorLast;
           sbg.yawErrorLast=sbgError;

           status.cnt[1]++;
           if(status.cnt[1]*status.ms<=6*1000){
               if(down_or_for){
                   tempValue[MAIN_LEFT]=-35+status.val[MAIN_LEFT].p*sbgError/100.0
                             +status.val[MAIN_LEFT].d*sbgDiff/sbgDiffT;
                   tempValue[MAIN_RIGHT]=-35+status.val[MAIN_RIGHT].p*sbgError/100.0
                             +status.val[MAIN_RIGHT].d*sbgDiff/sbgDiffT;
                   tempValue[SIDE_UP]=0;
                   tempValue[SIDE_DOWN]=0;
               }
               if(!down_or_for){
                   tempValue[MAIN_LEFT]=35+status.val[MAIN_LEFT].p*sbgError/100.0
                             +status.val[MAIN_LEFT].d*sbgDiff/sbgDiffT;
                   tempValue[MAIN_RIGHT]=35+status.val[MAIN_RIGHT].p*sbgError/100.0
                             +status.val[MAIN_RIGHT].d*sbgDiff/sbgDiffT;
                   tempValue[SIDE_UP]=0;
                   tempValue[SIDE_DOWN]=0;
               }
               for(int i=0;i<4;i++){
                   if(tempValue[hList[i]]>=45){
                       tempValue[hList[i]]=45;
                   }
                   else if(tempValue[hList[i]]<=-45){
                       tempValue[hList[i]]=-45;
                   }
               }
               for(int i=0;i<4;i++){
                   tempList.push_back(make_pair<>(hList[i],tempValue[hList[i]]));
               }
               emit setHMotors(tempList);
        }
        else if(status.cnt[1]*status.ms<=12*1000)
        {
               status.cnt[3]++;
               if(status.cnt[3]<=1){
                   qDebug() << "Up The Hand.";
                   emit grabIt(false);
                   emit setGoal(2.0);
                   loadConfig(HANG);
                   updateConfig();
               }
               if(down_or_for){
                   tempValue[MAIN_LEFT]=35+status.val[MAIN_LEFT].p*sbgError/100.0
                             +status.val[MAIN_LEFT].d*sbgDiff/sbgDiffT;
                   tempValue[MAIN_RIGHT]=35+status.val[MAIN_RIGHT].p*sbgError/100.0
                             +status.val[MAIN_RIGHT].d*sbgDiff/sbgDiffT;
                   tempValue[SIDE_UP]=0;
                   tempValue[SIDE_DOWN]=0;
               }
               if(!down_or_for){
                   tempValue[MAIN_LEFT]=-35+status.val[MAIN_LEFT].p*sbgError/100.0
                             +status.val[MAIN_LEFT].d*sbgDiff/sbgDiffT;
                   tempValue[MAIN_RIGHT]=-35+status.val[MAIN_RIGHT].p*sbgError/100.0
                             +status.val[MAIN_RIGHT].d*sbgDiff/sbgDiffT;
                   tempValue[SIDE_UP]=0;
                   tempValue[SIDE_DOWN]=0;
               }
               for(int i=0;i<4;i++){
                   if(tempValue[hList[i]]>=45){
                       tempValue[hList[i]]=45;
                   }
                   else if(tempValue[hList[i]]<=-45){
                       tempValue[hList[i]]=-45;
                   }
               }
               for(int i=0;i<4;i++){
                   tempList.push_back(make_pair<>(hList[i],tempValue[hList[i]]));
               }
               emit setHMotors(tempList);
        }
        else if(status.cnt[1]*status.ms>12*1000){
            QList<TASKS_ID> idList1;
           // if(lid_status=1){
                idList1.push_back(DROP_MARK1);
            //}
            //else{
               // idList1.push_back(DROP_MARK);
            //}
            for(auto t:idList1){
                auto p=taskList.find(t);
                if(p!=taskList.end()){
                     currentTaskList.push(p->second);
                }
            }
            setZero();
            emit endTask();
            return;
        }
      }
      float DeepError=4.5-deep.value;
      static float DeepErrorI;
      DeepErrorI += DeepError;
      float DeepDiffT=(deep.tNow-deep.tLast)/1000.0;
      float DeepDiff=DeepError-deep.deepErrorLast;
      deep.deepErrorLast=DeepError;

      loadConfig(LID_UPDOWN);
      qDebug()<<"LID_UPDOWN";
      tempValue[LEFT_UP]=-40+status.val[LEFT_UP].p*DeepError
               +status.val[LEFT_UP].i* DeepErrorI/100.0
               +status.val[LEFT_UP].d*DeepDiff/DeepDiffT;
      tempValue[LEFT_DOWN]=-40+status.val[LEFT_DOWN].p*DeepError
               +status.val[LEFT_DOWN].i* DeepErrorI/100.0
               +status.val[LEFT_DOWN].d*DeepDiff/DeepDiffT;
      tempValue[RIGHT_UP]=-40+status.val[RIGHT_UP].p*DeepError
               +status.val[RIGHT_UP].i* DeepErrorI/100.0
               +status.val[RIGHT_UP].d*DeepDiff/DeepDiffT;
      tempValue[RIGHT_DOWN]=-40+status.val[RIGHT_DOWN].p*DeepError
               +status.val[RIGHT_DOWN].i* DeepErrorI/100.0
               +status.val[RIGHT_DOWN].d*DeepDiff/DeepDiffT;

      for(int i=4;i<8;i++){
          if(tempValue[hList[i]]>=10){
               tempValue[hList[i]]=10;
          }
          else if(tempValue[hList[i]]<=-60){
               tempValue[hList[i]]=-60;
          }
       }
       for(int i=4;i<8;i++){
               tempList.push_back(make_pair<>(hList[i],tempValue[hList[i]]));
       }
       emit setVMotors(tempList);
    }
}

void controller::endOpenBin()
{
    status.cnt.clear();
    //loadConfig(STOP);
    setZero();
    //emit missionFinished(DROP_MARK);
    qDebug()<<"endOpeBin";
}

void controller::initDropMark1()
{
    emit setGoal(4.0);
    loadConfig(HANG);
    updateConfig();
    setDeepCtr(true);
    img.bin_angle_goal=89.9;
    acos.goal=0;
    status.cnt.push_back(0);//time
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
   // emit missionStarted(DROP_MARK);
    qDebug()<<"initDropMark1";
}

void controller::ctrDropMark1()
{
    status.cnt[3]++;
    if(status.cnt[3]*status.ms>=120*1000){
        QList<TASKS_ID> idList1;
        idList1.push_back(DROP_MARK_SIMP);
        for(auto t:idList1){
            auto p=taskList.find(t);
            if(p!=taskList.end()){
                 currentTaskList.push(p->second);
            }
        }
        setZero();
        emit endTask();
        return;
    }
    qDebug()<<"DropMark1";
    visionClass::visionData && tmp=vision->getData();
    if(tmp.t_now==img.t_now){
        return;
    }

    if(tmp.m5_bin_angle==999 || tmp.m5_bin_dx==999 || tmp.m5_bin_dy==999){
        //处理没有找到的情况。现在未完工
        return;
    }

    float tempValue[NUMBER_OF_MOTORS];
    const static MOTORS hList[8]={MAIN_LEFT,MAIN_RIGHT,SIDE_UP,SIDE_DOWN,LEFT_UP,LEFT_DOWN,RIGHT_UP,RIGHT_DOWN};
    QList<pair<MOTORS,float>> tempList;
    img.bin_angle_last=img.bin_angle;
    img.bin_dx_last=img.bin_dx;
    img.bin_dy_last=img.bin_dy;
    img.t_last=img.t_now;
    img.bin_angle=tmp.m5_bin_angle;
    img.bin_dx=tmp.m5_bin_dx;
    img.bin_dy=tmp.m5_bin_dy;
    img.t_now=tmp.t_now;
    //根据angle修正,取与yaw相同pid值
    float img_angleError;
    img_angleError=img.bin_angle_goal-img.bin_angle;
    if(img_angleError>90){
       img_angleError=-180+img_angleError;
    }
    float deep1=deep.value;
    static float img_angleErrorI;
    img_angleErrorI += img_angleError;
    float img_angleDiffT=(img.t_now-img.t_last)/1000.0;
    float img_angleDiff=img_angleError-img.bin_angle_Errorlast;
    img.bin_angle_Errorlast=img_angleError;

    float img_dxError=img.bin_dx;
    static float img_dxErrorI;
    img_dxErrorI += img_dxError;
    float img_dxDiffT=(img.t_now-img.t_last)/1000.0;
    float img_dxDiff=img_dxError-img.bin_dx_last;
    img.bin_dx_last=img.bin_dx;

    float img_dyError=img.bin_dy;
    static float img_dyErrorI;
    img_dyErrorI += img_dyError;
    float img_dyDiffT=(img.t_now-img.t_last)/1000.0;
    float img_dyDiff=img_dyError-img.bin_dy_last;
    img.bin_dy_last=img.bin_dy;
    if(acos.phi<=70){
        status.cnt[4]=0;
        float acosError=-acos.goal+acos.theta;
        float acosDiffT=(acos.tNow-acos.tLast)/1000.0;
        float acosDiff=acosError-acos.thetaErrorLast;
        acos.thetaErrorLast=acosError;
        float DelTheta=acos.theta-acos.thetaLast;
        if(DelTheta>180){
            DelTheta=-360+DelTheta;
        }
        if(DelTheta<-180){
            DelTheta=360+DelTheta;
        }
        if(DelTheta>=30){
            acos.theta=acos.thetaLast+30;
            if(acos.theta>180){
                acos.theta=-360+acos.theta;
            }
        }
        if(DelTheta<=-30){
            acos.theta=acos.thetaLast-30;
            if(acos.theta<-180){
                acos.theta=360+acos.theta;
            }
        }
        if(acos.theta == acos.thetaLast && acos.phi == acos.phiLast){
            tempValue[MAIN_LEFT]=32;
            tempValue[MAIN_RIGHT]=32;
            tempValue[SIDE_UP]=0;
            tempValue[SIDE_DOWN]=0;
        }
        else{
            tempValue[MAIN_LEFT]=32+status.val[MAIN_LEFT].p*acosError/100.0
                             +status.val[MAIN_LEFT].d*acosDiff/acosDiffT;
            tempValue[MAIN_RIGHT]=32+status.val[MAIN_RIGHT].p*acosError/100.0
                             +status.val[MAIN_RIGHT].d*acosDiff/acosDiffT;

            if(abs((int)acosError)>=5){
                 tempValue[SIDE_UP]=status.val[SIDE_UP].p*acosError/100.0
                                +status.val[SIDE_UP].d*acosDiff/acosDiffT;
                 tempValue[SIDE_DOWN]=status.val[SIDE_DOWN].p*acosError/100.0
                                  +status.val[SIDE_DOWN].d*acosDiff/acosDiffT;
            }
        }
    }
  //  else if(acos.phi>70){
        status.cnt[4]++;
        if(status.cnt[4]*status.ms>=3*2000){
            if(abs(img.bin_dx)<=50 && abs(img.bin_dy)<=40 && abs(img_angleError)<=15)
            {
               status.cnt[0]++;
               if(status.cnt[0]>=8){
                   QList<TASKS_ID> idList1;
                   idList1.push_back(DROP_IT);
                   for(auto t:idList1){
                       auto p=taskList.find(t);
                       if(p!=taskList.end()){
                           currentTaskList.push(p->second);
                       }
                   }
                   setZero();
                   emit endTask();
                   return;
               }
           }
           else if(abs(img.bin_dx)>50 || abs(img.bin_dy)>40 || abs(img_angleError)<=15)
           {
               status.cnt[0]=0;
               loadConfig(LOCATE_BOTTOM_BIN);
               //updateConfig();
               tempValue[MAIN_LEFT]=status.val[MAIN_LEFT].p*img_dyError/100.0
                       +status.val[MAIN_LEFT].i*img_dyErrorI/100.0
                       +status.val[MAIN_LEFT].d*img_dyDiff/img_dyDiffT;
               tempValue[MAIN_RIGHT]=status.val[MAIN_RIGHT].p*img_dyError/100.0
                       +status.val[MAIN_RIGHT].i*img_dyErrorI/100.0
                       +status.val[MAIN_RIGHT].d*img_dyDiff/img_dyDiffT;

               tempValue[SIDE_UP]=status.val[SIDE_UP].p*img_dxError/100.0
                       +status.val[SIDE_UP].i*img_dxErrorI/100.0
                       +status.val[SIDE_UP].d*img_dxDiff/img_dxDiffT;
               tempValue[SIDE_DOWN]=status.val[SIDE_DOWN].p*img_dxError/100.0
                       +status.val[SIDE_DOWN].i*img_dxErrorI/100.0
                       +status.val[SIDE_DOWN].d*img_dxDiff/img_dxDiffT;

           }
           else if(abs(img.bin_dx)<=50 && abs(img.bin_dy)<=40)
           {
               status.cnt[0]=0;
               loadConfig(ROTATING);
               //updateConfig();
               tempValue[MAIN_LEFT]=0;
               tempValue[MAIN_RIGHT]=0;
               tempValue[SIDE_UP]=status.val[SIDE_UP].p*img_angleError/100.0
                       +status.val[SIDE_UP].i*img_angleErrorI/100.0
                       +status.val[SIDE_UP].d*img_angleDiff/img_angleDiffT;
               tempValue[SIDE_DOWN]=status.val[SIDE_DOWN].p*img_angleError/100.0
                       +status.val[SIDE_DOWN].i*img_angleErrorI/100.0
                       +status.val[SIDE_DOWN].d*img_angleDiff/img_angleDiffT;


           }
        }
  //  }
    for(int i=0;i<4;i++){
        if(tempValue[hList[i]]>=45){
            tempValue[hList[i]]=45;
        }
        else if(tempValue[hList[i]]<=-45){
            tempValue[hList[i]]=-45;
        }
    }
    for(int i=0;i<4;i++){
        tempList.push_back(make_pair<>(hList[i],tempValue[hList[i]]));
    }

    emit setHMotors(tempList);

}

void controller::endDropMark1()
{
    status.cnt.clear();
    //loadConfig(STOP);
    setZero();
    setDeepCtr(false);
    emit missionFinished(DROP_MARK);
    qDebug()<<"endDropMark1";
}

void controller::initDropMarksimp()
{
    emit setGoal(4.0);
    loadConfig(HANG);
    updateConfig();
    setDeepCtr(true);
    img.bin_angle_goal=89.9;
    status.cnt.push_back(0);//time
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    emit missionStarted(DROP_MARK_SIMP);
    qDebug()<<"initDropMarksimp";
}

void controller::ctrDropMarksimp()
{
    status.cnt[3]++;
    if(status.cnt[3]*status.ms>=120*1000){
        QList<TASKS_ID> idList1;
        idList1.push_back(DROP_IT);
        for(auto t:idList1){
            auto p=taskList.find(t);
            if(p!=taskList.end()){
                 currentTaskList.push(p->second);
            }
        }
        setZero();
        emit endTask();
        return;
    }
    qDebug()<<"DropMarksimp";
    visionClass::visionData && tmp=vision->getData();
    if(tmp.t_now==img.t_now){
        return;
    }

    if(tmp.m6_binsim_angle==999 || tmp.m6_binsim_dx==999 || tmp.m6_binsim_dy==999){
        //处理没有找到的情况。现在未完工
        return;
    }

    float tempValue[NUMBER_OF_MOTORS];
    const static MOTORS hList[4]={MAIN_LEFT,MAIN_RIGHT,SIDE_UP,SIDE_DOWN};
    QList<pair<MOTORS,float>> tempList;
    img.bin_angle_last=img.bin_angle;
    img.bin_dx_last=img.bin_dx;
    img.bin_dy_last=img.bin_dy;
    img.t_last=img.t_now;
    img.bin_angle=tmp.m6_binsim_angle;
    img.bin_dx=tmp.m6_binsim_dx;
    img.bin_dy=tmp.m6_binsim_dy;
    img.t_now=tmp.t_now;

    float img_dxError=img.bin_dx;
    static float img_dxErrorI;
    img_dxErrorI += img_dxError;
    float img_dxDiffT=(img.t_now-img.t_last)/1000.0;
    float img_dxDiff=img_dxError-img.bin_dx_last;
    img.bin_dx_last=img.bin_dx;

    float img_dyError=img.bin_dy;
    static float img_dyErrorI;
    img_dyErrorI += img_dyError;
    float img_dyDiffT=(img.t_now-img.t_last)/1000.0;
    float img_dyDiff=img_dyError-img.bin_dy_last;
    img.bin_dy_last=img.bin_dy;
    if(acos.phi<=50){
        status.cnt[4]=0;
        float acosError=-acos.goal+acos.theta;
        float acosDiffT=(acos.tNow-acos.tLast)/1000.0;
        float acosDiff=acosError-acos.thetaErrorLast;
        acos.thetaErrorLast=acosError;
        float DelTheta=acos.theta-acos.thetaLast;
        if(DelTheta>180){
            DelTheta=-360+DelTheta;
        }
        if(DelTheta<-180){
            DelTheta=360+DelTheta;
        }
        if(DelTheta>=30){
            acos.theta=acos.thetaLast+30;
            if(acos.theta>180){
                acos.theta=-360+acos.theta;
            }
        }
        if(DelTheta<=-30){
            acos.theta=acos.thetaLast-30;
            if(acos.theta<-180){
                acos.theta=360+acos.theta;
            }
        }
        if(acos.theta == acos.thetaLast && acos.phi == acos.phiLast){
            tempValue[MAIN_LEFT]=32;
            tempValue[MAIN_RIGHT]=32;
            tempValue[SIDE_UP]=0;
            tempValue[SIDE_DOWN]=0;
        }
        else{
            tempValue[MAIN_LEFT]=32+status.val[MAIN_LEFT].p*acosError/100.0
                             +status.val[MAIN_LEFT].d*acosDiff/acosDiffT;
            tempValue[MAIN_RIGHT]=32+status.val[MAIN_RIGHT].p*acosError/100.0
                             +status.val[MAIN_RIGHT].d*acosDiff/acosDiffT;

            if(abs((int)acosError)>=5){
                 tempValue[SIDE_UP]=status.val[SIDE_UP].p*acosError/100.0
                                +status.val[SIDE_UP].d*acosDiff/acosDiffT;
                 tempValue[SIDE_DOWN]=status.val[SIDE_DOWN].p*acosError/100.0
                                  +status.val[SIDE_DOWN].d*acosDiff/acosDiffT;
            }
        }
    }
   // else if(acos.phi>70){
        if(abs(img.bin_dx)<=50 && abs(img.bin_dy)<=40)
        {
            status.cnt[0]++;
            if(status.cnt[0]>=5){
                QList<TASKS_ID> idList1;
                idList1.push_back(DROP_IT);
                for(auto t:idList1){
                    auto p=taskList.find(t);
                    if(p!=taskList.end()){
                         currentTaskList.push(p->second);
                    }
                }
                setZero();
                emit endTask();
                return;
            }
        }
        else if(abs(img.bin_dx)>50 || abs(img.bin_dy)>40)
        {
            status.cnt[0]=0;
            loadConfig(LOCATE_BOTTOM_BIN);
            //updateConfig();
            tempValue[MAIN_LEFT]=status.val[MAIN_LEFT].p*img_dyError/100.0
                    +status.val[MAIN_LEFT].i*img_dyErrorI/100.0
                    +status.val[MAIN_LEFT].d*img_dyDiff/img_dyDiffT;
            tempValue[MAIN_RIGHT]=status.val[MAIN_RIGHT].p*img_dyError/100.0
                    +status.val[MAIN_RIGHT].i*img_dyErrorI/100.0
                    +status.val[MAIN_RIGHT].d*img_dyDiff/img_dyDiffT;

            tempValue[SIDE_UP]=status.val[SIDE_UP].p*img_dxError/100.0
                    +status.val[SIDE_UP].i*img_dxErrorI/100.0
                    +status.val[SIDE_UP].d*img_dxDiff/img_dxDiffT;
            tempValue[SIDE_DOWN]=status.val[SIDE_DOWN].p*img_dxError/100.0
                    +status.val[SIDE_DOWN].i*img_dxErrorI/100.0
                    +status.val[SIDE_DOWN].d*img_dxDiff/img_dxDiffT;

        }
 //   }
    for(int i=0;i<4;i++){
        if(tempValue[hList[i]]>=45){
            tempValue[hList[i]]=45;
        }
        else if(tempValue[hList[i]]<=-45){
            tempValue[hList[i]]=-45;
        }
    }
    for(int i=0;i<4;i++){
        tempList.push_back(make_pair<>(hList[i],tempValue[hList[i]]));
    }

    emit setHMotors(tempList);

}

void controller::endDropMarksimp()
{
    status.cnt.clear();
    //loadConfig(STOP);
    setZero();
    setDeepCtr(false);
    //emit missionFinished(DROP_MARK_SIMP);
    qDebug()<<"endDropMarksimp";
}

void controller::initDropIt()
{
    emit setGoal(6.0);
    loadConfig(HANG);
    updateConfig();
    setDeepCtr(true);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    status.cnt.push_back(0);
    qDebug()<<"initDropIt";
}

void controller::ctrDropIt()
{
    visionClass::0x01visionData && tmp=vision->getData();
    if(tmp.t_now==img.t_now){
        return;
    }

    if(tmp.m6_binsim_angle==999 || tmp.m6_binsim_dx==999 || tmp.m6_binsim_dy==999){
        //处理没有找到的情况。现在未完工
        return;
    }

    status.cnt[0]++;
    if(status.cnt[0]*status.ms>=30*1000 && status.cnt[1]==0){
         emit drop8(true);
         status.cnt[1]++;
    }
    if(status.cnt[0]*status.ms>=32*1000 && status.cnt[2]==0){
         emit drop8(false);
         status.cnt[2]++;
    }
    if(status.cnt[0]*status.ms>=33*1000 && status.cnt[3]==0){
         emit drop9(true);
         status.cnt[3]++;
    }
    if(status.cnt[0]*status.ms>=35*1000){
         emit drop9(false);
         if(task.cnt==1){
            if(status.cnt[0]*status.ms>=60*1000){
               QList<TASKS_ID> idList1;
               idList1.push_back(RANDOM_PINGER);
               for(auto t:idList1){
                   auto p=taskList.find(t);
                   if(p!=taskList.end()){
                        currentTaskList.push(p->second);
                   }
               }
            }
            task.list=1;
            task.cnt=2;
         }
         else if(task.cnt==2){
             setZero1();
         }
         setZero();
         emit endTask();
         return;
    }

    float tempValue[NUMBER_OF_MOTORS];
    const static MOTORS hList[4]={MAIN_LEFT,MAIN_RIGHT,SIDE_UP,SIDE_DOWN};
    QList<pair<MOTORS,float>> tempList;
    img.bin_angle_last=img.bin_angle;
    img.bin_dx_last=img.bin_dx;
    img.bin_dy_last=img.bin_dy;
    img.t_last=img.t_now;
    img.bin_angle=tmp.m6_binsim_angle;
    img.bin_dx=tmp.m6_binsim_dx;
    img.bin_dy=tmp.m6_binsim_dy;
    img.t_now=tmp.t_now;
    //根据angle修正,取与yaw相同pid值
    float img_angleError;
    img_angleError=img.bin_angle_goal-img.bin_angle;
    if(img_angleError>90){
       img_angleError=-180+img_angleError;
    }

    float img_dxError=img.bin_dx;
    static float img_dxErrorI;
    img_dxErrorI += img_dxError;
    float img_dxDiffT=(img.t_now-img.t_last)/1000.0;
    float img_dxDiff=img_dxError-img.bin_dx_last;
    img.bin_dx_last=img.bin_dx;

    float img_dyError=img.bin_dy;
    static float img_dyErrorI;
    img_dyErrorI += img_dyError;
    float img_dyDiffT=(img.t_now-img.t_last)/1000.0;
    float img_dyDiff=img_dyError-img.bin_dy_last;
    img.bin_dy_last=img.bin_dy;

    loadConfig(LOCATE_BOTTOM_BIN);
    tempValue[MAIN_LEFT]=status.val[MAIN_LEFT].p*img_dyError/100.0
            +status.val[MAIN_LEFT].i*img_dyErrorI/100.0
            +status.val[MAIN_LEFT].d*img_dyDiff/img_dyDiffT;
    tempValue[MAIN_RIGHT]=status.val[MAIN_RIGHT].p*img_dyError/100.0
            +status.val[MAIN_RIGHT].i*img_dyErrorI/100.0
            +status.val[MAIN_RIGHT].d*img_dyDiff/img_dyDiffT;

    tempValue[SIDE_UP]=status.val[SIDE_UP].p*img_dxError/100.0
            +status.val[SIDE_UP].i*img_dxErrorI/100.0
            +status.val[SIDE_UP].d*img_dxDiff/img_dxDiffT;
    tempValue[SIDE_DOWN]=status.val[SIDE_DOWN].p*img_dxError/100.0
            +status.val[SIDE_DOWN].i*img_dxErrorI/100.0
            +status.val[SIDE_DOWN].d*img_dxDiff/img_dxDiffT;
    for(int i=0;i<4;i++){
        if(tempValue[hList[i]]>=45){
            tempValue[hList[i]]=45;
        }
        else if(tempValue[hList[i]]<=-45){
            tempValue[hList[i]]=-45;
        }
    }
    for(int i=0;i<4;i++){
        tempList.push_back(make_pair<>(hList[i],tempValue[hList[i]]));
    }
    emit setHMotors(tempList);
}

void controller::endDropIt()
{
    status.cnt.clear();
    //loadConfig(STOP);
    setZero();
    setDeepCtr(false);
    //emit setAutoDeep(false);
    emit missionFinished(DROP_MARK_SIMP);
    qDebug()<<"endDropIt";
}
*/
