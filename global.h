#ifndef GLOBAL_H
#define GLOBAL_H

#include <QAbstractItemModel>

#define _WSTR(x) QString::fromWCharArray(x)

const int NUMBER_OF_MOTORS=8;

enum MOTORS{
    LEFT_UP=0,
    LEFT_DOWN=1,
    RIGHT_UP=2,
    RIGHT_DOWN=3,
    MAIN_LEFT=4,
    MAIN_RIGHT=5,
    SIDE_UP=6,
    SIDE_DOWN=7
};
enum TASKS_ID{
    //USA
    ALL=0,
    DEEP_TEST=1,
    FORWARD=2,
    START=3,
    YAW_TEST=4,
    PATH_DIR1=5,
    PATH_DIR=6,
    HIT_BALL=7,
    PASS_GATE=8,
    RANDOM_PINGER=9,
    GO_UP=10,
    DROP_MARK=11,
    OPEN_BIN=12,
    DROP_MARK1=13,
    GATE_ACT=14,
    DROP_IT=15,
    DROP_MARK_SIMP=16,
    YAW_TEST1=17,
    FORWARD1=18,
    TURN=19,
    ACTION_TEST = 20,
    //Singapore
    Gate = 21,
    Drop = 22,
    Flare = 23,
    Acquire = 24,
    FORWARD_SBG = 25,
    BACKWARD_SBG = 26,
    ROTATE_SBG = 27,
    HANG_SBG = 28,
    SWING_SBG = 29,
    GET_PHOTOF = 30,
    GET_PHOTOB = 31,
    Gate2 = 32,
    Gate3 = 33,
    Gate4 = 34,
    PreTask = 35
};

enum ACTION_ID{
    NOP=0,
    ACTION1 = 1,
    ACTION2 = 2,
    ACTION3 = 3,
    ACTION4 = 4,
    ACTION5 = 5,
    ACTION6 = 6,
    FORWARDACTION = 7,
    BACKWARDACTION = 8,
    ROTATEACTION = 9,
    HANGACTION = 10,
    ACOSREVISE = 11,
    DRUMREVISE = 12,
    BALLREVISE = 13,
    DIVINGACTION = 14,
    GETTINGACTION = 15,
    GRABACTION = 16,
    ABANDON = 17,
    SWINGACTION = 18,
    FIND_THE_GATE = 19,
    FIND_THE_GATE2 = 20,
    FIND_THE_GATE3 = 21,
    FORWARD_GATE = 22,
    ARM_DOWN = 23,
    ARM_UP = 24,
    BACKWARDDRUM = 25
};

enum CONFIG_ID{
    STOP=0, //停止
    HANG=1, //悬停
    FORWARD_SLOW=2,  //慢速前进（由水声或sbg角度导引）
    BACKWARD=3,  //后退
    ROTATING=4,  //仅旋转
    LOCATE_BOTTOM=5,  //下视导引的平移
    FORWARD_FORSEE=6, //前视
    FORWARD_UPDOWN=7,
    LOCATE_BOTTOM_BIN=8,
    LID_UPDOWN=9,
    SWING=10,
    FORWARD_ACOS=11,
    GATE_FORSEE=12
};


const int NUMBER_OF_RELAYS=12;

enum ROLES{
    CFG_ID_ROLE=Qt::UserRole+0,
    MOTOR_ROLE=Qt::UserRole+1,
    TASK_ID_ROLE=Qt::UserRole+2
};

Q_DECLARE_METATYPE(MOTORS)
Q_DECLARE_METATYPE(TASKS_ID)
Q_DECLARE_METATYPE(ACTION_ID)
Q_DECLARE_METATYPE(CONFIG_ID)

#endif // GLOBAL_H
