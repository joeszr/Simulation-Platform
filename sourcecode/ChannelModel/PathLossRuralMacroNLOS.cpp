///@file PathLossRuralMacroNLOS.cpp
///@brief  PathLossRuralMacroNLOS类定义
///@author wangfei
#include "../Parameters/Parameters.h"
#include "P.h"
#include "PathLossRuralMacroNLOS.h"

using namespace cm;

///计算DB值
double PathLossRuralMacroNLOS_ModeA::Db(double _dDisM_3D, double _dUEHeightM) {
    //    assert( P::s().MacroTX.DAntennaHeightM == 35 );
    assert(P::s().RX.DAntennaHeightM == 1.5);

    const static double dAveBuildHeightM = 5;
    const static double dStreetWidthM = 20;
    double tempa = 161.04 - 7.1 * std::log10(dStreetWidthM)
            + 7.5 * std::log10(dAveBuildHeightM)
            - (24.37 - 3.7 * pow(dAveBuildHeightM / P::s().MacroTX.DAntennaHeightM, 2.0))
            * std::log10(P::s().MacroTX.DAntennaHeightM)
            + 20 * std::log10(P::s().FX.DRadioFrequencyMHz_Macro / 1e3)
            - 3.2 * pow(std::log10(11.75 * _dUEHeightM), 2.0)
            + 4.97;
    double result_PL_NLOS = -1 * (tempa + (43.42 - 3.1 * std::log10(P::s().MacroTX.DAntennaHeightM))
            * (std::log10(_dDisM_3D) - 3));

    if (Parameters::Instance().BASIC.ISubScenarioModel ==
            Parameters::SCENARIO_Phase1_RMA_LMLC) {
        double result_PL_dot_NLOS = result_PL_NLOS;
        double restlt_PL_RMa_LOS = PL_LOS.Db(_dDisM_3D, _dUEHeightM);
        return std::min(restlt_PL_RMa_LOS, result_PL_dot_NLOS + 12);
    } else {
        return result_PL_NLOS;
    }
}