#include "RISRx.h"
#include "AntennaOrientGain.h"
#include "P.h"
#include "../Parameters/Parameters.h"

using namespace cm;

///构造函数的实现
RISRxNode::RISRxNode(Point& _point) :  Rx(_point){
    m_iSpecial = 0;
    m_iIsLowloss = -1;
    m_iFloorNum = 0;
    m_iTotalFloorNum = 0;
    m_dInCarLossDB=0;
}

RISRxNode::RISRxNode(const RISRxNode& _risrx) : Rx(_risrx){
    *this = _risrx;
}

RISRxNode::~RISRxNode(void) {
}


