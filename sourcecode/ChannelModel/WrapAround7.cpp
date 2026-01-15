///@file WrapAround7.cpp
///@brief 实现实现7小区拓扑结构的函数
///@author wangfei
#include "Point.h"
#include "WrapAround7.h"
#include "../Parameters/Parameters.h"

using namespace cm;

WrapAround7::WrapAround7() {
    //    //
    //    m_p[0] = Point(0, 0);
    //    m_p[1] = Point(2.5 * P::s().MacroTX.DSiteDistanceM, 0.5 * M_SQRT3 * P::s().MacroTX.DSiteDistanceM);
    //   // m_p[1] = Point(3.0 * P::s().MacroTX.DSiteDistanceM, 2.0 * M_SQRT3 * P::s().MacroTX.DSiteDistanceM);
    //    m_p[2] = Point(0.5 * P::s().MacroTX.DSiteDistanceM, 1.5 * M_SQRT3 * P::s().MacroTX.DSiteDistanceM);
    //    m_p[3] = Point(-2.0 * P::s().MacroTX.DSiteDistanceM, 1.0 * M_SQRT3 * P::s().MacroTX.DSiteDistanceM);
    //    m_p[4] = Point(-2.5 * P::s().MacroTX.DSiteDistanceM, -0.5 * M_SQRT3 * P::s().MacroTX.DSiteDistanceM);
    //    m_p[5] = Point(-0.5 * P::s().MacroTX.DSiteDistanceM, -1.5 * M_SQRT3 * P::s().MacroTX.DSiteDistanceM);
    //    m_p[6] = Point(2.0 * P::s().MacroTX.DSiteDistanceM, -1.0 * M_SQRT3 * P::s().MacroTX.DSiteDistanceM);
    //    //m_p[7] = Point(4.5 * P::s().MacroTX.DSiteDistanceM, -0.5 * M_SQRT3 * P::s().MacroTX.DSiteDistanceM);
    double r = Parameters::Instance().Macro.DCellRadiusM;
    m_p[0] = Point(0, 0);
    m_p[1] = Point(2 * r*M_SQRT3, 3 * r);
    m_p[2] = Point(r *-0.5 * M_SQRT3, 4.5 * r);
    m_p[3] = Point(-2.5 * r *M_SQRT3, 1.5 * r);
    m_p[4] = Point(-2 * r *M_SQRT3, -3 * r);
    m_p[5] = Point(0.5 * r *M_SQRT3, -4.5 * r);
    m_p[6] = Point(2.5 * r * M_SQRT3, -1.5 * r);
}
///keep the UE position fixed, calculate the mapped BS position.
///@see WrapAround::WrapTx()

Point WrapAround7::WrapTx(const Point& _ue, const Point& _bs) {
    int k = 0;
    double d, dmin = Distance(_ue, _bs);

    for (int i = 1; i < 7; ++i) {
        //
        d = Distance(_ue, _bs + m_p[i]);
        if (d < dmin) {
            dmin = d;
            k = i;
        }
    }

    //    if (k == -1) {
    //        return _bs;
    //    } else {
    return _bs + m_p[k];
    //    }
}

///keep the BS position fixed, calculate the mapped UE position.
///@see WrapAround::WrapRx()

Point WrapAround7::WrapRx(const Point& _ue, const Point& _bs) {
    int k = 0;
    double d, dmin = Distance(_ue, _bs);

    for (int i = 1; i < 7; ++i) {
        //
        d = Distance(_ue + m_p[i], _bs);
        if (d < dmin) {
            dmin = d;
            k = i;
        }
    }

    //    if (k == -1) {
    //        return _ue;
    //    } else {
    return _ue + m_p[k];
    //    }
}

///keep the UE position fixed, calculate the mapped BS position of different wrapping position.
///@see WrapAround::WrapTxi()

Point WrapAround7::WrapTxi(const Point& _bs, int i) {
    return _bs + m_p[i];
}