///@file BSManager.cpp
///@brief  基站管理类函数实现
///@author wangxiaozhou
#include "../Utility/IDType.h"
#include "../Utility/RBID.h"
#include "../BaseStation/BTSID.h"
#include "../DetectAlgorithm/Detector_DL.h"
#include "../Scheduler/SchedulingMessageUL.h"
#include "../MobileStation/MS.h"
#include "../LinklevelInterface/LinkLevelInterface.h"
#include "../Scheduler/SchedulerDL.h"
#include "../Scheduler/SchedulerUL.h"
#include "../BaseStation/BS.h"
#include "../MobileStation/ACKNAKMessageDL.h"
#include "../BaseStation/BTS.h"
#include "BSManager.h"
#include "../ChannelModel/LinkMatrix.h"
#include <tuple>
#include <string>
using namespace std;

BSManager::BSManager():mMacroID2ClustersPos(Parameters::Instance().BASIC.IBTSPerBS*Parameters::Instance().BASIC.INumBSs) {
}

BTS& BSManager::GetBTS(const BTSID& _btsid) {
    return m_BSs[_btsid.ToInt()].GetBTS(_btsid.GetIndex());
}
///增加BS

void BSManager::AddBSs() {
    ///按输入参数为每个BS增加相应数量BTS
    for (int i = 0; i < Parameters::Instance().BASIC.INumBSs; ++i) {
        m_BSs.emplace_back(BS(i));
        m_BSs[i].AddBTSs();
    }
}

///基站初始化

void BSManager::InitializeBSs() {
    for(auto& bs: m_BSs){
        bs.Initialize();
    }
}

///基站位置分布初始化

