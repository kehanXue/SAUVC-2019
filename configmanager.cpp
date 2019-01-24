#include "configmanager.h"
#include <dirent.h>
#include <QDebug>

configManager::configManager(QObject *parent) : QObject(parent)
{   
    cfgIDList=boost::assign::list_of<bimap<CONFIG_ID,string>::relation>\
            (STOP,"STOP")(HANG,"HANG")(FORWARD_SLOW,"FORWARD_SLOW")
            (BACKWARD,"BACKWARD")
            (ROTATING,"ROTATING")(LOCATE_BOTTOM,"LOCATE_BOTTOM")
            (BACKWARD,"BACKWARD")(FORWARD_FORSEE,"FORWARD_FORSEE")
            (FORWARD_UPDOWN,"FORWARD_UPDOWN")(LOCATE_BOTTOM_BIN,"LOCATE_BOTTOM_BIN")
            (LID_UPDOWN,"LID_UPDOWN")(SWING,"SWING")(FORWARD_ACOS,"FORWARD_ACOS")(GATE_FORSEE,"GATE_FORSEE");
    motorList=boost::assign::list_of<bimap<MOTORS,string>::relation>\
        (LEFT_UP,"LEFT_UP")\
        (LEFT_DOWN,"LEFT_DOWN")\
        (RIGHT_UP,"RIGHT_UP")\
        (RIGHT_DOWN,"RIGHT_DOWN")\
        (MAIN_LEFT,"MAIN_LEFT")\
        (MAIN_RIGHT,"MAIN_RIGHT")\
        (SIDE_UP,"SIDE_UP")\
        (SIDE_DOWN,"SIDE_DOWN");
    taskIDList=boost::assign::list_of<bimap<TASKS_ID,string>::relation>\
            (DEEP_TEST,"DEEP_TEST")\
            (RANDOM_PINGER,"RANDOM_PINGER")\
            (FORWARD,"FORWARD")\
            (FORWARD1,"FORWARD1")\
            (START,"START")\
            (GO_UP,"GO_UP")\
            (YAW_TEST,"YAW_TEST")\
            (YAW_TEST1,"YAW_TEST1")\
            (PATH_DIR,"PATH_DIR")\
            (PATH_DIR1,"PATH_DIR1")\
            (HIT_BALL,"HIT_BALL")\
            (PASS_GATE,"PASS_GATE")\
            (DROP_MARK,"DROP_MARK")\
            (DROP_MARK1,"DROP_MARK1")\
            (DROP_MARK_SIMP,"DROP_MARK_SIMP")\
            (OPEN_BIN,"OPEN_BIN")\
            (GATE_ACT,"GATE_ACT")\
            (DROP_IT,"DROP_IT")\
            (TURN,"TURN")\
            (ACTION_TEST,"ACTION_TEST")\
            (SWING_SBG,"SWING_SBG")\
            (PreTask,"PreTask")\
            (Gate,"Gate")\
            (Gate2,"Gate2")\
            (Gate3,"Gate3")\
            (Gate4,"Gate4")\
            (Flare,"Flare")\
            (Drop,"Drop")\
            (Acquire,"Acquire")\
            (FORWARD_SBG,"FORWARD_SBG")\
            (BACKWARD_SBG,"BACKWARD_SBG")\
            (ROTATE_SBG,"ROTATE_SBG")\
            (HANG_SBG,"HANG_SBG")\
            (GET_PHOTOB,"GET_PHOTOB")\
            (GET_PHOTOF,"GET_PHOTOF");
}

void configManager::setPIDBasePath(const char *path)
{
    PIDBasePath.clear();
    if(nullptr!=path && NULL!=path){
        PIDBasePath.assign(path);
    }
}

