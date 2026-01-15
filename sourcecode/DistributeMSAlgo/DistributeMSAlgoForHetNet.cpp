///@zxy picoout场景下MS的分布函数定义

#include "../Utility/Include.h"
#include "../BaseStation/BTSID.h"
#include "../Utility/RBID.h"                     ///@xlong
#include "../MobileStation/MSID.h"
#include "../Scheduler/SchedulerDL.h"
#include "../Scheduler/SchedulerUL.h"
#include "../DetectAlgorithm/Detector_DL.h"
#include "../MobileStation/ACKNAKMessageDL.h"
#include "../BaseStation/BTS.h"
#include "../BaseStation/BS.h"
#include "../Scheduler/SchedulingMessageUL.h"
#include "../MobileStation/MS.h"
#include "../NetworkDrive/MSManager.h"
#include "../NetworkDrive/BSManager.h"
#include "DistributeMSAlgoForHetNet.h"

void DistributeMSAlgoForHetNet::DistributeMS(MSManager& _msm) {
    ///@zxy 用于获取各种场景下的相关数据
    switch(Parameters::Instance().SmallCell.LINK.IHetnetEnvironmentType) {
        case Parameters::IHetnet_Normal: ///第一种场景下MS的分布与pico无关，不对上面两个变量进行赋值
            ///xlong:采用大菱形撒点
            DistributeMSNormalHexagon(_msm);    ///@xlong   20110902   校正
            break;
        case Parameters::IHetnet_4b:
            DistributeMS4b(_msm);
            break;
        case Parameters::IHetnet_Cluster:
            DistributeMS_Cluster(_msm);
            break;
        default:
            assert(false);
            break;
    }
}

///@xlong normal场景下的MS撒点,在所有区域内均匀撒点，因此采用大菱形撒点，保证大范围的随机均匀性
void DistributeMSAlgoForHetNet::DistributeMSNormal(MSManager& _msm) {
    ///@xlong：将撒点模式改成大菱形撒点
    double dDis = Parameters::Instance().Macro.DSiteDistanceM;
    double dRadus = 0;
    double dorient = 0;
    double dx = 0 , dy = 0;
    double dOffsetx = 0;
    double dOffsety = 0;
    switch(Parameters::Instance().BASIC.INumBSs){
        case 7:
            dRadus = sqrt(7) * dDis;
            dorient =  atan(M_SQRT3 / 5) - (2.0 / 3) * pi;
            dOffsetx = 0 ;
            dOffsety = 0;
            break;
        case 19:
            dRadus = sqrt(19) * dDis;
            dorient =  atan(3 * M_SQRT3 / 7) - (2.0 / 3) * pi;
            dOffsetx = 0;
            dOffsety = 0;
            break;
        default:
            break;
    }
    for (int iMsNum = 0; iMsNum < Parameters::Instance().BASIC.ITotalMSNum ; ++iMsNum) {
        int count = 0;      bool flag = true;
        while(count < 100 && flag) {
            ++ count;       flag = false;
            double rx = random.xUniform_distributems(-1, 0);
            double ry = random.xUniform_distributems(0, 1);
            double ytemp = dRadus * 0.5 * M_SQRT3 * ry;
            double xtemp = dRadus * rx + ytemp / M_SQRT3;
            dx = xtemp * cos(dorient) - ytemp * sin(dorient) + dOffsetx;
            dy = ytemp * cos(dorient) + xtemp * sin(dorient) + dOffsety;

            ///@xlong   计算与各类基站(BTS以及Pico)之间的距离
            for(BTSID btsid = BTSID::Begin(); btsid <= BTSID::End(); ++btsid) {
                BTS& bts = btsid.GetBTS();
                ///@xlong   根据基站类型选择最小限制距离
                double dMinDist = btsid.GetIndex() < Parameters::Instance().BASIC.IBTSPerBS ? Parameters::Instance().Macro.LINK.DMinDistanceM : Parameters::Instance().SmallCell.LINK.DMinDistPico2UEM;
                double ddx = dx - bts.GetX();
                double ddy = dy - bts.GetY();
                double dist = sqrt(ddx * ddx + ddy * ddy);

                if (dist < dMinDist) {
                    flag = true;
                    break;
                }
            }
        }
        _msm.GetMS(iMsNum).SetXY(dx, dy);
        Observer::Print("MS_Position")<<_msm.GetMS(iMsNum).GetID()<<setw(20)<<dx<<setw(20)<<dy<<endl;
    }
}

