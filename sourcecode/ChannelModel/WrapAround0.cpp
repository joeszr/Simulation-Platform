/// COPRIGHT NOTICE
/// Copyright (c) 2010,
/// All right reserved.
///
///@file WrapAround0.cpp
///@brief 实现一个小区结构的函数
///
///@author wangfei
#include "Point.h"
#include "WrapAround0.h"

using namespace cm;

///keep the UE position fixed, calculate the mapped BS position.
///@see WrapAround::WrapTx()
Point WrapAround0::WrapTx( const Point& _ue,const Point& _bs ){
    return _bs;
}
///keep the BS position fixed, calculate the mapped UE position.
///@see WrapAround::WrapRx()
Point WrapAround0::WrapRx( const Point& _ue,const Point& _bs ){
    return _ue;
}
///keep the UE position fixed, calculate the mapped BS position of different wrapping position.
///@see WrapAround::WrapTxi()
Point WrapAround0::WrapTxi( const Point& _bs,int i ){
    return _bs;
}