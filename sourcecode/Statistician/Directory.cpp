#include "../Utility/Include.h"
#include "Directory.h"
using namespace std;
Directory* Directory::m_pDir = nullptr;
char Directory::cOutputPath[260];

Directory& Directory::Instance() {
    if (!m_pDir) {
        m_pDir = new Directory;
    }
    return *m_pDir;
}

boost::filesystem::path Directory::GetPath(const std::string& _filename) {
    return m_PrePath / _filename;
}

void Directory::SetOutputPath(char* _casename[]) {
    strcpy(cOutputPath, _casename[1]);
    cout << "当前输出目录为" << cOutputPath << endl;

}

void Directory::Initialize() {
    string sDir = "./outputfiles";
    time_t timep;
    struct tm *p;
    time(&timep);
    p = localtime(&timep);
    string sYear = boost::lexical_cast<string > (1900 + p->tm_year);
    string sMonth = boost::lexical_cast<string > (1 + p->tm_mon);
    string sDay = boost::lexical_cast<string > (p->tm_mday);
    string sHour = boost::lexical_cast<string > (p->tm_hour);
    string sMin = boost::lexical_cast<string > (p->tm_min);
    string sSec = boost::lexical_cast<string > (p->tm_sec);
    string sCaseName = boost::lexical_cast<string > (cOutputPath);
    //    char* m_SuDdir;
    string sOutPut;
    if (!sCaseName.empty()) {
        //        m_SuDdir = const_cast<char*> ((sCaseName + "_" + sYear + "." + sMonth + "." + sDay + "@" + sHour + "." + sMin + "." + sSec).c_str());
        sOutPut = sCaseName ;//+ "_" + sYear + "." + sMonth + "." + sDay + "@" + sHour + "." + sMin + "." + sSec;
    } else {
        //        m_SuDdir = const_cast<char*> ((sYear + "." + sMonth + "." + sDay + "@" + sHour + "." + sMin + "." + sSec).c_str());
        sOutPut = sYear + "." + sMonth + "." + sDay + "@" + sHour + "." + sMin + "." + sSec;
    }
    //    string sOutPut = boost::lexical_cast<string > (m_SuDdir);
    boost::filesystem::path DirPath = boost::filesystem::path(sDir);
    boost::filesystem::create_directories(DirPath / sOutPut);
    m_PrePath = boost::filesystem::path(sDir + "/" + sOutPut + "/");
}