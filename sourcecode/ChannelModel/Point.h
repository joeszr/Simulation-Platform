/// COPRIGHT NOTICE
/// Copyright (c) 2010,
/// All right reserved.
///
///@file Point.h
///@brief 定义了point类，及一些获取坐标的函数
///@author wangfei

#pragma once
#include "libfiles.h"

namespace cm{
    /// @brief 点值类，基于STL库的pair类型实现
    class Point : private std::pair<double, double>{
    private:
        /// 点的移动速度，单位mps
        std::complex<double> m_SpeedMPS; 
    public:
        /// @brief 返回x坐标
        double GetX()const{
            return first;
        } 
        /// @brief 返回y坐标
        double GetY()const{
            return second;
        }
        /// @brief 设置x坐标
        void SetX( double _x ){
            first = _x;
        } 
        /// @brief 设置y坐标
        void SetY( double _y ){
            second = _y;
        } 
        /// @brief 同时设置x和y坐标
        void SetXY( double _x,double _y ){
            first = _x;second = _y;
        }
        /// @brief 返回移动速度矢量，单位mps
        std::complex<double> GetSpeedMPS()const{
            return m_SpeedMPS;
        }
        /// @brief 返回移动速率，单位mps.
        double GetVelocityMPS()const{
            return std::abs( m_SpeedMPS );
        }
        /// @brief 设置移动速率矢量，单位mps
        void SetSpeed( double _dXMPS,double _dYMPS ){
            m_SpeedMPS = std::complex<double>( _dXMPS, _dYMPS );
        };
        /// @brief 设置移动速度，单位mps
        void SetSpeed( std::complex<double> _cSpeedMPS ){
            m_SpeedMPS = _cSpeedMPS;
        };
        /// @brief 返回移动方向，单位弧度
        double GetMoveDirecRAD()const{
            return std::arg( m_SpeedMPS );
        }
        /// @brief 返回两个点值之间的距离
        friend double Distance( const Point& _p1,const Point& _p2 );
        /// @brief 将两个点值矢量相加
        friend Point operator +( const Point& _p1,const Point& _p2 );
        /// @brief 将两个点值矢量相减
        friend Point operator -( const Point& _p1,const Point& _p2 ); 
    public:
        /// @brief 构造函数
        explicit Point( double _x = 0,double _y = 0 );
        /// @brief 析构函数
        virtual ~Point()=default;
    };

    /// @brief 返回两个点值之间的距离
    double Distance( const Point& _p1,const Point& _p2 );
    /// @brief 将两个点值矢量相加
    Point operator +( const Point& _p1,const Point& _p2 );
    /// @brief 将两个点值矢量相减
    Point operator -( const Point& _p1,const Point& _p2 );
}