void configManager::loadPID()
{
    vector<string> && nameList=getAllInfo(PIDBasePath.c_str(),"(.)(cfg)");
    fstream fin;
    string tStr;
    vector<string> tSplit;
    for(string & p: nameList){
        shared_ptr<PIDFile> tFile=shared_ptr<PIDFile>(new PIDFile);
        fin.close();
        fin.open(PIDBasePath+"/"+p,std::ios_base::in);
        if(!fin.is_open()){
            qDebug() << "Cannot open file "<< PIDBasePath.c_str() << "/" << p.c_str() << ". Ignored\n";
            continue;
        }
        tFile->fileName=p;

        skipInvalidLines(fin,tStr);
        if(tStr!=PIDCfgHeader || !fin){
            qDebug() << "The file " << PIDBasePath.c_str() << "/" << p.c_str() << " is invalid. Ignored.\n";
            continue;
        }

        skipInvalidLines(fin,tStr);
        if(!istarts_with(tStr,configNameStr) || !fin){
            qDebug() << "The file " << PIDBasePath.c_str() << "/" << p.c_str() << " is invalid. Ignored.\n";
            continue;
        }
        ierase_first(tStr,configNameStr);
        trim(tStr);
        qDebug() << "Read config name " << tStr.c_str() << ".\n";
        if(cfgNameList.find(tStr)!=cfgNameList.end()){
            qDebug() << "The config name " << tStr.c_str() << " has been used. Ignored.\n";
            continue;
        }

        tFile->cfgName=tStr;

        skipInvalidLines(fin,tStr);
        if(!istarts_with(tStr,configIdStr) || !fin){
            qDebug() << "The file " << PIDBasePath.c_str() << "/" << p.c_str() << " is invalid. Ignored.\n";
            continue;
        }

        ierase_first(tStr,configIdStr);
        trim(tStr);
        split(tSplit,tStr,isspace,token_compress_on);
        {
            bool faultFlag=true;
            for(auto & st:tSplit){
                qDebug() << "Read config id " << st.c_str() << ".\n";
                auto tPtr=cfgIDList.right.find(st);
                if(tPtr==cfgIDList.right.end()){
                    faultFlag=true;
                    qDebug() << "The id " << st.c_str() << " is invalid. This file is ignored.\n";
                    break;
                }

                faultFlag=false;
                tFile->cfgList.push_back(shared_ptr<PIDCfg>(new PIDCfg({tFile,tPtr->second,{}})));
            }
            if(faultFlag){
                continue;
            }
        }

        bool isEmptyFlag=true;
        auto curCfg=tFile->cfgList.begin();
        while(fin){
            skipInvalidLines(fin,tStr);

            if(!fin){
                qDebug() << "The file " << PIDBasePath.c_str() << "/" << p.c_str() << " terminated.\n";
                break;
            }

            if(istarts_with(tStr,configSwitch)){
                curCfg++;
                if(tFile->cfgList.end()==curCfg){
                    break;
                }
                continue;
            }


            split(tSplit,tStr,isspace,token_compress_on);
            auto tPtr=motorList.right.find(*(tSplit.begin()));
            if(tPtr==motorList.right.end()){
                qDebug() << "The motor " << tSplit.begin()->c_str() << " is invalid. This line is ignored.\n";
                continue;
            }

            isEmptyFlag=false;

            PIDValue tVal;
            auto pos=tSplit.begin();
            tVal.motorID=tPtr->second;
            if(++pos!=tSplit.end()){
                tVal.pValue=atof(pos->c_str());
            }
            else {
                tVal.pValue=0;
            }
            if(++pos!=tSplit.end()){
                tVal.iValue=atof(pos->c_str());
            }
            else {
                tVal.iValue=0;
            }
            if(++pos!=tSplit.end()){
                tVal.dValue=atof(pos->c_str());
            }
            else {
                tVal.dValue=0;
            }

            (*curCfg)->value.push_back(tVal);
        }
        //清理无效配置
        for(auto p=tFile->cfgList.rbegin();p!=tFile->cfgList.rend();p++){
            if((*p)->value.empty()){
                qDebug() << "The config " << cfgIDList.left.find((*p)->id)->second.c_str() << " in file " << tFile->fileName.c_str() << " is empty. Cleaned it.";
                *p=*(tFile->cfgList.rbegin());
                tFile->cfgList.pop_back();
            }
        }
        if(!isEmptyFlag && !(tFile->cfgList.empty())){
            cfgNameList.insert(make_pair<>(tFile->cfgName,tFile));
            for(auto & t:tFile->cfgList){
                PIDList[t->id].push_back(t);
            }
        }
    }

    fin.close();

    for(auto & t: cfgIDList.left){
        auto tPtr=PIDList.find(t.first);
        if(tPtr!=PIDList.end() && !(tPtr->second.empty())){
            currentPID.insert(make_pair<>(t.first,tPtr->second[0]));
        }
    }
}

void configManager::writePID(const std::__cxx11::string &cfgName)
{
    shared_ptr<PIDFile> tFile=cfgNameList.find(cfgName)->second;

    fstream fout;
    fout.open(PIDBasePath+"/"+tFile->fileName,std::ios_base::out|std::ios_base::trunc);
    if(!fout.is_open()){
        qDebug() << "Cannot open file "<< PIDBasePath.c_str() << "/" << tFile->fileName.c_str() << ". Maybe there are some permission issues.\n";
        //发射失败信号
        return;
    }

    fout << "# Auto generated pid parameter for NWPU auv.\n";
    fout << "# The file header:\n";
    fout << PIDCfgHeader << "\n";
    fout << "# The config name must be unique.\n";
    fout << configNameStr << " " << tFile->cfgName << "\n";
    fout << "# The config IDs this file includes:\n";
    fout << configIdStr;
    for (pPIDCfg & t:tFile->cfgList){
        fout << " " << cfgIDList.left.find(t->id)->second;
    }
    fout << "\n";
    fout << "# Every config ID corresponds to three floats, ordered as P, I, D.\n";
    fout << "# Every line begins with the thruster name.\n";
    for(pPIDCfg & t:tFile->cfgList){\
        fout << "# " << cfgIDList.left.find(t->id)->second << "\n";
        for (PIDValue & tval:t->value){
            fout << motorList.left.find(tval.motorID)->second << " ";
            fout << tval.pValue << " ";
            fout << tval.iValue << " ";
            fout << tval.dValue << "\n";
        }
        fout << configSwitch;
    }

    fout.close();
}