///@xlong 4b场景下MS的撒点
void DistributeMSAlgoForHetNet::DistributeMS4b(MSManager& _msm) {
    ///@xlong 获取4b撒点时需要的参数,每个BTS内Pico数量，以及pico内撒点概率
    int iMSID = 0;///记录已撒的MS编号
    for(int i = 0; i < Parameters::Instance().BASIC.INumBSs ; ++ i) {
        for (int k = 0 ; k < Parameters::Instance().BASIC.IBTSPerBS ; ++ k) {
            ///@xlong 获取该BTS中MS的个数，为30，4b场景用户数为30 MSPerBTS
            int iMSNumInPicoZone = floor(Parameters::Instance().SmallCell.LINK.IPicoUENumPerCell4b * Parameters::Instance().SmallCell.LINK.DUEProbInPicoZone4b );
            ///@zxy 遍历一个BTS下的所有pico区域
            ///@zxy 计算每个PicoOut区域中应该撒的MS个数
            int iMSNumPerPicoZone = (int)(iMSNumInPicoZone / Parameters::Instance().SmallCell.LINK.IPicoNumPerMacro4b );
            for (int j = 0; j < Parameters::Instance().SmallCell.LINK.IPicoNumPerMacro4b ; ++j) {
                ///@zxy     计算PicoID
                int ipicoid =  Parameters::Instance().BASIC.IBTSPerBS + k * Parameters::Instance().SmallCell.LINK.IPicoNumPerMacro4b + j ;
                for(int iMSIndex = 0; iMSIndex < iMSNumPerPicoZone ; ++ iMSIndex, ++ iMSID) {
                    MS& ms = _msm.GetMS(iMSID);                                        
                    DistributeMSInPicoZone( ms , i , ipicoid );
                }
            }

            ///pico区域中的MS撒点完毕，将不属于pico区域中的MS撒入BTS中
            for(int index = iMSNumInPicoZone ; index < Parameters::Instance().SmallCell.LINK.IPicoUENumPerCell4b ; ++index,++iMSID) {
                MSID msid = MSID(iMSID);
                MS& ms = msid.GetMS();
                DistributeMSOutPicoZone( ms , i , k );
            }
        }
    }
}

///@xlong 4b场景下pico区域内部的MS的撒点,圆形撒点
void DistributeMSAlgoForHetNet::DistributeMSInPicoZone(MS& ms, int iBSID, int iBTSID ) {
    BTSID btsid  = BTSID(iBSID, iBTSID);
    BTS& bts = btsid.GetBTS();

    int counter = 0;    bool flag = true;
    do {
        flag = false;counter ++;
        ///采用圆形撒点，随机产生一个半径和相角
        double dRadiusTemp = sqrt(random.xUniform_distributems(pow(Parameters::Instance().SmallCell.LINK.DMinDistPico2UEM , 2),pow(Parameters::Instance().SmallCell.LINK.DPicoRadiusM , 2 )));
        double orient = random.xUniform_distributems(0,360)/ 180 * M_PI;
        ///转化为直角坐标
        double xtemp = dRadiusTemp * cos(orient);
        double ytemp = dRadiusTemp * sin(orient);
        double x = bts.GetX() + xtemp;
        double y = bts.GetY() + ytemp;
        ms.SetXY(x,y);

        ///计算新撒的MS节点与系统中所有Pico的最小间距
        for (int i = 0 ; i < Parameters::Instance().BASIC.INumBSs ; ++i) {
            for (int j = 0 ; j < Parameters::Instance().BASIC.IBTSPerBS * ( 1 + Parameters::Instance().SmallCell.LINK.IPicoNumPerMacro4b ); ++j) {
                BTSID btsid1 = BTSID(i,j);
                BTS& bts1 = btsid1.GetBTS();
                double dx = bts1.GetX() - x;
                double dy = bts1.GetY() - y;
                double distance = sqrt(dx * dx + dy * dy);
                if (j < Parameters::Instance().BASIC.IBTSPerBS) {
                    if (distance < Parameters::Instance().Macro.LINK.DMinDistanceM) {
                        flag = true;
                        break;
                    }
                } else {
                    if (distance < Parameters::Instance().SmallCell.LINK.DMinDistPico2UEM) {
                        flag = true;
                        break;
                    }
                }
            }
            if(flag) {
                break;
            }
        }
    }while(flag && (counter < 100));
    Observer::Print("MS_in_PicoZone ")<<ms.GetID()<<setw(20)<<ms.GetX()<<setw(20)<<ms.GetY()<<endl;             
}

