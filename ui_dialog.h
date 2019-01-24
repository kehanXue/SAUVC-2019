/********************************************************************************
** Form generated from reading UI file 'dialog.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_H
#define UI_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QWidget *formLayoutWidget;
    QFormLayout *formLayout;
    QLabel *camera;
    QSpinBox *cameraspinBox;
    QLabel *epmode;
    QSpinBox *epmodespinBox;
    QLabel *wbmode;
    QSpinBox *wbmodespinBox;
    QLabel *psmode;
    QSpinBox *psmodespinBox;
    QLabel *epsmax;
    QSpinBox *epsmaxspinBox;
    QLabel *epattg;
    QSpinBox *epattgspinBox;
    QLabel *wb_red;
    QDoubleSpinBox *wb_redSpinBox;
    QLabel *wbBlue;
    QDoubleSpinBox *wbbluespinBox;
    QLabel *start;
    QPushButton *startButton;
    QLabel *stop;
    QPushButton *stopButton;
    QLabel *mission;
    QLabel *test;
    QPushButton *testButton;
    QPushButton *missionButton;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QLabel *camB;
    QLabel *camF;
    QLabel *label;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QStringLiteral("Dialog"));
        Dialog->setEnabled(true);
        Dialog->resize(617, 623);
        Dialog->setMouseTracking(false);
        formLayoutWidget = new QWidget(Dialog);
        formLayoutWidget->setObjectName(QStringLiteral("formLayoutWidget"));
        formLayoutWidget->setGeometry(QRect(20, 20, 207, 591));
        formLayout = new QFormLayout(formLayoutWidget);
        formLayout->setSpacing(6);
        formLayout->setContentsMargins(11, 11, 11, 11);
        formLayout->setObjectName(QStringLiteral("formLayout"));
        formLayout->setHorizontalSpacing(4);
        formLayout->setVerticalSpacing(22);
        formLayout->setContentsMargins(0, 0, 0, 0);
        camera = new QLabel(formLayoutWidget);
        camera->setObjectName(QStringLiteral("camera"));

        formLayout->setWidget(1, QFormLayout::LabelRole, camera);

        cameraspinBox = new QSpinBox(formLayoutWidget);
        cameraspinBox->setObjectName(QStringLiteral("cameraspinBox"));
        cameraspinBox->setEnabled(true);
        cameraspinBox->setMinimum(-1);
        cameraspinBox->setMaximum(1);
        cameraspinBox->setValue(-1);

        formLayout->setWidget(1, QFormLayout::FieldRole, cameraspinBox);

        epmode = new QLabel(formLayoutWidget);
        epmode->setObjectName(QStringLiteral("epmode"));

        formLayout->setWidget(2, QFormLayout::LabelRole, epmode);

        epmodespinBox = new QSpinBox(formLayoutWidget);
        epmodespinBox->setObjectName(QStringLiteral("epmodespinBox"));
        epmodespinBox->setMinimum(-1);
        epmodespinBox->setMaximum(2);
        epmodespinBox->setValue(-1);

        formLayout->setWidget(2, QFormLayout::FieldRole, epmodespinBox);

        wbmode = new QLabel(formLayoutWidget);
        wbmode->setObjectName(QStringLiteral("wbmode"));

        formLayout->setWidget(3, QFormLayout::LabelRole, wbmode);

        wbmodespinBox = new QSpinBox(formLayoutWidget);
        wbmodespinBox->setObjectName(QStringLiteral("wbmodespinBox"));
        wbmodespinBox->setMinimum(-1);
        wbmodespinBox->setMaximum(2);
        wbmodespinBox->setValue(-1);

        formLayout->setWidget(3, QFormLayout::FieldRole, wbmodespinBox);

        psmode = new QLabel(formLayoutWidget);
        psmode->setObjectName(QStringLiteral("psmode"));

        formLayout->setWidget(4, QFormLayout::LabelRole, psmode);

        psmodespinBox = new QSpinBox(formLayoutWidget);
        psmodespinBox->setObjectName(QStringLiteral("psmodespinBox"));
        psmodespinBox->setMinimum(-1);
        psmodespinBox->setMaximum(2);
        psmodespinBox->setValue(-1);

        formLayout->setWidget(4, QFormLayout::FieldRole, psmodespinBox);

        epsmax = new QLabel(formLayoutWidget);
        epsmax->setObjectName(QStringLiteral("epsmax"));

        formLayout->setWidget(5, QFormLayout::LabelRole, epsmax);

        epsmaxspinBox = new QSpinBox(formLayoutWidget);
        epsmaxspinBox->setObjectName(QStringLiteral("epsmaxspinBox"));
        epsmaxspinBox->setMinimum(-1);
        epsmaxspinBox->setMaximum(1000000);
        epsmaxspinBox->setValue(-1);

        formLayout->setWidget(5, QFormLayout::FieldRole, epsmaxspinBox);

        epattg = new QLabel(formLayoutWidget);
        epattg->setObjectName(QStringLiteral("epattg"));

        formLayout->setWidget(6, QFormLayout::LabelRole, epattg);

        epattgspinBox = new QSpinBox(formLayoutWidget);
        epattgspinBox->setObjectName(QStringLiteral("epattgspinBox"));
        epattgspinBox->setMinimum(-1);
        epattgspinBox->setMaximum(100);
        epattgspinBox->setValue(-1);

        formLayout->setWidget(6, QFormLayout::FieldRole, epattgspinBox);

        wb_red = new QLabel(formLayoutWidget);
        wb_red->setObjectName(QStringLiteral("wb_red"));

        formLayout->setWidget(7, QFormLayout::LabelRole, wb_red);

        wb_redSpinBox = new QDoubleSpinBox(formLayoutWidget);
        wb_redSpinBox->setObjectName(QStringLiteral("wb_redSpinBox"));
        wb_redSpinBox->setMinimum(-1);
        wb_redSpinBox->setMaximum(3);
        wb_redSpinBox->setSingleStep(0.01);
        wb_redSpinBox->setValue(-1);

        formLayout->setWidget(7, QFormLayout::FieldRole, wb_redSpinBox);

        wbBlue = new QLabel(formLayoutWidget);
        wbBlue->setObjectName(QStringLiteral("wbBlue"));

        formLayout->setWidget(8, QFormLayout::LabelRole, wbBlue);

        wbbluespinBox = new QDoubleSpinBox(formLayoutWidget);
        wbbluespinBox->setObjectName(QStringLiteral("wbbluespinBox"));
        wbbluespinBox->setMinimum(-1);
        wbbluespinBox->setMaximum(3);
        wbbluespinBox->setSingleStep(0.01);
        wbbluespinBox->setValue(-1);

        formLayout->setWidget(8, QFormLayout::FieldRole, wbbluespinBox);

        start = new QLabel(formLayoutWidget);
        start->setObjectName(QStringLiteral("start"));

        formLayout->setWidget(9, QFormLayout::LabelRole, start);

        startButton = new QPushButton(formLayoutWidget);
        startButton->setObjectName(QStringLiteral("startButton"));
        startButton->setEnabled(false);
        startButton->setAutoDefault(false);

        formLayout->setWidget(9, QFormLayout::FieldRole, startButton);

        stop = new QLabel(formLayoutWidget);
        stop->setObjectName(QStringLiteral("stop"));

        formLayout->setWidget(10, QFormLayout::LabelRole, stop);
        stopButton = new QPushButton(formLayoutWidget);
        stopButton->setObjectName(QStringLiteral("stopButton"));
        stopButton->setEnabled(false);
        stopButton->setAutoDefault(false);

        formLayout->setWidget(10, QFormLayout::FieldRole, stopButton);

        mission= new QLabel(formLayoutWidget);
        mission->setObjectName(QStringLiteral("mission"));

        formLayout->setWidget(11, QFormLayout::LabelRole, mission);

        test = new QLabel(formLayoutWidget);
        test->setObjectName(QStringLiteral("test"));

        formLayout->setWidget(12, QFormLayout::LabelRole, test);

        testButton = new QPushButton(formLayoutWidget);
        testButton->setObjectName(QStringLiteral("testButton"));
        testButton->setAutoDefault(false);
        testButton->setEnabled(true);

        formLayout->setWidget(12, QFormLayout::FieldRole, testButton);

        missionButton = new QPushButton(formLayoutWidget);
        missionButton->setObjectName(QStringLiteral("missionButton"));
        missionButton->setEnabled(true);
        missionButton->setAutoDefault(false);

        formLayout->setWidget(11, QFormLayout::FieldRole, missionButton);

        stop->raise();
        camera->raise();
        cameraspinBox->raise();
        epmode->raise();
        epmodespinBox->raise();
        wbmode->raise();
        wbmodespinBox->raise();
        psmode->raise();
        epsmax->raise();
        epsmaxspinBox->raise();
        epattg->raise();
        epattgspinBox->raise();
        wb_red->raise();
        wb_redSpinBox->raise();
        wbBlue->raise();
        wbbluespinBox->raise();
        psmodespinBox->raise();
        startButton->raise();
        start->raise();
        stopButton->raise();
        mission->raise();
        test->raise();
        testButton->raise();
        missionButton->raise();
        verticalLayoutWidget = new QWidget(Dialog);
        verticalLayoutWidget->setObjectName(QStringLiteral("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(240, 20, 352, 541));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setSpacing(2);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setSizeConstraint(QLayout::SetFixedSize);
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        camB = new QLabel(verticalLayoutWidget);
        camB->setObjectName(QStringLiteral("camB"));
        camB->setEnabled(true);
        camB->setMinimumSize(QSize(315, 250));
        camB->setMaximumSize(QSize(315, 250));
        camB->setFrameShape(QFrame::Box);

        verticalLayout->addWidget(camB);

        camF = new QLabel(verticalLayoutWidget);
        camF->setObjectName(QStringLiteral("camF"));
        camF->setEnabled(true);
        camF->setMinimumSize(QSize(315, 250));
        camF->setMaximumSize(QSize(315, 250));
        camF->setBaseSize(QSize(0, 0));
        camF->setFrameShape(QFrame::Box);
        camF->setFrameShadow(QFrame::Plain);
        camF->setTextFormat(Qt::AutoText);

        verticalLayout->addWidget(camF);

        label = new QLabel(Dialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(750, 130, 111, 151));

        retranslateUi(Dialog);

        testButton->setDefault(false);


        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QApplication::translate("Dialog", "vision", 0));
        camera->setText(QApplication::translate("Dialog", "    camera", 0));
        epmode->setText(QApplication::translate("Dialog", "    epmode", 0));
        wbmode->setText(QApplication::translate("Dialog", "    wbmode", 0));
        psmode->setText(QApplication::translate("Dialog", "    psmode", 0));
        epsmax->setText(QApplication::translate("Dialog", "    epsmax", 0));
        epattg->setText(QApplication::translate("Dialog", "    epattg", 0));
        wb_red->setText(QApplication::translate("Dialog", "    wb_red", 0));
        wbBlue->setText(QApplication::translate("Dialog", "    wbBlue", 0));
        start->setText(QApplication::translate("Dialog", "    start", 0));
        startButton->setText(QApplication::translate("Dialog", "start", 0));
        stop->setText(QApplication::translate("Dialog", "    stop", 0));
        stopButton->setText(QApplication::translate("Dialog", "stop", 0));
        mission->setText(QApplication::translate("Dialog", "    mission", 0));
        test->setText(QApplication::translate("Dialog", "    test", 0));
        testButton->setText(QApplication::translate("Dialog", "test", 0));
        missionButton->setText(QApplication::translate("Dialog", "mission", 0));
        camB->setText(QString());
        camF->setText(QString());
        label->setText(QApplication::translate("Dialog", "TextLabel", 0));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_H
