#include "TwoLayerNetwork.h"
#include "../Parameters/Parameters.h"
#include "../DistributeMSAlgo/DistributeMSAlgo4indoor.h"
#include "../DistributeMSAlgo/DistributeMSAlgoCenterRectangle.h"
#include "../DistributeMSAlgo/DistributeMSAlgoDiamond.h"
#include "../DistributeMSAlgo/DistributeMSAlgoFile.h"
#include "../DistributeMSAlgo/DistributeMSAlgoHexagon.h"
#include "../DistributeMSAlgo/DistributeMSAlgoForHetNet.h"
#include "../DistributeMSAlgo/DistributeMSAlgoFix.h"
#include "../Statistician/Observer.h"

using namespace std;

///构造函数
TwoLayerNetwork::TwoLayerNetwork(void){
    // For HetNet
    m_pDMSA = std::shared_ptr<DistributeMSAlgo > (new DistributeMSAlgoForHetNet);
}

///析构函数
TwoLayerNetwork::~TwoLayerNetwork(void){
}

///基站位置分布初始化
void TwoLayerNetwork::DistributeBSs() {
    //第一层
    StandardDistribute();
    //第二层
    ConstructPicoGroups();
    //撒Pico
    DistributePicos_HetNet();
}

void TwoLayerNetwork::ConstructPicoGroups() {
    BSManager& bsm = BSManager::Instance();
    vector<std::shared_ptr<BS> >& vpBS = bsm.GetvpBS();
    if (Parameters::Instance().SmallCell.LINK.IHetnetEnvironmentType == Parameters::IHetnet_Cluster) {
        for (int i = 0; i < Parameters::Instance().BASIC.INumBSs; ++i) {
            for (int j = 0; j < Parameters::Instance().BASIC.IBTSPerBS; ++j) {
                vector<Group> vGroup;
                for (int k = 0; k < Parameters::Instance().SmallCell.LINK.IPicoClusterNumPerMacro_Cluster; ++k) {
                    GroupID groupID(i, j, k);
                    Group group(groupID);
                    ///可以在此设置Group的坐标
                    for (int l = Parameters::Instance().BASIC.IBTSPerBS + k * Parameters::Instance().SmallCell.LINK.IPicoNumPerCluster_Cluster; l < Parameters::Instance().BASIC.IBTSPerBS + (k + 1) * Parameters::Instance().SmallCell.LINK.IPicoNumPerCluster_Cluster; ++l)
                        group.AddPicoIDs(vpBS[i]->GetBTS(l).GetID());
                }
                vpBS[i]->AddGroups(vGroup);
            }
        }
    }
}

void TwoLayerNetwork::DistributePicos_HetNet() {
    BSManager& bsm = BSManager::Instance();
    int iNumBSs = Parameters::Instance().BASIC.INumBSs;
    if (Parameters::Instance().SmallCell.LINK.IHetnetEnvironmentType == Parameters::IHetnet_Normal || 
        Parameters::Instance().SmallCell.LINK.IHetnetEnvironmentType == Parameters::IHetnet_4b) {
        for (int i = 0; i < iNumBSs; ++i) {
            for (int j = Parameters::Instance().BASIC.IBTSPerBS; j < Parameters::Instance().BASIC.ITotalBTSNumPerBS; ++j) {
                BTSID btsid(i, j);
                DistributeSinglePico_HetNet(btsid);
            }
        }
    } else if (Parameters::Instance().SmallCell.LINK.IHetnetEnvironmentType == Parameters::IHetnet_Cluster) {
        bsm.GetClusterPos().clear();
        int iSiteNum = Parameters::Instance().BASIC.INumBSs;
        int iMacroNumPerSite = Parameters::Instance().BASIC.IBTSPerBS;
        int iClusterNumPerMacro = Parameters::Instance().SmallCell.LINK.IPicoClusterNumPerMacro_Cluster;
        //确定Cluster坐标
        for (int i = 0; i < iSiteNum; ++i) {
            for (int j = 0; j < iMacroNumPerSite; ++j) {
                MacroID macroid(i, j);
                vector<cm::Point> vClusterPos = OutdoorPicoClusters(macroid, iClusterNumPerMacro);
                map<MacroID, vector<cm::Point> >& mID2Pos = bsm.GetClusterPos();
                mID2Pos[macroid] = vClusterPos;
            }
        }
        //确定small cell坐标
        for (int i = 0; i < iSiteNum; ++i) {
            for (int j = 0; j < iMacroNumPerSite; ++j) {
                MacroID macroid(i, j);
                for (int iClusterIndex = 0; iClusterIndex < iClusterNumPerMacro; ++iClusterIndex) {
                    PicosInOutdoorCluster(macroid, iClusterIndex);
                }
            }
        }
    } else {
        cout << "不是Hetet场景，请确认参数！" << endl;
        assert(false);
    }
}

