#include "piddialog.h"
#include <QDebug>

PIDDialog::PIDDialog(QWidget *parent)
    : QDialog(parent)
{
    configLabel=new QLabel(_WSTR(L"配置列表："));
    configList=new QListWidget;
    selectLabel=new QLabel(_WSTR(L"选择配置包："));
    configSelect=new QComboBox;
    setDefault=new QPushButton(_WSTR(L"设为默认"));
    configInfo=new QLabel();
    tableLabel=new QLabel(_WSTR(L"该配置包含的参数："));
    paraTable=new QTableWidget;
    {
        QStringList tempList;
        tempList << "Motor" << "p_value" << "i_value" << "d_value";
        paraTable->setColumnCount(4);
        paraTable->setRowCount(9);
        paraTable->setHorizontalHeaderLabels(tempList);
    }
    editButton=new QPushButton(_WSTR(L"编辑"));
    resetButton=new QPushButton(_WSTR(L"重置"));
    cancelButton=new QPushButton(_WSTR(L"取消"));
    saveButton=new QPushButton(_WSTR(L"保存"));

    mainLayout=new QGridLayout;
    mainLayout->addWidget(configLabel,1,1,1,1);
    mainLayout->addWidget(configList,2,1,11,2);
    mainLayout->addWidget(selectLabel,1,3,1,1);
    mainLayout->addWidget(configSelect,1,4,1,2);
    mainLayout->addWidget(setDefault,1,6,1,1);
    mainLayout->addWidget(configInfo,1,7,1,2);
    mainLayout->addWidget(tableLabel,2,3,1,2);
    mainLayout->addWidget(paraTable,3,3,10,5);
    mainLayout->addWidget(editButton,13,4,1,1);
    mainLayout->addWidget(resetButton,13,5,1,1);
    mainLayout->addWidget(cancelButton,13,6,1,1);
    mainLayout->addWidget(saveButton,13,7,1,1);

    mainLayout->setSizeConstraint(QLayout::SetFixedSize);

    setLayout(mainLayout);

    connect(editButton,&QPushButton::clicked,this,&PIDDialog::enableEdit);
    connect(saveButton,&QPushButton::clicked,this,&PIDDialog::saveValue);
    connect(resetButton,&QPushButton::clicked,this,&PIDDialog::resetEdit);
    connect(cancelButton,&QPushButton::clicked,this,&PIDDialog::cancelEdit);
    connect(setDefault,&QPushButton::clicked,this,&PIDDialog::onSetDefault);
}

void PIDDialog::setConfig(configManager *cfg)
{
    config=cfg;

    emit PIDInfo(true,_WSTR(L"正在等待PID参数加载..."));
    config->setPIDBasePath("/home/nwpu/Code/Robosub_test2018-2-8/configs");
    config->loadPID();

    updateList();
    //连接其他槽
    connect(configList,&QListWidget::currentItemChanged,this,&PIDDialog::updateCombox);
    connect(configSelect,
            static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this,
            &PIDDialog::updateTable);
}

void PIDDialog::updateTable()
{
    paraTable->clearContents();
    const configManager::pPIDCfg pCfg=configSelect->currentData().value<configManager::pPIDCfg>();
    configInfo->setText(QString::fromStdString(pCfg->parent.lock()->fileName));
    auto & motorList=config->getMotorList();
    QTableWidgetItem * tItem=nullptr;
    int cnt=0;
    for(auto & t:pCfg->value){
        int i=0;
        tItem=new QTableWidgetItem;
        tItem->setText(QString::fromStdString(motorList.left.find(t.motorID)->second));
        tItem->setData(MOTOR_ROLE,t.motorID);
        tItem->setFlags(Qt::ItemIsEnabled);
        paraTable->setItem(cnt,i++,tItem);
        tItem=new QTableWidgetItem;
        tItem->setText(QString::number(t.pValue));
        tItem->setData(Qt::UserRole, t.pValue);
        tItem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable);
        paraTable->setItem(cnt,i++,tItem);
        tItem=new QTableWidgetItem;
        tItem->setText(QString::number(t.iValue));
        tItem->setData(Qt::UserRole, t.iValue);
        tItem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable);
        paraTable->setItem(cnt,i++,tItem);
        tItem=new QTableWidgetItem;
        tItem->setText(QString::number(t.dValue));
        tItem->setData(Qt::UserRole, t.dValue);
        tItem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable);
        paraTable->setItem(cnt,i++,tItem);

        cnt++;
    }
    paraTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void PIDDialog::updateCombox()
{
    disconnect(configSelect,
            static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this,
            &PIDDialog::updateTable);

    editButton->setEnabled(false);
    configSelect->clear();
    auto & PIDList=config->getPIDList();
    auto & curPIDList=config->getCurPID();
    CONFIG_ID id=*((const CONFIG_ID *)(configList->currentItem()->data(CFG_ID_ROLE).data()));
    QString temp;
    auto it=PIDList.find(id);
    if(it==PIDList.end()){
        paraTable->clearContents();
        configInfo->clear();
        return;
    }

    for(auto & t:it->second){
        temp=QString::fromStdString(t->parent.lock()->cfgName);
        if(t==curPIDList.find(id)->second){
            temp+=_WSTR(L"-当前");
        }
        configSelect->addItem(temp,QVariant::fromValue(t));
    }
    updateTable();
    editButton->setEnabled(true);
    resetButton->setEnabled(false);
    cancelButton->setEnabled(false);
    saveButton->setEnabled(false);

    connect(configSelect,
            static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this,
            &PIDDialog::updateTable);
}