void BSManager::DistributeBSs() {
    double x, y;
    int iNumBSs = Parameters::Instance().BASIC.INumBSs;
    if (Parameters::Instance().BASIC.IMacroTopology == Parameters::SERVICEAREA_INDOORHOTSPOT) {
        if (iNumBSs == 2) {
            assert(Parameters::Instance().BASIC.IBTSPerBS == 1);
            m_BSs[0].SetXY(-30, 0);
            m_BSs[1].SetXY(30, 0);
            for (int i = 0; i < iNumBSs; ++i) {
                    BS &bs = GetBS(i);
                double x = bs.GetX();
                double y = bs.GetY();
                //hyl 冗余
//                Observer::Print("BS_Position") << x << setw(20) << y << endl;
                    BTS &bts = bs.GetBTS(0);
                bts.SetXY(x, y);
                bts.SetTxOrientRAD(0);
                bts.SetTxHeightM(Parameters::Instance().Macro.DAntennaHeightM);
            }
        }///4基站(1 floor)矩形拓扑初始化
        else if (iNumBSs == 4) {
            assert(Parameters::Instance().BASIC.IBTSPerBS == 1);
            m_BSs[0].SetXY(-45, 0);
            m_BSs[1].SetXY(-15, 0);
            m_BSs[2].SetXY(15, 0);
            m_BSs[3].SetXY(45, 0);
            for (int i = 0; i < iNumBSs; ++i) {
                    BS &bs = GetBS(i);
                double x = bs.GetX();
                double y = bs.GetY();
                //hyl 冗余
//                Observer::Print("BS_Position") << x << setw(20) << y << endl;
                    BTS &bts = bs.GetBTS(0);
                bts.SetXY(x, y);
                bts.SetTxOrientRAD(0);
                bts.SetTxHeightM(Parameters::Instance().Macro.DAntennaHeightM);
            }
        }//hf 12 bs indoor open and mixed office
        else if (iNumBSs == 12) {

            for (int i = 0; i != 6; ++i)
                m_BSs[i].SetXY(-50 + i * 20, 10);
            for (int i = 6; i != 12; ++i)
                    m_BSs[i].SetXY(50 - (i - 6) * 20, -10);
            //            m_BSs[0].SetXY(-50, 10);
            //            m_BSs[1].SetXY(-30, 10);
            //            m_BSs[2].SetXY(-10, 10);
            //            m_BSs[3].SetXY(10, 10);
            //            m_BSs[4].SetXY(30, 10);
            //            m_BSs[5].SetXY(50, 10);
            //
            //            m_BSs[6].SetXY(50, -10);
            //            m_BSs[7].SetXY(30, -10);
            //            m_BSs[8].SetXY(10, -10);
            //            m_BSs[9].SetXY(-10, -10);
            //            m_BSs[10].SetXY(-30, -10);
            //            m_BSs[11].SetXY(-50, -10);
            for (int i = 0; i < iNumBSs; ++i) {
                    BS &bs = GetBS(i);
                double x = bs.GetX();
                double y = bs.GetY();
                //hyl 冗余
//                Observer::Print("BS_Position") << x << setw(20) << y << endl;
                if (Parameters::Instance().BASIC.IBTSPerBS == 3) {
                    // assert(Parameters::Instance().Macro.LINK.ILowfreqOrHighfreq == Parameters::ISHIGHFREQ);
                    for (int j = 0; j < Parameters::Instance().BASIC.IBTSPerBS; ++j) {
                            BTS &bts = bs.GetBTS(j);
                        bts.SetXY(x, y);
                        bts.SetTxOrientRAD(j * 2 * M_PI / 3); // 要不要+30 ？section 7.8
                        bts.SetTxHeightM(Parameters::Instance().Macro.DAntennaHeightM);
                    }
                } else if (Parameters::Instance().BASIC.IBTSPerBS == 1) {
                        BTS &bts = bs.GetBTS(0);
                    bts.SetXY(x, y);
                    bts.SetTxOrientRAD(0);
                    bts.SetTxHeightM(Parameters::Instance().Macro.DAntennaHeightM);
                } else {
                    cout << "Error in BSManager::DistributeBSs()!" << endl;
                }
            }
        }
    } else {
        ///7小区基站位置初始化
        if (iNumBSs == 7) {
            m_BSs[0].SetXY(0, 0);
            for (int i = 1; i <= 6; ++i) {
                x = Parameters::Instance().Macro.DSiteDistanceM * cos((i - 1) * M_PI / 3);
                y = Parameters::Instance().Macro.DSiteDistanceM * sin((i - 1) * M_PI / 3);
                m_BSs[i].SetXY(x, y);
            }
        }///19小区基站位置初始化
        else if (iNumBSs == 19) {
            ///0号BS位置初始化
            m_BSs[0].SetXY(0, 0);
            for (int i = 1; i <= 6; ++i) {
                ///1～6号BS位置初始化
                x = Parameters::Instance().Macro.DSiteDistanceM * cos((i - 1) * M_PI / 3);
                y = Parameters::Instance().Macro.DSiteDistanceM * sin((i - 1) * M_PI / 3);
                m_BSs[i].SetXY(x, y);
                ///7～12号BS位置初始化
                x = M_SQRT3 * Parameters::Instance().Macro.DSiteDistanceM * cos((i - 1) * M_PI / 3 + M_PI / 2);
                y = M_SQRT3 * Parameters::Instance().Macro.DSiteDistanceM * sin((i - 1) * M_PI / 3 + M_PI / 2);
                m_BSs[i + 6].SetXY(x, y);
                ///13～18号BS位置初始化
                x = 2 * Parameters::Instance().Macro.DSiteDistanceM * cos((i - 1) * M_PI / 3);
                y = 2 * Parameters::Instance().Macro.DSiteDistanceM * sin((i - 1) * M_PI / 3);
                m_BSs[i + 12].SetXY(x, y);
            }
        }
        if (iNumBSs == 2) {
            m_BSs[0].SetXY(0, 0);
            for (int i = 1; i <= 1; ++i) {
                x = Parameters::Instance().Macro.DSiteDistanceM * cos((i - 1) * M_PI / 3);
                y = Parameters::Instance().Macro.DSiteDistanceM * sin((i - 1) * M_PI / 3);
                m_BSs[i].SetXY(x, y);
            }
            } else if (iNumBSs == 1) {
            m_BSs[0].SetXY(0, 0);
        }
        for (int i = 0; i < iNumBSs; ++i) {
                BS &bs = GetBS(i);
            double x = bs.GetX();
            double y = bs.GetY();
            //hyl 冗余
//            Observer::Print("BS_Position") << x << setw(20) << y << endl;
            for (int j = 0; j < Parameters::Instance().BASIC.IBTSPerBS; ++j) {
                    BTS &bts = bs.GetBTS(j);
                bts.SetXY(x, y);
                bts.SetTxOrientRAD(j * 2 * M_PI / 3);
            }
        }
    }
}

