//
// Created by ChTY on 2022/10/7.
//

#include "MixedTraffic_MS.h"
#include "Universal_Packet.h"
#include "Packet_xrMULTI.h"
#include "../NetworkDrive/Clock.h"
#include "../NetworkDrive/NetWorkDrive.h"
#include "../MobileStation/MS.h"
#include "../BaseStation/BTS.h"
#include "../MobileStation/MSTxer.h"


extern int G_ICurDrop;
extern int G_PacketID;

std::mutex TrafficRecord_lock;
void MixedTraffic_MS::initialize(int _msid, bool _xrSingleOn, bool _xrMultiOn, bool _ftp3On, bool _fullbufferOn, bool _DatabaseOn){
    msid            = _msid;
    xrSingleOn      = _xrSingleOn;
    xrMultiOn       = _xrMultiOn;
    ftp3On          = _ftp3On;
    DatabaseOn      = _DatabaseOn;
    fullbufferOn    = _fullbufferOn;

    if(xrSingleOn)
    {
        xrSinglePara.initialize(Parameters::Instance().XR.SumRate);
    }

    if(xrMultiOn)
    {
        xrMultiPara.initialize(Parameters::Instance().XR.SumRate);
    }

    if(ftp3On)
    {
        ftp3Para.initialize(Parameters::Instance().TRAFFIC.DPacketSize, Parameters::Instance().TRAFFIC.DMacroLamda);
    }

    if(DatabaseOn)
    {
        DataBaseTrafficPara.initialize(msid);
    }
};

void MixedTraffic_MS::WorkSlot(){
    time = Clock::Instance().GetTimeSlot() + Parameters::Instance().BASIC.ISlotPerDrop * (G_ICurDrop - 1);
    time_ms = time * Parameters::Instance().BASIC.DSlotDuration_ms;
    if(xrSingleOn)
    {
        xrsingle(); //只有下行
    }
    if(xrMultiOn)
    {
        xrmulti();  //只有下行
    }
    if(ftp3On)
    {
        ftp3();
    }
    if(DatabaseOn)
    {
        DbTraffic();
    }
    if(fullbufferOn)
    {
        fullbuffer();
    }
}

