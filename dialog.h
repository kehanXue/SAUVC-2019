#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QStandardItemModel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QString>
#include <QGroupBox>
#include <QLineEdit>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QThread>

#include <vector>
#include <utility>

#include <boost/assert.hpp>
#include <boost/static_assert.hpp>

#include "global.h"
#include "configmanager.h"
#include "remotedialog.h"
#include "piddialog.h"
#include "ui_dialog.h"
#include "visiondialog.h"
#include "controller.h"
#include "transceiver.h"
#include "visionclass.h"
#include "recordmanager.h"

using std::vector;
using std::pair;

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = 0);
    ~Dialog();

private:
    //任务选择相关
    QGroupBox * taskGroup;
    QLabel * taskListLabel;
    QListWidget * taskList;
    //QList<QList<QListWidgetItem>> * tasks;
    QLabel * taskPckLabel;
    QLabel * curTaskStatusLabel;
    QLabel * curactionStatusLabel;
    QComboBox * selectTaskPck;
    QPushButton * startTask;
    QPushButton * stopTask;
    QGridLayout * taskLayout;

    void updateTaskList();
    void updatePackCombo();

    //PID参数选择
    QGroupBox * PIDGroup;
    QLabel * PIDStatus;
    QPushButton * editPID;
    QHBoxLayout * PIDLayout;

    void onPIDInfo(bool ok, QString info);
    void onEditPID();

    //仪表板tuple规范：
    typedef pair<QLabel*,QLabel*> labelItem;
    QLabel * getLabel(labelItem & i) { return i.second; }
    void initLabel(labelItem & i, const QString & str){
        i.first=new QLabel(str);
        i.second=new QLabel("null");
    }
    template <typename ArrayType, typename StrArray>
    void initLabelArray(ArrayType & arr,QGridLayout * layout, const StrArray arrStr){
        BOOST_ASSERT(arr.size() == arrStr.size());
        int i=1,j=1;
        int k=0;
        for(labelItem & p: arr){
            initLabel(p,arrStr[k++]);
            layout->addWidget(p.first,i,j,1,1);
            layout->addWidget(p.second,i,++j,1,1);
            j>=4? j=1,i++ : ++j;
        }
    }



    QGroupBox * monitorGroup;
    QGridLayout * monitorLayout;

    //SBG仪表板
    QGroupBox * sbgGroup;
    enum {
        SBG_YAW=0,
        SBG_ROLL=1,
        SBG_PITCH=2,
        SBG_MAG=3,
        //保证始终在最后
        SBG_PARA_NUM=4
    };
    vector<labelItem> sbgLists;
    const vector<QString> sbgStrs;
    QGridLayout * sbgLayout;

    //水声仪表板
    QGroupBox * acosGroup;
    enum {
        ACOS_THETA1=0,
        ACOS_PHI1=1,
        ACOS_THETA2=2,
        ACOS_PHI2=3,
        //保证始终在最后onSetTarget
        ACOS_PARA_NUM=4
    };
    vector<labelItem> acosLists;
    const vector<QString> acosStrs;
    QGridLayout * acosLayout;

    //深度仪表板
    QGroupBox * deepGroup;
    QGroupBox *sbgTarget;
    enum {
        DEEP_VALUE=0,
        DEEP_REVISE=1,
        //保证始终在最后
        DEEP_PARA_NUM=2
    };
    vector<labelItem> deepLists;
    const vector<QString> deepStrs;
    QLabel * enterNewRev;
    QLineEdit * newRev;
    QPushButton * setNewRev;
    QGridLayout * deepLayout;

    vector<labelItem> TargetLists;
    const vector<QString> TargetStrs;
    QLabel * enterTarget;
    QLineEdit * Target;
    QPushButton * setTarget;
    QGridLayout * TargetLayout;

    //温湿仪表板
    QGroupBox * humiAndHeatGroup;
    enum {
        HUH_HUMI1=0,
        HUH_HEAT1=1,
        HUH_HUMI2=2,
        HUH_HEAT2=3,
        //保证始终在最后
        HUH_PARA_NUM=4
    };
    vector<labelItem> humiAndHeatLists;
    const vector<QString> humiAndHeatStrs;
    QGridLayout * humiAndHeatLayout;

    //继电器仪表板
    QGroupBox * relayGroup;
    enum {
        RELAY_VALUE=0,
        //保证始终在最后
        RELAY_PARA_NUM=1
    };
    vector<labelItem> relayLists;
    const vector<QString> relayStrs;
    QPushButton * resetStm;
    QPushButton * resetAcos;
    QPushButton * openRmt;
    QPushButton * openVis;
    QGridLayout * relayLayout;

    void onOpenRmt();

    void onOpenVis();
    //通信仪表板
    QGroupBox * connectionGroup;
    enum {
        CONNECTION_TARGET=0,
        CONNECTION_STATUS=1,
        CONNECTION_ACOS_TARGET=2,
        CONNECTION_ACOS_STATUS=3,
        //保证始终在最后
        CONNECTION_PARA_NUM=4
    };
    vector<labelItem> connectionLists;
    const vector<QString> connectionStrs;
    QHBoxLayout * connectionLayout;
    //QPushButton * openAcos;
    //QPushButton * closeAcos;

    //转速仪表板
    QGroupBox * speedGroup;
    enum {
        SPEED_LEFT_UP=0,
        SPEED_LEFT_DOWN=1,
        SPEED_RIGHT_UP=2,
        SPEED_RIGHT_DOWN=3,
        SPEED_MAIN_LEFT=4,
        SPEED_MAIN_RIGHT=5,
        SPEED_SIDE_UP=6,
        SPEED_SIDE_DOWN=7,
        //保证始终在最后
        SPEED_PARA_NUM=8
    };
    vector<labelItem> speedLists;
    const vector<QString> speedStrs;
    QGridLayout * speedLayout;

    //图像仪表板
    QGroupBox * visionGroup;
    //QLabel * filenameLabel;
    //QListWidget * filenameList;
    //QPushButton * freshFile;
    QGridLayout * visionLayout;

    //数据记录仪表版
    QGroupBox * recordGroup;
    QPushButton * acosRecord;
    QPushButton * ctrRecord;
    QPushButton * visionRecord;
    //QPushButton * photoRecord;
    QPushButton * photoFRecord;
    QPushButton * photoBRecord;
    QPushButton * transRecord;
    QPushButton * allCloseRecord;
    QPushButton * allOpenRecord;
    QGridLayout * recordLayout;

    QGridLayout * mainLayout;

    QThread * visionThread;
    visionClass * vision;

    remoteDialog * remoteDlg;
    PIDDialog * pidDlg;
    visionDialog * visDlg;

    configManager * config;

    QThread * ctrThread;
    controller * ctr;

    QThread * transThread;
    transceiver * trans;

    QThread * recordThread;
    recordManager * record;
    int record_status;

    void onStartTask();
    //void onChangeCurTasksLabel(TASKS_ID);
    void onChangeCurActionLabel(ACTION_ID);
    void onStopTask();
    void onFinishedTask();

    void onConnected(bool connected,QHostAddress addr,int port);
    void onSbgRecv(float roll, float pitch, float yaw, float magNorm);
    void onAcosRecv(float theta1, float phi1, float theta2, float phi2);
    void onDeepRecv(float deep);
    void onSpeedRecv(float lu,float ld,float ru,float rd);
    void onHTRecv(float humi1,float tem1,float humi2,float tem2);
    void onSetHMotors(QList<pair<MOTORS,float>> value);
    void onSetVMotors(QList<pair<MOTORS,float>> value);
    void onSetRelay(QList<pair<int,bool>> value);
    void onSetRev();
    void onSetTarget();

    void onOpenAcosClicked();
    void onCloseAcosClicked();
    void onOpenAcosRecord();
    void onCloseAcosRecord();
    void onOpenCtrRecord();
    void onCloseCtrRecord();
    void onOpenTransRecord();
    void onCloseTransRecord();
    void onOpenVisionRecord();
    void onCloseVisionRecord();
    void onOpenPhotoFRecord();
    void onOpenPhotoBRecord();
    void onClosePhotoFRecord();
    void onClosePhotoBRecord();
    void onAllCloseRecord();
    void onAllOpenRecord();
    void onSetRecordStatus(int sw);
    void onAcosConnected(QHostAddress addr, bool ok);

    void onImageShow(Mat * mat, const char * name);
    void onCreateWindow(const char * name);
    void onDestroyWindow();
    void createBar(const char * barName, const char *winName, int *val, int count);

signals:
    void startSig();
    void stopSig();

    void initAll();

    void setDeepRev(float);
    void setYawTarget(float);
    void setRecordStatus(int);
};

#endif // DIALOG_H