///@xlong      4b场景下pico区域外部的MS的撒点
void DistributeMSAlgoForHetNet::DistributeMSOutPicoZone(MS& ms, int iBSID, int iBTSID ) {
    BTSID btsid = BTSID(iBSID,iBTSID);
    BTS& bts = btsid.GetBTS();

    int counter = 0;    bool flag = true;
    do {
        flag = false;counter ++;
        ///将指定的MS在指定的BTS下进行六边形撒点
        double x = 0, y = 0;
        double orient = 0;
        double dx = 0, dy = 0;
        double p0 = 1.0/3.0;
        double dRadius = Parameters::Instance().Macro.DCellRadiusM;
        double p1 = random.xUniform_distributems(0,1);
        
        if(p1 < p0) {
            x = random.xUniform_distributems(0 , 0.5 * dRadius);
            y = random.xUniform_distributems(- 0.5 * M_SQRT3 * dRadius , 0.5 * M_SQRT3 * dRadius);
            if(abs(y) > M_SQRT3 * x) {
                x += 1.5 * dRadius;
                y += (y > 0 ? -1: 1) * 0.5 * M_SQRT3 * dRadius;
            }
        }
        else {
            x = random.xUniform_distributems(0.5 * dRadius , 1.5 * dRadius);
            y = random.xUniform_distributems(-0.5 * M_SQRT3 * dRadius , 0.5 * M_SQRT3 * dRadius);
        }

        ///3扇区和全向天线不同
        if (Parameters::Instance().BASIC.IBTSPerBS == 1) {
            orient = floor(random.xUniform_distributems(0, 1) * 3) * 2 * M_PI / 3;
        }///3扇区天线
        else if (Parameters::Instance().BASIC.IBTSPerBS == 3) {
            orient = bts.GetTxOrientRAD();
        }
        dx = x * cos(orient) - y * sin(orient);
        dy = y * cos(orient) + x * sin(orient);

        x = bts.GetX() + dx ;
        y = bts.GetY() + dy ;
        ms.SetXY(x,y);

        ////@xlong 新撒的节点还需要满足在所有Pico区域之外，即与所有Pico的距离不小于40m的条件以及与所有BTS的距离不小于35m的条件
        for (int i = 0; i < Parameters::Instance().BASIC.INumBSs; ++i) {
            ////@xlong 同时考虑真正BTS以及PICO的节点
            for (int j = 0 ; j < Parameters::Instance().BASIC.IBTSPerBS * ( 1 + Parameters::Instance().SmallCell.LINK.IPicoNumPerMacro4b ); ++j) {
                BTSID _btsid = BTSID(i, j);
                BTS& _bts = _btsid.GetBTS();
                double _dx = _bts.GetX() - x;
                double _dy = _bts.GetY() - y;
                double distance = sqrt(_dx * _dx + _dy * _dy);
                if ( j > Parameters::Instance().BASIC.IBTSPerBS -1) {///@xlong   如果该节点是被当作BTS的pico节点
                    if (distance < Parameters::Instance().SmallCell.LINK.DMinDistPico2UEM ) {         ///@xlong   P.Macro.DPicoOutRadiusM(其余用户应该在BTS范围内（包括Pico的圆形区域）均匀撒点，但是得满足最小距离要求)
                        flag = true;
                        break;
                    }
                }
                else {
                    if (distance < Parameters::Instance().Macro.LINK.DMinDistanceM) {
                        flag = true;
                        break;
                    }
                }
            }
            if(flag) {///@xlong     为了跳出外层循环
                break;
            }
        }

    }while(flag && (counter < 100));
    Observer::Print("MS_outof_PicoOutZone ")<<ms.GetID()<<setw(20)<<ms.GetX()<<setw(20)<<ms.GetY()<<endl;
}