void BSManager::DistributePicos_HetNet() {
    if (Parameters::Instance().BASIC.IScenarioModel != Parameters::SCENARIO_5G_DENSE_URBAN_2LAYER)
        return;
    int iNumBSs = Parameters::Instance().BASIC.INumBSs;
    if (Parameters::Instance().SmallCell.LINK.IHetnetEnvironmentType == Parameters::IHetnet_Normal
            || Parameters::Instance().SmallCell.LINK.IHetnetEnvironmentType == Parameters::IHetnet_4b) {
        for (int i = 0; i < iNumBSs; ++i) {
            for (int j = Parameters::Instance().BASIC.IBTSPerBS; j < Parameters::Instance().BASIC.ITotalBTSNumPerBS; ++j) {
                BTSID btsid(i, j);
                DistributeSinglePico_HetNet(btsid);
            }
        }
    } else if (Parameters::Instance().SmallCell.LINK.IHetnetEnvironmentType == Parameters::IHetnet_Cluster) {
        mMacroID2ClustersPos.clear();
        int iSiteNum = Parameters::Instance().BASIC.INumBSs;
        int iMacroNumPerSite = Parameters::Instance().BASIC.IBTSPerBS;
        int iClusterNumPerMacro = Parameters::Instance().SmallCell.LINK.IPicoClusterNumPerMacro_Cluster;
        int iPicoNumPerCluster = Parameters::Instance().SmallCell.LINK.IPicoNumPerCluster_Cluster;
        //确定Cluster坐标
        for (int i = 0; i < iSiteNum; ++i) {
            for (int j = 0; j < iMacroNumPerSite; ++j) {
                MacroID macroid(i, j);
                vector<cm::Point> vClusterPos = DistributeOutdoorPicoClusters_ClusterHetNet(macroid, iClusterNumPerMacro);
                mMacroID2ClustersPos[macroid.ToInt()] = vClusterPos;
            }
        }
        //确定small cell坐标
        for (int i = 0; i < iSiteNum; ++i) {
            for (int j = 0; j < iMacroNumPerSite; ++j) {
                MacroID macroid(i, j);
                for (int iClusterIndex = 0; iClusterIndex < iClusterNumPerMacro; ++iClusterIndex) {
                    DistributePicosInOneOutdoorCluster_ClusterHetNet(macroid, iClusterIndex, iPicoNumPerCluster);
                }
            }
        }
    } else {
        cout << "不是Hetet场景，请确认参数！" << endl;
        assert(false);
    }
}

