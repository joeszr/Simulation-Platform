/// @file MRC.h
/// @author zhangyong

#pragma once
#include "../Utility/Include.h"
#include "../DetectAlgorithm/Detector_DL.h"

class MRC_DL : public Detector_DL {
public:
    itpp::mat CalculateSINR(const cmat& _mS, const cmat& _mH, const cmat& _mP,
            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
            const vector<double>& _vWIL) override;
public:
    MRC_DL() = default;
    ~MRC_DL() override = default;
};