///@zxy 单个MS撒点（六边形撒点，将特定的MS撒在特定的BTS下）
void DistributeMSAlgoForHetNet::DistributeMS(MS& _ms, int ibsid, int ibtsid,int iPicoOutNumPerBTS) {
    BTSID btsid = BTSID(ibsid,ibtsid);
    BTS& bts = btsid.GetBTS();

    int counter = 0;    bool flag = true;
    do {
        flag = false;counter ++;
        ///将指定的MS在指定的BTS下进行六边形撒点
        double x = 0, y = 0;
        double orient = 0;
        double dx = 0, dy = 0;
        double p0 = 1.0/3.0;
        double dRadius = Parameters::Instance().Macro.DCellRadiusM;
        double p1 = random.xUniform_distributems(0,1);
        
        if(p1 < p0) {
            x = random.xUniform_distributems(0 , 0.5 * dRadius);
            y = random.xUniform_distributems(- 0.5 * M_SQRT3 * dRadius , 0.5 * M_SQRT3 * dRadius);
            if(abs(y) > M_SQRT3 * x) {
                x += 1.5 * dRadius;
                y += (y > 0 ? -1: 1) * 0.5 * M_SQRT3 * dRadius;
            }
        }
        else {
            x = random.xUniform_distributems(0.5 * dRadius , 1.5 * dRadius);
            y = random.xUniform_distributems(-0.5 * M_SQRT3 * dRadius , 0.5 * M_SQRT3 * dRadius);
        }

        ///3扇区和全向天线不同
        if (Parameters::Instance().BASIC.IBTSPerBS == 1) {
            orient = floor(random.xUniform_distributems(0, 1) * 3) * 2 * M_PI / 3;
        }///3扇区天线
        else if (Parameters::Instance().BASIC.IBTSPerBS == 3) {
            //orient = floor(xUniform_distributems(0, 1) * 3) * 2 * M_PI / 3;
            orient = bts.GetTxOrientRAD();
        }
        dx = x * cos(orient) - y * sin(orient);
        dy = y * cos(orient) + x * sin(orient);

        x = bts.GetX() + dx ;
        y = bts.GetY() + dy ;
        _ms.SetXY(x, y);

        ////@zxy 新撒的节点还需要满足与所有PicoOut的距离不小于10m的条件以及与所有BTS的距离不小于35m的条件
        for (int i = 0; i < Parameters::Instance().BASIC.INumBSs; ++i) {
            ////@zxy 同时考虑真正的和被当作BTS的节点
            for (int j = 0 ; j < Parameters::Instance().BASIC.IBTSPerBS * ( 1 + iPicoOutNumPerBTS ); ++j) {
                BTSID btsid = BTSID(i, j);
                BTS& bts = btsid.GetBTS();
                double dx = bts.GetX() - x;
                double dy = bts.GetY() - y;
                double distance = sqrt(dx * dx + dy *dy);
                if (j > Parameters::Instance().BASIC.IBTSPerBS -1) {///@zxy 如果该BTS是被当作BTS的pico节点
                    if (distance < Parameters::Instance().SmallCell.LINK.DMinDistPico2UEM) {
                        flag = true;
                        break;
                    }
                }
                else {
                    if (distance < Parameters::Instance().Macro.LINK.DMinDistanceM) {
                        flag = true;
                        break;
                    }
                }
            }
            if(flag) {///@zxy 为了跳出外层循环
                break;
            }
        }

    }while(flag && (counter < 100));
    Observer::Print("MSID ")<<_ms.GetID()<<setw(20)<<_ms.GetX()<<setw(20)<<_ms.GetY()<<endl;          
}

