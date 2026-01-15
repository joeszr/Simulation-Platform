#pragma once
#include "../ChannelModel/libfiles.h"
#include "../ChannelModel/P.h"
#include "../ChannelModel/AntennaPanel.h"
#include "../ChannelModel/LinkMatrix.h"
#include "../MobileStation/MS.h"
class RIS;

namespace cm {

    ///@brief 链路矩阵类，维护所有发送者和接收者之间的链路的状态
    ///
    ///包含每一个发送者和每一个接收者之间的链路的状态，其中一部分链路被定义为强链路，强链路提供大尺度和小尺度
    ///信道模型，通过小尺度信道模型可以得到子载波上的信道矩阵；弱链路只提供大尺度信道模型。本类提供了若干接口
    ///返回大尺度的信道状态信息和小尺度的信道状态信息。
    class RISMatrix {
        typedef std::pair<int, int> RISRxID;
        typedef std::map<RISRxID, ChannelState> RISRxCS;
        typedef std::pair<int, int> TxRxID;
        typedef std::pair<int, int> TxRISID;
        typedef std::map<TxRISID, ChannelState> TxRISCS;

    private:
        RISRxCS m_RISRx2CS;
        TxRISCS m_TxRIS2CS;
        std::map<std::pair<int, int>, double> m_Pos2Din;
        std::map<std::pair<int, int>, double> m_Pos2PentratinlossSF;
        std::vector<RIS> RIS_Update;
    public:
        void InitThread_RIS(std::vector<MS*>& vpMs);
        void UpdateThread_RIS(std::vector<MS*>& vpMs);
        void CalH_Total(MS& ms);
        void CalH_Total(BTS& _bts,RIS& _RIS, MS& ms);//建立小尺度级联信道矩阵
        void CalH_Total(int BS_BeamIndex, BTS& _bts,RIS& _RIS, MS& ms);
        void WorkSlot();
        void WorkSlot(MS& ms);
        void ClearRISMatrix(Rx& _rx);
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
        double GetCouplingLossDB(Tx& _tx, RIS& _ris);
        double GetCouplingLossDB(RIS& _ris, Rx& _rx);
        /// @brief 取发送者与接收者之间的阴影衰落
        /// @param _tx 发送者引用
        /// @param _rx 接收者引用
        /// @return 阴影衰落（dB）
        double GetShadowFadeDB(Tx& _tx, RIS& _ris);
        double GetShadowFadeDB(RIS& _ris, Rx& _rx);
        /// 取发送者与接收者之间的距离
        /// @param _tx 发送者引用
        /// @param _rx 接收者引用
        /// @return 距离（单位：米）
        double Get2DDistanceM(Tx& _tx, RIS& _ris);
        double Get2DDistanceM(RIS& _ris, Rx& _rx);
        /// 取发送者与接收者之间的3D距离
        /// @param _tx 发送者引用
        /// @param _rx 接收者引用
        /// @return 距离（单位：米）
        double Get3DDistanceM(Tx& _tx, RIS& _ris);
        double Get3DDistanceM(RIS& _ris, Rx& _rx);
        /// @brief 取发送者与接收者之间的路径损耗
        /// @param _tx 发送者引用
        /// @param _rx 接收者引用
        /// @return 路径损耗（单位：dB）（纯链路损耗，注意与总链路损耗相区分）
        /// @see GetLinkLossDB
        double GetPathLossDB(Tx& _tx, RIS& _ris);
        double GetPathLossDB(RIS& _ris, Rx& _rx);

        double GetLoSAoDRAD(Tx& _tx, RIS& _ris);
        double GetLoSEoDRAD(RIS& _ris, Rx& _rx);
        double GetLoSAoARAD(Tx& _tx, RIS& _ris);
        double GetLoSEoARAD(RIS& _ris, Rx& _rx);

        /// RIS相关链路初始化
        void Initialize();
        void Initialize(MS& _ms);
        /// @brief 重置函数，清空本类所持有的所有链路
        void Reset();
        /// @brief 时间驱动的接口函数，将所有链路更新到输入参数所指定的时间点
        /// @param _dTimeSec 信道状态的时间点，（单位：秒）
//        void WorkSlot(double _dTimeSec);
//        /// @brief 时间驱动的接口函数，将指定接收者对应的所有链路更新到输入参数所指定的时间点
//        /// @param _rx 接收者引用
//        /// @param _dTimeSec 信道状态的时间点，（单位：秒）
//        void WorkSlot(RIS& _ris, double _dTimeSec);
//        /// @brief 返回计算路径损耗的函数
//        /// @param _tx 发送者引用
//        /// @param _rx 接收者引用
//        /// @return 一个可以计算路径损耗的智能指针
        std::shared_ptr<PathLoss> GetPathLossFun(Tx& _tx, RIS& _ris, bool m_bIsLOS);
        std::shared_ptr<PathLoss> GetPathLossFun(RIS& _ris, Rx& _rx, bool m_bIsLOS);
        void SetPos2Din(Tx& _tx, Rx& _rx);
        double GetPos2Din(std::pair<int, int> & _pos);

    private:
        /// 配合实现“单件”模式的静态指针变量
        static RISMatrix* m_pRM;
    public:
        /// @brief 通过该静态函数返回唯一的链路矩阵对象
        static RISMatrix& Instance();
    private:
        /// @brief 构造函数
        RISMatrix();
        /// @brief 拷贝构造函数
        RISMatrix(const RISMatrix&);
        /// @brief 赋值构造函数
        RISMatrix & operator=(const RISMatrix&);
        /// @brief 析构函数
        ~RISMatrix();
    };
}