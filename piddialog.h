#ifndef PIDDIALOG_H
#define PIDDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QListWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QGridLayout>
#include <QStringList>
#include <QCloseEvent>

#include <utility>

#include "configmanager.h"
#include "global.h"

using std::pair;
using std::make_pair;

class PIDDialog : public QDialog
{
    Q_OBJECT

public:
    PIDDialog(QWidget *parent = 0);

    void setConfig(configManager * cfg);

private:
    QLabel * configLabel;
    QListWidget * configList;
    QLabel * selectLabel;
    QComboBox * configSelect;
    QPushButton * setDefault;
    QLabel * configInfo;
    QLabel * tableLabel;
    QTableWidget * paraTable;
    QPushButton * editButton;
    QPushButton * resetButton;
    QPushButton * cancelButton;
    QPushButton * saveButton;

    QGridLayout * mainLayout;

    configManager * config;


    void updateTable();
    void updateCombox();
    void updateList();
    void enableEdit();
    void saveValue();
    void resetEdit();
    void cancelEdit();
    void exitEdit();
    void onSetDefault();

protected:
    void closeEvent(QCloseEvent *event);

signals:
    void PIDInfo(bool ok, QString info);
};

#endif // PIDDIALOG_H
