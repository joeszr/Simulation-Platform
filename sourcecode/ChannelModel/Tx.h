///@file Tx.h
///@brief  Tx类声明
///
#pragma once
#include "./Point.h"
#include "../SafeUnordered_map.h"
#include "Antenna.h"

namespace cm {
    class AntennaOrientGain;
    /// @brief 发射器基类

    class Tx : public Point {
    public:
        int m_iTxID;
        double m_dTxHeight;
        double m_dTxOrientRAD;
        int m_iAntNum;
        double m_dTxAntGainDB;
        std::shared_ptr<cm::AntennaOrientGain> m_pTxAOG;
        static int txcounter;
        static std::unordered_map<int, Tx*> m_mTxID2PTx;

        std::shared_ptr<cm::Antenna> m_pAntenna;
        
    public:
        std::shared_ptr<cm::Antenna> GetAntennaPointer() {
            return m_pAntenna;
        }
        
    public:
        /// @brief 取发射器的ID序号
        int GetTxID() const;
        /// @brief 取发射器的天线朝向
        /// @return 返回值单位：弧度
        double GetTxOrientRAD() const;
        /// @brief 设置发射器的天线朝向
        /// @param _dRAD 发射器天线朝向，单位：弧度
        void SetTxOrientRAD(double _dRAD);
        /// @brief 取发射器的天线高度
        /// @return 返回值单位：m
        double GetTxHeightM() const;
        /// @brief 设置发射器的天线高度
        /// @param 单位：m
        void SetTxHeightM(double _dHeightM);
        /// @brief 取发射器天线数
        int GetAntNum() const;
        /// @brief 取天线对特定方向性的增益值
        /// @param _dAngleRAD 发射器与接收器连线与基站天线朝向之间的夹角
        /// @param _dDownTiltRad 发射器的天线下倾角
        /// @return DB值表示的天线增益值
        double GetTxAOGDB(double _dAngleRAD, double _dDownTiltRad);

        void GetRISID();

        /// @brief 返回发射器的总对象个数
        static int CountTx();
        /// @brief 根据发射器ID返回发射器的引用
        static Tx& GetTx(int _iTxID);
        
    protected:
        void Build_BS_Antenna();
        //chty 1111 b
    private:
        int m_PannelNum;
        int m_HTxRUNum;
        int m_VTxRUNum;
        int m_TotalTxRUNum;
    public:
        int GetPannelNum() const {
            return m_PannelNum;
        }

        int GetHTxRUNum() const {
            return m_HTxRUNum;
        }

        int GetVTxRUNum() const {
            return m_VTxRUNum;
        }

        int GetTotalTxRUNum() const {
            return m_TotalTxRUNum;
        };
        //chty 1111 e
    public:
        /// @brief 构造函数
        Tx();
        /// @brief 拷贝构造函数
        Tx(const Tx& _tx);

        Tx(Point & _point);


        /// @brief 纯虚析构函数
        ~Tx() override;
    };
}