///zxy:单个的MS撒点也改成大菱形撒点
void DistributeMSAlgoForHetNet::DistributeMS(MS& _ms) {
    double dDis = Parameters::Instance().Macro.DSiteDistanceM;
    double dRadus = 0;
    double dorient = 0;
    double dx = 0 , dy = 0;
    double dOffsetx = 0;
    double dOffsety = 0;
    switch(Parameters::Instance().BASIC.INumBSs){
        case 7:
            dRadus = sqrt(7) * dDis;
            dorient =  atan(M_SQRT3 / 5) - (2.0 / 3) * pi;
            dOffsetx = 0 ;
            dOffsety = 0;
            break;
        case 19:
            dRadus = sqrt(19) * dDis;
             dorient =  atan(3 * M_SQRT3 / 7) - (2.0 / 3) * pi;
            dOffsetx = 0 ;
            dOffsety = 0;
            break;
        default:
            break;
    }

    int count = 0;bool flag = true;
    while(count < 100 && flag) {
        ++ count; flag = false;

        double rx = random.xUniform_distributems(-1, 0);
        double ry = random.xUniform_distributems(0, 1);
        double ytemp = dRadus * 0.5 * M_SQRT3 * ry;
        double xtemp = dRadus * rx + ytemp / M_SQRT3;
        dx = xtemp * cos(dorient) - ytemp * sin(dorient) + dOffsetx;
        dy = ytemp * cos(dorient) + xtemp * sin(dorient) + dOffsety;

        for(BTSID btsid = BTSID::Begin(); btsid <= BTSID::End(); ++btsid) {
            BTS& bts = btsid.GetBTS();
            ///zxy:根据基站类型选择最小限制距离
            double dMinDist = btsid.GetIndex() < Parameters::Instance().BASIC.IBTSPerBS ? Parameters::Instance().Macro.LINK.DMinDistanceM : Parameters::Instance().SmallCell.LINK.DMinDistPico2UEM;
            double ddx = dx - bts.GetX();
            double ddy = dy - bts.GetY();
            double dist = sqrt(ddx * ddx + ddy * ddy);

            if (dist < dMinDist) {
                flag = true;
                break;
            }
        }
    }

    _ms.SetXY(dx, dy);

    Observer::Print("MS_Position")<<_ms.GetID()<<setw(20)<<dx<<setw(20)<<dy<<endl;
}

void DistributeMSAlgoForHetNet::DistributeMSNormalHexagon(MSManager& _msm) {
     int iAllMS = Parameters::Instance().BASIC.ITotalMSNum;
    ///采用源平台中随机选择小区进行六边形撒点的方式
    for (int ims = 0; ims < iAllMS; ++ ims) {
        DistributeMSHexagon(_msm.GetMS(ims));
    }
}

///单个MS的六边形撒点不作修改，只增加对各基站节点的距离限制条件
void DistributeMSAlgoForHetNet::DistributeMSHexagon(MS& _ms) {
    ///计算BS编号
    int iBs = _ms.GetID().ToInt() / (Parameters::Instance().SmallCell.LINK.IPicoUENumPerCellNormal* Parameters::Instance().BASIC.IBTSPerBS);
    ///计算BTS编号
    int iBts = (_ms.GetID().ToInt() % (Parameters::Instance().SmallCell.LINK.IPicoUENumPerCellNormal * Parameters::Instance().BASIC.IBTSPerBS)) / Parameters::Instance().SmallCell.LINK.IPicoUENumPerCellNormal;
    BTSID btsid = BTSID(iBs, iBts);
    BTS& bts = btsid.GetBTS();

    int count = 0 ;bool flag = true;
    double dRadius = Parameters::Instance().Macro.DCellRadiusM;
    double p = 1/3.0;
    double x = 0, y = 0;
    while(count < 100 && flag) {
        ++ count ;
        flag = false;

        double orient = 0;
        double dx = 0, dy = 0;
        double xtemp  = 0, ytemp = 0;

        double temp = random.xUniform_distributems(0,1);
        if (temp < p) {
            dx = random.xUniform_distributems(0 , 0.5 * dRadius);
            dy = random.xUniform_distributems(-0.5 * sqrt(3) * dRadius , 0.5 * sqrt(3) * dRadius);
            if (abs(dy) > sqrt(3) * dx) {
                dx += 1.5 * dRadius;
                dy += (dy > 0 ? -1 : 1) * 0.5 * sqrt(3) * dRadius;
            }
        }
        else {
            dx = random.xUniform_distributems(0.5 * dRadius , 1.5 * dRadius);
            dy = random.xUniform_distributems(-0.5 * sqrt(3) * dRadius , 0.5 * sqrt(3) * dRadius);
        }

        ///3扇区和全向天线不同
        if (Parameters::Instance().BASIC.IBTSPerBS == 1) {
            orient = floor(random.xUniform_distributems(0, 1) * 3) * 2 * M_PI / 3;
        }///3扇区天线
        else if (Parameters::Instance().BASIC.IBTSPerBS == 3) {
            orient = bts.GetTxOrientRAD();
        }
        xtemp = dx * cos(orient) - dy * sin(orient);
        ytemp = dy * cos(orient) + dx * sin(orient);

        x = bts.GetX() + xtemp;
        y = bts.GetY() + ytemp;

        //计算与各类节点之间的距离限制
        for (int i = 0; i < Parameters::Instance().BASIC.INumBSs; ++ i ) {
            for (int j = 0; j < Parameters::Instance().BASIC.IBTSPerBS * (1 + Parameters::Instance().SmallCell.LINK.IPicoNumPerMacroNormal); ++ j) {
                double  DistLimit = (j < Parameters::Instance().BASIC.IBTSPerBS) ? Parameters::Instance().Macro.LINK.DMinDistanceM : Parameters::Instance().SmallCell.LINK.DMinDistPico2UEM;
                BTSID btsid = BTSID(i , j);
                BTS& bts = btsid.GetBTS();
                double d1 =  x - bts.GetX();
                double d2 =  y - bts.GetY();
                double dist = sqrt(d1 * d1 + d2 * d2);
                if (dist < DistLimit) {
                    flag = true;
                    break;
                }
            }
            if (flag) {
                break;
            }
        }
    }
    _ms.SetXY(x, y);
    Observer::Print("MSPosition_Case1")<<_ms.GetID()<<setw(20)<<_ms.GetX()<<setw(20)<<_ms.GetY()<<endl;
}