void MixedTraffic_MS::xrsingle()
{
    if (time == xrSinglePara.next_arrive_time)
    {
        double dPeriod_slot = xrSinglePara.dPeriod_ms / Parameters::Instance().BASIC.DSlotDuration_ms;
        xrSinglePara.m_iFrameNo++;
        double dFrameSize_Kbit;
        double mean = (xrSinglePara.jittermax+xrSinglePara.jittermin)/2;
        double temp = random.xNormal(mean, xrSinglePara.jitterstd);//jitter
        temp=min(xrSinglePara.jittermax,temp);
        temp=max(xrSinglePara.jittermin,temp);
        temp /= Parameters::Instance().BASIC.DSlotDuration_ms;
        double dFrameSize_temp = random.xNormal(xrSinglePara.dPave, xrSinglePara.dS);
        dFrameSize_temp=min(dFrameSize_temp,xrSinglePara.dPmax);
        dFrameSize_temp=max(dFrameSize_temp,xrSinglePara.dPmin);
        dFrameSize_Kbit = dFrameSize_temp;
        Observer::Print("PacketCount") << setw(20) << "Slot" << setw(20) << time
                                       << setw(20) << "msid" << setw(20) << msid
                                       << setw(20) << "PacketSize" << setw(20) << dFrameSize_Kbit
                                       << endl;
        MSID MSid=MSID(msid);
        std::shared_ptr<Universal_Packet> pPacket = std::shared_ptr<Universal_Packet > (new Universal_Packet(Parameters::XRSinglePacket,G_PacketID++, dFrameSize_Kbit, time, 1, MSid, MSid.GetMS().GetMainServBTS(), xrSinglePara.m_originalbornslot),+
                                                                                       deleteUP);
        MSid.GetMS().GetMainServBTS().GetBTS().ReceivePacketDL(MSid, pPacket);
        MSid.GetMS().SetTrafficState(true);
        m_qTotalPacketQueue.push_back(pPacket);
        xrSinglePara.m_originalbornslot += dPeriod_slot;
        xrSinglePara.next_arrive_time = ceil(temp) + (int)xrSinglePara.m_originalbornslot;
        if (xrSinglePara.next_arrive_time < time) {
            cout << "invalid next_arrive_time";
            assert(false);
        }
        Statistician::Instance().m_MSData_DL[msid].Packetscounts++;
    }
}
void MixedTraffic_MS::xrmulti()
{
    if (time == xrMultiPara.next_arrive_time)
    {
        xrMultiPara.m_iFrameNo++;
        double mean = (xrSinglePara.jittermax+xrSinglePara.jittermin)/2;
        double temp = random.xNormal(mean, xrSinglePara.jitterstd);//jitter
        temp=min(xrSinglePara.jittermax,temp);
        temp=max(xrSinglePara.jittermin,temp);
        temp /= Parameters::Instance().BASIC.DSlotDuration_ms;
        int order=xrMultiPara.get_and_renew_order();
        double dFrameSize;
        if(order==0)
        {
            dFrameSize = random.xNormal(xrMultiPara.dIave, xrMultiPara.dIS);
            dFrameSize=min(dFrameSize,xrMultiPara.dImax);
            dFrameSize=max(dFrameSize,xrMultiPara.dImin);
            MSID MSid=MSID(msid);
            std::shared_ptr<Universal_Packet> pPacket = std::shared_ptr<Universal_Packet > (new Universal_Packet(Parameters::XRMultiI,G_PacketID++, dFrameSize, time, 0, MSid, MSid.GetMS().GetMainServBTS(),xrMultiPara.m_originalbornslot));
            MSid.GetMS().GetMainServBTS().GetBTS().ReceivePacketDL(MSid, pPacket);
            MSid.GetMS().SetTrafficState(true);
            m_qTotalPacketQueue.push_back(pPacket);
        }
        else
        {
            dFrameSize = random.xNormal(xrMultiPara.dPave, xrMultiPara.dPS);
            dFrameSize=min(dFrameSize,xrMultiPara.dPmax);
            dFrameSize=max(dFrameSize,xrMultiPara.dPmin);
            MSID MSid=MSID(msid);
            std::shared_ptr<Universal_Packet>   pPacket= std::shared_ptr<Universal_Packet > (new Universal_Packet(Parameters::XRMultiP,G_PacketID++, dFrameSize, time, 0, MSid, MSid.GetMS().GetMainServBTS(),xrMultiPara.m_originalbornslot));
            MSid.GetMS().GetMainServBTS().GetBTS().ReceivePacketDL(MSid, pPacket);
            MSid.GetMS().SetTrafficState(true);
            m_qTotalPacketQueue.push_back(pPacket);
        }
        Observer::Print("PacketCount") << setw(20) << "Slot" << setw(20) << time
                                       << setw(20) << "msid" << setw(20) << msid
                                       << setw(20) << "PacketSize" << setw(20) << dFrameSize<<setw(20)<<((order==0)?1:0)
                                       << endl;

        xrMultiPara.m_originalbornslot+=1000.0/xrMultiPara.FPS/Parameters::Instance().BASIC.DSlotDuration_ms;
        xrMultiPara.next_arrive_time=(int)(temp+xrMultiPara.m_originalbornslot);
    }

}
void MixedTraffic_MS::ftp3(){
    MSID _msid(msid);
    if(Parameters::Instance().BASIC.IDLORUL==Parameters::DL||Parameters::Instance().BASIC.IDLORUL==Parameters::ULandDL){
        //模型产生业务
        if (_msid.GetMS().packettime == Clock::Instance().GetTimeSlot())
        {
            std::shared_ptr<Universal_Packet > pPacket = std::shared_ptr<Universal_Packet > (new Universal_Packet(Parameters::FTPPacket,  G_PacketID++, ftp3Para.PacketSize, time, 0, _msid, _msid.GetMS().GetMainServBTS()));
            _msid.GetMS().GetMainServBTS().GetBTS().ReceivePacketDL(_msid, pPacket);
            m_qTotalPacketQueue.push_back(pPacket);
            _msid.GetMS().SetTrafficState(true);
            double interval = random.xExponent(ftp3Para.lamda*4 / 1000 * Parameters::Instance().BASIC.DSlotDuration_ms);
            while (interval < 1) {
                std::shared_ptr<Universal_Packet > pPacket1 = std::shared_ptr<Universal_Packet > (new Universal_Packet(Parameters::FTPPacket,G_PacketID++, ftp3Para.PacketSize, time, 0, _msid, _msid.GetMS().GetMainServBTS()));
                _msid.GetMS().GetMainServBTS().GetBTS().ReceivePacketDL(_msid, pPacket1);
                m_qTotalPacketQueue.push_back(pPacket1);
                interval += random.xExponent(ftp3Para.lamda*4 / 1000 * Parameters::Instance().BASIC.DSlotDuration_ms);
            }
            _msid.GetMS().packettime += (int)interval;
        }
    }
    if(Parameters::Instance().BASIC.IDLORUL==Parameters::UL||Parameters::Instance().BASIC.IDLORUL==Parameters::ULandDL){
        if (_msid.GetMS().UL.packettime == Clock::Instance().GetTimeSlot()) {
            std::shared_ptr<Universal_Packet > pPacket = std::shared_ptr<Universal_Packet > (new Universal_Packet(Parameters::FTPPacket,G_PacketID++, ftp3Para.PacketSize, time, 1, _msid, _msid.GetMS().GetMainServBTS()));
            _msid.GetMS().m_clsMacMs.ArrivePacket(pPacket);
            m_qTotalPacketQueue.push_back(pPacket);
            _msid.GetMS().SetTrafficState(true);
            double interval = random.xExponent(ftp3Para.lamda / 1000 * Parameters::Instance().BASIC.DSlotDuration_ms);
            while (interval < 1) {
                std::shared_ptr<Universal_Packet > pPacket1 = std::shared_ptr<Universal_Packet > (new Universal_Packet(Parameters::FTPPacket,G_PacketID++, ftp3Para.PacketSize, time, 1, _msid, _msid.GetMS().GetMainServBTS()));
                _msid.GetMS().m_clsMacMs.ArrivePacket(pPacket1);
                interval += random.xExponent(ftp3Para.lamda / 1000 * Parameters::Instance().BASIC.DSlotDuration_ms);
            }
            _msid.GetMS().UL.packettime += interval;
        }
    }
}
void MixedTraffic_MS::fullbuffer()
{
    MSID _msid(msid);
    if(Parameters::Instance().BASIC.IDLORUL==Parameters::DL||Parameters::Instance().BASIC.IDLORUL==Parameters::ULandDL)
    {
        //fullbuffer
        if (1 == time)
        {
            std::shared_ptr<Universal_Packet > pPacket = std::shared_ptr<Universal_Packet > (new Universal_Packet(Parameters::FTPPacket,  G_PacketID++, 100000000, time, 0, _msid, _msid.GetMS().GetMainServBTS()));
            _msid.GetMS().GetMainServBTS().GetBTS().ReceivePacketDL(_msid, pPacket);
            m_qTotalPacketQueue.push_back(pPacket);
            _msid.GetMS().SetTrafficState(true);
        }
    }
    if(Parameters::Instance().BASIC.IDLORUL==Parameters::UL||Parameters::Instance().BASIC.IDLORUL==Parameters::ULandDL){
        if (1 == Clock::Instance().GetTimeSlot()) //多drop时UE的buffer会清空，所以每个drop产生一次包
        {
            std::shared_ptr<Universal_Packet > pPacket = std::shared_ptr<Universal_Packet > (new Universal_Packet(Parameters::FTPPacket,G_PacketID++, 100000000, time, 1, _msid, _msid.GetMS().GetMainServBTS()));
            _msid.GetMS().m_clsMacMs.ArrivePacket(pPacket);
            m_qTotalPacketQueue.push_back(pPacket);
            _msid.GetMS().SetTrafficState(true);
        }
    }
}
void MixedTraffic_MS::DbTraffic()
{
    MSID _msid(msid);
    if(Parameters::Instance().BASIC.IDLORUL==Parameters::DL||Parameters::Instance().BASIC.IDLORUL==Parameters::ULandDL)
    {
//        数据库读取的业务
        double packet_size = -1;
        while(!DataBaseTrafficPara.m_qPacketToGenerateQueueDL.empty() && DataBaseTrafficPara.m_qPacketToGenerateQueueDL.front().true_time <= time_ms)
        {
            packet_size = DataBaseTrafficPara.m_qPacketToGenerateQueueDL.front().PacketSize*8/1000; //数据库里包大小是Byte, 这里转成Kbit
            DataBaseTrafficPara.m_qPacketToGenerateQueueDL.pop_front();
            std::shared_ptr<Universal_Packet > pPacket = std::shared_ptr<Universal_Packet > (new Universal_Packet(Parameters::FTPPacket,  G_PacketID++, packet_size, time, 0, _msid, _msid.GetMS().GetMainServBTS()));
            _msid.GetMS().GetMainServBTS().GetBTS().ReceivePacketDL(_msid, pPacket);
            m_qTotalPacketQueue.push_back(pPacket);
            _msid.GetMS().SetTrafficState(true);
        }
    }
    if(Parameters::Instance().BASIC.IDLORUL==Parameters::UL||Parameters::Instance().BASIC.IDLORUL==Parameters::ULandDL)
    {
        //上行目前数据库没有，先用fullbuffer代替
        if (1 == Clock::Instance().GetTimeSlot()) //多drop时UE的buffer会清空，所以每个drop产生一次包
        {
            std::shared_ptr<Universal_Packet > pPacket = std::shared_ptr<Universal_Packet > (new Universal_Packet(Parameters::FTPPacket,G_PacketID++, 100000000, time, 1, _msid, _msid.GetMS().GetMainServBTS()));
            _msid.GetMS().m_clsMacMs.ArrivePacket(pPacket);
            m_qTotalPacketQueue.push_back(pPacket);
            _msid.GetMS().SetTrafficState(true);
        }
        //数据库业务的代码
//        double packet_size = -1;
//        while(!DataBaseTrafficPara.m_qPacketToGenerateQueueUL.empty() && DataBaseTrafficPara.m_qPacketToGenerateQueueUL.front().true_time <= time_ms)
//        {
//            packet_size = DataBaseTrafficPara.m_qPacketToGenerateQueueUL.front().PacketSize*8/1000; //数据库里包大小是Byte, 这里转成Kbit
//            DataBaseTrafficPara.m_qPacketToGenerateQueueUL.pop_front();
//            std::shared_ptr<Universal_Packet > pPacket = std::shared_ptr<Universal_Packet > (new Universal_Packet(Parameters::FTPPacket,  G_PacketID++, packet_size, time, 0, _msid, _msid.GetMS().GetMainServBTS()));
//            _msid.GetMS().m_clsMacMs.ArrivePacket(pPacket);
//            m_qTotalPacketQueue.push_back(pPacket);
//            _msid.GetMS().SetTrafficState(true);
//        }
    }
}

