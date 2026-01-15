///@file LinkMatrix.h
///@brief 定义channelmodel的接口类的函数
///@author wangfei

#pragma once
#include "./libfiles.h"
#include "P.h"
#include "AntennaPanel.h"
#include "../Statistician/ChannelInfo.h"
#include "../Utility/Random.h"
//@threads
#include "../SafeMap.h"
#include "../SafeUnordered_map.h"
#include <condition_variable>
#include "../MobileStation/MS.h"
#include "../BaseStation/BTS.h"
//@threads
class ChannelUpdateThread;
namespace cm {
    class AntennaOrientGain;
    class Tx;
    class Rx;
    class ChannelState;
    class ChannelState_Simplified;
    class PathLoss;
    class GaussianMap;


    ///@brief 链路矩阵类，维护所有发送者和接收者之间的链路的状态
    ///
    ///包含每一个发送者和每一个接收者之间的链路的状态，其中一部分链路被定义为强链路，强链路提供大尺度和小尺度
    ///信道模型，通过小尺度信道模型可以得到子载波上的信道矩阵；弱链路只提供大尺度信道模型。本类提供了若干接口
    ///返回大尺度的信道状态信息和小尺度的信道状态信息。
    class LinkMatrix {
        typedef std::pair<int, int> TxRxID;
        typedef std::map<TxRxID, ChannelState> TxRxCS;
        typedef std::pair<int, int> TxRISID;
        typedef std::map<TxRISID, ChannelState> TxRISCS;
        typedef std::pair<int, int> RISRxID;
        typedef std::map<RISRxID, ChannelState> RISRxCS;
        //@threads
        //typedef map<TxRxID, ChannelState> TxRxCS;
        friend ChannelUpdateThread;

    //private:
    public:
        Random random;
        TxRxCS m_TxRx2CS;
        RISRxCS m_RISRx2CS;
        TxRISCS m_TxRIS2CS;
        //        std::map<std::pair<double, double>, double> m_Pos2Din;
        SafeUnordered_map<std::pair<int, int>, double> m_Pos2Din;
        SafeUnordered_map<std::pair<int, int>, double> m_Pos2PentratinlossSF;

    public:
        /// @brief 判断发送者与接收者之间的链路是否存在直射径
        /// @param _tx 发送者引用
        /// @param _rx 接收者引用
        /// @return true 表示存在直射径，false 表示不存在直射径
        bool IsLOS(Tx& _tx, Rx& _rx);
        /// @brief 判断发送者与接收者之间的链路是否是强链路，如果是强链路则存在小尺度信道模型，否则只存在大尺度信道模型
        /// @param _tx 发送者引用
        /// @param _rx 接收者引用
        /// @return true 表示是强链路，存在小尺度信道模型；false 表示是弱链路
        bool IsStrong(Tx& _tx, Rx& _rx);
        /// @brief 取发送者与接收者之间的总链路损耗
        /// @param _tx 发送者引用
        /// @param _rx 接收者引用
        /// @return 总链路损耗(单位dB)，总链路损耗包含：1.发送天线增益；2.路径损耗；3.车体损耗；4.穿透损耗；5.阴影衰落；6.接收天线增益
        double GetCouplingLossDB(Tx& _tx, Rx& _rx);
        ///@该函数主要用于输出根据LoS径天线增益算出的Linkloss，主要用于选择强链路
        double GetLoSLinkLossDB(Tx& _tx, Rx& _rx);
        /// @brief 取发送者与接收者之间的阴影衰落
        /// @param _tx 发送者引用
        /// @param _rx 接收者引用
        /// @return 阴影衰落（dB）
        double GetShadowFadeDB(Tx& _tx, Rx& _rx);
        /// 取发送者与接收者之间的距离
        /// @param _tx 发送者引用
        /// @param _rx 接收者引用
        /// @return 距离（单位：米）
        double Get2DDistanceM(Tx& _tx, Rx& _rx);
        /// 取发送者与接收者之间的3D距离
        /// @param _tx 发送者引用
        /// @param _rx 接收者引用
        /// @return 距离（单位：米）
        double Get3DDistanceM(Tx& _tx, Rx& _rx);
        /// @brief 取发送者与接收者之间的路径损耗
        /// @param _tx 发送者引用
        /// @param _rx 接收者引用
        /// @return 路径损耗（单位：dB）（纯链路损耗，注意与总链路损耗相区分）
        /// @see GetLinkLossDB
        double GetPathLossDB(Tx& _tx, Rx& _rx);
        /// @brief 取发送者与接收者之间的除天线增益外的路径损耗
        /// @param _tx 发送者引用
        /// @param _rx 接收者引用
        /// @return 除天线增益外的路径损耗（单位：dB）（纯链路损耗，注意与总链路损耗相区分）
        /// @see GetLinkLossDB
        double GetPurePathLossDB(Tx& _tx, Rx& _rx);
        
