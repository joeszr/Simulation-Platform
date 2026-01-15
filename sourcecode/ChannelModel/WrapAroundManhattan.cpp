///@file WrapAroundManhattan.cpp
///@brief 实现实现曼哈顿模型拓扑结构的函数
///@author wangfei

#include "Point.h"
#include "WrapAroundManhattan.h"

using namespace cm;

///keep the UE position fixed, calculate the mapped BS position.
///@see WrapAround::WrapTx()
Point WrapAroundManhattan::WrapTx(const Point& _ue, const Point& _bs) {
    return _bs; //error
}

///keep the BS position fixed, calculate the mapped UE position.
///@see WrapAround::WrapRx()
Point WrapAroundManhattan::WrapRx(const Point& _ue, const Point& _bs) {
    return _ue; //error
}

///keep the UE position fixed, calculate the mapped BS position of different wrapping position.
Point WrapAroundManhattan::WrapTxi( const Point& _bs,int i ){
    return _bs;
}