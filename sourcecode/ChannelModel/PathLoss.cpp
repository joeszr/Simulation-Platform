///@file PathLoss.cpp
///@brief  PathLoss类定义
///@author wangfei

#include "PathLoss.h"
#include "functions.h"

using namespace cm;

double PathLoss::Linear( double _dDisM, double _dUEHeightM){
    return DB2L( this->Db( _dDisM,_dUEHeightM ) );
}