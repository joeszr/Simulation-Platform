///20171219

#include "PathLossRuralMacroLOS_Highfreq.h"
#include "P.h"


using namespace cm;

///计算DB值
//

// 20171204
double PathLossRuralMacroLOS_ModeB::Db( double _dDisM_3D , double _dUEHeightM){
    assert( P::s().MacroTX.DAntennaHeightM == 35 );
    assert( P::s().RX.DAntennaHeightM == 1.5 );

    double PL_RMa_LOS_ModelB = PL_RMa_LOS.Db(_dDisM_3D,_dUEHeightM);
    return PL_RMa_LOS_ModelB;
}