void BSManager::DistributeSinglePico_HetNet(const BTSID& btsid) {
    int ibsid = btsid.GetBS().GetID().ToInt();
    int ipicoid = btsid.GetIndex();
    int iPicoOutNumPerBTS;
    if (Parameters::Instance().BASIC.IScenarioModel == Parameters::SCENARIO_5G_DENSE_URBAN_2LAYER && Parameters::Instance().SmallCell.LINK.IHetnetEnvironmentType == Parameters::IHetnet_Normal) {
        iPicoOutNumPerBTS = Parameters::Instance().SmallCell.LINK.IPicoNumPerMacroNormal;
    } else if (Parameters::Instance().BASIC.IScenarioModel == Parameters::SCENARIO_5G_DENSE_URBAN_2LAYER && Parameters::Instance().SmallCell.LINK.IHetnetEnvironmentType == Parameters::IHetnet_4b) {
        iPicoOutNumPerBTS = Parameters::Instance().SmallCell.LINK.IPicoNumPerMacro4b;
    } else {
        cout << "不是Hetet场景，请确认参数！" << endl;
        assert(false);
    }
    ///@zxy 撒pico时的参考BTS（包括位置和天线朝向）
    int ibtsid = (int) ((ipicoid - Parameters::Instance().BASIC.IBTSPerBS) / iPicoOutNumPerBTS);
    BTSID btsid0 = BTSID(ibsid, ibtsid);
    BTS& bts0 = btsid0.GetBTS();
    ///@zxy 需要撒的pico ，被当作BTS处理
    BTSID btsid1 = BTSID(ibsid, ipicoid);
    BTS& bts1 = btsid1.GetBTS();

    int counter = 0;
    bool flag = true;
    do {
        flag = false;
        counter++;
        double p0 = 1.0 / 3.0;
        double x = 0, y = 0;
        double a = 0, b = 0;
        double orient = 0, p = 0;
        double dRadus = Parameters::Instance().Macro.DCellRadiusM;

        p = random.xUniform_distributepico(0, 1);
        if (p < p0) {
            x = random.xUniform_distributepico(0, dRadus * 0.5);
            y = random.xUniform_distributepico(-M_SQRT3 * dRadus * 0.5, M_SQRT3 * dRadus * 0.5);
            if (abs(y) > x * M_SQRT3) {
                x += 1.5 * dRadus;
                y += (y > 0 ? -1 : 1) * M_SQRT3 * dRadus * 0.5;
            }
        } else {
            x = random.xUniform_distributepico(dRadus * 0.5, 1.5 * dRadus);
            y = random.xUniform_distributepico(-M_SQRT3 * dRadus * 0.5, M_SQRT3 * dRadus * 0.5);
        }
        ///全向天线
        if (Parameters::Instance().BASIC.IBTSPerBS == 1) {
            orient = floor(random.xUniform_distributepico(0, 1) * 3) * 2 * M_PI / 3;
        }///3扇区天线
        else if (Parameters::Instance().BASIC.IBTSPerBS == 3) {
            orient = bts0.GetTxOrientRAD();
        }
        a = x * cos(orient) - y * sin(orient);
        b = y * cos(orient) + x * sin(orient);
        x = bts0.GetX() + a;
        y = bts0.GetY() + b;
        bts1.SetXY(x, y);

        ///@zxy 计算每个pico与BS以及已完成撒点的pico的距离
        for (int i = 0; i < Parameters::Instance().BASIC.INumBSs; ++i) {
            double dx = m_BSs[i].GetX() - x;
            double dy = m_BSs[i].GetY() - y;
            double distance = sqrt(dx * dx + dy * dy);
            if (distance < Parameters::Instance().SmallCell.LINK.DMinDistPico2MacroM) {
                flag = true;
                break;
            }
        }
        if (!flag) {
            for (int j = 0; j < ibsid - 1; ++j) { ///@zxy 对于正在撒点的BTS所处的BS单独处理
                for (int k = Parameters::Instance().BASIC.IBTSPerBS; k < Parameters::Instance().BASIC.IBTSPerBS * (1 + iPicoOutNumPerBTS); ++k) {
                    BTSID btsid2 = BTSID(j, k);
                    BTS& bts2 = btsid2.GetBTS();
                    double ddx = bts2.GetX() - x;
                    double ddy = bts2.GetY() - y;
                    double distance2 = sqrt(ddx * ddx + ddy * ddy);
                    if (distance2 < Parameters::Instance().SmallCell.LINK.DMinDistPico2PicoM) {
                        flag = true;
                        break;
                    }
                }
                if (flag) ///@zxy 用于跳出外层循环
                    break;
            }
            ////@zxy 计算正在撒点的pico所在的BS下的其他pico的距离
            if (!flag) {
                for (int m = Parameters::Instance().BASIC.IBTSPerBS; m < ipicoid; ++m) {
                    BTSID btsid3 = BTSID(ibsid, m);
                    BTS& bts3 = btsid3.GetBTS();
                    double ddx = bts3.GetX() - x;
                    double ddy = bts3.GetY() - y;
                    double distance3 = sqrt(ddx * ddx + ddy * ddy);
                    if (distance3 < Parameters::Instance().SmallCell.LINK.DMinDistPico2PicoM) {
                        flag = true;
                        break;
                    }
                }
            }
        }
    } while (flag && (counter < 100));

    Observer::Print("Pico_Position") << bts1.GetX() << setw(20) << bts1.GetY() << endl;
}

