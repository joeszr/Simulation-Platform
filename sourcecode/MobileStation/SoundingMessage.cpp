#include "../Utility/IDType.h"
#include "../Utility/RBID.h"
#include "MSID.h"
#include "../Utility/functions.h"
#include "../NetworkDrive/Clock.h"
#include "SoundingMessage.h"

SoundingMessage::SoundingMessage(MSID _msid):m_mH(Parameters::Instance().BASIC.IRBNum) {
    m_iBornTime = Clock::Instance().GetTimeSlot();
    m_MSID = _msid;
}

cmat SoundingMessage::GetH(RBID _RBID) {
    return m_mH[_RBID.ToInt()];
}

void SoundingMessage::SetH(const RBID& _RBID, cmat _mH,int _iAnt) {
    m_iAntennaID = _iAnt;
    m_mH[_RBID.ToInt()] = _mH.get_rows(_iAnt,_iAnt);
}

void SoundingMessage::SetH(const RBID& _RBID, cmat _mH) {
    m_iAntennaID = 2;
    m_mH[_RBID.ToInt()] = _mH;
}

int SoundingMessage::GetBornTime()const{
    return m_iBornTime;
}

int SoundingMessage::GetAntennaID()const{
    return m_iAntennaID;
}

MSID SoundingMessage::GetMSID(){
    return m_MSID;
}