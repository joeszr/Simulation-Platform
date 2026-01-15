///@file WrapAround.cpp
///@brief 实现WrapAround功能的函数
///@author wangfei

#include "Point.h"
#include "WrapAround0.h"
#include "WrapAround7.h"
#include "WrapAround19.h"
#include "WrapAround.h"

using namespace cm;

WrapAround* WrapAround::m_pWA = nullptr;

///@brief return the unique object of WrapAround.
///
///可以选择不同的WrapAround类型
WrapAround& WrapAround::Instance(){
    if(!m_pWA){
        switch( P::s().IMacroTopology ){
        case P::HEXAGONAL7CELL:
            m_pWA = new WrapAround7();break;
        case P::HEXAGONAL19CELL:
            m_pWA = new WrapAround19();break;
        default:
            m_pWA = new WrapAround0();break;
        }
    }
    return *m_pWA;
}
