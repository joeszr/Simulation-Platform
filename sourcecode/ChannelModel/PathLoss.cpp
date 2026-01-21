///@file PathLoss.cpp
///@brief  PathLoss类定义
///@author wangfei

#include "PathLoss.h"
#include "functions.h"

using namespace cm;

// 构造函数实现
PathLoss::PathLoss(void) {
}

// 析构函数实现（纯虚析构函数也需要实现）
PathLoss::~PathLoss() {
}

double PathLoss::Linear( double _dDisM, double _dUEHeightM){
    return DB2L( this->Db( _dDisM,_dUEHeightM ) );
}