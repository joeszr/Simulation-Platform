/// COPRIGHT NOTICE
/// Copyright (c) 2010,
/// All right reserved.
///
///@file Point.h
///@brief 定义了point类，及一些获取坐标的函数
///@author wangfei

#include "Point.h"
using namespace cm;

Point::Point(double _x, double _y) : std::pair<double, double>(_x, _y) {
}

double cm::Distance(const Point& _p1, const Point& _p2) {
    return sqrt(pow(_p1.GetX() - _p2.GetX(), 2.0) + pow(_p1.GetY() - _p2.GetY(), 2.0));
}

Point cm::operator +(const Point& _p1, const Point& _p2) {
    return Point(_p1.GetX() + _p2.GetX(), _p1.GetY() + _p2.GetY());
}

Point cm::operator -(const Point& _p1, const Point& _p2) {
    return Point(_p1.GetX() - _p2.GetX(), _p1.GetY() - _p2.GetY());
}