vector<cm::Point> BSManager::DistributeOutdoorPicoClusters_ClusterHetNet(MacroID macroid, int _iClusterNumPerMacro) {
    assert(IsMacro(macroid));
    vector<cm::Point> vClusterPos;
    int iSiteNum = Parameters::Instance().BASIC.INumBSs;
    int iMacroNumPerSite = Parameters::Instance().BASIC.IBTSPerBS;
    double dMinDistCluster2Macro = Parameters::Instance().SmallCell.LINK.DMinDistCluster2MacroM;
    double dMinDistCluster2Cluster = 2 * Parameters::Instance().SmallCell.LINK.DMinDistClusterRadiusM_PicoDrop;
    int i = 0;
    int iCount = 0;
    while (i < _iClusterNumPerMacro) {
        double dx = 0, dy = 0;
        double rx = random.xUniform_distributepico(-1, 0);
        double ry = random.xUniform_distributepico(0, 1);
        double dRadus = Parameters::Instance().Macro.DCellRadiusM;
        double ytemp = dRadus * 0.5 * sqrt(3) * ry;
        double xtemp = dRadus * rx + ytemp / M_SQRT3;
        double orient = floor(random.xUniform_distributepico(0, 1) * 3) * 2 * M_PI / 3;
        double dxtemp = xtemp * cos(orient) - ytemp * sin(orient);
        double dytemp = ytemp * cos(orient) + xtemp * sin(orient);
        int index = macroid.GetIndex();
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

        double orient2 = index * 2 * M_PI / 3;
        dx = dxtemp * cos(orient2) - dytemp * sin(orient2);
        dy = dytemp * cos(orient2) + dxtemp * sin(orient2);
        double x = macroid.GetBTS().GetX() + dx + doffset1;
        double y = macroid.GetBTS().GetY() + dy + doffset2;

        //        double x = macroid.GetBTS().GetX() + dxtemp + doffset1;
        //        double y = macroid.GetBTS().GetY() + dytemp + doffset2;
        cm::Point clusterPos(x, y);

        ///判断是否满足距离要求
        ///与所在的macro之间的距离
        bool flag = true;
        if (cm::Distance(clusterPos, GetBTS(macroid)) < dMinDistCluster2Macro) {
            flag = false;
            continue;
        }

        ///与其他macro中已有clusters之间的距离
        for (int iSite = 0; iSite < iSiteNum; ++iSite) {
            for (int iMacro = 0; iMacro < iMacroNumPerSite; ++iMacro) {
                MacroID otherMacroid(iSite, iMacro);
                if (otherMacroid < macroid) {
                    // 没在用的函数，这个find应该不能直接用ToInt，先不管了
                    assert(mMacroID2ClustersPos.find(otherMacroid.ToInt()) != mMacroID2ClustersPos.end());
                    vector<cm::Point> vOtherClusterPos = mMacroID2ClustersPos[otherMacroid.ToInt()];
                    for (const auto& pos : vOtherClusterPos) {
                        cm::Point WrapClusterPos = cm::WrapAround::Instance().WrapRx(clusterPos, GetBTS(otherMacroid)); //
                        if (cm::Distance(WrapClusterPos,pos) < dMinDistCluster2Cluster) {
                            flag = false;
                            break;
                        }
                    }
                }
                if (!flag) break;
            }
            if (!flag) break;
        }

        ///与所在的macro中已有clusters之间的距离
        int iClusterNum = static_cast<int> (vClusterPos.size());
        for (int index = 0; index < iClusterNum; ++index) {
            if (cm::Distance(clusterPos, vClusterPos[index]) < dMinDistCluster2Cluster) {
                flag = false;
                break;
            }
        }

        if (!flag) {
            if (++iCount > 5000) {
                vClusterPos.clear();
                i = 0;
            }
            continue;
        } else {
            vClusterPos.push_back(clusterPos);
            i++;
            Observer::Print("ClusterPos") << x << setw(20) << y << endl;
        }
    }

    return vClusterPos;
}