void MixedTraffic_MS::OutputTrafficInfo()
{
    std::lock_guard<std::mutex> l(TrafficRecord_lock);
    auto it = m_qTotalPacketQueue.begin();
    while (it != m_qTotalPacketQueue.end()) {
        std::shared_ptr<Universal_Packet> p=std::static_pointer_cast<Universal_Packet>(*it);
        int iID = p->GetID();
        MSID msid = p->GetMSID();
        BTSID btsid = p->GetBTSID();
        bool iIsFinished = p->IsFullySuccessRecieved();
        bool iIsCompleted = p->IsCompleted();
        int iBornTimeSlot = p->GetBornTimeSlot();
        int iFinishTimeSlot = p->GetFinishTimeSlot();
        int iDelaySlot = p->GetDelayTimeSlot();
        double dOriginSizeKbits = p->GetOriginSizeKbits();
        double dRemainderSizeKbits = p->GetRemainUntransmitted_SizeKbits();
        double dTxSizeKbits = dOriginSizeKbits - dRemainderSizeKbits;
        double dRecievedSizeKbits = p->GetSuccessRecieved_SizeKbits();
        //hyl 冗余
//        Observer::Print("TrafficRecord") << setw(20) << iID
//                                         << setw(20) << msid
//                                         << setw(20) << btsid
//                                         << setw(20) << dOriginSizeKbits
//                                         << setw(20) << dRemainderSizeKbits
//                                         << setw(20) << dTxSizeKbits
//                                         << setw(20) << dRecievedSizeKbits
//                                         << setw(20) << (iIsFinished ? 1 : 0)
//                                         << setw(20) << (iIsCompleted ? 1 : 0)
//                                         << setw(20) << iBornTimeSlot
//                                         << setw(20) << iFinishTimeSlot
//                << setw(20) << iDelaySlot
//                << setw(20) << p->type
//                << setw(20) << p->qos
//                << setw(20) << (p->IsDLPacket()?"DL":"UL")
//                << endl;
        it++;
    }
}