void DistributeMSAlgoForHetNet::DistributeMS_Cluster(MSManager& _msm) {
    int iMSID = 0;///记录已撒的MS编号
    int iSiteNum = Parameters::Instance().BASIC.INumBSs;
    int iMacroNumPerSite = Parameters::Instance().BASIC.IBTSPerBS;
    int iClusterNumPerMacro = Parameters::Instance().SmallCell.LINK.IPicoClusterNumPerMacro_Cluster;
    int iPicoNumPerCluster = Parameters::Instance().SmallCell.LINK.IPicoNumPerCluster_Cluster;
    int iUENumPerCell = Parameters::Instance().SmallCell.LINK.IUENumPerCell_Cluster;
    double dMinDistPico2UE = Parameters::Instance().SmallCell.LINK.DMinDistPico2UEM;
    double dMinDistMacro2UE = Parameters::Instance().Macro.LINK.DMinDistanceM;
    double dUEProbInCluster = Parameters::Instance().SmallCell.LINK.DUEProbInCluster_Cluster;
    for(int i = 0; i < iSiteNum ; ++i) {
        for (int j = 0 ; j < iMacroNumPerSite; ++j) {
            MacroID macroid(i,j);
            vector<cm::Point> vClusterPos = BSManager::Instance().GetClusterPos(macroid);
            assert(static_cast<int>(vClusterPos.size()) == iClusterNumPerMacro);
            int iUENumInClustersPerCell = floor(iUENumPerCell * dUEProbInCluster);
            int iUENumInOneCluster = floor(iUENumInClustersPerCell/iClusterNumPerMacro);
            assert(iUENumInOneCluster * iClusterNumPerMacro == iUENumInClustersPerCell);
            for (int iUEindex = 0; iUEindex < iUENumInClustersPerCell; ++iUEindex, ++iMSID) {
                int iClusterIndex = iUEindex/iUENumInOneCluster;
                cm::Point clusterPos = vClusterPos[iClusterIndex];
                MS& ms = _msm.GetMS(iMSID);
                DistributeMSInCluster_Cluster(ms,macroid, clusterPos);
            }

            for(int iUEindex = iUENumInClustersPerCell ; iUEindex < iUENumPerCell; ++iUEindex,++iMSID) {
                MS& ms = _msm.GetMS(iMSID);
                DistributeMSInMacro_Cluster(ms,macroid);
            }
        }
    }
}

