#pragma once

#include "libfiles.h"

#include "AntennaPanel.h"
#include "../Utility/Random.h"

namespace cm {    
    class Tx;
    class Rx;
    
    class Antenna : public std::enable_shared_from_this<Antenna> {
        Random random;
    public:
        std::vector< std::shared_ptr<AntennaPanel> > m_vAntennaPanels;
        

        std::vector<double> m_vEtiltRAD;    // V
        std::vector<double> m_vEscanRAD;    // H
        
        double m_TxRxOrientRAD;
        double m_MechanicalTiltRAD;
        
    public:        
        void SetTxRxOrientRAD(double _TxRxOrientRAD) {
            m_TxRxOrientRAD = _TxRxOrientRAD;
        }
        
    public:
        std::vector< std::shared_ptr<AntennaPanel> >& GetvAntennaPanels() {
            return m_vAntennaPanels;
        }
        
        std::shared_ptr<AntennaPanel> GetFirstAntennaPanelPointer() {
            return m_vAntennaPanels[0];
        }
        
    public:
        bool SelfCheck ();
        
    public:
        int GetTotalAntennaPanel_Num() const {
            return m_vAntennaPanels.size();
        }
        
        int GetTotalTXRU_Num() const;
        
        int GetActiveTXRU_Num() const {
            return m_ActiveTXRU_Num;
        }

        int m_ActiveTXRU_Num;
        
    public:
        int Get_V_BeamIndex(int _BeamIndex) const;
        
        int Get_H_BeamIndex(int _BeamIndex) const;
        
        int Get_CombBeamIndex(int _V_BeamIndex, int _H_BeamIndex) const;
        
        int GetRandomBeamIndex();///这里删除const
        
    public:
        int Get_V_BeamNum() const;
        int Get_H_BeamNum() const;
        
//        double GetEtiltRAD(int _V_BeamIndex) const;
//        double GetEscanRAD(int _H_BeamIndex) const;
        
        double GetEtiltRAD(int _BeamIndex) const;
        double GetEscanRAD(int _BeamIndex) const;
        
        double GetTxRxOrientRAD() const {
            return m_TxRxOrientRAD;
        }
        
        double GetMechanicalTiltRAD() const {
            return m_MechanicalTiltRAD;
        }
                
    public:
        Antenna(int _AntennaPanelNum, int _EtiltRADNum, int _EscanRADNum);
        virtual ~Antenna() = default;
        
    protected:
        
        friend class Tx;
        friend class Rx;
    };
    
}