void PIDDialog::updateList()
{
    disconnect(configList,&QListWidget::currentItemChanged,this,&PIDDialog::updateCombox);
    configList->clear();
    auto & cfgIDList=config->getCfgIDList();
    auto & curPIDList=config->getCurPID();
    QListWidgetItem * tItem=nullptr;
    bool ok=true;
    for(auto & t:cfgIDList.left){
        tItem=new QListWidgetItem;
        tItem->setData(CFG_ID_ROLE,t.first);
        tItem->setText(QString::fromStdString(t.second));
        auto it=curPIDList.find(t.first);
        if(it==curPIDList.end() || it->second->value.empty()){
            tItem->setForeground(Qt::red);
            ok=false;
        }
        configList->addItem(tItem);
    }
    configList->setCurrentRow(0);
    updateCombox();

    if(ok){
        emit PIDInfo(ok,_WSTR(L"加载完成。"));
    }
    else {
        emit PIDInfo(ok,_WSTR(L"缺少某些配置。"));
    }

    connect(configList,&QListWidget::currentItemChanged,this,&PIDDialog::updateCombox);
}

void PIDDialog::enableEdit()
{
    paraTable->setEditTriggers(QAbstractItemView::DoubleClicked|QAbstractItemView::SelectedClicked);
    resetButton->setEnabled(true);
    cancelButton->setEnabled(true);
    saveButton->setEnabled(true);
    editButton->setEnabled(false);
    configList->setEnabled(false);
    configSelect->setEnabled(false);
}

void PIDDialog::saveValue()
{
    //auto & cfgIDList=config->getCfgIDList();
    const configManager::pPIDCfg pCfg=configSelect->currentData().value<configManager::pPIDCfg>();
    vector<configManager::PIDValue> tVect;
    configManager::PIDValue tVal;


    bool ok;
    for(int i=0;i<paraTable->rowCount();i++){
        int j=0;

        if(paraTable->item(i,0)==nullptr){
            break;
        }

        tVal.motorID=*((const MOTORS *)(paraTable->item(i,j++)->data(MOTOR_ROLE).data()));

        tVal.pValue=paraTable->item(i,j++)->text().toFloat(&ok);
        if(!ok){
            return;
        }

        tVal.iValue=paraTable->item(i,j++)->text().toFloat(&ok);
        if(!ok){
            return;
        }
        tVal.dValue=paraTable->item(i,j++)->text().toFloat(&ok);
        if(!ok){
            return;
        }

        tVect.push_back(tVal);
    }

    config->setPIDValue(pCfg->parent.lock()->cfgName,pCfg->id,tVect);
    config->writePID(pCfg->parent.lock()->cfgName);

    updateTable();
    exitEdit();
}

void PIDDialog::resetEdit()
{
    updateTable();
}

void PIDDialog::cancelEdit()
{
    resetEdit();
    exitEdit();
}

void PIDDialog::exitEdit()
{
    paraTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    resetButton->setEnabled(false);
    cancelButton->setEnabled(false);
    saveButton->setEnabled(false);
    editButton->setEnabled(true);
    configList->setEnabled(true);
    configSelect->setEnabled(true);
}

void PIDDialog::onSetDefault()
{
    configManager::pPIDCfg pCfg=configSelect->currentData().value<configManager::pPIDCfg>();
    config->setCurPID(pCfg->id,pCfg);
    updateCombox();
}

void PIDDialog::closeEvent(QCloseEvent *event)
{
    this->hide();
}
