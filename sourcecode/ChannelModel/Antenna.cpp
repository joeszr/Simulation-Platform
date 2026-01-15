/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


#include "Antenna.h"
#include "../Parameters/Parameters.h"

using namespace cm;

Antenna::Antenna(int _AntennaPanelNum, int _EtiltRADNum, int _EscanRADNum) {

    m_vAntennaPanels.resize(
            _AntennaPanelNum, std::shared_ptr<AntennaPanel>());

    m_vEtiltRAD.resize(_EtiltRADNum, 0.0);
    m_vEscanRAD.resize(_EscanRADNum, 0.0);
}

int Antenna::Get_V_BeamNum() const {
    return m_vEtiltRAD.size();
}

int Antenna::Get_H_BeamNum() const {
    return m_vEscanRAD.size();
}

int Antenna::GetTotalTXRU_Num() const {
    assert(GetTotalAntennaPanel_Num() > 0);

    int TotalTXRUNum = 0;

    BOOST_FOREACH(std::shared_ptr<AntennaPanel> pAntennaPanel, m_vAntennaPanels) {
        TotalTXRUNum += pAntennaPanel->GetTXRU_Num();
    }
    return TotalTXRUNum;
}

int Antenna::Get_V_BeamIndex(int _BeamIndex) const {
    int _V_BeamIndex = _BeamIndex / Get_H_BeamNum();

    return _V_BeamIndex;
}

int Antenna::Get_H_BeamIndex(int _BeamIndex) const {
    int _H_BeamIndex = _BeamIndex % Get_H_BeamNum();

    return _H_BeamIndex;
}

int Antenna::Get_CombBeamIndex(int _V_BeamIndex, int _H_BeamIndex) const {
    int _CombBeamIndex = _V_BeamIndex * Get_H_BeamNum() + _H_BeamIndex;

    return _CombBeamIndex;
}

int Antenna::GetRandomBeamIndex() {///这里删除const
    int _BeamIndex = random.xUniformInt(0,
            Get_V_BeamNum() * Get_H_BeamNum() - 1);

    return _BeamIndex;
}

double Antenna::GetEtiltRAD(int _BeamIndex) const {
    int _V_BeamIndex = Get_V_BeamIndex(_BeamIndex);

    assert(_V_BeamIndex >= 0 && _V_BeamIndex < Get_V_BeamNum());

    return m_vEtiltRAD[_V_BeamIndex];
}

double Antenna::GetEscanRAD(int _BeamIndex) const {
    int _H_BeamIndex = Get_H_BeamIndex(_BeamIndex);

    assert(_H_BeamIndex >= 0 && _H_BeamIndex < Get_H_BeamNum());

    return m_vEscanRAD[_H_BeamIndex];
}

bool Antenna::SelfCheck() {
    bool check_result = true;

    for (int i = 0; i < this->GetTotalAntennaPanel_Num(); i++) {
        std::shared_ptr<AntennaPanel> pAntennaPanel =
                m_vAntennaPanels[i];

        if (i != pAntennaPanel->GetPanelIndex()) {
            check_result = false;
            return check_result;
        }
    }

    std::vector<int> vTXRU_Indexes;
    int TotalTXRU_Num = GetTotalTXRU_Num();
    vTXRU_Indexes.resize(TotalTXRU_Num, -1);

    int i = 0;

    BOOST_FOREACH(std::shared_ptr<AntennaPanel> pAntennaPanel,
            GetvAntennaPanels()) {

        BOOST_FOREACH(std::shared_ptr<CTXRU> pTXRU,
                pAntennaPanel->GetvTXRUs()) {
            vTXRU_Indexes[i++] = pTXRU->GetTXRUIndex();
        }
    }

    std::sort(vTXRU_Indexes.begin(), vTXRU_Indexes.end());
    vTXRU_Indexes.erase(
            unique(vTXRU_Indexes.begin(), vTXRU_Indexes.end()),
            vTXRU_Indexes.end());

    if ((vTXRU_Indexes.size() != TotalTXRU_Num) ||
            (vTXRU_Indexes[0] != 0) ||
            (vTXRU_Indexes[vTXRU_Indexes.size() - 1] != TotalTXRU_Num - 1)) {
        check_result = false;
        return check_result;
    }


    return check_result;
}