        double GetESD(Tx& _tx, Rx& _rx);
        double GetESA(Tx& _tx, Rx& _rx);
        double GetLoSAoDRAD(Tx& _tx, Rx& _rx);
        double GetLoSEoDRAD(Tx& _tx, Rx& _rx);
        double GetLoSAoARAD(Tx& _tx, Rx& _rx);
        double GetLoSEoARAD(Tx& _tx, Rx& _rx);

        int GetStrongestUEPanelIndex(Tx& _tx, Rx& _rx);
        
        // get BSBeamIndex for strongset panel pair
        int GetStrongestBSBeamIndex(Tx& _tx, Rx& _rx);
        
        // get UEBeamIndex for strongset panel pair
        int GetStrongestUEBeamIndex(Tx& _tx, Rx& _rx);
        
//        std::shared_ptr<AntennaPanel> GetStrongestUEPanel(Tx& _tx, Rx& _rx);
//        std::shared_ptr<AntennaPanel> GetStrongestBSPanel(Tx& _tx, Rx& _rx);
        
        double GetCouplingLoss_linear_u36873_w_BestBeamPair_4_BestPanelPair(
            Tx& _tx, Rx& _rx);
        
        double GetCouplingLoss_linear_u36873_w_RandomBSBeam_4_BestPanelPair(
            Tx& _tx, Rx& _rx);
        double GetDinM(Tx& _tx, Rx& _rx);

        bool GetPathInfo(
                Tx& _tx, 
                Rx& _rx, 
                std::vector<double>& _vPathPower,
                std::vector<double>& _vSubPathPower, 
                std::vector<double>& _vPathAoD, 
                std::vector<double>& _vSubPathAoD, 
                std::vector<double>& _vPathEoD, 
                std::vector<double>& _vSubPathEoD, 
                std::vector<double>& _vPathEoA, 
                std::vector<double>& _vSubPathEoA);
        /// @brief 初始化所有发送者与接收者之间的链路
        void Initialize();

        void InitStrongSCM();

        /// @brief 初始化一个接收者与所有发送者之间的链路
        /// @param _rx 接收者引用
        void Initialize(Rx& _rx);

        void RISInitialize_thread1(MS &ms);

        /// @brief 重置函数，清空本类所持有的所有链路
        void Reset();
        /// @brief 时间驱动的接口函数，将所有链路更新到输入参数所指定的时间点
        /// @param _dTimeSec 信道状态的时间点，（单位：秒）
        void WorkSlot(double _dTimeSec);
        /// @brief 时间驱动的接口函数，将指定接收者对应的所有链路更新到输入参数所指定的时间点
        /// @param _rx 接收者引用
        /// @param _dTimeSec 信道状态的时间点，（单位：秒）
        void WorkSlot(Rx& _rx, double _dTimeSec);

        void WorkSlot_RIS(double t);

        /// @brief 返回计算路径损耗的函数
        /// @param _tx 发送者引用
        /// @param _rx 接收者引用
        /// @return 一个可以计算路径损耗的智能指针
        std::shared_ptr<PathLoss> GetPathLossFun(Tx& _tx, Rx& _rx, bool _bIsLOS);
        void SetPos2Din(Tx& _tx, Rx& _rx);
        double GetPos2Din(std::pair<int, int> & _pos);

        itpp::cmat GetFadingMat_wABF_for_active_TXRU_Pairs_per_PanelPair(
            Tx& _tx, Rx& _rx, int _scid,
            AntennaPanel* _pBS_Panel,
            AntennaPanel* _pUE_Pane);
        
        itpp::cmat GetFadingMat_wABF_for_all_active_TXRU_Pairs(
            Tx& _tx, Rx& _rx, int _scid);

        itpp::cmat Get_mD_wABF_for_all_active_TXRU_Pairs(
                Tx& _tx, Rx& _rx, int _scid);

        itpp::cmat GetFadingMat_wABF_for_all_active_TXRU_Pairs2(
            Tx& _tx, Rx& _rx, int _scid, 
            const std::vector<std::pair<int, std::vector<int> > >& _vUEPanelAndvBSBeam);

        const itpp::cmat GetFadingMat_wABF_for_all_active_TXRU_Pairs_RISIntf(
                Tx& _tx, Rx& _rx, int _scid);

        itpp::cmat GetmD_dl(Tx& _tx, Rx& _rx, int _scid);
        itpp::cmat GetmD_ul(Tx& _tx, Rx& _rx, int _scid);
 
    private:
        /// 配合实现“单件”模式的静态指针变量
        static LinkMatrix* m_pLM;
        int ms_finished = 0;
        int link2_finished=0;
    public:
        /// @brief 通过该静态函数返回唯一的链路矩阵对象
        static LinkMatrix& Instance();
        void RISInitialize();
        //@threads
        static unsigned int threadnum;
        //@threads
    private:
        /// @brief 构造函数
        LinkMatrix();
        /// @brief 拷贝构造函数
        LinkMatrix(const LinkMatrix&);
        /// @brief 赋值构造函数
        LinkMatrix & operator=(const LinkMatrix&);
        /// @brief 析构函数
        ~LinkMatrix() = default;
        void initialthread(int i);
    protected:
        int m_H_AntNumPerTXRU;
        int m_V_AntNumPerTXRU;

    };
}