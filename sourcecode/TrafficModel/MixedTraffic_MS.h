//
// Created by ChTY on 2022/10/7.
//

#ifndef CHTY_MIXEDTRAFFIC_MS_H
#define CHTY_MIXEDTRAFFIC_MS_H
//每个用户的业务生成
#include "../Utility/Random.h"
#include "../Statistician/Statistician.h"
#include "../NetworkDrive/NetWorkDrive.h"
extern int G_ICurDrop;
class Packet;
struct XRSinglePara
{
    int FPS;
    int SumRate;
    double jittermax;//ms
    double jittermin;//ms
    double jitterstd;//ms
    double s;//包大小标准差
    double max_mean_rate;//最大包大小/平均包大小
    double min_mean_rate;//最小包大小/平均包大小
    double dPeriod_ms;
    double dS; //13625;
    double dPacketSizeDefault_Kbit; //15000;
    double dPmax; //150000;
    double dPave; //104167 Bytes;
    double dPmin;
    int next_arrive_time;
    int m_iFrameNo;
    double m_originalbornslot;

    Random random;

    void setSumRate(int sumRate) {
        SumRate = sumRate;
    }


    void initialize(int _sumrate = 30, int _FPS = 60,
                    double _jittermax = 4, double _jittermin = -4, double _jitterstd=2,
                    double _max_mean_rate = 1.5, double _min_mean_rate = 0.5 ,double _s=0.105)
    {
        SumRate         = _sumrate;
        FPS             = _FPS;
        jittermax       = _jittermax;
        jittermin       = _jittermin;
        jitterstd       = _jitterstd;
        max_mean_rate   = _max_mean_rate;
        min_mean_rate   = _min_mean_rate;
        s               = _s;


        dPeriod_ms = 1000.0 / FPS;
        dPave = 1000.0 * SumRate / FPS;
        dPmax = dPave * max_mean_rate;
        dPmin = dPave * min_mean_rate;
        dS = dPave * s;
        m_iFrameNo = 0;

        m_originalbornslot=random.xUniform(0, 1000.0/FPS/ Parameters::Instance().BASIC.DSlotDuration_ms);
        next_arrive_time=ceil(m_originalbornslot);
    }

    XRSinglePara() = default;
};

struct XRMultiPara
{
    int FPS;
    int SumRate;
    double alpha;
    double jittermax;//ms
    double jittermin;//ms
    double jitterstd;//ms
    double s;//包大小标准差
    double I_max_mean_rate;//I帧最大包大小/平均包大小
    double I_min_mean_rate;//I帧最小包大小/平均包大小
    double P_max_mean_rate;//P帧最大包大小/平均包大小
    double P_min_mean_rate;//P帧最小包大小/平均包大小
    double dPeriod_ms;
    double dS; //13625;
    double dPacketSizeDefault_Kbit; //15000;
    double dPmax; //150000;
    double dPave; //104167 Bytes;
    double dPmin;
    double dPS;
    double dImax; //150000;
    double dIave; //104167 Bytes;
    double dImin;
    double dIS;
    int next_arrive_time;
    int m_iFrameNo;
    double m_originalbornslot;
    int K;//GOP:K
    int order;//标记当前I帧P帧循环的位置

    Random random;

