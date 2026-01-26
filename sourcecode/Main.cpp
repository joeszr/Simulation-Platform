#include "./NetworkDrive/NetWorkDrive.h"
#include "./Utility/Include.h"
#include "./Statistician/Directory.h"
#include "LinklevelInterface/LinkLevelInterface.h"
#include "./MobileStation/MSID.h"

int main(int argc, char* argv[])
{
    if (argc == 2) {
        Directory::Instance().SetOutputPath(argv);
        cout << "输出目录为:" << argv[1] << endl;
    } else if (argc == 1) {
        cout << "默认输出目录..." << endl;
    } else{
        return 0;
    }
    ////////系统初始化//////////////
    Directory::Instance().Initialize();
    Parameters::Instance();
    long int timestart = 0, timeend = 0;
    time(&timestart);
    ////////仿真运行///////////////
    NetWorkDrive::Instance().SimulateRun();

    time(&timeend);
    std::cout << "All Done" << endl << "The program run for " << timeend-timestart;

    return 0;

}

//success11


