#ifndef REMOTEDIALOG_H
#define REMOTEDIALOG_H

#include <QDialog>
#include <QButtonGroup>
#include <QPushButton>
#include <QRadioButton>
#include <QGroupBox>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCloseEvent>

#include <vector>
#include <utility>

#include <boost/assert.hpp>

using std::vector;
using std::pair;
using std::make_pair;

#include "global.h"
#include "controller.h"
#include "transceiver.h"


class remoteDialog : public QDialog
{
    Q_OBJECT
public:
    remoteDialog(QWidget *parent = 0);
    void setCtr(controller * pCtr);
    void setTrans(transceiver * pTrans);
private:

    enum STATE{
        CONNECTED=0,
        DISCONNECTED=1
    };

    enum FORREV{
        FORWARD=0,
        REVERSE=1,
        STOP=2
    };

    enum TRANSDOF{
        FORBACK=1,
        UPDOWN=2,
        LEFTRIGHT=3
    };

    const static int SPEED_LIMIT=80;
    const static int STEP=2;
    bool hand_status = false;
    bool step_ok = false;
    const MOTORS vList[4]={LEFT_UP,LEFT_DOWN,RIGHT_UP,RIGHT_DOWN};
    const MOTORS hList[4]={MAIN_LEFT,MAIN_RIGHT,SIDE_UP,SIDE_DOWN};
    const MOTORS xList[2]={MAIN_LEFT,MAIN_RIGHT};
    const MOTORS zList[2]={SIDE_UP,SIDE_DOWN};

    QGroupBox * motorGroup;
    const vector<QString> motorNameStr; 
    vector<QGroupBox *> motorNameGroup;
    vector<QVBoxLayout *> motorNameLayout;
    vector<QButtonGroup *> motorCtr;
    vector<vector<QRadioButton *>> motorRadio;
    QPushButton * motorUp;
    QPushButton * motorDown;
    QPushButton * motorClose;
    QGroupBox * sevoGroup;
    QPushButton * sevoOpen;
    QPushButton * sevoClose;
    QLineEdit * sevoPulse;
    QPushButton * sevoPulseButton;

    QPushButton * stepOpen;
    QPushButton * stepClose;
    QPushButton * stepHandControlButton;
    QLineEdit * stepTime;
    QGroupBox * stepGroup;

    QLineEdit * DeepIntUp;
    QLineEdit * DeepIntDown;
    QLineEdit * DeepCtrUp;
    QLineEdit * DeepCtrDown;
    QPushButton * ChangeDeepLimit;
    QPushButton * DefaultDeepLimit;
    QGroupBox * DeepLimitGroup;
    QLabel * DeepIntUpLabel;
    QLabel * DeepIntDownLabel;
    QLabel * DeepCtrUpLabel;
    QLabel * DeepCtrDownLabel;


    const static int NUMBER_OF_COMBINES=8;
    const vector<QString> combineStr;
    vector<QPushButton *> combineButton;
    const vector<void (remoteDialog::*)()> combineFunc;
    QGridLayout * motorLayout;
    float motorValue[NUMBER_OF_MOTORS];

    const static int NUMBER_OF_TRANDOF=3;
    QComboBox * TransDofSelect;
    const vector<QString> TransDofStr;
    bimap<TRANSDOF,string> dofIDList;

    QGroupBox * relayGroup;
    const vector<QString> relayNameStr;
    vector<QButtonGroup *> relayCtr;
    vector<QGroupBox *> relayNameGroup;
    vector<QVBoxLayout *> relayNameLayout;
    vector<pair<QRadioButton *,QRadioButton *>> relayRadio;
    QPushButton * resetAcos;
    QPushButton * resetStm;
    QPushButton * allOpen;
    QPushButton * allClose;
    QGridLayout * relayLayout;
    QGridLayout * sevoLayout;
    QGridLayout * stepLayout;
    QGridLayout * DeepLimitLayout;
    QGridLayout * mainLayout;

    controller * ctr;
    transceiver * trans;
    void onHMotorChanged();
    void onVMotorChanged();
    void onRelayChanged();

    void onIncrease();
    void onDecrease();
    void onMotorClose();

    void onOpenSevo();
    void onCloseSevo();
    void onChangeSevoPulse();

    void onOpenStep();
    void onCloseStep();
    void onChangeHandControlStep();

    void onChangeDeepLimit();
    void onDefaultDeepLimit();

    void onForward();
    void onBackward();
    void onMoveUp();
    void onMoveDown();
    void onClockwise();
    void onAntiClockwise();
    void onLeftward();
    void onRightward();

    void onFBInc();
    void onFBDec();
    void onUDInc();
    void onUDDec();
    void onLRInc();
    void onLRDec();
    int getTransDof(QString& Str);

    void onRelayOn();
    void onRelayoff();

protected:
    void closeEvent(QCloseEvent * event);

signals:
    void setHMotors(QList<pair<MOTORS,float>> value);
    void setVMotors(QList<pair<MOTORS,float>> value);
    void setRelay(QList<pair<int,bool>> value);
    void setSevo(bool ok,int pulse = 0);
    void setStep(bool ok,unsigned int time = 0,bool isHandControl = false,bool distance = 0);
    void setDeepPIDRev(int dIntUp,int dIntDown,int dCtrUp,int dCtrDown);
};

#endif // REMOTEDIALOG_H
