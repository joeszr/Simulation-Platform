#include "RISTx.h"
#include "AntennaOrientGain.h"
#include "P.h"
#include "../Parameters/Parameters.h"

using namespace cm;

///构造函数的实现
RISTxNode::RISTxNode(Point& _point) :  Tx(_point){

}

RISTxNode::RISTxNode(const RISTxNode& _ristx) : Tx(_ristx){
    *this = _ristx;
}

RISTxNode::~RISTxNode(void) {
}