void DistributeMSAlgoForHetNet::DistributeMSInCluster_Cluster(MS& ms, const MacroID& _macroid, const cm::Point& _clusterPos) {
    double dMinDistClusterDropUERadius = Parameters::Instance().SmallCell.LINK.DMinDistClusterRadiusM_UEDrop;
    double dMinDistPico2UE = Parameters::Instance().SmallCell.LINK.DMinDistPico2UEM;
    double dMinDistMacro2UE = Parameters::Instance().Macro.LINK.DMinDistanceM;
    bool flag = true;
    do {
        flag = true;
        //撒UE
        double dx = 0;
        double dy = 0;
        do {
            dx = dMinDistClusterDropUERadius*random.xUniform_distributems(-1,1);
            dy = dMinDistClusterDropUERadius*random.xUniform_distributems(-1,1);
        }while(sqrt(pow(dx,2.0)+pow(dy,2.0))>=dMinDistClusterDropUERadius);

        double x = _clusterPos.GetX() + dx;
        double y = _clusterPos.GetY() + dy;
        cm::Point UEPos(x,y);
        ///判断是否满足距离要求
        for(BTSID btsid=BTSID::Begin(); btsid<=BTSID::End();++btsid) {
            if(BSManager::IsMacro(btsid)) {
                cm::Point WrapUEPos;
                WrapUEPos = cm::WrapAround::Instance().WrapRx(UEPos, btsid.GetBTS());
                if (cm::Distance(WrapUEPos, btsid.GetBTS()) < dMinDistMacro2UE) {
                    flag = false;
                    break;
                }
            }else{
                cm::Point WrapUEPos;
                WrapUEPos = cm::WrapAround::Instance().WrapRx(UEPos, BSManager::GetMacroIDofPico(btsid).GetBTS());
                double d = cm::Distance(WrapUEPos, btsid.GetBTS());
                if (d < dMinDistPico2UE) {
                    flag = false;
                    break;
                }
            }
        }

        if(flag) {
            ms.SetXY(x,y);
            Observer::Print("ClusterUEPos")<<x<<setw(20)<<y<<endl;
        }
    }while(!flag);
}

void DistributeMSAlgoForHetNet::DistributeMSInMacro_Cluster(MS& ms, const MacroID& _macroid) {
    double dMinDistPico2UE = Parameters::Instance().SmallCell.LINK.DMinDistPico2UEM;
    double dMinDistMacro2UE = Parameters::Instance().Macro.LINK.DMinDistanceM;
    double dRadus = Parameters::Instance().Macro.DCellRadiusM;
    bool flag = true;
    do {
        flag = true;
        //撒UE
        double dx = 0, dy = 0;
        double rx = random.xUniform_distributems(-1, 0);
        double ry = random.xUniform_distributems(0, 1);
        double ytemp = dRadus * 0.5 * sqrt(3) * ry;
        double xtemp = dRadus * rx + ytemp / M_SQRT3;
        double orient = floor(random.xUniform_distributems(0, 1) * 3) * 2 * M_PI / 3;
        double dxtemp = xtemp * cos(orient) - ytemp * sin(orient);
        double dytemp = ytemp * cos(orient) + xtemp * sin(orient);
        int index = _macroid.GetIndex();
        double doffset1 = 0;
        double doffset2 = 0;
        switch (index) {
            case 0:
                doffset1 = dRadus;
                break;
            case 1:
                doffset1 = -1 * dRadus / 2;
                doffset2 = dRadus * sqrt(3) / 2;
                break;
            case 2:
                doffset1 = -1 * dRadus / 2;
                doffset2 = -1 * dRadus * sqrt(3) / 2;
                break;
            default:
                break;
        }

//        double orient2 = index * 2 * M_PI / 3;
//        dx = dxtemp * cos(orient2) - dytemp * sin(orient2);
//        dy = dytemp * cos(orient2) + dxtemp * sin(orient2);
//        double x = _macroid.GetBTS().GetX() + dx + doffset1;
//        double y = _macroid.GetBTS().GetY() + dy + doffset2;
        double x = _macroid.GetBTS().GetX() + dxtemp + doffset1;
        double y = _macroid.GetBTS().GetY() + dytemp + doffset2;

        cm::Point UEPos(x,y);
        ///判断是否满足距离要求
        for(BTSID btsid=BTSID::Begin(); btsid<=BTSID::End();++btsid) {
            if(BSManager::IsMacro(btsid)) {
                cm::Point WrapUEPos;
                WrapUEPos = cm::WrapAround::Instance().WrapRx(UEPos, btsid.GetBTS());

                if (cm::Distance(WrapUEPos, btsid.GetBTS()) < dMinDistMacro2UE) {
                    flag = false;
                    break;
                }
            }else{
                cm::Point WrapUEPos;
                WrapUEPos = cm::WrapAround::Instance().WrapRx(UEPos, BSManager::GetMacroIDofPico(btsid).GetBTS());
                double d = cm::Distance(WrapUEPos, btsid.GetBTS());
                if (d < dMinDistPico2UE) {
                    flag = false;
                    break;
                }
            }
        }

        if(flag) {
            ms.SetXY(x,y);
            Observer::Print("MacroUEPos")<<x<<setw(20)<<y<<endl;
        }
    }while(!flag);
}