void BSManager::DistributePicosInOneOutdoorCluster_ClusterHetNet(const MacroID& macroid, int _iClusterIndex, int _iPicoNumPerCluster) {
    vector<cm::Point> vPicoPos;
    int iSiteNum = Parameters::Instance().BASIC.INumBSs;
    int iMacroNumPerSite = Parameters::Instance().BASIC.IBTSPerBS;
    int iClusterNumPerMacro = Parameters::Instance().SmallCell.LINK.IPicoClusterNumPerMacro_Cluster;
    int iPicoNumPerCluster = Parameters::Instance().SmallCell.LINK.IPicoNumPerCluster_Cluster;
    double dMinDistClusterDropPicoRadius = Parameters::Instance().SmallCell.LINK.DMinDistClusterRadiusM_PicoDrop;
    double dMinDistPico2Pico = Parameters::Instance().SmallCell.LINK.DMinDistPico2PicoM;
    int iPicoStartIndex = iMacroNumPerSite + macroid.GetIndex() * iClusterNumPerMacro * iPicoNumPerCluster + _iClusterIndex*iPicoNumPerCluster;
    int iPicoEndIndex = iMacroNumPerSite + macroid.GetIndex() * iClusterNumPerMacro * iPicoNumPerCluster + (_iClusterIndex + 1) * iPicoNumPerCluster;
    while (iPicoStartIndex < iPicoEndIndex) {
        //撒Pico
        PicoID picoid(macroid.GetBSID().ToInt(), iPicoStartIndex);
        assert(IsPico(picoid));
        //        double orient = xUniform_distributepico(0, 2*pi);
        //        double r = xUniform_distributepico(0, dMinDistClusterDropPicoRadius);
        //        double dx = r * cos(orient);
        //        double dy = r * sin(orient);

        double dx = 0;
        double dy = 0;
        do {
            dx = dMinDistClusterDropPicoRadius * random.xUniform_distributepico(-1, 1);
            dy = dMinDistClusterDropPicoRadius * random.xUniform_distributepico(-1, 1);
        } while (sqrt(pow(dx, 2.0) + pow(dy, 2.0)) >= dMinDistClusterDropPicoRadius);
        // 没在用的函数，这个find应该不能直接用ToInt，先不管了
        assert(mMacroID2ClustersPos.find(macroid.ToInt()) != mMacroID2ClustersPos.end());
        cm::Point clusterPos = mMacroID2ClustersPos[macroid.ToInt()][_iClusterIndex];
        double x = clusterPos.GetX() + dx;
        double y = clusterPos.GetY() + dy;
        cm::Point picoPos(x, y);

        ///判断是否满足距离要求
        bool flag = true;
        for (BTSID btsid = BTSID::Begin(); btsid < picoid; ++btsid) {
            if (IsMacro(btsid)) continue;
            cm::Point WrapPicoPos = cm::WrapAround::Instance().WrapRx(picoPos, BSManager::GetMacroIDofPico(btsid).GetBTS());
            if (cm::Distance(WrapPicoPos, GetBTS(btsid)) < dMinDistPico2Pico) {
                flag = false;
                break;
            }
        }
        if (!flag) continue;

        for (const auto& pos : vPicoPos) {
                if (cm::Distance(picoPos,pos) < dMinDistPico2Pico) {
                    flag = false;
                    break;
                }

        }

        if (!flag) {
            continue;
        } else {
            BTS& pico = picoid.GetBTS();
            pico.SetXY(x, y);
            vPicoPos.push_back(picoPos);
            iPicoStartIndex++;
            Observer::Print("SmallCellPos") << x << setw(20) << y << endl;
        }

    }
}