void configManager::setPIDValue(const std::__cxx11::string &cfgName, CONFIG_ID id, const vector<configManager::PIDValue> &val)
{
    auto tPtr=cfgNameList.find(cfgName);
    if(tPtr==cfgNameList.end()){
        return;
    }

    for(auto & t:tPtr->second->cfgList){
        if(t->id==id){
            t->value=val;
            break;
        }
    }
}

void configManager::setCurPID(CONFIG_ID id, configManager::pPIDCfg cfg)
{
    if(cfg.get()!=nullptr && cfg->id == id){
        currentPID[id]=cfg;
    }
}

void configManager::setTaskBasePath(const char *path)
{
    taskBasePath.clear();
    if(nullptr!=path && NULL!=path){
        taskBasePath.assign(path);
    }
}

void configManager::loadTask()
{
    vector<string> && nameList=getAllInfo(taskBasePath.c_str(),"(.)(pck)");
    fstream fin;
    string tStr;
    vector<string> tSplit;

    for(string & p: nameList){
        taskPack tFile;
        fin.close();
        fin.open(taskBasePath+"/"+p,std::ios_base::in);
        if(!fin.is_open()){
            qDebug() << "Cannot open file "<< taskBasePath.c_str() << "/" << p.c_str() << ". Ignored\n";
            continue;
        }
        tFile.fileName=p;

        skipInvalidLines(fin,tStr);
        if(tStr!=packHeader || !fin){
            qDebug() << "The file " << taskBasePath.c_str() << "/" << p.c_str() << " is invalid. Ignored.\n";
            continue;
        }

        skipInvalidLines(fin,tStr);
        if(!istarts_with(tStr,packNameStr) || !fin){
            qDebug() << "The file " << taskBasePath.c_str() << "/" << p.c_str() << " is invalid. Ignored.\n";
            continue;
        }
        ierase_first(tStr,packNameStr);
        trim(tStr);
        qDebug() << "Read task pack name " << tStr.c_str() << ".\n";
        if(taskPackList.find(tStr)!=taskPackList.end()){
            qDebug() << "The task pack name " << tStr.c_str() << " has been used. Ignored.\n";
            continue;
        }
        tFile.packName=tStr;

        while(fin){
            skipInvalidLines(fin,tStr);
            if(!fin){
                qDebug() << "The file " << taskBasePath.c_str() << "/" << p.c_str() << " terminated.\n";
                break;
            }

            auto tPtr=taskIDList.right.find(tStr);
            if(tPtr==taskIDList.right.end()){
                qDebug() << "The task id " << tStr.c_str() << " is invalid. This line is ignored.\n";
                continue;
            }

            tFile.taskList.push_back(tPtr->second);
        }

        if(tFile.taskList.empty()){
            qDebug() << "The task pack in file " << tFile.fileName.c_str() << " is empty. Ignored it.";
            continue;
        }

        taskPackList.insert(make_pair<>(tFile.packName,tFile));
    }
    fin.close();
}

vector<QString> configManager::getPackList() const
{
    vector<QString> temp;
    for(auto & t:taskPackList){
        temp.push_back(QString::fromStdString(t.second.packName));
    }
    return temp;
}

vector<TASKS_ID> configManager::getTaskIDList(const QString &packName) const
{
    vector<TASKS_ID> temp;
    auto t=taskPackList.find(packName.toStdString());
    if(t!=taskPackList.end()){
        return t->second.taskList;
    }
    else {
        return temp;
    }
}

vector<QString> configManager::getTaskStrList(const QString &packName) const
{
    vector<QString> temp;
    auto t=taskPackList.find(packName.toStdString());
    if(t!=taskPackList.end()){
        for(auto &st:t->second.taskList){
            temp.push_back(QString::fromStdString((taskIDList.left.find(st)->second)));
        }
    }

    return temp;
}

vector<std::__cxx11::string> configManager::getAllInfo(const char *path, const char *postfix, const char *prefix)
{
    vector<string> strList;
    if(nullptr==path || NULL==path){
        return strList;
    }
    DIR * dir=nullptr;
    dirent64 * p=nullptr;
    string regStr,temp;
    regStr.assign("(.*)");
    if(prefix!=nullptr && prefix!=NULL){
        regStr.assign("^"+(prefix+regStr));
    }
    if(postfix!=nullptr && postfix!=NULL){
        regStr.append(postfix).append("$");
    }
    regex reg(regStr);

    dir=opendir(path);
    while((p=readdir64(dir))!=NULL){
        if('.'==p->d_name[0]){
            continue;
        }

        temp.assign(p->d_name);
        if(regex_match(temp,reg)){
            strList.push_back(temp);
        }
    }
    return strList;
}

void configManager::skipInvalidLines(std::fstream &fin, std::__cxx11::string &str)
{
    getline(fin,str);
    trim(str);
    while(fin && ( str.empty() || *(str.begin())=='\0' || *(str.begin())=='#')){
        getline(fin,str);
        trim(str);
    }
}
