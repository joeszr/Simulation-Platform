///20171219

#include "PathLossRuralMacroNLOS_Highfreq.h"
#include "P.h"
#include "../Parameters/Parameters.h"

using namespace cm;

double PathLossRuralMacroNLOS_ModeB::Db(double _dDisM_3D, double _dUEHeightM) {
    // assert( P::s().MacroTX.DAntennaHeightM == 35 );
    assert(P::s().RX.DAntennaHeightM == 1.5);


    double dPL_LOS_ModelB = PL_RMa_LOS_ModelB.Db(_dDisM_3D, _dUEHeightM);

    const static double dAveBuildHeightM = 5;
    const static double dStreetWidthM = 20;
    double tempa = 161.04 - 7.1 * std::log10(dStreetWidthM)
            + 7.5 * std::log10(dAveBuildHeightM)
            - (24.37 - 3.7 * pow(dAveBuildHeightM / P::s().MacroTX.DAntennaHeightM, 2.0))
            * std::log10(P::s().MacroTX.DAntennaHeightM)
            + 20 * std::log10(P::s().FX.DRadioFrequencyMHz_Macro / 1e3)
            - 3.2 * pow(std::log10(11.75 * _dUEHeightM), 2.0)
            + 4.97;
    double dPL_NLOS_dot_ModelB = -1 * (tempa + (43.42 - 3.1 * std::log10(P::s().MacroTX.DAntennaHeightM))
            * (std::log10(_dDisM_3D) - 3));

    double dPLb;
    if (Parameters::Instance().BASIC.ISubScenarioModel ==
            Parameters::SCENARIO_Phase1_RMA_LMLC) {
        dPLb = std::min(dPL_LOS_ModelB, dPL_NLOS_dot_ModelB + 12.0);
    } else {
        dPLb = std::min(dPL_LOS_ModelB, dPL_NLOS_dot_ModelB);
    }
    return dPLb;
}

