///@file WrapAround19.cpp
///@brief 实现实现19小区拓扑结构的函数
///@author wangfei
#include "Point.h"
#include "WrapAround19.h"
#include "../Utility/Include.h"
using namespace cm;

///Wrapround19类的构造函数的实现
WrapAround19::WrapAround19() {
    m_p[0] = Point(3.5 * P::s().MacroTX.DSiteDistanceM, 1.5 * M_SQRT3 * P::s().MacroTX.DSiteDistanceM);
    m_p[1] = Point(3.0 * P::s().MacroTX.DSiteDistanceM, 4.0 * M_SQRT3 * P::s().MacroTX.DSiteDistanceM);
    m_p[2] = Point(-0.5 * P::s().MacroTX.DSiteDistanceM, 2.5 * M_SQRT3 * P::s().MacroTX.DSiteDistanceM);
    m_p[3] = Point(-4.0 * P::s().MacroTX.DSiteDistanceM, 1.0 * M_SQRT3 * P::s().MacroTX.DSiteDistanceM);
    m_p[4] = Point(-3.5 * P::s().MacroTX.DSiteDistanceM, -1.5 * M_SQRT3 * P::s().MacroTX.DSiteDistanceM);
    m_p[5] = Point(0.5 * P::s().MacroTX.DSiteDistanceM, -2.5 * M_SQRT3 * P::s().MacroTX.DSiteDistanceM);
    m_p[6] = Point(4.0 * P::s().MacroTX.DSiteDistanceM, -1.0 * M_SQRT3 * P::s().MacroTX.DSiteDistanceM);
    m_p[7] = Point(7.5 * P::s().MacroTX.DSiteDistanceM, 0.5 * M_SQRT3 * P::s().MacroTX.DSiteDistanceM);
}

///keep the UE position fixed, calculate the mapped BS position.的坐标
///@see WrapAround::WrapTx()
Point WrapAround19::WrapTx(const Point& _ue, const Point& _bs) {
    int k = -1;
    double d, dmin = Distance(_ue, _bs);

    for (int i = 0; i < 8; ++i) {
        //
        d = Distance(_ue, _bs + m_p[i]);
        if (d < dmin) {
            dmin = d;
            k = i;
        }
    }

    if (k == -1) {
        return _bs;
    } else {
        return _bs + m_p[k];
    }
}

///keep the BS position fixed, calculate the mapped UE position.
///@see WrapAround::WrapRx()
Point WrapAround19::WrapRx(const Point& _ue, const Point& _bs) {
    int k = -1;
    double d, dmin = Distance(_ue, _bs);

    for (int i = 0; i < 8; ++i) {
        //
        d = Distance(_ue + m_p[i], _bs);
        if (d < dmin) {
            dmin = d;
            k = i;
        }
    }

    if (k == -1) {
        return _ue;
    } else {
        return _ue + m_p[k];
    }
}

///keep the UE position fixed, calculate the mapped BS position of different wrapping position.
///@see WrapAround::WrapTxi()
Point WrapAround19::WrapTxi( const Point& _bs,int i ){
    return _bs + m_p[i];
}