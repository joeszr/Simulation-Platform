/// COPRIGHT NOTICE
/// Copyright (c) 2010,
/// All right reserved.
///
///@file Rx.h
///@brief 定义了和MS相关的一些函数
///
///@author wangfei
#pragma once
#include "Point.h"
#include "Antenna.h"
#include "../SafeUnordered_map.h"
#include "../Utility/Random.h"
#include "../SafeMap.h"
namespace cm{
    class AntennaOrientGain;
    ///Rx class
    class Rx : public Point{
    protected:
        Random random;
        ///RX的ID
        int m_iRxID;
        ///0---Outdoor,1---Indoor,2---Incar
        int m_iSpecial;
        int m_iIsLowloss;
        int m_iTotalFloorNum;
        int m_iFloorNum;
        double m_dRxHeight;
        double m_dLOSEODDeg;
        double m_dLOSEOADeg;
        double m_dLOSAODDeg;
        double m_dLOSAOADeg;
        ///Rx方向角
        double m_dRxOrientRAD; 
        int m_iAntNum; 
        double m_dInCarLossDB;

        int static rxcounter;
        static SafeMap<int,Rx*> m_mRxID2PRx;
    public:
        std::shared_ptr<cm::Antenna> m_pAntenna;
        std::shared_ptr<cm::AntennaOrientGain> m_pRxAOG;
        double m_dRxAntGainDB;

    private:
        int type;//1 for origin, 2用于另一套天线参数    
    public:
        std::shared_ptr<cm::Antenna> GetAntennaPointer() {
            return m_pAntenna;
        }
        int gettype() const{
        return type;
        }

    public:
        ///获取Rx的ID
        int GetRxID() const{
            return m_iRxID;
        };
        ///获取Rx方向角
        double GetRxOrientRAD() const{
            return m_dRxOrientRAD;
        };
        ///设置Rx的方向角
        void SetRxOrientRAD( double _rad );
        /// @brief 取接收器的天线高度
        /// @return 返回值单位：m
        double GetRxHeightM() const{
            return m_dRxHeight;
        };
        /// @brief 设置接收器的天线高度
        /// @param 单位：m
        void SetRxHeightM(double _dHeightM) {
            m_dRxHeight = _dHeightM;
        };
        /// @brief 取UE所处的层数
        int GetRxFloorNum() const{
            return m_iFloorNum;
        };
        /// @brief 设置接收器的天线高度
        /// @param 单位：m
        void SetRxFloorNum(int _iFloorNum) {
            m_iFloorNum = _iFloorNum;
        };
        ///
        int GetRxTotalFloorNum() const{
            return m_iTotalFloorNum;
        };
        ///
        void SetRxTotalFloorNum(int _iTotalFloorNum) {
            m_iTotalFloorNum = _iTotalFloorNum;
        };
        ///
        double GetLOSEODDeg() const{
            return m_dLOSEODDeg;
        };
        ///
        void SetLOSEODDeg(double _dLOSEODDeg) {
            m_dLOSEODDeg = _dLOSEODDeg;
        };
        ///获取天线数目
        int GetAntNum() const{
            return m_iAntNum;
        };
        ///获取Rx的天线方向性增益
//        double GetRxAOGDB( double _dAngleRAD);
        double GetRxAOGDB( double _dAngleRAD ,double _dDownTiltRAD)const;
        ///获取special的值
        int GetSpecial() const{
            return m_iSpecial;
        };

        int IsLowloss() const{
            return m_iIsLowloss;
        };
        ///获取incar下的损耗
        double GetInCarLossDB() const{
            return m_dInCarLossDB;
        };

        ///记录Rx的数目
        static int CountRx();
        ///获得Rx的引用
        static Rx& GetRx(int _iRxID);
        
    protected:
        void Build_UE_Antenna();
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
        ///Rx的构造函数
        Rx();
        explicit Rx(int _type);
        ///RX的拷贝构造函数
        Rx( const Rx& _rx );

         Rx( Point & _point);


        ///虚析构函数
        ~Rx() override;
    };
}