void TwoLayerNetwork::DistributeSinglePico_HetNet(BTSID btsid) {
    BSManager& bsm = BSManager::Instance();
    vector<std::shared_ptr<BS> >& vpBS = bsm.GetvpBS();
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

        p = xUniform_distributepico(0, 1);
        if (p < p0) {
            x = xUniform_distributepico(0, dRadus * 0.5);
            y = xUniform_distributepico(-M_SQRT3 * dRadus * 0.5, M_SQRT3 * dRadus * 0.5);
            if (abs(y) > x * M_SQRT3) {
                x += 1.5 * dRadus;
                y += (y > 0 ? -1 : 1) * M_SQRT3 * dRadus * 0.5;
            }
        } else {
            x = xUniform_distributepico(dRadus * 0.5, 1.5 * dRadus);
            y = xUniform_distributepico(-M_SQRT3 * dRadus * 0.5, M_SQRT3 * dRadus * 0.5);
        }
        ///全向天线
        if (Parameters::Instance().BASIC.IBTSPerBS == 1) {
            orient = floor(xUniform_distributepico(0, 1) * 3) * 2 * M_PI / 3;
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
            double dx = vpBS[i]->GetX() - x;
            double dy = vpBS[i]->GetY() - y;
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

vector<cm::Point> TwoLayerNetwork::OutdoorPicoClusters(MacroID macroid, int _iClusterNumPerMacro) {
    BSManager& bsm = BSManager::Instance();
    assert(bsm.IsMacro(macroid));
    vector<cm::Point> vClusterPos;
    int iSiteNum = Parameters::Instance().BASIC.INumBSs;
    int iMacroNumPerSite = Parameters::Instance().BASIC.IBTSPerBS;
    double dMinDistCluster2Macro = Parameters::Instance().SmallCell.LINK.DMinDistCluster2MacroM;
    double dMinDistCluster2Cluster = 2 * Parameters::Instance().SmallCell.LINK.DMinDistClusterRadiusM_PicoDrop;
    int i = 0;
    int iCount = 0;
    while (i < _iClusterNumPerMacro) {
        double dx = 0, dy = 0;
        double rx = xUniform_distributepico(-1, 0);
        double ry = xUniform_distributepico(0, 1);
        double dRadus = Parameters::Instance().Macro.DCellRadiusM;
        double ytemp = dRadus * 0.5 * sqrt(3) * ry;
        double xtemp = dRadus * rx + ytemp / M_SQRT3;
        double orient = floor(xUniform_distributepico(0, 1) * 3) * 2 * M_PI / 3;
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
        if (cm::Distance(clusterPos, bsm.GetBTS(macroid)) < dMinDistCluster2Macro) {
            flag = false;
            continue;
        }

        ///与其他macro中已有clusters之间的距离
        for (int iSite = 0; iSite < iSiteNum; ++iSite) {
            for (int iMacro = 0; iMacro < iMacroNumPerSite; ++iMacro) {
                MacroID otherMacroid(iSite, iMacro);
                if (otherMacroid < macroid) {
                    map<MacroID, vector<cm::Point> >& mID2Pos = bsm.GetClusterPos();
                    assert(mID2Pos.find(otherMacroid) != mID2Pos.end());
                    vector<cm::Point> vOtherClusterPos = mID2Pos[otherMacroid];
                    for (int index = 0; index<static_cast<int> (vOtherClusterPos.size()); ++index) {
                        cm::Point WrapClusterPos = cm::WrapAround::Instance().WrapRx(clusterPos, bsm.GetBTS(otherMacroid)); //
                        if (cm::Distance(WrapClusterPos, vOtherClusterPos[index]) < dMinDistCluster2Cluster) {
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

void TwoLayerNetwork::PicosInOutdoorCluster(MacroID macroid, int _iClusterIndex) {
    vector<cm::Point> vPicoPos;
    BSManager& bsm = BSManager::Instance();
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
        assert(bsm.IsPico(picoid));

        double dx = 0;
        double dy = 0;
        do {
            dx = dMinDistClusterDropPicoRadius * xUniform_distributepico(-1, 1);
            dy = dMinDistClusterDropPicoRadius * xUniform_distributepico(-1, 1);
        } while (sqrt(pow(dx, 2.0) + pow(dy, 2.0)) >= dMinDistClusterDropPicoRadius);

        map<MacroID, vector<cm::Point> >& mID2Pos = bsm.GetClusterPos();
        assert(mID2Pos.find(macroid) != mID2Pos.end());
        cm::Point clusterPos = mID2Pos[macroid][_iClusterIndex];
        double x = clusterPos.GetX() + dx;
        double y = clusterPos.GetY() + dy;
        cm::Point picoPos(x, y);

        ///判断是否满足距离要求
        bool flag = true;
        for (BTSID btsid = BTSID::Begin(); btsid < picoid; ++btsid) {
            if (bsm.IsMacro(btsid)) continue;
            cm::Point WrapPicoPos = cm::WrapAround::Instance().WrapRx(picoPos, BSManager::GetMacroIDofPico(btsid).GetBTS());
            if (cm::Distance(WrapPicoPos, bsm.GetBTS(btsid)) < dMinDistPico2Pico) {
                flag = false;
                break;
            }
        }
        if (!flag) continue;

        for (int index = 0; index<static_cast<int> (vPicoPos.size()); ++index) {
            if (cm::Distance(picoPos, vPicoPos[index]) < dMinDistPico2Pico) {
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