///基站管理类每TTI进行的操作

void BSManager::WorkSlot() {
    //@threads
    for (int ibs = 0; ibs < CountBS(); ibs++) {
        m_BSs[ibs].WorkSlot();
    }
}

///重置基站管理类

void BSManager::Reset() {
    for(auto& bs:m_BSs){
        bs.Reset();
    }
}

/// @brief 根据BTSID判断该BTS是否为Pico

bool BSManager::IsPico(const BTSID& btsid) {
    if (btsid.GetIndex() < Parameters::Instance().BASIC.IBTSPerBS) {
        return false;
    } else {
        assert(btsid.GetIndex() < Parameters::Instance().BASIC.ITotalBTSNumPerBS);
        return true;
    }
}

/// @brief 根据BTSID判断该BTS是否为Macro

bool BSManager::IsMacro(const BTSID& btsid) {
    return !IsPico(btsid);
}

///@brief 取得pico所属的MacroID

MacroID BSManager::GetMacroIDofPico(PicoID picoid) {
    if (!IsPico(picoid)) {
        cout << "This is not a pico!" << endl;
        assert(false);
    }
    MacroID macroid(picoid.GetBSID(), (picoid.GetIndex() - Parameters::Instance().BASIC.IBTSPerBS) / Parameters::Instance().SmallCell.LINK.IPicoNumPerMacro);
    return macroid;
}

///@brief 取得Macro的PicoIDs

vector<PicoID> BSManager::GetPicoIDsofMacro(MacroID macroid) {
    if (!IsMacro(macroid)) {
        cout << "This is not a Macro!" << endl;
        assert(false);
    }
    vector<PicoID> vpicoid;
    for (int i = 0; i < Parameters::Instance().SmallCell.LINK.IPicoNumPerMacro; ++i) {
        PicoID picoid(macroid.GetBSID(), macroid.GetIndex() * Parameters::Instance().SmallCell.LINK.IPicoNumPerMacro + Parameters::Instance().BASIC.IBTSPerBS + i);
        vpicoid.push_back(picoid);
    }
    return vpicoid;
}

void BSManager::ConstructPicoGroups() {
    if (Parameters::Instance().BASIC.IScenarioModel == Parameters::SCENARIO_5G_DENSE_URBAN_2LAYER && Parameters::Instance().SmallCell.LINK.IHetnetEnvironmentType == Parameters::IHetnet_Cluster) {
        for (int i = 0; i < Parameters::Instance().BASIC.INumBSs; ++i) {
            for (int j = 0; j < Parameters::Instance().BASIC.IBTSPerBS; ++j) {
                vector<Group> vGroup;
                for (int k = 0; k < Parameters::Instance().SmallCell.LINK.IPicoClusterNumPerMacro_Cluster; ++k) {
                    GroupID groupID(i, j, k);
                    Group group(groupID);
                    ///可以在此设置Group的坐标
                    for (int l = Parameters::Instance().BASIC.IBTSPerBS + k * Parameters::Instance().SmallCell.LINK.IPicoNumPerCluster_Cluster; l < Parameters::Instance().BASIC.IBTSPerBS + (k + 1) * Parameters::Instance().SmallCell.LINK.IPicoNumPerCluster_Cluster; ++l)
                        group.AddPicoIDs(m_BSs[i].GetBTS(l).GetID());
                }
                m_BSs[i].AddGroups(vGroup);
            }
        }
    }
}

vector<cm::Point> BSManager::GetClusterPos(const MacroID& macroid) {
    vector<cm::Point> vClusterPos;
    if (mMacroID2ClustersPos.find(macroid.ToInt()) != mMacroID2ClustersPos.end())
        vClusterPos = mMacroID2ClustersPos[macroid.ToInt()];

    return vClusterPos;
}