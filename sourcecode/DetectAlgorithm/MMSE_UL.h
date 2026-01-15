///@author zhangyong

#include "./Detector_UL.h"

class MMSE_UL : public Detector_UL {
public:
    ///@brief 根据理论公式计算SINR
    ///
    ///当计算公共参考信号的SINR时采用下面的加权向量
    ///itpp::cmat v = std::sqrt(1.0 / iBSAntNum / iRank) * itpp::ones_c(iBSAntNum, iRank);
    ///@param _mS 有用信号的符号
    ///@param _mH 有用信号经过的信道矩阵（包括大尺度和小尺度）
    ///@param _mP 有用信号的PMI
    ///@param _vSIS 所有强干扰的发送信号
    ///@param _vSIH 所有强干扰的发送信号经过的信道矩阵包括大尺度和小尺度）
    ///@param _vSIP 所有强干扰的PMI
    ///@param _vWIS 所有弱干扰的发送信号
    ///@param _vWIL 所有弱干的大尺度
    ///@return  返回一个SINR的矩阵
    mat CalculateSINR(const cmat& _mS, const cmat& _mH, const cmat& _mP,
            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
            const vector<double>& _vWIL) override;

    //    mat CalculateSINR(const cmat& _mS, const cmat& _mH, const cmat& _mP, const cmat& _mPA,
    //            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
    //            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
    //            const vector<double>& _vWIL);

    mat CalculateIoT(const cmat& _mS, const cmat& _mH, const cmat& _mP,
            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
            const vector<double>& _vWIL) override; //add by zhengyi

    itpp::cmat CalculateDetecMatrix(const cmat& _mS, const cmat& _mH, const cmat& _mP,
            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
            const vector<double>& _vWIL) override; //add by zhengyi

public:
    MMSE_UL() = default;
    ~MMSE_UL() override = default;
};