    int get_and_renew_order(){
        if(order<K)
            return order++;
        else{
            order=0;
            return K;
        }
    };
    void initialize(int _sumrate = 30, int _FPS = 60,int _k=8,double _alpha=1.5,
                    double _jittermax = 4, double _jittermin = -4, double _jitterstd=2,
                    double _I_max_mean_rate = 1.5, double _I_min_mean_rate = 0.5 ,double _P_max_mean_rate = 1.5, double _P_min_mean_rate = 0.5 ,double _s=0.105)
    {
        SumRate     = _sumrate;
        FPS         = _FPS;
        K           = _k;
        alpha       = _alpha;
        jittermax   = _jittermax;
        jittermin   = _jittermin;
        jitterstd   = _jitterstd;
        I_max_mean_rate = _I_max_mean_rate;
        I_min_mean_rate = _I_min_mean_rate;
        P_max_mean_rate = _P_max_mean_rate;
        P_min_mean_rate = _P_min_mean_rate;

        dPeriod_ms = 1000.0 / FPS;
        dPave = 1000.0*SumRate*(K-1)/(K-1+alpha) *K/((K-1)*FPS);
        dPmax = dPave * P_max_mean_rate;
        dPmin = dPave * P_min_mean_rate;
        dPS = dPave * s;
        dIave = 1000.0*SumRate*alpha/(K-1+alpha) *K/FPS;
        dImax = dIave * P_max_mean_rate;
        dImin = dIave * P_min_mean_rate;
        dIS = dIave * s;
        order=0;
        m_iFrameNo=0;

        m_originalbornslot=random.xUniform(0, 1000.0/FPS/ Parameters::Instance().BASIC.DSlotDuration_ms);
        next_arrive_time=ceil(m_originalbornslot);
    }
    XRMultiPara() = default;
};
struct FTP3Para{
    double PacketSize;
    double lamda;
    int PacketArriveTimeDL; //下次数据包到达时间
    int PacketArriveTimeUL; //下次数据包到达时间
    Random random;

    void initialize(double _PacketSize=500,double _lamda=5)
    {
        PacketSize = _PacketSize;
        lamda = _lamda;
        double interval = random.xExponent(lamda / 1000 / Parameters::Instance().BASIC.DSlotDuration_ms);
        while (interval < 1)
        {
            interval += random.xExponent(lamda / 1000 / Parameters::Instance().BASIC.DSlotDuration_ms);
        }
        PacketArriveTimeDL = interval;
        PacketArriveTimeUL = interval;
    }
    FTP3Para() = default;
};


struct PacketInfo{
    double PacketSize;
    double true_time;
};
struct DataBaseTraffic
{
    deque<PacketInfo> m_qPacketToGenerateQueueDL;
    deque<PacketInfo> m_qPacketToGenerateQueueUL;

    void initialize(int msid)
    {
        double time_begin = Parameters::Instance().BASIC.DSlotDuration_ms * (G_ICurDrop-1) * Parameters::Instance().BASIC.ISlotPerDrop;
        double time_end = Parameters::Instance().BASIC.DSlotDuration_ms * G_ICurDrop * Parameters::Instance().BASIC.ISlotPerDrop;
        //UL暂时没有数据
    }
    DataBaseTraffic() = default;
};

class MixedTraffic_MS
{
public:
    Random random;
    int msid;
    int time;
    double time_ms;

    deque<std::shared_ptr<Packet> > m_qTotalPacketQueue;

    virtual void OutputTrafficInfo();
    explicit MixedTraffic_MS(int);
    void WorkSlot();
    void initialize(int msid, bool _xrSingleOn = false, bool _xrMultiOn = false, bool _ftp3On = false, bool _fullbufferOn = true, bool _DatabaseOn = false);

    bool xrSingleOn, xrMultiOn, ftp3On, DatabaseOn, fullbufferOn;  //业务模型开关

    XRSinglePara xrSinglePara;
    XRMultiPara xrMultiPara;
    FTP3Para ftp3Para;
    DataBaseTraffic DataBaseTrafficPara;
/**********************************************************************
* 函数名称：xrsingle
* 功能描述：产生XRsingle业务包并放入用户的缓存队列
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 全局变量：无
* 修改记录：
* 其他说明：
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    void xrsingle();
    void xrmulti();
    void ftp3();
    void fullbuffer();
/**********************************************************************
* 函数名称：DbTraffic
* 功能描述：按照数据库中的业务产生业务包并放入用户的缓存队列
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 全局变量：无
* 修改记录：
* 其他说明：
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    void DbTraffic();

    MixedTraffic_MS() = default;
//    MixedTraffic_MS(int _msid);

};


#endif //CHTY_MIXEDTRAFFIC_MS_H
