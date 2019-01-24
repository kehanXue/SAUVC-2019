#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QObject>
#include <QString>

#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <regex>

#include <boost/algorithm/string.hpp>
#include <boost/bimap.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <boost/assign.hpp>

#include "global.h"

using boost::split;
using boost::to_upper;
using boost::trim;
using boost::bimap;
using boost::token_compress_on;
using boost::istarts_with;
using boost::is_any_of;
using boost::is_space;
using boost::ierase_first;
using boost::shared_ptr;
using boost::weak_ptr;

using std::fstream;
using std::getline;
using std::string;
using std::vector;
using std::unordered_map;
using std::regex;
using std::regex_match;
using std::regex_search;
using std::smatch;
using std::make_pair;

class configManager : public QObject
{
    Q_OBJECT
public:
    explicit configManager(QObject *parent = 0);

    struct  PIDValue{
        MOTORS motorID;
        float pValue;
        float iValue;
        float dValue;
    };

    struct PIDFile;

    struct PIDCfg{
        weak_ptr<PIDFile> parent;
        CONFIG_ID id;
        vector<PIDValue> value;
    };

    typedef shared_ptr<PIDCfg> pPIDCfg;

    struct PIDFile{
        string fileName;    //该配置的文件名
        string cfgName;     //该配置辨识名称，不可重复
        vector<pPIDCfg> cfgList;
    };

    struct taskPack{
        string fileName;
        string packName;
        vector<TASKS_ID> taskList;
    };

private:
    //PID部分
    string PIDBasePath;

    const string PIDCfgHeader="PID_CFG IN NWPU AUV";
    const string configIdStr="CONFIG_ID:";
    const string configNameStr="CONFIG_NAME:";
    const string configSwitch="NEXT_CONFIG:";

    bimap<CONFIG_ID,string> cfgIDList;                 //CONFIG_ID与对应字符串的互查找表
    bimap<MOTORS,string> motorList;                     //MOTOR与对应字符串互查找表
    unordered_map<CONFIG_ID,vector<pPIDCfg>,std::hash<int>> PIDList;   //当前所有已载入的配置
    unordered_map<CONFIG_ID,pPIDCfg,std::hash<int>> currentPID;        //当前使用的配置组合
    unordered_map<string,shared_ptr<PIDFile>> cfgNameList;          //配置名称查找表

    //任务包部分
    string taskBasePath;

    const string packHeader="TASK_PACK IN NWPU AUV";
    const string packNameStr="PACK_NAME:";

    bimap<TASKS_ID,string> taskIDList;
    unordered_map<string,taskPack> taskPackList;

    //公用部分

    vector<string> getAllInfo(const char * path, const char * postfix=nullptr, const char * prefix=nullptr);
    void skipInvalidLines(fstream & fin, string & str);


public:

    void setPIDBasePath(const char * path);
    void loadPID();
    void writePID(const string & cfgName);

    std::add_const<decltype((PIDList))>::type getPIDList() { return PIDList; }
    void setPIDValue(const string & cfgName, CONFIG_ID id, const vector<PIDValue> & val);
    const decltype(currentPID) & getCurPID() const { return currentPID; }
    void setCurPID(CONFIG_ID id, pPIDCfg cfg);
    const decltype(cfgIDList) & getCfgIDList() const { return cfgIDList; }
    const decltype(motorList) & getMotorList() const { return motorList; }

    const vector<PIDValue> & getPID(CONFIG_ID id) const { return currentPID.find(id)->second->value; }

    void setTaskBasePath(const char * path);
    void loadTask();

    vector<QString> getPackList() const;
    vector<TASKS_ID> getTaskIDList(const QString & packName) const;
    vector<QString> getTaskStrList(const QString & packName) const;


signals:

public slots:
};

Q_DECLARE_METATYPE(configManager::pPIDCfg)

#endif // CONFIGMANAGER_H
